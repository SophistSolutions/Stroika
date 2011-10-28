/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_CriticalSection_inl_
#define	_Stroika_Foundation_Execution_CriticalSection_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Exceptions.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			
		//	class	CriticalSection
			inline	CriticalSection::CriticalSection ()
				{
				#if qUseThreads_WindowsNative
					memset (&fCritSec, 0, sizeof(CRITICAL_SECTION));
					::InitializeCriticalSection (&fCritSec);
				#endif
				}
			inline	CriticalSection::~CriticalSection()
				{
				#if qUseThreads_WindowsNative
					IgnoreExceptionsForCall (::DeleteCriticalSection (&fCritSec));
				#endif
				}
			inline	void	CriticalSection::Lock () 
				{
				#if		qUseThreads_WindowsNative
					::EnterCriticalSection (&fCritSec);
				#elif		qUseThreads_StdCPlusPlus
					fMutex_.lock ();
				#endif
				}
			inline	void CriticalSection::Unlock()
				{
				#if		qUseThreads_WindowsNative
					::LeaveCriticalSection (&fCritSec);
				#elif		qUseThreads_StdCPlusPlus
					fMutex_.unlock ();
				#endif
				}


		//	class	AutoCriticalSection
			template	<typename LOCKTYPE>
				inline	AutoCriticalSectionT<LOCKTYPE>::AutoCriticalSectionT (LOCKTYPE& critSec)
					: fCritSec (critSec)
					{
						fCritSec.Lock ();
					}
			template	<typename LOCKTYPE>
				inline	AutoCriticalSectionT<LOCKTYPE>::~AutoCriticalSectionT ()
					{
						fCritSec.Unlock ();
					}

		}
	}
}
#endif	/*_Stroika_Foundation_Execution_CriticalSection_inl_*/
