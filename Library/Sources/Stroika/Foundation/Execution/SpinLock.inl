/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_SpinLock_inl_
#define _Stroika_Foundation_Execution_SpinLock_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include <thread>

namespace Stroika::Foundation {
    namespace Execution {

        /*
         ********************************************************************************
         ******************************** Execution::SpinLock ***************************
         ********************************************************************************
         */
        inline SpinLock::SpinLock (BarrierFlag barrier)
            : fBarrierFlag_ (barrier)
        {
#if qStroika_FeatureSupported_Valgrind
            Stroika_Foundation_Debug_ValgrindDisableCheck_stdatomic (fLock_);
            VALGRIND_HG_MUTEX_INIT_POST (&fLock_, true);
#endif
        }
#if qStroika_FeatureSupported_Valgrind
        inline SpinLock::~SpinLock ()
        {
            VALGRIND_HG_MUTEX_DESTROY_PRE (&fLock_);
            VALGRIND_HG_ENABLE_CHECKING (&fLock_, sizeof (fLock_));
        }
#endif
        inline bool SpinLock::try_lock ()
        {
			// Atomically set fLock to true and examine the previous value. If it was false, we
			// successfully gained the lock. If it was already true (someone else holds the lock),
			// we did NOT gain the lock, but we changed nothing. The fact that we changed nothing in the case where
			// we didn't gain the lock, is why this is not a race.
#if qStroika_FeatureSupported_Valgrind
            VALGRIND_HG_MUTEX_LOCK_PRE (&fLock_, true);
#endif
            /*
             *  NOTE: I don't understand why memory_order_acquire is good enough here since if
             *  we make change we need to make sure its published to other threads.
             *
             *  But - the example on http://en.cppreference.com/w/cpp/atomic/atomic_flag_test_and_set shows
             *      while (std::atomic_flag_test_and_set_explicit (&lock, std::memory_order_acquire))
             *          ; // spin until the lock is acquired
             */
            bool result = not fLock_.test_and_set (std::memory_order_acquire);
#if qStroika_FeatureSupported_Valgrind
            if (result) {
                VALGRIND_HG_MUTEX_LOCK_POST (&fLock_);
            }
#endif
            if (result) {
                /*
                     *  See https://stroika.atlassian.net/browse/STK-494 for notes on why this is right (using eReleaseAcquire/memory_order_acquire)
                     */
                switch (fBarrierFlag_) {
                    case BarrierFlag::eReleaseAcquire:
                        std::atomic_thread_fence (std::memory_order_acquire);
                        break;
                    case BarrierFlag::eMemoryTotalOrder:
                        std::atomic_thread_fence (std::memory_order_seq_cst);
                        break;
                    default:
                        break;
                }
            }
            return result;
        }
        inline void SpinLock::lock ()
        {
            // Acquire lock. If / when fails, yield processor to avoid too much busy waiting.
            while (not try_lock ()) {
                std::this_thread::yield (); // nb: until Stroika v2.0a209, this called Execution::Yield (), making this a cancelation point. That is principally bad because it makes SpinLock not interchangeable with mutex
            }
        }
        inline void SpinLock::unlock ()
        {
            // See notes in try_lock () for cooresponding thread_fence calls()
            switch (fBarrierFlag_) {
                case BarrierFlag::eReleaseAcquire:
                    std::atomic_thread_fence (std::memory_order_release);
                    break;
                case BarrierFlag::eMemoryTotalOrder:
                    std::atomic_thread_fence (std::memory_order_seq_cst);
                    break;
                default:
                    break;
            }
                    // release lock
#if qStroika_FeatureSupported_Valgrind
            VALGRIND_HG_MUTEX_UNLOCK_PRE (&fLock_);
#endif
            fLock_.clear (std::memory_order_release);
#if qStroika_FeatureSupported_Valgrind
            VALGRIND_HG_MUTEX_UNLOCK_POST (&fLock_);
#endif
        }
    }
}
#endif /*_Stroika_Foundation_Execution_SpinLock_inl_*/
