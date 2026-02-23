/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <system_error>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/IO/FileSystem/FileInputStream.h"
#include "Stroika/Foundation/IO/FileSystem/FileOutputStream.h"
#include "Stroika/Foundation/IO/FileSystem/ThroughTmpFileWriter.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Time/Duration.h"

#include "LocalDocumentDB.h"

using namespace Stroika::Foundation;

using namespace Characters;
using namespace Containers;
using namespace Debug;
using namespace DataExchange;
using namespace Database;
using namespace Database::Document::LocalDocumentDB;
using namespace Execution;
using namespace Time;

using Database::Document::EngineProperties;
using Database::Document::Filter;
using Database::Document::IDType;
using Database::Document::Projection;

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

using Common::GUID;
using Database::Document::EngineProperties;

namespace {

    template <InternallySynchronized SYNC_STYLE>
    using MyMaybeLock_ =
        conditional_t<SYNC_STYLE == InternallySynchronized::eNotKnownInternallySynchronized, Debug::AssertExternallySynchronizedMutex, recursive_mutex>;
    static_assert (Common::BasicLockable<MyMaybeLock_<InternallySynchronized::eNotKnownInternallySynchronized>>);
    static_assert (Common::BasicLockable<MyMaybeLock_<InternallySynchronized::eInternallySynchronized>>);

    /*
     *  Store collections entirely in RAM.
     *     \note   \em Thread-Safety  depends on InternallySynchronized
     */
    template <InternallySynchronized SYNC_STYLE>
    struct MemoryDatabaseRep_ final : Database::Document::LocalDocumentDB::IRep {

        using CollectionRep_ = Mapping<GUID, Document::Document>;

        const Document::LocalDocumentDB::Options fOptions_;
        [[no_unique_address]] mutable MyMaybeLock_<SYNC_STYLE> fMaybeLock_; // mutable cuz this is what we lock to assure internal sync for const/non-const methods
        Mapping<String, CollectionRep_> fCollections_;

        struct MyCollectionRep_ final : Document::Collection::IRep {
            const shared_ptr<MemoryDatabaseRep_> fConnectionRep_; // save to bump reference count (so lifetime of collection always >= lifetime of documentDB)
            const String fTableName_;

            MyCollectionRep_ (const shared_ptr<MemoryDatabaseRep_>& connectionRep, const String& collectionName)
                : fConnectionRep_{connectionRep}
                , fTableName_{collectionName}
            {
            }
            virtual String GetName () const override
            {
                scoped_lock critSec{fConnectionRep_->fMaybeLock_};
                return fTableName_;
            }
            virtual IDType Add (const Document::Document& v) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"LocalDocumentDB::MemoryDatabaseRep_::MyCollectionRep_::Add"};
#endif
                scoped_lock critSec{fConnectionRep_->fMaybeLock_};
                return fConnectionRep_->WrapExecute_ (
                    [&] () {
                        optional<VariantValue> vID = v.Lookup (Document::kID);
                        Require (not vID.has_value () or fConnectionRep_->fOptions_.fAddAllowsExternallySpecifiedIDs);
                        GUID               id         = vID.has_value () ? GUID{vID->As<String> ()} : GUID::GenerateNew ();
                        CollectionRep_     collection = fConnectionRep_->fCollections_.LookupValue (fTableName_);
                        Document::Document doc2Add    = v;
                        if (vID) {
                            doc2Add.Remove (Document::kID); // already in parent KEY so don't store redundantly
                        }
                        collection.Add (id, doc2Add);
                        fConnectionRep_->fCollections_.Add (fTableName_, collection);
                        return id.ToString ();
                    },
                    fTableName_, true);
            }
            virtual optional<Document::Document> Get (const IDType& id, const optional<Projection>& projection) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"LocalDocumentDB::MemoryDatabaseRep_::MyCollectionRep_::Get"};
#endif
                scoped_lock critSec{fConnectionRep_->fMaybeLock_};
                return fConnectionRep_->WrapExecute_ (
                    [&] () {
                        optional<Document::Document> r = fConnectionRep_->fCollections_.LookupValue (fTableName_).Lookup (GUID{id});
                        if (r) {
                            r->Add (Document::kID, id);
                        }
                        if (projection and r) {
                            r = projection->Apply (*r);
                        }
                        return r;
                    },
                    fTableName_, false);
            }
            virtual Sequence<Document::Document> GetAll (const optional<Filter>& filter, const optional<Projection>& projection) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"LocalDocumentDB::MemoryDatabaseRep_::MyCollectionRep_::GetAll", "filter={}, projection={}"_f, filter, projection};
