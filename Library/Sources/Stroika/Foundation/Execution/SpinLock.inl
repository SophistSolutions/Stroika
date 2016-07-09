/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_SpinLock_inl_
#define _Stroika_Foundation_Execution_SpinLock_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /*
             *  Avoid interference with Windows SDK headers. I hate macros
             */
#ifdef Yield
#undef Yield
#endif
            // Avoid mutual include
            void    Yield ();   // from Thread.h


            /*
             ********************************************************************************
             ******************************** Execution::SpinLock ***************************
             ********************************************************************************
             */
            inline  SpinLock::SpinLock (bool threadFence)
                : fThreadFence_ (threadFence)
#if     !qCompilerAndStdLib_atomic_flag_atomic_flag_init_Buggy
                , fLock_ (ATOMIC_FLAG_INIT)
#endif
            {
#if     qStroika_FeatureSupported_Valgrind
                Stroika_Foundation_Debug_ValgrindDisableCheck_stdatomic (fLock_);
                VALGRIND_HG_MUTEX_INIT_POST (&fLock_, true);
#endif
#if     qCompilerAndStdLib_atomic_flag_atomic_flag_init_Buggy
                fLock_.clear (std::memory_order_release);   // docs indicate no, but looking at MSFT impl, seems yes (to avoid issue with flag_init not working?
#endif
            }
#if     qStroika_FeatureSupported_Valgrind
            inline  SpinLock::~SpinLock ()
            {
                VALGRIND_HG_MUTEX_DESTROY_PRE (&fLock_);
                VALGRIND_HG_ENABLE_CHECKING (&fLock_, sizeof (fLock_));
            }
#endif
            inline  bool    SpinLock::try_lock ()
            {
                // Atomically set fLock to true and examine the previous value. If it was false, we
                // successfully gained the lock. If it was already true (someone else holds the lock),
                // we did NOT gain the lock, but we changed nothing. The fact that we changed nothing in the case where
                // we didn't gain the lock, is why this is not a race.
#if     qStroika_FeatureSupported_Valgrind
                VALGRIND_HG_MUTEX_LOCK_PRE (&fLock_, true);
#endif
                bool result = not fLock_.test_and_set (std::memory_order_acquire);
#if     qStroika_FeatureSupported_Valgrind
                if (result) {
                    VALGRIND_HG_MUTEX_LOCK_POST (&fLock_);
                }
#endif
                if (result and fThreadFence_) {
                    /*
                     *  https://stroika.atlassian.net/browse/STK-494
                     *
                     *  According to http://www.boost.org/doc/libs/1_54_0/doc/html/atomic/usage_examples.html#boost_atomic.usage_examples.example_spinlock
                     *  it makes sense to use memory_order_acquire, but that doesn't seem right, as the
                     *  lock could protect reading or writing another memory area.
                     */
                    std::atomic_thread_fence (std::memory_order_seq_cst);
                }
                return result;
            }
            inline  void    SpinLock::lock ()
            {
                // Acquire lock. If / when fails, yield processor to avoid too much busy waiting.
                while (not try_lock ()) {
                    Yield ();
                }
            }
            inline  void    SpinLock::unlock ()
            {
                // release lock
#if     qStroika_FeatureSupported_Valgrind
                VALGRIND_HG_MUTEX_UNLOCK_PRE (&fLock_);
#endif
                fLock_.clear (std::memory_order_release);
#if     qStroika_FeatureSupported_Valgrind
                VALGRIND_HG_MUTEX_UNLOCK_POST (&fLock_);
#endif
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_SpinLock_inl_*/
