/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_Foundation_Common_Platform_Windows
#include <windows.h>
#endif

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/Format.h"
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

#if qStroika_HasComponent_mongocxxdriver

namespace {
    using Connection::Options;
    struct Rep_ final : Stroika::Foundation::Database::Document::MongoDBClient::Connection::IRep {

        mongocxx::client fClient_;

        Rep_ (const Options& options)
            : fClient_{mongocxx::uri{options.fConnectionString.AsUTF8<string> ()}}  // @todo not sure about charset to map to?
        {
            TraceContextBumper ctx{"Document::MongoDBClient::Connection::Rep_::Rep_"};
        }
        ~Rep_ ()
        {
        }
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
            return {};
        }

    public:
        virtual void CreateCollection (const String& name) override
        {
        }

    public:
        virtual void DropCollection (const String& name) override
        {
        }
        virtual Document::Transaction mkTransaction () override
        {
            Connection::Ptr conn = Connection::Ptr{Debug::UncheckedDynamicPointerCast<Connection::IRep> (shared_from_this ())};
            return Transaction{conn};
        }

        virtual void run_command (const VariantValue& v) override
        {
           // fClient_.run_command (bsoncxx::from_json (R"({ "ping": 1 })"));
        }
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
        fAssertExternallySynchronizedMutex.SetAssertExternallySynchronizedMutexContext (src->fAssertExternallySynchronizedMutex.GetSharedContext ());
    }
#endif
}

/*
 ********************************************************************************
 ******************* Document::MongoDBClient::Connection::New *******************
 ********************************************************************************
 */
auto Document::MongoDBClient::Connection::New (const Options& options) -> Ptr
{
    return Ptr{make_shared<Rep_> (options)};
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
