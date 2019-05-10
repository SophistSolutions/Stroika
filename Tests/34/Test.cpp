/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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
            using Statement = Database::SQLite::Connection::Statement;
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
                DB (const String& testDBFile)
                {
                    bool created = false;
                    try {
                        fDB_ = make_unique<Database::SQLite::Connection> (testDBFile, [&created] (Database::SQLite::Connection& db) { created = true; InitialSetup_ (db); });
                    }
                    catch (...) {
                        DbgTrace (L"Error %s experiment DB: %s: %s", created ? L"creating" : L"opening", testDBFile.c_str (), Characters::ToString (current_exception ()).c_str ());
                        Execution::ReThrow ();
                    }
                    if (created) {
                        DbgTrace (L"Initialized new experiment DB: %s", testDBFile.c_str ());
                    }
                    else {
                        DbgTrace (L"Opened experiment DB: %s", testDBFile.c_str ());
                    }
                }
                DB (Database::SQLite::Connection::InMemoryDBFlag)
                {
                    bool created = false;
                    try {
                        fDB_ = make_unique<Database::SQLite::Connection> (Database::SQLite::Connection::eInMemoryDB, [&created] (Database::SQLite::Connection& db) { created = true; InitialSetup_ (db); });
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
                DB (const DB&)         = delete;
                nonvirtual DB& operator= (const DB&) = delete;
                nonvirtual ScanIDType_ ScanPersistenceAdd (const DateTime& ScanStart, const DateTime& ScanEnd, const optional<String>& ScanLabel, ScanKindType_ scanKind, const optional<SpectrumType_>& rawSpectrum)
                {
                    String insertSQL = [&] () {
                        StringBuilder sb;
                        sb += L"insert into Scans (StartAt, EndAt, ScanTypeIDRef, RawScanData, ScanLabel)";
                        sb += L"select ";
                        sb += L"'" + ScanStart.AsUTC ().Format (DateTime::PrintFormat::eISO8601) + L"',";
                        sb += L"'" + ScanEnd.AsUTC ().Format (DateTime::PrintFormat::eISO8601) + L"',";
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
                    fDB_->Exec (L"%s", insertSQL.c_str ());
                    Statement s{fDB_.get (), L"SELECT MAX(ScanId) FROM Scans;"};

                    if (optional<Statement::RowType> r = s.GetNextRow ()) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace (L"ROW: %s", Characters::ToString (*r).c_str ());
#endif
                        return r->Lookup (L"MAX(ScanId)")->As<ScanIDType_> ();
                    }
                    AssertNotReached ();
                    return ScanIDType_{};
                }
                nonvirtual optional<ScanIDType_> GetLastScan (ScanKindType_ scanKind)
                {
                    Statement s{fDB_.get (), L"select MAX(ScanId) from Scans where  ScanTypeIDRef='%d';", scanKind};
                    if (optional<Statement::RowType> r = s.GetNextRow ()) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace (L"ROW: %s", Characters::ToString (*r).c_str ());
#endif
                        return r->Lookup (L"MAX(ScanId)")->As<ScanIDType_> ();
                    }
                    return optional<ScanIDType_>{};
                }
                static void InitialSetup_ (Database::SQLite::Connection& db)
                {
                    TraceContextBumper ctx ("ScanDB_::DB::InitialSetup_");
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
                        db.Exec (L"Alter table Scans add column DependsOnScanSetIdRef bigint references  ScanSet(ScanSetID);");
                        db.Exec (L"Alter table Scans add column RawScanData BLOB;");
                    };
                    tableSetup_ScanTypes ();
                    tableSetup_Scans ();
                    tableSetup_ScanSets ();
                    tableSetup_AuxData ();
                    tableSetup_ExtraForeignKeys ();
                }
                unique_ptr<Database::SQLite::Connection> fDB_;
            };
        }
        void DoIt ()
        {
            using namespace PRIVATE_;
            TraceContextBumper ctx ("ScanDB::DB::RunTest");
            auto               test = [] (PRIVATE_::DB& db, unsigned nTimesRanBefore) {
                db.fDB_->Exec (L"select * from ScanTypes;");
                {
                    Statement s{db.fDB_.get (), L"select * from ScanTypes;"};
                    while (optional<Statement::RowType> r = s.GetNextRow ()) {
                        DbgTrace (L"ROW: %s", Characters::ToString (*r).c_str ());
                    }
                }
                DbgTrace ("Latest Reference=%d", db.GetLastScan (ScanKindType_::Reference).value_or (static_cast<ScanIDType_> (-1)));
                SpectrumType_      spectrum;
                const unsigned int kNRecordsAddedPerTestCall = 100;
                for (int i = 0; i < kNRecordsAddedPerTestCall; ++i) {
                    DateTime    scanStartTime = DateTime::Now () - Duration (.1);
                    DateTime    scanEndTime   = DateTime::Now ();
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
                String dbFileName = IO::FileSystem::WellKnownLocations::GetTemporary () + L"foo.db";
                IO::FileSystem::Default ().RemoveFileIf (dbFileName);
                for (unsigned int i = 0; i < 5; ++i) {
                    PRIVATE_::DB db{dbFileName};
                    test (db, i);
                }
            }
            {
                PRIVATE_::DB db{SQLite::Connection::eInMemoryDB};
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
