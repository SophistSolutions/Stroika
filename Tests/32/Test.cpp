/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
//  TEST    Foundation::Database
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Characters/Format.h"
#include    "Stroika/Foundation/Characters/StringBuilder.h"
#include    "Stroika/Foundation/Characters/ToString.h"
#include    "Stroika/Foundation/Database/SQLite.h"
#include    "Stroika/Foundation/Debug/Trace.h"
#include    "Stroika/Foundation/Time/Duration.h"

#include    "../TestHarness/TestHarness.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Database;
using   namespace   Stroika::Foundation::Debug;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::Time;



// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1



namespace {
    // @todo MOVE TO LIB
    String  QuoteStringForDB_ (const String& s)
    {
        // @todo discuss with John/review sqlite docs
        if (s.Contains ('\'')) {
            StringBuilder sb;
            for (Character c : s) {
                if (c == '\'') {
                    sb += '\'';
                }
                sb += c;
            }
            return sb.str ();
        }
        else {
            return s;
        }
    }
}

namespace   {
    namespace RegressionTest1_x_ {
        namespace PRIVATE_ {
            using   Statement   =   Database::SQLite::DB::Statement;
            enum class ScanKindType_ {
                Background,
                Reference,
                Sample,
                Stroika_Define_Enum_Bounds(Background, Sample)
            };
            using   ScanIDType_                 =   uint64_t;
            using   SpectrumType_               =   Mapping<double, double>;
            using   PersistenceScanAuxDataType_ =   Mapping<String, String>;
            struct DB {
            public:
                DB ()
                {
                    bool    created = false;
                    String experimentDBFullPath = L"/C:/temp/foo.db";
                    try {
                        fDB_ = make_unique<Database::SQLite::DB> (experimentDBFullPath, [&created] (Database::SQLite::DB & db) { created = true; InitialSetup_ (db); });
                    }
                    catch (...) {
                        DbgTrace (L"Error %s experiment DB: %s: %s", created ? L"creating" : L"opening", experimentDBFullPath.c_str (), Characters::ToString (current_exception ()).c_str ());
                        Execution::ReThrow ();
                    }
                    if (created) {
                        DbgTrace (L"Initialized new experiment DB: %s", experimentDBFullPath.c_str ());
                    }
                    else {
                        DbgTrace (L"Opened experiment DB: %s", experimentDBFullPath.c_str ());
                    }
                }
                DB (const DB&) = delete;
                nonvirtual  DB& operator= (const DB&) = delete;
                nonvirtual  ScanIDType_ ScanPersistenceAdd (const DateTime& ScanStart, const DateTime& ScanEnd, const Optional<String>& ScanLabel, ScanKindType_ scanKind, const Optional<SpectrumType_>& rawSpectrum, const PersistenceScanAuxDataType_& AuxData, const Optional<ScanIDType_>& Background, const Optional<ScanIDType_>& Reference)
                {
                    String  insertSQL = [&] () {
                        StringBuilder sb;
                        sb +=  L"insert into Scans (StartAt, EndAt, ScanTypeIDRef, RawScanData, ScanLabel)";
                        sb += L"select ";
                        sb += L"'" + ScanStart.AsUTC ().Format (DateTime::PrintFormat::eISO8601) +  L"',";
                        sb += L"'" + ScanEnd.AsUTC ().Format (DateTime::PrintFormat::eISO8601) +  L"',";
                        sb += Characters::Format (L"%d", scanKind) + L",";
                        if (rawSpectrum) {

                            //
                            // @todo next!!!
                            // QuoteStringForDB_ (JSON::Writer ().WriteAsString (mapper.AsVariantValue (*rawSpectrum)))


                            sb += L"'" + String (L"SomeLongASCIIStringSomeLongASCIIStringSomeLongASCIIStringSomeLongASCIIString") +  L"',";
                        }
                        else {
                            sb += L"NULL,";
                        }
                        if (ScanLabel) {
                            sb += L"'" + QuoteStringForDB_ (*ScanLabel) +  L"'";
                        }
                        else {
                            sb += L"NULL";
                        }
                        sb += L";";
                        return sb.str ();
                    } ();
                    fDB_->Exec (insertSQL);
                    Statement s { fDB_.get (),  L"SELECT MAX(ScanId) FROM Scans;" };

                    if (Optional<Statement::RowType> r = s.GetNextRow ()) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace (L"ROW: %s", Characters::ToString (*r).c_str ());
#endif
                        return r->Lookup (L"MAX(ScanId)")->As<ScanIDType_> ();
                    }
                    AssertNotReached ();
                    return ScanIDType_ {};
                }
                nonvirtual  Optional<ScanIDType_> GetLastScan (ScanKindType_ scanKind)
                {
                    Statement s { fDB_.get (),  Characters::Format (L"select MAX(ScanId) from Scans where  ScanTypeIDRef='%d';", scanKind)};
                    if (Optional<Statement::RowType> r = s.GetNextRow ()) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace (L"ROW: %s", Characters::ToString (*r).c_str ());
#endif
                        return r->Lookup (L"MAX(ScanId)")->As<ScanIDType_> ();
                    }
                    return Optional<ScanIDType_> {};
                }
                static  void    InitialSetup_ (Database::SQLite::DB& db)
                {
                    TraceContextBumper ctx (SDKSTR ("ScanDB_::DB::InitialSetup_"));
                    auto tableSetup_ScanTypes = [&db] () {
                        db.Exec (L"create table 'ScanTypes' "
                                 L"("
                                 L"ScanTypeId tinyint Primary Key,"
                                 L"TypeName varchar(255) not null"
                                 L");"
                                );
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
                            L");"
                        );
                    };
                    auto tableSetup_ScanSets = [&db] () {
                        db.Exec (
                            L"Create table ScanSet"
                            L"("
                            L"ScanSetID bigint,"
                            L"ScanIDRef integer,"
                            L"Foreign key (ScanIdRef) References Scans(ScanId)"
                            L");"
                        );
                    };
                    auto tableSetup_AuxData = [&db] () {
                        db.Exec (
                            L"Create table AuxData"
                            L"("
                            L"ScanSetIDRef bigint Primary Key,"
                            L"Results varchar,"
                            L"Foreign key (ScanSetIDRef) References ScanSet(ScanSetID)"
                            L");"
                        );
                    };
                    auto tableSetup_ExtraForeignKeys = [&db] () {
                        db.Exec (L"Alter table Scans add column DependsOnScanSetIdRef bigint references  ScanSet(ScanSetID);");
                        db.Exec (L"Alter table Scans add column RawScanData varchar;");
                    };
                    tableSetup_ScanTypes ();
                    tableSetup_Scans ();
                    tableSetup_ScanSets ();
                    tableSetup_AuxData ();
                    tableSetup_ExtraForeignKeys ();
                }
                unique_ptr<Database::SQLite::DB>    fDB_;
            };

        }
        void    DoIt ()
        {
            using   namespace   PRIVATE_;
            TraceContextBumper ctx (SDKSTR ("ScanDB::DB::RunTest"));
            PRIVATE_::DB db;
            db.fDB_->Exec (L"select * from ScanTypes;");
            {
                Statement s { db.fDB_.get (), L"select * from ScanTypes;" };
                while (Optional<Statement::RowType> r = s.GetNextRow ()) {
                    DbgTrace (L"ROW: %s", Characters::ToString (*r).c_str ());
                }
            }
            DbgTrace ("Latest Reference=%d", db.GetLastScan (ScanKindType_::Reference).Value (-1));
            SpectrumType_    spectrum;

            PersistenceScanAuxDataType_ AuxData;
            Optional<ScanIDType_> Background;
            Optional<ScanIDType_> Reference;
            for (int i = 0; i < 100; ++i) {
                DateTime    scanStartTime   =   DateTime::Now () - Duration (.1);
                DateTime    scanEndTime     =   DateTime::Now ();
                ScanIDType_ sid = db.ScanPersistenceAdd (scanStartTime, scanEndTime, String {L"Hi Mom"}, ScanKindType_::Reference, spectrum, AuxData, Background, Reference);
                DbgTrace ("ScanPersistenceAdd returned id=%d, and laserScan reported=%d", sid, db.GetLastScan (ScanKindType_::Reference).Value (-1));
            }
        }
    }
}



namespace   {

    void    DoRegressionTests_ ()
    {
        RegressionTest1_x_::DoIt ();
    }

}





int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