#endif
                scoped_lock critSec{fConnectionRep_->fMaybeLock_};
                return fConnectionRep_->WrapExecute_ (
                    [&] () {
                        return fConnectionRep_->fCollections_.LookupValue (fTableName_)
                            .template Map<Sequence<Document::Document>> (
                                [&] (const KeyValuePair<GUID, Document::Document>& kvp) -> optional<Document::Document> {
                                    Document::Document d = kvp.fValue;
                                    d.Add (Document::kID, kvp.fKey.ToString ());
                                    if (filter and not filter->Matches (d)) {
                                        return nullopt; // skip cuz didn't match filter
                                    }
                                    else {
                                        if (projection) {
                                            d = projection->Apply (d);
                                        }
                                        return d;
                                    }
                                });
                    },
                    fTableName_, false);
            }
            virtual void Update (const IDType& id, const Document::Document& newV, const optional<Set<String>>& onlyTheseFields) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"LocalDocumentDB::MemoryDatabaseRep_::MyCollectionRep_::Update"};
#endif
                scoped_lock critSec{fConnectionRep_->fMaybeLock_};
                fConnectionRep_->WrapExecute_ (
                    [&] () {
                        Document::Document uploadDoc = newV;
                        if (onlyTheseFields) {
                            uploadDoc.RetainAll (*onlyTheseFields);
                        }
                        static const auto  kExcept1_           = RuntimeErrorException{"no such table"sv};
                        static const auto  kNoSuchIDException_ = RuntimeErrorException{"no such id"sv};
                        CollectionRep_     collection          = fConnectionRep_->fCollections_.LookupChecked (fTableName_, kExcept1_);
                        Document::Document d2Update = onlyTheseFields ? collection.LookupChecked (id, kNoSuchIDException_) : uploadDoc;
                        // any fields listed in onlyTheseFields, but not present in newV need to be removed
                        if (onlyTheseFields) {
                            d2Update.AddAll (uploadDoc);
                            Set<String> removeMe = *onlyTheseFields - newV.Keys ();
                            d2Update.RemoveAll (removeMe);
                        }
                        d2Update.RemoveIf (Document::kID);
                        collection.Add (id, d2Update);
                        fConnectionRep_->fCollections_.Add (fTableName_, collection); // replace the actual collection in our master database of collections
                    },
                    fTableName_, true);
            }
            virtual void Remove (const IDType& id) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"LocalDocumentDB::MemoryDatabaseRep_::MyCollectionRep_::Remove"};
