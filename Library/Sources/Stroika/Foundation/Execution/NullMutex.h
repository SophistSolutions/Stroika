/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_NullMutex_h_
#define _Stroika_Foundation_Execution_NullMutex_h_ 1

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
     *  Sometimes you want to write code that can use locks or not. Use of the NullMutex allows the syntax of
     *  locking to be present, but effectively all compiled (even constexpr) away.
     */
    struct NullMutex {
        constexpr void lock () const;
        constexpr bool try_lock () const;
        constexpr void unlock () const;
        constexpr void lock_shared () const;
        constexpr bool try_lock_shared () const;
        constexpr void unlock_shared () const;
    };
    static_assert (Common::StdCompat::Lockable<NullMutex>);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "NullMutex.inl"

#endif /*_Stroika_Foundation_Execution_NullMutex_h_*/
