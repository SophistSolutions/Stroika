/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_AssertExternallySynchronizedLock_h_
#define _Stroika_Foundation_Debug_AssertExternallySynchronizedLock_h_  1

#include    "../StroikaPreComp.h"

#include    <atomic>

#include    "../Configuration/Common.h"
#include    "../Debug/Assertions.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Debug {


            /**
             *  \brief      NOT a real lock - just a debugging infrastrucutre support tool so in debug builds we assure used threadsafe
             *
             * This class is used as a 'no op' in production builds, as a 'locker' for a class that needs
             *  no thread locking because its externally synchronized.
             *
             *  Externally synchronized means that some external applicaiton control guarnatees the seciton of code (or data)
             *  is only accessed by a single thread.
             *
             *  In debug builds, it enforces this fact through assertions.
             */
            class   AssertExternallySynchronizedLock {
            public:
                AssertExternallySynchronizedLock ();
                AssertExternallySynchronizedLock (const AssertExternallySynchronizedLock&) = delete;

            public:
                nonvirtual  AssertExternallySynchronizedLock& operator= (const AssertExternallySynchronizedLock&) = delete;

            public:
                nonvirtual  void    lock ();

            public:
                nonvirtual  void    unlock ();

#if     qDebug
            private:
                atomic_flag fLock_;
#endif
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "AssertExternallySynchronizedLock.inl"

#endif  /*_Stroika_Foundation_Debug_AssertExternallySynchronizedLock_h_*/
