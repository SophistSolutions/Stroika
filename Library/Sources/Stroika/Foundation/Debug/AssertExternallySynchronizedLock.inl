/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_AssertExternallySynchronizedLock_inl_
#define _Stroika_Foundation_Debug_AssertExternallySynchronizedLock_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Debug {


            /*
             ********************************************************************************
             *************** Debug::AssertExternallySynchronizedLock ************************
             ********************************************************************************
             */
            inline  AssertExternallySynchronizedLock::AssertExternallySynchronizedLock ()
#if     qDebug && !qCompilerAndStdLib_atomic_flag_atomic_flag_init_Buggy
                : fLock_ (ATOMIC_FLAG_INIT)
#endif
            {
#if     qDebug && qCompilerAndStdLib_atomic_flag_atomic_flag_init_Buggy
                fLock_.clear (std::memory_order_release);   // docs indicate no, but looking at MSFT impl, seems yes (to avoid issue with flag_init not working?
#endif
            }
            inline  void    AssertExternallySynchronizedLock::lock ()
            {
#if     qDebug
                if (fLock_.test_and_set (std::memory_order_acquire)) {
                    AssertNotReached ();
                }
#endif
            }
            inline  void    AssertExternallySynchronizedLock::unlock ()
            {
#if     qDebug
                fLock_.clear (std::memory_order_release);
#endif
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Debug_AssertExternallySynchronizedLock_inl_*/
