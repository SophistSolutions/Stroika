/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Configuration/StroikaConfig.h"

#include <list>
#include <sstream>
#if qPlatform_Windows
#include <process.h>
#include <windows.h>
#endif

#include "../Characters/CString/Utilities.h"
#include "../Characters/Format.h"
#include "../Characters/String.h"
#include "../Characters/String_Constant.h"
#include "../Characters/ToString.h"
#include "../Containers/Set.h"
#include "../Debug/Trace.h"
#include "../Time/Realtime.h"

#include "Common.h"
#include "DLLSupport.h"
#include "Exceptions.h"
#include "Synchronized.h"
#include "TimeOutException.h"

#if qPlatform_POSIX
#include "Platform/POSIX/SignalBlock.h"
#include "SignalHandlers.h"
#endif
#if qPlatform_Windows
#include "Platform/Windows/WaitSupport.h"
#endif

#include "Thread.h"

using namespace Stroika::Foundation;

using Containers::Set;
using Time::DurationSecondsType;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

// Leave this off by default since I'm not sure its safe, and at best it uses some time. But make it
// easy to turn on it release builds...
//      -- LGP 2009-05-28
// According to http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx - its best NOT to call this RaiseException call
// unless a debugger is present. Use IsDebuggerPresent(). Still not perfect.
//
//#define   qSupportSetThreadNameDebuggerCall_   0
#ifndef qSupportSetThreadNameDebuggerCall_
#if qDebug && qPlatform_Windows
#define qSupportSetThreadNameDebuggerCall_ 1
#endif
#endif
#ifndef qSupportSetThreadNameDebuggerCall_
#if qPlatform_POSIX
#define qSupportSetThreadNameDebuggerCall_ 1
#endif
#endif
#ifndef qSupportSetThreadNameDebuggerCall_
#define qSupportSetThreadNameDebuggerCall_ 0
#endif

using namespace Characters;
using namespace Execution;

namespace {
    // Used to use 'atomic' but not needed, bcause always used from the same thread (thread local)
    using InterruptSuppressCountType_ = unsigned int;
}

namespace {
    using PRIVATE_::InterruptFlagState_;
    using PRIVATE_::InterruptFlagType_;
}

namespace {
    thread_local InterruptFlagType_          t_Interrupting_{InterruptFlagState_::eNone};
    thread_local InterruptSuppressCountType_ t_InterruptionSuppressDepth_{0};
}

#if qDebug
namespace {
    /*
     *  This wont work 100% of the time, but try to detect threads running before main, or after the end of main ()
     */
    bool sKnownBadBeforeMainOrAfterMain_{true};
    struct MainDetector_ {
        MainDetector_ () { sKnownBadBeforeMainOrAfterMain_ = false; }
        ~MainDetector_ () { sKnownBadBeforeMainOrAfterMain_ = true; }
    };
    MainDetector_ sMainDetector_;
}
#endif

#if qStroika_Foundation_Exection_Thread_SupportThreadStatistics
namespace {
    // use mutext and set<> to avoid interdependencies between low level Stroika facilities
    mutex               sThreadSupportStatsMutex_;
    set<Thread::IDType> sRunningThreads_; // protected by sThreadSupportStatsMutex_

    struct AllThreadsDeadDetector_ {
        AllThreadsDeadDetector_ ()
        {
            Require (sRunningThreads_.empty ());
        }
        ~AllThreadsDeadDetector_ ()
        {
#if qDebug
            if (not sRunningThreads_.empty ()) {
                DbgTrace (L"Threads %s running", Characters::ToString (Thread::GetStatistics ().fRunningThreads).c_str ());
                Require (sRunningThreads_.empty ());
            }
#endif
        }
    };
    AllThreadsDeadDetector_ sAllThreadsDeadDetector_;
}
#endif

#if qPlatform_Windows
namespace {
#if (_WIN32_WINNT < 0x0502)
    namespace XXX {
        struct CLIENT_ID {
            DWORD UniqueProcess;
            DWORD UniqueThread;
        };
        using NTSTATUS = LONG;
#define STATUS_SUCCESS ((NTSTATUS)0x00000000)
        using KPRIORITY = LONG;
        struct THREAD_BASIC_INFORMATION {
            NTSTATUS  ExitStatus;
            PVOID     TebBaseAddress;
            CLIENT_ID ClientId;
            KAFFINITY AffinityMask;
            KPRIORITY Priority;
            KPRIORITY BasePriority;
        };
        enum THREAD_INFORMATION_CLASS {
            ThreadBasicInformation = 0,
        };
        using pfnNtQueryInformationThread = NTSTATUS (__stdcall*) (HANDLE, THREAD_INFORMATION_CLASS, PVOID, ULONG, PULONG);
    }
#endif
    DWORD MyGetThreadId_ (HANDLE thread)
    {
#if (_WIN32_WINNT >= 0x0502)
        return ::GetThreadId (thread);
#else
        // See details in http://www.codeguru.com/forum/showthread.php?t=355572 on this... - backcompat - only support
        // GetThreadId (HANDLE) in Win 2003 Server or later
        using namespace XXX;
        static DLLLoader                   ntdll (SDKSTR ("ntdll.dll"));
        static pfnNtQueryInformationThread NtQueryInformationThread = (pfnNtQueryInformationThread)ntdll.GetProcAddress ("NtQueryInformationThread");
        if (NtQueryInformationThread == nullptr)
            return 0; // failed to get proc address
        THREAD_BASIC_INFORMATION tbi;
        THREAD_INFORMATION_CLASS tic = ThreadBasicInformation;
        if (NtQueryInformationThread (thread, tic, &tbi, sizeof (tbi), nullptr) != STATUS_SUCCESS) {
            return 0;
        }
        return tbi.ClientId.UniqueThread;
#endif
    }
}
#endif

using Debug::TraceContextBumper;

#if qPlatform_POSIX
namespace {
    Synchronized<bool> sHandlerInstalled_{false};
}
#endif

#if qPlatform_POSIX
// Important to use direct signal handler because we send the signal to a specific thread, and must set a thread local
// variable
SignalHandler kCallInRepThreadAbortProcSignalHandler_ = SIG_IGN;
#endif

#if qCompiler_cpp17InlineStaticMemberOfTemplateLinkerUndefined_Buggy
/*
 ********************************************************************************
 **************************** Configuration::DefaultNames ***********************
 ********************************************************************************
 */
