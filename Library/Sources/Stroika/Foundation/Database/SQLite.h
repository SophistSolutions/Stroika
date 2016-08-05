/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_SQLite_h_
#define _Stroika_Foundation_Database_SQLite_h_  1

#include    "../StroikaPreComp.h"

#if     qHasFeature_sqlite
#include    "sqlite3.h"
#endif

/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Database {
            namespace   SQLite {


                using   Characters::SDKString;
                using   Characters::SDKChar;

#if     qHasFeature_sqlite
                class DB {
                public:
                    using   ExperimentID    =   uint64_t;
                public:
                    using   ScanID  =   uint64_t;
                public:
                    DB (const String& experimentDBFullPath);
                    DB (const DB&) = delete;
                    DB& operator= (const DB&) = delete;
                    ~DB ();

                public:
                    class Statement;

                public:
                    void    Exec (const String& cmd2Exec);

                private:
                    sqlite3* fDB_ {};
                };



                class DB::Statement {
                public:
                    Statement (sqlite3* db, const String& query);
                    ~Statement_ ();

                public:
                    // redo as iterator
                    using   RowType = Mapping<String, VariantValue>;
                    /// returns 'missing' on EOF, exception on error
                    Optional<RowType>   GetNextRow ();


                private:
                    sqlite3_stmt* stmt;

                    size_t  nParams;
                    Sequence<String>    fColNames;
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
