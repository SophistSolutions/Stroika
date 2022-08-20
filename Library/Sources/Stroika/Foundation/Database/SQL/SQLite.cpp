/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <system_error>

#include "../../Characters/Format.h"
#include "../../Characters/StringBuilder.h"
#include "../../Characters/ToString.h"
#include "../../Debug/Trace.h"
#include "../../Time/Duration.h"

#include "SQLite.h"

using std::byte;

using namespace Stroika::Foundation;

using namespace Characters;
using namespace Debug;
using namespace Database;
using namespace Database::SQL::SQLite;
using namespace Execution;
using namespace Time;

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
    [[noreturn]] void ThrowSQLiteError_ (int errCode, sqlite3* sqliteConnection = nullptr)
    {
        Require (errCode != SQLITE_OK);
        optional<String> errMsgDetails;
        if (sqliteConnection != nullptr) {
            errMsgDetails = String::FromUTF8 (::sqlite3_errmsg (sqliteConnection));
        }
        switch (errCode) {
            case SQLITE_BUSY: {
                DbgTrace (L"SQLITE_BUSY"); //  The database file is locked
                Execution::Throw (system_error{make_error_code (errc::device_or_resource_busy)});
            } break;
            case SQLITE_LOCKED: {
                DbgTrace (L"SQLITE_LOCKED"); //  A table in the database is locked
                Execution::Throw (system_error{make_error_code (errc::device_or_resource_busy)});
            } break;
            case SQLITE_CONSTRAINT: {
                if (errMsgDetails) {
                    Execution::Throw (Exception{Characters::Format (L"SQLITE_CONSTRAINT: %s", errMsgDetails->c_str ())});
                }
                else {
                    static const auto kEx_ = Exception{L"SQLITE_CONSTRAINT"sv};
                    Execution::Throw (kEx_);
                }
            } break;
            case SQLITE_TOOBIG: {
                static const auto kEx_ = Exception{L"SQLITE_TOOBIG"sv};
                Execution::Throw (kEx_);
            } break;
            case SQLITE_FULL: {
                DbgTrace (L"SQLITE_FULL");
                Execution::Throw (system_error{make_error_code (errc::no_space_on_device)});
            } break;
            case SQLITE_READONLY: {
                static const auto kEx_ = Exception{L"SQLITE_READONLY"sv};
                Execution::Throw (kEx_);
            } break;
            case SQLITE_MISUSE: {
                if (errMsgDetails) {
                    Execution::Throw (Exception{Characters::Format (L"SQLITE_MISUSE: %s", errMsgDetails->c_str ())});
                }
                else {
                    static const auto kEx_ = Exception{L"SQLITE_MISUSE"sv};
                    Execution::Throw (kEx_);
                }
            } break;
            case SQLITE_ERROR: {
                if (errMsgDetails) {
                    Execution::Throw (Exception{Characters::Format (L"SQLITE_ERROR: %s", errMsgDetails->c_str ())});
                }
                else {
                    static const auto kEx_ = Exception{L"SQLITE_ERROR"sv};
                    Execution::Throw (kEx_);
                }
            } break;
            case SQLITE_NOMEM: {
                DbgTrace (L"SQLITE_NOMEM translated to bad_alloc");
                Execution::Throw (bad_alloc{});
            } break;
        }
        if (errMsgDetails) {
            Execution::Throw (Exception{Characters::Format (L"SQLite Error: %s (code %d)", errMsgDetails->c_str (), errCode)});
        }
        else {
            Execution::Throw (Exception{Characters::Format (L"SQLite Error: %d", errCode)});
        }
    }
    void ThrowSQLiteErrorIfNotOK_ (int errCode, sqlite3* sqliteConnection = nullptr)
    {
        static_assert (SQLITE_OK == 0);
        if (errCode != SQLITE_OK) [[UNLIKELY_ATTR]] {
            ThrowSQLiteError_ (errCode, sqliteConnection);
        }
    }
}

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
#if SQLITE_VERSION_NUMBER < 3038000
            Assert (CompiledOptions::kThe.ENABLE_JSON1 == !!::sqlite3_compileoption_used ("ENABLE_JSON1"));
#else
            Assert (CompiledOptions::kThe.ENABLE_JSON1 == !::sqlite3_compileoption_used ("OMIT_JSON1"));
#endif
        }
    } sVerifyFlags_;
}

/*
 ********************************************************************************
 *************************** SQLite::Connection::Rep_ ***************************
 ********************************************************************************
 */
