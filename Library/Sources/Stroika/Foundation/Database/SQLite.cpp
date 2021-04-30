/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/Format.h"
#include "../Characters/StringBuilder.h"
#include "../Characters/ToString.h"
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

#if qHasFeature_sqlite
namespace {
    void ThrowSQLiteErrorIfNotOK_ (int errCode, sqlite3* sqliteConnection = nullptr)
    {
        static_assert (SQLITE_OK == 0);
        if (errCode != SQLITE_OK) [[UNLIKELY_ATTR]] {
            if (sqliteConnection != nullptr) {
                Execution::Throw (Exception{Characters::Format (L"SQLite Error: %s (code %d)", String::FromUTF8 (::sqlite3_errmsg (sqliteConnection)).c_str (), errCode)});
            }
            Execution::Throw (Exception{Characters::Format (L"SQLite Error: %d", errCode)});
        }
    }
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
 *************************** SQLite::CompiledOptions ****************************
 ********************************************************************************
 */
namespace {
    struct VerifyFlags_ {
        VerifyFlags_ ()
        {
            Assert (CompiledOptions::kThe.ENABLE_NORMALIZE == !!::sqlite3_compileoption_used ("ENABLE_NORMALIZE"));
            Assert (CompiledOptions::kThe.THREADSAFE == !!::sqlite3_compileoption_used ("THREADSAFE"));
        }
    } sVerifyFlags_;
}

/*
 ********************************************************************************
 *************************** SQLite::Connection *********************************
 ********************************************************************************
 */

auto SQLite::Connection::New (const Options& options, const function<void (const Connection::Ptr&)>& dbInitializer) -> Ptr
{
    auto tmp = make_shared<Rep_> (options);
    Ptr  result{tmp};
    if (tmp->fTmpHackCreated_) {
        dbInitializer (result);
    }
    return result;
}

Connection::Rep_::Rep_ (const Options& options)
{
    TraceContextBumper ctx{"SQLite::Connection::Connection"};

    int flags = 0;
    if (options.fThreadingMode) {
        switch (*options.fThreadingMode) {
            case Options::ThreadingMode::eSingleThread:
                break;
            case Options::ThreadingMode::eMultiThread:
                Require (::sqlite3_threadsafe ());
                flags += SQLITE_OPEN_NOMUTEX;
                break;
            case Options::ThreadingMode::eSerialized:
                Require (::sqlite3_threadsafe ());
                flags += SQLITE_OPEN_FULLMUTEX;
                break;
        }
    }

    if (options.fImmutable) {
        // NYI cuz requires uri syntax
        WeakAssertNotImplemented ();
        Require (options.fReadOnly);
    }
    flags |= options.fReadOnly ? SQLITE_OPEN_READONLY : SQLITE_OPEN_READWRITE;

    string uriArg;
    {
        int n{};
        if (options.fDBPath) {
            n++;
        }
        if (options.fTemporaryDB) {
            n++;
        }
        if (options.fInMemoryDB) {
            n++;
        }
        Require (n == 1); // exactly one of fDBPath, fTemporaryDB, fInMemoryDB must be provided
    }
    if (options.fDBPath) {
        uriArg = options.fDBPath->generic_string ();
        if (uriArg[0] == ':') {
            uriArg = "./" + uriArg; // sqlite docs warn to do this, to avoid issues with :memory or other extensions
        }
    }
    if (options.fTemporaryDB) {
        uriArg = string{}; // not sure how to give name to tmp file - maybe must use url syntax?
        if (not options.fTemporaryDB->empty ()) {
            AssertNotImplemented ();
        }
    }
    if (options.fInMemoryDB) {
        flags |= SQLITE_OPEN_MEMORY;
        Require (not options.fReadOnly);
        Require (options.fCreateDBPathIfDoesNotExist);
        uriArg = options.fInMemoryDB->AsNarrowSDKString (); // often empty string
        if (uriArg.empty ()) {
            uriArg = ":memory";
        }
        else {
            WeakAssertNotImplemented (); // maybe can do this with URI syntax, but not totally clear
        }
        // For now, it appears we ALWAYS create memory DBS when opening (so cannot find a way to open shared) - so always set created flag
        fTmpHackCreated_ = true;
    }

    int e;
    if ((e = ::sqlite3_open_v2 (uriArg.c_str (), &fDB_, flags, options.fVFS ? options.fVFS->AsNarrowSDKString ().c_str () : nullptr)) == SQLITE_CANTOPEN) {
        if (options.fCreateDBPathIfDoesNotExist) {
            if (fDB_ != nullptr) {
                Verify (::sqlite3_close (fDB_) == SQLITE_OK);
                fDB_ = nullptr;
            }
            if ((e = ::sqlite3_open_v2 (uriArg.c_str (), &fDB_, SQLITE_OPEN_CREATE | flags, options.fVFS ? options.fVFS->AsNarrowSDKString ().c_str () : nullptr)) == SQLITE_OK) {
                fTmpHackCreated_ = true;
            }
        }
    }
    if (e != SQLITE_OK) [[UNLIKELY_ATTR]] {
        if (fDB_ != nullptr) {
            Verify (::sqlite3_close (fDB_) == SQLITE_OK);
        }
        ThrowSQLiteErrorIfNotOK_ (e, fDB_);
    }
    EnsureNotNull (fDB_);
}

Connection::Rep_::~Rep_ ()
{
    AssertNotNull (fDB_);
    Verify (::sqlite3_close (fDB_) == SQLITE_OK);
}

#if qHasFeature_sqlite
sqlite3* Connection::Rep_::Peek ()
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this}; // not super helpful, but could catch errors - reason not very helpful is we lose lock long before we stop using ptr
    return fDB_;
}
#endif

