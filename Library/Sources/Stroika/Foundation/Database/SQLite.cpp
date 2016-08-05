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




#if     qHasFeature_sqlite
/*
 ********************************************************************************
 ************************* SQLite::DB::Statement ********************************
 ********************************************************************************
 */
DB::Statement::Statement (sqlite3* db, const String& query)
{
    int rc = sqlite3_prepare_v2(db, query.AsUTF8 ().c_str (), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        Execution::Throw (StringException (Characters::Format (L"SQLite Error %s:", String::FromUTF8 (sqlite3_errmsg (db)).c_str ())));
    }
    nParams = sqlite3_column_count (stmt);
    for (size_t i = 0; i < nParams; ++i) {
        fColNames += String::FromUTF8 (sqlite3_column_name (stmt, i));

        // add VaroamtVa;ue"::Type list based on sqlite3_column_decltype
    }
}

/// returns 'missing' on EOF, exception on error
auto   DB::Statement::GetNextRow () -> Optional<RowType> {
    // use SQLITE_API const char *SQLITE_STDCALL sqlite3_column_name(sqlite3_stmt*, int N);
    // to get column name


    // @todo redo with https://www.sqlite.org/c3ref/value.html

    int rc;
    if (( rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        RowType row;
        for (size_t i = 0; i < nParams; ++i) {
            //DbgTrace (L"sqlite3_column_decltype(i) = %s", String::FromUTF8 (sqlite3_column_decltype(stmt, i)).c_str ());
            //DbgTrace (L"COLNAME=%s", fColNames[i].c_str () );
            row.Add (fColNames[i], VariantValue (String::FromUTF8 (reinterpret_cast<const char*> (sqlite3_column_text (stmt, i)))));
            //DbgTrace ("rowsize now %d", row.size ());
        }
        return row;
    }
    //DbgTrace ("***sqlite3_step returned %d", rc);
    return Optional<RowType> ();
}

DB::Statement::~Statement ()
{
    sqlite3_finalize(stmt);
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
    bool    created = false;
    int e;
    if ((e = ::sqlite3_open_v2 ((L"file://" + experimentDBFullPath).AsUTF8 ().c_str (), &fDB_,  SQLITE_OPEN_URI | SQLITE_OPEN_READWRITE, nullptr)) == SQLITE_CANTOPEN) {
        if ((e = ::sqlite3_open_v2 ((L"file://" + experimentDBFullPath).AsUTF8 ().c_str (), &fDB_,  SQLITE_OPEN_URI |  SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, nullptr)) == SQLITE_OK) {
            try {
                dbInitializer (*this);
            }
            catch (...) {
                //Logger::Get ().Log (Logger::Priority::eInfo, L"Errir settuibg yo d: %s: %s", experimentDBFullPath.c_str (), Characters::ToString (current_exception ()).c_str ());
                ::sqlite3_close(fDB_);
                Execution::ReThrow ();
            }
            created = true;
        }
    }
    else if (e == SQLITE_OK) {
        if (created) {
            //Logger::Get ().Log (Logger::Priority::eInfo, L"Created new experiment DB: %s", experimentDBFullPath.c_str ());
        }
        else {
            //Logger::Get ().Log (Logger::Priority::eInfo, L"Opened experiment DB: %s", experimentDBFullPath.c_str ());
        }
    }
    else {
        //Logger::Get ().Log (Logger::Priority::eError, L"FAILED opening experiment DB: %s", experimentDBFullPath.c_str ());
        Execution::Throw (StringException (Characters::Format (L"SQLite Error %d:", e)));
    }
}

DB:: ~DB ()
{
    ::sqlite3_close(fDB_);
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
