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
#include    "../Memory/Optional.h"

/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Early</a>
 *
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Database {
            namespace   SQLite {


                using   Characters::String;
                using   Containers::Mapping;
                using   Containers::Sequence;
                using   Memory::Optional;


#if     qHasFeature_sqlite
                /**
                 */
                class   DB {
                public:
                    DB (const String& experimentDBFullPath);
                    DB (const DB&) = delete;
                    DB& operator= (const DB&) = delete;
                    ~DB ();

                public:
                    class Statement;

                public:
                    nonvirtual  void    Exec (const String& cmd2Exec);

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
