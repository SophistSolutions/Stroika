/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <system_error>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Time/Duration.h"

#include "SQLite.h"

using namespace Stroika::Foundation;

using namespace Characters;
using namespace Containers;
using namespace Debug;
using namespace Database;
using namespace Database::Document::SQLite;
using namespace Execution;
using namespace Time;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

#if qStroika_HasComponent_sqlite && defined(_MSC_VER)
// Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#pragma comment(lib, "sqlite.lib")
#endif

using Database::Document::EngineProperties;

#if qStroika_HasComponent_sqlite
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
                DbgTrace ("SQLITE_BUSY"_f); //  The database file is locked
                Throw (system_error{make_error_code (errc::device_or_resource_busy)});
            } break;
            case SQLITE_LOCKED: {
                DbgTrace ("SQLITE_LOCKED"_f); //  A table in the database is locked
                Throw (system_error{make_error_code (errc::device_or_resource_busy)});
            } break;
            case SQLITE_CONSTRAINT: {
                if (errMsgDetails) {
                    Throw (Exception{"SQLITE_CONSTRAINT: {}"_f(errMsgDetails)});
                }
                else {
                    static const auto kEx_ = Exception{"SQLITE_CONSTRAINT"sv};
                    Throw (kEx_);
                }
            } break;
            case SQLITE_TOOBIG: {
                static const auto kEx_ = Exception{"SQLITE_TOOBIG"sv};
                Throw (kEx_);
            } break;
            case SQLITE_FULL: {
                DbgTrace ("SQLITE_FULL"_f);
                Throw (system_error{make_error_code (errc::no_space_on_device)});
            } break;
            case SQLITE_READONLY: {
                static const auto kEx_ = Exception{"SQLITE_READONLY"sv};
                Throw (kEx_);
            } break;
            case SQLITE_MISUSE: {
                if (errMsgDetails) {
                    Throw (Exception{"SQLITE_MISUSE: {}"_f(errMsgDetails)});
                }
                else {
                    static const auto kEx_ = Exception{"SQLITE_MISUSE"sv};
                    Throw (kEx_);
                }
            } break;
            case SQLITE_ERROR: {
                if (errMsgDetails) {
                    Throw (Exception{"SQLITE_ERROR: {}"_f(errMsgDetails)});
                }
                else {
                    static const auto kEx_ = Exception{"SQLITE_ERROR"sv};
                    Throw (kEx_);
                }
            } break;
            case SQLITE_NOMEM: {
                DbgTrace ("SQLITE_NOMEM translated to bad_alloc"_f);
                Throw (bad_alloc{});
            } break;
        }
        if (errMsgDetails) {
            Throw (Exception{"SQLite Error: {} (code {})"_f(errMsgDetails, errCode)});
        }
        else {
            Throw (Exception{"SQLite Error: {}"_f(errCode)});
        }
    }
    void ThrowSQLiteErrorIfNotOK_ (int errCode, sqlite3* sqliteConnection = nullptr)
    {
        static_assert (SQLITE_OK == 0);
        if (errCode != SQLITE_OK) [[unlikely]] {
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
            Assert (::sqlite3_compileoption_used ("ENABLE_JSON1"));
#else
            Assert (!::sqlite3_compileoption_used ("OMIT_JSON1"));
#endif
        }
    } sVerifyFlags_;
}

namespace {
    using Connection::Options;
    struct Rep_ final : Database::Document::SQLite::Connection::IRep, private Debug::AssertExternallySynchronizedMutex {
        Rep_ (const Options& options)
        {
            TraceContextBumper ctx{"SQLite::Connection::Rep_::Rep_"};

            int flags = 0;
            if (options.fThreadingMode) {
                // https://www.sqlite.org/threadsafe.html expalins the threadsafety stuff. Not sure I have it right, but hopefully --LGP 2023-09-13
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
            if constexpr (qStroika_Foundation_Debug_AssertionsChecked) {
                [[maybe_unused]] int n{};
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
                    if ((e = ::sqlite3_open_v2 (uriArg.c_str (), &fDB_, SQLITE_OPEN_CREATE | flags,
                                                options.fVFS ? options.fVFS->AsNarrowSDKString ().c_str () : nullptr)) == SQLITE_OK) {
                        fTmpHackCreated_ = true;
                    }
                }
            }
            if (e != SQLITE_OK) [[unlikely]] {
                [[maybe_unused]] auto&& cleanup = Finally ([this] () noexcept {
                    if (fDB_ != nullptr) {
                        Verify (::sqlite3_close (fDB_) == SQLITE_OK);
                    }
                });
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
                    return "SQLite"sv;
                }
            };
            static const shared_ptr<const EngineProperties> kProps_ = make_shared<const MyEngineProperties_> ();
            return kProps_;
        }
        virtual Set<String> GetCollections () override
        {
            return {};
        }
        virtual void CreateCollection (const String& name) override
        {
            AssertNotImplemented ();
        }
        virtual void DropCollection (const String& name) override
        {
            AssertNotImplemented ();
        }
        virtual Document::Collection::Ptr GetCollection (const String& name) override
        {
            return Document::Collection::Ptr{nullptr}; // @todo - implement this!
        }
        virtual Document::Transaction mkTransaction () override

