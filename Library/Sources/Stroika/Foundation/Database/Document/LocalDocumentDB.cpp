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

    /*
     *  Store collections entirely in RAM.
     *     \note   \em Thread-Safety   <a href='#Internally-Synchronized-Thread-Safety'>Internally-Synchronized-Thread-Safety</a>
     */
    struct MemoryDatabaseRep_ final : Database::Document::Connection::IRep {

        using CollectionRep_ = Mapping<GUID, Document::Document>;

        const Document::LocalDocumentDB::Options      fOptions_;
        Synchronized<Mapping<String, CollectionRep_>> fCollections_;

        struct MyCollectionRep_ final : Document::Collection::IRep {
            const shared_ptr<MemoryDatabaseRep_> fConnectionRep_; // save to bump reference count (so lifetime of collection always >= lifetime of documentDB)
            const String fTableName_;

            MyCollectionRep_ (const shared_ptr<MemoryDatabaseRep_>& connectionRep, const String& collectionName)
                : fConnectionRep_{connectionRep}
                , fTableName_{collectionName}
            {
            }
            virtual IDType Add (const Document::Document& v) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"LocalDocumentDB::MemoryDatabaseRep_::MyCollectionRep_::Add"};
#endif
                return fConnectionRep_->WrapExecute_ (
                    [&] () {
                        optional<VariantValue> vID = v.Lookup (Document::kID);
                        Require (not vID.has_value () or fConnectionRep_->fOptions_.fAddAllowsExternallySpecifiedIDs);
                        GUID               id         = vID.has_value () ? GUID{vID->As<String> ()} : GUID::GenerateNew ();
                        auto               rwLock     = fConnectionRep_->fCollections_.rwget ();
                        CollectionRep_     collection = rwLock.cref ().LookupValue (fTableName_);
                        Document::Document doc2Add    = v;
                        if (vID) {
                            doc2Add.Remove (Document::kID); // already in parent KEY so don't store redundantly
                        }
                        collection.Add (id, doc2Add);
                        rwLock.rwref ().Add (fTableName_, collection);
                        return id.ToString ();
                    },
                    fTableName_, true);
            }
            virtual optional<Document::Document> Get (const IDType& id, const optional<Projection>& projection) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"LocalDocumentDB::MemoryDatabaseRep_::MyCollectionRep_::Get"};
