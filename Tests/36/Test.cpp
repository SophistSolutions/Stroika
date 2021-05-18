/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
//  TEST    Foundation::Database
#include "Stroika/Foundation/StroikaPreComp.h"

#include <random>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Database/SQLUtils.h"
#include "Stroika/Foundation/Database/SQLite.h"
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
using namespace Stroika::Foundation::Database;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Time;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

#if qHasFeature_sqlite
namespace {
    // Bad example (for now) without Bind - just formatting sql lines
    namespace RegressionTest1_sqlite_bad_without_bind_ {
        namespace PRIVATE_ {
            using namespace Database::SQLite;
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
                    bool created = false;
                    try {
#if __cpp_designated_initializers
                        fDB_ = Connection::New (Options{.fDBPath = testDBFile}, [&created] (Database::SQLite::Connection::Ptr db) { created = true; InitialSetup_ (db); });
#else
                        fDB_ = Connection::New (Options{testDBFile}, [&created] (Database::SQLite::Connection::Ptr db) { created = true; InitialSetup_ (db); });
#endif
                    }
                    catch (...) {
                        DbgTrace (L"Error %s experiment DB: %s: %s", created ? L"creating" : L"opening", Characters::ToString (testDBFile).c_str (), Characters::ToString (current_exception ()).c_str ());
                        Execution::ReThrow ();
                    }
                    if (created) {
                        DbgTrace (L"Initialized new experiment DB: %s", Characters::ToString (testDBFile).c_str ());
                    }
                    else {
                        DbgTrace (L"Opened experiment DB: %s", Characters::ToString (testDBFile).c_str ());
                    }
                }
                DB ()
                {
                    bool created = false;
                    try {
#if __cpp_designated_initializers
                        fDB_ = Connection::New (Options{.fInMemoryDB = L""}, [&created] (Database::SQLite::Connection::Ptr db) { created = true; InitialSetup_(db); });
#else
                        fDB_ = Connection::New (Options{nullopt, true, nullopt, L""}, [&created] (Database::SQLite::Connection::Ptr db) { created = true; InitialSetup_(db); });
#endif
                    }
                    catch (...) {
                        DbgTrace (L"Error %s experiment DB: %s: %s", created ? L"creating" : L"opening", L"MEMORY", Characters::ToString (current_exception ()).c_str ());
                        Execution::ReThrow ();
                    }
                    if (created) {
                        DbgTrace (L"Initialized new experiment DB: %s", L"MEMORY");
                    }
                    else {
                        DbgTrace (L"Opened experiment DB: %s", L"MEMORY");
                    }
                }
                DB (const DB&)                 = delete;
                nonvirtual DB&         operator= (const DB&) = delete;
                nonvirtual ScanIDType_ ScanPersistenceAdd (const DateTime& ScanStart, const DateTime& ScanEnd, const optional<String>& ScanLabel, ScanKindType_ scanKind, const optional<SpectrumType_>& rawSpectrum)
                {
                    // @todo write rawSpectrum isntead o fhardwired string...
                    constexpr bool kUseBind_ = true;
                    if (kUseBind_) {
                        Statement s{fDB_, L"insert into Scans (StartAt, EndAt, ScanTypeIDRef, RawScanData, ScanLabel) Values (:StartAt, :EndAt, :ScanTypeIDRef, :RawScanData, :ScanLabel);"};
                        s.Bind (L":StartAt", ScanStart.AsUTC ().Format (DateTime::kISO8601Format));
                        s.Bind (L":EndAt", ScanEnd.AsUTC ().Format (DateTime::kISO8601Format));
                        s.Bind (L":ScanTypeIDRef", (int)scanKind);
                        if (rawSpectrum) {
                            s.Bind (L":RawScanData", VariantValue{L"SomeLongASCIIStringS\r\r\n\t'omeLongASCIIStringSomeLongASCIIStringSomeLongASCIIString"});
                        }
                        if (ScanLabel) {
                            s.Bind (L":ScanLabel", VariantValue{*ScanLabel});
                        }
                        s.GetNextRow ();
                    }
                    else {
                        String insertSQL = [&] () {
                            StringBuilder sb;
                            sb += L"insert into Scans (StartAt, EndAt, ScanTypeIDRef, RawScanData, ScanLabel)";
                            sb += L"select ";
                            sb += L"'" + ScanStart.AsUTC ().Format (DateTime::kISO8601Format) + L"',";
                            sb += L"'" + ScanEnd.AsUTC ().Format (DateTime::kISO8601Format) + L"',";
                            sb += Characters::Format (L"%d", scanKind) + L",";
                            if (rawSpectrum) {
                                sb += L"'" + Database::SQLUtils::QuoteStringForDB (L"SomeLongASCIIStringS\r\r\n\t'omeLongASCIIStringSomeLongASCIIStringSomeLongASCIIString") + L"',";
                            }
                            else {
                                sb += L"NULL,";
                            }
                            if (ScanLabel) {
                                sb += L"'" + Database::SQLUtils::QuoteStringForDB (*ScanLabel) + L"'";
                            }
                            else {
                                sb += L"NULL";
                            }
                            sb += L";";
                            return sb.str ();
                        }();
                        fDB_.Exec (insertSQL);
                    }
                    Statement s{fDB_, L"SELECT MAX(ScanId) FROM Scans;"};
                    DbgTrace (L"Statement: %s", Characters::ToString (s).c_str ());
                    return s.GetNextRow ()->Lookup (L"MAX(ScanId)")->As<ScanIDType_> ();
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
                        return r->Lookup (L"MAX(ScanId)")->As<ScanIDType_> ();
                    }
                    return nullopt;
                }
                nonvirtual optional<ScanIDType_> GetLastScan_Bind_ (ScanKindType_ scanKind)
                {
                    Statement s{fDB_, L"select MAX(ScanId) from Scans where  ScanTypeIDRef=:ScanKind;"};
                    s.Bind (L":ScanKind", VariantValue{(int)scanKind});
                    DbgTrace (L"Statement: %s", Characters::ToString (s).c_str ());
                    if (optional<Statement::Row> r = s.GetNextRow ()) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace (L"ROW: %s", Characters::ToString (*r).c_str ());
#endif
                        return r->Lookup (L"MAX(ScanId)")->As<ScanIDType_> ();
                    }
                    return nullopt;
                }
                static void InitialSetup_ (Database::SQLite::Connection::Ptr db)
                {
                    TraceContextBumper ctx{"ScanDB_::DB::InitialSetup_"};
                    auto               tableSetup_ScanTypes = [&db] () {
                        db.Exec (L"create table 'ScanTypes' "
                                 L"("
                                 L"ScanTypeId tinyint Primary Key,"
                                 L"TypeName varchar(255) not null"
                                 L");");
                        db.Exec (Characters::Format (L"insert into ScanTypes (ScanTypeId, TypeName) select %d, 'Reference';", ScanKindType_::Reference));
                        db.Exec (Characters::Format (L"insert into ScanTypes (ScanTypeId, TypeName) select %d, 'Sample';", ScanKindType_::Sample));
                        db.Exec (Characters::Format (L"insert into ScanTypes (ScanTypeId, TypeName) select %d, 'Background';", ScanKindType_::Background));
                    };
                    auto tableSetup_Scans = [&db] () {
                        db.Exec (
                            L"create table 'Scans'"
                            L"("
                            L"ScanId integer Primary Key AUTOINCREMENT,"
                            L"StartAt Datetime not null,"
                            L"EndAt Datetime not null,"
                            L"ScanTypeIDRef tinyint not null,"
                            L"ScanLabel varchar,"
                            L"Foreign key (ScanTypeIDRef) References ScanTypes (ScanTypeId)"
                            L");");
                    };
                    auto tableSetup_ScanSets = [&db] () {
                        db.Exec (
                            L"Create table ScanSet"
                            L"("
                            L"ScanSetID bigint,"
                            L"ScanIDRef integer,"
                            L"Foreign key (ScanIdRef) References Scans(ScanId)"
                            L");");
                    };
                    auto tableSetup_AuxData = [&db] () {
                        db.Exec (
                            L"Create table AuxData"
                            L"("
                            L"ScanSetIDRef bigint Primary Key,"
                            L"Results varchar,"
                            L"Foreign key (ScanSetIDRef) References ScanSet(ScanSetID)"
                            L");");
                    };
                    auto tableSetup_ExtraForeignKeys = [&db] () {
                        db.Exec (L"Alter table Scans add column DependsOnScanSetIdRef bigint references ScanSet(ScanSetID);");
                        db.Exec (L"Alter table Scans add column RawScanData BLOB;");
                    };
                    tableSetup_ScanTypes ();
                    tableSetup_Scans ();
                    tableSetup_ScanSets ();
                    tableSetup_AuxData ();
                    tableSetup_ExtraForeignKeys ();
                }
                Database::SQLite::Connection::Ptr fDB_;
            };
        }
        void DoIt ()
        {
            //static const     DateTime   kScanStartTime4Reference_   = DateTime::Now ();
            static const DateTime kScanStartTime4Reference_ = DateTime{Date{Year{2020}, MonthOfYear::eApril, DayOfMonth::e1}, TimeOfDay{4, 0, 0}}; // hardwired data to be able to ccompare DBs
            using namespace PRIVATE_;
            TraceContextBumper ctx{"ScanDB::DB::RunTest"};
            auto               test = [] (PRIVATE_::DB& db, unsigned nTimesRanBefore) {
                db.fDB_->Exec (L"select * from ScanTypes;");
                {
                    Statement s{db.fDB_, L"select * from ScanTypes;"};
                    DbgTrace (L"Statement: %s", Characters::ToString (s).c_str ());
                    while (optional<Statement::Row> r = s.GetNextRow ()) {
                        DbgTrace (L"ROW: %s", Characters::ToString (*r).c_str ());
                    }
                }
                DbgTrace ("Latest Reference=%d", db.GetLastScan (ScanKindType_::Reference).value_or (static_cast<ScanIDType_> (-1)));
                SpectrumType_      spectrum;
                const unsigned int kNRecordsAddedPerTestCall = 100;
                for (int i = 0; i < kNRecordsAddedPerTestCall; ++i) {
                    DateTime    scanStartTime = kScanStartTime4Reference_ - 100ms;
                    DateTime    scanEndTime   = kScanStartTime4Reference_;
                    ScanIDType_ sid           = db.ScanPersistenceAdd (scanStartTime, scanEndTime, String{L"Hi Mom"}, ScanKindType_::Reference, spectrum);
                    Verify (sid == *db.GetLastScan (ScanKindType_::Reference));
                    Verify (sid == nTimesRanBefore * kNRecordsAddedPerTestCall + i + 1);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace ("ScanPersistenceAdd returned id=%d, and laserScan reported=%d", (int)sid, (int)db.GetLastScan (ScanKindType_::Reference).Value (-1));
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

    namespace RegressionTest2_sqlite_with_threads_ {

        using namespace Database::SQLite;

        namespace PRIVATE_ {

            Connection::Ptr SetupDB_ (const Options& options)
            {
                TraceContextBumper ctx{"RegressionTest2_sqlite_with_threads_::SetupDB_"};
                auto               initializeDB = [] (const Connection::Ptr& c) {
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
                auto r = Connection::New (o, initializeDB);
                VerifyTestResult (Math::NearlyEquals (r.pBusyTimeout ().As<double> (), 1.0));
                return r;
            }

            void PeriodicallyUpdateEmployeesTable_ (Connection::Ptr conn)
            {
                TraceContextBumper ctx{"RegressionTest2_sqlite_with_threads_::PeriodicallyUpdateEmployeesTable_"};

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
                TraceContextBumper ctx{"RegressionTest2_sqlite_with_threads_::PeriodicallyWriteChecksForEmployeesTable_"};
                Statement          addPaycheckStatement{conn, L"INSERT INTO PAYCHECKS (EMPLOYEEREF,AMOUNT,DATE) values (:EMPLOYEEREF, :AMOUNT, :DATE);"};
                Statement          getAllActiveEmployees{conn, L"Select ID,NAME,SALARY from EMPLOYEES where STILL_EMPLOYED=1;"};

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

            void ThreadTest_ (const Options& options)
            {
                TraceContextBumper ctx{"RegressionTest2_sqlite_with_threads_::ThreadTest_"};
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

        }

        void DoIt ()
        {
            TraceContextBumper ctx{"RegressionTest2_sqlite_with_threads_::DoIt"};
            using namespace Database::SQLite;
            auto dbPath = IO::FileSystem::WellKnownLocations::GetTemporary () / "threads-test.db";
            (void)std::filesystem::remove (dbPath);
#if __cpp_designated_initializers
            PRIVATE_::ThreadTest_ (Options{.fDBPath = dbPath});
#else
            PRIVATE_::ThreadTest_ (Options{dbPath});
#endif
        }
    }
}
#endif

namespace {
    void DoRegressionTests_ ()
    {
#if qHasFeature_sqlite
        RegressionTest1_sqlite_bad_without_bind_::DoIt ();
        RegressionTest2_sqlite_with_threads_::DoIt ();
#endif
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
