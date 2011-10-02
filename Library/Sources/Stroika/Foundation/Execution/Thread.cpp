/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	"../Configuration/StroikaConfig.h"

#if		qPlatform_Windows
	#include	<process.h>
	#include	<windows.h>
#endif

#include	"DLLSupport.h"
#include	"../Debug/Trace.h"
#include	"../Characters/StringUtils.h"
#include	"../Containers/VectorUtils.h"
#include	"../Execution/ErrNoException.h"
#include	"../Time/Realtime.h"

#if		qUseThreads_WindowsNative
	#include	"Platform/Windows/WaitSupport.h"
#endif

#include	"Thread.h"



using	namespace	Stroika::Foundation;

using	Time::DurationSecondsType;



// Leave this off by default since I'm not sure its safe, and at best it uses some time. But make it
// easy to turn on it release builds...
//		-- LGP 2009-05-28
// According to http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx - its best NOT to call this RaiseException call
// unless a debugger is present. Use IsDebuggerPresent(). Still not perfect.
//
//#define	qSupportSetThreadNameDebuggerCall	0
#ifndef	qSupportSetThreadNameDebuggerCall
	#if		qDebug && qPlatform_Windows
		#define	qSupportSetThreadNameDebuggerCall	1
	#endif
#endif
#ifndef	qSupportSetThreadNameDebuggerCall
	#define	qSupportSetThreadNameDebuggerCall	0
#endif



using	namespace	Characters;
using	namespace	Execution;

#ifndef	qUseTLSForSAbortingFlag
	#if		defined (__GNUC__)
	#define	qUseTLSForSAbortingFlag		1
	#endif
#endif

// RE-Examine altenrate appraoches to this and beter docuemnt!!!!
//
//		http://bugzilla/show_bug.cgi?id=646
#ifndef	qUseTLSForSAbortingFlag
#define	qUseTLSForSAbortingFlag		0
#endif
#ifndef	qUseSleepExForSAbortingFlag
#define	qUseSleepExForSAbortingFlag	qPlatform_Windows
#endif


#if		qUseTLSForSAbortingFlag
#if		defined (__GNUC__)
__thread bool	s_Aborting	=	false;
#else
__declspec( thread )	bool	s_Aborting	=	false;
#endif
#endif



#if			qPlatform_Windows
namespace	{
	#if (_WIN32_WINNT < 0x0502)
	namespace XXX {
		struct CLIENT_ID {
			DWORD UniqueProcess; 
			DWORD UniqueThread;
		};
		typedef LONG NTSTATUS;
		#define STATUS_SUCCESS ((NTSTATUS) 0x00000000)
		typedef LONG KPRIORITY;
		struct	THREAD_BASIC_INFORMATION {
			NTSTATUS   ExitStatus;
			PVOID      TebBaseAddress;
			CLIENT_ID  ClientId;
			KAFFINITY  AffinityMask;
			KPRIORITY  Priority;
			KPRIORITY  BasePriority;
		};
		enum THREAD_INFORMATION_CLASS {
			ThreadBasicInformation = 0,
		};
		typedef NTSTATUS (__stdcall *pfnNtQueryInformationThread) (HANDLE, THREAD_INFORMATION_CLASS, PVOID, ULONG, PULONG);
	}
	#endif
	int MyGetThreadId (HANDLE thread)
		{
			#if (_WIN32_WINNT >= 0x0502)
				return ::GetThreadId (thread);
			#else
				// See details in http://www.codeguru.com/forum/showthread.php?t=355572 on this... - backcompat - only support
				// GetThreadId (HANDLE) in Win 2003 Server or later
				using namespace XXX;
				static	DLLLoader	ntdll (TSTR ("ntdll.dll"));
				static	pfnNtQueryInformationThread NtQueryInformationThread = (pfnNtQueryInformationThread)ntdll.GetProcAddress ("NtQueryInformationThread");
				if (NtQueryInformationThread == nullptr)
					return 0;	// failed to get proc address
				THREAD_BASIC_INFORMATION	tbi;
				THREAD_INFORMATION_CLASS	tic = ThreadBasicInformation;
				if (NtQueryInformationThread (thread, tic, &tbi, sizeof (tbi), nullptr) != STATUS_SUCCESS) {
					return 0;
				}
				return tbi.ClientId.UniqueThread;
			#endif
		}
}
#endif




