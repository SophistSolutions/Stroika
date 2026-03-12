/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ************************************ NullMutex *********************************
     ********************************************************************************
     */
    constexpr void NullMutex::lock () const
    {
    }
    constexpr bool NullMutex::try_lock () const
    {
        return true;
    }
    constexpr void NullMutex::unlock () const
    {
    }
    constexpr void NullMutex::lock_shared () const
    {
    }
    constexpr bool NullMutex::try_lock_shared () const
    {
        return true;
    }
    constexpr void NullMutex::unlock_shared () const
    {
    }

}
