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




#if qJustUseStdCCritSecStuff
			typedef std::recursive_mutex   CriticalSection;
#else

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
				nonvirtual  void    lock () {Lock (); }
				nonvirtual  void    unlock () { Unlock (); }

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

#endif


#if qJustUseStdCCritSecStuff
			typedef lock_guard<CriticalSection>   AutoCriticalSection;
#else
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
#endif
			


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
