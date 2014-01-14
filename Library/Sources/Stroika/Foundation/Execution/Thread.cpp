/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Configuration/StroikaConfig.h"

#include    <list>
#if     qPlatform_Windows
#include    <process.h>
#include    <windows.h>
#endif

#include    "DLLSupport.h"
#include    "../Characters/CString/Utilities.h"
#include    "../Characters/Format.h"
#include    "../Containers/STL/VectorUtils.h"
#include    "../Debug/Trace.h"
#include    "../Execution/ErrNoException.h"
#include    "../Execution/Lockable.h"
#include    "../Time/Realtime.h"

#if     qPlatform_POSIX
#include    "SignalHandlers.h"
#include    "Platform/POSIX/SignalBlock.h"
#endif
#if     qPlatform_Windows
#include    "Platform/Windows/WaitSupport.h"
#endif

#include    "Thread.h"



using   namespace   Stroika::Foundation;

using   Time::DurationSecondsType;



// Leave this off by default since I'm not sure its safe, and at best it uses some time. But make it
// easy to turn on it release builds...
//      -- LGP 2009-05-28
// According to http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx - its best NOT to call this RaiseException call
// unless a debugger is present. Use IsDebuggerPresent(). Still not perfect.
//
//#define   qSupportSetThreadNameDebuggerCall   0
#ifndef qSupportSetThreadNameDebuggerCall
#if     qDebug && qPlatform_Windows
#define qSupportSetThreadNameDebuggerCall   1
#endif
#endif
#ifndef qSupportSetThreadNameDebuggerCall
#if     qPlatform_POSIX
#define qSupportSetThreadNameDebuggerCall   1
#endif
#endif
#ifndef qSupportSetThreadNameDebuggerCall
#define qSupportSetThreadNameDebuggerCall   0
#endif



using   namespace   Characters;
using   namespace   Execution;


namespace {
    thread_local bool           s_Aborting_  =   false;
    thread_local unsigned int   s_AbortSupressDepth_ = 0;               // doesnt need to be std::atomic because only updated from one thread
}



