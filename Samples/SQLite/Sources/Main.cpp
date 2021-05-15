/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdlib>

#include "Stroika/Foundation/Database/SQLite.h"

#include "SimpleEmployeesDB.h"

using namespace std;

using namespace Stroika::Foundation;


int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    using namespace Stroika::Samples::SQLite;

    {
#if __cpp_designated_initializers
        SimpleEmployeesDB (Options{.fInMemoryDB = u"simple-employees-test"});
#else
        SimpleEmployeesDB (Options{u"simple-employees-test"});
#endif
    }
    {
        auto dbPath = filesystem::current_path () / "simple-employees-test.db";
        (void)std::filesystem::remove (dbPath);
#if __cpp_designated_initializers
        SimpleEmployeesDB (Options{.fDBPath = dbPath});
#else
        SimpleEmployeesDB (Options{dbPath});
#endif
    }

    return EXIT_SUCCESS;
}
