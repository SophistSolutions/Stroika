/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/Format.h"
#include "../Characters/StringBuilder.h"
#include "../Debug/Trace.h"

#include "SQLite.h"

using std::byte;

using namespace Stroika::Foundation;

using namespace Characters;
using namespace Debug;
using namespace Database;
using namespace Database::SQLite;
using namespace Execution;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

#if qHasFeature_sqlite && defined(_MSC_VER)
// Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#pragma comment(lib, "sqlite.lib")
#endif

#if qHasFeature_sqlite
namespace {
    struct ModuleShutdown_ {
        ~ModuleShutdown_ ()
        {
            Verify (::sqlite3_shutdown () == SQLITE_OK); // mostly pointless but avoids memory leak complaints
        }
    } sModuleShutdown_;
}
#endif

/*
 ********************************************************************************
 ************************* SQLite::QuoteStringForDB *****************************
 ********************************************************************************
 */
String SQLite::QuoteStringForDB (const String& s)
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

#if qHasFeature_sqlite
/*
 ********************************************************************************
 ********************* SQLite::Connection::Statement ****************************
 ********************************************************************************
 */
Connection::Statement::Statement (Connection* db, const wchar_t* formatQuery, ...)
    : fConnectionCritSec_{*db}
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx ("SQLite::DB::Statement::CTOR");
#endif
    RequireNotNull (db);
    RequireNotNull (db->Peek ());
    va_list argsList;
    va_start (argsList, formatQuery);
    String query = Characters::FormatV (formatQuery, argsList);
    va_end (argsList);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"(db=%p,query='%s')", db, query.c_str ());
#endif
    int rc = ::sqlite3_prepare_v2 (db->Peek (), query.AsUTF8 ().c_str (), -1, &fStatementObj_, NULL);
    if (rc != SQLITE_OK)
        [[UNLIKELY_ATTR]]
        {
            Execution::Throw (Exception (Characters::Format (L"SQLite Error %s:", String::FromUTF8 (::sqlite3_errmsg (db->Peek ())).c_str ())));
        }
    AssertNotNull (fStatementObj_);
    fParamsCount_ = ::sqlite3_column_count (fStatementObj_);
    for (unsigned int i = 0; i < fParamsCount_; ++i) {
        fColNames_ += String::FromUTF8 (::sqlite3_column_name (fStatementObj_, i));
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"sqlite3_column_decltype(i) = %s", ::sqlite3_column_decltype (fStatementObj_, i) == nullptr ? L"{nullptr}" : String::FromUTF8 (::sqlite3_column_decltype (fStatementObj_, i)).c_str ());
#endif
        // add VariantValue::Type list based on sqlite3_column_decltype
    }
}

auto Connection::Statement::GetNextRow () -> optional<RowType>
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx ("SQLite::DB::Statement::GetNextRow");
#endif
    // @todo redo with https://www.sqlite.org/c3ref/value.html
    int rc;
    AssertNotNull (fStatementObj_);
    if ((rc = ::sqlite3_step (fStatementObj_)) == SQLITE_ROW) {
        RowType row;
        for (unsigned int i = 0; i < fParamsCount_; ++i) {
            // redo as sqlite3_column_text16
            // @todo AND iether use value object or check INTERANL TYOPE  - https://www.sqlite.org/c3ref/column_blob.html and return the right one  - NULL, INTEGER, FLOAT, TEXT, BLOB
            VariantValue v;
            switch (::sqlite3_column_type (fStatementObj_, i)) {
                case SQLITE_INTEGER: {
                    v = VariantValue (::sqlite3_column_int (fStatementObj_, i));
                } break;
                case SQLITE_FLOAT: {
                    v = VariantValue (::sqlite3_column_double (fStatementObj_, i));
                } break;
                case SQLITE_BLOB: {
                    const byte* data      = reinterpret_cast<const byte*> (::sqlite3_column_blob (fStatementObj_, i));
                    size_t      byteCount = static_cast<size_t> (::sqlite3_column_bytes (fStatementObj_, i));
                    v                     = VariantValue (Memory::BLOB (data, data + byteCount));
                } break;
                case SQLITE_NULL: {
                    // default to null value
                } break;
                case SQLITE_TEXT: {
                    AssertNotNull (::sqlite3_column_text (fStatementObj_, i));
                    v = VariantValue (String::FromUTF8 (reinterpret_cast<const char*> (::sqlite3_column_text (fStatementObj_, i))));
                } break;
                default: {
                    AssertNotReached ();
                } break;
            }
            row.Add (fColNames_[i], v);
        }
        return row;
    }
    return nullopt;
}

Connection::Statement::~Statement ()
{
    AssertNotNull (fStatementObj_);
    ::sqlite3_finalize (fStatementObj_);
}

