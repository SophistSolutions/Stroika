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
#include "../Common/Property.h"
#include "../Containers/Mapping.h"
#include "../Containers/Sequence.h"
#include "../DataExchange/VariantValue.h"
#include "../Debug/AssertExternallySynchronizedLock.h"
#include "../IO/Network/URI.h"
#include "../Time/Duration.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Early</a>
 *
 *  TODO
 *      @todo   Create SQLite Exception class and use sqlite3_errstr () to generate good string 
 *              message (that seems to return threadafe static const strings)
 */

namespace Stroika::Foundation::Database::SQLite {

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

    class Statement;
    class Transaction;

    /**
     *  'Connection' is a quasi-namespace.
     */
    class Connection {
    public:
        /**
         */
        class IRep;

    public:
        /**
         */
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
        static Ptr New (
            const Options& options, const function<void (const Connection::Ptr&)>& dbInitializer = [] (const Connection::Ptr&) {});
    };

    /**
     *  Connection provides an API for accessing an SQLite database.
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
    class Connection::IRep : protected Debug::AssertExternallySynchronizedLock {
    public:
        /**
         */
        virtual ~IRep () = default;

    public:
        /**
         *  This returns nothing, but raises exceptions on errors.
         */
        virtual void Exec (const String& sql) = 0;

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
    class Connection::Ptr : private Debug::AssertExternallySynchronizedLock {
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
         *  This returns nothing, but raises exceptions on errors.
         *
         *  \todo - EXTEND this to write the RESPONSE (use the callback) to DbgTrace () calls - perhaps optionally?)
         */
        nonvirtual void Exec (const String& sql) const;

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

    public:
        nonvirtual auto operator== (const Ptr& rhs) const;
        nonvirtual bool operator== (nullptr_t) const noexcept;
#if __cpp_impl_three_way_comparison < 201907
        nonvirtual bool operator!= (const Ptr& rhs) const;
        nonvirtual bool operator!= (nullptr_t) const;
#endif

    private:
        shared_ptr<IRep> fRep_;

