/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>
#include <random>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/Database/Document/Connection.h"
#include "Stroika/Foundation/Database/Document/ObjectCollection.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Thread.h"

#include "EmployeesDB.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Common;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Database;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;

using namespace Database::Document;

namespace {

    /*
     * Sample data to write to the database
     */
    struct Employee {
        optional<IDType> ID{};
        String           fName;
        int              fAge{};
        String           fAddress;
        double           fSalary{};
        bool             fStillEmployed{};

        static const ObjectVariantMapper kMapper;
    };
    /*
     *  Define mapping to VariantValues (think JSON)
     */
    const ObjectVariantMapper Employee::kMapper{[] () {
        ObjectVariantMapper mapper;
        mapper.AddCommonType<optional<IDType>> ();
        mapper.AddClass<Employee> ({
            {"id"sv, &Employee::ID},
            {"Name"sv, &Employee::fName},
            {"Age"sv, &Employee::fAge},
            {"Address"sv, &Employee::fAddress},
            {"Salary"sv, &Employee::fSalary},
            {"Still-Employed"sv, &Employee::fStillEmployed},
        });
        return mapper;
    }()};

    /*
     * Sample data to write to the database
     */
    struct Paycheck {
        optional<IDType> ID{};
        IDType           fEmployeeRef;
        double           fAmount{};
        Time::Date       fDate;

        static const ObjectVariantMapper kMapper;
    };
    /*
     *  Define mapping to VariantValues (think JSON)
     */
    const ObjectVariantMapper Paycheck::kMapper{[] () {
        ObjectVariantMapper mapper;
        mapper.AddCommonType<optional<int>> ();
        mapper.AddClass<Paycheck> ({
            {"id"sv, &Paycheck::ID},
            {"Employee-Ref"sv, &Paycheck::fEmployeeRef},
            {"Amount"sv, &Paycheck::fAmount},
            {"Date"sv, &Paycheck::fDate},
        });
        return mapper;
    }()};

    /**
     *  Combine all the ObjectVariantMappers for the objects we use in this database into one, and
     *  AMEND any mappers as needed to accommodate possible changes in the mappings (like representing
     *  some things as strings vs. BLOBs etc).
     */
    const ObjectVariantMapper kDBObjectMapper_{[] () {
        ObjectVariantMapper mapper;
        mapper += Employee::kMapper;
        mapper += Paycheck::kMapper;
        return mapper;
    }()};

    /*
     * Example thread making updates to the employees table.
     */
    void PeriodicallyUpdateEmployeesTable_ (Connection::Ptr conn)
    {
        TraceContextBumper ctx{"{}::PeriodicallyUpdateEmployeesTable_"};

        ObjectCollection::Ptr<Employee> employeeCollection = ObjectCollection::New<Employee> (conn.CreateCollection ("Employees"), kDBObjectMapper_);

        // Add Initial Employees
        try {
            employeeCollection.Add (Employee{.fName = "Paul", .fAge = 32, .fAddress = "California", .fSalary = 20000.00, .fStillEmployed = true});
            employeeCollection.Add (Employee{.fName = "Allen", .fAge = 25, .fAddress = "Texas", .fSalary = 15000.00, .fStillEmployed = true});
            employeeCollection.Add (Employee{.fName = "Teddy", .fAge = 23, .fAddress = "Norway", .fSalary = 20000.00, .fStillEmployed = true});
            employeeCollection.Add (Employee{.fName = "Mark", .fAge = 25, .fAddress = "Rich-Mond", .fSalary = 65000.00, .fStillEmployed = true});
            employeeCollection.Add (Employee{.fName = "David", .fAge = 27, .fAddress = "Texas", .fSalary = 85000.00, .fStillEmployed = true});
            employeeCollection.Add (Employee{.fName = "Kim", .fAge = 22, .fAddress = "South-Hall", .fSalary = 45000.00, .fStillEmployed = true});
            employeeCollection.Add (Employee{.fName = "James", .fAge = 24, .fAddress = "Houston", .fSalary = 10000.00, .fStillEmployed = true});
        }
        catch (...) {
            cerr << "\tException adding initial employees to DB - this should generally not happen: {}"_f(current_exception ()) << endl;
        }

        default_random_engine         generator;
        uniform_int_distribution<int> distribution{1, 6};

        // then keep adding/removing people randomly (but dont really remove just mark no longer employed so we
        // can REF in paycheck table
        while (true) {
            static const Sequence<String>    kNames_{"Joe", "Phred", "Barny", "Sue", "Anne"};
            uniform_int_distribution<int>    namesDistr{0, static_cast<int> (kNames_.size () - 1)};
            uniform_int_distribution<int>    ageDistr{25, 50};
            static const Sequence<String>    kAddresses{"Houston", "Pittsburg", "New York", "Paris", "California"};
            uniform_int_distribution<int>    addressesDistr{0, static_cast<int> (kAddresses.size () - 1)};
            uniform_real_distribution<float> salaryDistr{10000.00, 50000.00};

            try {
                uniform_int_distribution<int> whatTodoDistr{0, 3};
                switch (whatTodoDistr (generator)) {
                    case 0:
                    case 1: {
                        String name = kNames_[namesDistr (generator)];
                        cout << "\tAdding employee {}"_f(name) << endl;
                        employeeCollection.Add (Employee{nullopt, name, ageDistr (generator), kAddresses[addressesDistr (generator)],
                                                         salaryDistr (generator), true});
                    } break;
                    case 2: {
                        // Look somebody up, and fire them
                        auto activeEmps = employeeCollection.GetAll ();
                        if (not activeEmps.empty ()) {
                            uniform_int_distribution<int> empDistr{0, static_cast<int> (activeEmps.size () - 1)};
                            Employee                      killMe = activeEmps[empDistr (generator)];
                            Assert (killMe.ID.has_value ());
                            cout << "\tFiring employee: {}, {}"_f(*killMe.ID, killMe.fName) << endl;
                            killMe.fStillEmployed = false;
                            employeeCollection.Replace (*killMe.ID, killMe);
                            // employeeTableConnection->Update (killMe);
                        }
                    } break;
                }
            }
            catch (...) {
                // no need to check for ThreadAbort exception, since Sleep is a cancelation point
                cerr << "\tException updating database: this should generally not happen: {}"_f(current_exception ()) << endl;
            }

            Sleep (1s); // **cancelation point**
        }
    }

