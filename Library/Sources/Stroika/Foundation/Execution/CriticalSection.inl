/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
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
					memset (&fCritSec, 0, sizeof(CRITICAL_SECTION));
					::InitializeCriticalSection (&fCritSec);
				}
			inline	CriticalSection::~CriticalSection()
				{
					IgnoreExceptionsForCall (::DeleteCriticalSection (&fCritSec));
				}
			inline	void	CriticalSection::Lock () 
				{
					::EnterCriticalSection (&fCritSec);
				}
			inline	void CriticalSection::Unlock()
				{
					::LeaveCriticalSection (&fCritSec);
				}
			inline	CriticalSection::operator CRITICAL_SECTION& ()
				{
					return fCritSec;
				}

		//	class	AutoCriticalSection
			inline	AutoCriticalSection::AutoCriticalSection (CRITICAL_SECTION& critSec):
				fCritSec (critSec)
				{
					::EnterCriticalSection (&fCritSec);
				}
			inline	AutoCriticalSection::~AutoCriticalSection ()
				{
					IgnoreExceptionsForCall (::LeaveCriticalSection (&fCritSec));
				}


		}
	}
}
#endif	/*_Stroika_Foundation_Execution_CriticalSection_inl_*/
