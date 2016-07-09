/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_SpinLock_h_
#define _Stroika_Foundation_Execution_SpinLock_h_  1

#include    "../StroikaPreComp.h"

#include    <atomic>

#include    "../Configuration/Common.h"
#include    "../Time/Realtime.h"


/**
 * TODO
 *
 *  \file
 *
 *      PROGRESS NOTES GETTING THREAD INTERUPTION VIA SIGNALS WORKING ON POSIX
 *
 *      @todo   Since this lock is NOT recursive, we could add a capture of the current thread, and then
 *              ASSERT no calls come in on the same thread, as they would be guarnateed to deadlock!
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             *  Implementation based on
             *      http://en.cppreference.com/w/cpp/atomic/atomic_flag
             *
             *  About to run tests to compare performance numbers. But this maybe useful for at least some(many) cases.
             *
             *  Note - SpinLock - though generally faster than most mutexes for short accesses, are not recursive mutexes!
             */
            class   SpinLock {
            public:
                /**
                 *  In typical usage, one would use a SpinLock as a mutex, and expect it to create a memory fence.
                 *  However, sometimes you want to spinlock and handle the memory ordering yourself. So that feature
                 *  is optional (defaulting to the safer, but slower - true).
                 */
                SpinLock (bool threadFence = true);
                SpinLock (const SpinLock&) = delete;

#if     qStroika_FeatureSupported_Valgrind
            public:
                ~SpinLock ();
#endif

            public:
                nonvirtual  SpinLock& operator= (const SpinLock&) = delete;

            public:
                /**
                 */
                nonvirtual  bool    try_lock ();

            public:
                /**
                 */
                nonvirtual  void    lock ();

            public:
                /**
                 */
                nonvirtual  void    unlock ();

            private:
                bool    fThreadFence_;

            private:
                atomic_flag fLock_;
                //atomic_flag fLock_ { ATOMIC_FLAG_INIT };

            private:
                // put in private subroutine instead of directly calling cuz MSVC impl is inline and
                // makes call-point larger, and we dont want this inlined (performance impact)
                static  dont_inline void    Yield_ ();
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "SpinLock.inl"

#endif  /*_Stroika_Foundation_Execution_SpinLock_h_*/
