/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_SpinLock_h_
#define _Stroika_Foundation_Execution_SpinLock_h_  1

#include    "../StroikaPreComp.h"

#include    <atomic>

#include    "../Configuration/Common.h"
#include    "../Time/Realtime.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             *  Implementation based on
             *      http://en.cppreference.com/w/cpp/atomic/atomic_flag
             *
             *  Abotu to run tests to compare perofmrance numbers. But this maybe useful for at least some(many) cases.
             *
             */
            class   SpinLock {
            public:
                SpinLock ();
                SpinLock (const SpinLock&) = delete;

            public:
                const SpinLock& operator= (const SpinLock&) = delete;

            public:
                nonvirtual  void    lock ();

            public:
                nonvirtual  void    unlock ();

            private:
                atomic_flag fLock_;
                //atomic_flag fLock_ = ATOMIC_FLAG_INIT;

            private:
                // put in private subroutine instead of directly calling cuz MSVC impl is inline and
                // makes call-point larger, and we dont want this inlined (performance impact)
                static  void    Yield_ ();
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
