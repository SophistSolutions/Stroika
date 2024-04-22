/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

#include "Stroika/Foundation/Debug/Cast.h"

namespace Stroika::Foundation::Database::SQL::ODBC {

#if qHasLibrary_ODBC

    /*
     ********************************************************************************
     ****************************** ODBC::Connection::Ptr ***************************
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
#endif

}
