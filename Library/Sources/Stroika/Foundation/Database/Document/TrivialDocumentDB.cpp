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

#include "TrivialDocumentDB.h"

using namespace Stroika::Foundation;

using namespace Characters;
using namespace Containers;
using namespace Debug;
using namespace DataExchange;
using namespace Database;
using namespace Database::Document::TrivialDocumentDB;
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
                TraceContextBumper ctx{"TrivialDocumentDB::MemoryDatabaseRep_::MyCollectionRep_::Add()"};
#endif
                auto           rwLock     = fConnectionRep_->fCollections_.rwget ();
                CollectionRep_ collection = rwLock.cref ().LookupValue (fTableName_);
                GUID           id         = GUID::GenerateNew ();
                collection.Add (id, v);
                rwLock.rwref ().Add (fTableName_, collection);
                return id.ToString ();
            }
            virtual optional<Document::Document> GetOne (const IDType& id, const optional<Projection>& projection) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"TrivialDocumentDB::MemoryDatabaseRep_::MyCollectionRep_::GetOne()"};
#endif
                auto r = fConnectionRep_->fCollections_->LookupValue (fTableName_).Lookup (GUID{id});
                if (r) {
                    r->Add (Document::kID, id);
                }
                if (projection and r) {
                    r = projection->Apply (*r);
                }
                return r;
            }
            virtual Sequence<Document::Document> GetAll (const optional<Filter>& filter, const optional<Projection>& projection) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"TrivialDocumentDB::MemoryDatabaseRep_::MyCollectionRep_::GetAll()", "filter={}, projection={}"_f,
                                       filter, projection};
#endif
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
            }
            virtual void Update (const IDType& id, const Document::Document& newV, const optional<Set<String>>& onlyTheseFields) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"TrivialDocumentDB::MemoryDatabaseRep_::MyCollectionRep_::Update()"};
#endif
                Document::Document uploadDoc = newV;
                if (onlyTheseFields) {
                    uploadDoc.RetainAll (*onlyTheseFields);
                }
                static const auto  kExcept1_  = RuntimeErrorException{"no such table"sv};
                static const auto  kExcept_   = RuntimeErrorException{"no such id"sv};
                auto               rwLock     = fConnectionRep_->fCollections_.rwget ();
                CollectionRep_     collection = rwLock.cref ().LookupChecked (fTableName_, kExcept1_);
                Document::Document d2Update   = onlyTheseFields ? collection.LookupChecked (id, kExcept_) : uploadDoc;
                // any fields listed in onlyTheseFields, but not present in newV need to be removed
                if (onlyTheseFields) {
                    d2Update.AddAll (uploadDoc);
                    Set<String> removeMe = *onlyTheseFields - newV.Keys ();
                    d2Update.RemoveAll (removeMe);
                }
                collection.Add (id, d2Update);
                rwLock.rwref ().Add (fTableName_, collection); // replace the actual collection in our master database of collections
            }
            virtual void Remove (const IDType& id) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"TrivialDocumentDB::MemoryDatabaseRep_::MyCollectionRep_::Remove()"};
