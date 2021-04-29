
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
    constexpr CompiledOptions CompiledOptions::kThe
    {
#if __cpp_designated_initializers
        .ENABLE_NORMALIZE = defined (SQLITE_ENABLE_NORMALIZE)
#else
        defined (SQLITE_ENABLE_NORMALIZE)
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
