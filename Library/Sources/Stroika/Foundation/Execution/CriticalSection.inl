/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_CriticalSection_inl_
#define _Stroika_Foundation_Execution_CriticalSection_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Exceptions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            //  class   CriticalSection
            inline  CriticalSection::CriticalSection ()
            {
#if qUseThreads_WindowsNative
                memset (&fCritSec_, 0, sizeof(fCritSec_));
#if     qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(suppress: 28125)
#endif
                ::InitializeCriticalSection (&fCritSec_);
#endif
            }
            inline  CriticalSection::~CriticalSection()
            {
#if qUseThreads_WindowsNative
                IgnoreExceptionsForCall (::DeleteCriticalSection (&fCritSec_));
#endif
            }
            inline  void    CriticalSection::Lock ()
            {
#if     qUseThreads_WindowsNative
                ::EnterCriticalSection (&fCritSec_);
#elif       qUseThreads_StdCPlusPlus
                fMutex_.lock ();
#endif
            }
            inline  void CriticalSection::Unlock()
            {
#if     qUseThreads_WindowsNative
                ::LeaveCriticalSection (&fCritSec_);
#elif       qUseThreads_StdCPlusPlus
                fMutex_.unlock ();
#endif
            }


            //  class   AutoCriticalSection
            template    <typename LOCKTYPE>
            inline  AutoCriticalSectionT<LOCKTYPE>::AutoCriticalSectionT (LOCKTYPE& critSec)
                : fCritSec_ (critSec)
            {
                fCritSec_.Lock ();
            }
            template    <typename LOCKTYPE>
            inline  AutoCriticalSectionT<LOCKTYPE>::~AutoCriticalSectionT ()
            {
                fCritSec_.Unlock ();
            }

        }
    }
}
#endif  /*_Stroika_Foundation_Execution_CriticalSection_inl_*/
