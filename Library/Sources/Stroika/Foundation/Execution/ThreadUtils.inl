/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_ThreadUtils_inl
#define	_ThreadUtils_inl	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include	"../Memory/Basics.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

	//	class	IRunnable
		inline	IRunnable::~IRunnable ()
			{
			}




	//	class	WaitAbandonedException
		inline	WaitAbandonedException::WaitAbandonedException ():
			StringException (L"WAIT on object abandoned because the owning thread terminated")
			{
			}

	//	class	WaitTimedOutException
		inline	WaitTimedOutException::WaitTimedOutException ():
			StringException (L"WAIT timed out")
			{
			}



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



	// class	Event
		inline	Event::Event (bool manualReset, bool initialState):
			fEventHandle (::CreateEvent (NULL, manualReset, initialState, NULL))
			{
				ThrowIfFalseGetLastError (fEventHandle != NULL);
				#if		qTrack_Execution_HandleCounts
					::InterlockedIncrement (&sCurAllocatedHandleCount);
				#endif
			}
		inline	Event::~Event ()
			{
				Verify (::CloseHandle (fEventHandle));
				#if		qTrack_Execution_HandleCounts
					::InterlockedDecrement (&sCurAllocatedHandleCount);
				#endif
			}
		inline	void	Event::Pulse() throw ()
			{
				AssertNotNil (fEventHandle);
				Verify (::PulseEvent (fEventHandle));
			}
		inline	void	Event::Reset () throw ()
			{
				AssertNotNil (fEventHandle);
				Verify (::ResetEvent (fEventHandle));
			}
		inline	void	Event::Set () throw ()
			{
				AssertNotNil (fEventHandle);
				Verify (::SetEvent (fEventHandle));
			}
		inline	void	Event::Wait (float timeout) const
			{
				AssertNotNil (fEventHandle);
				DWORD	milliseconds	=	static_cast<DWORD> (timeout * 1000);
				if (timeout > 1000) {
					milliseconds = INFINITE;	// must be careful about rounding errors in int->float->int
				}
				DWORD	result	=	::WaitForSingleObject (fEventHandle, milliseconds);
				switch (result) {
					case	WAIT_TIMEOUT:	DoThrow (WaitTimedOutException ());
					case	WAIT_ABANDONED:	DoThrow (WaitAbandonedException ());
				}
				Verify (result == WAIT_OBJECT_0);
			}
		inline	Event::operator HANDLE () const
			{
				AssertNotNil (fEventHandle);
				return fEventHandle;
			}


	// class	Sleep
		inline	void	Sleep (float seconds2Wait)
			{
				::SleepEx (static_cast<int> (seconds2Wait * 1000), true);
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


#if 0
	// class	CheckForThreadAborting
		inline	void	CheckForThreadAborting ()
			{
				/*
					* I think we could use SleepEx() or WaitForMultipleObjectsEx(), but SleepEx(0,true) may cause a thread to give up
					* the CPU (ask itself to be rescheduled). WaitForMultipleObjectsEx - from the docs - doesn't appear to do this.
					* I think its a lower-cost way to check for a thread being aborted...
					*			-- LGP 2010-10-26
					*/
::SleepEx (0, true);
//				(void)::WaitForMultipleObjectsEx (0, NULL, false, 0, true);
			}
#endif


	// class	ThreadAbortException
		inline	ThreadAbortException::ThreadAbortException ()
			{
			}

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

	}
}
#endif	/*_ThreadUtils_inl*/
