/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Configuration/StroikaConfig.h"

#include    <list>
#include    <sstream>
#if     qPlatform_Windows
#include    <process.h>
#include    <windows.h>
#endif

#include    "../Characters/CString/Utilities.h"
#include    "../Characters/Format.h"
#include    "../Debug/Trace.h"
#include    "../Time/Realtime.h"

#include    "Common.h"
#include    "ErrNoException.h"
#include    "DLLSupport.h"
#include    "Synchronized.h"
#include    "TimeOutException.h"

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
#if     qCompilerAndStdLib_thread_local_with_atomic_keyword_Buggy
    using   TLSInterruptFlagType_  =   volatile bool;
#else
    using   TLSInterruptFlagType_  =   atomic<bool>;
#endif
}


namespace {
#if     qCompilerAndStdLib_thread_local_with_atomic_keyword_Buggy
    using   InterruptSuppressCountType_ =   volatile unsigned int;
#else
    using   InterruptSuppressCountType_ =   atomic<unsigned int>;
#endif
}



namespace {
    mutex                                       sChangeInterruptingMutex_;
    thread_local TLSInterruptFlagType_          s_Aborting_                     { false };
    thread_local TLSInterruptFlagType_          s_Interrupting_                 { false };
    thread_local InterruptSuppressCountType_    s_InterruptionSuppressDepth_    { 0 };          // atomic because updated from one thread but peeked at from another
}