#endif
                auto rwLock = fConnectionRep_->fCollections_.rwget ();
                if (optional<CollectionRep_> oc = rwLock.cref ().Lookup (fTableName_)) {
                    CollectionRep_ c = *oc;
                    if (c.RemoveIf (id)) {
                        rwLock.rwref ().Add (fTableName_, c); // replace the actual collection in our master database of collections
                    }
                }
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
        MemoryDatabaseRep_ ([[maybe_unused]] const Options& options)
        {
            TraceContextBumper ctx{"TrivialDocumentDB::MemoryDatabaseRep_::MemoryDatabaseRep_"};
            //Assert (shared_from_this ().get () == this); // only support allocating with make_shared - cannot check here cuz object not yet fully constructed
        }
        virtual shared_ptr<const EngineProperties> GetEngineProperties () const override
        {
            struct MyEngineProperties_ final : EngineProperties {
                virtual String GetEngineName () const override
                {
                    return "TrivialDocumentDB.MemoryDB"sv;
                }
            };
            static const shared_ptr<const EngineProperties> kProps_ = Memory::MakeSharedPtr<const MyEngineProperties_> ();
            return kProps_;
        }
        virtual Set<String> GetCollections () override
        {
            return Set<String>{fCollections_.load ().Keys ()};
        }
        virtual void CreateCollection (const String& name) override
        {
            auto rwLock = fCollections_.rwget ();
            if (not rwLock.cref ().Lookup (name)) {
                rwLock.rwref ().Add (name, {});
            }
        }
        virtual void DropCollection (const String& name) override
        {
            auto rwLock = fCollections_.rwget ();
            rwLock.rwref ().RemoveIf (name);
        }
        virtual Document::Collection::Ptr GetCollection (const String& name) override
        {
            return Document::Collection::Ptr{
                Memory::MakeSharedPtr<MyCollectionRep_> (Debug::UncheckedDynamicPointerCast<MemoryDatabaseRep_> (shared_from_this ()), name)};
        }
        virtual Document::Transaction mkTransaction () override
        {
            return Document::Transaction{make_unique<MyTransactionRep_> ()};
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
            shared_ptr<Database::Document::Collection::IRep> fDelegateToInMemoryDB_; // inside memorydb

            MyCollectionRep_ (const shared_ptr<SingleFileDatabaseRep_>& dbRep, const shared_ptr<Database::Document::Collection::IRep>& delgateImplTo)
                : fDBRep_{dbRep}
                , fDelegateToInMemoryDB_{delgateImplTo}
            {
            }
            virtual IDType Add (const Document::Document& v) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"TrivialDocumentDB::SingleFileDatabaseRep_::MyCollectionRep_::Add()"};
#endif
                [[maybe_unused]] auto rwLock = fDBRep_->fMemoryDB_->fCollections_.rwget ();
                auto                  id     = fDelegateToInMemoryDB_->Add (v);
                fDBRep_->DoWriteToFS ();
                return id;
            }
            virtual optional<Document::Document> GetOne (const IDType& id, const optional<Projection>& projection) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"TrivialDocumentDB::SingleFileDatabaseRep_::MyCollectionRep_::GetOne()"};
#endif
                return fDelegateToInMemoryDB_->GetOne (id, projection);
            }
            virtual Sequence<Document::Document> GetAll (const optional<Filter>& filter, const optional<Projection>& projection) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"TrivialDocumentDB::SingleFileDatabaseRep_::MyCollectionRep_::GetAll()",
                                       "filter={}, projection={}"_f, filter, projection};
#endif
                return fDelegateToInMemoryDB_->GetAll (filter, projection);
            }
            virtual void Update (const IDType& id, const Document::Document& newV, const optional<Set<String>>& onlyTheseFields) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"TrivialDocumentDB::SingleFileDatabaseRep_::MyCollectionRep_::Update()"};
#endif
                [[maybe_unused]] auto rwLock = fDBRep_->fMemoryDB_->fCollections_.rwget ();
                fDelegateToInMemoryDB_->Update (id, newV, onlyTheseFields);
                fDBRep_->DoWriteToFS ();
            }
            virtual void Remove (const IDType& id) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"TrivialDocumentDB::SingleFileDatabaseRep_::MyCollectionRep_::Remove()"};