namespace Stroika::Foundation::Configuration {
    constexpr EnumNames<Execution::Thread::Status> DefaultNames<Execution::Thread::Status>::k;
}
#endif

/*
 ********************************************************************************
 ************** Thread::SuppressInterruptionInContext ***************************
 ********************************************************************************
 */
Thread::SuppressInterruptionInContext::SuppressInterruptionInContext ()
{
    t_InterruptionSuppressDepth_++;
}

Thread::SuppressInterruptionInContext::~SuppressInterruptionInContext ()
{
    Assert (t_InterruptionSuppressDepth_ >= 1);
    t_InterruptionSuppressDepth_--;
    /*
     *  Would LIKE to do:
     *
     *  if (t_InterruptionSuppressDepth_ == 0 and t_Interrupting_ != InterruptFlagState_::eNone) {
     *      DbgTrace (L"~SuppressInterruptionInContext () completing with interruption pending, so this thread will interupt at the next cancelation point");
     *  }
     *  But cannot safely/easily, because DbgTrace internally uses SuppressInterruptionInContext!
     */
}

/*
 ********************************************************************************
 ********************** Thread::InterruptException ******************************
 ********************************************************************************
 */
Thread::InterruptException::InterruptException ()
    : InterruptException (L"Thread Interrupt"sv)
{
}

Thread::InterruptException::InterruptException (const Characters::String& msg)
    : Exception<> (msg)
{
}
const Thread::InterruptException Thread::InterruptException::kThe;

/*
 ********************************************************************************
 ************************** Thread::AbortException ******************************
 ********************************************************************************
 */
Thread::AbortException::AbortException ()
    : InterruptException (L"Thread Abort"sv)
{
}

const Thread::AbortException Thread::AbortException::kThe;

/*
 ********************************************************************************
 **************************** Characters::ToString ******************************
 ********************************************************************************
 */
namespace Stroika::Foundation::Characters {
    template <>
    String ToString (const thread::id& t)
    {
        return String::FromASCII (Execution::FormatThreadID_A (t));
    }
}

/*
 ********************************************************************************
 ********************************** Thread::Rep_ ********************************
 ********************************************************************************
 */
Thread::Rep_::Rep_ (const function<void ()>& runnable, [[maybe_unused]] const optional<Configuration>& configuration)
    : fRunnable_ (runnable)
{
    // @todo - never used anything from configuration (yet) - should!)
#if qPlatform_POSIX
    static bool sDidInit_{false}; // initialize after main() started, but before any threads
    if (not sDidInit_) {
        sDidInit_                               = true;
        kCallInRepThreadAbortProcSignalHandler_ = SignalHandler (Rep_::InterruptionSignalHandler_, SignalHandler::Type::eDirect);
    }
#elif qPlatform_Windows
    if (configuration.has_value () and configuration->fThrowInterruptExceptionInsideUserAPC.has_value ()) {
        fThrowInterruptExceptionInsideUserAPC_ = configuration->fThrowInterruptExceptionInsideUserAPC.value ();
    }
#endif
}

void Thread::Rep_::DoCreate (const shared_ptr<Rep_>* repSharedPtr)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx{L"Thread::Rep_::DoCreate"};
#endif
    RequireNotNull (repSharedPtr);
    RequireNotNull (*repSharedPtr);

    /*
     *  Once we have constructed the other thread, its critical it be allowed to run at least to the
     *  point where it's bumped its reference count before we allow aborting this thread.
     */
    SuppressInterruptionInContext suppressInterruptionsOfThisThreadWhileConstructingOtherElseLoseSharedPtrEtc;

    {
        lock_guard<mutex> critSec{(*repSharedPtr)->fAccessSTDThreadMutex_};
        (*repSharedPtr)->fThread_ = thread ([&repSharedPtr] () -> void { ThreadMain_ (repSharedPtr); });
    }
    try {
        (*repSharedPtr)->fRefCountBumpedEvent_.Wait (); // assure we wait for this, so we don't ever let refcount go to zero before the thread has started
    }
    catch (...) {
        AssertNotReached ();
        Execution::ReThrow ();
    }
    (*repSharedPtr)->ApplyThreadName2OSThreadObject ();
}

Thread::Rep_::~Rep_ ()
{
    Assert (fStatus_ != Status::eRunning);

    /*
     *  Use thread::detach() - since this could be called from another thread, or from the
     *  thread which fThread_ refers to. Calling from the later case thread would deadlock
     *  and is a C++ error to call.
     *
     *  thread::detach will cause all resources for the thread to be deleted once the thread
     *  terminates.
     *
     *  From http://en.cppreference.com/w/cpp/thread/thread/detach:
     *      Separates the thread of execution from the thread object, allowing execution to continue
     *      independently. Any allocated resources will be freed once the thread exits.
     *
     * no need for lock_guard<mutex>   critSec  { fAccessSTDThreadMutex_ }; because if destroying, only one thread can reference this smart-ptr
     */
    if (fThread_.joinable ()) {
        fThread_.detach ();
    }
}

void Thread::Rep_::Run_ ()
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

Characters::String Thread::Rep_::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    sb += L"id: " + Characters::ToString (GetID ()) + L", ";
    if (not fThreadName_.empty ()) {
        sb += L"name: '" + fThreadName_ + L"', ";
    }
    sb += L"status: " + Characters::ToString (fStatus_.load ());
    sb += L"}";
    return sb.str ();
}

void Thread::Rep_::ApplyThreadName2OSThreadObject ()
{
    if (GetNativeHandle () != NativeHandleType{}) {
#if qSupportSetThreadNameDebuggerCall_
#if qPlatform_Windows
        if (::IsDebuggerPresent ()) {
            // This hack from http://www.codeproject.com/KB/threads/Name_threads_in_debugger.aspx
            struct THREADNAME_INFO {
                DWORD  dwType;     // must be 0x1000
                LPCSTR szName;     // pointer to name (in user addr space)
                DWORD  dwThreadID; // thread ID (-1=caller thread)
                DWORD  dwFlags;    // reserved for future use, must be zero
            };
            string          useThreadName = String (fThreadName_).AsNarrowSDKString ();
            THREADNAME_INFO info;
            {
                info.dwType     = 0x1000;
                info.szName     = useThreadName.c_str ();
                info.dwThreadID = MyGetThreadId_ (GetNativeHandle ());
                info.dwFlags    = 0;
            }
            IgnoreExceptionsForCall (::RaiseException (0x406D1388, 0, sizeof (info) / sizeof (DWORD), (ULONG_PTR*)&info));
        }
#elif qPlatform_POSIX && (__GLIBC__ > 2 or (__GLIBC__ == 2 and __GLIBC_MINOR__ >= 12))
        // could have called prctl(PR_SET_NAME,"<null> terminated string",0,0,0) - but seems less portable
        //
        // according to http://man7.org/linux/man-pages/man3/pthread_setname_np.3.html - the length max is 15 characters
        constexpr size_t kMaxNameLen_{16 - 1}; // 16 chars including nul byte
        string           narrowThreadName = String (fThreadName_).AsNarrowSDKString ();
        if (narrowThreadName.length () > kMaxNameLen_) {
            narrowThreadName.erase (kMaxNameLen_);
        }
        ::pthread_setname_np (GetNativeHandle (), narrowThreadName.c_str ());
#endif
#endif
    }
}

