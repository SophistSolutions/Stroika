
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Database_SQLite_inl_
#define _Stroika_Foundation_Database_SQLite_inl_ 1

namespace Stroika::Foundation {
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
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this}; // not super helpful, but could catch errors - reason not very helpful is we lose lock long before we stop using ptr
                return fDB_;
            }
#endif
        }
    }
}
#endif /*_Stroika_Foundation_Database_SQLite_inl_*/
