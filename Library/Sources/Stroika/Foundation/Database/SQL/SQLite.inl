
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Database_SQL_SQLite_inl_
#define _Stroika_Foundation_Database_SQL_SQLite_inl_ 1

namespace Stroika::Foundation::Database::SQL::SQLite {

#if qHasFeature_sqlite
    /*
     ********************************************************************************
     ****************************** SQLite::CompiledOptions *************************
     ********************************************************************************
     */
    inline constexpr CompiledOptions CompiledOptions::kThe
    {
#if __cpp_designated_initializers
#ifdef SQLITE_ENABLE_NORMALIZE
        .ENABLE_NORMALIZE = true
#else
        .ENABLE_NORMALIZE = false
#endif
#ifdef SQLITE_THREADSAFE
            ,
        .THREADSAFE = SQLITE_THREADSAFE
#elif defined(THREADSAFE)
            ,
        .THREADSAFE = THREADSAFE
#else
        , .THREADSAFE = 1
#endif
            ,
        .ENABLE_JSON1 = true

#else
#ifdef SQLITE_ENABLE_NORMALIZE
        , SQLITE_ENABLE_NORMALIZE
#else
        false
#endif
#ifdef SQLITE_THREADSAFE
            ,
            SQLITE_THREADSAFE
#elif defined(THREADSAFE)
            ,
            THREADSAFE
#else
        , 1
#endif
            ,
            true
#endif
    };

    /*
     ********************************************************************************
     ****************************** SQLite::Connection::Ptr *************************
     ********************************************************************************
     */
    inline Connection::Ptr::Ptr (const Ptr& src)
        : Ptr{dynamic_pointer_cast<IRep> (src._fRep)}
    {
    }
    inline Connection::Ptr& Connection::Ptr::operator= (const Ptr& src)
    {
        inherited::operator= (src);
        return *this;
    }
    inline Connection::Ptr& Connection::Ptr::operator= (Ptr&& src) noexcept
    {
        inherited::operator= (move (src));
        return *this;
    }
    inline Connection::IRep* Connection::Ptr::operator-> () const noexcept
    {
        return dynamic_pointer_cast<IRep> (_fRep).get ();
    }
    inline ::sqlite3* Connection::Ptr::Peek () const
    {
        return dynamic_pointer_cast<IRep> (_fRep)->Peek ();
    }
#endif

}

#endif /*_Stroika_Foundation_Database_SQL_SQLite_inl_*/
