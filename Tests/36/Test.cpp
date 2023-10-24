/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
//  TEST    Foundation::Database
#include "Stroika/Foundation/StroikaPreComp.h"

#include <random>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/Database/SQL/ORM/Schema.h"
#include "Stroika/Foundation/Database/SQL/ORM/TableConnection.h"
#include "Stroika/Foundation/Database/SQL/ORM/Versioning.h"
#include "Stroika/Foundation/Database/SQL/SQLite.h"
#include "Stroika/Foundation/Database/SQL/Utils.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/IO/FileSystem/FileSystem.h"
#include "Stroika/Foundation/IO/FileSystem/WellKnownLocations.h"
#include "Stroika/Foundation/Time/Duration.h"

#include "../TestHarness/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Common;
using namespace Stroika::Foundation::Database;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Time;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

#if qHasFeature_sqlite
namespace {
    // Bad example (for now) without Bind - just formatting sql lines
    namespace RegressionTest1_sqlite_ScansDBTest_ {
        namespace PRIVATE_ {
            using namespace Database::SQL::SQLite;
            enum class ScanKindType_ {
                Background,
                Reference,
                Sample,
                Stroika_Define_Enum_Bounds (Background, Sample)
            };
            using ScanIDType_                 = uint64_t;
            using SpectrumType_               = Mapping<double, double>;
            using PersistenceScanAuxDataType_ = Mapping<String, String>;
            struct DB {
            public:
                DB (const filesystem::path& testDBFile)
                {
                    try {
                        fDB_ = Connection::New (Connection::Options{.fDBPath = testDBFile});
                        InitialSetup_ (fDB_);
                    }
                    catch (...) {
                        DbgTrace (L"Error %s experiment DB: %s: %s", L"opening", Characters::ToString (testDBFile).c_str (),
                                  Characters::ToString (current_exception ()).c_str ());
                        Execution::ReThrow ();
                    }
                }
                DB ()
                {
                    try {
                        fDB_ = Connection::New (Connection::Options{.fInMemoryDB = ""sv});
                        InitialSetup_ (fDB_);
                    }
                    catch (...) {
                        DbgTrace (L"Error %s experiment DB: %s: %s", L"opening", L"MEMORY", Characters::ToString (current_exception ()).c_str ());
                        Execution::ReThrow ();
                    }
                }
                DB (const DB&)                       = delete;
                nonvirtual DB& operator= (const DB&) = delete;
                nonvirtual ScanIDType_ ScanPersistenceAdd (const DateTime& ScanStart, const DateTime& ScanEnd, const optional<String>& ScanLabel,
                                                           ScanKindType_ scanKind, const optional<SpectrumType_>& rawSpectrum)
                {
                    // @todo write rawSpectrum isntead o fhardwired string...
                    constexpr bool kUseBind_ = true;
                    if (kUseBind_) {
                        Statement s{fDB_, "insert into Scans (StartAt, EndAt, ScanTypeIDRef, RawScanData, ScanLabel) Values (:StartAt, "
                                          ":EndAt, :ScanTypeIDRef, :RawScanData, :ScanLabel);"};
                        s.Bind (":StartAt"sv, ScanStart.AsUTC ().Format (DateTime::kISO8601Format));
                        s.Bind (":EndAt"sv, ScanEnd.AsUTC ().Format (DateTime::kISO8601Format));
                        s.Bind (":ScanTypeIDRef"sv, (int)scanKind);
                        if (rawSpectrum) {
                            s.Bind (":RawScanData"sv,
                                    VariantValue{"SomeLongASCIIStringS\r\r\n\t'omeLongASCIIStringSomeLongASCIIStringSomeLongASCIIString"sv});
                        }
                        if (ScanLabel) {
                            s.Bind (":ScanLabel", VariantValue{*ScanLabel});
                        }
                        s.GetNextRow ();
                    }
                    else {
                        String insertSQL = [&] () {
                            StringBuilder sb;
                            sb += "insert into Scans (StartAt, EndAt, ScanTypeIDRef, RawScanData, ScanLabel)"sv;
                            sb += "select "sv;
                            sb += "'"sv + ScanStart.AsUTC ().Format (DateTime::kISO8601Format) + "',"sv;
                            sb += "'"sv + ScanEnd.AsUTC ().Format (DateTime::kISO8601Format) + "',"sv;
                            sb += Characters::Format (L"%d", scanKind) + ","sv;
                            if (rawSpectrum) {
                                sb += "'" + Database::SQL::Utils::QuoteStringForDB ("SomeLongASCIIStringS\r\r\n\t'omeLongASCIIStringSomeLongASCIIStringSomeLongASCIIString"sv) +
                                      "',"sv;
                            }
                            else {
                                sb += "NULL,"sv;
                            }
                            if (ScanLabel) {
                                sb += "'"sv + Database::SQL::Utils::QuoteStringForDB (*ScanLabel) + "'"sv;
                            }
                            else {
                                sb += "NULL"sv;
                            }
                            sb += ";";
                            return sb.str ();
                        }();
                        fDB_.Exec (insertSQL);
                    }
                    Statement s{fDB_, "SELECT MAX(ScanId) FROM Scans;"sv};
                    DbgTrace (L"Statement: %s", Characters::ToString (s).c_str ());
                    return s.GetNextRow ()->Lookup ("MAX(ScanId)"sv)->As<ScanIDType_> ();
                }
                nonvirtual optional<ScanIDType_> GetLastScan (ScanKindType_ scanKind)
                {
                    auto r1 = GetLastScan_Explicit_ (scanKind);
                    auto r2 = GetLastScan_Bind_ (scanKind);
                    VerifyTestResult (r1 == r2);
                    return r2;
                }
                nonvirtual optional<ScanIDType_> GetLastScan_Explicit_ (ScanKindType_ scanKind)
                {
                    Statement s{fDB_, Characters::Format (L"select MAX(ScanId) from Scans where  ScanTypeIDRef='%d';", scanKind)};
                    DbgTrace (L"Statement: %s", Characters::ToString (s).c_str ());
                    if (optional<Statement::Row> r = s.GetNextRow ()) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace (L"ROW: %s", Characters::ToString (*r).c_str ());
#endif
                        return r->Lookup ("MAX(ScanId)")->As<ScanIDType_> ();
                    }
                    return nullopt;
                }
                nonvirtual optional<ScanIDType_> GetLastScan_Bind_ (ScanKindType_ scanKind)
                {
                    Statement s{fDB_, "select MAX(ScanId) from Scans where  ScanTypeIDRef=:ScanKind;"};
                    s.Bind (":ScanKind", VariantValue{(int)scanKind});
                    DbgTrace (L"Statement: %s", Characters::ToString (s).c_str ());
                    if (optional<Statement::Row> r = s.GetNextRow ()) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace (L"ROW: %s", Characters::ToString (*r).c_str ());
#endif
                        return r->Lookup ("MAX(ScanId)")->As<ScanIDType_> ();
                    }
                    return nullopt;
                }
                static void InitialSetup_ (Database::SQL::SQLite::Connection::Ptr db)
                {
                    // @todo rewrite this using Bind()
                    TraceContextBumper               ctx{"ScanDB_::DB::InitialSetup_"};
                    bool                             created          = false;
                    constexpr Configuration::Version kCurrentVersion_ = Configuration::Version{1, 0, Configuration::VersionStage::Alpha, 0};
                    SQL::ORM::ProvisionForVersion (
                        db, kCurrentVersion_,
                        initializer_list<SQL::ORM::TableProvisioner>{
                            {"ScanTypes"sv,
                             [&created] (SQL::Connection::Ptr c, optional<Configuration::Version> v, [[maybe_unused]] Configuration::Version targetDBVersion) -> void {
                                 // for now no upgrade support
                                 if (not v) {
                                     created = true;
                                     c.Exec ("create table 'ScanTypes' "
                                             "("
                                             "ScanTypeId tinyint Primary Key,"
                                             "TypeName varchar(255) not null"
                                             ");");
                                     c.Exec (Characters::Format (L"insert into ScanTypes (ScanTypeId, TypeName) select %d, 'Reference';",
                                                                 ScanKindType_::Reference));
                                     c.Exec (Characters::Format (L"insert into ScanTypes (ScanTypeId, TypeName) select %d, 'Sample';",
                                                                 ScanKindType_::Sample));
                                     c.Exec (Characters::Format (L"insert into ScanTypes (ScanTypeId, TypeName) select %d, 'Background';",
                                                                 ScanKindType_::Background));
                                 }
                             }},
                            {"Scans"sv,
                             [&created] (SQL::Connection::Ptr c, optional<Configuration::Version> v, [[maybe_unused]] Configuration::Version targetDBVersion) -> void {
                                 // for now no upgrade support
                                 if (not v) {
                                     created = true;
                                     c.Exec ("create table 'Scans'"
                                             "("
                                             "ScanId integer Primary Key AUTOINCREMENT,"
                                             "StartAt Datetime not null,"
                                             "EndAt Datetime not null,"
                                             "ScanTypeIDRef tinyint not null,"
                                             "ScanLabel varchar,"
                                             "Foreign key (ScanTypeIDRef) References ScanTypes (ScanTypeId)"
                                             ");");
                                     c.Exec ("Alter table Scans add column RawScanData BLOB;");
                                 }
                             }},
                            {"ScanSet"sv,
                             [&created] (SQL::Connection::Ptr c, optional<Configuration::Version> v, [[maybe_unused]] Configuration::Version targetDBVersion) -> void {
                                 // for now no upgrade support
                                 if (not v) {
                                     created = true;
                                     c.Exec ("Create table ScanSet"
                                             "("
                                             "ScanSetID bigint,"
                                             "ScanIDRef integer,"
                                             "Foreign key (ScanIdRef) References Scans(ScanId)"
                                             ");");
                                     c.Exec ("Alter table Scans add column DependsOnScanSetIdRef bigint references ScanSet(ScanSetID);");
                                 }
                             }},
                            {"AuxData"sv,
                             [&created] (SQL::Connection::Ptr c, optional<Configuration::Version> v, [[maybe_unused]] Configuration::Version targetDBVersion) -> void {
                                 // for now no upgrade support
                                 if (not v) {
                                     created = true;
                                     c.Exec ("Create table AuxData"
                                             "("
                                             "ScanSetIDRef bigint Primary Key,"
                                             "Results varchar,"
                                             "Foreign key (ScanSetIDRef) References ScanSet(ScanSetID)"
                                             ");");
                                 }
                             }},
                        });
                    if (created) {
                        DbgTrace (L"Initialized new experiment DB: %s", Characters::ToString (db).c_str ());
                    }
                    else {
                        DbgTrace (L"Opened experiment DB: %s", Characters::ToString (db).c_str ());
                    }
                }
                Database::SQL::SQLite::Connection::Ptr fDB_;
            };
        }
        void DoIt ()
        {
            //static const     DateTime   kScanStartTime4Reference_   = DateTime::Now ();
            static const DateTime kScanStartTime4Reference_ = DateTime{Date{Year{2020}, April, day{1}}, TimeOfDay{4, 0, 0}}; // hardwired data to be able to ccompare DBs
            using namespace PRIVATE_;
            TraceContextBumper ctx{"ScanDB::DB::RunTest"};
            auto               test = [] (PRIVATE_::DB& db, unsigned nTimesRanBefore) {
                db.fDB_->Exec ("select * from ScanTypes;");
                {
                    Statement s{db.fDB_, "select * from ScanTypes;"};
                    DbgTrace (L"Statement: %s", Characters::ToString (s).c_str ());
                    while (optional<Statement::Row> r = s.GetNextRow ()) {
                        DbgTrace (L"ROW: %s", Characters::ToString (*r).c_str ());
                    }
                }
                DbgTrace ("Latest Reference=%d", db.GetLastScan (ScanKindType_::Reference).value_or (static_cast<ScanIDType_> (-1)));
                SpectrumType_      spectrum;
                const unsigned int kNRecordsAddedPerTestCall = 100;
                for (int i = 0; i < kNRecordsAddedPerTestCall; ++i) {
                    DateTime scanStartTime = kScanStartTime4Reference_ - 100ms;
                    DateTime scanEndTime   = kScanStartTime4Reference_;
                    ScanIDType_ sid = db.ScanPersistenceAdd (scanStartTime, scanEndTime, String{"Hi Mom"}, ScanKindType_::Reference, spectrum);
                    Verify (sid == *db.GetLastScan (ScanKindType_::Reference));
                    Verify (sid == nTimesRanBefore * kNRecordsAddedPerTestCall + i + 1);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace ("ScanPersistenceAdd returned id=%d, and laserScan reported=%d", (int)sid,
                              (int)db.GetLastScan (ScanKindType_::Reference).Value (-1));
#endif
                }
            };
            {
                // re-open the file several times and assure right number of records present
                filesystem::path dbFileName = IO::FileSystem::WellKnownLocations::GetTemporary () / "foo.db";
                (void)remove (dbFileName);
                for (unsigned int i = 0; i < 5; ++i) {
                    PRIVATE_::DB db{dbFileName};
                    test (db, i);
                }
            }
            {
                PRIVATE_::DB db{};
                test (db, 0);
            }
        }
    }

    namespace RegressionTest2_sqlite_EmployeesDB_with_threads_ {

        using namespace Database::SQL::SQLite;

        namespace PRIVATE_ {

            Connection::Ptr SetupDB_ (const Connection::Options& options)
            {
                TraceContextBumper  ctx{"RegressionTest2_sqlite_EmployeesDB_with_threads_::SetupDB_"};
                Connection::Options o = options;
                o.fBusyTimeout        = o.fBusyTimeout.value_or (1s); // default to 1 second busy timeout for these tests
                auto conn             = Connection::New (o);
                VerifyTestResult (Math::NearlyEquals (conn.pBusyTimeout ().As<double> (), 1.0));
                constexpr Configuration::Version kCurrentVersion_ = Configuration::Version{1, 0, Configuration::VersionStage::Alpha, 0};
                SQL::ORM::ProvisionForVersion (
                    conn, kCurrentVersion_,
                    initializer_list<SQL::ORM::TableProvisioner>{
                        {"EMPLOYEES"sv,
                         [] (SQL::Connection::Ptr c, optional<Configuration::Version> v, [[maybe_unused]] Configuration::Version targetDBVersion) -> void {
                             // for now no upgrade support
                             if (not v) {
                                 c.Exec ("CREATE TABLE EMPLOYEES("
                                         "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                                         "NAME           TEXT    NOT NULL,"
                                         "AGE            INT     NOT NULL,"
                                         "ADDRESS        CHAR(50),"
                                         "SALARY         REAL,"
                                         "STILL_EMPLOYED INT"
                                         ");");
                             }
                         }},
                        {"PAYCHECKS"sv,
                         [] (SQL::Connection::Ptr c, optional<Configuration::Version> v, [[maybe_unused]] Configuration::Version targetDBVersion) -> void {
                             // for now no upgrade support
                             if (not v) {
                                 c.Exec ("CREATE TABLE PAYCHECKS("
                                         "ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                                         "EMPLOYEEREF INT NOT NULL,"
                                         "AMOUNT REAL,"
                                         "DATE TEXT"
                                         ");");
                             }
                         }},
                    });
                return conn;
            }

            void PeriodicallyUpdateEmployeesTable_ (Connection::Ptr conn)
            {
                TraceContextBumper ctx{"RegressionTest2_sqlite_EmployeesDB_with_threads_::PeriodicallyUpdateEmployeesTable_"};

                Statement addEmployeeStatement{conn, "INSERT INTO EMPLOYEES (NAME,AGE,ADDRESS,SALARY,STILL_EMPLOYED) values (:NAME, :AGE, "
                                                     ":ADDRESS, :SALARY, :STILL_EMPLOYED);"};

                // Add Initial Employees
                addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
                    {":NAME", "Paul"},
                    {":AGE", 32},
                    {":ADDRESS", "California"},
                    {":SALARY", 20000.00},
                    {":STILL_EMPLOYED", 1},
                });
                addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
                    {":NAME", "Allen"},
                    {":AGE", 25},
                    {":ADDRESS", "Texas"},
                    {":SALARY", 15000.00},
                    {":STILL_EMPLOYED", 1},
                });
                addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
                    {":NAME", "Teddy"},
                    {":AGE", 23},
                    {":ADDRESS", "Norway"},
                    {":SALARY", 20000.00},
                    {":STILL_EMPLOYED", 1},
                });
                addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
                    {":NAME", "Mark"},
                    {":AGE", 25},
                    {":ADDRESS", "Rich-Mond"},
                    {":SALARY", 65000.00},
                    {":STILL_EMPLOYED", 1},
                });
                addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
                    {":NAME", "David"},
                    {":AGE", 27},
                    {":ADDRESS", "Texas"},
                    {":SALARY", 85000.00},
                    {":STILL_EMPLOYED", 1},
                });
                addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
                    {":NAME", "Kim"},
                    {":AGE", 22},
                    {":ADDRESS", "South-Hall"},
                    {":SALARY", 45000.00},
                    {":STILL_EMPLOYED", 1},
                });
                addEmployeeStatement.Execute (initializer_list<Statement::ParameterDescription>{
                    {":NAME", "James"},
                    {":AGE", 24},
                    {":ADDRESS"sv, "Houston"},
                    {":SALARY"sv, 10000.00},
                    {":STILL_EMPLOYED"sv, 1},
                });

                default_random_engine         generator;
                uniform_int_distribution<int> distribution{1, 6};

                Statement getAllActiveEmployees{conn, "Select ID,NAME from EMPLOYEES where STILL_EMPLOYED=1;"};

                Statement fireEmployee{conn, "Update EMPLOYEES Set STILL_EMPLOYED=0 where ID=:ID;"};

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
                                    fireEmployee.Execute (initializer_list<Statement::ParameterDescription>{{":ID", get<0> (killMe).As<int> ()}});
                                }
                            } break;
                        }
                    }
                    catch (...) {
                        // no need to check for ThreadAbort excepton, since Sleep is a cancelation point
                        DbgTrace (L"Exception processing SQL - this should generally not happen: %s",
                                  Characters::ToString (current_exception ()).c_str ());
                    }

                    Sleep (1s); // **cancelation point**
                }
            }

            void PeriodicallyWriteChecksForEmployeesTable_ (Connection::Ptr conn)
            {
                TraceContextBumper ctx{"RegressionTest2_sqlite_EmployeesDB_with_threads_::PeriodicallyWriteChecksForEmployeesTable_"};
                Statement          addPaycheckStatement{conn,
                                               "INSERT INTO PAYCHECKS (EMPLOYEEREF,AMOUNT,DATE) values (:EMPLOYEEREF, :AMOUNT, :DATE);"};
                Statement          getAllActiveEmployees{conn, "Select ID,NAME,SALARY from EMPLOYEES where STILL_EMPLOYED=1;"};

                while (true) {
                    try {
                        for (auto employee : getAllActiveEmployees.GetAllRows (0, 1, 2)) {
                            int    id     = get<0> (employee).As<int> ();
                            String name   = get<1> (employee).As<String> ();
                            double salary = get<2> (employee).As<double> ();
                            DbgTrace (L"Writing paycheck for employee #%d (%s) amount %f", id, name.c_str (), salary);
                            addPaycheckStatement.Execute (initializer_list<Statement::ParameterDescription>{
                                {":EMPLOYEEREF", id},
                                {":AMOUNT", salary / 12},
                                {":DATE", DateTime::Now ().Format (DateTime::kISO8601Format)},
                            });
                        }
                    }
                    catch (...) {
                        // no need to check for ThreadAbort excepton, since Sleep is a cancelation point
                        DbgTrace (L"Exception processing SQL - this should generally not happen: %s",
                                  Characters::ToString (current_exception ()).c_str ());
                    }
                    Sleep (2s); // **cancelation point**
                }
            }

            void ThreadTest_ (const Connection::Options& options)
            {
                TraceContextBumper ctx{"RegressionTest2_sqlite_EmployeesDB_with_threads_::ThreadTest_"};
                /*
                 *  Create threads for each of our activities.
                 *  When the waitable even times out, the threads will automatically be 'canceled' as they go out of scope.
                 */
                Connection::Ptr    conn1 = SetupDB_ (options); // serialize construction of connections so no race creating/setting up DB
                Connection::Ptr    conn2 = SetupDB_ (options);
                Thread::CleanupPtr updateEmpDBThread{
                    Thread::CleanupPtr::eAbortBeforeWaiting,
                    Thread::New ([=] () { PeriodicallyUpdateEmployeesTable_ (conn1); }, Thread::eAutoStart, "Update Employee Table")};
                Thread::CleanupPtr writeChecks{
                    Thread::CleanupPtr::eAbortBeforeWaiting,
                    Thread::New ([=] () { PeriodicallyWriteChecksForEmployeesTable_ (conn2); }, Thread::eAutoStart, "Write Checks")};
                Execution::WaitableEvent{}.WaitQuietly (15s);
            }

        }

        void DoIt ()
        {
            TraceContextBumper ctx{"RegressionTest2_sqlite_EmployeesDB_with_threads_::DoIt"};
            using namespace Database::SQL::SQLite;
            auto dbPath = IO::FileSystem::WellKnownLocations::GetTemporary () / "threads-test.db";
            (void)std::filesystem::remove (dbPath);
            PRIVATE_::ThreadTest_ (Connection::Options{.fDBPath = dbPath, .fThreadingMode = Connection::Options::ThreadingMode::eMultiThread});
        }
    }

    namespace RegressionTest3_sqlite_EmployeesDB_with_ORM_and_threads_ {

        using namespace SQL::ORM;
        using namespace SQL::SQLite;

        namespace PRIVATE_ {

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
                , {.fName = "AGE"sv, .fVariantValueName = "Age"sv, .fVariantValueType = VariantValue::eInteger}
                , {.fName = "ADDRESS"sv, .fVariantValueName = "Address"sv, .fVariantValueType = VariantValue::eString}
                , {.fName = "SALARY"sv, .fVariantValueName = "Salary"sv, .fVariantValueType = VariantValue::eFloat}
                , {.fName = "STILL_EMPLOYED"sv, .fVariantValueName = "Still-Employed"sv, .fVariantValueType = VariantValue::eInteger}
                },
                Schema::CatchAllField{}};

            /*
             * Define the schema, and how to map between the VariantValue objects and the database
             * for the PAYCHECKS table.
             */
            const Schema::Table kPaychecksTableSchema_{
                "PAYCHECKS"sv,
                Collection<Schema::Field>{
                {.fName = "ID"sv, .fVariantValueName = "id"sv, .fRequired = true, .fVariantValueType = VariantValue::eInteger, .fIsKeyField = true, .fDefaultExpression = Schema::Field::kDefaultExpression_AutoIncrement}
                , {.fName = "EMPLOYEEREF"sv, .fVariantValueName = "Employee-Ref"sv, .fRequired = true,  .fVariantValueType = VariantValue::eInteger}
                , {.fName = "AMOUNT"sv, .fVariantValueName = "Amount"sv, .fVariantValueType = VariantValue::eFloat}
                , {.fName = "DATE"sv, .fVariantValueName = "Date"sv, .fVariantValueType = VariantValue::eDate}
             }};
            // clang-format on

            /*
             * Create database connection, with hook to establish the database schema,
             * (and soon to provide database schema upgrades as needed)
             */
            Connection::Ptr SetupDB_ (const Connection::Options& options)
            {
                TraceContextBumper  ctx{"RegressionTest3_sqlite_EmployeesDB_with_ORM_and_threads_::SetupDB_"};
                Connection::Options o = options;
                o.fBusyTimeout        = o.fBusyTimeout.value_or (1s); // default to 1 second busy timeout for these tests
                auto conn             = Connection::New (o);
                VerifyTestResult (Math::NearlyEquals (conn.pBusyTimeout ().As<double> (), 1.0));
                constexpr Configuration::Version kCurrentVersion_ = Configuration::Version{1, 0, Configuration::VersionStage::Alpha, 0};
                SQL::ORM::ProvisionForVersion (conn, kCurrentVersion_,
                                               Traversal::Iterable<SQL::ORM::Schema::Table>{kEmployeesTableSchema_, kPaychecksTableSchema_});
                return conn;
            }

            /*
             * Example thread making updates to the employees table.
             */
            void PeriodicallyUpdateEmployeesTable_ (Connection::Ptr conn)
            {
                TraceContextBumper ctx{"RegressionTest3_sqlite_EmployeesDB_with_ORM_and_threads_::PeriodicallyUpdateEmployeesTable_"};

                auto employeeTableConnection = make_unique<SQL::ORM::TableConnection<Employee>> (conn, kEmployeesTableSchema_, kDBObjectMapper_);

                // Add Initial Employees
                // @todo use __cpp_designated_initializers when we can assume it
                employeeTableConnection->AddNew (Employee{nullopt, "Paul", 32, "California", 20000.00, true});
                employeeTableConnection->AddNew (Employee{nullopt, "Allen", 25, "Texas", 15000.00, true});
                employeeTableConnection->AddNew (Employee{nullopt, "Teddy", 23, "Norway", 20000.00, true});
                employeeTableConnection->AddNew (Employee{nullopt, "Mark", 25, "Rich-Mond", 65000.00, true});
                employeeTableConnection->AddNew (Employee{nullopt, "David", 27, "Texas", 85000.00, true});
                employeeTableConnection->AddNew (Employee{nullopt, "Kim", 22, "South-Hall", 45000.00, true});
                employeeTableConnection->AddNew (Employee{nullopt, "James", 24, "Houston", 10000.00, true});

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
                                DbgTrace (L"Adding employee %s", name.c_str ());
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
                                    DbgTrace (L"Firing employee: %d, %s", *killMe.ID, killMe.fName.c_str ());
                                    killMe.fStillEmployed = false;
                                    employeeTableConnection->Update (killMe);
                                }
                            } break;
                        }
                    }
                    catch (...) {
                        // no need to check for ThreadAbort excepton, since Sleep is a cancelation point
                        DbgTrace (L"Exception processing SQL - this should generally not happen: %s",
                                  Characters::ToString (current_exception ()).c_str ());
                    }

                    Sleep (1s); // **cancelation point**
                }
            }

            /*
             * Example thread making updates to the paychecks table (while consulting the employees table).
             */
            void PeriodicallyWriteChecksForEmployeesTable_ (Connection::Ptr conn)
            {
                TraceContextBumper ctx{
                    "RegressionTest3_sqlite_EmployeesDB_with_ORM_and_threads_::PeriodicallyWriteChecksForEmployeesTable_"};
                auto employeeTableConnection = make_unique<SQL::ORM::TableConnection<Employee>> (conn, kEmployeesTableSchema_, kDBObjectMapper_);
                auto paycheckTableConnection = make_unique<SQL::ORM::TableConnection<Paycheck>> (conn, kPaychecksTableSchema_, kDBObjectMapper_);
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
                        DbgTrace (L"Exception processing SQL - this should generally not happen: %s",
                                  Characters::ToString (current_exception ()).c_str ());
                    }
                    Sleep (2s); // **cancelation point**
                }
            }

            void ThreadTest_ (const Connection::Options& options)
            {
                TraceContextBumper ctx{"RegressionTest3_sqlite_EmployeesDB_with_ORM_and_threads_::ThreadTest_"};
                /*
                 *  Create threads for each of our activities.
                 *  When the waitable even times out, the threads will automatically be 'canceled' as they go out of scope.
                 */
                Connection::Ptr    conn1 = SetupDB_ (options); // serialize construction of connections so no race creating/setting up DB
                Connection::Ptr    conn2 = SetupDB_ (options);
                Thread::CleanupPtr updateEmpDBThread{
                    Thread::CleanupPtr::eAbortBeforeWaiting,
                    Thread::New ([=] () { PeriodicallyUpdateEmployeesTable_ (conn1); }, Thread::eAutoStart, "Update Employee Table")};
                Thread::CleanupPtr writeChecks{
                    Thread::CleanupPtr::eAbortBeforeWaiting,
                    Thread::New ([=] () { PeriodicallyWriteChecksForEmployeesTable_ (conn2); }, Thread::eAutoStart, "Write Checks")};
                Execution::WaitableEvent{}.WaitQuietly (15s);
            }
        }

        void DoIt ()
        {
            TraceContextBumper ctx{"RegressionTest3_sqlite_EmployeesDB_with_ORM_and_threads_::DoIt"};
            using namespace Database::SQL::SQLite;
            auto dbPath = IO::FileSystem::WellKnownLocations::GetTemporary () / "threads-and-orm-test.db";
            (void)std::filesystem::remove (dbPath);
            PRIVATE_::ThreadTest_ (Connection::Options{.fDBPath = dbPath, .fThreadingMode = Connection::Options::ThreadingMode::eMultiThread});
        }
    }
}
#endif

namespace {
    void DoRegressionTests_ ()
    {
#if qHasFeature_sqlite
        RegressionTest1_sqlite_ScansDBTest_::DoIt ();
        RegressionTest2_sqlite_EmployeesDB_with_threads_::DoIt ();
        RegressionTest3_sqlite_EmployeesDB_with_ORM_and_threads_::DoIt ();
#endif
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
