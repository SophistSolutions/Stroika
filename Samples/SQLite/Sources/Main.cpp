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
        // Example roughly from https://www.tutorialspoint.com/sqlite/sqlite_insert_query.htm
        auto initializeDB = [] (const Connection::Ptr& c) {
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
        auto dbPath = filesystem::current_path () / "testdb.db";
        (void)std::filesystem::remove (dbPath);
#if __cpp_designated_initializers
        Connection::Ptr conn = Connection::New (Options{.fDBPath = dbPath}, initializeDB);
#else
        Connection::Ptr conn = Connection::New (Options{dbPath}, initializeDB);
#endif
        /*
            ID          NAME        AGE         ADDRESS     SALARY
            ----------  ----------  ----------  ----------  ----------
            1           Paul        32          California  20000.0
            2           Allen       25          Texas       15000.0
            3           Teddy       23          Norway      20000.0
            4           Mark        25          Rich-Mond   65000.0
            5           David       27          Texas       85000.0
            6           Kim         22          South-Hall  45000.0
            7           James       24          Houston     10000.0
        */
        Statement addCompanyStatement{conn, L"INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) values (:ID, :NAME, :AGE, :ADDRESS, :SALARY);"};
        addCompanyStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {L":ID", 1},
            {L":NAME", L"Paul"},
            {L":AGE", 32},
            {L":ADDRESS", L"California"},
            {L":SALARY", 20000.00},
        });
        addCompanyStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {L":ID", 2},
            {L":NAME", L"Allen"},
            {L":AGE", 25},
            {L":ADDRESS", L"Texas"},
            {L":SALARY", 15000.00},
        });
        addCompanyStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {L":ID", 3},
            {L":NAME", L"Teddy"},
            {L":AGE", 23},
            {L":ADDRESS", L"Norway"},
            {L":SALARY", 20000.00},
        });
        addCompanyStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {L":ID", 4},
            {L":NAME", L"Mark"},
            {L":AGE", 25},
            {L":ADDRESS", L"Rich-Mond"},
            {L":SALARY", 65000.00},
        });
        addCompanyStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {L":ID", 5},
            {L":NAME", L"David"},
            {L":AGE", 27},
            {L":ADDRESS", L"Texas"},
            {L":SALARY", 85000.00},
        });
        addCompanyStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {L":ID", 6},
            {L":NAME", L"Kim"},
            {L":AGE", 22},
            {L":ADDRESS", L"South-Hall"},
            {L":SALARY", 45000.00},
        });
        addCompanyStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {L":ID", 7},
            {L":NAME", L"James"},
            {L":AGE", 24},
            {L":ADDRESS", L"Houston"},
            {L":SALARY", 10000.00},
        });
        Statement        getAllNames{conn, L"Select NAME from COMPANY;"};
        Sequence<String> allNames = getAllNames.GetAllRows (0).Select<String> ([] (VariantValue v) { return v.As<String> (); }).As<Sequence<String>> ();
        Assert (allNames.length () == 7 and allNames[6] == L"James");
#endif
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Test1_ ();
    return EXIT_SUCCESS;
}
