
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Database_SQLite_inl_
#define _Stroika_Foundation_Database_SQLite_inl_    1

namespace   Stroika {
    namespace   Foundation {
        namespace   Database {
            namespace   SQLite {


                /*
                 ********************************************************************************
                 *********************************** SQLite::DB *********************************
                 ********************************************************************************
                 */
                inline  sqlite3*    DB::Peek ()
                {
                    return fDB_;
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_Database_SQLite_inl_*/
