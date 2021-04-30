
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Database_SQLite_inl_
#define _Stroika_Foundation_Database_SQLite_inl_ 1

namespace Stroika::Foundation::Database::SQLite {

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
        , .THREADSAFE = SQLITE_THREADSAFE
    #elif defined (THREADSAFE)
        , .THREADSAFE = THREADSAFE
    #else
        , .THREADSAFE = 1
    #endif
#else
    #ifdef SQLITE_ENABLE_NORMALIZE
        , SQLITE_ENABLE_NORMALIZE
    #else
        false
    #endif
    #ifdef SQLITE_THREADSAFE
        , SQLITE_THREADSAFE
    #elif defined(THREADSAFE)
        , THREADSAFE
    #else
        , 1
    #endif
#endif
    };

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

    /*
     ********************************************************************************
     ****************************** SQLite::Statement *******************************
     ********************************************************************************
     */
    inline auto Statement::GetColumns () const -> Sequence<ColumnDescription>
    {
        return Sequence<ColumnDescription>{fColumns_};
    }
    inline auto Statement::GetParameters () const -> Sequence<ParameterDescription>
    {
        return fParameters_;
    }

}
#endif /*_Stroika_Foundation_Database_SQLite_inl_*/