struct Connection::Rep_ final : IRep {
    Rep_ (const Options& options)
    {
        TraceContextBumper ctx{"SQLite::Connection::Rep_::Rep_"};

        int flags = 0;
        if (options.fThreadingMode) {
            switch (*options.fThreadingMode) {
                case Options::ThreadingMode::eSingleThread:
                    break;
                case Options::ThreadingMode::eMultiThread:
                    Require (CompiledOptions::kThe.THREADSAFE);
                    Require (::sqlite3_threadsafe ());
                    flags += SQLITE_OPEN_NOMUTEX;
                    break;
                case Options::ThreadingMode::eSerialized:
                    Require (CompiledOptions::kThe.THREADSAFE);
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
                ++n;
            }
            if (options.fTemporaryDB) {
                ++n;
            }
            if (options.fInMemoryDB) {
                ++n;
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
            uriArg = string{};
            // According to https://sqlite.org/inmemorydb.html, temporary DBs appear to require empty name
            // @todo MAYBE fix to find a way to do named temporary DB? - or adjust API so no string name provided.
            Require (not options.fTemporaryDB->empty ());
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
                uriArg = "file:" + uriArg + "?mode=memory&cache=shared";
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
            [[maybe_unused]] auto&& cleanup = Finally ([this] () noexcept { if (fDB_ != nullptr) { Verify (::sqlite3_close (fDB_) == SQLITE_OK); } });
            ThrowSQLiteError_ (e, fDB_);
        }
        if (options.fBusyTimeout) {
            SetBusyTimeout (*options.fBusyTimeout);
        }
        if (options.fJournalMode) {
            SetJournalMode (*options.fJournalMode);
        }
        EnsureNotNull (fDB_);
    }
    ~Rep_ ()
    {
        AssertNotNull (fDB_);
        Verify (::sqlite3_close (fDB_) == SQLITE_OK);
    }
    bool                                       fTmpHackCreated_{false};
    virtual shared_ptr<const EngineProperties> GetEngineProperties () const override
    {
        struct MyEngineProperties_ final : EngineProperties {
            virtual String GetEngineName () const override
            {
                return L"SQLite"sv;
            }
            virtual String GetSQL (NonStandardSQL n) const override
            {
                switch (n) {
                    case NonStandardSQL::eDoesTableExist:
                        return L"SELECT name FROM sqlite_master WHERE type='table' AND name="sv + SQL::EngineProperties::kDoesTableExistParameterName;
                }
                AssertNotReached ();
                return String{};
            }
            virtual bool RequireStatementResetAfterModifyingStatmentToCompleteTransaction () const override
            {
                return true;
            }
            virtual bool SupportsNestedTransactions () const override
            {
                return false;
            }
        };
        static const shared_ptr<const EngineProperties> kProps_ = make_shared<const MyEngineProperties_> ();
        return kProps_;
    }
    virtual SQL::Statement mkStatement (const String& sql) override
    {
        Connection::Ptr conn = Connection::Ptr{dynamic_pointer_cast<Connection::IRep> (shared_from_this ())};
        return SQLite::Statement{conn, sql};
    }
    virtual SQL::Transaction mkTransaction () override
    {
        Connection::Ptr conn = Connection::Ptr{dynamic_pointer_cast<Connection::IRep> (shared_from_this ())};
        return SQLite::Transaction{conn};
    }
    virtual void Exec (const String& sql) override
    {
        lock_guard<const AssertExternallySynchronizedMutex> critSec{*this};
        [[maybe_unused]] char*                              db_err{}; // could use but its embedded in the fDB_ error string anyhow, and thats already peeked at by ThrowSQLiteErrorIfNotOK_ and it generates better exceptions (maps some to std c++ exceptions)
        int                                                 e = ::sqlite3_exec (fDB_, sql.AsUTF8 ().c_str (), NULL, 0, &db_err);
        if (e != SQLITE_OK) [[UNLIKELY_ATTR]] {
            ThrowSQLiteErrorIfNotOK_ (e, fDB_);
        }
    }
    virtual ::sqlite3* Peek () override
    {
        lock_guard<const AssertExternallySynchronizedMutex> critSec{*this}; // not super helpful, but could catch errors - reason not very helpful is we lose lock long before we stop using ptr
        return fDB_;
    }
    virtual Duration GetBusyTimeout () const override
    {
        lock_guard<const AssertExternallySynchronizedMutex> critSec{*this};
        optional<int>                                       d;
        auto                                                callback = [] (void* lamdaArg, [[maybe_unused]] int argc, char** argv, [[maybe_unused]] char** azColName) {
            optional<int>* pd = reinterpret_cast<optional<int>*> (lamdaArg);
            AssertNotNull (pd);
            Assert (argc == 1);
            Assert (::strcmp (azColName[0], "timeout") == 0);
            int val = ::atoi (argv[0]);
            Assert (val >= 0);
            *pd = val;
            return 0;
        };
        ThrowSQLiteErrorIfNotOK_ (::sqlite3_exec (fDB_, "pragma busy_timeout;", callback, &d, nullptr));
        Assert (d);
        return Duration{double (*d) / 1000.0};
    }
    virtual void SetBusyTimeout (const Duration& timeout) override
    {
        lock_guard<const AssertExternallySynchronizedMutex> critSec{*this};
        ThrowSQLiteErrorIfNotOK_ (::sqlite3_busy_timeout (fDB_, (int)(timeout.As<float> () * 1000)), fDB_);
    }
    virtual JournalModeType GetJournalMode () const override
    {
        optional<string> d;
        auto             callback = [] (void* lamdaArg, [[maybe_unused]] int argc, char** argv, [[maybe_unused]] char** azColName) {
            optional<string>* pd = reinterpret_cast<optional<string>*> (lamdaArg);
            AssertNotNull (pd);
            Assert (argc == 1);
            Assert (::strcmp (azColName[0], "journal_mode") == 0);
            *pd = argv[0];
            return 0;
        };
        ThrowSQLiteErrorIfNotOK_ (::sqlite3_exec (fDB_, "pragma journal_mode;", callback, &d, nullptr));
        Assert (d);
        if (d == "delete") {
            return JournalModeType::eDelete;
        }
        if (d == "truncate") {
            return JournalModeType::eTruncate;
        }
        if (d == "persist") {
            return JournalModeType::ePersist;
        }
        if (d == "memory") {
            return JournalModeType::eMemory;
        }
        if (d == "wal") {
            return JournalModeType::eWAL;
        }
        if (d == "wal2") {
            return JournalModeType::eWAL2;
        }
        if (d == "off") {
            return JournalModeType::eOff;
        }
        AssertNotReached ();
        return JournalModeType::eDelete;
    }
    virtual void SetJournalMode (JournalModeType journalMode) override
    {
        lock_guard<const AssertExternallySynchronizedMutex> critSec{*this};
        [[maybe_unused]] char*                              db_err{};
        switch (journalMode) {
            case JournalModeType::eDelete:
                ThrowSQLiteErrorIfNotOK_ (::sqlite3_exec (fDB_, "pragma journal_mode = 'delete';", nullptr, 0, &db_err));
                break;
            case JournalModeType::eTruncate:
                ThrowSQLiteErrorIfNotOK_ (::sqlite3_exec (fDB_, "pragma journal_mode = 'truncate';", nullptr, 0, &db_err));
                break;
            case JournalModeType::ePersist:
                ThrowSQLiteErrorIfNotOK_ (::sqlite3_exec (fDB_, "pragma journal_mode = 'persist';", nullptr, 0, &db_err));
                break;
            case JournalModeType::eMemory:
                ThrowSQLiteErrorIfNotOK_ (::sqlite3_exec (fDB_, "pragma journal_mode = 'memory';", nullptr, 0, &db_err));
                break;
            case JournalModeType::eWAL:
                ThrowSQLiteErrorIfNotOK_ (::sqlite3_exec (fDB_, "pragma journal_mode = 'wal';", nullptr, 0, &db_err));
                break;
            case JournalModeType::eWAL2:
                if (GetJournalMode () == JournalModeType::eWAL) {
                    ThrowSQLiteErrorIfNotOK_ (::sqlite3_exec (fDB_, "pragma journal_mode = 'delete';", nullptr, 0, &db_err));
                }
                ThrowSQLiteErrorIfNotOK_ (::sqlite3_exec (fDB_, "pragma journal_mode = 'wal2';", nullptr, 0, &db_err));
                break;
            case JournalModeType::eOff:
                ThrowSQLiteErrorIfNotOK_ (::sqlite3_exec (fDB_, "pragma journal_mode = 'off';", nullptr, 0, &db_err));
                break;
        }
    }

    ::sqlite3* fDB_{};
};

/*
 ********************************************************************************
 *********************** SQL::SQLite::Connection::Ptr ***************************
 ********************************************************************************
 */
SQL::SQLite::Connection::Ptr::Ptr (const shared_ptr<IRep>& src)
    : inherited{src}
    , pBusyTimeout{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
              const Ptr* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Ptr::pBusyTimeout);
              RequireNotNull (thisObj->operator-> ());
              return thisObj->operator-> ()->GetBusyTimeout ();
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, auto timeout) {
              Ptr* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Ptr::pBusyTimeout);
              RequireNotNull (thisObj->operator-> ());
              thisObj->operator-> ()->SetBusyTimeout (timeout);
          }}
    , pJournalMode{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
              const Ptr* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Ptr::pJournalMode);
              RequireNotNull (thisObj->operator-> ());
              return thisObj->operator-> ()->GetJournalMode ();
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, auto journalMode) {
              Ptr* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Ptr::pJournalMode);
              RequireNotNull (thisObj->operator-> ());
              thisObj->operator-> ()->SetJournalMode (journalMode);
          }}
{
}

