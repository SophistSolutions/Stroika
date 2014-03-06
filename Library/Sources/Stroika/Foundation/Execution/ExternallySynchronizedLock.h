/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ExternallySynchronizedLock_h_
#define _Stroika_Foundation_Execution_ExternallySynchronizedLock_h_  1

#include    "../StroikaPreComp.h"

#include    <atomic>

#include    "../Configuration/Common.h"
#include    "../Debug/Assertions.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             * This class is used as a 'no op' in production builds, as a 'locker' for a class that needs
             *  no thread locking because its externally synchonized.
             *
             *  Externally synchonized means that some external applicaiton control guarnatees the seciton of code (or data)
             *  is only accessed by a single thread.
             *
             *  In debug builds, it enforces this fact through assertions.
             */
            class   ExternallySynchronizedLock {
            public:
                ExternallySynchronizedLock ();
                ExternallySynchronizedLock (const ExternallySynchronizedLock&) = delete;

            public:
                const ExternallySynchronizedLock& operator= (const ExternallySynchronizedLock&) = delete;

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
#include    "ExternallySynchronizedLock.inl"

#endif  /*_Stroika_Foundation_Execution_ExternallySynchronizedLock_h_*/
