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
             ********************************************************************************
             ******************************** Execution::SpinLock ***************************
             ********************************************************************************
             */
            inline  SpinLock::SpinLock ()
#if     !qCompilerAndStdLib_atomic_flag_atomic_flag_init_Buggy
                : fLock_ (ATOMIC_FLAG_INIT)
#endif
            {
#if     qCompilerAndStdLib_atomic_flag_atomic_flag_init_Buggy
                fLock_.clear (std::memory_order_release);   // docs indicate no, but looking at MSFT impl, seems yes (to avoid issue with flag_init not working?
#endif
            }
            inline  bool    SpinLock::try_lock ()
            {
                // Atomically set fLock to true and examine the previous value. If it was false, we
                // successfully gained the lock. If it was already true (someone else holds the lock),
                // we did NOT gain the lock, but we changed nothing. The fact that we changed nothing in the case where
                // we didn't gain the lock, is why this is not a race.
                return not fLock_.test_and_set (std::memory_order_acquire);
            }
            inline  void    SpinLock::lock ()
            {
                // Acquire lock. If / when fails, yield processor to avoid too much busy waiting.
                while (not try_lock ()) {
                    Yield_ ();
                }
            }
            inline  void    SpinLock::unlock ()
            {
                // release lock
                fLock_.clear (std::memory_order_release);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_SpinLock_inl_*/