/*
 ********************************************************************************
 ************************** SQL::SQLite::Connection *****************************
 ********************************************************************************
 */
auto SQL::SQLite::Connection::New (const Options& options) -> Ptr
{
    return Ptr{make_shared<Rep_> (options)};
}

/*
 ********************************************************************************
 ******************************* SQLite::Statement ******************************
 ********************************************************************************
 */
struct Statement::MyRep_ : IRep {
    MyRep_ (const Connection::Ptr& db, const String& query)
        : fConnectionPtr_{db}
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        TraceContextBumper ctx{"SQLite::Statement::MyRep_::CTOR", Stroika_Foundation_Debug_OptionalizeTraceArgs (L "db=%p, query='%s'", db, query.c_str ())};
#endif
        RequireNotNull (db);
        RequireNotNull (db->Peek ());
#if qDebug
        _fSharedContext = fConnectionPtr_.GetSharedContext ();
#endif
        string                                                     queryUTF8 = query.AsUTF8 ();
        lock_guard<const Debug::AssertExternallySynchronizedMutex> critSec{*this};
        const char*                                                pzTail = nullptr;
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
    ~MyRep_ ()
    {
        lock_guard<const Debug::AssertExternallySynchronizedMutex> critSec{*this};
        AssertNotNull (fStatementObj_);
        (void)::sqlite3_finalize (fStatementObj_);
    }
    virtual String GetSQL (WhichSQLFlag whichSQL) const override
    {
        shared_lock<const Debug::AssertExternallySynchronizedMutex> critSec{*this};
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
// This ifdef should NOT be needed (because if constexpr should prevent it from being evaluated), but
// that doesn't appear to work on MSVC 2k19 - and for now - more of a PITA than its worth for bug define
#ifdef SQLITE_ENABLE_NORMALIZE
                    return String::FromUTF8 (::sqlite3_normalized_sql (fStatementObj_));
#endif
                }
                RequireNotReached ();
                return String{};
            default:
                RequireNotReached ();
                return String{};
        }
    }
    virtual Sequence<ColumnDescription> GetColumns () const override
    {
        shared_lock<const Debug::AssertExternallySynchronizedMutex> critSec{*this};
        return Sequence<ColumnDescription>{fColumns_};
    };
    virtual Sequence<ParameterDescription> GetParameters () const override
    {
        shared_lock<const Debug::AssertExternallySynchronizedMutex> critSec{*this};
        return fParameters_;
    };
    virtual void Bind () override
    {
        lock_guard<const Debug::AssertExternallySynchronizedMutex> critSec{*this};
        ThrowSQLiteErrorIfNotOK_ (::sqlite3_clear_bindings (fStatementObj_));
    }
    virtual void Bind (unsigned int parameterIndex, const VariantValue& v) override
    {
        lock_guard<const Debug::AssertExternallySynchronizedMutex> critSec{*this};
        fParameters_[parameterIndex].fValue = v;
        switch (v.GetType ()) {
            case VariantValue::eDate:
            case VariantValue::eDateTime:
            case VariantValue::eString: {
                string u = v.As<String> ().AsUTF8 ();
                ThrowSQLiteErrorIfNotOK_ (::sqlite3_bind_text (fStatementObj_, parameterIndex + 1, u.c_str (), static_cast<int> (u.length ()), SQLITE_TRANSIENT), fConnectionPtr_->Peek ());
            } break;
            case VariantValue::eBoolean:
            case VariantValue::eInteger:
                ThrowSQLiteErrorIfNotOK_ (::sqlite3_bind_int64 (fStatementObj_, parameterIndex + 1, v.As<sqlite3_int64> ()), fConnectionPtr_->Peek ());
                break;
            case VariantValue::eFloat:
                ThrowSQLiteErrorIfNotOK_ (::sqlite3_bind_double (fStatementObj_, parameterIndex + 1, v.As<double> ()), fConnectionPtr_->Peek ());
                break;
            case VariantValue::eBLOB: {
                Memory::BLOB b = v.As<Memory::BLOB> ();
                ThrowSQLiteErrorIfNotOK_ (::sqlite3_bind_blob64 (fStatementObj_, parameterIndex + 1, b.begin (), b.size (), SQLITE_TRANSIENT), fConnectionPtr_->Peek ());
            } break;
            case VariantValue::eNull:
                ThrowSQLiteErrorIfNotOK_ (::sqlite3_bind_null (fStatementObj_, parameterIndex + 1), fConnectionPtr_->Peek ());
                break;
            default:
                AssertNotImplemented (); // add more types
                break;
        }
    }
    virtual void Bind (const String& parameterName, const VariantValue& v) override
    {
        Require (not parameterName.empty ());
        lock_guard<const Debug::AssertExternallySynchronizedMutex> critSec{*this};
        String                                                     pn = parameterName;
        if (pn[0] != ':') {
            pn = L":" + pn;
        }
        for (unsigned int i = 0; i < fParameters_.length (); ++i) {
            if (fParameters_[i].fName == pn) {
                Bind (i, v);
                return;
            }
        }
        DbgTrace (L"Statement::Bind: Parameter '%s' not found in list %s", parameterName.c_str (), Characters::ToString (fParameters_.Select<String> ([] (const auto& i) { return i.fName; })).c_str ());
        RequireNotReached (); // invalid paramter name provided
    }
    virtual void Reset () override
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        TraceContextBumper ctx{"SQLite::Statement::MyRep_::Statement::Reset"};
#endif
        lock_guard<const Debug::AssertExternallySynchronizedMutex> critSec{*this};
        AssertNotNull (fStatementObj_);
        ThrowSQLiteErrorIfNotOK_ (::sqlite3_reset (fStatementObj_), fConnectionPtr_->Peek ());
    }
    virtual optional<Row> GetNextRow () override
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        TraceContextBumper ctx{"SQLite::Statement::MyRep_::Statement::GetNextRow"};
#endif
        lock_guard<const Debug::AssertExternallySynchronizedMutex> critSec{*this};
        // @todo MAYBE redo with https://www.sqlite.org/c3ref/value.html
        AssertNotNull (fStatementObj_);
        int rc = ::sqlite3_step (fStatementObj_);
        switch (rc) {
            case SQLITE_OK: {
                AssertNotReached (); // I think this should never happen with this API
            } break;
            case SQLITE_ROW: {
                Row row;
                for (unsigned int i = 0; i < fColumns_.size (); ++i) {
                    VariantValue v;
                    // The actual returned type may not be the same as the DECLARED type (for example if the column is declared variant)
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
            } break;
            case SQLITE_DONE: {
                return nullopt;
            } break;
        }
        ThrowSQLiteError_ (rc, fConnectionPtr_->Peek ());
    }

    Connection::Ptr                fConnectionPtr_;
    ::sqlite3_stmt*                fStatementObj_;
    vector<ColumnDescription>      fColumns_;
    Sequence<ParameterDescription> fParameters_;
};