#endif
                scoped_lock critSec{fConnectionRep_->fMaybeLock_};
                fConnectionRep_->WrapExecute_ (
                    [&] () {
                        if (optional<CollectionRep_> oc = fConnectionRep_->fCollections_.Lookup (fTableName_)) {
                            CollectionRep_ c = *oc;
                            if (c.RemoveIf (id)) {
                                fConnectionRep_->fCollections_.Add (fTableName_, c); // replace the actual collection in our master database of collections
                            }
                        }
                    },
                    fTableName_, true);
            }
        };

        struct MyTransactionRep_ final : Database::Document::Transaction::IRep {
            virtual void Commit () override
            {
                // nothing todo
            }
            virtual void Rollback () override
            {
                AssertNotImplemented ();
            }
            virtual Disposition GetDisposition () const override
            {
                return Disposition::eCompleted;
            }
        };

        MemoryDatabaseRep_ ()                          = delete;
        MemoryDatabaseRep_ (const MemoryDatabaseRep_&) = delete;
        MemoryDatabaseRep_ ([[maybe_unused]] const Document::LocalDocumentDB::Options& options)
            : fOptions_{options}
        {
            TraceContextBumper ctx{"LocalDocumentDB::MemoryDatabaseRep_::MemoryDatabaseRep_"};
            //Assert (shared_from_this ().get () == this); // only support allocating with make_shared - cannot check here cuz object not yet fully constructed
        }
        virtual shared_ptr<const EngineProperties> GetEngineProperties () const override
        {
            struct MyEngineProperties_ final : EngineProperties {
                virtual String GetEngineName () const override
                {
                    return "LocalDocumentDB.MemoryDB"sv;
                }
            };
            static const shared_ptr<const EngineProperties> kProps_ = Memory::MakeSharedPtr<const MyEngineProperties_> ();
            return kProps_;
        }
        virtual Database::Document::Connection::Options GetOptions () const override
        {
            return fOptions_;
        }
        static uintmax_t EstimateSize_ (const VariantValue& vv)
        {
            switch (vv.GetType ()) {
                case VariantValue::Type::eBLOB:
                    return vv.As<Memory::BLOB> ().size ();
                case VariantValue::Type::eString:
                    return vv.As<String> ().size ();
                case VariantValue::Type::eFloat:
                    return sizeof (long double);
                default:
                    AssertNotImplemented ();
                    return 1000; //tmphack
            }
            return 0;
        }
        virtual uintmax_t GetSpaceConsumed () const override
        {
            uintmax_t totalSize{};
            // WAG/Weak but adequate Estimate
            for (const KeyValuePair<String, CollectionRep_>& ci : fCollections_) {
                totalSize += ci.fKey.size () + 3;
                for (const KeyValuePair<GUID, Document::Document>& di : ci.fValue) {
                    totalSize += 20; // for GUID
                    for (const KeyValuePair<String, VariantValue>& xi : di.fValue) {
                        totalSize += xi.fKey.size () + 3;
                        totalSize += EstimateSize_ (xi.fValue) + 4;
                    }
                }
            }
            return totalSize;
        }
        virtual Set<String> GetCollections () override
        {
            scoped_lock declareContext{fMaybeLock_};
            return Set<String>{fCollections_.Keys ()};
        }
        virtual Document::Collection::Ptr CreateCollection (const String& name) override
        {
            scoped_lock declareContext{fMaybeLock_};
            if (not fCollections_.Lookup (name)) {
                fCollections_.Add (name, {});
            }
            return GetCollection (name);
        }
        virtual void DropCollection (const String& name) override
        {
            scoped_lock declareContext{fMaybeLock_};
            fCollections_.RemoveIf (name);
        }
        virtual Document::Collection::Ptr GetCollection (const String& name) override
        {
            scoped_lock declareContext{fMaybeLock_};
            Require (fCollections_.ContainsKey (name));
            return Document::Collection::Ptr{
                Memory::MakeSharedPtr<MyCollectionRep_> (Debug::UncheckedDynamicPointerCast<MemoryDatabaseRep_> (shared_from_this ()), name)};
        }
        virtual Document::Transaction mkTransaction () override
        {
            return Document::Transaction{make_unique<MyTransactionRep_> ()};
        }
        virtual void Flush () override
        {
            // nothing todo - all in memory
        }
        template <typename FUN>
        inline auto WrapExecute_ (FUN&& f, const optional<String>& collectionName, bool write) -> invoke_result_t<FUN>
        {
            return Document::Connection::Private_::WrapLoggingExecuteHelper_ (forward<FUN> (f), this, fOptions_, collectionName, write);
        }
    };

    /*
     *  Store collections in json file (leveraging MemoryDatabaseRep_ internally).
     *     \note   \em Thread-Safety   <a href='#Internally-Synchronized-Thread-Safety'>Internally-Synchronized-Thread-Safety</a>
     */
    template <InternallySynchronized SYNC_STYLE>
    struct SingleFileDatabaseRep_ final : Database::Document::LocalDocumentDB::IRep {

        const filesystem::path fExternalFile_;
        shared_ptr<MemoryDatabaseRep_<SYNC_STYLE>> fMemoryDB_; // already internally synrchonized, must be shared_ptr cuz it uses shared_from_this
        const DataExchange::Variant::Reader fReader_;
        const DataExchange::Variant::Writer fWriter_;
        const bool                          fFlushOnEachWrite_;
        bool                                fDirty_{true}; // if true, we have changes that haven't yet been flushed to disk
        const OpertionCallbackPtr           fOperationLoggingCallback_{nullptr};
#if qStroika_Foundation_Common_Platform_Windows
        const optional<Time::DurationSeconds> fRetryOnSharingViolationFor_;
#endif

        struct MyCollectionRep_ final : Document::Collection::IRep {
            const shared_ptr<SingleFileDatabaseRep_>         fDBRep_; // save to bump reference count (lifetime safety), and to force write
            const String                                     fName_;
            shared_ptr<Database::Document::Collection::IRep> fDelegateToInMemoryDB_; // inside memorydb

            MyCollectionRep_ (const shared_ptr<SingleFileDatabaseRep_>& dbRep, const String& name,
                              const shared_ptr<Database::Document::Collection::IRep>& delgateImplTo)
                : fDBRep_{dbRep}
                , fName_{name}
                , fDelegateToInMemoryDB_{delgateImplTo}
            {
            }
            virtual String GetName () const override
            {
                return fName_;
            }
            virtual IDType Add (const Document::Document& v) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"LocalDocumentDB::SingleFileDatabaseRep_::MyCollectionRep_::Add"};
#endif
                scoped_lock critSec{fDBRep_->fMemoryDB_->fMaybeLock_};
                return fDBRep_->WrapExecute_ (
                    [&] () {
                        auto id = fDelegateToInMemoryDB_->Add (v);
                        fDBRep_->DataChangedSoMaybeWrite2Disk ();
                        return id;
                    },
                    fName_, true);
            }
            virtual optional<Document::Document> Get (const IDType& id, const optional<Projection>& projection) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"LocalDocumentDB::SingleFileDatabaseRep_::MyCollectionRep_::Get"};
