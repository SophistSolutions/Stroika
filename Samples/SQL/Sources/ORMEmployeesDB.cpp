/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <random>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Database/SQL/ORM/Schema.h"
#include "Stroika/Foundation/Database/SQL/ORM/TableConnection.h"
#include "Stroika/Foundation/Database/SQL/ORM/Versioning.h"
#include "Stroika/Foundation/Database/SQL/SQLite.h"
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

#if qHasFeature_sqlite
using namespace Database::SQL::SQLite;
using namespace SQL::ORM;
using namespace SQL::SQLite;

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
    const ConstantProperty<ObjectVariantMapper> Employee::kMapper{[] () {
        ObjectVariantMapper mapper;
        mapper.AddCommonType<optional<int>> ();
        mapper.AddClass<Employee> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
            {L"id", StructFieldMetaInfo{&Employee::ID}},
            {L"Name", StructFieldMetaInfo{&Employee::fName}},
            {L"Age", StructFieldMetaInfo{&Employee::fAge}},
            {L"Address", StructFieldMetaInfo{&Employee::fAddress}},
            {L"Salary", StructFieldMetaInfo{&Employee::fSalary}},
            {L"Still-Employed", StructFieldMetaInfo{&Employee::fStillEmployed}},
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
    const ConstantProperty<ObjectVariantMapper> Paycheck::kMapper{[] () {
        ObjectVariantMapper mapper;
        mapper.AddCommonType<optional<int>> ();
        mapper.AddClass<Paycheck> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
            {L"id", StructFieldMetaInfo{&Paycheck::ID}},
            {L"Employee-Ref", StructFieldMetaInfo{&Paycheck::fEmployeeRef}},
            {L"Amount", StructFieldMetaInfo{&Paycheck::fAmount}},
            {L"Date", StructFieldMetaInfo{&Paycheck::fDate}},
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
        L"EMPLOYEES",
        /*
         *  use the same names as the ObjectVariantMapper for simpler mapping, or specify an alternate name
         *  for ID, just as an example.
         */
        // clang-format off
        Collection<Schema::Field>{
#if __cpp_designated_initializers
        {.fName = L"ID", .fVariantValueFieldName = L"id"sv, .fVariantType = VariantValue::eInteger, .fIsKeyField = true, .fAutoIncrement = true}
        , {.fName = L"NAME", .fVariantValueFieldName = L"Name"sv, .fVariantType = VariantValue::eString}
        , {.fName = L"AGE", .fVariantValueFieldName = L"Age"sv, .fVariantType = VariantValue::eInteger}
        , {.fName = L"ADDRESS", .fVariantValueFieldName = L"Address"sv, .fVariantType = VariantValue::eString}
        , {.fName = L"SALARY", .fVariantValueFieldName = L"Salary"sv, .fVariantType = VariantValue::eFloat}
        , {.fName = L"STILL_EMPLOYED", .fVariantValueFieldName = L"Still-Employed"sv, .fVariantType = VariantValue::eInteger}
#else
        {L"ID", L"id"sv, false, VariantValue::eInteger, nullopt, true, nullopt, nullopt, false, true}
        , {L"name", L"Name"sv, false, VariantValue::eString}
        , {L"AGE", L"Age"sv, false, VariantValue::eInteger}
        , {L"ADDRESS", L"Address"sv, false, VariantValue::eString}
        , {L"SALARY", L"Salary"sv, false, VariantValue::eFloat}
        , {L"STILL_EMPLOYED", L"Still-Employed"sv, false, VariantValue::eInteger}
#endif
        },
        Schema::CatchAllField{}};

    /*
     * Define the schema, and how to map between the VariantValue objects and the database
     * for the PAYCHECKS table.
     */
    const Schema::Table kPaychecksTableSchema_{
        L"PAYCHECKS",
        Collection<Schema::Field>{
#if __cpp_designated_initializers
        {.fName = L"ID", .fVariantValueFieldName = L"id"sv, .fVariantType = VariantValue::eInteger, .fIsKeyField = true, .fAutoIncrement = true}
        , {.fName = L"EMPLOYEEREF", .fVariantValueFieldName = L"Employee-Ref"sv, .fVariantType = VariantValue::eInteger, .fNotNull = true}
        , {.fName = L"AMOUNT", .fVariantValueFieldName = L"Amount"sv, .fVariantType = VariantValue::eFloat}
        , {.fName = L"DATE", .fVariantValueFieldName = L"Date"sv, .fVariantType = VariantValue::eDate}
        #else
        {L"ID", L"id"sv, false, VariantValue::eInteger, nullopt, true, nullopt, nullopt, false, true}
        , {L"EMPLOYEEREF", L"Employee-Ref"sv, false, VariantValue::eInteger, nullopt, false, nullopt, nullopt, true}
        , {L"AMOUNT", L"Amount"sv, false, VariantValue::eFloat}
        , {L"DATE", L"Date"sv, false, VariantValue::eDate}
#endif
        }};
    // clang-format on

    /*
     * Create database connection, with hook to establish the database schema,
     * (and soon to provide database schema upgrades as needed)
     */
    Connection::Ptr SetupDB_ (const Options& options)
    {
        TraceContextBumper ctx{"{}::SetupDB_"};
        Options o      = options;
        o.fBusyTimeout = o.fBusyTimeout.value_or (1s); // default to 1 second busy timeout for these tests
        auto r         = Connection::New (o);
        Assert (Math::NearlyEquals (r.pBusyTimeout ().As<double> (), 1.0));

        constexpr Configuration::Version kCurrentVersion_ = Configuration::Version{1, 0, Configuration::VersionStage::Alpha, 0};
        SQL::ORM::ProvisionForVersion (r,
                                       kCurrentVersion_,
                                       Traversal::Iterable<SQL::ORM::Schema::Table>{kEmployeesTableSchema_, kPaychecksTableSchema_});
        return r;
    }

    /*
     * Example thread making updates to the employees table.
     */
    void PeriodicallyUpdateEmployeesTable_ (Connection::Ptr conn)
    {
        TraceContextBumper ctx{"{}::PeriodicallyUpdateEmployeesTable_"};

        auto employeeTableConnection = make_unique<SQL::ORM::TableConnection<Employee>> (conn, kEmployeesTableSchema_, kDBObjectMapper_);

        // Add Initial Employees
        // @todo use __cpp_designated_initializers when we can assume it
        employeeTableConnection->AddNew (Employee{nullopt, L"Paul", 32, L"California", 20000.00, true});
        employeeTableConnection->AddNew (Employee{nullopt, L"Allen", 25, L"Texas", 15000.00, true});
        employeeTableConnection->AddNew (Employee{nullopt, L"Teddy", 23, L"Norway", 20000.00, true});
        employeeTableConnection->AddNew (Employee{nullopt, L"Mark", 25, L"Rich-Mond", 65000.00, true});
        employeeTableConnection->AddNew (Employee{nullopt, L"David", 27, L"Texas", 85000.00, true});
        employeeTableConnection->AddNew (Employee{nullopt, L"Kim", 22, L"South-Hall", 45000.00, true});
        employeeTableConnection->AddNew (Employee{nullopt, L"James", 24, L"Houston", 10000.00, true});

        default_random_engine         generator;
        uniform_int_distribution<int> distribution{1, 6};

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
                        employeeTableConnection->AddNew (Employee{nullopt, name, ageDistr (generator), kAddresses[addressesDistr (generator)], salaryDistr (generator), true});
                    } break;
                    case 2: {
                        // Look somebody up, and fire them
                        auto activeEmps = employeeTableConnection->GetAll ();
                        if (not activeEmps.empty ()) {
                            uniform_int_distribution<int> empDistr{0, static_cast<int> (activeEmps.size () - 1)};
                            Employee                      killMe = activeEmps[empDistr (generator)];
                            Assert (killMe.ID.has_value ());
                            DbgTrace (L"Firing employee: %d, %s", *killMe.ID, killMe.fName.c_str ());
                            killMe.fStillEmployed = false;
                            employeeTableConnection->Update (killMe);
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

    /*
     * Example thread making updates to the paychecks table (while consulting the employees table).
     */
    void PeriodicallyWriteChecksForEmployeesTable_ (Connection::Ptr conn)
    {
        TraceContextBumper ctx{"{}::PeriodicallyWriteChecksForEmployeesTable_"};
        auto               employeeTableConnection = make_unique<SQL::ORM::TableConnection<Employee>> (conn, kEmployeesTableSchema_, kDBObjectMapper_);
        auto               paycheckTableConnection = make_unique<SQL::ORM::TableConnection<Paycheck>> (conn, kPaychecksTableSchema_, kDBObjectMapper_);
        while (true) {
            try {
                for (auto employee : employeeTableConnection->GetAll ()) {
                    Assert (employee.ID != nullopt);
                    DbgTrace (L"Writing paycheck for employee #%d (%s) amount %f", *employee.ID, employee.fName.c_str (), employee.fSalary);
                    paycheckTableConnection->AddNew (Paycheck{nullopt, *employee.ID, employee.fSalary / 12, DateTime::Now ().GetDate ()});
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

void Stroika::Samples::SQLite::ORMEmployeesDB (const Options& options)
{
    TraceContextBumper ctx{"ORMEmployeesDB"};
    using namespace Database::SQL::SQLite;
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
