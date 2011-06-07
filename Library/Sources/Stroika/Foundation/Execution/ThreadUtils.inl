/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_ThreadUtils_inl_
#define	_Stroika_Foundation_Execution_ThreadUtils_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {




	//	class	CopyableCriticalSection
		inline	CopyableCriticalSection::CopyableCriticalSection ():
			fCritSec (DEBUG_NEW CriticalSection ())
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



	// class	Sleep
		inline	void	Sleep (float seconds2Wait)
			{
				::SleepEx (static_cast<int> (seconds2Wait * 1000), true);
			}




		}
	}
}
#endif	/*_Stroika_Foundation_Execution_ThreadUtils_inl_*/