    private:
        friend class Statement;
    };

    /**
     *  \note - Design Note - we use String for the result-column-name - and could use int or Atom. But
     *        String slightly simpler, and nearly as performant, so going with that for now.
     *
     *  \todo   CONSIDER redo Row as iterator; or maybe GetREsults () method that returns iterable of Rows? and lazy pulls them?
     * 
     *  Unlike a Connection::Ptr, a Statement cannot be copied (though you can use shared_ptr<Statement> if you wish to copy them).
     */
    class Statement : private Debug::AssertExternallySynchronizedLock {
    public:
        /**
         */
        Statement () = delete;
        Statement (const Connection::Ptr& db, const String& query);
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
         *  Could use 'int' index for this and faster, but tracking names maybe slightly better for readability and logging,
         *  and given that string shared_ptr, not too bad to copy around/compare
         *  COULD use Atom<> - which would be a good performance/funcitonality compromise, but would want to use private atomMgr,
         *  so scope of names limited (else list of interned strings could become large). Not worth the efforts/risk for small benefit for now.
         */
        using ColumnName = String;

    public:
        /**
         */
        using Row = Mapping<ColumnName, VariantValue>;

    public:
        /**
         *  This mimics the sqlite3_reset () functionality/API.
         * 
         *  This clears any ongoing query, so the next call to GetNextRow () will start with the first row from the current query.
         *  This does NOT affect any values currently bound.
         * 
         *  BUT NOTE, it is required to call Reset() (if there are any ongoing queries) before calling Bind.
         */
        nonvirtual void Reset ();

    public:
        /**
         *  If called on a new Statement, or on a statement that has been reset (since the last call to GetNextRow() - this re-runs the query.
         *  But either way, it returns the next row.
         *
         * returns 'missing' on EOF, exception on error
         */
        nonvirtual optional<Row> GetNextRow ();

    public:
        /**
         *  \brief - Call GetNextRow () repeatedly, and accumulate Rows into a Sequence (@see GetAllRows ())
         * 
         *  The overloads that take a sequence of column numbers return each row as a tuple of columns (VariantValue)
         *  for just the specified column numbers.
         * 
         * ... @todo use variadic templates to generatelas GetAllRows()
         * ... @todo COULD overload so columns named by 'name' instead of index, but simple to use index (as specified by result of
         *           GetColumns ()
         */
        nonvirtual Sequence<Row> GetAllRemainingRows ();
        nonvirtual Sequence<VariantValue> GetAllRemainingRows (size_t restrictToColumn);
        nonvirtual Sequence<tuple<VariantValue, VariantValue>> GetAllRemainingRows (size_t restrictToColumn1, size_t restrictToColumn2);
        nonvirtual Sequence<tuple<VariantValue, VariantValue, VariantValue>> GetAllRemainingRows (size_t restrictToColumn1, size_t restrictToColumn2, size_t restrictToColumn3);

    public:
        /**
         *  \brief - call Reset (), and then GetAllRemainingRows () - which always starts current statement with current bindings from the beginning.
         * 
         *  The overloads that take a sequence of column numbers return each row as a tuple of columns (VariantValue)
         *  for just the specified column numbers.
         * 
         * ... @todo use variadic templates to generatelas GetAllRows()
         * ... @todo COULD overload so columns named by 'name' instead of index, but simple to use index (as specified by result of
         *           GetColumns ()
         */
        nonvirtual Sequence<Row> GetAllRows ();
        nonvirtual Sequence<VariantValue> GetAllRows (size_t restrictToColumn);
        nonvirtual Sequence<tuple<VariantValue, VariantValue>> GetAllRows (size_t restrictToColumn1, size_t restrictToColumn2);
        nonvirtual Sequence<tuple<VariantValue, VariantValue, VariantValue>> GetAllRows (size_t restrictToColumn1, size_t restrictToColumn2, size_t restrictToColumn3);

    public:
        enum class WhichSQLFlag {
            /**
             *  This is the original SQL passed in as argument to the statement.
             */
            eOriginal,

            /**
             * string containing the SQL text of prepared statement P with [bound parameters] expanded
             */
            eExpanded,

            /**
             * This option is available iff CompiledOptions::kThe.ENABLE_NORMALIZE
             */
            eNormalized
        };

    public:
        /**
         *  Return the original (or normalized or expanded with bindings) SQL associated with this statement.
         */
        nonvirtual String GetSQL (WhichSQLFlag whichSQL = WhichSQLFlag::eOriginal) const;

    public:
        /**
         *  This describes an SQL column. The 'type' is a string (and optional at that), and refers to the SQLite type system.
         */
        struct ColumnDescription {
            /**
             */
            ColumnName fName;

            /**
             * sqlite3_column_decltype
             */
            optional<String> fType;

            /**
             *  @see Characters::ToString ()
             */
            nonvirtual String ToString () const;
        };

    public:
        /**
         *  \note the types returned in .fType are generally wrong until we've run our first query).
         * 
         * @ todo consider rename to GetResultColumns
         */
        nonvirtual Sequence<ColumnDescription> GetColumns () const;

    public:
        /**
         *  Parameters are set with a call to "Bind" so maybe also called parameter bindings.
         */
        struct ParameterDescription {
            /**
             *  name is optional - bindings can be specified by index
             */
            optional<String> fName;

            /**
             */
            VariantValue fValue;

            /**
             *  @see Characters::ToString ()
             */
            nonvirtual String ToString () const;
        };

    public:
        /**
         *  Gets the names and values of all the current parameters to this sql statement.
         * 
         *  \see Bind ()
         */
        nonvirtual Sequence<ParameterDescription> GetParameters () const;

    public:
        /**
         *  Specify one, or a collection of parameter bindings. This never changes the order of the bindings, just
         *  applies them to the appropriate binding elements.
         * 
         *  \note the paramterIndex is 'zero-based' unlike sqlite native APIs
         *
         *  \req parameterIndex < GetParameters ().length ()
         *  \req paramterName FOUND in GetParameters ().fName's
         *  and similarly for other overloads
         *
         *  \note - parameterName can be the name of the variable with or without the prefixing :
         *
         *  If iterable argument to Bind (), the if the arguments have parameter names, the association will be done by name.
         *  if they do not have names, the order in the bind argument list will be interpretted as argument order (parameter order) for that item)
         * 
         *  \see GetParameters ()
         */
        nonvirtual void Bind (unsigned int parameterIndex, const VariantValue& v);
        nonvirtual void Bind (const String& parameterName, const VariantValue& v);
        nonvirtual void Bind (const Traversal::Iterable<ParameterDescription>& parameters);
        nonvirtual void Bind (const Traversal::Iterable<Common::KeyValuePair<String, VariantValue>>& parameters);

    public:
        /**
         *  Execute the given statement, and ignore its result value. Do NOT mix Execute() with GetNextRow() or GetAllRows ().
         *  It is legal to call this on an SQL statement that returns results, but you will not see the results.
         * 
         *  Execute () with a list of ParameterDescriptions is like:
         *      >   Reset
         *      >   Bind () with that list of parameters and then  (can be empty list/missing)
         *      >   Execute ()
         * 
         *  No need to call Reset() before calling Execute () as it calls it internally.
         */
        nonvirtual void Execute ();
        nonvirtual void Execute (const Traversal::Iterable<ParameterDescription>& parameters);
        nonvirtual void Execute (const Traversal::Iterable<Common::KeyValuePair<String, VariantValue>>& parameters);

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual String ToString () const;

    private:
        Connection::Ptr                fConnectionPtr_;
        ::sqlite3_stmt*                fStatementObj_;
        vector<ColumnDescription>      fColumns_;
        Sequence<ParameterDescription> fParameters_;
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
    class Transaction : private Debug::AssertExternallySynchronizedLock {
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

    public:
        /**
         */
        ~Transaction ();

    public:
        /**
         */
        nonvirtual Transaction& operator= (const Transaction&) = delete;

    public:
        /**
         *  Cause the transaction to end successfully, flushing to the database.
         *  It is (an assertion) error to call this multiple times. And calling OMITTING
         *  a call before the destructor causes the transaction to Rollback.
         */
        nonvirtual void Commit ();

    public:
        /**
         *  This cannot be called after a rollback or commit.
         * 
         *  This causes no data to be written for the commands already issued in the transaction.
         * 
         *  This is equivilent to just destroying this object, except that it can propagate
         *  exceptions if needed, whereas a destructor cannot.
         */
        nonvirtual void Rollback ();

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual String ToString () const;

    private:
        Connection::Ptr fConnectionPtr_;
        bool            fCompleted_{false};
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