/*
 ********************************************************************************
 *************************** SQLite::Connection *********************************
 ********************************************************************************
 */
Connection::Connection (const URL& dbURL, const function<void(Connection&)>& dbInitializer)
{
    TraceContextBumper ctx ("SQLite::Connection::Connection");
    // @todo - code cleanup!!!
    int e;
    if ((e = ::sqlite3_open_v2 (dbURL.GetFullURL ().AsUTF8 ().c_str (), &fDB_, SQLITE_OPEN_URI | SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX, nullptr)) == SQLITE_CANTOPEN) {
        if (fDB_ != nullptr) {
            Verify (::sqlite3_close (fDB_) == SQLITE_OK);
            fDB_ = nullptr;
        }
        if ((e = ::sqlite3_open_v2 (dbURL.GetFullURL ().AsUTF8 ().c_str (), &fDB_, SQLITE_OPEN_URI | SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX, nullptr)) == SQLITE_OK) {
            try {
                dbInitializer (*this);
            }
            catch (...) {
                Verify (::sqlite3_close (fDB_) == SQLITE_OK);
                Execution::ReThrow ();
            }
        }
    }
    else if (e != SQLITE_OK)
        [[UNLIKELY_ATTR]]
        {
            Assert (fDB_ == nullptr);
            // @todo add error string
            Execution::Throw (Exception (Characters::Format (L"SQLite Error %d:", e)));
        }
}

Connection::Connection (const String& dbPath, const function<void(Connection&)>& dbInitializer)
{
    TraceContextBumper ctx ("SQLite::Connection::Connection");
    // @todo - code cleanup!!!
    int e;
    if ((e = ::sqlite3_open_v2 (dbPath.AsUTF8 ().c_str (), &fDB_, SQLITE_OPEN_READWRITE, nullptr)) == SQLITE_CANTOPEN) {
        if (fDB_ != nullptr) {
            Verify (::sqlite3_close (fDB_) == SQLITE_OK);
            fDB_ = nullptr;
        }
        if ((e = ::sqlite3_open_v2 (dbPath.AsUTF8 ().c_str (), &fDB_, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, nullptr)) == SQLITE_OK) {
            try {
                dbInitializer (*this);
            }
            catch (...) {
                Verify (::sqlite3_close (fDB_) == SQLITE_OK);
                Execution::ReThrow ();
            }
        }
    }
    else if (e != SQLITE_OK)
        [[UNLIKELY_ATTR]]
        {
            Assert (fDB_ == nullptr);
            // @todo add error string
            Execution::Throw (Exception (Characters::Format (L"SQLite Error %d:", e)));
        }
}

Connection::Connection (InMemoryDBFlag, const function<void(Connection&)>& dbInitializer)
{
    TraceContextBumper ctx ("SQLite::Connection::Connection");
    // @todo - code cleanup!!!
    int e;
    if ((e = ::sqlite3_open_v2 ("memory:", &fDB_, SQLITE_OPEN_MEMORY | SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, nullptr)) == SQLITE_OK) {
        try {
            dbInitializer (*this);
        }
        catch (...) {
            Verify (::sqlite3_close (fDB_) == SQLITE_OK);
            Execution::ReThrow ();
        }
    }
    else if (e != SQLITE_OK)
        [[UNLIKELY_ATTR]]
        {
            Assert (fDB_ == nullptr);
            // @todo add error string
            Execution::Throw (Exception (Characters::Format (L"SQLite Error %d:", e)));
        }
}

Connection::~Connection ()
{
    AssertNotNull (fDB_);
    Verify (::sqlite3_close (fDB_) == SQLITE_OK);
}

void Connection::Exec (const wchar_t* formatCmd2Exec, ...)
{
    RequireNotNull (formatCmd2Exec);
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    va_list                                            argsList;
    va_start (argsList, formatCmd2Exec);
    String cmd2Exec = Characters::FormatV (formatCmd2Exec, argsList);
    va_end (argsList);
    char* db_err{};
    int   e = ::sqlite3_exec (fDB_, cmd2Exec.AsUTF8 ().c_str (), NULL, 0, &db_err);
    if (e != SQLITE_OK)
        [[UNLIKELY_ATTR]]
        {
            if (db_err == nullptr or *db_err == '\0') {
                DbgTrace (L"Failed doing sqllite command: %s", cmd2Exec.c_str ());
                Execution::Throw (Exception (Characters::Format (L"SQLite Error %d", e)));
            }
            else {
                Execution::Throw (Exception (Characters::Format (L"SQLite Error %d: %s", e, String::FromUTF8 (db_err).c_str ())));
            }
        }
}
#endif