Statement::Statement (const Connection::Ptr& db, const String& query)
    : inherited{make_unique<MyRep_> (db, query)}
{
}

/*
 ********************************************************************************
 ******************************* SQLite::Transaction ****************************
 ********************************************************************************
 */
struct Transaction::MyRep_ : public SQL::Transaction::IRep {
    MyRep_ (const Connection::Ptr& db, Flag f)
        : fConnectionPtr_{db}
    {
        switch (f) {
            case Flag::eDeferred:
                db->Exec (L"BEGIN DEFERRED TRANSACTION;"sv);
                break;
            case Flag::eExclusive:
                db->Exec (L"BEGIN EXCLUSIVE TRANSACTION;"sv);
                break;
            case Flag::eImmediate:
                db->Exec (L"BEGIN IMMEDIATE TRANSACTION;"sv);
                break;
            default:
                RequireNotReached ();
        }
    }
    virtual void Commit () override
    {
        Require (not fCompleted_);
        fCompleted_ = true;
        fConnectionPtr_->Exec (L"COMMIT TRANSACTION;"sv);
    }
    virtual void Rollback () override
    {
        Require (not fCompleted_);
        fCompleted_ = true;
        fConnectionPtr_->Exec (L"ROLLBACK TRANSACTION;"sv);
    }
    virtual Disposition GetDisposition () const override
    {
        // @todo record more info so we can report finer grained status ; try/catch in rollback/commit and dbgtraces
        return fCompleted_ ? Disposition::eCompleted : Disposition::eNone;
    }
    Connection::Ptr fConnectionPtr_;
    bool            fCompleted_{false};
};
Transaction::Transaction (const Connection::Ptr& db, Flag f)
    : inherited{make_unique<MyRep_> (db, f)}
{
}
#endif
