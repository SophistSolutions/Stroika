/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdlib>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Database/SQL/Connection.h"
#include "Stroika/Foundation/Database/SQL/ORM/Versioning.h"
#include "Stroika/Foundation/Database/SQL/Statement.h"

#include "DirectEmployeesDB.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Database;
using namespace Stroika::Foundation::Database::SQL;

void Stroika::Samples::SQL::DirectEmployeesDB (const std::function<Connection::Ptr ()>& connectionFactory)
{
    /*
     ***** CONNECT TO DATABASE ****
     */
    Connection::Ptr conn = connectionFactory ();

    /*
     ***** SETUP SCHEMA ****
     */
    // Example schema roughly from https://www.tutorialspoint.com/sqlite/sqlite_insert_query.htm
    constexpr Configuration::Version kCurrentVersion_ = Configuration::Version{1, 0, Configuration::VersionStage::Alpha, 0};
    ORM::ProvisionForVersion (
        conn, kCurrentVersion_,
        initializer_list<ORM::TableProvisioner>{
            {"DEPARTMENT"sv,
             [] (SQL::Connection::Ptr c, optional<Configuration::Version> v, [[maybe_unused]] Configuration::Version targetDBVersion) -> void {
                 // for now no upgrade support
                 if (not v) {
                     c.Exec ("CREATE TABLE DEPARTMENT(ID INT PRIMARY KEY NOT NULL,"
                             "NAME CHAR (50) NOT NULL"
                             ");"sv);
                 }
             }},
            {"EMPLOYEES"sv,
             [] (Connection::Ptr c, optional<Configuration::Version> v, [[maybe_unused]] Configuration::Version targetDBVersion) -> void {
                 // for now no upgrade support
                 if (not v) {
                     c.Exec ("CREATE TABLE EMPLOYEES("
                             "ID INT PRIMARY KEY     NOT NULL," // See example ThreadTest for simple example using AUTOINCREMENT instead of explicit IDs
                             "NAME           TEXT    NOT NULL,"
                             "AGE            INT     NOT NULL,"
                             "ADDRESS        CHAR(50),"
                             "SALARY         REAL"
                             ");"sv);
                 }
             }},
        });

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
    Statement addDepartment = conn.mkStatement ("INSERT INTO DEPARTMENT (ID, NAME) values (:ID, :NAME);"sv);
    addDepartment.Execute (initializer_list<Statement::ParameterDescription>{
        {":ID"sv, 1},
        {":NAME"sv, "Washing machines"sv},
    });
    Statement addEmployeeStatement =
        conn.mkStatement ("INSERT INTO EMPLOYEES (ID,NAME,AGE,ADDRESS,SALARY) values (:ID, :NAME, :AGE, :ADDRESS, :SALARY);"sv);
    addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
        {":ID"sv, 1},
        {":NAME"sv, "Paul"sv},
        {":AGE"sv, 32},
        {":ADDRESS"sv, "California"sv},
        {":SALARY"sv, 20000.00},
    });
    addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
        {":ID"sv, 2},
        {":NAME"sv, "Allen"sv},
        {":AGE"sv, 25},
        {":ADDRESS"sv, "Texas"sv},
        {":SALARY"sv, 15000.00},
    });
    addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
        {":ID", 3},
        {":NAME", "Teddy"sv},
        {":AGE", 23},
        {":ADDRESS", "Norway"sv},
        {":SALARY", 20000.00},
    });
    addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
        {":ID", 4},
        {":NAME", "Mark"sv},
        {":AGE", 25},
        {":ADDRESS", "Rich-Mond"sv},
        {":SALARY", 65000.00},
    });
    addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
        {":ID", 5},
        {":NAME", "David"sv},
        {":AGE", 27},
        {":ADDRESS", "Texas"sv},
        {":SALARY", 85000.00},
    });
    addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
        {":ID", 6},
        {":NAME", "Kim"sv},
        {":AGE", 22},
        {":ADDRESS", "South-Hall"sv},
        {":SALARY", 45000.00},
    });
    addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
        {":ID", 7},
        {":NAME", "James"sv},
        {":AGE", 24},
        {":ADDRESS", "Houston"sv},
        {":SALARY", 10000.00},
    });
    /*
     ***** INSERT ROWS (ERROR CHECKING) ****
     */
    // This call will generate a REQUIRE assertion error - terminating your program. Don't violate assertions!
    if (false) {
        addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {":BAD-ARGUMENT", 7}, // almost as good as static type checking to make sure paramter names match call
            {":NAME", "James"sv},
            {":AGE", 24},
            {":ADDRESS", "Houston"sv},
            {":SALARY", 10000.00},
        });
        AssertNotReached ();
    }

    try {
        addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {":ID", 7},
            {":NAME", "James"sv},
            {":AGE", 24},
            {":ADDRESS", "Houston"sv},
            {":SALARY", 10000.00},
        });
        AssertNotReached (); // RE-USED ID!!! - only detectable at runtime - so exception thrown
    }
    catch (...) {
        DbgTrace (L"Note good error message: %s", Characters::ToString (current_exception ()).c_str ()); // silently ignore this here...
    }

    /*
     ***** SIMPLE QUERIES ****
     */
    Statement   getAllNames = conn.mkStatement ("Select NAME from EMPLOYEES;"sv);
    Set<String> allNames    = getAllNames.GetAllRows (0).Map<String> ([] (VariantValue v) { return v.As<String> (); }).As<Set<String>> ();
    Assert ((allNames == Set<String>{"Paul", "Allen", "Kim", "David", "Mark", "James", "Teddy"}));

    // Either rollup using SQL, or using C++ functional (Iterable) wrappers.
    Statement               sumAllSalarys     = conn.mkStatement ("select SUM(SALARY) from EMPLOYEES;"sv);
    [[maybe_unused]] double sumSalaryUsingSQL = sumAllSalarys.GetAllRows (0)[0].As<double> ();
    Statement               getAllSalarys     = conn.mkStatement ("select SALARY from EMPLOYEES;"sv);
    [[maybe_unused]] double sumSalaryUsingIterableApply =
        getAllSalarys.GetAllRows (0).Map<double> ([] (VariantValue v) { return v.As<double> (); }).SumValue ();
    Assert (Math::NearlyEquals (sumSalaryUsingSQL, sumSalaryUsingIterableApply));
}
