/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_SQLite_h_
#define _Stroika_Foundation_Database_SQLite_h_  1

#include    "../StroikaPreComp.h"

#if     qHasFeature_sqlite
#include    <sqlite/sqlite3.h>
#endif

#include    "../Characters/String.h"
#include    "../Containers/Mapping.h"
#include    "../Containers/Sequence.h"
#include    "../DataExchange/VariantValue.h"
#include    "../IO/Network/URL.h"
#include    "../Memory/Optional.h"


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Early</a>
 *
 *  TODO
 *      @todo   Consider how we handle Threads. One possability is to wrap our object with
 *              DebugAssertExtenrallySYnchonized and define the SQLITE_THREADSAFE=0 flag when we build
 *              sqlite3.  Another possabiliyt is to let it handle the threadsafety, and in our wrapper
 *              code provide thread safety if we find SQLITE_THREADSAFE is not defined.
 *
 *              Very unsure best way.
 *
 *      @todo   Add REQUIRE statements on DB::Statement that only one statement is passed in.
 *
 *      @todo   Add DB::Statements object - like DB::Statement - but which allows for multiple statements, and just combines the results.
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Database {
            namespace   SQLite {


                using   Characters::String;
                using   Containers::Mapping;
                using   Containers::Sequence;
                using   DataExchange::VariantValue;
                using   Memory::Optional;
                using   IO::Network::URL;


                /**
                 *  @todo - probably move to common area - for all DB stuff - not specific to SQLite (maybe have folder for "SQL")
                 */
                String  QuoteStringForDB (const String& s);


#if     qHasFeature_sqlite
                /**
                 */
                class   DB {
                public:
                    enum InMemoryDBFlag {
                        eInMemoryDB,
                    };
                public:
                    /**
                     */
                    DB (const URL& dbURL, const function<void(DB&)>& dbInitializer = [] (DB&) {});
                    DB (const String& dbPath, const function<void(DB&)>& dbInitializer = [] (DB&) {});
                    DB (InMemoryDBFlag memoryDBFlag, const function<void(DB&)>& dbInitializer = [] (DB&) {});
                    DB (const DB&) = delete;

                public:
                    DB& operator= (const DB&) = delete;

                public:
                    ~DB ();

                public:
                    class Statement;

                public:
                    /**
                     *  This returns nothing, but raises exceptions on errors.
                     *
                     *  \todo - EXTEND this to write the RESPONSE (use the callback) to DbgTrace () calls - perhaps optionally?)
                     */
                    nonvirtual  void    Exec (const wchar_t* formatCmd2Exec, ...);

                public:
                    /**
                     */
                    nonvirtual  sqlite3*    Peek ();

                private:
                    sqlite3* fDB_ {};
                };


                /**
                 *  \note - for now - this only supports a SINGLE STATEMENT at a time. BUt if you give more than one, the subsequent ones are ignored.
                 *          Obviously that sucks, and needs work - @todo
                 */
                class DB::Statement {
                public:
                    Statement (DB* db, const wchar_t* formatQuery, ...);
                    Statement (sqlite3* db,  const wchar_t* formatQuery, ...);
                    ~Statement ();

                public:
                    // redo as iterator
                    using   RowType = Mapping<String, VariantValue>;
                    /// returns 'missing' on EOF, exception on error
                    nonvirtual  Optional<RowType>   GetNextRow ();

                private:
                    sqlite3_stmt*       fStatementObj_;
                    unsigned int        fParamsCount_;
                    Sequence<String>    fColNames_;
                };
#endif


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "SQLite.inl"

#endif  /*_Stroika_Foundation_Database_SQLite_h_*/
