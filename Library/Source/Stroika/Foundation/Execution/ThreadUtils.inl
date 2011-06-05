/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../Memory/Memory.h"

namespace	Stroika {	
	namespace	Foundation {

	//	class	ThreadUtils::IRunnable
		inline	ThreadUtils::IRunnable::~IRunnable ()
			{
			}




	//	class	ThreadUtils::WaitAbandonedException
		inline	ThreadUtils::WaitAbandonedException::WaitAbandonedException ():
			StringException (L"WAIT on object abandoned because the owning thread terminated")
			{
			}

	//	class	ThreadUtils::WaitTimedOutException
		inline	ThreadUtils::WaitTimedOutException::WaitTimedOutException ():
			StringException (L"WAIT timed out")
			{
			}

			
	//	class	ThreadUtils::CriticalSection
		inline	ThreadUtils::CriticalSection::CriticalSection () throw ()
			{
				memset (&fCritSec, 0, sizeof(CRITICAL_SECTION));
				::InitializeCriticalSection (&fCritSec);
			}
		inline	ThreadUtils::CriticalSection::~CriticalSection()
			{
				IgnoreExceptionsForCall (::DeleteCriticalSection (&fCritSec));
			}
		inline	void	ThreadUtils::CriticalSection::Lock () 
			{
				::EnterCriticalSection (&fCritSec);
			}
		inline	void ThreadUtils::CriticalSection::Unlock()
			{
				::LeaveCriticalSection (&fCritSec);
			}
		inline	ThreadUtils::CriticalSection::operator CRITICAL_SECTION& ()
			{
				return fCritSec;
			}


	//	class	ThreadUtils::CopyableCriticalSection
		inline	ThreadUtils::CopyableCriticalSection::CopyableCriticalSection ():
			fCritSec (DEBUG_NEW CriticalSection ())
			{
			}
		inline	void	ThreadUtils::CopyableCriticalSection::Lock () 
			{
				fCritSec->Lock ();
			}
		inline	void ThreadUtils::CopyableCriticalSection::Unlock ()
			{
				fCritSec->Unlock ();
			}
		inline	ThreadUtils::CopyableCriticalSection::operator CRITICAL_SECTION& ()
			{
				return *fCritSec;
			}


	//	class	ThreadUtils::AutoCriticalSection
		inline	ThreadUtils::AutoCriticalSection::AutoCriticalSection (CRITICAL_SECTION& critSec):
			fCritSec (critSec)
			{
				::EnterCriticalSection (&fCritSec);
			}
		inline	ThreadUtils::AutoCriticalSection::~AutoCriticalSection ()
			{
				IgnoreExceptionsForCall (::LeaveCriticalSection (&fCritSec));
			}



	// class	ThreadUtils::Event
		inline	ThreadUtils::Event::Event (bool manualReset, bool initialState):
			fEventHandle (::CreateEvent (NULL, manualReset, initialState, NULL))
			{
				ThrowIfFalseGetLastError (fEventHandle != NULL);
				#if		qTrack_ThreadUtils_HandleCounts
					::InterlockedIncrement (&sCurAllocatedHandleCount);
				#endif
			}
		inline	ThreadUtils::Event::~Event ()
			{
				Verify (::CloseHandle (fEventHandle));
				#if		qTrack_ThreadUtils_HandleCounts
					::InterlockedDecrement (&sCurAllocatedHandleCount);
				#endif
			}
		inline	void	ThreadUtils::Event::Pulse() throw ()
			{
				AssertNotNil (fEventHandle);
				Verify (::PulseEvent (fEventHandle));
			}
		inline	void	ThreadUtils::Event::Reset () throw ()
			{
				AssertNotNil (fEventHandle);
				Verify (::ResetEvent (fEventHandle));
			}
		inline	void	ThreadUtils::Event::Set () throw ()
			{
				AssertNotNil (fEventHandle);
				Verify (::SetEvent (fEventHandle));
			}
		inline	void	ThreadUtils::Event::Wait (float timeout) const
			{
				AssertNotNil (fEventHandle);
				DWORD	milliseconds	=	static_cast<DWORD> (timeout * 1000);
				if (timeout > 1000) {
					milliseconds = INFINITE;	// must be careful about rounding errors in int->float->int
				}
				DWORD	result	=	::WaitForSingleObject (fEventHandle, milliseconds);
				switch (result) {
					case	WAIT_TIMEOUT:	Exceptions::DoThrow (WaitTimedOutException ());
					case	WAIT_ABANDONED:	Exceptions::DoThrow (WaitAbandonedException ());
				}
				Verify (result == WAIT_OBJECT_0);
			}
		inline	ThreadUtils::Event::operator HANDLE () const
			{
				AssertNotNil (fEventHandle);
				return fEventHandle;
			}


	// class	ThreadUtils::Sleep
		inline	void	ThreadUtils::Sleep (float seconds2Wait)
			{
				::SleepEx (static_cast<int> (seconds2Wait * 1000), true);
			}


	// class	ThreadUtils::CheckForThreadAborting
	template	<unsigned int kEveryNTimes>
		void	ThreadUtils::CheckForThreadAborting ()
			{
				static	unsigned int	n	=	0;
				if (++n % kEveryNTimes == kEveryNTimes-1) {
					CheckForThreadAborting ();
				}
			}


#if 0
	// class	ThreadUtils::CheckForThreadAborting
		inline	void	ThreadUtils::CheckForThreadAborting ()
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


	// class	ThreadUtils::ThreadAbortException
		inline	ThreadUtils::ThreadAbortException::ThreadAbortException ()
			{
			}

	// class	ThreadUtils::SimpleThread::Rep
		inline	void	ThreadUtils::SimpleThread::Rep::Start ()
			{
				fOK2StartEvent.Set ();
			}
		inline	void	ThreadUtils::SimpleThread::Rep::ThrowAbortIfNeeded () const
			{
				Require (::GetCurrentThreadId () == MyGetThreadId_ ());
				AutoCriticalSection enterCritcalSection (fStatusCriticalSection);
				if (fStatus == eAborting) {
					Exceptions::DoThrow (ThreadAbortException ());
				}
			}


	// class	ThreadUtils::SimpleThread
		inline	HANDLE	ThreadUtils::SimpleThread::GetOSThreadHandle () const
			{
				return fRep->fThread;
			}
		inline	RefCntPtr<ThreadUtils::SimpleThread::Rep>	ThreadUtils::SimpleThread::GetRep () const
			{
				return fRep;
			}
		inline	bool	ThreadUtils::SimpleThread::operator< (const SimpleThread& rhs) const
			{
				return fRep < rhs.fRep;
			}
		inline	void	ThreadUtils::SimpleThread::StopAndWaitForDone (float timeout)
			{
				Stop ();
				WaitForDone (timeout);
			}
		inline	wstring	ThreadUtils::SimpleThread::GetThreadName () const
			{
				return fRep->fThreadName;
			}

	}
}