    /*
     * Example thread making updates to the paychecks table (while consulting the employees table).
     */
    void PeriodicallyWriteChecksForEmployeesTable_ (Connection::Ptr conn)
    {
        TraceContextBumper ctx{"{}::PeriodicallyWriteChecksForEmployeesTable_"};
        ObjectCollection::Ptr<Employee> employeeCollection = ObjectCollection::New<Employee> (conn.CreateCollection ("Employees"), kDBObjectMapper_);
        ObjectCollection::Ptr<Paycheck> paycheckCollection = ObjectCollection::New<Paycheck> (conn.CreateCollection ("Paychecks"), kDBObjectMapper_);

        while (true) {
            try {
                for (const auto& employee : employeeCollection.GetAll ()) {
                    Assert (employee.ID != nullopt);
                    cout << "\tWriting paycheck for employee #{} ({}) amount {}"_f(*employee.ID, employee.fName, employee.fSalary) << endl;
                    paycheckCollection.Add (Paycheck{nullopt, *employee.ID, employee.fSalary / 12, DateTime::Now ().GetDate ()});
                }
            }
            catch (...) {
                // no need to check for ThreadAbort exception, since Sleep is a cancelation point
                cout << "\tException processing paychecks - this should generally not happen: {}"_f(current_exception ()) << endl;
            }
            Sleep (2s); // **cancelation point**
        }
    }
}

void Stroika::Samples::Document::EmployeesDB (const function<Connection::Ptr ()>& connectionFactory)
{
    TraceContextBumper ctx{"EmployeesDB"};

    Connection::Ptr conn1 = connectionFactory ();
    Connection::Ptr conn2 = connectionFactory ();

    /*
     *  Create threads for each of our activities.
     *  When the waitable even times out, the threads will automatically be 'canceled' as they go out of scope.
     */
    Thread::CleanupPtr updateEmpDBThread{Thread::CleanupPtr::eAbortBeforeWaiting,
                                         Thread::New ([=] () { PeriodicallyUpdateEmployeesTable_ (conn1); }, Thread::eAutoStart, "Update Employee Table"sv)};
    Thread::CleanupPtr writeChecks{Thread::CleanupPtr::eAbortBeforeWaiting,
                                   Thread::New ([=] () { PeriodicallyWriteChecksForEmployeesTable_ (conn2); }, Thread::eAutoStart, "Write Checks"sv)};
    Execution::WaitableEvent{}.WaitQuietly (15s);
}