#endif
                return fConnectionRep_->WrapExecute_ (
                    [&] () {
                        optional<Document::Document> r = fConnectionRep_->fCollections_->LookupValue (fTableName_).Lookup (GUID{id});
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
                return fConnectionRep_->WrapExecute_ (
                    [&] () {
                        return fConnectionRep_->fCollections_->LookupValue (fTableName_)
                            .Map<Sequence<Document::Document>> ([&] (const KeyValuePair<GUID, Document::Document>& kvp) -> optional<Document::Document> {
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
                fConnectionRep_->WrapExecute_ (
                    [&] () {
                        Document::Document uploadDoc = newV;
                        if (onlyTheseFields) {
                            uploadDoc.RetainAll (*onlyTheseFields);
                        }
                        static const auto  kExcept1_           = RuntimeErrorException{"no such table"sv};
                        static const auto  kNoSuchIDException_ = RuntimeErrorException{"no such id"sv};
                        auto               rwLock              = fConnectionRep_->fCollections_.rwget ();
                        CollectionRep_     collection          = rwLock.cref ().LookupChecked (fTableName_, kExcept1_);
                        Document::Document d2Update = onlyTheseFields ? collection.LookupChecked (id, kNoSuchIDException_) : uploadDoc;
                        // any fields listed in onlyTheseFields, but not present in newV need to be removed
                        if (onlyTheseFields) {
                            d2Update.AddAll (uploadDoc);
                            Set<String> removeMe = *onlyTheseFields - newV.Keys ();
                            d2Update.RemoveAll (removeMe);
                        }
                        d2Update.RemoveIf (Document::kID);
                        collection.Add (id, d2Update);
                        rwLock.rwref ().Add (fTableName_, collection); // replace the actual collection in our master database of collections
                    },
                    fTableName_, true);
            }
            virtual void Remove (const IDType& id) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"LocalDocumentDB::MemoryDatabaseRep_::MyCollectionRep_::Remove"};
#endif
                fConnectionRep_->WrapExecute_ (
                    [&] () {
                        auto rwLock = fConnectionRep_->fCollections_.rwget ();
                        if (optional<CollectionRep_> oc = rwLock.cref ().Lookup (fTableName_)) {
                            CollectionRep_ c = *oc;
                            if (c.RemoveIf (id)) {
                                rwLock.rwref ().Add (fTableName_, c); // replace the actual collection in our master database of collections
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
            for (const KeyValuePair<String, CollectionRep_>& ci : fCollections_.load ()) {
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
            return Set<String>{fCollections_.load ().Keys ()};
        }
        virtual Document::Collection::Ptr CreateCollection (const String& name) override
        {
            auto rwLock = fCollections_.rwget ();
            if (not rwLock.cref ().Lookup (name)) {
                rwLock.rwref ().Add (name, {});
            }
            return GetCollection (name);
        }
        virtual void DropCollection (const String& name) override
        {
            auto rwLock = fCollections_.rwget ();
            rwLock.rwref ().RemoveIf (name);
        }
        virtual Document::Collection::Ptr GetCollection (const String& name) override
        {
            Require (fCollections_.load ().ContainsKey (name));
            return Document::Collection::Ptr{
                Memory::MakeSharedPtr<MyCollectionRep_> (Debug::UncheckedDynamicPointerCast<MemoryDatabaseRep_> (shared_from_this ()), name)};
        }
        virtual Document::Transaction mkTransaction () override
        {
            return Document::Transaction{make_unique<MyTransactionRep_> ()};
        }
        template <typename FUN>
        inline auto WrapExecute_ (FUN&& f, const optional<String>& collectionName, bool write)
        {
            return Document::Connection::Private_::WrapLoggingExecuteHelper_ (forward<FUN> (f), this, fOptions_, collectionName, write);
        }
    };

    /*
     *  Store collections in json file (leveraging MemoryDatabaseRep_ internally).
     *     \note   \em Thread-Safety   <a href='#Internally-Synchronized-Thread-Safety'>Internally-Synchronized-Thread-Safety</a>
     */
    struct SingleFileDatabaseRep_ final : Database::Document::Connection::IRep {

        const filesystem::path              fExternalFile_;
        shared_ptr<MemoryDatabaseRep_>      fMemoryDB_; // already internally syncrhonized, must be shared_ptr cuz it uses shared_from_this
        const DataExchange::Variant::Reader fReader_;
        const DataExchange::Variant::Writer fWriter_;

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
            virtual IDType Add (const Document::Document& v) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"LocalDocumentDB::SingleFileDatabaseRep_::MyCollectionRep_::Add"};
#endif
                return fDBRep_->WrapExecute_ (
                    [&] () {
                        [[maybe_unused]] auto rwLock = fDBRep_->fMemoryDB_->fCollections_.rwget ();
                        auto                  id     = fDelegateToInMemoryDB_->Add (v);
                        fDBRep_->DoWriteToFS ();
                        return id;
                    },
                    fName_, true);
            }
            virtual optional<Document::Document> Get (const IDType& id, const optional<Projection>& projection) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"LocalDocumentDB::SingleFileDatabaseRep_::MyCollectionRep_::Get"};
#endif
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
                fDBRep_->WrapExecute_ (
                    [&] () {
                        [[maybe_unused]] auto rwLock = fDBRep_->fMemoryDB_->fCollections_.rwget ();
                        fDelegateToInMemoryDB_->Update (id, newV, onlyTheseFields);
                        fDBRep_->DoWriteToFS ();
                    },
                    fName_, true);
            }
            virtual void Remove (const IDType& id) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"LocalDocumentDB::SingleFileDatabaseRep_::MyCollectionRep_::Remove"};
#endif
                fDBRep_->WrapExecute_ (
                    [&] () {
                        [[maybe_unused]] auto rwLock = fDBRep_->fMemoryDB_->fCollections_.rwget ();
                        fDelegateToInMemoryDB_->Remove (id);
                        fDBRep_->DoWriteToFS ();
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
        const OpertionCallbackPtr fOperationLoggingCallback_{nullptr};

        SingleFileDatabaseRep_ ()                              = delete;
        SingleFileDatabaseRep_ (const SingleFileDatabaseRep_&) = delete;
        SingleFileDatabaseRep_ ([[maybe_unused]] const Document::LocalDocumentDB::Options&   options,
                                const Document::LocalDocumentDB::Options::SingleFileStorage& sfOptions)
            : fExternalFile_{sfOptions.fFile}
            , fMemoryDB_{make_shared<MemoryDatabaseRep_> (stripOptionsForMemDB_ (options))}
            , fReader_{get<DataExchange::Variant::Reader> (sfOptions.fSerialization)}
            , fWriter_{get<DataExchange::Variant::Writer> (sfOptions.fSerialization)}
            , fOperationLoggingCallback_{options.fOperationLoggingCallback}
        {
            DoReadFromFS ();
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
            Database::Document::Connection::Options o = fMemoryDB_->GetOptions ();
            o.fOperationLoggingCallback               = fOperationLoggingCallback_;
            return o;
        }
        virtual uintmax_t GetSpaceConsumed () const override
        {
            error_code ignoredEC;
            return filesystem::file_size (fExternalFile_, ignoredEC);
        }
        virtual Set<String> GetCollections () override
        {
            return fMemoryDB_->GetCollections ();
        }
        virtual Document::Collection::Ptr CreateCollection (const String& name) override
        {
            return WrapExecute_ (
                [&] () {
                    [[maybe_unused]] auto rwLock = fMemoryDB_->fCollections_.rwget ();
                    fMemoryDB_->CreateCollection (name);
                    DoWriteToFS ();
                    return GetCollection (name);
                },
                name, true);
        }
        virtual void DropCollection (const String& name) override
        {
            WrapExecute_ (
                [&] () {
                    [[maybe_unused]] auto rwLock = fMemoryDB_->fCollections_.rwget ();
                    fMemoryDB_->DropCollection (name);
                    DoWriteToFS ();
                },
                name, true);
        }
        virtual Document::Collection::Ptr GetCollection (const String& name) override
        {
            Document::Collection::Ptr memDBCollection = fMemoryDB_->GetCollection (name);
            return Document::Collection::Ptr{Memory::MakeSharedPtr<MyCollectionRep_> (
                Debug::UncheckedDynamicPointerCast<SingleFileDatabaseRep_> (shared_from_this ()), name, memDBCollection)};
        }
        virtual Document::Transaction mkTransaction () override
        {
            return Document::Transaction{make_unique<MyTransactionRep_> ()};
        }
        void DoReadFromFS ()
        {
            using namespace IO::FileSystem;
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            TraceContextBumper ctx{"LocalDocumentDB::SingleFileDatabaseRep_::DoReadFromFS", "path={}"_f, fExternalFile_};
#endif
            if (filesystem::exists (fExternalFile_)) {
                auto rwLock = fMemoryDB_->fCollections_.rwget ();
                rwLock.rwref ().clear ();
                for (KeyValuePair<String, VariantValue> collectionAndDocument :
                     fReader_.Read (FileInputStream::New (fExternalFile_)).As<Mapping<String, VariantValue>> ()) {
                    rwLock.rwref ().Add (collectionAndDocument.fKey,
                                         collectionAndDocument.fValue.As<Mapping<String, VariantValue>> ().Map<Mapping<GUID, Document::Document>> (
                                             [&] (const KeyValuePair<String, VariantValue>& kvp) -> KeyValuePair<GUID, Document::Document> {
                                                 return {GUID{kvp.fKey}, kvp.fValue.As<Document::Document> ()};
                                             }));
                }
            }
        }
        void DoWriteToFS ()
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            TraceContextBumper ctx{"LocalDocumentDB::SingleFileDatabaseRep_::DoWriteToFS", "path={}"_f, fExternalFile_};
#endif
            using namespace IO::FileSystem;
            ThroughTmpFileWriter                  tmpFile{fExternalFile_};
            IO::FileSystem::FileOutputStream::Ptr outStream = IO::FileSystem::FileOutputStream::New (tmpFile.GetFilePath ());
            Mapping<String, VariantValue>         collectionsAsVV;
            for (const KeyValuePair<String, Mapping<GUID, Document::Document>>& collection : fMemoryDB_->fCollections_.load ()) {
                Mapping<GUID, Document::Document> collectionValue = collection.fValue;
                Mapping<String, VariantValue>     collWithStringKey;
                for (const KeyValuePair<GUID, Document::Document>& kvp : collectionValue) {
                    collWithStringKey.Add (kvp.fKey.ToString (), VariantValue{kvp.fValue});
                }
                collectionsAsVV.Add (collection.fKey, VariantValue{collWithStringKey});
            }
            this->fWriter_.Write (VariantValue{collectionsAsVV}, outStream);
            outStream.Close (); // close like this so we can throw exception - cannot throw if we count on DTOR
            tmpFile.Commit ();  // any exceptions cause the tmp file to be automatically cleaned up
        }
        template <typename FUN>
        inline auto WrapExecute_ (FUN&& f, const optional<String>& collectionName, bool write)
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
    struct DirectoryFilesystemDatabaseRep_ final : Database::Document::Connection::IRep {
        const Document::LocalDocumentDB::Options fOptions_;
        const filesystem::path                   fRoot_;
        const DataExchange::Variant::Reader      fReader_;
        const DataExchange::Variant::Writer      fWriter_;

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
                                    fDBRep_->fReader_.Read (IO::FileSystem::FileInputStream::New (entry.path ())).As<Document::Document> ();
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
                    return fDBRep_->fReader_.Read (FileInputStream::New (docFilePath)).As<Document::Document> ();
                }
                return nullopt;
            }
            void DoWriteToFS_ (const GUID& id, const VariantValue& vv)
            {
                filesystem::path docFilePath = GetDocumentFilePath_ (id);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"LocalDocumentDB::DirectoryFilesystemDatabaseRep_::DoWriteToFS()", "path={}"_f, docFilePath};
#endif
                using namespace IO::FileSystem;
                ThroughTmpFileWriter                  tmpFile{docFilePath};
                IO::FileSystem::FileOutputStream::Ptr outStream = IO::FileSystem::FileOutputStream::New (tmpFile.GetFilePath ());
                fDBRep_->fWriter_.Write (vv, outStream);
                outStream.Close (); // close like this so we can throw exception - cannot throw if we count on DTOR
                tmpFile.Commit ();  // any exceptions cause the tmp file to be automatically cleaned up
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
        template <typename FUN>
        inline auto WrapExecute_ (FUN&& f, const optional<String>& collectionName, bool write)
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
    if (get_if<Options::MemoryStorage> (&options.fStorage)) {
        return Ptr{Memory::MakeSharedPtr<MemoryDatabaseRep_> (options)};
    }
    else if (auto fop = get_if<Options::SingleFileStorage> (&options.fStorage)) {
        return Ptr{Memory::MakeSharedPtr<SingleFileDatabaseRep_> (options, *fop)};
    }
    else if (auto dop = get_if<Options::DirectoryFileStorage> (&options.fStorage)) {
        return Ptr{Memory::MakeSharedPtr<DirectoryFilesystemDatabaseRep_> (options, *dop)};
    }
    AssertNotImplemented ();
    return nullptr;
}
