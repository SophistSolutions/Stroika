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
            inline  AssertExternallySynchronizedLock::AssertExternallySynchronizedLock (const AssertExternallySynchronizedLock& src)
                : AssertExternallySynchronizedLock ()
            {
                lock_guard<const AssertExternallySynchronizedLock> critSec1 { src };
                lock_guard<const AssertExternallySynchronizedLock> critSec2 { *this };
            }
            inline  AssertExternallySynchronizedLock&   AssertExternallySynchronizedLock::operator= (const AssertExternallySynchronizedLock& rhs)
            {
                lock_guard<const AssertExternallySynchronizedLock> critSec1 { rhs };
                lock_guard<const AssertExternallySynchronizedLock> critSec2 { *this };
                return *this;
            }
            inline  void    AssertExternallySynchronizedLock::lock () const
            {
#if     qDebug
                if (fLock_.test_and_set (std::memory_order_acquire)) {
                    AssertNotReached ();
                }
#endif
            }
            inline  void    AssertExternallySynchronizedLock::unlock () const
            {
#if     qDebug
                fLock_.clear (std::memory_order_release);
#endif
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Debug_AssertExternallySynchronizedLock_inl_*/
