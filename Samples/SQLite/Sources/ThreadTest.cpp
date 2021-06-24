/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdlib>
#include <random>
#include <tuple>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Set.h"

#include "Stroika/Foundation/Database/SQL/SQLite.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Time/DateTime.h"

#include "ThreadTest.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Database;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Time;

#if qHasFeature_sqlite
using namespace Database::SQL::SQLite;

namespace {
    Connection::Ptr SetupDB_ (const Options& options)
    {
        auto initializeDB = [] (const Connection::Ptr& c) {
            // Use Connection::Ptr::Exec because no parameter bindings needed
            c.Exec (
                L"CREATE TABLE EMPLOYEES("
                L"ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                L"NAME           TEXT    NOT NULL,"
                L"AGE            INT     NOT NULL,"
                L"ADDRESS        CHAR(50),"
                L"SALARY         REAL,"
                L"STILL_EMPLOYED INT"
                L");");
            c.Exec (
                L"CREATE TABLE PAYCHECKS("
                L"ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                L"EMPLOYEEREF INT NOT NULL,"
                L"AMOUNT REAL,"
                L"DATE TEXT"
                L");");
        };
        Options o      = options;
        o.fBusyTimeout = o.fBusyTimeout.value_or (1s); // default to 1 second busy timeout for these tests
        return Connection::New (o, initializeDB);
    }