void Thread::Rep_::ApplyPriority (Priority priority)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"Thread::Rep_::ApplyPriority", L"threads=%s, priority=%s", Characters::ToString (*this).c_str (), Characters::ToString (priority).c_str ())};
#endif
    NativeHandleType nh = GetNativeHandle ();
    if (nh != NativeHandleType{}) {
#if qPlatform_Windows
        switch (priority) {
            case Priority::eLowest:
                Verify (::SetThreadPriority (nh, THREAD_PRIORITY_LOWEST));
                break;
            case Priority::eBelowNormal:
                Verify (::SetThreadPriority (nh, THREAD_PRIORITY_BELOW_NORMAL));
                break;
            case Priority::eNormal:
                Verify (::SetThreadPriority (nh, THREAD_PRIORITY_NORMAL));
                break;
            case Priority::eAboveNormal:
                Verify (::SetThreadPriority (nh, THREAD_PRIORITY_ABOVE_NORMAL));
                break;
            case Priority::eHighest:
                Verify (::SetThreadPriority (nh, THREAD_PRIORITY_HIGHEST));
                break;
            default:
                RequireNotReached ();
        }
#elif qPlatform_POSIX
        /*
         *  pthreads - use http://man7.org/linux/man-pages/man3/pthread_getschedparam.3.html
         *
         *  Linux notes:
         *      From http://man7.org/linux/man-pages/man7/sched.7.html
         *
         *          Since Linux 2.6.23, the default scheduler is CFS, the "Completely
         *          Fair Scheduler".  The CFS scheduler replaced the earlier "O(1)"
         *          scheduler.
         *
         *          ...
         *
         *          For threads scheduled under one of the normal scheduling policies
         *         (SCHED_OTHER, SCHED_IDLE, SCHED_BATCH), sched_priority is not used in
         *         scheduling decisions (it must be specified as 0).
         *
         *         So - bottom line - this is a complete waste of time.
         */
        int priorityMin;
        int priorityMax;
        int schedulingPolicy{}; // on Linux, this appears to always be 0 - SCHED_OTHER, so cannot set priorities
        {
            sched_param param{};
            Verify (::pthread_getschedparam (nh, &schedulingPolicy, &param) == 0);
            priorityMin = ::sched_get_priority_min (schedulingPolicy);
            priorityMax = ::sched_get_priority_max (schedulingPolicy);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("schedulingPolicy=%d, default-priority=%d, sPriorityMin_=%d, priorityMax=%d", schedulingPolicy, param.sched_priority, priorityMin, priorityMax);
#endif
        }
        int newPThreadPriority{priorityMin};
        switch (priority) {
            case Priority::eLowest:
                newPThreadPriority = priorityMin;
                break;
            case Priority::eBelowNormal:
                newPThreadPriority = (priorityMax - priorityMin) * .25 + priorityMin;
                break;
            case Priority::eNormal:
                newPThreadPriority = (priorityMax - priorityMin) * .5 + priorityMin;
                break;
            case Priority::eAboveNormal:
                newPThreadPriority = (priorityMax - priorityMin) * .75 + priorityMin;
                break;
            case Priority::eHighest:
                newPThreadPriority = priorityMax;
                break;
            default:
                RequireNotReached ();
                newPThreadPriority = (priorityMax - priorityMin) * .5 + priorityMin;
        }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"Setting os thread priority for thread %lld to %d", (long long int)(nh), newPThreadPriority);
#endif
        /*
         *  \note Slightly simpler to use POSIX pthread_setschedprio - http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_setschedprio.html
         *        but alas MacOSX (XCode 10) doesn't support this, so keep more common code, and use about the same process - pthread_setschedparam
         */
        sched_param sp{};
        sp.sched_priority = newPThreadPriority;
        Verify (::pthread_setschedparam (nh, schedulingPolicy, &sp) == 0 or errno == EPERM);
#else
        // Cannot find any way todo this
        WeakAssertNotImplemented ();
#endif
    }
}