void Connection::Rep_::Exec (const String& sql)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    char*                                              db_err{};
    int                                                e = ::sqlite3_exec (fDB_, sql.AsUTF8 ().c_str (), NULL, 0, &db_err);
    if (e != SQLITE_OK) [[UNLIKELY_ATTR]] {
        if (db_err == nullptr or *db_err == '\0') {
            ThrowSQLiteErrorIfNotOK_ (e, fDB_);
        }
        else {
            Execution::Throw (Exception{Characters::Format (L"SQLite Error %d: %s", e, String::FromUTF8 (db_err).c_str ())});
        }
    }
}
#endif

#if qHasFeature_sqlite
/*
 ********************************************************************************
 ********************* SQLite::Statement::ColumnDescription *********************
 ********************************************************************************
 */
String Statement::ColumnDescription::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    sb += L"name: " + Characters::ToString (fName) + L", ";
    sb += L"type: " + Characters::ToString (fType);
    sb += L"}";
    return sb.str ();
}
#endif

#if qHasFeature_sqlite
/*
 ********************************************************************************
 ****************** SQLite::Statement::ParameterDescription *********************
 ********************************************************************************
 */
String Statement::ParameterDescription::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    sb += L"name: " + Characters::ToString (fName) + L", ";
    sb += L"value: " + Characters::ToString (fValue);
    sb += L"}";
    return sb.str ();
}
#endif

#if qHasFeature_sqlite
/*
 ********************************************************************************
 ******************************* SQLite::Statement ******************************
 ********************************************************************************
 */
Statement::Statement (const Connection::Ptr& db, const wchar_t* formatQuery, ...)
    : fConnectionPtr_{db}
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx{"SQLite::DB::Statement::CTOR"};
#endif
    RequireNotNull (db);
    RequireNotNull (db->Peek ());
    //  lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{*fConnectionPtr_};
    va_list argsList;
    va_start (argsList, formatQuery);
    String query = Characters::FormatV (formatQuery, argsList);
    va_end (argsList);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"(db=%p,query='%s')", db, query.c_str ());
#endif
    string      queryUTF8 = query.AsUTF8 ();
    const char* pzTail    = nullptr;
    ThrowSQLiteErrorIfNotOK_ (::sqlite3_prepare_v2 (db->Peek (), queryUTF8.c_str (), -1, &fStatementObj_, &pzTail), db->Peek ());
    Assert (pzTail != nullptr);
    if (*pzTail != '\0') {
        // @todo possibly should allow 0 or string of whitespace and ignore that too? -- LGP 2021-04-29
        Execution::Throw (Exception{L"Unexpected text after query"sv});
    }
    AssertNotNull (fStatementObj_);
    unsigned int colCount = static_cast<unsigned int> (::sqlite3_column_count (fStatementObj_));
    for (unsigned int i = 0; i < colCount; ++i) {
        const char* colTypeUTF8 = ::sqlite3_column_decltype (fStatementObj_, i);
        fColumns_.push_back (ColumnDescription{String::FromUTF8 (::sqlite3_column_name (fStatementObj_, i)), (colTypeUTF8 == nullptr) ? optional<String>{} : String::FromUTF8 (colTypeUTF8)});
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"sqlite3_column_decltype(i) = %s", ::sqlite3_column_decltype (fStatementObj_, i) == nullptr ? L"{nullptr}" : String::FromUTF8 (::sqlite3_column_decltype (fStatementObj_, i)).c_str ());
#endif
    }

    // Default setting (not documented, but I assume) is null
    unsigned int paramCount = static_cast<unsigned int> (::sqlite3_bind_parameter_count (fStatementObj_));
    for (unsigned int i = 1; i <= paramCount; ++i) {
        const char* tmp = ::sqlite3_bind_parameter_name (fStatementObj_, i); // can be null
        fParameters_ += ParameterDescription{tmp == nullptr ? optional<String>{} : String::FromUTF8 (tmp), nullptr};
    }
}