    void PeriodicallyUpdateEmployeesTable_ (Connection::Ptr conn)
    {
        Statement addEmployeeStatement{conn, L"INSERT INTO EMPLOYEES (NAME,AGE,ADDRESS,SALARY,STILL_EMPLOYED) values (:NAME, :AGE, :ADDRESS, :SALARY, :STILL_EMPLOYED);"};

        // Add Initial Employees
        addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {L":NAME", L"Paul"},
            {L":AGE", 32},
            {L":ADDRESS", L"California"},
            {L":SALARY", 20000.00},
            {L":STILL_EMPLOYED", 1},
        });
        addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {L":NAME", L"Allen"},
            {L":AGE", 25},
            {L":ADDRESS", L"Texas"},
            {L":SALARY", 15000.00},
            {L":STILL_EMPLOYED", 1},
        });
        addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {L":NAME", L"Teddy"},
            {L":AGE", 23},
            {L":ADDRESS", L"Norway"},
            {L":SALARY", 20000.00},
            {L":STILL_EMPLOYED", 1},
        });
        addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {L":NAME", L"Mark"},
            {L":AGE", 25},
            {L":ADDRESS", L"Rich-Mond"},
            {L":SALARY", 65000.00},
            {L":STILL_EMPLOYED", 1},
        });
        addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {L":NAME", L"David"},
            {L":AGE", 27},
            {L":ADDRESS", L"Texas"},
            {L":SALARY", 85000.00},
            {L":STILL_EMPLOYED", 1},
        });
        addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {L":NAME", L"Kim"},
            {L":AGE", 22},
            {L":ADDRESS", L"South-Hall"},
            {L":SALARY", 45000.00},
            {L":STILL_EMPLOYED", 1},
        });
        addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {L":NAME", L"James"},
            {L":AGE", 24},
            {L":ADDRESS", L"Houston"},
            {L":SALARY", 10000.00},
            {L":STILL_EMPLOYED", 1},
        });

        default_random_engine         generator;
        uniform_int_distribution<int> distribution{1, 6};

        Statement getAllActiveEmployees{conn, L"Select ID,NAME from EMPLOYEES where STILL_EMPLOYED=1;"};

        Statement fireEmployee{conn, L"Update EMPLOYEES Set STILL_EMPLOYED=0 where ID=:ID;"};

        // then keep adding/removing people randomly (but dont really remove just mark no longer employed so we
        // can REF in paycheck table
        while (true) {
            static const Sequence<String>    kNames_{L"Joe", L"Phred", L"Barny", L"Sue", L"Anne"};
            uniform_int_distribution<int>    namesDistr{0, static_cast<int> (kNames_.size () - 1)};
            uniform_int_distribution<int>    ageDistr{25, 50};
            static const Sequence<String>    kAddresses{L"Houston", L"Pittsburg", L"New York", L"Paris", L"California"};
            uniform_int_distribution<int>    addressesDistr{0, static_cast<int> (kAddresses.size () - 1)};
            uniform_real_distribution<float> salaryDistr{10000.00, 50000.00};

            try {
                uniform_int_distribution<int> whatTodoDistr{0, 3};
                switch (whatTodoDistr (generator)) {
                    case 0:
                    case 1: {
                        String name = kNames_[namesDistr (generator)];
                        DbgTrace (L"Adding employee %s", name.c_str ());
                        addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
                            {L":NAME", name},
                            {L":AGE", ageDistr (generator)},
                            {L":ADDRESS", kAddresses[addressesDistr (generator)]},
                            {L":SALARY", salaryDistr (generator)},
                            {L":STILL_EMPLOYED", 1},
                        });
                    } break;
                    case 2: {
                        // Look somebody up, and fire them
                        Sequence<tuple<VariantValue, VariantValue>> activeEmps = getAllActiveEmployees.GetAllRows (0, 1);
                        if (not activeEmps.empty ()) {
                            uniform_int_distribution<int>     empDistr{0, static_cast<int> (activeEmps.size () - 1)};
                            tuple<VariantValue, VariantValue> killMe = activeEmps[empDistr (generator)];
                            DbgTrace (L"Firing employee: %d, %s", get<0> (killMe).As<int> (), get<1> (killMe).As<String> ().c_str ());
                            fireEmployee.Execute (initializer_list<Statement::ParameterDescription>{
                                {L":ID", get<0> (killMe).As<int> ()}});
                        }
                    } break;
                }
            }
            catch (...) {
                // no need to check for ThreadAbort excepton, since Sleep is a cancelation point
                DbgTrace (L"Exception processing SQL - this should generally not happen: %s", Characters::ToString (current_exception ()).c_str ());
            }

            Sleep (1s); // **cancelation point**
        }
    }

    void PeriodicallyWriteChecksForEmployeesTable_ (Connection::Ptr conn)
    {
        Statement addPaycheckStatement{conn, L"INSERT INTO PAYCHECKS (EMPLOYEEREF,AMOUNT,DATE) values (:EMPLOYEEREF, :AMOUNT, :DATE);"};
        Statement getAllActiveEmployees{conn, L"Select ID,NAME,SALARY from EMPLOYEES where STILL_EMPLOYED=1;"};

        while (true) {
            try {
                for (auto employee : getAllActiveEmployees.GetAllRows (0, 1, 2)) {
                    int    id     = get<0> (employee).As<int> ();
                    String name   = get<1> (employee).As<String> ();
                    double salary = get<2> (employee).As<double> ();
                    DbgTrace (L"Writing paycheck for employee #%d (%s) amount %f", id, name.c_str (), salary);
                    addPaycheckStatement.Execute (initializer_list<Statement::ParameterDescription>{
                        {L":EMPLOYEEREF", id},
                        {L":AMOUNT", salary / 12},
                        {L":DATE", DateTime::Now ().Format (DateTime::kISO8601Format)},
                    });
                }
            }
            catch (...) {
                // no need to check for ThreadAbort excepton, since Sleep is a cancelation point
                DbgTrace (L"Exception processing SQL - this should generally not happen: %s", Characters::ToString (current_exception ()).c_str ());
            }
            Sleep (2s); // **cancelation point**
        }
    }
}

void Stroika::Samples::SQLite::ThreadTest (const Options& options)
{
    /*
     *  Create threads for each of our activities.
     *  When the waitable even times out, the threads will automatically be 'canceled' as they go out of scope.
     */
    Connection::Ptr    conn1 = SetupDB_ (options); // serialize construction of connections so no race creating/setting up DB
    Connection::Ptr    conn2 = SetupDB_ (options);
    Thread::CleanupPtr updateEmpDBThread{Thread::CleanupPtr::eAbortBeforeWaiting, Thread::New ([=] () { PeriodicallyUpdateEmployeesTable_ (conn1); }, Thread::eAutoStart, L"Update Employee Table")};
    Thread::CleanupPtr writeChecks{Thread::CleanupPtr::eAbortBeforeWaiting, Thread::New ([=] () { PeriodicallyWriteChecksForEmployeesTable_ (conn2); }, Thread::eAutoStart, L"Write Checks")};
    Execution::WaitableEvent{}.WaitQuietly (15s);
}
#endif
