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

namespace Stroika {
    namespace Foundation {
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
                bool result = not fLock_.test_and_set (std::memory_order_acquire);
#if qStroika_FeatureSupported_Valgrind
                if (result) {
                    VALGRIND_HG_MUTEX_LOCK_POST (&fLock_);
                }
#endif
                if (result) {
                    /*
                     *  https://stroika.atlassian.net/browse/STK-494
                     *
                     *  According to http://www.boost.org/doc/libs/1_54_0/doc/html/atomic/usage_examples.html#boost_atomic.usage_examples.example_spinlock
                     *  it makes sense to use memory_order_acquire, but that doesn't seem right, as the
                     *  lock could protect reading or writing another memory area.
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
}
#endif /*_Stroika_Foundation_Execution_SpinLock_inl_*/