void Thread::Rep_::ThreadMain_ (const shared_ptr<Rep_>* thisThreadRep) noexcept
{
    RequireNotNull (thisThreadRep);
    TraceContextBumper ctx ("Thread::Rep_::ThreadMain_");
    Require (not sKnownBadBeforeMainOrAfterMain_);

#if qDebug
    [[maybe_unused]] auto&& cleanupCheckMain = Finally ([] () noexcept { Require (not sKnownBadBeforeMainOrAfterMain_); });
#endif

    try {
        shared_ptr<Rep_> incRefCnt = *thisThreadRep; // assure refcount incremented so object not deleted while the thread is running

        /*
         *  \note   SUBTLE!!!!
         *
         *      We cannot refernece anything pointer to by incRefCnt (aka Rep_*), because the fThread has not necesarily been
         *      assigned to until after incRefCnt->fRefCountBumpedEvent_, and in fact since its in another thread
         *      we have no idea how long to wait. So don't!
         *
         *  The only time we can be SURE its safe is after 'fOK2StartEvent_'
         */
        IDType thisThreadID = GetCurrentThreadID (); // CANNOT USE incRefCnt->GetID (); because of above!

#if qStroika_Foundation_Exection_Thread_SupportThreadStatistics
        {
            Require (not sKnownBadBeforeMainOrAfterMain_);
            [[maybe_unused]] auto&& critSec = lock_guard{sThreadSupportStatsMutex_};
            DbgTrace (L"Adding thread id %s to sRunningThreads_ (%s)", Characters::ToString (thisThreadID).c_str (), Characters::ToString (sRunningThreads_).c_str ());
            Verify (sRunningThreads_.insert (thisThreadID).second); // .second true if inserted, so checking not already there
        }
        [[maybe_unused]] auto&& cleanup = Finally (
            [thisThreadID] () noexcept {
                Thread::SuppressInterruptionInContext suppressThreadInterrupts; // may not be needed, but safer/harmless
                Require (not sKnownBadBeforeMainOrAfterMain_);                  // Note: A crash in this code is FREQUENTLY the result of an attempt to destroy a thread after existing main () has started
                [[maybe_unused]] auto&& critSec = lock_guard{sThreadSupportStatsMutex_};
                DbgTrace (L"removing thread id %s from sRunningThreads_ (%s)", Characters::ToString (thisThreadID).c_str (), Characters::ToString (sRunningThreads_).c_str ());
                Verify (sRunningThreads_.erase (thisThreadID) == 1); // verify exactly one erased
            });
#endif

        /*
         *  Subtle, and not super clearly documented, but this is taking the address of a thread-local variable, and storing it in a non-thread-local
         *  instance, and hoping all that works correctly (that the memory access all work correctly).
         */
        incRefCnt->fTLSInterruptFlag_ = &t_Interrupting_;
        Stroika_Foundation_Debug_ValgrindDisableCheck_stdatomic (*incRefCnt->fTLSInterruptFlag_);
        Stroika_Foundation_Debug_ValgrindDisableCheck_stdatomic (incRefCnt->fStatus_);

        try {
            // We cannot possibly get interrupted BEFORE this - because only after this fRefCountBumpedEvent_ does the rest of the APP know about our thread ID
            // baring an external process sending us a bogus signal)
            //
            // Note that BOTH the fRefCountBumpedEvent_ and the fOK2StartEvent_ wait MUST come inside the try/catch for
            incRefCnt->fRefCountBumpedEvent_.Set ();

#if qPlatform_POSIX
            {
                // we inherit blocked abort signal given how we are created in DoCreate() - so unblock it -
                // and accept aborts after we've marked reference count as set.
                sigset_t mySet;
                sigemptyset (&mySet);                                         // nb: cannot use :: cuz crapple uses macro --LGP 2016-12-31
                (void)sigaddset (&mySet, GetSignalUsedForThreadInterrupt ()); // ""
                Verify (::pthread_sigmask (SIG_UNBLOCK, &mySet, nullptr) == 0);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"Just set SIG_UNBLOCK for signal %s in this thread", SignalToName (GetSignalUsedForThreadInterrupt ()).c_str ());
#endif
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
             *               Execution::Throw (InterruptException::kThe);
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

            bool doRun = false;
            {
                Status prevValue = Status::eNotYetRunning;
                if (incRefCnt->fStatus_.compare_exchange_strong (prevValue, Status::eRunning)) {
                    doRun = true;
                }
                else {
                    DbgTrace (L"Attempt to run thread - and transition from not yet running to running failed because status was already %s", Characters::ToString (prevValue).c_str ());
                }
            }

            /*
             *  Note - be careful NOT to directly or indirectly refrence fThead - because we may deadlock 
             *  trying to shutdown (join) the thread -
             *  until we get inside the if (doRun)
             *
             *  The reason is - we normally check fThreadDoneAndCanJoin_ before joining, but in the special case of 
             *  abort when eNotYetRunning - we set fStatus_ = Status::eCompleted directly and fThreadDoneAndCanJoin_
             */
            if (doRun) {
                DbgTrace (L"In Thread::Rep_::ThreadMain_ - set state to RUNNING for thread: %s", incRefCnt->ToString ().c_str ());
                Assert (thisThreadID == incRefCnt->GetID ()); // By NOW we know this is OK
                incRefCnt->Run_ ();
                DbgTrace (L"In Thread::Rep_::ThreadProc_ - setting state to COMPLETED for thread: %s", incRefCnt->ToString ().c_str ());
                incRefCnt->fStatus_ = Status::eCompleted;
                incRefCnt->fThreadDoneAndCanJoin_.Set ();
            }
            Assert (incRefCnt->fStatus_ == Status::eCompleted); // someone else must have set it to completed before we got a chance to run - like Abort ()
        }
        catch (const InterruptException&) {
            SuppressInterruptionInContext suppressCtx;
            DbgTrace (L"In Thread::Rep_::ThreadProc_ - setting state to COMPLETED (InterruptException) for thread: %s", incRefCnt->ToString ().c_str ());
            incRefCnt->fStatus_ = Status::eCompleted;
            incRefCnt->fThreadDoneAndCanJoin_.Set ();
        }
        catch (...) {
            SuppressInterruptionInContext suppressCtx;
#if qPlatform_POSIX
            Platform::POSIX::ScopedBlockCurrentThreadSignal blockThreadAbortSignal (GetSignalUsedForThreadInterrupt ());
            t_Interrupting_ = InterruptFlagState_::eNone; //  else .Set() below will THROW EXCPETION and not set done flag!
#endif
            DbgTrace (L"In Thread::Rep_::ThreadProc_ - setting state to COMPLETED (EXCEPT) for thread: %s", incRefCnt->ToString ().c_str ());
            incRefCnt->fStatus_ = Status::eCompleted;
            incRefCnt->fThreadDoneAndCanJoin_.Set ();
        }
    }
    catch (const InterruptException&) {
        DbgTrace ("SERIOUS ERROR in Thread::Rep_::ThreadMain_ () - uncaught InterruptException - see sigsetmask stuff above - somehow still not working");
        //SB ASSERT BUT DISABLE SO I CAN DEBUG OTHER STUFF FIRST
        // TI THINK ISSUE IS
        AssertNotReached (); // This should never happen - but if it does - better a trace message in a tracelog than 'unexpected' being called (with no way out)
    }
    catch (...) {
        DbgTrace ("SERIOUS ERROR in Thread::Rep_::ThreadMain_ () - uncaught exception");

        //SB ASSERT BUT DISABLE SO I CAN DEBUG OTHER STUFF FIRST
        // TI THINK ISSUE IS
        AssertNotReached (); // This should never happen - but if it does - better a trace message in a tracelog than 'unexpected' being called (with no way out)
    }
}