#if         qPlatform_Windows
namespace   {
#if (_WIN32_WINNT < 0x0502)
    namespace XXX {
        struct CLIENT_ID {
            DWORD UniqueProcess;
            DWORD UniqueThread;
        };
        using   NTSTATUS    =   LONG;
#define STATUS_SUCCESS ((NTSTATUS) 0x00000000)
        using   KPRIORITY   =   LONG;
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
        using   pfnNtQueryInformationThread     =   NTSTATUS (__stdcall*) (HANDLE, THREAD_INFORMATION_CLASS, PVOID, ULONG, PULONG);
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
    Synchronized<bool, Synchronized_Traits<bool, recursive_mutex>>  sHandlerInstalled_      { false };
}
#endif


#if     qPlatform_POSIX
// Important to use direct signal handler because we send the signal to a specific thread, and must set a thread local
// variable
SignalHandler   kCallInRepThreadAbortProcSignalHandler_ = SIG_IGN;
#endif







/*
 ********************************************************************************
 ************** Thread::SuppressInterruptionInContext ***************************
 ********************************************************************************
 */
Thread::SuppressInterruptionInContext::SuppressInterruptionInContext ()
{
    s_InterruptionSuppressDepth_++;
}

Thread::SuppressInterruptionInContext::~SuppressInterruptionInContext ()
{
    s_InterruptionSuppressDepth_--;
}




/*
 ********************************************************************************
 ************************************* Thread::Rep_ *****************************
 ********************************************************************************
 */
Thread::Rep_::Rep_ (const Function<void()>& runnable)
    : fRunnable_ (runnable)
    , fThread_ ()
    , fStatus_ (Status::eNotYetRunning)
    , fRefCountBumpedEvent_ (WaitableEvent::eAutoReset)
    , fOK2StartEvent_ (WaitableEvent::eAutoReset)
    , fThreadDone_ (WaitableEvent::eAutoReset)
    , fThreadName_ ()
{
#if     qPlatform_POSIX
    static  bool    sDidInit_ { false };
    if (not sDidInit_) {
        sDidInit_ = true;
        kCallInRepThreadAbortProcSignalHandler_ =   SignalHandler (Rep_::CalledInRepThreadAbortProc_, SignalHandler::Type::eDirect);
    }
#endif
}

void    Thread::Rep_::DoCreate (shared_ptr<Rep_>* repSharedPtr)
{
    TraceContextBumper ctx ("Thread::Rep_::DoCreate");
    RequireNotNull (repSharedPtr);
    RequireNotNull (*repSharedPtr);

#if 0
#if     qPlatform_POSIX
    Platform::POSIX::ScopedBlockCurrentThreadSignal  blockThreadAbortSignal (GetSignalUsedForThreadAbort ());
#endif
#endif
    /*
     *  Once we have constructed the other thread, its critical it be allowed to run at least to the
     *  point where it's bumped its reference count before we allow aborting this thread.
     */
    SuppressInterruptionInContext  suppressInterruptionsOfThisThreadWhileConstructingOtherElseLoseSharedPtrEtc;

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
    try {
        fRunnable_ ();
    }
    catch (const InterruptException&) {
        // Note: intentionally not saved in fSavedException_.
        // See ThrowIfDoneWithException
        throw;
    }
    catch (...) {
        fSavedException_ = current_exception ();
        throw;
    }
}

void    Thread::Rep_::ThreadMain_ (shared_ptr<Rep_>* thisThreadRep) noexcept {
    RequireNotNull (thisThreadRep);
    TraceContextBumper ctx ("Thread::Rep_::ThreadMain_");

    try {
        /*
         * NB: It is important that we do NOT call ::_endthreadex () here because that would cause the
         * shared_ptr<> here to NOT be destroyed. We could force that with an explicit scope, but there
         * is no need, since the docs for _beginthreadex () say that _endthreadex () is called automatically.
         */
        shared_ptr<Rep_> incRefCnt   =   *thisThreadRep; // assure refcount incremented so object not deleted while the thread is running

#if     qCompilerAndStdLib_thread_local_initializers_Buggy
        s_Aborting_ = false;             // reset in case thread re-allocated - TLS may not be properly reinitialized (didn't appear to be on GCC/Linux)
        s_Interrupting_ = false;
        s_InterruptionSuppressDepth_ = 0;
#endif
        /*
         *  Subtle, and not super clearly documented, but this is taking the address of a thread-local variable, and storing it in a non-thread-local
         *  instance, and hoping all that works correctly (that the memory access all work correctly).
         */
        incRefCnt->fTLSAbortFlag_ = &s_Aborting_;
        incRefCnt->fTLSInterruptFlag_ = &s_Interrupting_;

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

            /*
             *  We used to 'SuspendThread' but that was flakey. Instead - wait until the caller says
             *  we really want to start this thread.
             *
             *  But - even that's not good enough. The caller COULD create a thread (which creates this
             *  c++ thread object) but never call Thread::Start(). In which case - this would never return.
             *
             *  Luckily, we can detect that (rare uninteresting) case without much cost. Just wait for a while,
             *  and then check if refcount==1 (so nobody owns this and we're not started yet).
             *      while (not incRefCnt->fOK2StartEvent_.WaitQuietly (5)) {
             *          if (incRefCnt.unique ()) {
             *               Execution::DoThrow (InterruptException ());
             *          }
             *      }
             *
             *  Actually - there is no need todo this. NVM... The caller should be responsible for calling Abort()
             *  the thread it never started.
             *
             *  @todo   MAYBE should change the DTOR for Thead object - so taht if use_count () == 1 or 2 and 'never started' - then
             *          call Abort();
             */
            incRefCnt->fOK2StartEvent_.Wait ();

            DbgTrace (L"In Thread::Rep_::ThreadMain_ - setting state to RUNNING for thread= %s", FormatThreadID (incRefCnt->GetID ()).c_str ());
            bool    doRun   =   false;
            {
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
                incRefCnt->fStatus_ = Status::eCompleted;
            }
            incRefCnt->fThreadDone_.Set ();
        }
        catch (const InterruptException&)
        {
            DbgTrace (L"In Thread::Rep_::ThreadProc_ - setting state to COMPLETED (InterruptException) for thread = %s", FormatThreadID (incRefCnt->GetID ()).c_str ());
            incRefCnt->fStatus_ = Status::eCompleted;
            incRefCnt->fThreadDone_.Set ();
        }
        catch (...)
        {
#if     qPlatform_POSIX
            Platform::POSIX::ScopedBlockCurrentThreadSignal  blockThreadAbortSignal (GetSignalUsedForThreadAbort ());
            {
                lock_guard<mutex>   critSec  { sChangeInterruptingMutex_ };
                s_Aborting_ = false;     //  else .Set() below will THROW EXCPETION and not set done flag!
                s_Interrupting_ = false;
            }
#endif
            DbgTrace (L"In Thread::Rep_::ThreadProc_ - setting state to COMPLETED (EXCEPT) for thread = %s", FormatThreadID (incRefCnt->GetID ()).c_str ());
            {
                incRefCnt->fStatus_ = Status::eCompleted;
            }
            incRefCnt->fThreadDone_.Set ();
        }
    }
    catch (const InterruptException&)
    {
        DbgTrace ("SERIOUS ERORR in Thread::Rep_::ThreadMain_ () - uncaught InterruptException - see sigsetmask stuff above - somehow still not working");
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

void    Thread::Rep_::NotifyOfInteruptionFromAnyThread_ (bool aborting)
{
    Require (fStatus_ == Status::eAborting or fStatus_ == Status::eCompleted);
    //TraceContextBumper ctx ("Thread::Rep_::NotifyOfAbortFromAnyThread_");

    {
        lock_guard<mutex>   critSec  { sChangeInterruptingMutex_ };
        // Harmless todo multiple times - even if already set
        AssertNotNull (fTLSInterruptFlag_);
        *fTLSInterruptFlag_ = true;

        if (aborting) {
            AssertNotNull (fTLSAbortFlag_);
            *fTLSAbortFlag_ = true;
        }
    }

    if (GetCurrentThreadID () == GetID ()) {
#if 1
        Assert (fTLSInterruptFlag_ == &s_Interrupting_);
        Assert (fTLSAbortFlag_ == &s_Aborting_);
        CheckForThreadInterruption ();      // unless suppressed, this will throw

        // NOTE - using CheckForThreadInterruption uses TLS s_Aborting_ instead of fStatus,but I think thats better...
        //  --LGP 2015-02-26
#else
        Assert (s_Aborting_);
        if (fStatus_ == Status::eAborting and s_InterruptionSuppressDepth_ == 0) {
            Execution::DoThrow (AbortException ());
        }
#endif
    }
    // Note we fall through here either if we have throws suppressed, or if sending to another thread

    // @todo note - this used to check fStatus flag and I just changed to checking *fTLSInterruptFlag_ -- LGP 2015-02-26
    if (fStatus_ == Status::eAborting) {
        Assert (*fTLSAbortFlag_);
        Assert (*fTLSInterruptFlag_);       // except maybe possible slight race til I use atomic exchange!!! -- LGP 2015-02-26
        // saw triggered on UNIX, gcc48, 2015-04-03 running regtests
    }
    if (*fTLSInterruptFlag_ /*fStatus_ == Status::eAborting*/) {
#if     qPlatform_POSIX
        {
            DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Wfuture-compat\"");
            auto    critSec { make_unique_lock (sHandlerInstalled_) };
            DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Wfuture-compat\"");
            if (not sHandlerInstalled_)
            {
                SignalHandlerRegistry::Get ().AddSignalHandler (GetSignalUsedForThreadAbort (), kCallInRepThreadAbortProcSignalHandler_);
                sHandlerInstalled_ = true;
            }
        }

        {
            /*
             * siginterupt gaurantees for the given signal - the SA_RESTART flag is not set, so that any pending system calls
             * will return EINTR - which is crucial to our strategy to interupt them!
             */
            Verify (::siginterrupt (GetSignalUsedForThreadAbort (), true) == 0);
        }

        (void)Execution::SendSignal (GetNativeHandle (), GetSignalUsedForThreadAbort ());
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
    // unsafe to call trace code - because called as unsafe handler
    //TraceContextBumper ctx ("Thread::Rep_::CalledInRepThreadAbortProc_");
    //Require (GetCurrentThreadID () == rep->GetID ());         must be true but we dont have the rep as argument
#if 1
#if 0
    // LGP this used to set the TLS flags but they shouldbe bset throurh ptr, and here we dont know which one(s) to set so DONT
    Assert (s_Interrupting_);       // just to debug - technically we cannot assert this because the interrupted
    // thread could handle and clear the flag before the singal handler gets to it....
#endif
#else
    s_Interrupting_ = true;
    s_Aborting_ = true;
#endif

#if 0
    /*
     * siginterupt gaurantees for the given signal - the SA_RESTART flag is not set, so that any pending system calls
     * will return EINTR - which is crucial to our strategy to interupt them!
     */
    Verify (::siginterrupt (signal, true) == 0);
#endif
}
#elif           qPlatform_Windows
void    CALLBACK    Thread::Rep_::CalledInRepThreadAbortProc_ (ULONG_PTR lpParameter)
{
    TraceContextBumper ctx ("Thread::Rep_::CalledInRepThreadAbortProc_");

    Thread::Rep_*   rep =   reinterpret_cast<Thread::Rep_*> (lpParameter);
    Require (GetCurrentThreadID () == rep->GetID ());
#if 1
    // @todo review/test carefully - cahgnged LGP 2015-02-26 to suppor tinterupt and abort
    /*
     *  Note why this is safe here:
     *      o   This callback is called by the APC mechanism only if in an 'alertable state' call, like SleepEx().
     *      o   This also respects the TLS variable (current thread) copy of the suppress throw flags
     *          inside CheckForThreadInterruption()
     */
    Assert (rep->fTLSInterruptFlag_ == &s_Interrupting_);
    Assert (rep->fTLSAbortFlag_ == &s_Aborting_);
    switch (rep->fStatus_) {
        case Status::eAborting:
        case Status::eRunning: {
                CheckForThreadInterruption ();
            }
            break;
    }
#else
    Require (rep->fStatus_ == Status::eAborting or rep->fStatus_ == Status::eCompleted);
    /*
     *  Note - this only gets called by special thread-proces marked as alertable (like sleepex or waitfor...event,
     *  so its safe to throw there.
     */
    Require (GetCurrentThreadID () == rep->GetID ());
    Assert (s_Aborting_);
    // this isn't the race it might look like because this can only be called when the target (rep) thread is in an alertable state, meaning
    // inside a call to SleepEx, etc... so not updating variables
    if (rep->fStatus_ == Status::eAborting) {
        if (s_InterruptionSuppressDepth_ == 0) {
            Execution::DoThrow (AbortException ());
        }
        else {
            return; // dont assert out at the end
        }
    }

    // normally we don't reach this - but we could if we've already been marked completed somehow
    // before the abortProc got called/finsihed...
    Require (rep->fStatus_ == Status::eCompleted);
#endif
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

Thread::Thread (const Function<void()>& fun2CallOnce)
    : fRep_ (shared_ptr<Rep_> (new Rep_ (fun2CallOnce)))
{
    Rep_::DoCreate (&fRep_);
}

Thread::Thread (const Function<void()>& fun2CallOnce, AutoStartFlag)
    : Thread (fun2CallOnce)
{
    Start ();
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
        default:
            RequireNotReached ();
    }
#elif   qPlatform_POSIX
    // for pthreads - use http://man7.org/linux/man-pages/man3/pthread_getschedparam.3.html
    static  bool    sValidPri_ = false;
    static  int     sPriorityMin_;
    static  int     sPriorityMax_;
    {
        if (not sValidPri_) {
            int         sched_policy;
            sched_param param;
            Verify (pthread_getschedparam (pthread_self (), &sched_policy, &param) == 0);
            sPriorityMin_ = sched_get_priority_min (sched_policy);
            sPriorityMax_ = sched_get_priority_max (sched_policy);
            sValidPri_ = true;
        }
    }
    switch (priority) {
        case Priority::eLowest:
            Verify (::pthread_setschedprio (fRep_->GetNativeHandle (), sPriorityMin_) == 0);
            break;
        case Priority::eBelowNormal:
            Verify (::pthread_setschedprio (fRep_->GetNativeHandle (), (sPriorityMax_ - sPriorityMin_) * .25 + sPriorityMin_) == 0);
            break;
        case Priority::eNormal:
            Verify (::pthread_setschedprio (fRep_->GetNativeHandle (), (sPriorityMax_ - sPriorityMin_) * .5 + sPriorityMin_) == 0);
            break;
        case Priority::eAboveNormal:
            Verify (::pthread_setschedprio (fRep_->GetNativeHandle (), (sPriorityMax_ - sPriorityMin_) * .75 + sPriorityMin_) == 0);
            break;
        case Priority::eHighest:
            Verify (::pthread_setschedprio (fRep_->GetNativeHandle (), sPriorityMax_) == 0);
            break;
        default:
            RequireNotReached ();
    }
#else
    AssertNotImplemented ();
#endif
}

#if     qPlatform_POSIX
void    Thread::SetSignalUsedForThreadAbort (SignalID signalNumber)
{
    DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Wfuture-compat\"");
    auto    critSec { make_unique_lock (sHandlerInstalled_) };
    DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Wfuture-compat\"");
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
        TraceContextBumper  ctx ("Execution::Thread::SetThreadName");
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
    Debug::TraceContextBumper ctx ("Thread::Abort");
    if (fRep_.get () == nullptr) {
        // then its effectively already stopped.
        return;
    }
    // not status not protected by critsection, but SB OK for this
    DbgTrace (L"(thread = %s, name='%s', status=%d)", FormatThreadID (GetID ()).c_str (), fRep_->fThreadName_.c_str (), fRep_->fStatus_.load ());

    // first try to send abort exception, and then - if force - get serious!
    // goto aborting, unless the previous value was completed, and then leave it completed.
    if (fRep_->fStatus_.exchange (Status::eAborting) == Status::eCompleted) {
        fRep_->fStatus_ = Status::eCompleted;
    }
    if (fRep_->fStatus_ == Status::eAborting) {
        // by default - tries to trigger a throw-abort-excption in the right thread using UNIX signals or QueueUserAPC ()
        fRep_->NotifyOfInteruptionFromAnyThread_ (true);
    }
}

