/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdlib>
#include <random>
#include <tuple>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Database/SQL/Connection.h"
#include "Stroika/Foundation/Database/SQL/ORM/Versioning.h"
#include "Stroika/Foundation/Database/SQL/Statement.h"
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

using namespace Database::SQL;

namespace {
    void SetupDB_ (Connection::Ptr conn)
    {
        constexpr Configuration::Version kCurrentVersion_ = Configuration::Version{1, 0, Configuration::VersionStage::Alpha, 0};
        SQL::ORM::ProvisionForVersion (conn,
                                       kCurrentVersion_,
                                       initializer_list<SQL::ORM::TableProvisioner>{
                                           {L"EMPLOYEES"sv,
                                            [] (SQL::Connection::Ptr c, optional<Configuration::Version> v, [[maybe_unused]] Configuration::Version targetDBVersion) -> void {
                                                // for now no upgrade support
                                                if (not v) {
                                                    c.Exec (
                                                        L"CREATE TABLE EMPLOYEES("
                                                        L"ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                                                        L"NAME           TEXT    NOT NULL,"
                                                        L"AGE            INT     NOT NULL,"
                                                        L"ADDRESS        CHAR(50),"
                                                        L"SALARY         REAL,"
                                                        L"STILL_EMPLOYED INT"
                                                        L");");
                                                }
                                            }},
                                           {L"PAYCHECKS"sv,
                                            [] (SQL::Connection::Ptr c, optional<Configuration::Version> v, [[maybe_unused]] Configuration::Version targetDBVersion) -> void {
                                                // for now no upgrade support
                                                if (not v) {
                                                    c.Exec (
                                                        L"CREATE TABLE PAYCHECKS("
                                                        L"ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                                                        L"EMPLOYEEREF INT NOT NULL,"
                                                        L"AMOUNT REAL,"
                                                        L"DATE TEXT"
                                                        L");");
                                                }
                                            }},
                                       });
    }

    void PeriodicallyUpdateEmployeesTable_ (Connection::Ptr conn)
    {
        Statement addEmployeeStatement = conn.mkStatement (L"INSERT INTO EMPLOYEES (NAME,AGE,ADDRESS,SALARY,STILL_EMPLOYED) values (:NAME, :AGE, :ADDRESS, :SALARY, :STILL_EMPLOYED);");

        // Add Initial Employees
        addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {":NAME", L"Paul"},
            {":AGE", 32},
            {":ADDRESS", L"California"},
            {":SALARY", 20000.00},
            {":STILL_EMPLOYED", 1},
        });
        addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {":NAME", L"Allen"},
            {":AGE", 25},
            {":ADDRESS", L"Texas"},
            {":SALARY", 15000.00},
            {":STILL_EMPLOYED", 1},
        });
        addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {":NAME", L"Teddy"},
            {":AGE", 23},
            {":ADDRESS", L"Norway"},
            {":SALARY", 20000.00},
            {":STILL_EMPLOYED", 1},
        });
        addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {":NAME", L"Mark"},
            {":AGE", 25},
            {":ADDRESS", L"Rich-Mond"},
            {":SALARY", 65000.00},
            {":STILL_EMPLOYED", 1},
        });
        addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {":NAME", L"David"},
            {":AGE", 27},
            {":ADDRESS", L"Texas"},
            {":SALARY", 85000.00},
            {":STILL_EMPLOYED", 1},
        });
        addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {":NAME", L"Kim"},
            {":AGE", 22},
            {":ADDRESS", L"South-Hall"},
            {":SALARY", 45000.00},
            {":STILL_EMPLOYED", 1},
        });
        addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {":NAME", L"James"},
            {":AGE", 24},
            {":ADDRESS", L"Houston"},
            {":SALARY", 10000.00},
            {":STILL_EMPLOYED", 1},
        });

        default_random_engine         generator;
        uniform_int_distribution<int> distribution{1, 6};

        Statement getAllActiveEmployees = conn.mkStatement ("Select ID,NAME from EMPLOYEES where STILL_EMPLOYED=1;"sv);

        Statement fireEmployee = conn.mkStatement ("Update EMPLOYEES Set STILL_EMPLOYED=0 where ID=:ID;"sv);

        // then keep adding/removing people randomly (but dont really remove just mark no longer employed so we
        // can REF in paycheck table
        while (true) {
            static const Sequence<String>    kNames_{"Joe", "Phred", "Barny", "Sue", "Anne"};
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
                            {":NAME"sv, name},
                            {":AGE"sv, ageDistr (generator)},
                            {":ADDRESS"sv, kAddresses[addressesDistr (generator)]},
                            {":SALARY"sv, salaryDistr (generator)},
                            {":STILL_EMPLOYED"sv, 1},
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
        Statement addPaycheckStatement  = conn.mkStatement ("INSERT INTO PAYCHECKS (EMPLOYEEREF,AMOUNT,DATE) values (:EMPLOYEEREF, :AMOUNT, :DATE);"sv);
        Statement getAllActiveEmployees = conn.mkStatement ("Select ID,NAME,SALARY from EMPLOYEES where STILL_EMPLOYED=1;"sv);

        while (true) {
            try {
                for (const auto& employee : getAllActiveEmployees.GetAllRows (0, 1, 2)) {
                    int    id     = get<0> (employee).As<int> ();
                    String name   = get<1> (employee).As<String> ();
                    double salary = get<2> (employee).As<double> ();
                    DbgTrace (L"Writing paycheck for employee #%d (%s) amount %f", id, name.c_str (), salary);
                    addPaycheckStatement.Execute (initializer_list<Statement::ParameterDescription>{
                        {":EMPLOYEEREF"sv, id},
                        {":AMOUNT"sv, salary / 12},
                        {":DATE"sv, DateTime::Now ().Format (DateTime::kISO8601Format)},
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

void Stroika::Samples::SQL::ThreadTest (const function<Connection::Ptr ()>& connectionFactory)
{
    /*
     *  Create threads for each of our activities.
     *  When the waitable even times out, the threads will automatically be 'canceled' as they go out of scope.
     */
    SQL::Connection::Ptr conn1 = connectionFactory ();
    SQL::Connection::Ptr conn2 = connectionFactory ();
    SetupDB_ (conn1);
    Thread::CleanupPtr updateEmpDBThread{Thread::CleanupPtr::eAbortBeforeWaiting, Thread::New ([=] () { PeriodicallyUpdateEmployeesTable_ (conn1); }, Thread::eAutoStart, "Update Employee Table"sv)};
    Thread::CleanupPtr writeChecks{Thread::CleanupPtr::eAbortBeforeWaiting, Thread::New ([=] () { PeriodicallyWriteChecksForEmployeesTable_ (conn2); }, Thread::eAutoStart, "Write Checks"sv)};
    Execution::WaitableEvent{}.WaitQuietly (15s);
}