void Thread::Rep_::NotifyOfInterruptionFromAnyThread_ (bool aborting)
{
    Require (fStatus_ == Status::eAborting or fStatus_ == Status::eCompleted or (not aborting)); // if aborting, must be in state aborting or completed, but for interruption can be other state
    //TraceContextBumper ctx ("Thread::Rep_::NotifyOfAbortFromAnyThread_");

    AssertNotNull (fTLSInterruptFlag_);
    if (aborting) {
        fTLSInterruptFlag_->store (InterruptFlagState_::eAborted);
    }
    else {
        /*
         *  Only upgrade
         *
         *  If was none, upgrade to interrupted. If was interrupted, already done. If was aborted, don't actually want to change.
         */
        InterruptFlagState_ v = InterruptFlagState_::eNone;
        if (not fTLSInterruptFlag_->compare_exchange_strong (v, InterruptFlagState_::eInterrupted)) {
            Assert (v == InterruptFlagState_::eInterrupted or v == InterruptFlagState_::eAborted);
        }
        // Weak assert because this COULD fail - you could stop just before this check and the handling thread could handle the interruption and
        // clear the flag. I've seen this once (2019-06-13)
        WeakAssert (fTLSInterruptFlag_->load () == InterruptFlagState_::eInterrupted or fTLSInterruptFlag_->load () == InterruptFlagState_::eAborted);
    }

    if (GetCurrentThreadID () == GetID ()) {
        Assert (fTLSInterruptFlag_ == &t_Interrupting_);
        // NOTE - using CheckForThreadInterruption uses TLS t_Interrupting_ instead of fStatus
        //      --LGP 2015-02-26
        CheckForThreadInterruption (); // unless suppressed, this will throw
    }
    // Note we fall through here either if we have throws suppressed, or if sending to another thread

    // @todo note - this used to check fStatus flag and I just changed to checking *fTLSInterruptFlag_ -- LGP 2015-02-26
    if (fStatus_ == Status::eAborting) {
        Assert (*fTLSInterruptFlag_ == InterruptFlagState_::eAborted);
    }
    if (*fTLSInterruptFlag_ != InterruptFlagState_::eNone) {
#if qPlatform_POSIX
        {
            auto&& critSec = lock_guard{sHandlerInstalled_};
            if (not sHandlerInstalled_) {
                SignalHandlerRegistry::Get ().AddSignalHandler (GetSignalUsedForThreadInterrupt (), kCallInRepThreadAbortProcSignalHandler_);
                sHandlerInstalled_ = true;
            }
        }

        {
            /*
             * siginterrupt gaurantees for the given signal - the SA_RESTART flag is not set, so that any pending system calls
             * will return EINTR - which is crucial to our strategy to interrupt them!
             *
             *  @todo PROBABLY NOT NEEDED ANYMORE (due to use of sigaction) -- LGP 2015-08-29
             */
            Verify (::siginterrupt (GetSignalUsedForThreadInterrupt (), true) == 0);
        }

        (void)Execution::SendSignal (GetNativeHandle (), GetSignalUsedForThreadInterrupt ());
#elif qPlatform_Windows
        Verify (::QueueUserAPC (&CalledInRepThreadAbortProc_, GetNativeHandle (), reinterpret_cast<ULONG_PTR> (this)));
#endif
    }
}

Thread::IDType Thread::Rep_::GetID () const
{
    [[maybe_unused]] auto&& critSec = lock_guard{fAccessSTDThreadMutex_};
    return fThread_.get_id ();
}

Thread::NativeHandleType Thread::Rep_::GetNativeHandle ()
{
    [[maybe_unused]] auto&& critSec = lock_guard{fAccessSTDThreadMutex_};
    return fThread_.native_handle ();
}

#if qPlatform_POSIX
void Thread::Rep_::InterruptionSignalHandler_ (SignalID signal) noexcept
{
    //
    //#if USE_NOISY_TRACE_IN_THIS_MODULE_
    // unsafe to call trace code - because called as unsafe (SignalHandler::Type::eDirect) handler
    //TraceContextBumper ctx ("Thread::Rep_::InterruptionSignalHandler_");
    //#endif
    // This doesn't REALLY need to get called. Its enough to have the side-effect of the EINTR from system calls.
    // the TLS variable gets set through the rep poitner in NotifyOfInterruptionFromAnyThread_
    //
    // Note - using SIG_IGN doesn't work, because then the signal doesn't get delivered, and the EINTR doesn't happen
    //
}
#elif qPlatform_Windows
void CALLBACK Thread::Rep_::CalledInRepThreadAbortProc_ (ULONG_PTR lpParameter)
{
    TraceContextBumper ctx{"Thread::Rep_::CalledInRepThreadAbortProc_"};
    [[maybe_unused]] Thread::Rep_* rep = reinterpret_cast<Thread::Rep_*> (lpParameter);
    Require (GetCurrentThreadID () == rep->GetID ());
    if (rep->fThrowInterruptExceptionInsideUserAPC_)
        [[UNLIKELY_ATTR]]
        {
            CheckForThreadInterruption ();
        }
}
#endif

/*
 ********************************************************************************
 ******************************** Thread::Ptr ***********************************
 ********************************************************************************
 */
#if qPlatform_POSIX
SignalID Thread::sSignalUsedForThreadInterrupt_ = SIGUSR2;
#endif
namespace {
    Synchronized<Thread::Configuration> sDefaultConfiguration_;
}

namespace {
    Thread::Configuration CombineCFGs_ (const optional<Thread::Configuration>& cfg)
    {
        Thread::Configuration result = Thread::GetDefaultConfiguration ();
        if (cfg) {
            if (cfg->fStackSize) {
                result.fStackSize = *cfg->fStackSize;
            }
            if (cfg->fStackGuard) {
                result.fStackSize = *cfg->fStackGuard;
            }
#if qPlatform_Windows
            if (cfg->fThrowInterruptExceptionInsideUserAPC) {
                result.fThrowInterruptExceptionInsideUserAPC = *cfg->fThrowInterruptExceptionInsideUserAPC;
            }
#endif
        }
        return result;
    }
}

void Thread::Ptr::SetThreadPriority (Priority priority) const
{
    RequireNotNull (fRep_);
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this}; // smart ptr - its the ptr thats const, not the rep
    NativeHandleType                                    nh = GetNativeHandle ();
    /**
     *  @todo - not important - but this is a race (bug). If two Thread::Ptrs refer to same thread, and one calls start, and the other calls
     *          SetThreadPriority () - the priority change could get dropped on the floor.
     */
    if (nh == NativeHandleType{}) {
        // This can happen if you set the thread priority before starting the thread (actually probably a common sequence of events)
        fRep_->fInitialPriority_.store (priority);
        return;
    }
    else {
        fRep_->ApplyPriority (priority);
    }
}

