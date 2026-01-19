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
                if (projection && r) {
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
                static const auto  kExcept1_  = RuntimeErrorException{"no such table"};
                static const auto  kExcept_   = RuntimeErrorException{"no such id"};
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

        MemoryDatabaseRep_ ([[maybe_unused]] const Options& options)
        {
            TraceContextBumper ctx{"TrivialDocumentDB::MemoryDatabaseRep_::MemoryDatabaseRep_"};
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

    struct SingleFileDatabaseRep_ final : Database::Document::Connection::IRep {

        using CollectionRep_ = Mapping<GUID, Document::Document>;

        Synchronized<MemoryDatabaseRep_> fMemoryDB_;

        struct MyCollectionRep_ final : Document::Collection::IRep {
            const shared_ptr<SingleFileDatabaseRep_> fOwningCollectionRep_; // save to bump reference count (lifetime safety), and to force write

            shared_ptr<Database::Document::Collection::IRep> fDelegateTo_;

            MyCollectionRep_ (const shared_ptr<SingleFileDatabaseRep_>& connectionRep, const shared_ptr<Database::Document::Collection::IRep>& delgateImplTo)
                : fOwningCollectionRep_{connectionRep}
                , fDelegateTo_{delgateImplTo}
            {
            }
            virtual IDType Add (const Document::Document& v) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"TrivialDocumentDB::SingleFileDatabaseRep_::MyCollectionRep_::Add()"};
#endif
                auto id = fDelegateTo_->Add (v);
                fOwningCollectionRep_->DoWriteToFS ();
                return id;
            }
            virtual optional<Document::Document> GetOne (const IDType& id, const optional<Projection>& projection) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"TrivialDocumentDB::SingleFileDatabaseRep_::MyCollectionRep_::GetOne()"};
#endif
                return fDelegateTo_->GetOne (id, projection);
            }
            virtual Sequence<Document::Document> GetAll (const optional<Filter>& filter, const optional<Projection>& projection) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"TrivialDocumentDB::SingleFileDatabaseRep_::MyCollectionRep_::GetAll()",
                                       "filter={}, projection={}"_f, filter, projection};
#endif
                return fDelegateTo_->GetAll (filter, projection);
            }
            virtual void Update (const IDType& id, const Document::Document& newV, const optional<Set<String>>& onlyTheseFields) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"TrivialDocumentDB::SingleFileDatabaseRep_::MyCollectionRep_::Update()"};
#endif
                fDelegateTo_->Update (id, newV, onlyTheseFields);
                fOwningCollectionRep_->DoWriteToFS ();
            }
            virtual void Remove (const IDType& id) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"TrivialDocumentDB::SingleFileDatabaseRep_::MyCollectionRep_::Remove()"};
#endif
                fDelegateTo_->Remove (id);
                fOwningCollectionRep_->DoWriteToFS ();
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

        SingleFileDatabaseRep_ ([[maybe_unused]] const Options& options)
            : fMemoryDB_{options}
        {
            TraceContextBumper ctx{"TrivialDocumentDB::SingleFileDatabaseRep_::SingleFileDatabaseRep_"};
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
            return fMemoryDB_.rwget ()->GetCollections ();
        }
        virtual void CreateCollection (const String& name) override
        {
            auto rwLock = fMemoryDB_.rwget ();
            rwLock.rwref ().CreateCollection (name);
            DoWriteToFS ();
        }
        virtual void DropCollection (const String& name) override
        {
            auto rwLock = fMemoryDB_.rwget ();
            rwLock.rwref ().DropCollection (name);
            DoWriteToFS ();
        }
        virtual Document::Collection::Ptr GetCollection (const String& name) override
        {
            return Document::Collection::Ptr{Memory::MakeSharedPtr<MyCollectionRep_> (
                Debug::UncheckedDynamicPointerCast<SingleFileDatabaseRep_> (shared_from_this ()), fMemoryDB_.rwget ()->GetCollection (name))};
        }
        virtual Document::Transaction mkTransaction () override
        {
            return Document::Transaction{make_unique<MyTransactionRep_> ()};
        }

        void DoReadFromFS ()
        {
            // @todo
        }
        void DoWriteToFS ()
        {
            // @todo
        }
    };

    struct DirectoryFilesystemDatabaseRep_ final : Database::Document::Connection::IRep {

        using CollectionRep_ = Mapping<GUID, Document::Document>;

        // @todo alot to rewrite - store PATH to collection (A DIRECTORY under root directory)

        // @todo figure out how to map filenames safely so no conflicts) - URL-ENCODE filenames?
        // then must URL_DECODE?

        Synchronized<Mapping<String, CollectionRep_>> fCollections_;

        struct MyCollectionRep_ final : Document::Collection::IRep {
            const shared_ptr<DirectoryFilesystemDatabaseRep_> fConnectionRep_; // save to bump reference count
            const String                                      fTableName_;

            MyCollectionRep_ (const shared_ptr<DirectoryFilesystemDatabaseRep_>& connectionRep, const String& collectionName)
                : fConnectionRep_{connectionRep}
                , fTableName_{collectionName}
            {
            }
            virtual IDType Add (const Document::Document& v) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"TrivialDocumentDB::DirectoryFilesystemDatabaseRep_::MyCollectionRep_::Add()"};
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
                TraceContextBumper ctx{"TrivialDocumentDB::DirectoryFilesystemDatabaseRep_::MyCollectionRep_::GetOne()"};
#endif
                auto r = fConnectionRep_->fCollections_->LookupValue (fTableName_).Lookup (GUID{id});
                if (r) {
                    r->Add (Document::kID, id);
                }
                if (projection && r) {
                    r = projection->Apply (*r);
                }
                return r;
            }
            virtual Sequence<Document::Document> GetAll (const optional<Filter>& filter, const optional<Projection>& projection) override
            {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                TraceContextBumper ctx{"TrivialDocumentDB::DirectoryFilesystemDatabaseRep_::MyCollectionRep_::GetAll()",
                                       "filter={}, projection={}"_f, filter, projection};
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
                TraceContextBumper ctx{"TrivialDocumentDB::DirectoryFilesystemDatabaseRep_::MyCollectionRep_::Update()"};
#endif
                Document::Document uploadDoc = newV;
                if (onlyTheseFields) {
                    uploadDoc.RetainAll (*onlyTheseFields);
                }
                static const auto  kExcept1_  = RuntimeErrorException{"no such table"};
                static const auto  kExcept_   = RuntimeErrorException{"no such id"};
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
                TraceContextBumper ctx{"TrivialDocumentDB::DirectoryFilesystemDatabaseRep_::MyCollectionRep_::Remove()"};
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

        DirectoryFilesystemDatabaseRep_ ([[maybe_unused]] const Options& options)
        {
            TraceContextBumper ctx{"TrivialDocumentDB::DirectoryFilesystemDatabaseRep_::DirectoryFilesystemDatabaseRep_"};
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
    else if (auto op = get_if<Options::SingleFileStorage> (&options.fStorage)) {
        return Ptr{Memory::MakeSharedPtr<SingleFileDatabaseRep_> (options)};
    }
    else if (auto op = get_if<Options::DirectoryFileStorage> (&options.fStorage)) {
        return Ptr{Memory::MakeSharedPtr<DirectoryFilesystemDatabaseRep_> (options)};
    }
    AssertNotImplemented ();
    return nullptr;
}
