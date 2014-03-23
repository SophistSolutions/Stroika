/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
                : fLock_ (/*ATOMIC_FLAG_INIT*/)
            {
                // unclear we need to set???
                fLock_.clear (std::memory_order_release);   // docs indicate no, but looking at MSFT impl, seems yes (to avoid issue with flag_init not working?
            }
            inline  void    SpinLock::lock ()
            {
                // Acquire lock. If / when fails, yield processor to avoid too much busy waiting.
                while (fLock_.test_and_set (std::memory_order_acquire)) {
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