#if         qPlatform_Windows
namespace   {
#if (_WIN32_WINNT < 0x0502)
    namespace XXX {
        struct CLIENT_ID {
            DWORD UniqueProcess;
            DWORD UniqueThread;
        };
        typedef LONG NTSTATUS;
#define STATUS_SUCCESS ((NTSTATUS) 0x00000000)
        typedef LONG KPRIORITY;
        struct  THREAD_BASIC_INFORMATION {
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
        typedef NTSTATUS (__stdcall* pfnNtQueryInformationThread) (HANDLE, THREAD_INFORMATION_CLASS, PVOID, ULONG, PULONG);
    }
#endif
    DWORD   MyGetThreadId_ (HANDLE thread)
    {
#if     (_WIN32_WINNT >= 0x0502)
        return ::GetThreadId (thread);
#else
        // See details in http://www.codeguru.com/forum/showthread.php?t=355572 on this... - backcompat - only support
        // GetThreadId (HANDLE) in Win 2003 Server or later
        using namespace XXX;
        static  DLLLoader   ntdll (SDKSTR ("ntdll.dll"));
        static  pfnNtQueryInformationThread NtQueryInformationThread = (pfnNtQueryInformationThread)ntdll.GetProcAddress ("NtQueryInformationThread");
        if (NtQueryInformationThread == nullptr)
            return 0;   // failed to get proc address
        THREAD_BASIC_INFORMATION    tbi;
        THREAD_INFORMATION_CLASS    tic = ThreadBasicInformation;
        if (NtQueryInformationThread (thread, tic, &tbi, sizeof (tbi), nullptr) != STATUS_SUCCESS) {
            return 0;
        }
        return tbi.ClientId.UniqueThread;
#endif
    }
}
#endif






using   Debug::TraceContextBumper;




#if     qPlatform_POSIX
namespace   {
    Lockable<bool>  sHandlerInstalled_      =   false;
}
#endif


#if     qPlatform_POSIX
// Important to use direct signal handler because we send the signal to a specific thread, and must set a thread local
// variable
SignalHandler   kCallInRepThreadAbortProcSignalHandler_ = SIG_IGN;
#endif





/*
 ********************************************************************************
 ******************* Thread::SuppressAbortInContext *****************************
 ********************************************************************************
 */
Thread::SuppressAbortInContext::SuppressAbortInContext ()
{
    s_AbortSupressDepth_++;
}

Thread::SuppressAbortInContext::~SuppressAbortInContext ()
{
    s_AbortSupressDepth_--;
}




/*
 ********************************************************************************
 ************************************* Thread::Rep_ *****************************
 ********************************************************************************
 */
Thread::Rep_::Rep_ (const IRunnablePtr& runnable)
    : fRunnable_ (runnable)
    , fTLSAbortFlag_ (nullptr)           // Can only be set properly within the MAINPROC of the thread
    , fThread_ ()
#if     qUSE_MUTEX_FOR_STATUS_FIELD_
    , fStatusCriticalSection_ ()
#endif
    , fStatus_ (Status::eNotYetRunning)
    , fRefCountBumpedEvent_ ()
    , fOK2StartEvent_ ()
    , fThreadDone_ ()
    , fThreadName_ ()
{
#if     qPlatform_POSIX
    static  bool    sDidInit = false;
    if (!sDidInit) {
        sDidInit = true;
        kCallInRepThreadAbortProcSignalHandler_ =   SignalHandler (Rep_::CalledInRepThreadAbortProc_, SignalHandler::Type::eDirect);
    }
#endif
}

void    Thread::Rep_::DoCreate (shared_ptr<Rep_>* repSharedPtr)
{
    TraceContextBumper ctx (SDKSTR ("Thread::Rep_::DoCreate"));
    RequireNotNull (repSharedPtr);
    RequireNotNull (*repSharedPtr);

#if     qPlatform_POSIX
    Platform::POSIX::ScopedBlockCurrentThreadSignal  blockThreadAbortSignal (GetSignalUsedForThreadAbort ());
#endif

    (*repSharedPtr)->fThread_ = thread ([&repSharedPtr]() -> void { ThreadMain_ (repSharedPtr); });
    try {
        (*repSharedPtr)->fRefCountBumpedEvent_.Wait (); // assure we wait for this, so we don't ever let refcount go to zero before the
        // thread has started...
    }
    catch (...) {
        //???
        Execution::DoReThrow ();
    }
}

Thread::Rep_::~Rep_ ()
{
    Assert (fStatus_ != Status::eRunning);
    /*
     *  Use thread::detach() - since we have no desire to wait, and detach
     *  will cause all resources for the thread to be deleted once the thread
     *  terminates.
     *
     *  From http://en.cppreference.com/w/cpp/thread/thread/detach:
     *      Separates the thread of execution from the thread object, allowing execution to continue
     *      independently. Any allocated resources will be freed once the thread exits.
     */
    if (fThread_.joinable  ()) {
        fThread_.detach ();
    }
}

void    Thread::Rep_::Run_ ()
{
    fRunnable_->Run ();
}

void    Thread::Rep_::ThreadMain_ (shared_ptr<Rep_>* thisThreadRep) noexcept {
    RequireNotNull (thisThreadRep);
    TraceContextBumper ctx (SDKSTR ("Thread::Rep_::ThreadMain_"));

    try {
        /*
         * NB: It is important that we do NOT call ::_endthreadex () here because that would cause the
         * shared_ptr<> here to NOT be destroyed. We could force that with an explicit scope, but there
         * is no need, since the docs for _beginthreadex () say that _endthreadex () is called automatically.
         */
        shared_ptr<Rep_> incRefCnt   =   *thisThreadRep; // assure refcount incremented so object not deleted while the thread is running

#if     qCompilerAndStdLib_thread_local_initializers_Buggy
        s_Aborting_ = false;             // reset in case thread re-allocated - TLS may not be properly reinitialized (didn't appear to be on GCC/Linux)
        s_AbortSupressDepth_ = 0;
#endif
        incRefCnt->fTLSAbortFlag_ = &s_Aborting_;

        try {
            // We cannot possibly get interupted BEFORE this - because only after this fRefCountBumpedEvent_ does the rest of the APP know about our thread ID
            // baring an external process sending us a bogus signal)
            //
            // Note that BOTH the fRefCountBumpedEvent_ and the fOK2StartEvent_ wait MUST come inside the try/catch for
            incRefCnt->fRefCountBumpedEvent_.Set ();

#if     qPlatform_POSIX
            {
                // we inherit blocked abort signal given how we are created in DoCreate() - so unblock it - and acept aborts after we've marked
                // reference count as set.
                sigset_t    mySet;
                sigemptyset (&mySet);
                (void)::sigaddset (&mySet, GetSignalUsedForThreadAbort ());
                Verify (pthread_sigmask (SIG_UNBLOCK,  &mySet, nullptr) == 0);
            }
#endif

            incRefCnt->fOK2StartEvent_.Wait (); // we used to 'SuspendThread' but that was flakey. Instead - wait until teh caller says
            // we really want to start this thread.

            DbgTrace (L"In Thread::Rep_::ThreadMain_ - setting state to RUNNING for thread= %s", FormatThreadID (incRefCnt->GetID ()).c_str ());
            bool    doRun   =   false;
            {
#if     qUSE_MUTEX_FOR_STATUS_FIELD_
                lock_guard<recursive_mutex> enterCritcalSection (incRefCnt->fStatusCriticalSection_);
#endif
                if (incRefCnt->fStatus_ == Status::eNotYetRunning)
                {
                    incRefCnt->fStatus_ = Status::eRunning;
                    doRun = true;
                }
            }
            if (doRun)
            {
                incRefCnt->Run_ ();
            }
            DbgTrace (L"In Thread::Rep_::ThreadProc_ - setting state to COMPLETED for thread= %s", FormatThreadID (incRefCnt->GetID ()).c_str ());
            {
#if     qUSE_MUTEX_FOR_STATUS_FIELD_
                lock_guard<recursive_mutex> enterCritcalSection (incRefCnt->fStatusCriticalSection_);
#endif
                incRefCnt->fStatus_ = Status::eCompleted;
            }
            incRefCnt->fThreadDone_.Set ();
        }
        catch (const ThreadAbortException&)
        {
            /// vaguely #if     qUSE_MUTEX_FOR_STATUS_FIELD related - but not quite.... - not just...
            /// --LGP 2014-01-14 change...
#if 1
            DbgTrace (L"In Thread::Rep_::ThreadProc_ - setting state to COMPLETED (ThreadAbortException) for thread = %s", FormatThreadID (incRefCnt->GetID ()).c_str ());
#if     qUSE_MUTEX_FOR_STATUS_FIELD_
            {
                lock_guard<recursive_mutex> enterCritcalSection (incRefCnt->fStatusCriticalSection_);
                incRefCnt->fStatus_ = Status::eCompleted;
            }
#else
            incRefCnt->fStatus_ = Status::eCompleted;
#endif
#else
#if     qPlatform_POSIX
            Platform::POSIX::ScopedBlockCurrentThreadSignal  blockThreadAbortSignal (GetSignalUsedForThreadAbort ());
            s_Aborting_ = false;     //  else .Set() below will THROW EXCPETION and not set done flag!
#endif
            DbgTrace (L"In Thread::Rep_::ThreadProc_ - setting state to COMPLETED (ThreadAbortException) for thread = %s", FormatThreadID (incRefCnt->GetID ()).c_str ());
            {
                lock_guard<recursive_mutex> enterCritcalSection (incRefCnt->fStatusCriticalSection_);
                incRefCnt->fStatus_ = Status::eCompleted;
            }
#endif
            incRefCnt->fThreadDone_.Set ();
        }
        catch (...)
        {
#if     qPlatform_POSIX
            Platform::POSIX::ScopedBlockCurrentThreadSignal  blockThreadAbortSignal (GetSignalUsedForThreadAbort ());
            s_Aborting_ = false;     //  else .Set() below will THROW EXCPETION and not set done flag!
#endif
            DbgTrace (L"In Thread::Rep_::ThreadProc_ - setting state to COMPLETED (EXCEPT) for thread = %s", FormatThreadID (incRefCnt->GetID ()).c_str ());
            {
#if     qUSE_MUTEX_FOR_STATUS_FIELD_
                lock_guard<recursive_mutex> enterCritcalSection (incRefCnt->fStatusCriticalSection_);
#endif
                incRefCnt->fStatus_ = Status::eCompleted;
            }
            incRefCnt->fThreadDone_.Set ();
        }
    }
    catch (const ThreadAbortException&)
    {
        DbgTrace ("SERIOUS ERORR in Thread::Rep_::ThreadMain_ () - uncaught ThreadAbortException - see sigsetmask stuff above - somehow still not working");
//SB ASSERT BUT DISABLE SO I CAN DEBUG OTHER STUFF FIRST
// TI THINK ISSUE IS
        AssertNotReached ();    // This should never happen - but if it does - better a trace message in a tracelog than 'unexpected' being called (with no way out)
    }
    catch (...)
    {
        DbgTrace ("SERIOUS ERORR in Thread::Rep_::ThreadMain_ () - uncaught exception");

//SB ASSERT BUT DISABLE SO I CAN DEBUG OTHER STUFF FIRST
// TI THINK ISSUE IS
        AssertNotReached ();    // This should never happen - but if it does - better a trace message in a tracelog than 'unexpected' being called (with no way out)
    }
}

void    Thread::Rep_::NotifyOfAbortFromAnyThread_ ()
{
    Require (fStatus_ == Status::eAborting or fStatus_ == Status::eCompleted);
    //TraceContextBumper ctx (SDKSTR ("Thread::Rep_::NotifyOfAbortFromAnyThread_"));

    // Harmless todo multiple times - even if already set
    AssertNotNull (fTLSAbortFlag_);
    *fTLSAbortFlag_ = true;

    if (GetCurrentThreadID () == GetID ()) {
        ThrowAbortIfNeededFromRepThread_ ();
    }

#if     qUSE_MUTEX_FOR_STATUS_FIELD_
    lock_guard<recursive_mutex> enterCritcalSection (fStatusCriticalSection_);
#endif
    if (fStatus_ == Status::eAborting) {
#if     qPlatform_POSIX
        {
            lock_guard<recursive_mutex> critSec (sHandlerInstalled_);
            if (not sHandlerInstalled_)
            {
                SignalHandlerRegistry::Get ().AddSignalHandler (GetSignalUsedForThreadAbort (), kCallInRepThreadAbortProcSignalHandler_);
                sHandlerInstalled_ = true;
            }
        }
        Execution::SendSignal (GetNativeHandle (), GetSignalUsedForThreadAbort ());
#elif   qPlatform_Windows
        Verify (::QueueUserAPC (&CalledInRepThreadAbortProc_, GetNativeHandle (), reinterpret_cast<ULONG_PTR> (this)));
#endif
    }
}

Thread::IDType  Thread::Rep_::GetID () const
{
    return fThread_.get_id ();
}

Thread::NativeHandleType    Thread::Rep_::GetNativeHandle ()
{
    return fThread_.native_handle ();
}

#if     qPlatform_POSIX
void    Thread::Rep_::CalledInRepThreadAbortProc_ (SignalID signal)
{
    TraceContextBumper ctx (SDKSTR ("Thread::Rep_::CalledInRepThreadAbortProc_"));
    //Require (GetCurrentThreadID () == rep->GetID ()); must be true but we dont have the rep as argument
    s_Aborting_ = true;
    /*
     * siginterupt gaurantees for the given signal - the SA_RESTART flag is not set, so that any pending system calls
     * will return EINTR - which is crucial to our strategy to interupt them!
     */
    Verify (::siginterrupt (signal, true) == 0);
}
#elif           qPlatform_Windows
void    CALLBACK    Thread::Rep_::CalledInRepThreadAbortProc_ (ULONG_PTR lpParameter)
{
    TraceContextBumper ctx (SDKSTR ("Thread::Rep_::CalledInRepThreadAbortProc_"));
    s_Aborting_ = true;
    Thread::Rep_*   rep =   reinterpret_cast<Thread::Rep_*> (lpParameter);
    Require (rep->fStatus_ == Status::eAborting or rep->fStatus_ == Status::eCompleted);
    /*
     *  Note - this only gets called by special thread-proces marked as alertable (like sleepex or waitfor...event,
     *  so its safe to throw there.
     */
    Require (GetCurrentThreadID () == rep->GetID ());
    rep->ThrowAbortIfNeededFromRepThread_ ();
    // normally we don't reach this - but we could if we've already been marked completed somehow
    // before the abortProc got called/finsihed...
    Require (rep->fStatus_ == Status::eCompleted);
}
#endif








/*
 ********************************************************************************
 *********************************** Thread *************************************
 ********************************************************************************
 */
#if     qPlatform_POSIX
SignalID        Thread::sSignalUsedForThreadAbort_  =   SIGUSR2;
#endif

Thread::Thread ()
    : fRep_ ()
{
}

Thread::Thread (const std::function<void()>& fun2CallOnce)
    : fRep_ (shared_ptr<Rep_> (DEBUG_NEW Rep_ (mkIRunnablePtr (fun2CallOnce))))
{
    Rep_::DoCreate (&fRep_);
}

Thread::Thread (const IRunnablePtr& runnable)
    : fRep_ (shared_ptr<Rep_> (DEBUG_NEW Rep_ (runnable)))
{
    Rep_::DoCreate (&fRep_);
}

void    Thread::SetThreadPriority (Priority priority)
{
    RequireNotNull (fRep_);
#if         qPlatform_Windows
    switch (priority) {
        case Priority::eLowest:
            Verify (::SetThreadPriority (GetNativeHandle (), THREAD_PRIORITY_LOWEST));
            break;
        case Priority::eBelowNormal:
            Verify (::SetThreadPriority (GetNativeHandle (), THREAD_PRIORITY_BELOW_NORMAL));
            break;
        case Priority::eNormal:
            Verify (::SetThreadPriority (GetNativeHandle (), THREAD_PRIORITY_NORMAL));
            break;
        case Priority::eAboveNormal:
            Verify (::SetThreadPriority (GetNativeHandle (), THREAD_PRIORITY_ABOVE_NORMAL));
            break;
        case Priority::eHighest:
            Verify (::SetThreadPriority (GetNativeHandle (), THREAD_PRIORITY_HIGHEST));
            break;
    }
#else
    // for pthreads - use http://man7.org/linux/man-pages/man3/pthread_getschedparam.3.html
    AssertNotImplemented();
#endif
}

#if     qPlatform_POSIX
void    Thread::SetSignalUsedForThreadAbort (SignalID signalNumber)
{
    lock_guard<recursive_mutex> critSec (sHandlerInstalled_);
    if (sHandlerInstalled_) {
        SignalHandlerRegistry::Get ().RemoveSignalHandler (GetSignalUsedForThreadAbort (), kCallInRepThreadAbortProcSignalHandler_);
        sHandlerInstalled_ = false;
    }
    sSignalUsedForThreadAbort_ = signalNumber;
    // install new handler
    if (not sHandlerInstalled_) {
        SignalHandlerRegistry::Get ().AddSignalHandler (GetSignalUsedForThreadAbort (), kCallInRepThreadAbortProcSignalHandler_);
        sHandlerInstalled_ = true;
    }
}
#endif

void    Thread::SetThreadName (const wstring& threadName)
{
    RequireNotNull (fRep_);
    if (fRep_->fThreadName_ != threadName) {
        TraceContextBumper  ctx (SDKSTR ("Execution::Thread::SetThreadName"));
        DbgTrace (L"(ThreadName = '%s')", threadName.c_str ());
        fRep_->fThreadName_ = threadName;
#if     qSupportSetThreadNameDebuggerCall
#if     qPlatform_Windows
        if (::IsDebuggerPresent ()) {
            // This hack from http://www.codeproject.com/KB/threads/Name_threads_in_debugger.aspx
            struct THREADNAME_INFO {
                DWORD dwType;       // must be 0x1000
                LPCSTR szName;      // pointer to name (in user addr space)
                DWORD dwThreadID;   // thread ID (-1=caller thread)
                DWORD dwFlags;      // reserved for future use, must be zero
            };
            string  useThreadName   =   WideStringToNarrowSDKString (threadName);
            THREADNAME_INFO info;
            {
                info.dwType = 0x1000;
                info.szName = useThreadName.c_str ();
                info.dwThreadID = MyGetThreadId_ (fRep_->fThread_.native_handle ());
                info.dwFlags = 0;
            }
            IgnoreExceptionsForCall (::RaiseException (0x406D1388, 0, sizeof(info) / sizeof(DWORD), (ULONG_PTR*)&info));
        }
#elif   qPlatform_POSIX
        // could have called prctl(PR_SET_NAME,"<null> terminated string",0,0,0) - but seems less portable
        //
        // according to http://man7.org/linux/man-pages/man3/pthread_setname_np.3.html - the length max is 15 characters
        string tnUTF8 = String (threadName).AsUTF8 ();
        if (tnUTF8.length () > 15) {
            tnUTF8.erase (15);
        }
        pthread_setname_np (fRep_->GetNativeHandle (), tnUTF8.c_str ());
#endif
#endif
    }
}

void    Thread::Start ()
{
    RequireNotNull (fRep_);
    Require (GetStatus () == Status::eNotYetRunning);
    DbgTrace (L"Thread::Start: (thread = %s, name='%s')", FormatThreadID (GetID ()).c_str (), fRep_->fThreadName_.c_str ());
    fRep_->fOK2StartEvent_.Set ();
}

void    Thread::Abort ()
{
    Debug::TraceContextBumper ctx (SDKSTR ("Thread::Abort"));
    if (fRep_.get () == nullptr) {
        // then its effectively already stopped.
        return;
    }
    // not status not protected by critsection, but SB OK for this
    DbgTrace (L"(thread = %s, name='%s', status=%d)", FormatThreadID (GetID ()).c_str (), fRep_->fThreadName_.c_str (), fRep_->fStatus_);

    // first try to send abort exception, and then - if force - get serious!
#if     qUSE_MUTEX_FOR_STATUS_FIELD_
    lock_guard<recursive_mutex> enterCritcalSection (fRep_->fStatusCriticalSection_);
    if (fRep_->fStatus_ != Status::eCompleted) {
        fRep_->fStatus_ = Status::eAborting;
    }
#else
    // goto aborting, unless the previous value was completed, and then leave it completed.
    if (fRep_->fStatus_.exchange (Status::eAborting) == Status::eCompleted) {
        fRep_->fStatus_ = Status::eCompleted;
    }
#endif
    if (fRep_->fStatus_ == Status::eAborting) {
        // by default - tries to trigger a throw-abort-excption in the right thread using UNIX signals or QueueUserAPC ()
        fRep_->NotifyOfAbortFromAnyThread_ ();
    }
}

void    Thread::Abort_Forced_Unsafe ()
{
    if (fRep_.get () == nullptr) {
        // then its effectively already stopped.
        return;
    }

    Abort ();

    // Wait some reasonable amount of time for the thread to abort
    IgnoreExceptionsForCall (WaitForDone (5.0f));
    AssertNotImplemented ();
}

void    Thread::AbortAndWaitForDone (Time::DurationSecondsType timeout)
{
    Time::DurationSecondsType   endTime =   Time::GetTickCount () + timeout;
    // an abort may need to be resent (since there could be a race and we may need to force wakeup again)
    unsigned int tries = 0;
    while (true) {
        const   Time::DurationSecondsType   kTimeBetweenAborts_     =   1.0f;
        Abort ();
        tries++;
        Time::DurationSecondsType   timeLeft    =   endTime - Time::GetTickCount ();
        if (timeLeft <= kTimeBetweenAborts_) {
            WaitForDone (timeLeft);     // throws if we should throw
            return;
        }
        else {
            // If timeLeft BIG - ignore timeout exception and go through loop again
            try {
                WaitForDone (kTimeBetweenAborts_);
                return;
            }
            catch (const WaitTimedOutException&) {
            }
        }
        if (tries <= 1) {
            // this COULD happen due to a lucky race - OR - the code could just be BUSY for a while (not calling CheckForAborted). But even then - it COULD make
            // a blocking call which needs interuption.
            DbgTrace ("This should ALMOST NEVER happen - where we did an abort but it came BEFORE the system call and so needs to be called again to re-interupt.");
        }
        else {
            DbgTrace ("OK - maybe the target thread is ingoring abort exceptions? try/catch/ignore?");
        }
    }
}

void    Thread::WaitForDone (Time::DurationSecondsType timeout) const
{
    Debug::TraceContextBumper ctx (SDKSTR ("Thread::WaitForDone"));
    //DbgTrace ("(timeout = %.2f)", timeout);
    if (fRep_.get () == nullptr) {
        // then its effectively already done.
        return;
    }
    if (fRep_->fStatus_ == Status::eCompleted) {
        return;
    }
    if (timeout < 0) {
        DoThrow (WaitTimedOutException ());
    }
    bool    doWait  =   false;
    /*
     *  First wait on fThreadDone_. If we get passed it, its safe to block indefinitely (since we're exiting
     *  the thread).
     */
    fRep_->fThreadDone_.Wait (timeout);
    // If not joinable, presume that means cuz its done
    if (fRep_->fThread_.joinable  ()) {
        // this will block indefinitely - but if a timeout was specified
        fRep_->fThread_.join ();
    }
}

#if     qPlatform_Windows
void    Thread::WaitForDoneWhilePumpingMessages (Time::DurationSecondsType timeout) const
{
    if (fRep_.get () == nullptr) {
        // then its effectively already done.
        return;
    }
    HANDLE  thread  =   fRep_->GetNativeHandle ();
    if (thread == INVALID_HANDLE_VALUE) {
        return;
    }
    DurationSecondsType timeoutAt   =   Time::GetTickCount () + timeout;
    // CRUDDY impl - but decent enuf for first draft
    while (GetStatus () != Thread::Status::eCompleted) {
        DurationSecondsType     time2Wait   =   timeoutAt - Time::GetTickCount ();
        if (time2Wait <= 0) {
            DoThrow (WaitTimedOutException ());
        }
        Platform::Windows::WaitAndPumpMessages (nullptr, { thread }, time2Wait);
    }
}
#endif

Thread::Status  Thread::GetStatus_ () const noexcept
{
    Require (fRep_.get () != nullptr);
    if (fRep_.get () == nullptr) {
        return Status::eNull;
    }
#if     qUSE_MUTEX_FOR_STATUS_FIELD_
    lock_guard<recursive_mutex> enterCritcalSection (fRep_->fStatusCriticalSection_);
#endif
    return fRep_->fStatus_;
}







/*
 ********************************************************************************
 **************************** Execution::FormatThreadID *************************
 ********************************************************************************
 */
wstring Execution::FormatThreadID (Thread::IDType threadID)
{
    // it appears these IDs are < 16bits, so making the printout format shorter makes it a bit more readable.
    Assert (sizeof (threadID) >= sizeof (int));
    if (sizeof (Thread::IDType) >= sizeof (uint64_t)) {
        uint64_t    threadIDInt =   *reinterpret_cast<uint64_t*> (&threadID);
        return Characters::CString::Format (L"0x%016lx", threadIDInt);
    }
    else {
        //
        // @todo: this case is wrong - sizeof(unsigned int) could be 64bits, but sizeof(threadID) smaller so peeking at bad data!!!
        //
        unsigned int    threadIDInt =   *reinterpret_cast<unsigned int*> (&threadID);
        if (threadIDInt <= 0xffff) {
            return Characters::CString::Format (L"0x%04x", threadIDInt);
        }
        else {
            return Characters::CString::Format (L"0x%08x", threadIDInt);
        }
    }
    AssertNotImplemented ();
}








/*
 ********************************************************************************
 ************************* Execution::CheckForThreadAborting ********************
 ********************************************************************************
 */
void    Execution::CheckForThreadAborting ()
{
    if (s_Aborting_ and s_AbortSupressDepth_ == 0) {
        Execution::DoThrow (ThreadAbortException ());
    }
//      http://bugzilla/show_bug.cgi?id=646
//      I THINK this is obsolete.... throw should  be fine

// RE-ENABLE for windows - qUseSleepExForSAbortingFlag - this is BAD, NOT GOOD.
// MUST get this working again...
//  -- LGP 2013-03-26


//#define qUseSleepExForSAbortingFlag 0
#ifndef qUseSleepExForSAbortingFlag
#define qUseSleepExForSAbortingFlag qPlatform_Windows
#endif
#if     qUseSleepExForSAbortingFlag
    /*
        * I think we could use SleepEx() or WaitForMultipleObjectsEx(), but SleepEx(0,true) may cause a thread to give up
        * the CPU (ask itself to be rescheduled). WaitForMultipleObjectsEx - from the docs - doesn't appear to do this.
        * I think its a lower-cost way to check for a thread being aborted...
        *           -- LGP 2010-10-26
        */
//      (void)::WaitForMultipleObjectsEx (0, nullptr, false, 0, true);
    ::SleepEx (0, true);
#endif
}

