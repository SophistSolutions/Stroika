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
             *  \note Use of this API, as of Stroika 2.1b12, may result in poor error messages, due to how errors are stored (and maybe othe rsuch
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
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *                              with caveats!
     *
     *          The Connection itself is standardC++ thread safety. The thread-safety of the underlying database depends on the setting
     *          of Options::fThreadingMode when the database is constructed.
     * 
     * &&& REDO THE DOCS HERE ONCE I MOVE TO IRep and Ptr...
     */
    class Connection {
    public:
        struct IRep : Debug::AssertExternallySynchronizedLock {
            virtual ~IRep () = default;

            /**
             *  This returns nothing, but raises exceptions on errors.
             */
            virtual void Exec (const String& sql) = 0;

            /**
             *  Use of Peek () is discouraged, and unsafe, but allowed for now because we don't have a full wrapper on the sqlite API.
             */
            virtual ::sqlite3* Peek () = 0;
        };

    public:
        class Ptr {
        public:
            Ptr () = default;
            Ptr (const Ptr& src) = default;
            Ptr (Ptr&& src)      = default;
            Ptr (const shared_ptr<IRep>& src)
                : fRep_{src}
            {
            }
            Ptr& operator= (const Ptr& src) = default;
            Ptr& operator= (Ptr&& src) = default;

            IRep* operator-> () const noexcept
            {
                return fRep_.get ();
            }

            auto operator== (const Ptr& rhs) const
            {
                return fRep_ == rhs.fRep_;
            }
            bool operator== (nullptr_t) const noexcept
            {
                return fRep_.get () == nullptr;
            }

            /**
             *  This returns nothing, but raises exceptions on errors.
             *
             *  \todo - EXTEND this to write the RESPONSE (use the callback) to DbgTrace () calls - perhaps optionally?)
             */
            nonvirtual void Exec (const wchar_t* formatCmd2Exec, ...)
            {
                RequireNotNull (formatCmd2Exec);
                va_list argsList;
                va_start (argsList, formatCmd2Exec);
                String cmd2Exec = Characters::FormatV (formatCmd2Exec, argsList);
                va_end (argsList);
                fRep_->Exec (cmd2Exec);
            }

            /**
             *  Use of Peek () is discouraged, and unsafe, but allowed for now because we don't have a full wrapper on the sqlite API.
             */
            nonvirtual ::sqlite3* Peek ()
            {
                return fRep_->Peek ();
            }

        private:
            shared_ptr<IRep> fRep_;
        };

    private:
        struct Rep_ final : IRep {
            Rep_ (const Options& options);
            ~Rep_ ();
            bool               fTmpHackCreated_{false};
            virtual void       Exec (const String& sql) override;
            virtual ::sqlite3* Peek () override;
            ::sqlite3*         fDB_{};
        };

    public:
        /**
         *  Quasi-namespace class - don't construct - construct a Connection:::Ptr class with Connection::New ()
         */
        Connection () = delete;

    public:
        /**
         */
        static Ptr New (const Options& options, const function<void (const Connection::Ptr&)>& dbInitializer = [] (const Connection::Ptr&) {});

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
        Statement (const Connection::Ptr& db, const wchar_t* formatQuery, ...);
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
         * returns 'missing' on EOF, exception on error
         */
        nonvirtual optional<Row> GetNextRow ();

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
         *  This describes an SQL column. THe 'type' is a string (and optional at that), and refers to the SQLite type system.
         */
        struct ColumnDescription {
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
         */
        nonvirtual Sequence<ColumnDescription> GetColumns () const;

    public:
        /**
         *  Parameters are set with a call to "Bind" so maybe also called parameter bindings.
         */
        struct ParameterDescription {
            optional<String> fName; // name is optional - bindings can be specified by index
            VariantValue     fValue;

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
         *  \req paramterName FOUND in GetParaemters ().fName's
         *  and similarly for other overloads
         * 
         *  If iterable argument to Bind (), the if the arguments have parameter names, the association will be done by name.
         *  if they do not have names, the order in the bind argument list will be interpretted as argument order (parameter order) for that item)
         * 
         *  \see GetParameters ()
         */
        nonvirtual void Bind (unsigned int parameterIndex, const VariantValue& v);
        nonvirtual void Bind (const String& parameterName, const VariantValue& v);
        nonvirtual void Bind (const Traversal::Iterable<ParameterDescription>& parameters);

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
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SQLite.inl"

#endif /*_Stroika_Foundation_Database_SQLite_h_*/