        {
            Connection::Ptr conn = Connection::Ptr{Debug::UncheckedDynamicPointerCast<Connection::IRep> (shared_from_this ())};
            return Database::Document::SQLite::Transaction{conn};
        }
        virtual void Exec (const String& sql) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
            [[maybe_unused]] char* db_err{}; // could use but its embedded in the fDB_ error string anyhow, and thats already peeked at by ThrowSQLiteErrorIfNotOK_ and it generates better exceptions (maps some to std c++ exceptions)
            int e = ::sqlite3_exec (fDB_, sql.AsUTF8<string> ().c_str (), NULL, 0, &db_err);
            if (e != SQLITE_OK) [[unlikely]] {
                ThrowSQLiteErrorIfNotOK_ (e, fDB_);
            }
        }
        virtual ::sqlite3* Peek () override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this}; // not super helpful, but could catch errors - reason not very helpful is we lose lock long before we stop using ptr
            return fDB_;
        }
        virtual Duration GetBusyTimeout () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
            optional<int>                                         d;
            auto callback = [] (void* lamdaArg, [[maybe_unused]] int argc, char** argv, [[maybe_unused]] char** azColName) {
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
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
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
            if (d == "delete"sv) {
                return JournalModeType::eDelete;
            }
            if (d == "truncate"sv) {
                return JournalModeType::eTruncate;
            }
            if (d == "persist"sv) {
                return JournalModeType::ePersist;
            }
            if (d == "memory"sv) {
                return JournalModeType::eMemory;
            }
            if (d == "wal"sv) {
                return JournalModeType::eWAL;
            }
            if (d == "wal2"sv) {
                return JournalModeType::eWAL2;
            }
            if (d == "off"sv) {
                return JournalModeType::eOff;
            }
            AssertNotReached ();
            return JournalModeType::eDelete;
        }
        virtual void SetJournalMode (JournalModeType journalMode) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
            [[maybe_unused]] char*                                 db_err{};
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
}

/*
 ********************************************************************************
 *********************** SQL::SQLite::Connection::Ptr ***************************
 ********************************************************************************
 */
Document::SQLite::Connection::Ptr::Ptr (const shared_ptr<IRep>& src)
    : inherited{src}
    , busyTimeout{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
                      const Ptr* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Ptr::busyTimeout);
                      RequireNotNull (thisObj->operator->());
                      return thisObj->operator->()->GetBusyTimeout ();
                  },
                  [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, auto timeout) {
                      Ptr* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Ptr::busyTimeout);
                      RequireNotNull (thisObj->operator->());
                      thisObj->operator->()->SetBusyTimeout (timeout);
                  }}
    , journalMode{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
                      const Ptr* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Ptr::journalMode);
                      RequireNotNull (thisObj->operator->());
                      return thisObj->operator->()->GetJournalMode ();
                  },
                  [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, auto journalMode) {
                      Ptr* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Ptr::journalMode);
                      RequireNotNull (thisObj->operator->());
                      thisObj->operator->()->SetJournalMode (journalMode);
                  }}
{
#if qStroika_Foundation_Debug_AssertExternallySynchronizedMutex_Enabled
    if (src != nullptr) {
        fAssertExternallySynchronizedMutex.SetAssertExternallySynchronizedMutexContext (src->fAssertExternallySynchronizedMutex.GetSharedContext ());
    }
#endif
}

/*
 ********************************************************************************
 ************************** SQL::SQLite::Connection *****************************
 ********************************************************************************
 */
auto Document::SQLite::Connection::New (const Options& options) -> Ptr
{
    return Ptr{make_shared<Rep_> (options)};
}

/*
 ********************************************************************************
 ******************************* SQLite::Transaction ****************************
 ********************************************************************************
 */
struct Transaction::MyRep_ : public Database::Document::Transaction::IRep {
    MyRep_ (const Connection::Ptr& db, Flag f)
        : fConnectionPtr_{db}
    {
        switch (f) {
            case Flag::eDeferred:
                db->Exec ("BEGIN DEFERRED TRANSACTION;"sv);
                break;
            case Flag::eExclusive:
                db->Exec ("BEGIN EXCLUSIVE TRANSACTION;"sv);
                break;
            case Flag::eImmediate:
                db->Exec ("BEGIN IMMEDIATE TRANSACTION;"sv);
                break;
            default:
                RequireNotReached ();
        }
    }
    virtual void Commit () override
    {
        Require (not fCompleted_);
        fCompleted_ = true;
        fConnectionPtr_->Exec ("COMMIT TRANSACTION;"sv);
    }
    virtual void Rollback () override
    {
        Require (not fCompleted_);
        fCompleted_ = true;
        fConnectionPtr_->Exec ("ROLLBACK TRANSACTION;"sv);
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
