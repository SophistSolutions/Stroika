/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_Thread_inl_
#define	_Stroika_Foundation_Execution_Thread_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"ThreadAbortException.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {



		// class	Thread::Rep
			inline	void	Thread::Rep::Start ()
				{
					fOK2StartEvent.Set ();
				}
			inline	void	Thread::Rep::ThrowAbortIfNeeded () const
				{
				#if			qUseThreads_WindowsNative
					Require (::GetCurrentThreadId () == MyGetThreadId_ ());
				#endif
					AutoCriticalSection enterCritcalSection (fStatusCriticalSection);
					if (fStatus == eAborting) {
						DoThrow (ThreadAbortException ());
					}
				}


		// class	Thread
		#if			qUseThreads_WindowsNative
			inline	HANDLE	Thread::GetOSThreadHandle () const
				{
					return fRep->fThread;
				}
		#endif
			inline	RefCntPtr<Thread::Rep>	Thread::GetRep () const
				{
					return fRep;
				}
			inline	bool	Thread::operator< (const Thread& rhs) const
				{
					return fRep < rhs.fRep;
				}
			inline	void	Thread::AbortAndWaitForDone (Time::DurationSecondsType timeout)
				{
					Abort ();
					WaitForDone (timeout);
				}
			inline	wstring	Thread::GetThreadName () const
				{
					return fRep->fThreadName;
				}
			inline	Thread::Status	Thread::GetStatus () const
				{
					if (fRep.IsNull ()) {
						return eNull;
					}
					return GetStatus_ ();
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
#endif	/*_Stroika_Foundation_Execution_Thread_inl_*/
