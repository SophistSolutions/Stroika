/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdlib>

#if qHasFeature_sqlite
#include "Stroika/Foundation/Database/SQLite.h"
#endif

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Database;

namespace {
    void Test1_ ()
    {
#if qHasFeature_sqlite
        using namespace SQLite;
        auto initializeDB = [] (Connection& c) {
            c.Exec (
                L"CREATE TABLE COMPANY("
                L"ID INT PRIMARY KEY     NOT NULL,"
                L"NAME           TEXT    NOT NULL,"
                L"AGE            INT     NOT NULL,"
                L"ADDRESS        CHAR(50),"
                L"SALARY         REAL"
                L");");
            c.Exec (
                L"CREATE TABLE DEPARTMENT( ID INT PRIMARY KEY NOT NULL,"
                L"DEPT CHAR (50) NOT NULL,"
                L" EMP_ID INT NOT     NULL"
                L");");
        };
#if __cpp_designated_initializers
        Connection conn{Options{.fDBPath = filesystem::current_path () / "testdb.db"}, initializeDB};
#else
        Connection conn{Options{filesystem::current_path () / "testdb.db"}, initializeDB};
#endif
#endif
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Test1_ ();
    return EXIT_SUCCESS;
}