#endif
                scoped_lock critSec{fDBRep_->fMemoryDB_->fMaybeLock_};
                return fDBRep_->WrapExecute_ ([&] () { return fDelegateToInMemoryDB_->Get (id, projection); }, fName_, false);
            }
            virtual Sequence<Document::Document> GetAll (const optional<Filter>& filter, const optional<Projection>& projection) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"LocalDocumentDB::SingleFileDatabaseRep_::MyCollectionRep_::GetAll", "filter={}, projection={}"_f,
                                       filter, projection};
#endif
                return fDBRep_->WrapExecute_ ([&] () { return fDelegateToInMemoryDB_->GetAll (filter, projection); }, fName_, false);
            }
            virtual void Update (const IDType& id, const Document::Document& newV, const optional<Set<String>>& onlyTheseFields) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"LocalDocumentDB::SingleFileDatabaseRep_::MyCollectionRep_::Update"};
#endif
                scoped_lock critSec{fDBRep_->fMemoryDB_->fMaybeLock_};
                fDBRep_->WrapExecute_ (
                    [&] () {
                        fDelegateToInMemoryDB_->Update (id, newV, onlyTheseFields);
                        fDBRep_->DataChangedSoMaybeWrite2Disk ();
                    },
                    fName_, true);
            }
            virtual void Remove (const IDType& id) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"LocalDocumentDB::SingleFileDatabaseRep_::MyCollectionRep_::Remove"};
#endif
                scoped_lock critSec{fDBRep_->fMemoryDB_->fMaybeLock_};
                fDBRep_->WrapExecute_ (
                    [&] () {
                        fDelegateToInMemoryDB_->Remove (id);
                        fDBRep_->DataChangedSoMaybeWrite2Disk ();
                    },
                    fName_, true);
            }
        };

        struct MyTransactionRep_ final : Database::Document::Transaction::IRep {
            virtual void Commit () override
            {
                // nothing todo
            }
            virtual void Rollback () override
            {
                AssertNotImplemented ();
            }
            virtual Disposition GetDisposition () const override
            {
                return Disposition::eCompleted;
            }
        };

        static Document::LocalDocumentDB::Options stripOptionsForMemDB_ (Document::LocalDocumentDB::Options o)
        {
            o.fOperationLoggingCallback = nullptr;
            return o;
        }

        SingleFileDatabaseRep_ ()                              = delete;
        SingleFileDatabaseRep_ (const SingleFileDatabaseRep_&) = delete;
        SingleFileDatabaseRep_ ([[maybe_unused]] const Document::LocalDocumentDB::Options&   options,
                                const Document::LocalDocumentDB::Options::SingleFileStorage& sfOptions)
            : fExternalFile_{sfOptions.fFile}
            , fMemoryDB_{make_shared<MemoryDatabaseRep_<SYNC_STYLE>> (stripOptionsForMemDB_ (options))}
            , fReader_{get<DataExchange::Variant::Reader> (sfOptions.fSerialization)}
            , fWriter_{get<DataExchange::Variant::Writer> (sfOptions.fSerialization)}
            , fFlushOnEachWrite_{sfOptions.fFlushOnEachWrite}
            , fDirty_{not fFlushOnEachWrite_}
            , fOperationLoggingCallback_{options.fOperationLoggingCallback}
#if qStroika_Foundation_Common_Platform_Windows
            , fRetryOnSharingViolationFor_{sfOptions.fRetryOnSharingViolationFor}
