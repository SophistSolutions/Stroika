/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_AbortableMutex_h_
#define _Stroika_Foundation_Execution_AbortableMutex_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>
#include <mutex>

#include "Stroika/Foundation/Common/Common.h"

/*
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 *
 *
 * TODO:
 *      @todo   CONSIDER DEPRECATING - since totally unused as of 2024-06-24 - I think - and looks like a bad idea.
 * 
 *      @todo   http://stroika-bugs.sophists.com/browse/STK-600
 *              Rename this to InterruptibleMutex - and simple wrapper around any existing timed mutex, but with args
 *              saying time freq to check, and automatically so try with shorter timeouts.
 *
 *      @todo   TOTALLY UNTESTED
 *
 *      @todo   May need (want) to add try_lock etc (Lockable versus BasicLockable)
 *
 *      @todo   Initial implementation is inefficent (using timed_mutex). COULD do #ifdef based
 *              more efficient impls, directly using pthread_mutex (since it returns on EINTR) I think.
 *
 *              Not SURE how todo better with MSFT - but I'm sure they have some alertable API that
 *              comes down to a mutex (or close)
 *
 *              But this is a good enough start -- good enuf to test if this fixes any issues and is
 *              a reasonable placeholder.
 *
 *      @todo   Need AbortableRecursiveMutex, and possibly AbortableTimedMutex, and AbortableTimedRecursiveMutext.
 *
 *      @todo   Add regtests for this
 *
 *      @todo   make NOT copyable - /moveable etc - just like std::mutex (for docs - autodone by priovate fM_).
 *
 * Notes:
 *
 *
 *
 */

namespace Stroika::Foundation::Execution {

    /**
     *  This is equivalent to std::mutex, except that when a thread is blocked in a lock() call
     *  it can still be interrupted by Thread::Abort ().
     *
     *  This is not generally necessary for quick mutexes, but for any mutex use where you could
     *  block/lock for an extended time, it makes sense to use this instead. This is completely
     *  compatible with std::mutex otherwise, and can be used with std::lock_guard<> etc.
     */
    class AbortableMutex {
    public:
        /**
         *  \note   ***Cancelation Point***
         */
        nonvirtual void lock ();

        /**
         */
        nonvirtual void unlock ();

    private:
        timed_mutex fM_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "AbortableMutex.inl"

#endif /*_Stroika_Foundation_Execution_AbortableMutex_h_*/
