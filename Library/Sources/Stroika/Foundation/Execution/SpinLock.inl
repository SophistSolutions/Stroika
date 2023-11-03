/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_SpinLock_inl_
#define _Stroika_Foundation_Execution_SpinLock_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include <thread>

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ******************************** Execution::SpinLock ***************************
     ********************************************************************************
     */
    inline SpinLock::SpinLock (BarrierType barrier)
        : fBarrierFlag_{barrier}
    {
    }
    inline bool SpinLock::try_lock ()
    {
        /*
         *  NOTE: I don't understand why memory_order_acquire is good enough here since if
         *  we make change we need to make sure its published to other threads.
         *
         *  But - the example on http://en.cppreference.com/w/cpp/atomic/atomic_flag_test_and_set shows
         *      while (atomic_flag_test_and_set_explicit (&lock, memory_order_acquire))
         *          ; // spin until the lock is acquired
         */
        bool result = not fLock_.test_and_set (memory_order_acquire);
        if (result) {
            DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wtsan\"");   // Needed in g++-13 and later, it appears, with --sanitize=thread, but so far no obvious real problems
            /*
             *  See https://stroika.atlassian.net/browse/STK-494 for notes on why this is right (using eReleaseAcquire/memory_order_acquire)
             */
            switch (fBarrierFlag_) {
                case BarrierType::eReleaseAcquire:
                    atomic_thread_fence (memory_order_acquire);
                    break;
                case BarrierType::eMemoryTotalOrder:
                    atomic_thread_fence (memory_order_seq_cst);
                    break;
                default:
                    break;
            }
            DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wtsan\"");
        }
        return result;
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
        // See notes in try_lock () for cooresponding thread_fence calls()
        switch (fBarrierFlag_) {
            case BarrierType::eReleaseAcquire:
                atomic_thread_fence (memory_order_release);
                break;
            case BarrierType::eMemoryTotalOrder:
                atomic_thread_fence (memory_order_seq_cst);
                break;
            default:
                break;
        }
        // release lock
        fLock_.clear (memory_order_release);
    }

}

#endif /*_Stroika_Foundation_Execution_SpinLock_inl_*/
