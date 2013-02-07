/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_CriticalSection_h_
#define _Stroika_Foundation_Execution_CriticalSection_h_    1

#include    "../StroikaPreComp.h"

#include    <mutex>
#if     qUseThreads_StdCPlusPlus
#include    <mutex>
#elif   qUseThreads_WindowsNative
#include    <windows.h>
#endif

#include    "../Configuration/Common.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {




            typedef std::recursive_mutex   CriticalSection;


            typedef lock_guard<CriticalSection>   AutoCriticalSection;


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
