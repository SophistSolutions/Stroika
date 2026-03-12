/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_NullLock_h_
#define _Stroika_Foundation_Execution_NullLock_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <mutex>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/StdCompat.h"

/*
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Execution {

    /**
     *  This class follows the Mutex concept - syntactically - but doesn't actually perform locking.
     *
     *  Sometimes you want to write code that can use locks or not. Use of the NullLock allows the syntax of
     *  locking to be present, but effectively all compiled (even constexpr) away.
     * 
     *  \note before Stroika v3.0d23 - this was called NullMutex
     */
    struct NullLock {
        constexpr void lock () const;
        constexpr bool try_lock () const;
        constexpr void unlock () const;
        constexpr void lock_shared () const;
        constexpr bool try_lock_shared () const;
        constexpr void unlock_shared () const;
    };
    static_assert (Common::StdCompat::Lockable<NullLock>);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "NullLock.inl"

#endif /*_Stroika_Foundation_Execution_NullLock_h_*/
