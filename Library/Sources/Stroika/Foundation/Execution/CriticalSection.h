/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_CriticalSection_h_
#define _Stroika_Foundation_Execution_CriticalSection_h_    1

#include    "../StroikaPreComp.h"

#if     qUseThreads_StdCPlusPlus
#include    <mutex>
#elif   qUseThreads_WindowsNative
#include    <windows.h>
#endif

#include    "../Configuration/Common.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            class   CriticalSection {
            public:
                NO_COPY_CONSTRUCTOR(CriticalSection);
                NO_ASSIGNMENT_OPERATOR(CriticalSection);

            public:
                CriticalSection ();
                ~CriticalSection ();

            public:
                nonvirtual  void    Lock ();
                nonvirtual  void    Unlock ();

            public:
                template    <typename T>
                nonvirtual  T   As ();

#if     qUseThreads_StdCPlusPlus
            private:
                std::recursive_mutex    fMutex_;
#elif   qUseThreads_WindowsNative
            private:
                CRITICAL_SECTION fCritSec_;
#endif
            };
#if     qUseThreads_WindowsNative
            template    <>
            inline  CRITICAL_SECTION&   CriticalSection::As ()
            {
                return fCritSec_;
            }
#endif





            // enter  in CTOR and LEAVE in DTOR
            template    <typename LOCKTYPE>
            class   AutoCriticalSectionT {
            public:
                explicit AutoCriticalSectionT (LOCKTYPE& critSec);
                ~AutoCriticalSectionT ();

            private:
                LOCKTYPE&   fCritSec_;
            };
            typedef AutoCriticalSectionT<CriticalSection>   AutoCriticalSection;


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_CriticalSection_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "CriticalSection.inl"
