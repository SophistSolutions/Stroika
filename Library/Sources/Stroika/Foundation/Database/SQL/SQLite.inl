/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

#include "Stroika/Foundation/Debug/Cast.h"

namespace Stroika::Foundation::Database::SQL::SQLite {

#if qStroika_HasComponent_sqlite
    /*
     ********************************************************************************
     ****************************** SQLite::CompiledOptions *************************
     ********************************************************************************
     */
    inline constexpr CompiledOptions CompiledOptions::kThe{
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
        .ENABLE_JSON1 = true};

    /*
     ********************************************************************************
     ****************************** SQLite::Connection::Ptr *************************
     ********************************************************************************
     */
    inline Connection::Ptr::Ptr (const Ptr& src)
        : Ptr{Debug::UncheckedDynamicPointerCast<IRep> (src._fRep)}
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
    inline Connection::IRep* Connection::Ptr::operator->() const noexcept
    {
        return Debug::UncheckedDynamicPointerCast<IRep> (_fRep).get ();
    }
    inline ::sqlite3* Connection::Ptr::Peek () const
    {
        return Debug::UncheckedDynamicPointerCast<IRep> (_fRep)->Peek ();
    }
#endif

}