String Thread::Ptr::GetThreadName () const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    if (fRep_ == nullptr) {
        return String{};
    }
    else {
        return fRep_->fThreadName_;
    }
}

void Thread::Ptr::SetThreadName (const String& threadName) const
{
    RequireNotNull (fRep_);
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this}; // smart ptr - its the ptr thats const, not the rep
    TraceContextBumper                                  ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"Execution::Thread::SetThreadName", L"thisThreadID=%s, threadName = '%s'", Characters::ToString (GetID ()).c_str (), threadName.c_str ())};
    if (fRep_->fThreadName_ != threadName) {
        fRep_->fThreadName_ = threadName.As<wstring> ();
        fRep_->ApplyThreadName2OSThreadObject ();
    }
}

Characters::String Thread::Ptr::ToString () const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    if (fRep_ == nullptr) {
        return L"nullptr";
    }
    else {
        return fRep_->ToString ();
    }
}

void Thread::Ptr::Start () const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this}; // smart ptr - its the ptr thats const, not the rep
    Debug::TraceContextBumper                           ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"Thread::Start", L"*this=%s", ToString ().c_str ())};
    RequireNotNull (fRep_);
    Require (GetStatus () == Status::eNotYetRunning);
    Rep_::DoCreate (&fRep_);
    if (optional<Priority> p = fRep_->fInitialPriority_.load ()) {
        fRep_->ApplyPriority (*p);
    }
    fRep_->fOK2StartEvent_.Set ();
    DbgTrace (L"%s started", ToString ().c_str ());
}

void Thread::Ptr::Abort () const
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"Thread::Abort", L"*this=%s", ToString ().c_str ())};
    Require (*this != nullptr);
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this}; // smart ptr - its the ptr thats const, not the rep

    // note status not protected by critsection, but SB OK for this

    // first try to send abort exception, and then - if force - get serious!
    // goto aborting, unless the previous value was completed, and then leave it completed.

    {
        // set to aborting, unless completed
        Status prevState = Status::eRunning;
        while (not fRep_->fStatus_.compare_exchange_strong (prevState, Status::eAborting)) {
            if (prevState == Status::eAborting or prevState == Status::eCompleted) {
                // Status::eAborting cannot happen first time through loop, but can on subsequent passes
                break; // leave state alone
            }
            else if (prevState == Status::eNotYetRunning) {
                if (fRep_->fStatus_.compare_exchange_strong (prevState, Status::eCompleted)) {
                    DbgTrace (L"thread never started, so marked as completed");
                    fRep_->fThreadDoneAndCanJoin_.Set ();
                    break; // leave state alone
                }
                else {
                    DbgTrace (L"very rare, but can happen, transitioned to aborting or completed by some other thread (cur state = %s)", Characters::ToString (prevState).c_str ());
                    prevState = Status::eRunning;
                    continue; // try again to transition to aborting
                }
            }
            else if (prevState == Status::eRunning) {
                continue; // try again - this should transition to aborting
            }
            else {
                // other cases - null - shouldn't get this far
                AssertNotReached ();
            }
        }
    }
    if (fRep_->fStatus_ == Status::eAborting) {
        // by default - tries to trigger a throw-abort-excption in the right thread using UNIX signals or QueueUserAPC ()
        fRep_->NotifyOfInterruptionFromAnyThread_ (true);
    }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"leaving thread-state = %s", Characters::ToString (fRep_->fStatus_.load ()).c_str ());
#endif
}

void Thread::Ptr::Interrupt () const
{
    Debug::TraceContextBumper ctx ("Thread::Interrupt");
    Require (*this != nullptr);
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};

    // not status not protected by critsection, but SB OK for this
    DbgTrace (L"*this=%s", ToString ().c_str ());

    Status cs = fRep_->fStatus_.load ();
    if (cs != Status::eAborting and cs != Status::eCompleted) {
        fRep_->NotifyOfInterruptionFromAnyThread_ (false);
    }
}
namespace {
    constexpr Time::DurationSecondsType kAbortAndWaitForDoneUntil_TimeBetweenAborts_ = 1.0;
}

void Thread::Ptr::AbortAndWaitForDoneUntil (Time::DurationSecondsType timeoutAt) const
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"Thread::AbortAndWaitForDoneUntil", L"*this=%s, timeoutAt=%e", ToString ().c_str (), timeoutAt)};
    RequireNotNull (*this);
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    // an abort may need to be resent (since there could be a race and we may need to force wakeup again)
    unsigned int tries = 0;
    while (true) {
        Abort ();
        tries++;
        Time::DurationSecondsType timeLeft = timeoutAt - Time::GetTickCount ();
        if (timeLeft <= kAbortAndWaitForDoneUntil_TimeBetweenAborts_) {
            WaitForDoneUntil (timeoutAt); // throws if we should throw
            return;
        }
        else {
            // If timeLeft BIG - ignore timeout exception and go through loop again
            if (WaitForDoneUntilQuietly (Time::GetTickCount () + kAbortAndWaitForDoneUntil_TimeBetweenAborts_) == WaitableEvent::kWaitQuietlySetResult) {
                return;
            }
            // timeout just continue to loop
        }
        if (tries <= 1) {
            // this COULD happen due to a lucky race - OR - the code could just be BUSY for a while (not calling CheckForAborted). But even then - it COULD make
            // a blocking call which needs interruption.
            DbgTrace ("This should ALMOST NEVER happen - where we did an abort but it came BEFORE the system call and so needs to be called again to re-interrupt: tries: %d.", tries);
        }
        else {
            DbgTrace ("OK - maybe the target thread is ingoring abort exceptions? try/catch/ignore?");
        }
    }
}

void Thread::Ptr::ThrowIfDoneWithException () const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"Thread::ThrowIfDoneWithException", L"*this=%s", Characters::ToString (*this).c_str ())};
#endif
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    if (fRep_) {
        if (fRep_->fStatus_ == Status::eCompleted and fRep_->fSavedException_) {
            ReThrow (fRep_->fSavedException_, L"Rethrowing exception across threads");
        }
    }
}

void Thread::Ptr::WaitForDoneUntil (Time::DurationSecondsType timeoutAt) const
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"Thread::WaitForDoneUntil", L"*this=%s, timeoutAt=%e", ToString ().c_str (), timeoutAt)};
    if (WaitForDoneUntilQuietly (timeoutAt) == WaitableEvent::kWaitQuietlyTimeoutResult) {
        Throw (TimeOutException::kThe);
    }
}