using	Debug::TraceContextBumper;



/*
 ********************************************************************************
 ************************************* Thread::Rep_ *****************************
 ********************************************************************************
 */
Thread::Rep_::Rep_ (const SharedPtr<IRunnable>& runnable)
#if			qUseThreads_WindowsNative
	: fThread (INVALID_HANDLE_VALUE)
	, fStatusCriticalSection ()
#else
	: fStatusCriticalSection ()
#endif
	, fStatus (eNotYetRunning)
	, fOK2StartEvent ()
	, fRefCountBumpedEvent ()
	, fRunnable (runnable)
{
	TraceContextBumper ctx (TSTR ("Thread::Rep_::Rep_"));
	RequireNotNull (runnable);
}

void	Thread::Rep_::DoCreate (SharedPtr<Rep_>* repSharedPtr)
{
	RequireNotNull (repSharedPtr);
	RequireNotNull (*repSharedPtr);
	#if			qUseThreads_WindowsNative
		(*repSharedPtr)->fThread = reinterpret_cast<HANDLE> (::_beginthreadex (nullptr, 0, &Rep_::ThreadProc_, repSharedPtr, 0, nullptr));
		if ((*repSharedPtr)->fThread == nullptr) {
			ThrowIfError_errno_t ();	// I THINK errno sb set, but in case not, do Win32 / GetLastError throw
			Platform::Windows::Exception::DoThrow (::GetLastError ());
		}
		try {
			(*repSharedPtr)->fRefCountBumpedEvent.Wait ();	// assure we wait for this, so we don't ever let refcount go to zero before the
															// thread has started...
		}
		catch (...) {
			::CloseHandle ((*repSharedPtr)->fThread);
			(*repSharedPtr)->fThread = INVALID_HANDLE_VALUE;
			Execution::DoReThrow ();
		}
	#endif
}

Thread::Rep_::~Rep_ ()
{
	Assert (fStatus != eRunning);
#if			qUseThreads_WindowsNative
	if (fThread != INVALID_HANDLE_VALUE) {
		::CloseHandle (fThread);
	}
#endif
}

void	Thread::Rep_::Run () override
{
	fRunnable->Run ();
}

void	Thread::Rep_::ThreadMain_ (SharedPtr<Rep_>* thisThreadRep) throw ()
{
	RequireNotNull (thisThreadRep);
	/*
	 * NB: It is important that we do NOT call ::_endthreadex () here because that would cause the
	 * SharedPtr<> here to NOT be destroyed. We could force that with an explicit scope, but there
	 * is no need, since the docs for _beginthreadex () say that _endthreadex () is called automatically.
	 */
	SharedPtr<Rep_>	incRefCnt	=	*thisThreadRep;	// assure refcount incremented so object not deleted while the thread is running
	incRefCnt->fRefCountBumpedEvent.Set ();

	incRefCnt->fOK2StartEvent.Wait ();	// we used to 'SuspendThread' but that was flakey. Instead - wait until teh caller says
										// we really want to start this thread.
	try {
		DbgTrace ("In Thread::Rep_::ThreadMain_ - setting state to RUNNING for thread= %s", FormatThreadID (incRefCnt->GetID ()).c_str ());
		bool	doRun	=	false;
		{
			AutoCriticalSection enterCritcalSection (incRefCnt->fStatusCriticalSection);
			if (incRefCnt->fStatus == eNotYetRunning) {
				incRefCnt->fStatus = eRunning;
				doRun = true;
			}
		}
		if (doRun) {
			incRefCnt->Run ();
		}
		DbgTrace ("In Thread::Rep_::ThreadProc_ - setting state to COMPLETED for thread= %s", FormatThreadID (incRefCnt->GetID ()).c_str ());
		{
			AutoCriticalSection enterCritcalSection (incRefCnt->fStatusCriticalSection);
			incRefCnt->fStatus = eCompleted;
		}
	}
	catch (ThreadAbortException&) {
		DbgTrace ("In Thread::Rep_::ThreadProc_ - setting state to COMPLETED (ThreadAbortException) for thread= %s", FormatThreadID (incRefCnt->GetID ()).c_str ());
		AutoCriticalSection enterCritcalSection (incRefCnt->fStatusCriticalSection);
		incRefCnt->fStatus = eCompleted;
	}
	catch (...) {
		DbgTrace ("In Thread::Rep_::ThreadProc_ - setting state to COMPLETED (EXCEPT) for thread= %s", FormatThreadID (incRefCnt->GetID ()).c_str ());
		AutoCriticalSection enterCritcalSection (incRefCnt->fStatusCriticalSection);
		incRefCnt->fStatus = eCompleted;
	}
}

