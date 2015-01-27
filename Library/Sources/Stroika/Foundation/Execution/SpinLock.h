/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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
                SpinLock ();
                SpinLock (const SpinLock&) = delete;

            public:
                nonvirtual  SpinLock& operator= (const SpinLock&) = delete;

            public:
                nonvirtual  bool    try_lock();

            public:
                nonvirtual  void    lock ();

            public:
                nonvirtual  void    unlock ();

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