bool Thread::Ptr::WaitForDoneUntilQuietly (Time::DurationSecondsType timeoutAt) const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"Thread::WaitForDoneUntilQuietly", L"*this=%s, timeoutAt=%e", ToString ().c_str (), timeoutAt)};
#endif
    Require (*this != nullptr);
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    CheckForThreadInterruption (); // always a cancelation point
    if (fRep_->fThreadDoneAndCanJoin_.WaitUntilQuietly (timeoutAt) == WaitableEvent::kWaitQuietlySetResult) {
        /*
         *  This is not critical, but has the effect of assuring the COUNT of existing threads is what the caller would expect.
         *  This really only has effect #if     qStroika_Foundation_Exection_Thread_SupportThreadStatistics
         *  because thats the only time we have an imporant side effect of the threads finalizing.
         *
         *  @see https://stroika.atlassian.net/browse/STK-496
         *
         *  NOTE: because we call this join () inside fAccessSTDThreadMutex_, its critical the running thread has terminated to the point where it will no
         *  longer access fThread_ (and therfore not lock fAccessSTDThreadMutex_)
         */
        lock_guard<mutex> critSec2{fRep_->fAccessSTDThreadMutex_};
        if (fRep_->fThread_.joinable ()) {
            // fThread_.join () will block indefinitely - but since we waited on fRep_->fThreadDoneAndCanJoin_ - it shouldn't really take long
            fRep_->fThread_.join ();
        }
        return WaitableEvent::kWaitQuietlySetResult;
    }
    return WaitableEvent::kWaitQuietlyTimeoutResult;
}

#if qPlatform_Windows
void Thread::Ptr::WaitForDoneWhilePumpingMessages (Time::DurationSecondsType timeout) const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    Require (*this != nullptr);
    CheckForThreadInterruption ();
    HANDLE thread = fRep_->GetNativeHandle ();
    if (thread == INVALID_HANDLE_VALUE) {
        return;
    }
    DurationSecondsType timeoutAt = Time::GetTickCount () + timeout;
    // CRUDDY impl - but decent enuf for first draft
    while (GetStatus () != Thread::Status::eCompleted) {
        DurationSecondsType time2Wait = timeoutAt - Time::GetTickCount ();
        if (time2Wait <= 0) {
            Throw (TimeOutException::kThe);
        }
        Platform::Windows::WaitAndPumpMessages (nullptr, {thread}, time2Wait);
    }
    WaitForDone (); // just to get the qStroika_Foundation_Exection_Thread_SupportThreadStatistics / join ()
}
#endif

Thread::Status Thread::Ptr::GetStatus_ () const noexcept
{
    Require (*this != nullptr);
    Assert (fRep_ != nullptr);
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    return fRep_->fStatus_;
}

bool Thread::Ptr::IsDone () const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    switch (GetStatus ()) {
        case Status::eNull:
            return true;
        case Status::eCompleted:
            return true;
    }
    return false;
}

/*
 ********************************************************************************
 ************************ Thread::CleanupPtr ************************************
 ********************************************************************************
 */
Thread::CleanupPtr::~CleanupPtr ()
{
    if (*this != nullptr) {
        SuppressInterruptionInContext suppressInterruption;
        if (fAbort_) {
            AbortAndWaitForDone ();
        }
        else {
            WaitForDone ();
        }
    }
}

/*
 ********************************************************************************
 *********************************** Thread *************************************
 ********************************************************************************
 */
Thread::Ptr Thread::New (const function<void ()>& fun2CallOnce, const optional<Characters::String>& name, const optional<Configuration>& configuration)
{
    Thread::Ptr ptr = Ptr{make_shared<Rep_> (fun2CallOnce, CombineCFGs_ (configuration))};
    if (name) {
        ptr.SetThreadName (*name);
    }
    return ptr;
}

Thread::Configuration Thread::GetDefaultConfiguration ()
{
    return sDefaultConfiguration_.load ();
}

void Thread::SetDefaultConfiguration (const Configuration& config)
{
    sDefaultConfiguration_.store (config);
}

#if qStroika_Foundation_Exection_Thread_SupportThreadStatistics
Thread::Statistics Thread::GetStatistics ()
{
    [[maybe_unused]] auto&& critSec = lock_guard{sThreadSupportStatsMutex_};
    return Statistics{Containers::Set<Thread::IDType>{sRunningThreads_}};
}
#endif

void Thread::Abort (const Traversal::Iterable<Thread::Ptr>& threads)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"Thread::Abort", L"threads=%s", Characters::ToString (threads).c_str ())};
#endif
    threads.Apply ([] (Thread::Ptr t) { t.Abort (); });
}

void Thread::Interrupt (const Traversal::Iterable<Thread::Ptr>& threads)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"Thread::Interrupt", L"threads=%s", Characters::ToString (threads).c_str ())};
#endif
    threads.Apply ([] (Thread::Ptr t) { t.Interrupt (); });
}

void Thread::AbortAndWaitForDoneUntil (const Traversal::Iterable<Thread::Ptr>& threads, Time::DurationSecondsType timeoutAt)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"Thread::AbortAndWaitForDoneUntil", L"threads=%s, timeoutAt=%f", Characters::ToString (threads).c_str (), timeoutAt)};
#endif
    threads.Apply ([] (const Thread::Ptr& t) { t.Abort (); }); // preflight not needed, but encourages less wait time if each given a short at abort first
#if 1                                                          /*qDefaultTracingOn*/
    {
        constexpr Time::DurationSecondsType kTimeBetweenDbgTraceWarnings_{5.0};
        Time::DurationSecondsType           timeOfLastWarning = Time::GetTickCount ();
        Time::DurationSecondsType           timeOfLastAborts  = timeOfLastWarning;
        Set<Thread::Ptr>                    threads2WaitOn{threads};
        while (not threads2WaitOn.empty ()) {
            for (Traversal::Iterator<Thread::Ptr> i = threads2WaitOn.begin (); i != threads2WaitOn.end (); ++i) {
                constexpr Time::DurationSecondsType kMinWaitThreshold_ = min (kTimeBetweenDbgTraceWarnings_, kAbortAndWaitForDoneUntil_TimeBetweenAborts_);
                Time::DurationSecondsType           to                 = min (Time::GetTickCount () + kMinWaitThreshold_, timeoutAt);
                if (i->WaitForDoneUntilQuietly (to)) {
                    threads2WaitOn.erase (i);
                }
            }
            if (not threads2WaitOn.empty () and timeOfLastWarning + kTimeBetweenDbgTraceWarnings_ < Time::GetTickCount ()) {
                DbgTrace (L"still waiting for %s - re-sending aborts", Characters::ToString (threads2WaitOn).c_str ());
            }
            if (not threads2WaitOn.empty () and timeOfLastAborts + kAbortAndWaitForDoneUntil_TimeBetweenAborts_ < Time::GetTickCount ()) {
                threads2WaitOn.Apply ([] (Thread::Ptr t) { t.Abort (); }); // preflight not needed, but encourages less wait time if each given a short at abort first
            }
        }
    }