#if			qUseThreads_WindowsNative
unsigned int	__stdcall	Thread::Rep_::ThreadProc_ (void* lpParameter)
{
	RequireNotNull (lpParameter);
	/*
	 * NB: It is important that we do NOT call ::_endthreadex () here because that would cause the
	 * SharedPtr<> here to NOT be destroyed. We could force that with an explicit scope, but there
	 * is no need, since the docs for _beginthreadex () say that _endthreadex () is called automatically.
	 */
	ThreadMain_ (reinterpret_cast<SharedPtr<Rep_>*> (lpParameter));
	return 0;
}
#endif

void	Thread::Rep_::NotifyOfAbort ()
{
	Require (fStatus == eAborting or fStatus == eCompleted);
	// CAREFUL WHEN OVERRIDING CUZ CALLED TYPICALLY FROM ANOTHER  THREAD!!!
	AutoCriticalSection enterCritcalSection (fStatusCriticalSection);
#if			qUseThreads_WindowsNative
	if (::GetCurrentThreadId () == GetID ()) {
		ThrowAbortIfNeeded ();
	}
	if (fStatus == eAborting) {
		#if		qUseTLSForSAbortingFlag
			s_Aborting = true;
		#endif
		Verify (::QueueUserAPC (&AbortProc_, fThread, reinterpret_cast<ULONG_PTR> (this)));
	}
#endif
}

Thread::IDType	Thread::Rep_::GetID () const
{
	#if		qUseThreads_WindowsNative
		return MyGetThreadId (fThread);
	#elif	qUseThreads_StdCPlusPlus
		AssertNotImplemented ();
		return IDType ();
	#else
		AssertNotImplemented ();
		return IDType ();
	#endif
}

#if			qUseThreads_WindowsNative
void	CALLBACK	Thread::Rep_::AbortProc_ (ULONG_PTR lpParameter)
{
	TraceContextBumper ctx (TSTR ("Thread::Rep_::AbortProc_"));
	Thread::Rep_*	rep	=	reinterpret_cast<Thread::Rep_*> (lpParameter);
	Require (rep->fStatus == eAborting || rep->fStatus == eCompleted);
	rep->ThrowAbortIfNeeded ();
	Require (rep->fStatus == eCompleted);	// normally we don't reach this - but we could if we've already been marked completed somehow
											// before the abortProc got called/finsihed...
}
#endif






/*
 ********************************************************************************
 *********************************** Thread *************************************
 ********************************************************************************
 */
Thread::Thread ()
	: fRep_ ()
{
}

Thread::Thread (void (*fun2CallOnce) (void* arg), void* arg)
	: fRep_ (SharedPtr<Rep_> (DEBUG_NEW Rep_ (SharedPtr<IRunnable> (new SimpleRunnable (fun2CallOnce, arg)))))
{
	Rep_::DoCreate (&fRep_);
}

Thread::Thread (const SharedPtr<IRunnable>& runnable)
	: fRep_ (SharedPtr<Rep_> (DEBUG_NEW Rep_ (runnable)))
{
	Rep_::DoCreate (&fRep_);
}

#if			qUseThreads_WindowsNative
void	Thread::SetThreadPriority (int nPriority)
{
	RequireNotNull (fRep_);
	Verify (::SetThreadPriority (GetOSThreadHandle (), nPriority));
}
#endif

