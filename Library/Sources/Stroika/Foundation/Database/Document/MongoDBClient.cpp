/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_HasComponent_mongocxxdriver
#include <mongocxx/collection-fwd.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#endif

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Writer.h"
#include "Stroika/Foundation/Database/Exception.h"
#include "Stroika/Foundation/Debug/Main.h"

#include "MongoDBClient.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Database;
using namespace Stroika::Foundation::Database::Document::MongoDBClient;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Debug;

using Database::Document::EngineProperties;

/**
 *  Character set Docx
 *      Haven't found clear docs about characterset (apis all take 'string') - but it appears (search through docs for
 *      characterset comes up with nothing but UTF-8) that they use UTF-8.
 */

#if qStroika_HasComponent_mongocxxdriver

namespace {
    VariantValue FromBSON_ (const bsoncxx::v_noabi::document::value& b)
    {
        // @todo - this is a ROUGH approximation - but deal with 'extended json' and make more efficient - especially BLOBS
        return Variant::JSON::Reader{}.Read (String::FromUTF8 (bsoncxx::to_json (b.view ())));
    }
    bsoncxx::v_noabi::document::value ToBSON_ (const VariantValue& vv)
    {
        // @todo - this is a ROUGH approximation - but deal with 'extended json' and make more efficient - especially BLOBS
        //        return bsoncxx::from_json (R"({ "ping": 1 })");
        return bsoncxx::from_json (Variant::JSON::Writer{}.WriteAsString (vv).AsUTF8<string> ());
    }
}

namespace {
    struct AdminRep_ final : Stroika::Foundation::Database::Document::MongoDBClient::AdminConnection::IRep {
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fAssertExternallySynchronizedMutex;
        mongocxx::client                                               fClient_;

        AdminRep_ (const AdminConnection::Options& options)
            : fClient_{mongocxx::uri{options.fConnectionString.AsUTF8<string> ()}} // @todo not sure about charset to map to?
        {
            TraceContextBumper ctx{"Document::MongoDBClient::AdminConnection::Rep_::Rep_"};
        }
        ~AdminRep_ () = default;

        // MongoDBClient::Connection::IRep overrides
    public:
        virtual VariantValue run_command (const VariantValue& v) override
        {
            mongocxx::database adminDB_;
            return FromBSON_ (fClient_.database ("admin").run_command (ToBSON_ (v)));
        }
        virtual Set<String> GetDatabases () override
        {
            vector<string> n = fClient_.list_database_names ();
            return Iterable<string>{n}.Map<Set<String>> ([] (string i) { return String{i}; });
        }
        virtual void DropDatabase (const String& dbName) override
        {
            mongocxx::database{fClient_.database (dbName.AsUTF8<string> ())}.drop ();
        }
        virtual void CreateDatabase (const String& dbName) override
        {
            // doesn't appear to be anything todo to create the database except maybe  writing to it
            mongocxx::database d{fClient_.database (dbName.AsUTF8<string> ())};
            d.create_collection ("_junk_");
            d.collection ("_junk_").drop ();
        }
    };
}

namespace {
    struct ConnectionRep_ final : Stroika::Foundation::Database::Document::MongoDBClient::Connection::IRep {
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fAssertExternallySynchronizedMutex;
        mongocxx::client                                               fClient_;
        mutable optional<mongocxx::database>                           fDatabase;

        ConnectionRep_ (const Connection::Options& options)
            : fClient_{mongocxx::uri{options.fConnectionString.AsUTF8<string> ()}} // @todo not sure about charset to map to?
        {
            TraceContextBumper ctx{"Document::MongoDBClient::Connection::Rep_::Rep_"};
            if (options.fDatabase) {
                fDatabase = mongocxx::database{fClient_.database (options.fDatabase.AsUTF8<string> ())};
            }
        }
        ~ConnectionRep_ () = default;

