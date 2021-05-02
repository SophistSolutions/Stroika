/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
//  TEST    Foundation::Database
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Database/SQLite.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/IO/FileSystem/FileSystem.h"
#include "Stroika/Foundation/IO/FileSystem/WellKnownLocations.h"
#include "Stroika/Foundation/Time/Duration.h"

#include "../TestHarness/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Database;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Time;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

#if qHasFeature_sqlite
namespace {
    namespace RegressionTest1_sqlite_ {
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
                    String insertSQL = [&] () {
                        StringBuilder sb;
                        sb += L"insert into Scans (StartAt, EndAt, ScanTypeIDRef, RawScanData, ScanLabel)";
                        sb += L"select ";
                        sb += L"'" + ScanStart.AsUTC ().Format (DateTime::kISO8601Format) + L"',";
                        sb += L"'" + ScanEnd.AsUTC ().Format (DateTime::kISO8601Format) + L"',";
                        sb += Characters::Format (L"%d", scanKind) + L",";
                        if (rawSpectrum) {
                            sb += L"'" + Database::SQLite::QuoteStringForDB (L"SomeLongASCIIStringS\r\r\n\t'omeLongASCIIStringSomeLongASCIIStringSomeLongASCIIString") + L"',";
                        }
                        else {
                            sb += L"NULL,";
                        }
                        if (ScanLabel) {
                            sb += L"'" + Database::SQLite::QuoteStringForDB (*ScanLabel) + L"'";
                        }
                        else {
                            sb += L"NULL";
                        }
                        sb += L";";
                        return sb.str ();
                    }();
                    fDB_.Exec (L"%s", insertSQL.c_str ());
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
                    Statement s{fDB_, L"select MAX(ScanId) from Scans where  ScanTypeIDRef='%d';", scanKind};
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
                        db.Exec (L"insert into ScanTypes (ScanTypeId, TypeName) select %d, 'Reference';", ScanKindType_::Reference);
                        db.Exec (L"insert into ScanTypes (ScanTypeId, TypeName) select %d, 'Sample';", ScanKindType_::Sample);
                        db.Exec (L"insert into ScanTypes (ScanTypeId, TypeName) select %d, 'Background';", ScanKindType_::Background);
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
}
#endif

namespace {
    void DoRegressionTests_ ()
    {
#if qHasFeature_sqlite
        RegressionTest1_sqlite_::DoIt ();
#endif
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