void	Thread::SetThreadName (const wstring& threadName)
{
	RequireNotNull (fRep_);
	if (fRep_->fThreadName != threadName) {
		TraceContextBumper	ctx (TSTR ("Execution::Thread::SetThreadName"));
		DbgTrace (L"(ThreadName = '%s')", threadName.c_str ());
		fRep_->fThreadName = threadName;
		#if		qSupportSetThreadNameDebuggerCall
			if (::IsDebuggerPresent ()) {
				// This hack from http://www.codeproject.com/KB/threads/Name_threads_in_debugger.aspx
				struct THREADNAME_INFO {
					DWORD dwType;		// must be 0x1000
					LPCSTR szName;		// pointer to name (in user addr space)
					DWORD dwThreadID;	// thread ID (-1=caller thread)
					DWORD dwFlags;		// reserved for future use, must be zero
				};
				string	useThreadName	=	WideStringToNarrowSDKString (threadName);
				THREADNAME_INFO info;
				{
					info.dwType = 0x1000;
					info.szName = useThreadName.c_str ();
					info.dwThreadID = MyGetThreadId (fRep_->fThread);
					info.dwFlags = 0;
				}
				IgnoreExceptionsForCall (::RaiseException (0x406D1388, 0, sizeof(info)/sizeof(DWORD), (ULONG_PTR*)&info));
			}
		#endif
	}
}

void	Thread::Start ()
{
	RequireNotNull (fRep_);
#if			qUseThreads_WindowsNative
	Assert (fRep_->fThread != INVALID_HANDLE_VALUE);
#endif
	DbgTrace (L"Thread::Start: (thread = %s, name='%s')", FormatThreadID (GetID ()).c_str (), fRep_->fThreadName.c_str ());
	fRep_->fOK2StartEvent.Set ();
}

void	Thread::Abort ()
{
	if (fRep_.IsNull ()) {
		// then its effectively already stopped.
		return;
	}

#if			qUseThreads_WindowsNative
	// I'm not sure this is 100% thread-friendly, in case two people from two differnt threads tried
	// to stop the same (third) thread at the same time. But its probably good enough for starters.
	//		-- LGP 2009-01-14

	// You cannot call STOP from within the thread you are stopping! Calling stop would cause a throw out - preventing the stop...
	Require (::GetCurrentThreadId () != MyGetThreadId (fRep_->fThread));
#endif

	// first try to send abort exception, and then - if force - get serious!
	AutoCriticalSection enterCritcalSection (fRep_->fStatusCriticalSection);
	if (fRep_->fStatus != eCompleted) {
		fRep_->fStatus = eAborting;
	}
	if (fRep_->fStatus == eAborting) {
		// by default - tries to trigger a throw-abort-excption in the right thread using UNIX signals or QueueUserAPC ()
		fRep_->NotifyOfAbort ();
	}
}

void	Thread::Abort_Forced_Unsafe ()
{
	if (fRep_.IsNull ()) {
		// then its effectively already stopped.
		return;
	}

#if			qUseThreads_WindowsNative
	// You cannot call STOP from within the thread you are stopping! Calling stop would cause a throw out - preventing the stop...
	Require (::GetCurrentThreadId () != MyGetThreadId (fRep_->fThread));
#endif

	Abort ();

	// Wait some reasonable amount of time for the thread to abort
	IgnoreExceptionsForCall (WaitForDone (5.0f));
	AutoCriticalSection enterCritcalSection (fRep_->fStatusCriticalSection);
#if			qUseThreads_WindowsNative
	if (fRep_->fStatus != eCompleted and fRep_->fThread != INVALID_HANDLE_VALUE) {
		// This is VERY bad to do. Put assert here that it never happens...
		Assert (false);
		::TerminateThread (fRep_->fThread, -1);
	}
#else
	AssertNotImplemented ();
#endif
}

void	Thread::WaitForDone (Time::DurationSecondsType timeout) const
{
	if (fRep_.IsNull ()) {
		// then its effectively already done.
		return;
	}

	bool	doWait	=	false;
#if			qUseThreads_WindowsNative
	HANDLE	thread	=	nullptr;
	{
		AutoCriticalSection enterCritcalSection (fRep_->fStatusCriticalSection);
		if (fRep_->fThread != INVALID_HANDLE_VALUE and fRep_->fStatus != eCompleted) {
			doWait = true;
			thread = fRep_->fThread;
		}
	}
	if (doWait) {
		DWORD	dTimeout	=	timeout < 0.0f? UINT_MAX: (DWORD)(timeout * 1000);
		if (::WaitForSingleObject (thread, dTimeout) == WAIT_TIMEOUT) {
			Platform::Windows::Exception::DoThrow (WAIT_TIMEOUT);
		}
	}
#else
	AssertNotImplemented ();
#endif
}

