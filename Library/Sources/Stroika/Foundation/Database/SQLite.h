/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_SQLite_h_
#define _Stroika_Foundation_Database_SQLite_h_ 1

#include "../StroikaPreComp.h"

#include <optional>

#if qHasFeature_sqlite
#include <sqlite/sqlite3.h>
#endif

#include "../Characters/String.h"
#include "../Containers/Mapping.h"
#include "../Containers/Sequence.h"
#include "../DataExchange/VariantValue.h"
#include "../Debug/AssertExternallySynchronizedLock.h"
#include "../IO/Network/URL.h"

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

namespace Stroika::Foundation {
    namespace Database {
        namespace SQLite {

            using Characters::String;
            using Containers::Mapping;
            using Containers::Sequence;
            using DataExchange::VariantValue;
            using IO::Network::URL;

            /**
             *  @todo - probably move to common area - for all DB stuff - not specific to SQLite (maybe have folder for "SQL")
             */
            String QuoteStringForDB (const String& s);

#if qHasFeature_sqlite
            /**
             *  Connection provides an API for accessing an SQLite database.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
             *          But though each connection can only be accessed from a single thread at a time, the underlying database is
             *          threadsafe (even if accessed across processes)
             *
             *          @see https://www.sqlite.org/threadsafe.html
             *          We set SQLITE_OPEN_NOMUTEX on open (so mode Multi-thread, but not Serialized).
             */
            class Connection : private Debug::AssertExternallySynchronizedLock {
            public:
                enum InMemoryDBFlag {
                    eInMemoryDB,
                };

            public:
                /**
                 */
                Connection () = delete;
                Connection (const URL& dbURL, const function<void(Connection&)>& dbInitializer = [](Connection&) {});
                Connection (const String& dbPath, const function<void(Connection&)>& dbInitializer = [](Connection&) {});
                Connection (InMemoryDBFlag memoryDBFlag, const function<void(Connection&)>& dbInitializer = [](Connection&) {});
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
                class Statement;

            public:
                /**
                 *  This returns nothing, but raises exceptions on errors.
                 *
                 *  \todo - EXTEND this to write the RESPONSE (use the callback) to DbgTrace () calls - perhaps optionally?)
                 */
                nonvirtual void Exec (const wchar_t* formatCmd2Exec, ...);

            public:
                /**
                 *  Use of Peek () is discouraged, and unsafe, but allowed for now because we dont have a full wrapper on the sqlite API.
                 */
                nonvirtual sqlite3* Peek ();

            private:
                sqlite3* fDB_{};
            };

            /**
             *  \note - for now - this only supports a SINGLE STATEMENT at a time. But if you give more than one, the subsequent ones are ignored.
             *          Obviously that sucks, and needs work - @todo
             */
            class Connection::Statement {
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
                lock_guard<const AssertExternallySynchronizedLock> fConnectionCritSec_;
                sqlite3_stmt*                                      fStatementObj_;
                unsigned int                                       fParamsCount_;
                Sequence<String>                                   fColNames_;
            };
#endif
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SQLite.inl"

#endif /*_Stroika_Foundation_Database_SQLite_h_*/
