/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_SQLite_h_
#define _Stroika_Foundation_Database_SQLite_h_ 1

#include "../StroikaPreComp.h"

#include <filesystem>
#include <optional>

#if qHasFeature_sqlite
#include <sqlite/sqlite3.h>
#endif

#include "../Characters/String.h"
#include "../Containers/Mapping.h"
#include "../Containers/Sequence.h"
#include "../DataExchange/VariantValue.h"
#include "../Debug/AssertExternallySynchronizedLock.h"
#include "../IO/Network/URI.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Early</a>
 *
 *  TODO
 *      @todo   Add REQUIRE statements on DB::Statement that only one statement is passed in.
 *
 *      @todo   Add DB::Statements object - like DB::Statement - but which allows for multiple statements, and just combines the results.
 *
 *      @todo   Create SQLite Exception class and use sqlite3_errstr () to generate good string message (that seems to return threadafe static const strings)
 */

namespace Stroika::Foundation::Database::SQLite {

    using Characters::String;
    using Containers::Mapping;
    using Containers::Sequence;
    using DataExchange::VariantValue;
    using IO::Network::URI;

    /**
     *  @todo - probably move to common area - for all DB stuff - not specific to SQLite (maybe have folder for "SQL")
     */
    String QuoteStringForDB (const String& s);

#if qHasFeature_sqlite
    /**
     */
    struct Options {
        /**
         *  NOTE - we choose to only support a PATH, and not the URI syntax, because the URI syntax is used to pass
         *  extra parameters (as from a GUI) and those can conflict with what is specified here (making it unclear or
         *  surprising how to interpret). @todo perhaps provide an API to 'parse' an sqlite URI into one of these Stroika
         *  SQLite options objects?
         * 
         *  \note - fInMemoryDB and fDBPath and fTemporaryDB are mutually exclusive options.
         */
        optional<filesystem::path> fDBPath;

        /**
         *  This option only applies if fDBPath is set. 
         *  \req fCreateDBPathIfDoesNotExist => not fReadOnly
         */
        bool fCreateDBPathIfDoesNotExist{true};

        /**
         *  fTemporaryDB is just like fInMemoryDB, except that it will be written to disk. But its like temporaryDB in that
         *  it will be automatically deleted when this connection (that created it) closes.
         * 
         *  \note - fInMemoryDB and fDBPath and fTemporaryDB are mutually exclusive options.
         */
        optional<String> fTemporaryDB;

        /**
         *  If provided, the database will not be stored to disk, but just saved in memory. The name still must be provided to allow
         *  for sharing the same (in memory) database between different connections).
         * 
         *  \note - fInMemoryDB and fDBPath and fTemporaryDB are mutually exclusive options.
         */
        optional<String> fInMemoryDB;

        /**
         *  @see https://www.sqlite.org/compile.html#threadsafe
         */
        enum class ThreadingMode {
#if SQLITE_THREADSAFE == 0
            /**
             *   SQLITE_OPEN_FULLMUTEX
             *  In this mode, all mutexes are disabled and SQLite is unsafe to use in more than a single thread at once
             */
            eSingleThread,
#endif
#if SQLITE_THREADSAFE != 0
            /**
             *  SQLITE_OPEN_NOMUTEX
             *  In this mode, SQLite can be safely used by multiple threads provided that no single database connection is used simultaneously in two or more threads.
             *  (Stroika Debug::AssertExternallySynchronizedLock enforces this)
             */
            eMultiThread,
            /**
             *  SQLITE_OPEN_FULLMUTEX
             *  In serialized mode, SQLite can be safely used by multiple threads with no restriction.
             *  (note even in this mode, each connection is Debug::AssertExternallySynchronizedLock)
             */
            eSerialized,
#endif
        };
        optional<ThreadingMode> fThreadingMode;

        /**
         *  This can generally be ignored, and primarily affects low level OS interface locking choices.
         *  @see https://www.sqlite.org/vfs.html
         */
        optional<String> fVFS;

        /**
         *  If a database is opened readonly, updates will fail, and if the database doesn't exist, it will not be automatically created.
         */
        bool fReadOnly{false};

        /**
         *  The immutable query parameter is a boolean that signals to SQLite that the underlying database file is held on read-only media and
         *  cannot be modified, even by another process with elevated privileges.
         * 
         *  \req fImmutable ==> fReadOnly
        */
        bool fImmutable{false};
    };

    class Statement;

    /**
     *  Connection provides an API for accessing an SQLite database.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *          But though each connection can only be accessed from a single thread at a time, the underlying database is
     *          threadsafe (even if accessed across processes)
     *
     *          @see https://www.sqlite.org/threadsafe.html
     *          We set SQLITE_OPEN_NOMUTEX on open (so mode Multi-thread, but not Serialized).
     */
    class Connection : private Debug::AssertExternallySynchronizedLock {
    public:
        /**
         */
        Connection () = delete;
        Connection (
            const Options& options, const function<void (Connection&)>& dbInitializer = [] (Connection&) {});
        Connection (const Connection&) = delete;

    public:
        /**
         */
        nonvirtual Connection& operator= (const Connection&) = delete;

    public:
        /**
         */
        ~Connection ();

    public:
        /**
         *  This returns nothing, but raises exceptions on errors.
         *
         *  \todo - EXTEND this to write the RESPONSE (use the callback) to DbgTrace () calls - perhaps optionally?)
         */
        nonvirtual void Exec (const wchar_t* formatCmd2Exec, ...);

    public:
        /**
         *  Use of Peek () is discouraged, and unsafe, but allowed for now because we don't have a full wrapper on the sqlite API.
         */
        nonvirtual sqlite3* Peek ();

    private:
        friend class Statement;

    private:
        sqlite3* fDB_{};
    };

    /**
     *  \note - for now - this only supports a SINGLE STATEMENT at a time. But if you give more than one, the subsequent ones are ignored.
     *          Obviously that sucks, and needs work - @todo
     */
    class Statement {
    public:
        /**
         */
        Statement () = delete;
        Statement (Connection* db, const wchar_t* formatQuery, ...);
        Statement (const Statement&) = delete;

    public:
        /**
         */
        ~Statement ();

    public:
        /**
         */
        nonvirtual Statement& operator= (const Statement&) = delete;

    public:
        /**
         * @todo redo as iterator
         */
        using RowType = Mapping<String, VariantValue>;

    public:
        /**
         * returns 'missing' on EOF, exception on error
         */
        nonvirtual optional<RowType> GetNextRow ();

    private:
        lock_guard<const Debug::AssertExternallySynchronizedLock> fConnectionCritSec_;
        sqlite3_stmt*                                             fStatementObj_;
        unsigned int                                              fParamsCount_;
        Sequence<String>                                          fColNames_;
    };
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SQLite.inl"

#endif /*_Stroika_Foundation_Database_SQLite_h_*/
