/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdlib>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Set.h"

#include "Stroika/Foundation/Database/SQL/SQLite.h"

#include "SimpleEmployeesDB.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Database;

#if qHasFeature_sqlite
using namespace Database::SQL::SQLite;

void Stroika::Samples::SQLite::SimpleEmployeesDB (const Options& options)
{
    /*
     ***** SETUP SCHEMA ****
     */
    // Example roughly from https://www.tutorialspoint.com/sqlite/sqlite_insert_query.htm
    auto initializeDB = [] (const Connection::Ptr& c) {
        // Use Connection::Ptr::Exec because no parameter bindings needed
        c.Exec (
            L"CREATE TABLE DEPARTMENT(ID INT PRIMARY KEY NOT NULL,"
            L"NAME CHAR (50) NOT NULL"
            L");");
        c.Exec (
            L"CREATE TABLE EMPLOYEES("
            L"ID INT PRIMARY KEY     NOT NULL," // See example ThreadTest for simple example using AUTOINCREMENT instead of explicit IDs
            L"NAME           TEXT    NOT NULL,"
            L"AGE            INT     NOT NULL,"
            L"ADDRESS        CHAR(50),"
            L"SALARY         REAL"
            L");");
    };
    /*
     ***** CONNECT TO DATABASE ****
     */
    Connection::Ptr conn = Connection::New (options, initializeDB);

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
    /*
     ***** INSERT ROWS ****
     */
    // Use Statement with named parameters (to avoid sql injection and to add clarity)
    Statement addDepartment{conn, L"INSERT INTO DEPARTMENT (ID, NAME) values (:ID, :NAME);"};
    addDepartment.Execute (initializer_list<Statement::ParameterDescription>{
        {L":ID", 1},
        {L":NAME", L"Washing machines"},
    });
    Statement addEmployeeStatement{conn, L"INSERT INTO EMPLOYEES (ID,NAME,AGE,ADDRESS,SALARY) values (:ID, :NAME, :AGE, :ADDRESS, :SALARY);"};
    addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
        {L":ID", 1},
        {L":NAME", L"Paul"},
        {L":AGE", 32},
        {L":ADDRESS", L"California"},
        {L":SALARY", 20000.00},
    });
    addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
        {L":ID", 2},
        {L":NAME", L"Allen"},
        {L":AGE", 25},
        {L":ADDRESS", L"Texas"},
        {L":SALARY", 15000.00},
    });
    addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
        {L":ID", 3},
        {L":NAME", L"Teddy"},
        {L":AGE", 23},
        {L":ADDRESS", L"Norway"},
        {L":SALARY", 20000.00},
    });
    addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
        {L":ID", 4},
        {L":NAME", L"Mark"},
        {L":AGE", 25},
        {L":ADDRESS", L"Rich-Mond"},
        {L":SALARY", 65000.00},
    });
    addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
        {L":ID", 5},
        {L":NAME", L"David"},
        {L":AGE", 27},
        {L":ADDRESS", L"Texas"},
        {L":SALARY", 85000.00},
    });
    addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
        {L":ID", 6},
        {L":NAME", L"Kim"},
        {L":AGE", 22},
        {L":ADDRESS", L"South-Hall"},
        {L":SALARY", 45000.00},
    });
    addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
        {L":ID", 7},
        {L":NAME", L"James"},
        {L":AGE", 24},
        {L":ADDRESS", L"Houston"},
        {L":SALARY", 10000.00},
    });
    /*
     ***** INSERT ROWS (ERROR CHECKING) ****
     */
    // This call will generate a REQUIRE assertion error - terminating your program. Don't violate assertions!
    if (false) {
        addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {L":BAD-ARGUMENT", 7}, // almost as good as static type checking to make sure paramter names match call
            {L":NAME", L"James"},
            {L":AGE", 24},
            {L":ADDRESS", L"Houston"},
            {L":SALARY", 10000.00},
        });
        AssertNotReached ();
    }

    try {
        addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {L":ID", 7},
            {L":NAME", L"James"},
            {L":AGE", 24},
            {L":ADDRESS", L"Houston"},
            {L":SALARY", 10000.00},
        });
        AssertNotReached (); // RE-USED ID!!! - only detectable at runtime - so exception thrown
    }
    catch (...) {
        DbgTrace (L"Note good error message: %s", Characters::ToString (current_exception ()).c_str ()); // silently ignore this here...
    }

    /*
     ***** SIMPLE QUERIES ****
     */
    Statement   getAllNames{conn, L"Select NAME from EMPLOYEES;"};
    Set<String> allNames = getAllNames.GetAllRows (0).Select<String> ([] (VariantValue v) { return v.As<String> (); }).As<Set<String>> ();
    Assert ((allNames == Set<String>{L"Paul", L"Allen", L"Kim", L"David", L"Mark", L"James", L"Teddy"}));

    // Either rollup using SQL, or using C++ functional (Iterable) wrappers.
    Statement               sumAllSalarys{conn, L"select SUM(SALARY) from EMPLOYEES;"};
    [[maybe_unused]] double sumSalaryUsingSQL = sumAllSalarys.GetAllRows (0)[0].As<double> ();
    Statement               getAllSalarys{conn, L"select SALARY from EMPLOYEES;"};
    [[maybe_unused]] double sumSalaryUsingIterableApply = getAllSalarys.GetAllRows (0).Select<double> ([] (VariantValue v) { return v.As<double> (); }).SumValue ();
    Assert (Math::NearlyEquals (sumSalaryUsingSQL, sumSalaryUsingIterableApply));
}
#endif
