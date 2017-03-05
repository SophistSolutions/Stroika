
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Database_SQLite_inl_
#define _Stroika_Foundation_Database_SQLite_inl_ 1

namespace Stroika {
    namespace Foundation {
        namespace Database {
            namespace SQLite {

/*
                 ********************************************************************************
                 ****************************** SQLite::Connection ******************************
                 ********************************************************************************
                 */
#if qHasFeature_sqlite
                inline sqlite3* Connection::Peek ()
                {
                    return fDB_;
                }
#endif
            }
        }
    }
}
#endif /*_Stroika_Foundation_Database_SQLite_inl_*/
