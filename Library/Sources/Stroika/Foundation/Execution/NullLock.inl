/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ************************************* NullLock *********************************
     ********************************************************************************
     */
    constexpr void NullLock::lock () const
    {
    }
    constexpr bool NullLock::try_lock () const
    {
        return true;
    }
    constexpr void NullLock::unlock () const
    {
    }
    constexpr void NullLock::lock_shared () const
    {
    }
    constexpr bool NullLock::try_lock_shared () const
    {
        return true;
    }
    constexpr void NullLock::unlock_shared () const
    {
    }

}
