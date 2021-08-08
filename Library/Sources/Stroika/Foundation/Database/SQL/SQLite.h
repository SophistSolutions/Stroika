/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_SQL_SQLite_h_
#define _Stroika_Foundation_Database_SQL_SQLite_h_ 1

#include "../../StroikaPreComp.h"

#include <filesystem>
#include <optional>

#if qHasFeature_sqlite
#include <sqlite/sqlite3.h>
#endif

#include "../../Characters/String.h"
#include "../../Common/Property.h"
#include "../../Containers/Mapping.h"
#include "../../Containers/Sequence.h"
#include "../../DataExchange/VariantValue.h"
#include "../../Debug/AssertExternallySynchronizedLock.h"
#include "../../IO/Network/URI.h"
#include "../../Time/Duration.h"

#include "Connection.h"
#include "Statement.h"
#include "Transaction.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Early</a>
 *
 *  TODO
 *      @todo   Create SQLite Exception class and use sqlite3_errstr () to generate good string 
 *              message (that seems to return threadafe static const strings)
 */

namespace Stroika::Foundation::Database::SQL::SQLite {

    using Characters::String;
    using Containers::Mapping;
    using Containers::Sequence;
    using DataExchange::VariantValue;
    using IO::Network::URI;
    using Time::Duration;

#if qHasFeature_sqlite

    /**
     *  This defines what options sqlite was compiled with.
     * 
     *  For a full list of possible options, see <https://www.sqlite.org/compile.html>
     *  (though we only capture a limited subset of these). To check the rest, callers
     *  can use ::sqlite3_compileoption_used ()
     * 
     *  Fields correspond to names looked up with ::sqlite3_compileoption_used () - only this is constexpr (and an incomplete replica).
     *  This is checked to correspond to the sqlite3_compileoption_used() values at startup with assertions.
     * 
     *      \todo   Find a better way to define! We want this to be available as a constexpr object. But the defines 
     *              are just done in the .c file that gets defined and the API to lookup values is very non-constexpr.
     * 
     *              So instead we take a good guess at the values (based on defaults and #defines done in this file)
     *              and check with asserts we got the right value.
     */
    class CompiledOptions final {
    public:
        /**
         *  c++ #define SQLITE_ENABLE_NORMALIZE (not in docs file but does appear as a compile time option - we need to check)
         */
        bool ENABLE_NORMALIZE;

        /**
         *  SQLITE_THREADSAFE = 0, 1, 2 (0 means no)
         */
        uint8_t THREADSAFE;

        /**
         *  SQLITE_ENABLE_JSON1 = true (for now required)
         */
        bool ENABLE_JSON1;

        /**
         *  Defined constexpr
         */
        static const CompiledOptions kThe;
    };

    /**
     *  These are options used to create a database Connection::Ptr object (with Connection::New).
     *
     *  Since this is also how you create a database, in a sense, its those options too.
     */
    struct Options final {
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
         *  for sharing the same (in memory) database between different connections). If the name is the empty string (String{}) then
         *  it is guaranteed unique.
         * 
         *  \note - fInMemoryDB and fDBPath and fTemporaryDB are mutually exclusive options.
         */
        optional<String> fInMemoryDB;

        /**
         *  @see https://www.sqlite.org/compile.html#threadsafe
         * 
         *  Note this refers to the threading mode for the underlying database. A Connection object is always single-threaded/externally
         *  synchronized.
         */
        enum class ThreadingMode {
            /**
             *   SQLITE_OPEN_FULLMUTEX
             *  In this mode, all mutexes are disabled and SQLite is unsafe to use in more than a single thread at once
             */
            eSingleThread,

            /**
             *  SQLITE_OPEN_NOMUTEX
             *  In this mode, SQLite can be safely used by multiple threads provided that no single database connection is used simultaneously in two or more threads.
             *  (Stroika Debug::AssertExternallySynchronizedLock enforces this)
             * 
             * This may not always be available depending on how sqlite was compiled, but we dont have access to SQLITE_THREADSAFE at compile time
             * (since just defined in C file from Stroika/ThirdPartyComponents/sqlite/Makefile);
             * call sqlite3_threadsafe, to see if this is enabled
             */
            eMultiThread,

            /**
             *  SQLITE_OPEN_FULLMUTEX
             *  In serialized mode, SQLite can be safely used by multiple threads with no restriction.
             *  (note even in this mode, each connection is Debug::AssertExternallySynchronizedLock)
             * 
             * This may not always be available depending on how sqlite was compiled, but we dont have access to SQLITE_THREADSAFE at compile time
             * (since just defined in C file from Stroika/ThirdPartyComponents/sqlite/Makefile);
             * call sqlite3_threadsafe, to see if this is enabled
             * 
             *  \note Use of this API, as of Stroika 2.1b12, may result in poor error messages, due to how errors are stored (and maybe other such
             *        issues - maybe we need to do lock around call to each function to avoid making this mode nearly pointless).
             */
            eSerialized,
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

        /**
         *  This is only useful if the database can be opened by multiple threads of control (multiple threads with connections
         *  within the same app, or multiple applications).
         */
        optional<Duration> fBusyTimeout;
    };