#endif
        {
            if (not sfOptions.fForceCreateNew) {
                DoReadFromFS ();
            }
        }
        virtual ~SingleFileDatabaseRep_ () override
        {
            if (fDirty_) {
                DoWriteToFS ();
            }
        }
        virtual shared_ptr<const EngineProperties> GetEngineProperties () const override
        {
            struct MyEngineProperties_ final : EngineProperties {
                virtual String GetEngineName () const override
                {
                    return "LocalDocumentDB.SingleFile"sv;
                }
            };
            static const shared_ptr<const EngineProperties> kProps_ = Memory::MakeSharedPtr<const MyEngineProperties_> ();
            return kProps_;
        }
        virtual Database::Document::Connection::Options GetOptions () const override
        {
            scoped_lock                             declareContext{fMemoryDB_->fMaybeLock_};
            Database::Document::Connection::Options o = fMemoryDB_->GetOptions ();
            o.fOperationLoggingCallback               = fOperationLoggingCallback_;
            return o;
        }
        virtual uintmax_t GetSpaceConsumed () const override
        {
            if (fDirty_) {
                IgnoreExceptionsExceptThreadAbortForCall (const_cast<SingleFileDatabaseRep_*> (this)->DoWriteToFS ()); // cannot get size otherwise
            }
            error_code ignoredEC;
            return filesystem::file_size (fExternalFile_, ignoredEC);
        }
        virtual Set<String> GetCollections () override
        {
            return fMemoryDB_->GetCollections ();
        }
        virtual Document::Collection::Ptr CreateCollection (const String& name) override
        {
            scoped_lock declareContext{fMemoryDB_->fMaybeLock_};
            return WrapExecute_ (
                [&] () {
                    fMemoryDB_->CreateCollection (name);
                    DataChangedSoMaybeWrite2Disk ();
                    return GetCollection (name);
                },
                name, true);
        }
        virtual void DropCollection (const String& name) override
        {
            scoped_lock declareContext{fMemoryDB_->fMaybeLock_};
            WrapExecute_ (
                [&] () {
                    fMemoryDB_->DropCollection (name);
                    DataChangedSoMaybeWrite2Disk ();
                },
                name, true);
        }
        virtual Document::Collection::Ptr GetCollection (const String& name) override
        {
            scoped_lock               declareContext{fMemoryDB_->fMaybeLock_};
            Document::Collection::Ptr memDBCollection = fMemoryDB_->GetCollection (name);
            return Document::Collection::Ptr{Memory::MakeSharedPtr<MyCollectionRep_> (
                Debug::UncheckedDynamicPointerCast<SingleFileDatabaseRep_> (shared_from_this ()), name, memDBCollection)};
        }
        virtual Document::Transaction mkTransaction () override
        {
            return Document::Transaction{make_unique<MyTransactionRep_> ()};
        }
        virtual void Flush () override
        {
            DoWriteToFS ();
        }
        void DoReadFromFS ()
        {
            using namespace IO::FileSystem;
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            TraceContextBumper ctx{"LocalDocumentDB::SingleFileDatabaseRep_::DoReadFromFS", "path={}"_f, fExternalFile_};
#endif
            scoped_lock declareContext{fMemoryDB_->fMaybeLock_};
            if (filesystem::exists (fExternalFile_)) {
                fMemoryDB_->fCollections_.clear ();
                for (KeyValuePair<String, VariantValue> collectionAndDocument :
                     fReader_.Read (FileInputStream::New (fExternalFile_)).template As<Mapping<String, VariantValue>> ()) {
                    fMemoryDB_->fCollections_.Add (
                        collectionAndDocument.fKey,
                        collectionAndDocument.fValue.As<Mapping<String, VariantValue>> ().template Map<Mapping<GUID, Document::Document>> (
                            [&] (const KeyValuePair<String, VariantValue>& kvp) -> KeyValuePair<GUID, Document::Document> {
                                return {GUID{kvp.fKey}, kvp.fValue.As<Document::Document> ()};
                            }));
                }
            }
        }
        void DataChangedSoMaybeWrite2Disk ()
        {
            if (fFlushOnEachWrite_) {
                DoWriteToFS ();
            }
            else {
                fDirty_ = true;
            }
        }
        void DoWriteToFS ()
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            TraceContextBumper ctx{"LocalDocumentDB::SingleFileDatabaseRep_::DoWriteToFS", "path={}"_f, fExternalFile_};
#endif
            scoped_lock declareContext{fMemoryDB_->fMaybeLock_};
            using namespace IO::FileSystem;
            ThroughTmpFileWriter                  tmpFile{fExternalFile_};
            IO::FileSystem::FileOutputStream::Ptr outStream = IO::FileSystem::FileOutputStream::New (tmpFile.GetFilePath ());
            Mapping<String, VariantValue>         collectionsAsVV;
            for (const KeyValuePair<String, Mapping<GUID, Document::Document>>& collection : fMemoryDB_->fCollections_) {
                Mapping<GUID, Document::Document> collectionValue = collection.fValue;
                Mapping<String, VariantValue>     collWithStringKey;
                for (const KeyValuePair<GUID, Document::Document>& kvp : collectionValue) {
                    collWithStringKey.Add (kvp.fKey.ToString (), VariantValue{kvp.fValue});
                }
                collectionsAsVV.Add (collection.fKey, VariantValue{collWithStringKey});
            }
            this->fWriter_.Write (VariantValue{collectionsAsVV}, outStream);
            outStream.Close (); // close like this so we can throw exception - cannot throw if we count on DTOR
#if qStroika_Foundation_Common_Platform_Windows
            tmpFile.fRetryOnSharingViolationFor = fRetryOnSharingViolationFor_;