void	Thread::PumpMessagesAndReturnWhenDoneOrAfterTime (Time::DurationSecondsType timeToPump) const
{
	if (fRep_.IsNull ()) {
		// then its effectively already done.
		return;
	}

#if			qUseThreads_WindowsNative
	HANDLE	thread	=	nullptr;
	{
		AutoCriticalSection enterCritcalSection (fRep_->fStatusCriticalSection);
		if (fRep_->fThread != INVALID_HANDLE_VALUE and fRep_->fStatus != eCompleted) {
			thread = fRep_->fThread;
		}
	}
	if (thread != nullptr) {
		Platform::Windows::WaitAndPumpMessages (nullptr, Containers::mkV<HANDLE> (thread), timeToPump);
	}
#else
	AssertNotImplemented ();
#endif
}

#if			qUseThreads_WindowsNative
void	Thread::WaitForDoneWhilePumpingMessages (Time::DurationSecondsType timeout) const
{
	DurationSecondsType	timeoutAt	=	Time::GetTickCount () + timeout;
	// CRUDDY impl - but decent enuf for first draft
	while (GetStatus () != Thread::eCompleted) {
		if (timeout < 0.0f) {
			PumpMessagesAndReturnWhenDoneOrAfterTime ();
		}
		else {
			DurationSecondsType	time2Wait	=	timeoutAt - Time::GetTickCount ();
			if (time2Wait <= 0) {
				Platform::Windows::Exception::DoThrow (WAIT_TIMEOUT);
			}
			PumpMessagesAndReturnWhenDoneOrAfterTime (time2Wait);
		}
	}
}
#endif

Thread::Status	Thread::GetStatus_ () const
{
	Require (not fRep_.IsNull ());
	if (fRep_.IsNull ()) {
		return eNull;
	}
	AutoCriticalSection enterCritcalSection (fRep_->fStatusCriticalSection);
	return fRep_->fStatus;
}







/*
 ********************************************************************************
 **************************** Execution::FormatThreadID *************************
 ********************************************************************************
 */
string	Execution::FormatThreadID (Thread::IDType threadID)
{
	// it appears these IDs are < 16bits, so making the printout format shorter makes it a bit more readable.
	#if		qUseThreads_StdCPlusPlus
		Assert (sizeof (threadID) >= sizeof (int));
		char	buf[1024];
		unsigned int	threadIDInt	=	*reinterpret_cast<unsigned int*> (&threadID);
		if (threadIDInt <= 0xffff) {
			snprintf (buf, NEltsOf (buf), "0x%04x", threadIDInt);
		}
		else {
			snprintf (buf, NEltsOf (buf), "0x%08x", threadIDInt);
		}
		return buf;
	#elif	qUseThreads_WindowsNative
		char	buf[1024];
		if (threadID <= 0xffff) {
			snprintf (buf, NEltsOf (buf), "0x%04x", threadID);
		}
		else {
			snprintf (buf, NEltsOf (buf), "0x%08x", threadID);
		}
		return buf;
	#else
		AssertNotImplemented ();
	#endif
}








/*
 ********************************************************************************
 ************************* Execution::CheckForThreadAborting ********************
 ********************************************************************************
 */
void	Execution::CheckForThreadAborting ()
{
	#if		qUseTLSForSAbortingFlag
		if (s_Aborting) {
			Execution::DoThrow (ThreadAbortException ());
		}
	#elif	qUseSleepExForSAbortingFlag
		::SleepEx (0, true);
	#else
		Assert (false);
		/*
			* I think we could use SleepEx() or WaitForMultipleObjectsEx(), but SleepEx(0,true) may cause a thread to give up
			* the CPU (ask itself to be rescheduled). WaitForMultipleObjectsEx - from the docs - doesn't appear to do this.
			* I think its a lower-cost way to check for a thread being aborted...
			*			-- LGP 2010-10-26
			*/
//				(void)::WaitForMultipleObjectsEx (0, nullptr, false, 0, true);
	#endif
}

