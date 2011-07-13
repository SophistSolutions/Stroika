/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_CopyableCriticalSection_inl_
#define	_Stroika_Foundation_Execution_CopyableCriticalSection_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			//	class	CopyableCriticalSection
				inline	CopyableCriticalSection::CopyableCriticalSection ()
					: fCritSec (DEBUG_NEW CriticalSection ())
					{
					}
				inline	void	CopyableCriticalSection::Lock () 
					{
						fCritSec->Lock ();
					}
				inline	void CopyableCriticalSection::Unlock ()
					{
						fCritSec->Unlock ();
					}
				inline	CopyableCriticalSection::operator CRITICAL_SECTION& ()
					{
						return *fCritSec;
					}

		}
	}
}
#endif	/*_Stroika_Foundation_Execution_CopyableCriticalSection_inl_*/
