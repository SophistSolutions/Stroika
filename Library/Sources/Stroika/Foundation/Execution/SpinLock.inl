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
                : fLock_ ( ATOMIC_FLAG_INIT)
            {
            }
            inline  void    SpinLock::lock ()
            {
                // acquire lock
                while (fLock_.test_and_set (std::memory_order_acquire))
                    ; // spin
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
