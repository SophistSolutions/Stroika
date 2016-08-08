/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"
#include    "../Debug/Trace.h"

#include    "SQLite.h"



using   namespace   Stroika::Foundation;

using   namespace   Characters;
using   namespace   Debug;
using   namespace   Database;
using   namespace   Database::SQLite;
using   namespace   Execution;



// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1



#if     qHasFeature_sqlite && defined (_MSC_VER)
// Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#pragma comment (lib, "sqlite.lib")
#endif





#if     qHasFeature_sqlite
/*
 ********************************************************************************
 ************************* SQLite::DB::Statement ********************************
 ********************************************************************************
 */
DB::Statement::Statement (sqlite3* db, const String& query)
{
    RequireNotNull (db);
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx (SDKSTR ("SQLite::DB::Statement::Statement"));
    DbgTrace (L"(db=%p,query='%s')", db, query.c_str ());
#endif
    int rc = ::sqlite3_prepare_v2 (db, query.AsUTF8 ().c_str (), -1, &fStatementObj_, NULL);
    if (rc != SQLITE_OK) {
        Execution::Throw (StringException (Characters::Format (L"SQLite Error %s:", String::FromUTF8 (::sqlite3_errmsg (db)).c_str ())));
    }
    AssertNotNull (fStatementObj_);
    fParamsCount_ = ::sqlite3_column_count (fStatementObj_);
    for (unsigned int i = 0; i < fParamsCount_; ++i) {
        fColNames_ += String::FromUTF8 (::sqlite3_column_name (fStatementObj_, i));
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"sqlite3_column_decltype(i) = %s", ::sqlite3_column_decltype (fStatementObj_, i) == nullptr ? L"{nullptr}" : String::FromUTF8 (::sqlite3_column_decltype (fStatementObj_, i)).c_str ());
#endif
        // add VaroamtVa;ue"::Type list based on sqlite3_column_decltype
    }
}

/// returns 'missing' on EOF, exception on error
auto   DB::Statement::GetNextRow () -> Optional<RowType> {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx (SDKSTR ("SQLite::DB::Statement::GetNextRow"));
#endif
    // @todo redo with https://www.sqlite.org/c3ref/value.html
    int rc;
    AssertNotNull (fStatementObj_);
    if (( rc = ::sqlite3_step (fStatementObj_)) == SQLITE_ROW)
    {
        RowType row;
        for (unsigned int i = 0; i < fParamsCount_; ++i) {
            // redo as sqlite3_column_text16
            // @todo AND iether use value object or check INTERANL TYOPE  - https://www.sqlite.org/c3ref/column_blob.html and return the right one  - NULL, INTEGER, FLOAT, TEXT, BLOB
            VariantValue    v;
            switch (::sqlite3_column_type (fStatementObj_, i)) {
                case SQLITE_INTEGER: {
                        v = VariantValue (::sqlite3_column_int (fStatementObj_, i));
                    }
                    break;
                case SQLITE_FLOAT: {
                        v = VariantValue (::sqlite3_column_double (fStatementObj_, i));
                    }
                    break;
                case SQLITE_BLOB: {
                        const Byte* data		=	reinterpret_cast<const Byte*> (::sqlite3_column_blob (fStatementObj_, i));
                        size_t		byteCount	=	static_cast<size_t> (::sqlite3_column_bytes (fStatementObj_, i));
                        v = VariantValue (Memory::BLOB (data, data + byteCount));
                    }
                    break;
                case SQLITE_NULL: {
                        // default to null value
                    }
                    break;
                case SQLITE_TEXT: {
                        AssertNotNull (::sqlite3_column_text (fStatementObj_, i));
                        v = VariantValue (String::FromUTF8 (reinterpret_cast<const char*> (::sqlite3_column_text (fStatementObj_, i))));
                    }
                    break;
                default: {
                        AssertNotReached ();
                    }
                    break;
            }
            row.Add (fColNames_[i], v);
        }
        return row;
    }
    return Optional<RowType> ();
}

DB::Statement::~Statement ()
{
    AssertNotNull (fStatementObj_);
    ::sqlite3_finalize (fStatementObj_);
}





/*
 ********************************************************************************
 ********************************** SQLite::DB **********************************
 ********************************************************************************
 */
DB::DB (const String& experimentDBFullPath, const function<void(DB&)>& dbInitializer)
{
    TraceContextBumper ctx (SDKSTR ("SQLite::DB::DB"));
    // @todo - code cleanup!!!
    int e;
    if ((e = ::sqlite3_open_v2 ((L"file://" + experimentDBFullPath).AsUTF8 ().c_str (), &fDB_,  SQLITE_OPEN_URI | SQLITE_OPEN_READWRITE, nullptr)) == SQLITE_CANTOPEN) {
        if ((e = ::sqlite3_open_v2 ((L"file://" + experimentDBFullPath).AsUTF8 ().c_str (), &fDB_,  SQLITE_OPEN_URI |  SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, nullptr)) == SQLITE_OK) {
            try {
                dbInitializer (*this);
            }
            catch (...) {
                Verify (::sqlite3_close (fDB_) == SQLITE_OK);
                Execution::ReThrow ();
            }
        }
    }
    else if (e != SQLITE_OK) {
        Assert (fDB_ == nullptr);
        // @todo add error string
        Execution::Throw (StringException (Characters::Format (L"SQLite Error %d:", e)));
    }
}

DB:: ~DB ()
{
    AssertNotNull (fDB_);
    Verify (::sqlite3_close (fDB_) == SQLITE_OK);
}

void    DB::Exec (const String& cmd2Exec)
{
    char* db_err {};
    int e = ::sqlite3_exec (fDB_, cmd2Exec.AsUTF8 ().c_str (), NULL, 0, &db_err);
    if (e != SQLITE_OK) {
        if (db_err == nullptr or * db_err == '\0') {
            DbgTrace (L"Failed doing sqllite command: %s", cmd2Exec.c_str ());
            Execution::Throw (StringException (Characters::Format (L"SQLite Error %d", e)));
        }
        else {
            Execution::Throw (StringException (Characters::Format (L"SQLite Error %d: %s", e, String::FromUTF8 (db_err).c_str ())));
        }
    }
}
#endif