#else
    threads.Apply ([timeoutAt] (Thread::Ptr t) { t.AbortAndWaitForDoneUntil (timeoutAt); });
#endif
}

void Thread::WaitForDoneUntil (const Traversal::Iterable<Thread::Ptr>& threads, Time::DurationSecondsType timeoutAt)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"Thread::WaitForDoneUntil", L"threads=%s, timeoutAt=%f", Characters::ToString (threads).c_str (), timeoutAt)};
#endif
    CheckForThreadInterruption (); // always a cancelation point (even if empty list)
    threads.Apply ([timeoutAt] (const Thread::Ptr& t) { t.WaitForDoneUntil (timeoutAt); });
}

#if qPlatform_POSIX
void Thread::SetSignalUsedForThreadInterrupt (SignalID signalNumber)
{
    auto&& critSec = lock_guard{sHandlerInstalled_};
    if (sHandlerInstalled_) {
        SignalHandlerRegistry::Get ().RemoveSignalHandler (GetSignalUsedForThreadInterrupt (), kCallInRepThreadAbortProcSignalHandler_);
        sHandlerInstalled_ = false;
    }
    sSignalUsedForThreadInterrupt_ = signalNumber;
    // install new handler
    if (not sHandlerInstalled_) {
        SignalHandlerRegistry::Get ().AddSignalHandler (GetSignalUsedForThreadInterrupt (), kCallInRepThreadAbortProcSignalHandler_);
        sHandlerInstalled_ = true;
    }
}
#endif

/*
 ********************************************************************************
 ************************** Execution::FormatThreadID_A *************************
 ********************************************************************************
 */
string Execution::FormatThreadID_A (Thread::IDType threadID)
{
    Thread::SuppressInterruptionInContext suppressAborts;

    /*
     *  stdc++ doesn't define a way to get the INT thread id, just a string. But they don't format it the
     *  way we usually format a thread ID (hex, fixed width). So do that, so thread IDs look more consistent.
     */
    stringstream out;
    out << threadID;

#if qPlatform_Windows
    constexpr size_t kSizeOfThreadID_ = sizeof (DWORD); // All MSFT SDK Thread APIs use DWORD for thread id
#elif qPlatform_POSIX
    constexpr size_t kSizeOfThreadID_ = sizeof (pthread_t);
#else
    // on MSFT this object is much larger than thread id because it includes handle and id
    // Not a reliable measure anywhere, but probably our best guess
    constexpr size_t kSizeOfThreadID_ = sizeof (Thread::IDType);
#endif

    if constexpr (kSizeOfThreadID_ >= sizeof (uint64_t)) {
        uint64_t threadIDInt = 0;
        out >> threadIDInt;
        return Characters::CString::Format ("0x%016llx", threadIDInt);
    }
    else {
        uint32_t threadIDInt = 0;
        out >> threadIDInt;
        /*
         *  Often, it appears ThreadIDs IDs are < 16bits, so making the printout format shorter makes it a bit more readable.
         *
         *  However, I don't see any reliable way to tell this is the case, so don't bother for now. A trouble with checking on
         *  a per-thread-id basis is that often the MAIN THREAD is 0, which is < 0xffff. Then we get one size and then
         *  on the rest a differnt size, so the layout in the debug trace log looks funny.
         */
        constexpr bool kUse16BitThreadIDsIfTheyFit_{false};
        const bool     kUse16Bit_ = kUse16BitThreadIDsIfTheyFit_ and threadIDInt <= 0xffff;
        if (kUse16Bit_) {
            return Characters::CString::Format ("0x%04x", threadIDInt);
        }
        else {
            return Characters::CString::Format ("0x%08x", threadIDInt);
        }
    }
}

/*
 ********************************************************************************
 ********************* Execution::CheckForThreadInterruption ********************
 ********************************************************************************
 */
void Execution::CheckForThreadInterruption ()
{
    /*
     *  NOTE: subtle but important that we use static Thread::InterruptException::kThe so we avoid
     *  re-throw with string operations. Otheriwse we would have to use SuppressInterruptionInContext
     *  just before the actual throw.
     */
    if (t_InterruptionSuppressDepth_ == 0) {
        Thread::SuppressInterruptionInContext suppressSoStringsDontThrow;
        switch (t_Interrupting_.load ()) {
            case InterruptFlagState_::eInterrupted: {
                // When we interrupt a thread, that state is not sticky - it happens just once, until someone calls Interrupt () again
                // but be careful not to undo an abort that comes after interrupt
                InterruptFlagState_ prev = InterruptFlagState_::eInterrupted;
                if (not t_Interrupting_.compare_exchange_strong (prev, InterruptFlagState_::eNone)) {
                    DbgTrace (L"Failed to reset interrupted thread state (state was %d)", prev); // most likely no problem - just someone did abort right after interrupt
                }
                Throw (Thread::InterruptException::kThe);
            } break;
            case InterruptFlagState_::eAborted:
                Throw (Thread::AbortException::kThe);
        }
    }
#if qDefaultTracingOn
    else if (t_Interrupting_ != InterruptFlagState_::eNone) {
        static atomic<unsigned int> sSuperSuppress_{};
        if (++sSuperSuppress_ <= 1) {
            IgnoreExceptionsForCall (DbgTrace ("Suppressed interupt throw: t_InterruptionSuppressDepth_=%d, t_Interrupting_=%d", t_InterruptionSuppressDepth_, t_Interrupting_.load ()));
            sSuperSuppress_--;
        }
    }
#endif
}

/*
 ********************************************************************************
 ********************************* Execution::Yield *****************************
 ********************************************************************************
 */
void Execution::Yield ()
{
    /*
     *  Check before so we abort more quickly, and after since while we were sleeping we could be interupted.
     *  And this (yeild) happens at a non-time critical point, so though checking before and after is redudnant,
     *  not importantly
     */
    CheckForThreadInterruption ();
    this_thread::yield ();
    CheckForThreadInterruption ();
}
