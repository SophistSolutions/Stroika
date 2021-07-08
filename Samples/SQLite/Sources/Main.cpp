/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdlib>

#include "Stroika/Foundation/Database/SQL/SQLite.h"

#include "ComputerNetwork.h"
#include "DirectEmployeesDB.h"
#include "ORMEmployeesDB.h"
#include "ThreadTest.h"

using namespace std;

using namespace Stroika::Foundation;

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    using namespace Stroika::Samples::SQLite;

#if qHasFeature_sqlite
    {
        // Use InMemory DB
#if __cpp_designated_initializers
        DirectEmployeesDB (Options{.fInMemoryDB = u"direct-employees-test"});
#else
        DirectEmployeesDB (Options{nullopt, true, nullopt, u"direct-employees-test"});
#endif
    }
    {
        // Same DirectEmployeesDB test, but write to a file so you can explore DB from command-line
        auto dbPath = filesystem::current_path () / "direct-employees-test.db";
        (void)std::filesystem::remove (dbPath);
#if __cpp_designated_initializers
        DirectEmployeesDB (Options{.fDBPath = dbPath});
#else
        DirectEmployeesDB (Options{dbPath});
#endif
    }
    {
        auto dbPath = filesystem::current_path () / "threads-test.db";
        (void)std::filesystem::remove (dbPath);
#if __cpp_designated_initializers
        ThreadTest (Options{.fDBPath = dbPath, .fThreadingMode = Options::ThreadingMode::eMultiThread});
#else
        ThreadTest (Options{dbPath, true, nullopt, nullopt, Options::ThreadingMode::eMultiThread});
#endif
    }
    {
        // EmployeesDB test, but using C++ objects and ORM mapping layer (and threads)
        auto dbPath = filesystem::current_path () / "orm-employees-test.db";
        (void)std::filesystem::remove (dbPath);
#if __cpp_designated_initializers
        ORMEmployeesDB (Options{.fDBPath = dbPath});
#else
        ORMEmployeesDB (Options{dbPath});
#endif
    }
    {
        auto dbPath = filesystem::current_path () / "computer-network.db";
        (void)std::filesystem::remove (dbPath);
#if __cpp_designated_initializers
        ComputerNetworksModel (Options{.fDBPath = dbPath});
#else
        ComputerNetworksModel (Options{dbPath});
#endif
    }
#endif

    return EXIT_SUCCESS;
}
