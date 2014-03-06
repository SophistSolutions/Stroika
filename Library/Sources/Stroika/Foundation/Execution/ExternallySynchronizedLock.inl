/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ExternallySynchronizedLock_inl_
#define _Stroika_Foundation_Execution_ExternallySynchronizedLock_inl_    1


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
             *************** Execution::ExternallySynchronizedLock **************************
             ********************************************************************************
             */
            inline  ExternallySynchronizedLock::ExternallySynchronizedLock ()
#if     qDebug
                : fLock_ ( /*ATOMIC_FLAG_INIT*/)
#endif
            {
            }
            inline  void    ExternallySynchronizedLock::lock ()
            {
#if     qDebug
                if (fLock_.test_and_set (std::memory_order_acquire)) {
                    AssertNotReached ();
                }
#endif
            }
            inline  void    ExternallySynchronizedLock::unlock ()
            {
#if     qDebug
                fLock_.clear (std::memory_order_release);
#endif
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_ExternallySynchronizedLock_inl_*/
