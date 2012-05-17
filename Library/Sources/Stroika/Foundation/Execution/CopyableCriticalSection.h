/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_CopyableCriticalSection_h_
#define _Stroika_Foundation_Execution_CopyableCriticalSection_h_    1

#include    "../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <Windows.h>
#endif

#include    "../Configuration/Common.h"
#include    "../Memory/SharedPtr.h"

#include    "CriticalSection.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            class   CopyableCriticalSection {
            private:
                Memory::SharedPtr<CriticalSection>  fCritSec;
            public:
                CopyableCriticalSection ();

            public:
                nonvirtual  void    Lock ();
                nonvirtual  void    Unlock ();

            public:
                template    <typename T>
                T   As ();
            };
#if     qUseThreads_WindowsNative
            template    <>
            inline  CRITICAL_SECTION&   CopyableCriticalSection::As () {
                return fCritSec->As<CRITICAL_SECTION&> ();
            }
#endif


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_CopyableCriticalSection_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "CopyableCriticalSection.inl"