#endif
                [[maybe_unused]] auto rwLock = fDBRep_->fMemoryDB_->fCollections_.rwget ();
                fDelegateToInMemoryDB_->Remove (id);
                fDBRep_->DoWriteToFS ();
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

        SingleFileDatabaseRep_ ()                              = delete;
        SingleFileDatabaseRep_ (const SingleFileDatabaseRep_&) = delete;
        SingleFileDatabaseRep_ ([[maybe_unused]] const Options& options, const Options::SingleFileStorage& sfOptions)
            : fMemoryDB_{make_shared<MemoryDatabaseRep_> (options)}
            , fExternalFile_{sfOptions.fFile}
            , fReader_{get<DataExchange::Variant::Reader> (sfOptions.fSerialization)}
            , fWriter_{get<DataExchange::Variant::Writer> (sfOptions.fSerialization)}
        {
            DoReadFromFS ();
        }
        virtual shared_ptr<const EngineProperties> GetEngineProperties () const override
        {
            struct MyEngineProperties_ final : EngineProperties {
                virtual String GetEngineName () const override
                {
                    return "TrivialDocumentDB.SingleFile"sv;
                }
            };
            static const shared_ptr<const EngineProperties> kProps_ = Memory::MakeSharedPtr<const MyEngineProperties_> ();
            return kProps_;
        }
        virtual Set<String> GetCollections () override
        {
            return fMemoryDB_->GetCollections ();
        }
        virtual void CreateCollection (const String& name) override
        {
            [[maybe_unused]] auto rwLock = fMemoryDB_->fCollections_.rwget ();
            fMemoryDB_->CreateCollection (name);
            DoWriteToFS ();
        }
        virtual void DropCollection (const String& name) override
        {
            [[maybe_unused]] auto rwLock = fMemoryDB_->fCollections_.rwget ();
            fMemoryDB_->DropCollection (name);
            DoWriteToFS ();
        }
        virtual Document::Collection::Ptr GetCollection (const String& name) override
        {
            Document::Collection::Ptr memDBCollection = fMemoryDB_->GetCollection (name);
            return Document::Collection::Ptr{Memory::MakeSharedPtr<MyCollectionRep_> (
                Debug::UncheckedDynamicPointerCast<SingleFileDatabaseRep_> (shared_from_this ()), memDBCollection)};
        }
        virtual Document::Transaction mkTransaction () override
        {
            return Document::Transaction{make_unique<MyTransactionRep_> ()};
        }
        void DoReadFromFS ()
        {
            using namespace IO::FileSystem;
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            TraceContextBumper ctx{"TrivialDocumentDB::SingleFileDatabaseRep_::DoReadFromFS", "path={}"_f, fExternalFile_};
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
            TraceContextBumper ctx{"TrivialDocumentDB::SingleFileDatabaseRep_::DoWriteToFS()", "path={}"_f, fExternalFile_};
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
    };

    // Store each collection in a folder under the root folder
    struct DirectoryFilesystemDatabaseRep_ final : Database::Document::Connection::IRep {
        const filesystem::path              fRoot_;
        const DataExchange::Variant::Reader fReader_;
        const DataExchange::Variant::Writer fWriter_;

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
                TraceContextBumper ctx{"TrivialDocumentDB::DirectoryFilesystemDatabaseRep_::MyCollectionRep_::Add()"};
#endif
                GUID id = GUID::GenerateNew ();
                DoWriteToFS_ (id, VariantValue{v});
                return id.As<IDType> ();
            }
            virtual optional<Document::Document> GetOne (const IDType& id, const optional<Projection>& projection) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"TrivialDocumentDB::DirectoryFilesystemDatabaseRep_::MyCollectionRep_::GetOne()"};
#endif
                if (auto od = DoReadFromFS_ (GUID{id})) {
                    Document::Document d = *od;
                    d.Add (Document::kID, id);
                    if (projection) {
                        d = projection->Apply (d);
                    }
                    return d;
                }
                else {
                    return nullopt;
                }
            }
            virtual Sequence<Document::Document> GetAll (const optional<Filter>& filter, const optional<Projection>& projection) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"TrivialDocumentDB::DirectoryFilesystemDatabaseRep_::MyCollectionRep_::GetAll()",
                                       "filter={}, projection={}"_f, filter, projection};
