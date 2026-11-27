/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_Private_SQLiteLibrary_h_
#define _Stroika_Foundation_Database_Private_SQLiteLibrary_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>

#if qStroika_HasComponent_sqlite
#include <sqlite/sqlite3.h>
#endif

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

/**
 *  \file   Private to Stroika's SQLite modules - Database::SQL::SQLite and Database::Document::SQLite
 */

#if qStroika_HasComponent_sqlite
namespace Stroika::Foundation::Database::Private {

    /**
     *  The sqlite library's lifetime, from sqlite3_initialize to sqlite3_shutdown - which frees sqlite's own allocations, so
     *  leak checkers do not report them at exit. sqlite requires every connection closed before sqlite3_shutdown, and a
     *  connection held in a static closes during static destruction, in any order relative to Stroika's own statics. So
     *  each connection holds a reference to the one SQLiteLibrary for its whole life, as does a static until static
     *  destruction: whichever lets go last shuts sqlite down.
     */
    class SQLiteLibrary {
    public:
        SQLiteLibrary ()
        {
            Verify (::sqlite3_initialize () == SQLITE_OK);
        }
        SQLiteLibrary (const SQLiteLibrary&) = delete;
        ~SQLiteLibrary ()
        {
            Verify (::sqlite3_shutdown () == SQLITE_OK);
        }

    public:
        nonvirtual SQLiteLibrary& operator= (const SQLiteLibrary&) = delete;

    public:
        /**
         *  Held by each connection, as a member - so released only after its destructor's sqlite3_close.
         */
        static shared_ptr<SQLiteLibrary> Reference ()
        {
            static const shared_ptr<SQLiteLibrary> sThe_ = Memory::MakeSharedPtr<SQLiteLibrary> ();
            return sThe_;
        }
    };

}
#endif

#endif /*_Stroika_Foundation_Database_Private_SQLiteLibrary_h_*/
