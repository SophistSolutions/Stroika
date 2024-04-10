/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <random>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Database/SQL/ORM/Schema.h"
#include "Stroika/Foundation/Database/SQL/ORM/TableConnection.h"
#include "Stroika/Foundation/Database/SQL/ORM/Versioning.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/IO/FileSystem/WellKnownLocations.h"

#include "ORMEmployeesDB.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Common;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Database;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;

using namespace Database::SQL;
using namespace SQL::ORM;

namespace {

    struct Employee {
        optional<int> ID{};
        String        fName;
        int           fAge{};
        String        fAddress;
        double        fSalary{};
        bool          fStillEmployed{};

        static const ConstantProperty<ObjectVariantMapper> kMapper;
    };
    /*
     *  Define mapping to VariantValues (think JSON)
     */
    const ConstantProperty<ObjectVariantMapper> Employee::kMapper{[] () {
        ObjectVariantMapper mapper;
        mapper.AddCommonType<optional<int>> ();
        mapper.AddClass<Employee> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
            {"id"sv, StructFieldMetaInfo{&Employee::ID}},
            {"Name"sv, StructFieldMetaInfo{&Employee::fName}},
            {"Age"sv, StructFieldMetaInfo{&Employee::fAge}},
            {"Address"sv, StructFieldMetaInfo{&Employee::fAddress}},
            {"Salary"sv, StructFieldMetaInfo{&Employee::fSalary}},
            {"Still-Employed"sv, StructFieldMetaInfo{&Employee::fStillEmployed}},
        });
        return mapper;
    }};

    struct Paycheck {
        optional<int> ID{};
        int           fEmployeeRef;
        double        fAmount{};
        Time::Date    fDate;

        static const ConstantProperty<ObjectVariantMapper> kMapper;
    };
    /*
     *  Define mapping to VariantValues (think JSON)
     */
    const ConstantProperty<ObjectVariantMapper> Paycheck::kMapper{[] () {
        ObjectVariantMapper mapper;
        mapper.AddCommonType<optional<int>> ();
        mapper.AddClass<Paycheck> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
            {"id"sv, StructFieldMetaInfo{&Paycheck::ID}},
            {"Employee-Ref"sv, StructFieldMetaInfo{&Paycheck::fEmployeeRef}},
            {"Amount"sv, StructFieldMetaInfo{&Paycheck::fAmount}},
            {"Date"sv, StructFieldMetaInfo{&Paycheck::fDate}},
        });
        return mapper;
    }};

    /**
     *  Combine all the ObjectVariantMappers for the objects we use in this database into one, and
     *  AMEND any mappers as needed to accomodate possible changes in the mappings (like represeting
     *  some things as strings vs. BLOBs etc).
     */
    const ConstantProperty<ObjectVariantMapper> kDBObjectMapper_{[] () {
        ObjectVariantMapper mapper;
        mapper += Employee::kMapper;
        mapper += Paycheck::kMapper;
        return mapper;
    }};

    /*
     * Define the schema, and how to map between the VariantValue objects and the database
     * for the EMPLOYEES table.
     */
    const Schema::Table kEmployeesTableSchema_{
        "EMPLOYEES"sv,
        /*
         *  use the same names as the ObjectVariantMapper for simpler mapping, or specify an alternate name
         *  for ID, just as an example.
         */
        // clang-format off
        Collection<Schema::Field>{
            {.fName = "ID"sv, .fVariantValueName = "id"sv, .fRequired = true, .fVariantValueType = VariantValue::eInteger, .fIsKeyField = true, .fDefaultExpression = Schema::Field::kDefaultExpression_AutoIncrement}
            , {.fName = "NAME"sv, .fVariantValueName = "Name"sv, .fVariantValueType = VariantValue::eString}
            , {.fName = "AGE"sv, .fVariantValueName = "Age"sv, .fVariantValueType = VariantValue::eInteger}, {.fName = "ADDRESS"sv, .fVariantValueName = "Address"sv, .fVariantValueType = VariantValue::eString}
            , {.fName = "SALARY"sv, .fVariantValueName = "Salary"sv, .fVariantValueType = VariantValue::eFloat}, {.fName = "STILL_EMPLOYED"sv, .fVariantValueName = "Still-Employed"sv, .fVariantValueType = VariantValue::eInteger}},
        Schema::CatchAllField{}};
    // clang-format on

    /*
     * Define the schema, and how to map between the VariantValue objects and the database
     * for the PAYCHECKS table.
     */
    const Schema::Table kPaychecksTableSchema_{
        L"PAYCHECKS"sv,
        Collection<Schema::Field>{
            // clang-format off
            {.fName = "ID"sv, .fVariantValueName = "id"sv, .fRequired = true, .fVariantValueType = VariantValue::eInteger, .fIsKeyField = true, .fDefaultExpression = Schema::Field::kDefaultExpression_AutoIncrement}
            , {.fName = "EMPLOYEEREF"sv, .fVariantValueName = "Employee-Ref"sv, .fRequired = true, .fVariantValueType = VariantValue::eInteger}
            , {.fName = "AMOUNT"sv, .fVariantValueName = "Amount"sv, .fVariantValueType = VariantValue::eFloat}
            , {.fName = "DATE"sv, .fVariantValueName = "Date"sv, .fVariantValueType = VariantValue::eDate}}};
    // clang-format on

    /*
     * Example thread making updates to the employees table.
     */
    void PeriodicallyUpdateEmployeesTable_ (Connection::Ptr conn)
    {
        TraceContextBumper ctx{"{}::PeriodicallyUpdateEmployeesTable_"};

        auto employeeTableConnection = make_unique<SQL::ORM::TableConnection<Employee>> (conn, kEmployeesTableSchema_, kDBObjectMapper_);

        // Add Initial Employees
        employeeTableConnection->AddNew (Employee{.fName = "Paul", .fAge = 32, .fAddress = "California", .fSalary = 20000.00, .fStillEmployed = true});
        employeeTableConnection->AddNew (Employee{.fName = "Allen", .fAge = 25, .fAddress = "Texas", .fSalary = 15000.00, .fStillEmployed = true});
        employeeTableConnection->AddNew (Employee{.fName = "Teddy", .fAge = 23, .fAddress = "Norway", .fSalary = 20000.00, .fStillEmployed = true});
        employeeTableConnection->AddNew (Employee{.fName = "Mark", .fAge = 25, .fAddress = "Rich-Mond", .fSalary = 65000.00, .fStillEmployed = true});
        employeeTableConnection->AddNew (Employee{.fName = "David", .fAge = 27, .fAddress = "Texas", .fSalary = 85000.00, .fStillEmployed = true});
        employeeTableConnection->AddNew (Employee{.fName = "Kim", .fAge = 22, .fAddress = "South-Hall", .fSalary = 45000.00, .fStillEmployed = true});
        employeeTableConnection->AddNew (Employee{.fName = "James", .fAge = 24, .fAddress = "Houston", .fSalary = 10000.00, .fStillEmployed = true});

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
                        DbgTrace ("Adding employee {}"_f, name);
                        employeeTableConnection->AddNew (Employee{nullopt, name, ageDistr (generator),
                                                                  kAddresses[addressesDistr (generator)], salaryDistr (generator), true});
                    } break;
                    case 2: {
                        // Look somebody up, and fire them
                        auto activeEmps = employeeTableConnection->GetAll ();
                        if (not activeEmps.empty ()) {
                            uniform_int_distribution<int> empDistr{0, static_cast<int> (activeEmps.size () - 1)};
                            Employee                      killMe = activeEmps[empDistr (generator)];
                            Assert (killMe.ID.has_value ());
                            DbgTrace ("Firing employee: {}, {}"_f, *killMe.ID, killMe.fName);
                            killMe.fStillEmployed = false;
                            employeeTableConnection->Update (killMe);
                        }
                    } break;
                }
            }
            catch (...) {
                // no need to check for ThreadAbort exception, since Sleep is a cancelation point
                DbgTrace ("Exception processing SQL - this should generally not happen: {}"_f, current_exception ());
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
        auto employeeTableConnection = make_unique<SQL::ORM::TableConnection<Employee>> (conn, kEmployeesTableSchema_, kDBObjectMapper_);
        auto paycheckTableConnection = make_unique<SQL::ORM::TableConnection<Paycheck>> (conn, kPaychecksTableSchema_, kDBObjectMapper_);
        while (true) {
            try {
                for (const auto& employee : employeeTableConnection->GetAll ()) {
                    Assert (employee.ID != nullopt);
                    DbgTrace ("Writing paycheck for employee #{} ({}) amount {}"_f, *employee.ID, employee.fName, employee.fSalary);
                    paycheckTableConnection->AddNew (Paycheck{nullopt, *employee.ID, employee.fSalary / 12, DateTime::Now ().GetDate ()});
                }
            }
            catch (...) {
                // no need to check for ThreadAbort excepton, since Sleep is a cancelation point
                DbgTrace ("Exception processing SQL - this should generally not happen: {}"_f, current_exception ());
            }
            Sleep (2s); // **cancelation point**
        }
    }
}

void Stroika::Samples::SQL::ORMEmployeesDB (const std::function<Connection::Ptr ()>& connectionFactory)
{
    TraceContextBumper ctx{"ORMEmployeesDB"};

    Connection::Ptr conn1 = connectionFactory ();
    Connection::Ptr conn2 = connectionFactory ();

    // setup DB schema (on either connection) before running access threads
    constexpr Configuration::Version kCurrentVersion_ = Configuration::Version{1, 0, Configuration::VersionStage::Alpha, 0};
    ORM::ProvisionForVersion (conn1, kCurrentVersion_, Traversal::Iterable<ORM::Schema::Table>{kEmployeesTableSchema_, kPaychecksTableSchema_});

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