#endif
                Sequence<Document::Document> result;
                for (const auto& entry : filesystem::directory_iterator{fCollectionRoot_}) {
                    if (entry.path ().extension () == ".json"sv) { // Check if the entry is a JSON file
                        Document::Document d =
                            fDBRep_->fReader_.Read (IO::FileSystem::FileInputStream::New (entry.path ())).As<Document::Document> ();
                        d.Add (Document::kID, entry.path ().stem ().string ());
                        if (not filter or not filter->Matches (d)) {
                            if (projection) {
                                d = projection->Apply (d);
                            }
                            result += d;
                        }
                    }
                }
                return result;
            }
            virtual void Update (const IDType& id, const Document::Document& newV, const optional<Set<String>>& onlyTheseFields) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"TrivialDocumentDB::DirectoryFilesystemDatabaseRep_::MyCollectionRep_::Update()"};
#endif
                Document::Document d = Memory::ValueOfOrThrow (DoReadFromFS_ (id), RuntimeErrorException{"no such id"sv});
                if (onlyTheseFields) {
                    Document::Document uploadDoc = newV;
                    if (onlyTheseFields) {
                        uploadDoc.RetainAll (*onlyTheseFields);
                    }
                    d.AddAll (uploadDoc);
                }
                DoWriteToFS_ (GUID{id}, VariantValue{d});
            }
            virtual void Remove (const IDType& id) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"TrivialDocumentDB::DirectoryFilesystemDatabaseRep_::MyCollectionRep_::Remove()"};
#endif
                (void)filesystem::remove (GetDocumentFilePath_ (GUID{id}));
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
                TraceContextBumper ctx{"TrivialDocumentDB::DirectoryFilesystemDatabaseRep_::DoReadFromFS", "path={}"_f, docFilePath};
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
                TraceContextBumper ctx{"TrivialDocumentDB::DirectoryFilesystemDatabaseRep_::DoWriteToFS()", "path={}"_f, docFilePath};
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
        DirectoryFilesystemDatabaseRep_ (const Options::DirectoryFileStorage& dfOptions)
            : fRoot_{dfOptions.fRoot}
            , fReader_{get<DataExchange::Variant::Reader> (dfOptions.fSerialization)}
            , fWriter_{get<DataExchange::Variant::Writer> (dfOptions.fSerialization)}
        {
        }
        virtual shared_ptr<const EngineProperties> GetEngineProperties () const override
        {
            struct MyEngineProperties_ final : EngineProperties {
                virtual String GetEngineName () const override
                {
                    return "TrivialDocumentDB.Folder"sv;
                }
            };
            static const shared_ptr<const EngineProperties> kProps_ = Memory::MakeSharedPtr<const MyEngineProperties_> ();
            return kProps_;
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
        virtual void CreateCollection (const String& name) override
        {
            filesystem::create_directories (GetCollectionFilePath_ (name));
        }
        virtual void DropCollection (const String& name) override
        {
            filesystem::remove_all (GetCollectionFilePath_ (name));
        }
        virtual Document::Collection::Ptr GetCollection (const String& name) override
        {
            return Document::Collection::Ptr{Memory::MakeSharedPtr<MyCollectionRep_> (
                Debug::UncheckedDynamicPointerCast<DirectoryFilesystemDatabaseRep_> (shared_from_this ()), name)};
        }
        virtual Document::Transaction mkTransaction () override
        {
            return Document::Transaction{make_unique<MyTransactionRep_> ()};
        }
    };
}

/*
 ********************************************************************************
 *********************** SQL::TrivialDocumentDB::New ****************************
 ********************************************************************************
 */
auto Document::TrivialDocumentDB::New (const Options& options) -> Ptr
{
    if (get_if<Options::MemoryStorage> (&options.fStorage)) {
        return Ptr{Memory::MakeSharedPtr<MemoryDatabaseRep_> (options)};
    }
    else if (auto fop = get_if<Options::SingleFileStorage> (&options.fStorage)) {
        return Ptr{Memory::MakeSharedPtr<SingleFileDatabaseRep_> (options, *fop)};
    }
    else if (auto dop = get_if<Options::DirectoryFileStorage> (&options.fStorage)) {
        return Ptr{Memory::MakeSharedPtr<DirectoryFilesystemDatabaseRep_> (*dop)};
    }
    AssertNotImplemented ();
    return nullptr;
}
