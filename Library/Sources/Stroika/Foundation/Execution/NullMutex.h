/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_NullMutex_h_
#define _Stroika_Foundation_Execution_NullMutex_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <mutex>

#include "Stroika/Foundation/Common/Common.h"

/*
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 * 
 *      @todo see concepts and add static_assert (ILockable - see diff lockable concepts and doc which and enforce wtih statis assert)
 */

namespace Stroika::Foundation::Execution {

    /**
     *  This class follows the Mutex concept - syntactically - but doesn't actually perform locking.
     *
     *  Sometimes you want to write code that can use locks or not. Use of the NullMutex allows the syntax of
     *  locking to be present, but effectively all compiled (even constexpr) away.
     */
    struct NullMutex {
        constexpr void lock () const
        {
        }
        constexpr bool try_lock () const
        {
            return true;
        }
        constexpr void unlock () const
        {
        }
        constexpr void lock_shared () const
        {
        }
        constexpr bool try_lock_shared () const
        {
            return true;
        }
        constexpr void unlock_shared () const
        {
        }
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "NullMutex.inl"

#endif /*_Stroika_Foundation_Execution_NullMutex_h_*/
