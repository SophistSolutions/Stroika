/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_SQL_SQLite_h_
#define _Stroika_Foundation_Database_SQL_SQLite_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <filesystem>
#include <optional>

#if qHasFeature_sqlite
#include <sqlite/sqlite3.h>
#endif

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/DataExchange/VariantValue.h"
#include "Stroika/Foundation/Database/SQL/Connection.h"
#include "Stroika/Foundation/Database/SQL/Statement.h"
#include "Stroika/Foundation/Database/SQL/Transaction.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/IO/Network/URI.h"
#include "Stroika/Foundation/Time/Duration.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO
 *      @todo   Create SQLite Exception class and use sqlite3_errstr () to generate good string 
 *              message (that seems to return threadsafe static const strings)
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
     *  https://www.sqlite.org/pragma.html#pragma_journal_mode
     * 
     *  In my experience, it appears WAL provides the best performance, for multithreaded applications.
     *      \see https://sqlite.org/wal.html
     *              "WAL provides more concurrency as readers do not block writers and a writer"
     *              "does not block readers. Reading and writing can proceed concurrently."
     */
    enum JournalModeType {
        eDelete,
        eTruncate,
        ePersist,
        eMemory,
        eWAL,
        eWAL2,
        eOff
    };

    class Statement;

    /**
     *  \brief SQLite::Connection namespace contains SQL::Connection::Ptr subclass, specific to SQLite, and ::New function factory.
     */
    namespace Connection {

        using namespace SQL::Connection;

        class IRep;

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
                 *  (Stroika Debug::AssertExternallySynchronizedMutex enforces this)
                 * 
                 * This may not always be available depending on how sqlite was compiled, but we dont have access to SQLITE_THREADSAFE at compile time
                 * (since just defined in C file from Stroika/ThirdPartyComponents/sqlite/Makefile);
                 * call sqlite3_threadsafe, to see if this is enabled
                 */
                eMultiThread,

                /**
                 *  SQLITE_OPEN_FULLMUTEX
                 *  In serialized mode, SQLite can be safely used by multiple threads with no restriction.
                 *  (note even in this mode, each connection is Debug::AssertExternallySynchronizedMutex)
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

            /**
             *  \note - see JournalModeType and Connection::Ptr::journalMode
             */
            optional<JournalModeType> fJournalMode;
        };

        /**
         *  Connection provides an API for accessing an SQLite database.
         *
         *  A new Connection::Ptr is typically created SQLite::Connection::New()
         *
         *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
         *          But though each connection can only be accessed from a single thread at a time, the underlying database may be
         *          threadsafe (even if accessed across processes) - depending on its construction OPtions::ThreadSafety
         *
         *          The Connection itself is standardC++ thread safety. The thread-safety of the underlying database depends on the setting
         *          of Options::fThreadingMode when the database is constructed.
         * 
         *          @see https://www.sqlite.org/threadsafe.html
         *          We set SQLITE_OPEN_NOMUTEX on open (so mode Multi-thread, but not Serialized).
         * 
         *          NOTE - two Connection::Ptr objects refering to the same underlying REP is NOT (probably) safe with SQLITE. But referring
         *          to the same database is safe.
         *
         */
        class Ptr : public SQL::Connection::Ptr {
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
            nonvirtual IRep* operator->() const noexcept;

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
            Common::Property<Duration> busyTimeout;

        public:
            /**
             *  This can significantly affect database performance, and reliability.
             */
            Common::Property<JournalModeType> journalMode;

        private:
            friend class Statement;
        };

        /**
         *  \brief create an SQLite database connection object, guided by argument Options.
         */
        Ptr New (const Options& options);

        /**
         *  Connection provides an API for accessing an SQLite database.
         * 
         *  Typically don't use this directly, but use Connection::Ptr, a smart ptr wrapper on this interface.
         *
         *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
         *          But though each connection can only be accessed from a single thread at a time, the underlying database may be
         *          threadsafe (even if accessed across processes) - depending on its construction OPtions::ThreadSafety
         *
         *          The Connection itself is standardC++ thread safety. The thread-safety of the underlying database depends on the setting
         *          of Options::fThreadingMode when the database is constructed.
         * 
         *          @see https://www.sqlite.org/threadsafe.html
         *          We set SQLITE_OPEN_NOMUTEX on open (so mode Multi-thread, but not Serialized).
         * 
         *          NOTE ALSO - its POSSIBLE we could lift this Debug::AssertExternallySynchronizedMutex code / restriction.
         *          But sqlite docs not super clear. Maybe I need to use thier locking APIs myself internally to use
         *          those locks to make a sequence of bindings safe? But for now just don't assume this is threadsafe and we'll be OK.
         */
        class IRep : public SQL::Connection::IRep {
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

        public:
            /**
             */
            virtual JournalModeType GetJournalMode () const = 0;

        public:
            /**
             */
            virtual void SetJournalMode (JournalModeType journalMode) = 0;

        public:
            [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fAssertExternallySynchronizedMutex;

        private:
            friend class Ptr;
        };

    }

    /**
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     *          See notes about thread safety for Connection::Ptr - since this copies around a Connection::Ptr.
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
             *  database while the transaction is underway.
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