void    Thread::Interrupt ()
{
    Debug::TraceContextBumper ctx ("Thread::Interrupt");
    if (fRep_.get () == nullptr) {
        // then its effectively already stopped.
        return;
    }
    // not status not protected by critsection, but SB OK for this
    DbgTrace (L"(thread = %s, name='%s', status=%d)", FormatThreadID (GetID ()).c_str (), fRep_->fThreadName_.c_str (), fRep_->fStatus_.load ());

    Status  cs = fRep_->fStatus_.load ();
    if (cs != Status::eAborting and cs != Status::eCompleted) {
        fRep_->NotifyOfInteruptionFromAnyThread_ (false);
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
            catch (const TimeOutException&) {
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

void    Thread::ThrowIfDoneWithException ()
{
    if (fRep_) {
        if (fRep_->fStatus_ == Status::eCompleted and fRep_->fSavedException_) {
            DoReThrow (fRep_->fSavedException_, L"Rethrowing exception across threads");
        }
    }
}

void    Thread::WaitForDoneUntil (Time::DurationSecondsType timeoutAt) const
{
    Debug::TraceContextBumper ctx ("Thread::WaitForDoneUntil");
    //DbgTrace ("(timeout = %.2f)", timeout);
    if (fRep_.get () == nullptr) {
        // then its effectively already done.
        return;
    }
    if (fRep_->fStatus_ == Status::eCompleted) {
        return;
    }
    if (timeoutAt < Time::GetTickCount ()) {
        DoThrow (TimeOutException ());
    }
    bool    doWait  =   false;
    /*
     *  First wait on fThreadDone_. If we get passed it, its safe to block indefinitely (since we're exiting
     *  the thread).
     */
    fRep_->fThreadDone_.WaitUntil (timeoutAt);
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
            DoThrow (TimeOutException ());
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
    return fRep_->fStatus_;
}







/*
 ********************************************************************************
 **************************** Execution::FormatThreadID *************************
 ********************************************************************************
 */
wstring Execution::FormatThreadID (Thread::IDType threadID)
{
    /*
     *  stdc++ doesn't define a way to get the INT thread id, just a string. But they dont format it the
     *  way we usually format a thread ID (hex, fixed width). So do that, so thread IDs look more consistent.
     */
    wstringstream   out;
    out << threadID;

#if     qPlatform_Windows
    constexpr   size_t  kSizeOfThreadID_    =   sizeof (DWORD);                 // All MSFT SDK Thread APIs use DWORD for thread id
#elif   qPlatform_POSIX
    constexpr   size_t  kSizeOfThreadID_    =   sizeof (pthread_t);
#else
    // on MSFT this object is much larger than thread id because it includes handle and id
    // Not a reliable measure anywhere, but probably our best guess
    constexpr   size_t  kSizeOfThreadID_    =   sizeof (Thread::IDType);
#endif

    if (kSizeOfThreadID_ >= sizeof (uint64_t)) {
        uint64_t   threadIDInt =   0;
        out >> threadIDInt;
        return Characters::CString::Format (L"0x%016llx", threadIDInt);
    }
    else {
        uint32_t    threadIDInt =   0;
        out >> threadIDInt;
        /*
         *  Often, it appears ThreadIDs IDs are < 16bits, so making the printout format shorter makes it a bit more readable.
         *
         *  However, I dont see any reliable way to tell this is the case, so don't bother for now. A trouble with checking on
         *  a per-thread-id basis is that often the MAIN THREAD is 0, which is < 0xffff. Then we get one size and then
         *  on the rest a differnt size, so the layout in the debug trace log looks funny.
         */
        constexpr   bool    kUse16BitThreadIDsIfTheyFit_ { false };
        const   bool    kUse16Bit_ = kUse16BitThreadIDsIfTheyFit_ and threadIDInt <= 0xffff;
        if (kUse16Bit_) {
            return Characters::CString::Format (L"0x%04x", threadIDInt);
        }
        else {
            return Characters::CString::Format (L"0x%08x", threadIDInt);
        }
    }
}





/*
 ********************************************************************************
 ********************* Execution::CheckForThreadInterruption ********************
 ********************************************************************************
 */
void    Execution::CheckForThreadInterruption ()
{
    if (s_InterruptionSuppressDepth_ == 0) {
        if (s_Interrupting_) {
            if (s_Aborting_) {
                Assert (s_Interrupting_);   // if s_Aborting_, then s_Interrupting_ must be true
                DoThrow (Thread::AbortException ());
            }
            else {
                lock_guard<mutex>   critSec  { sChangeInterruptingMutex_ };
                s_Interrupting_ = false;
                if (s_Aborting_) {
                    // @todo fix - still racy - we wnat to assure if s_Aborting_, then fTLSInterruptFlag_ true, but tricky... Maybe use exchange()?
                    s_Interrupting_ = true;
                }
                DoThrow (Thread::InterruptException ());
            }
        }
    }
}