#endif
            tmpFile.Commit (); // any exceptions cause the tmp file to be automatically cleaned up
            fDirty_ = false;
        }
        template <typename FUN>
        inline auto WrapExecute_ (FUN&& f, const optional<String>& collectionName, bool write) -> invoke_result_t<FUN>
        {
            if (fOperationLoggingCallback_) {
                Database::Document::Connection::Options o = fMemoryDB_->GetOptions ();
                o.fOperationLoggingCallback               = fOperationLoggingCallback_;
                return Document::Connection::Private_::WrapLoggingExecuteHelper_ (forward<FUN> (f), this, o, collectionName, write);
            }
            else {
                return f ();
            }
        }
    };

    // Store each collection in a folder under the root folder
    // Mostly intrinsically internally synchronized, but maybe could use locks here. Corner cases?
    // like update, while adds happening. Won't cause CORRUPTION, but unclear what guarantees we want
    // to offer about what completes before what?
    template <InternallySynchronized SYNC_STYLE>
    struct DirectoryFilesystemDatabaseRep_ final : Database::Document::LocalDocumentDB::IRep {
        const Document::LocalDocumentDB::Options fOptions_;
        const filesystem::path                   fRoot_;
        const DataExchange::Variant::Reader      fReader_;
        const DataExchange::Variant::Writer      fWriter_;
#if qStroika_Foundation_Common_Platform_Windows
        const optional<Time::DurationSeconds> fRetryOnSharingViolationFor_;
#endif

        struct MyCollectionRep_ final : Document::Collection::IRep {
            const shared_ptr<DirectoryFilesystemDatabaseRep_> fDBRep_; // save to bump reference count (lifetime safety)
            const String                                      fName_;
            const filesystem::path                            fCollectionRoot_;

            MyCollectionRep_ (const shared_ptr<DirectoryFilesystemDatabaseRep_>& dbRep, const String& name)
                : fDBRep_{dbRep}
                , fName_{name}
                , fCollectionRoot_{dbRep->GetCollectionFilePath_ (name)}
            {
            }
            virtual String GetName () const override
            {
                return fName_;
            }
            virtual IDType Add (const Document::Document& v) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"LocalDocumentDB::DirectoryFilesystemDatabaseRep_::MyCollectionRep_::Add"};
#endif
                optional<VariantValue> vID = v.Lookup (Document::kID);
                Require (not vID.has_value () or fDBRep_->fOptions_.fAddAllowsExternallySpecifiedIDs);
                GUID id = vID.has_value () ? GUID{vID->As<String> ()} : GUID::GenerateNew ();
                fDBRep_->WrapExecute_ (
                    [&] () {
                        Document::Document doc2Add = v;
                        if (vID) {
                            doc2Add.Remove (Document::kID); // already in parent KEY so don't store redundantly
                        }
                        DoWriteToFS_ (id, VariantValue{doc2Add});
                    },
                    fName_, true);
                return id.As<IDType> ();
            }
            virtual optional<Document::Document> Get (const IDType& id, const optional<Projection>& projection) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"LocalDocumentDB::DirectoryFilesystemDatabaseRep_::MyCollectionRep_::Get"};
#endif
                return fDBRep_->WrapExecute_ (
                    [&] () -> optional<Document::Document> {
                        if (auto od = DoReadFromFS_ (GUID{id})) {
                            Document::Document d = *od;
                            d.Add (Document::kID, id);
                            if (projection) {
                                d = projection->Apply (d);
                            }
                            return d;
                        }
                        return nullopt;
                    },
                    fName_, false);
            }
            virtual Sequence<Document::Document> GetAll (const optional<Filter>& filter, const optional<Projection>& projection) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"LocalDocumentDB::DirectoryFilesystemDatabaseRep_::MyCollectionRep_::GetAll",
                                       "filter={}, projection={}"_f, filter, projection};
#endif
                return fDBRep_->WrapExecute_ (
                    [&] () {
                        Sequence<Document::Document> result;
                        for (const auto& entry : filesystem::directory_iterator{fCollectionRoot_}) {
                            if (entry.path ().extension () == ".json"sv) { // Check if the entry is a JSON file
                                Document::Document d =
                                    fDBRep_->fReader_.Read (IO::FileSystem::FileInputStream::New (entry.path ())).template As<Document::Document> ();
                                d.Add (Document::kID, entry.path ().stem ().string ());
                                if (not filter or filter->Matches (d)) {
                                    if (projection) {
                                        d = projection->Apply (d);
                                    }
                                    result += d;
                                }
                            }
                        }
                        return result;
                    },
                    fName_, false);
            }
            virtual void Update (const IDType& id, const Document::Document& newV, const optional<Set<String>>& onlyTheseFields) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"LocalDocumentDB::DirectoryFilesystemDatabaseRep_::MyCollectionRep_::Update",
                                       "id={},newV={}, onlyTheseFields={}"_f, id, newV, onlyTheseFields};