String Statement::GetSQL (WhichSQLFlag whichSQL) const
{
    Assert (not CompiledOptions::kThe.ENABLE_NORMALIZE);
    // lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{*fConnectionPtr_};
    switch (whichSQL) {
        case WhichSQLFlag::eOriginal:
            return String::FromUTF8 (::sqlite3_sql (fStatementObj_));
        case WhichSQLFlag::eExpanded: {
            auto tmp = ::sqlite3_expanded_sql (fStatementObj_);
            if (tmp != nullptr) {
                String r = String::FromUTF8 (tmp);
                ::sqlite3_free (tmp);
                return r;
            }
            throw bad_alloc{};
        }
        case WhichSQLFlag::eNormalized:
            if constexpr (CompiledOptions::kThe.ENABLE_NORMALIZE) {
                return String::FromUTF8 (::sqlite3_normalized_sql (fStatementObj_));
            }
            RequireNotReached ();
            return String{};
        default:
            RequireNotReached ();
            return String{};
    }
}

auto Statement::GetNextRow () -> optional<Row>
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx{"SQLite::DB::Statement::GetNextRow"};
#endif
    //  lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{*fConnectionPtr_};
    // @todo MAYBE redo with https://www.sqlite.org/c3ref/value.html
    int rc;
    AssertNotNull (fStatementObj_);
    if ((rc = ::sqlite3_step (fStatementObj_)) == SQLITE_ROW) {
        Row row;
        for (unsigned int i = 0; i < fColumns_.size (); ++i) {
            VariantValue v;
            // The actual returned type may not be the same as the DECLARED type (for example if the column is declared varaint)
            switch (::sqlite3_column_type (fStatementObj_, i)) {
                case SQLITE_INTEGER: {
                    v = VariantValue{::sqlite3_column_int (fStatementObj_, i)};
                } break;
                case SQLITE_FLOAT: {
                    v = VariantValue{::sqlite3_column_double (fStatementObj_, i)};
                } break;
                case SQLITE_BLOB: {
                    const byte* data      = reinterpret_cast<const byte*> (::sqlite3_column_blob (fStatementObj_, i));
                    size_t      byteCount = static_cast<size_t> (::sqlite3_column_bytes (fStatementObj_, i));
                    v                     = VariantValue{Memory::BLOB{data, data + byteCount}};
                } break;
                case SQLITE_NULL: {
                    // default to null value
                } break;
                case SQLITE_TEXT: {
                    // @todo redo as sqlite3_column_text16, but doesn't help unix case? Maybe just iff sizeof(wchart_t)==2?
                    AssertNotNull (::sqlite3_column_text (fStatementObj_, i));
                    v = VariantValue{String::FromUTF8 (reinterpret_cast<const char*> (::sqlite3_column_text (fStatementObj_, i)))};
                } break;
                default: {
                    AssertNotReached ();
                } break;
            }
            row.Add (fColumns_[i].fName, v);
        }
        return row;
    }
    return nullopt;
}

Statement::~Statement ()
{
    AssertNotNull (fStatementObj_);
    //  lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{*fConnectionPtr_};
    ::sqlite3_finalize (fStatementObj_);
}

void Statement::Bind (unsigned int parameterIndex, const VariantValue& v)
{
    //  lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{*fConnectionPtr_};
    fParameters_[parameterIndex].fValue = v;
    switch (v.GetType ()) {
        case VariantValue::eString:
            ThrowSQLiteErrorIfNotOK_ (::sqlite3_bind_text (fStatementObj_, parameterIndex + 1, v.As<String> ().AsUTF8 ().c_str (), -1, SQLITE_TRANSIENT));
            break;
        case VariantValue::eInteger:
            ThrowSQLiteErrorIfNotOK_ (::sqlite3_bind_int64 (fStatementObj_, parameterIndex + 1, v.As<sqlite3_int64> ()));
            break;
        case VariantValue::eFloat:
            ThrowSQLiteErrorIfNotOK_ (::sqlite3_bind_double (fStatementObj_, parameterIndex + 1, v.As<double> ()));
            break;
        case VariantValue::eNull:
            ThrowSQLiteErrorIfNotOK_ (::sqlite3_bind_null (fStatementObj_, parameterIndex + 1));
            break;
        default:
            AssertNotImplemented (); // add more types - esp BLOB
            break;
    }
}

void Statement::Bind (const String& parameterName, const VariantValue& v)
{
    for (unsigned int i = 0; i < fParameters_.length (); ++i) {
        if (fParameters_[i].fName == parameterName) {
            Bind (i, v);
            return;
        }
    }
    RequireNotReached (); // invalid paramter name provided
}

void Statement::Bind (const Traversal::Iterable<ParameterDescription>& parameters)
{
    int idx = 0;
    for (auto i : parameters) {
        if (i.fName) {
            Bind (*i.fName, i.fValue);
        }
        else {
            Bind (idx, i.fValue);
        }
        idx++;
    }
}

String Statement::ToString () const
{
    //   lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{*fConnectionPtr_};
    StringBuilder sb;
    sb += L"{";
    sb += L"Parameter-Bindings: " + Characters::ToString (fParameters_) + L", ";
    sb += L"Column-Descriptions: " + Characters::ToString (fColumns_) + L", ";
    sb += L"Original-SQL: " + Characters::ToString (GetSQL ());
    sb += L"}";
    return sb.str ();
}

#endif
