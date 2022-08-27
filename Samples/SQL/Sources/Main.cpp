/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdlib>

#include "Stroika/Foundation/Database/SQL/ODBC.h"
#include "Stroika/Foundation/Database/SQL/SQLite.h"
#include "Stroika/Foundation/IO/FileSystem/WellKnownLocations.h"

#include "ComputerNetwork.h"
#include "DirectEmployeesDB.h"
#include "ORMEmployeesDB.h"
#include "ThreadTest.h"

using namespace std;

using namespace Stroika::Foundation;

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    using namespace Stroika::Foundation::Database::SQL;

    using namespace Stroika::Samples::SQL;

    {
#if qHasFeature_sqlite
        auto connectionFactory = [=] () {
            // Use InMemory DB
            return SQLite::Connection::New (SQLite::Options{.fInMemoryDB = u"direct-employees-test"});
        };
        DirectEmployeesDB (connectionFactory);
#endif
    }

    {
#if qHasFeature_sqlite
        auto dbPath = IO::FileSystem::WellKnownLocations::GetTemporary () / "direct-employees-test.db";
        (void)std::filesystem::remove (dbPath);
        auto connectionFactory = [=] () {
            // Same DirectEmployeesDB test, but write to a file so you can explore DB from command-line
            return SQLite::Connection::New (SQLite::Options{.fDBPath = dbPath});
        };
        DirectEmployeesDB (connectionFactory);
#endif
    }

#if qHasLibrary_ODBC
    {
        // Note - classes structured so you COULD use SQLite or ODBC transparently, but
        // the ODBC layer NYI (as of 2021-08-08) so commented out...
        // @todo change this sample so command-line arg grabs dsn from command-line
        auto connectionFactory = [=] () {
            return ODBC::Connection::New (ODBC::Options{L"some-dsn"});
        };
        // NYI - DirectEmployeesDB (connectionFactory);
    }
#endif

    {
#if qHasFeature_sqlite
        auto dbPath = IO::FileSystem::WellKnownLocations::GetTemporary () / "threads-test.db";
        (void)std::filesystem::remove (dbPath);
        auto connectionFactory = [=] () {
            // default to 1 second fBusyTimeout for these tests
            auto conn = SQLite::Connection::New (SQLite::Options{.fDBPath = dbPath, .fThreadingMode = SQLite::Options::ThreadingMode::eMultiThread, .fBusyTimeout = 1s});
            Assert (Math::NearlyEquals (conn.pBusyTimeout ().As<double> (), 1.0));
            return conn;
        };
        ThreadTest (connectionFactory);
#endif
    }

    {
        // EmployeesDB test, but using C++ objects and ORM mapping layer (and threads)
#if qHasFeature_sqlite
        auto dbPath = IO::FileSystem::WellKnownLocations::GetTemporary () / "orm-employees-test.db";
        (void)std::filesystem::remove (dbPath);
        auto connectionFactory = [=] () {
            // default to 1 second fBusyTimeout for these tests
            auto conn = SQLite::Connection::New (SQLite::Options{.fDBPath = dbPath, .fThreadingMode = SQLite::Options::ThreadingMode::eMultiThread, .fBusyTimeout = 1s});
            Assert (Math::NearlyEquals (conn.pBusyTimeout ().As<double> (), 1.0));
            return conn;
        };
        ORMEmployeesDB (connectionFactory);
#endif
    }

    {
#if qHasFeature_sqlite
        auto dbPath = IO::FileSystem::WellKnownLocations::GetTemporary () / "computer-network.db";
        (void)std::filesystem::remove (dbPath);
        auto connectionFactory = [=] () {
            return SQLite::Connection::New (SQLite::Options{.fDBPath = dbPath});
        };
        ComputerNetworksModel (connectionFactory);
#endif
    }

    return EXIT_SUCCESS;
}