#endif
                fDBRep_->WrapExecute_ (
                    [&] () {
                        Document::Document updatedDoc =
                            onlyTheseFields ? Memory::ValueOfOrThrow (DoReadFromFS_ (id), RuntimeErrorException{"no such id"sv}) : newV;
                        Document::Document updateWithDoc = newV;
                        if (onlyTheseFields) {
                            updateWithDoc.RetainAll (*onlyTheseFields);
                        }
                        updatedDoc.AddAll (updateWithDoc);
                        if (onlyTheseFields) {
                            // any fields listed in onlyTheseFields, but not present in newV need to be removed
                            Set<String> removeMe = *onlyTheseFields - newV.Keys ();
                            updatedDoc.RemoveAll (removeMe);
                        }
                        DoWriteToFS_ (GUID{id}, VariantValue{updatedDoc});
                    },
                    fName_, true);
            }
            virtual void Remove (const IDType& id) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"LocalDocumentDB::DirectoryFilesystemDatabaseRep_::MyCollectionRep_::Remove", "id={}"_f, id};
#endif
                fDBRep_->WrapExecute_ ([&] () { (void)filesystem::remove (GetDocumentFilePath_ (GUID{id})); }, fName_, true);
            }
            filesystem::path GetDocumentFilePath_ (const GUID& id) const
            {
                return fCollectionRoot_ / (id.As<String> () + ".json"sv).As<filesystem::path> ();
            }
            optional<Document::Document> DoReadFromFS_ (const GUID& id)
            {
                using namespace IO::FileSystem;
                filesystem::path docFilePath = GetDocumentFilePath_ (id);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"LocalDocumentDB::DirectoryFilesystemDatabaseRep_::DoReadFromFS", "path={}"_f, docFilePath};
#endif
                if (filesystem::exists (docFilePath)) {
                    return fDBRep_->fReader_.Read (FileInputStream::New (docFilePath)).template As<Document::Document> ();
                }
                return nullopt;
            }
            void DoWriteToFS_ (const GUID& id, const VariantValue& vv)
            {
                filesystem::path docFilePath = GetDocumentFilePath_ (id);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"LocalDocumentDB::DirectoryFilesystemDatabaseRep_::DoWriteToFS", "path={}"_f, docFilePath};
#endif
                using namespace IO::FileSystem;
                ThroughTmpFileWriter                  tmpFile{docFilePath};
                IO::FileSystem::FileOutputStream::Ptr outStream = IO::FileSystem::FileOutputStream::New (tmpFile.GetFilePath ());
                fDBRep_->fWriter_.Write (vv, outStream);
                outStream.Close (); // close like this so we can throw exception - cannot throw if we count on DTOR
#if qStroika_Foundation_Common_Platform_Windows
                tmpFile.fRetryOnSharingViolationFor = fDBRep_->fRetryOnSharingViolationFor_;
#endif
                tmpFile.Commit (); // any exceptions cause the tmp file to be automatically cleaned up
            }
        };

        struct MyTransactionRep_ final : Database::Document::Transaction::IRep {
            virtual void Commit () override
            {
                // nothing todo
            }
            virtual void Rollback () override
            {
                AssertNotImplemented ();
            }
            virtual Disposition GetDisposition () const override
            {
                return Disposition::eCompleted;
            }
        };

        filesystem::path GetCollectionFilePath_ (const String& collectionName) const
        {
            // @todo - in future - consider mapping name to URL-safe name
            // @todo use PCTEncode2String
            return fRoot_ / (collectionName.As<filesystem::path> ());
        }

        DirectoryFilesystemDatabaseRep_ ()                                       = delete;
        DirectoryFilesystemDatabaseRep_ (const DirectoryFilesystemDatabaseRep_&) = delete;
        DirectoryFilesystemDatabaseRep_ (const Document::LocalDocumentDB::Options&                       options,
                                         const Document::LocalDocumentDB::Options::DirectoryFileStorage& dfOptions)
            : fOptions_{options}
            , fRoot_{dfOptions.fRoot}
            , fReader_{get<DataExchange::Variant::Reader> (dfOptions.fSerialization)}
            , fWriter_{get<DataExchange::Variant::Writer> (dfOptions.fSerialization)}
#if qStroika_Foundation_Common_Platform_Windows
            , fRetryOnSharingViolationFor_{dfOptions.fRetryOnSharingViolationFor}
