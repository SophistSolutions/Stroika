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
			inline	CriticalSection::CriticalSection () throw ()
				{
				#if qUseWindowsNativeThreadSupport
					memset (&fCritSec, 0, sizeof(CRITICAL_SECTION));
					::InitializeCriticalSection (&fCritSec);
				#endif
				}
			inline	CriticalSection::~CriticalSection()
				{
				#if qUseWindowsNativeThreadSupport
					IgnoreExceptionsForCall (::DeleteCriticalSection (&fCritSec));
				#endif
				}
			inline	void	CriticalSection::Lock () 
				{
				#if qUseWindowsNativeThreadSupport
					::EnterCriticalSection (&fCritSec);
				#endif
				}
			inline	void CriticalSection::Unlock()
				{
				#if qUseWindowsNativeThreadSupport
					::LeaveCriticalSection (&fCritSec);
				#endif
				}
			#if		qUseWindowsNativeThreadSupport
			inline	CriticalSection::operator CRITICAL_SECTION& ()
				{
					return fCritSec;
				}
			#endif


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