    /**
     *  'Connection' is a quasi-namespace.
     */
    class Connection : SQL::Connection {
    public:
        class IRep;

    public:
        class Ptr;

    private:
        struct Rep_;

    public:
        /**
         *  Quasi-namespace class - don't construct - construct a Connection:::Ptr class with Connection::New ()
         */
        Connection () = delete;

    public:
        /**
         *  The dbInitializer is called IFF the New () call results in a newly created database (@todo RECONSIDER).
         */
        static Ptr                                                                       New (const Options& options);
        [[deprecated ("Since Stroika 2.1b13, use ORM::ProvisionForVersion")]] static Ptr New (const Options& options, const function<void (const Connection::Ptr&)>& dbInitializer);
    };

    /**
     *  Connection provides an API for accessing an SQLite database.
     * 
     *  Typically don't use this directly, but use Connecion::Ptr, a smart ptr wrapper on this interface.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter/a>
     *          But though each connection can only be accessed from a single thread at a time, the underlying database may be
     *          threadsafe (even if accessed across processes) - depending on its construction OPtions::ThreadSafety
     *
     *          The Connection itself is standardC++ thread safety. The thread-safety of the underlying database depends on the setting
     *          of Options::fThreadingMode when the database is constructed.
     * 
     *          @see https://www.sqlite.org/threadsafe.html
     *          We set SQLITE_OPEN_NOMUTEX on open (so mode Multi-thread, but not Serialized).
     */
    class Connection::IRep : public SQL::Connection::IRep {
    public:
        /**
         *  Use of Peek () is discouraged, and unsafe, but allowed for now because we don't have a full wrapper on the sqlite API.
         */
        virtual ::sqlite3* Peek () = 0;

    public:
        /**
         *  Fetched dynamically with pragma busy_timeout;
         */
        virtual Duration GetBusyTimeout () const = 0;

    public:
        /**
         *  \req timeout >= 0
         */
        virtual void SetBusyTimeout (const Duration& timeout) = 0;

    private:
        friend class Ptr;
    };

    /**
     *  Connection provides an API for accessing an SQLite database.
     *
     *  A new Connection::Ptr is typically created SQLite::Connection::New()
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter/a>
     *          But though each connection can only be accessed from a single thread at a time, the underlying database may be
     *          threadsafe (even if accessed across processes) - depending on its construction OPtions::ThreadSafety
     *
     *          The Connection itself is standardC++ thread safety. The thread-safety of the underlying database depends on the setting
     *          of Options::fThreadingMode when the database is constructed.
     * 
     *          @see https://www.sqlite.org/threadsafe.html
     *          We set SQLITE_OPEN_NOMUTEX on open (so mode Multi-thread, but not Serialized).
     *
     */
    class Connection::Ptr : public SQL::Connection::Ptr {
    private:
        using inherited = SQL::Connection::Ptr;

    public:
        /**
         */
        Ptr (const Ptr& src);
        Ptr (const shared_ptr<IRep>& src = nullptr);

    public:
        ~Ptr () = default;

    public:
        /**
         */
        nonvirtual Ptr& operator= (const Ptr& src);
        nonvirtual Ptr& operator= (Ptr&& src) noexcept;

    public:
        /**
         */
        nonvirtual IRep* operator-> () const noexcept;

    public:
        /**
         *  Use of Peek () is discouraged, and unsafe, but allowed for now because we don't have a full wrapper on the sqlite API.
         */
        nonvirtual ::sqlite3* Peek () const;

    public:
        /**
         *  When doing a query that would have failed due to SQL_BUSY timeout, sqlite will wait
         *  and retry up to this long, to avoid the timeout.
         */
        Common::Property<Duration> pBusyTimeout;
    };

    /**
     */
    class Statement : public SQL::Statement {
    private:
        using inherited = SQL::Statement;

    public:
        /**
         */
        Statement () = delete;
        Statement (const Connection::Ptr& db, const String& query);
        Statement (const Statement&) = delete;

    private:
        struct MyRep_;
    };

    /**
     *  \see https://www.sqlite.org/lang_transaction.html
     * 
     *  \note Transactions are not required. This is for explicit transactions. If you omit
     *        using transactions, sqlite creates mini transactions automatically for each statement.
     *
     *  \note Nested transactions not supported
     * 
     *  \todo Consider supporting SQLITE SAVEPOINT (like nested transaction)
     */
    class Transaction : public SQL::Transaction {
    private:
        using inherited = SQL::Transaction;

    public:
        enum Flag {
            /**
             *  Don't really start the transaction until the command to read/update the database
             */
            eDeferred,
            /**
             *  Start writing to the DB immediately (as of the transaction start); note this affects when you might
             *  get SQL_BUSY errors.
             */
            eImmediate,

            /**
             *  Depends on WAL mode, but generally prevents other database connections from reading the
             *  database while the transaction is underway
             */
            eExclusive,

            eDEFAULT = eDeferred
        };

    public:
        /**
         */
        Transaction () = delete;
        Transaction (const Connection::Ptr& db, Flag f = Flag::eDEFAULT);
        Transaction (const Transaction&) = delete;

    private:
        struct MyRep_;
    };
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SQLite.inl"

#endif /*_Stroika_Foundation_Database_SQL_SQLite_h_*/