#endif
        {
            filesystem::create_directories (fRoot_);
        }
        virtual shared_ptr<const EngineProperties> GetEngineProperties () const override
        {
            struct MyEngineProperties_ final : EngineProperties {
                virtual String GetEngineName () const override
                {
                    return "LocalDocumentDB.Folder"sv;
                }
            };
            static const shared_ptr<const EngineProperties> kProps_ = Memory::MakeSharedPtr<const MyEngineProperties_> ();
            return kProps_;
        }
        virtual Database::Document::Connection::Options GetOptions () const override
        {
            return fOptions_;
        }
        virtual uintmax_t GetSpaceConsumed () const override
        {
            uintmax_t totalSize{};
            try {
                for (const auto& entry : filesystem::recursive_directory_iterator (fRoot_, filesystem::directory_options::skip_permission_denied)) {
                    // Check if the current entry is a regular file before getting its size
                    if (filesystem::is_regular_file (entry.status ())) {
                        // file_size() throws an exception or returns (uintmax_t)-1 on error
                        std::uintmax_t file_size = entry.file_size ();
                        totalSize += file_size;
                    }
                }
            }
            catch (const filesystem::filesystem_error& e) {
                DbgTrace ("suppressing error in GetSpaceConsumed () = returning zero: {}"_f, e);
                return 0;
            }
            return totalSize;
        }
        virtual Set<String> GetCollections () override
        {
            Set<String> result;
            for (const auto& entry : filesystem::directory_iterator{fRoot_}) {
                if (filesystem::is_directory (entry.path ())) { // Check if the entry is a directory
                    result += String{entry.path ().filename ()};
                }
            }
            return result;
        }
        virtual Document::Collection::Ptr CreateCollection (const String& name) override
        {
            return WrapExecute_ (
                [&] () {
                    filesystem::create_directories (GetCollectionFilePath_ (name));
                    return Document::Collection::Ptr{Memory::MakeSharedPtr<MyCollectionRep_> (
                        Debug::UncheckedDynamicPointerCast<DirectoryFilesystemDatabaseRep_> (shared_from_this ()), name)};
                },
                name, true);
        }
        virtual void DropCollection (const String& name) override
        {
            WrapExecute_ ([&] () { filesystem::remove_all (GetCollectionFilePath_ (name)); }, name, true);
        }
        virtual Document::Collection::Ptr GetCollection (const String& name) override
        {
            Require (GetCollections ().Contains (name));
            return Document::Collection::Ptr{Memory::MakeSharedPtr<MyCollectionRep_> (
                Debug::UncheckedDynamicPointerCast<DirectoryFilesystemDatabaseRep_> (shared_from_this ()), name)};
        }
        virtual Document::Transaction mkTransaction () override
        {
            return Document::Transaction{make_unique<MyTransactionRep_> ()};
        }
        virtual void Flush () override
        {
            // nothing todo - already flushed to FS on each operation
        }
        template <typename FUN>
        inline auto WrapExecute_ (FUN&& f, const optional<String>& collectionName, bool write) -> invoke_result_t<FUN>
        {
            return Document::Connection::Private_::WrapLoggingExecuteHelper_ (forward<FUN> (f), this, fOptions_, collectionName, write);
        }
    };
}

/*
 ********************************************************************************
 *********************** SQL::LocalDocumentDB::New ******************************
 ********************************************************************************
 */
auto Document::LocalDocumentDB::New (const Options& options) -> Ptr
{
    switch (options.fInternallySynchronizedLetter) {
        case eInternallySynchronized:
            if (get_if<Options::MemoryStorage> (&options.fStorage)) {
                return Ptr{Memory::MakeSharedPtr<MemoryDatabaseRep_<eInternallySynchronized>> (options)};
            }
            else if (auto fop = get_if<Options::SingleFileStorage> (&options.fStorage)) {
                return Ptr{Memory::MakeSharedPtr<SingleFileDatabaseRep_<eInternallySynchronized>> (options, *fop)};
            }
            else if (auto dop = get_if<Options::DirectoryFileStorage> (&options.fStorage)) {
                return Ptr{Memory::MakeSharedPtr<DirectoryFilesystemDatabaseRep_<eInternallySynchronized>> (options, *dop)};
            }
            RequireNotReached ();
            return nullptr;
        case eNotKnownInternallySynchronized:
            if (get_if<Options::MemoryStorage> (&options.fStorage)) {
                return Ptr{Memory::MakeSharedPtr<MemoryDatabaseRep_<Execution::eNotKnownInternallySynchronized>> (options)};
            }
            else if (auto fop = get_if<Options::SingleFileStorage> (&options.fStorage)) {
                return Ptr{Memory::MakeSharedPtr<SingleFileDatabaseRep_<eNotKnownInternallySynchronized>> (options, *fop)};
            }
            else if (auto dop = get_if<Options::DirectoryFileStorage> (&options.fStorage)) {
                return Ptr{Memory::MakeSharedPtr<DirectoryFilesystemDatabaseRep_<eNotKnownInternallySynchronized>> (options, *dop)};
            }
            RequireNotReached ();
            return nullptr;
        default:
            RequireNotReached ();
            return nullptr;
    }

    AssertNotImplemented ();
    return nullptr;
}