        // Document::Connection::IRep overrides
    public:
        virtual shared_ptr<const EngineProperties> GetEngineProperties () const override
        {
            AssertNotImplemented ();
            struct MyEngineProperties_ final : EngineProperties {
                virtual String GetEngineName () const override
                {
                    return "MongoDBClient"sv; // must indirect to connection to get more info (from dns at least? not clear)
                }
            };
            return make_shared<const MyEngineProperties_> (); // dynamic info based on connection/dsn
        }
        virtual Set<String> GetCollections () const override
        {
            Require (fDatabase); // caller must specify a database in connection options
            try {
                vector<string> n = fDatabase->list_collection_names ();
                return Iterable<string>{n}.Map<Set<String>> ([] (string i) { return String{i}; });
            }
            catch (const mongocxx::v_noabi::operation_exception& e) {
                // a specific error here - check for that - and throw others... no such database gets mapped to empty collection list
                return {};
            }
        }
        virtual void CreateCollection (const String& name) override
        {
            Require (fDatabase); // caller must specify a database in connection options
            fDatabase->create_collection (name.AsUTF8<string> ());
        }
        virtual void DropCollection (const String& name) override
        {
            Require (fDatabase); // caller must specify a database in connection options
            fDatabase->collection (name.AsUTF8<string> ()).drop ();
        }
        virtual Document::Collection::Ptr GetCollection (const String& name) override
        {
            return Document::Collection::Ptr{nullptr}; // @todo - implement this!
        }
        virtual Document::Transaction mkTransaction () override
        {
            Require (fDatabase); // caller must specify a database in connection options
            Connection::Ptr conn = Connection::Ptr{Debug::UncheckedDynamicPointerCast<Connection::IRep> (shared_from_this ())};
            return Transaction{conn};
        }

        // MongoDBClient::Connection::IRep overrides
    public:
    };
}

/*
 ********************************************************************************
 ********************* Document::MongoDBClient::Activator ***********************
 ********************************************************************************
 */
#if qStroika_Foundation_Debug_AssertionsChecked
Document::MongoDBClient::Activator::Activator ()
{
    Require (Debug::AppearsDuringMainLifetime ());
}

Document::MongoDBClient::Activator::~Activator ()
{
    Require (Debug::AppearsDuringMainLifetime ());
}
#endif

/*
 ********************************************************************************
 ****************** Document::MongoDBClient::Connection::Ptr ********************
 ********************************************************************************
 */
Document::MongoDBClient::Connection::Ptr::Ptr (const shared_ptr<IRep>& src)
    : inherited{src}
{
#if qStroika_Foundation_Debug_AssertExternallySynchronizedMutex_Enabled
    if (src != nullptr) {
        // @todo fix... - cast...
        //       fAssertExternallySynchronizedMutex.SetAssertExternallySynchronizedMutexContext (src->fAssertExternallySynchronizedMutex.GetSharedContext ());
    }
#endif
}

/*
 ********************************************************************************
 **************** Document::MongoDBClient::AdminConnection::New *****************
 ********************************************************************************
 */
auto Document::MongoDBClient::AdminConnection::New (const AdminConnection::Options& options) -> Ptr
{
    return Ptr{make_shared<AdminRep_> (options)};
}

/*
 ********************************************************************************
 ******************* Document::MongoDBClient::Connection::New *******************
 ********************************************************************************
 */
auto Document::MongoDBClient::Connection::New (const Connection::Options& options) -> Ptr
{
    return Ptr{make_shared<ConnectionRep_> (options)};
}

/*
 ********************************************************************************
 ******************************* SQLite::Transaction ****************************
 ********************************************************************************
 */
struct Transaction::MyRep_ : public MongoDBClient::Transaction::IRep {
    MyRep_ (const Connection::Ptr& db)
        : fConnectionPtr_{db}
    {
        AssertNotImplemented ();
    }
    virtual void Commit () override
    {
        Require (not fCompleted_);
        fCompleted_ = true;
        // fConnectionPtr_->Exec ("COMMIT;"sv);
    }
    virtual void Rollback () override
    {
        Require (not fCompleted_);
        fCompleted_ = true;
        // fConnectionPtr_->Exec ("ROLLBACK;"sv);
    }
    virtual Disposition GetDisposition () const override
    {
        // @todo record more info so we can report finer grained status ; try/catch in rollback/commit and dbgtraces
        return fCompleted_ ? Disposition::eCompleted : Disposition::eNone;
    }
    Connection::Ptr fConnectionPtr_;
    bool            fCompleted_{false};
};
Transaction::Transaction (const Connection::Ptr& db)
    : inherited{make_unique<MyRep_> (db)}
{
}
#endif
