/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_CopyableCriticalSection_h_
#define _Stroika_Foundation_Execution_CopyableCriticalSection_h_    1

#include    "../StroikaPreComp.h"

#include    <memory>

#if     qPlatform_Windows
#include    <Windows.h>
#endif

#include    "../Configuration/Common.h"

#include    "CriticalSection.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


			//PROBABLY OBSOLETE - NO NEW USES... WILL BE GOING AWAY
            class   CopyableCriticalSection {
            private:
#if qJustUseStdCCritSecStuff
				shared_ptr<std::recursive_mutex>  fCritSec;
#else
                shared_ptr<CriticalSection>  fCritSec;
#endif
            public:
                CopyableCriticalSection ();

            public:
                nonvirtual  void    Lock ();
                nonvirtual  void    Unlock ();
				nonvirtual  void    lock () { Lock (); }
				nonvirtual  void    unlock () { Unlock (); }
#if 0
            public:
                template    <typename T>
                T   As ();
#endif
            };
#if 0
#if     qUseThreads_WindowsNative
            template    <>
            inline  CRITICAL_SECTION&   CopyableCriticalSection::As ()
            {
                return fCritSec->As<CRITICAL_SECTION&> ();
            }
#endif
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
