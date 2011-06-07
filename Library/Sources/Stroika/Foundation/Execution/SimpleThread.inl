/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_SimpleThread_inl_
#define	_Stroika_Foundation_Execution_SimpleThread_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"ThreadAbortException.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {



		// class	SimpleThread::Rep
			inline	void	SimpleThread::Rep::Start ()
				{
					fOK2StartEvent.Set ();
				}
			inline	void	SimpleThread::Rep::ThrowAbortIfNeeded () const
				{
					Require (::GetCurrentThreadId () == MyGetThreadId_ ());
					AutoCriticalSection enterCritcalSection (fStatusCriticalSection);
					if (fStatus == eAborting) {
						DoThrow (ThreadAbortException ());
					}
				}


		// class	SimpleThread
			inline	HANDLE	SimpleThread::GetOSThreadHandle () const
				{
					return fRep->fThread;
				}
			inline	RefCntPtr<SimpleThread::Rep>	SimpleThread::GetRep () const
				{
					return fRep;
				}
			inline	bool	SimpleThread::operator< (const SimpleThread& rhs) const
				{
					return fRep < rhs.fRep;
				}
			inline	void	SimpleThread::StopAndWaitForDone (float timeout)
				{
					Stop ();
					WaitForDone (timeout);
				}
			inline	wstring	SimpleThread::GetThreadName () const
				{
					return fRep->fThreadName;
				}



		// class	CheckForThreadAborting
		template	<unsigned int kEveryNTimes>
			void	CheckForThreadAborting ()
				{
					static	unsigned int	n	=	0;
					if (++n % kEveryNTimes == kEveryNTimes-1) {
						CheckForThreadAborting ();
					}
				}


		}
	}
}
#endif	/*_Stroika_Foundation_Execution_SimpleThread_inl_*/
