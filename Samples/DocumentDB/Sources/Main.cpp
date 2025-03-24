/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdlib>

#include "Stroika/Foundation/Database/Document/MongoDBClient.h"
#include "Stroika/Foundation/Database/Document/SQLite.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/IO/FileSystem/WellKnownLocations.h"

#include "ComputerNetwork.h"
#include "EmployeesDB.h"

using namespace std;

using namespace Stroika::Foundation;

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    using namespace Stroika::Foundation::Database::Document;

    using namespace Stroika::Samples::Document;


    #if qStroika_HasComponent_mongocxxdriver
    {
        using namespace Stroika::Foundation::Database::Document::MongoDBClient;
        const String kDefaultMongoConnectionString_ = "mongodb://admin:pass@localhost:27017"sv;
        String connectionString               = kDefaultMongoConnectionString_; // check cmdline
        Activator activator{Activator::eAllowReactivateFlag};               // must exist while using this library

        const String kTestDBName_ = "DocumentDB-Sample-Networks"sv;
        auto         adminDB      = AdminConnection::New (AdminConnection::Options{.fConnectionString = connectionString});
        IgnoreExceptionsForCall (adminDB.DropDatabase (kTestDBName_));
        adminDB.CreateDatabase (kTestDBName_);
        Database::Document::Connection::Ptr p =
            MongoDBClient::Connection::New (MongoDBClient::Connection::Options{.fConnectionString = connectionString, .fDatabase = kTestDBName_});
        ComputerNetworksModel ([=] () {
            return MongoDBClient::Connection::New (MongoDBClient::Connection::Options{.fConnectionString = connectionString, .fDatabase = kTestDBName_});
        });
    }
    #endif

    {
#if qStroika_HasComponent_sqlite && 0
        auto connectionFactory = [=] () {
            // Use InMemory DB
            return SQLite::Connection::New (SQLite::Connection::Options{.fInMemoryDB = u"direct-employees-test"});
        };
        DirectEmployeesDB (connectionFactory);
#endif
    }

    {
#if qStroika_HasComponent_sqlite && 0
        auto dbPath = IO::FileSystem::WellKnownLocations::GetTemporary () / "direct-employees-test.db";
        (void)std::filesystem::remove (dbPath);
        auto connectionFactory = [=] () {
            // Same DirectEmployeesDB test, but write to a file so you can explore DB from command-line
            return SQLite::Connection::New (SQLite::Connection::Options{.fDBPath = dbPath});
        };
        DirectEmployeesDB (connectionFactory);
#endif
    }

#if qStroika_HasComponent_ODBC && 0
    {
        // Note - classes structured so you COULD use SQLite or ODBC transparently, but
        // the ODBC layer NYI (as of 2021-08-08) so commented out...
        // @todo change this sample so command-line arg grabs dsn from command-line
        auto connectionFactory = [=] () { return ODBC::Connection::New (ODBC::Connection::Options{"some-dsn"}); };
        // NYI - DirectEmployeesDB (connectionFactory);
    }
#endif

    {
#if qStroika_HasComponent_sqlite && 0
        auto dbPath = IO::FileSystem::WellKnownLocations::GetTemporary () / "threads-test.db";
        (void)std::filesystem::remove (dbPath);
        auto connectionFactory = [=] () {
            // default to 1 second fBusyTimeout for these tests
            auto conn = SQLite::Connection::New (SQLite::Connection::Options{
                .fDBPath = dbPath, .fThreadingMode = SQLite::Connection::Options::ThreadingMode::eMultiThread, .fBusyTimeout = 1s});
            Assert (Math::NearlyEquals (conn.busyTimeout ().As<double> (), 1.0));
            return conn;
        };
        ThreadTest (connectionFactory);
#endif
    }

    {
        // EmployeesDB test, but using C++ objects and ORM mapping layer (and threads)
#if qStroika_HasComponent_sqlite && 0
        auto dbPath = IO::FileSystem::WellKnownLocations::GetTemporary () / "orm-employees-test.db";
        (void)std::filesystem::remove (dbPath);
        auto connectionFactory = [=] () {
            // default to 1 second fBusyTimeout for these tests
            auto conn = SQLite::Connection::New (SQLite::Connection::Options{
                .fDBPath = dbPath, .fThreadingMode = SQLite::Connection::Options::ThreadingMode::eMultiThread, .fBusyTimeout = 1s});
            Assert (Math::NearlyEquals (conn.busyTimeout ().As<double> (), 1.0));
            return conn;
        };
        ORMEmployeesDB (connectionFactory);
#endif
    }

    {
#if qStroika_HasComponent_sqlite && 0
        auto dbPath = IO::FileSystem::WellKnownLocations::GetTemporary () / "computer-network.db";
        (void)std::filesystem::remove (dbPath);
        auto connectionFactory = [=] () { return SQLite::Connection::New (SQLite::Connection::Options{.fDBPath = dbPath}); };
        ComputerNetworksModel (connectionFactory);
#endif
    }

    return EXIT_SUCCESS;
}
