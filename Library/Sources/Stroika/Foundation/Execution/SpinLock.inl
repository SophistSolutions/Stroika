/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include <thread>

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ******************************** Execution::SpinLock ***************************
     ********************************************************************************
     */
    inline bool SpinLock::try_lock ()
    {
        /*
         *  Acquire on the flag itself is all this does, and all a lock needs.
         *
         *  When test_and_set () returns false we took the lock, so this read-modify-write read the value written
         *  by the previous holder's unlock () - a release store. An acquire that reads from a release store
         *  synchronizes-with it, making everything the previous holder did before unlocking happen-before
         *  everything this thread does after locking. That is exactly what the data under the lock needs, and a
         *  separate fence would add nothing to it.
         *  @see https://github.com/SophistSolutions/Stroika/issues/628 (STK-494), where that was settled.
         *
         *  \note   A FAILED try_lock () acquires nothing, so it needs no ordering at all - but atomic_flag has no
         *          separate failure order (unlike compare_exchange_xxx), so the acquire is paid either way. Free
         *          on x86-64, not on weakly ordered hardware. Avoiding it means spinning on a relaxed test () and
         *          only attempting this RMW once the flag reads clear, which would also stop the spin dirtying
         *          the cache line every iteration. Not done here: it changes the contended path, so it wants its
         *          own measurement.
         */
        return not fLock_.test_and_set (memory_order_acquire);
    }
    inline void SpinLock::lock ()
    {
        // Acquire lock. If / when fails, yield processor to avoid too much busy waiting.
        while (not try_lock ()) {
            this_thread::yield (); // nb: until Stroika v2.0a209, this called Execution::Yield (), making this a cancelation point. That is principally bad because it makes SpinLock not interchangeable with mutex
        }
    }
    inline void SpinLock::unlock ()
    {
        // Release on the flag itself - this is the store the next locker's acquire reads. @see try_lock ()
        fLock_.clear (memory_order_release);
    }

}
