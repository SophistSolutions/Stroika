/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/StroikaConfig.h"

#include <list>
#include <sstream>
#if qPlatform_Windows
#include <process.h>
#include <windows.h>
#endif

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Debug/Main.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Time/Realtime.h"

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
using Debug::AssertExternallySynchronizedMutex;

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
    thread_local unsigned int t_InterruptionSuppressDepth_{0};
}

#if qStroika_Foundation_Execution_Thread_SupportThreadStatistics
namespace {
    // use mutex and set<> to avoid interdependencies between low level Stroika facilities
    mutex               sThreadSupportStatsMutex_;
    set<Thread::IDType> sRunningThreads_; // protected by sThreadSupportStatsMutex_

    struct AllThreadsDeadDetector_ {
        AllThreadsDeadDetector_ ()
        {
            Require (sRunningThreads_.empty ());
        }
        ~AllThreadsDeadDetector_ ()
        {
            if constexpr (qDebug) {
                if (not sRunningThreads_.empty ()) {
                    DbgTrace ("Threads {} running"_f, Thread::GetStatistics ().fRunningThreads);
                    Require (sRunningThreads_.empty ());
                }
            }
        }
    };
    AllThreadsDeadDetector_ sAllThreadsDeadDetector_;
}
#endif

#if qPlatform_Windows
namespace {
#if (_WIN32_WINNT < 0x0502)
    namespace XXX_ {
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
        using namespace XXX_;
        static DLLLoader                   ntdll (SDKSTR ("ntdll.dll"));
        static pfnNtQueryInformationThread NtQueryInformationThread =
            (pfnNtQueryInformationThread)ntdll.GetProcAddress ("NtQueryInformationThread");
        if (NtQueryInformationThread == nullptr)
            return 0; // failed to get proc address
        THREAD_BASIC_INFORMATION tbi{};
        THREAD_INFORMATION_CLASS tic = ThreadBasicInformation;
        if (::NtQueryInformationThread (thread, tic, &tbi, sizeof (tbi), nullptr) != STATUS_SUCCESS) {
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

/*
 ********************************************************************************
 ************** Thread::SuppressInterruptionInContext ***************************
 ********************************************************************************
 */
Thread::SuppressInterruptionInContext::SuppressInterruptionInContext ()
{
    ++t_InterruptionSuppressDepth_;
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
 ************************** Thread::AbortException ******************************
 ********************************************************************************
 */
Thread::AbortException::AbortException ()
    : Exception<>{"Thread Abort"sv}
{
}

/*
 ********************************************************************************
 ************************** Thread::IndexRegistrar ******************************
 ********************************************************************************
 */
Thread::IndexRegistrar::IndexRegistrar ()
{
    Assert (not fInitialized_);
    fInitialized_ = true;
}

Thread::IndexRegistrar::~IndexRegistrar ()
{
    Assert (fInitialized_);
    fInitialized_ = false;
}

unsigned int Thread::IndexRegistrar::GetIndex (const IDType& threadID, bool* wasNew)
{
    if (not fInitialized_) {
        if (wasNew != nullptr) {
            *wasNew = false;
        }
        return 0;
    }
    [[maybe_unused]] lock_guard critSec{fMutex_};
    auto                        i                = fShownThreadIDs_.find (threadID);
    unsigned int                threadIndex2Show = 0;
    if (i == fShownThreadIDs_.end ()) {
        threadIndex2Show = static_cast<unsigned int> (fShownThreadIDs_.size ());
        fShownThreadIDs_.insert ({threadID, threadIndex2Show});
    }
    else {
        threadIndex2Show = i->second;
    }
    if (wasNew != nullptr) {
        *wasNew = i == fShownThreadIDs_.end ();
    }
    return threadIndex2Show;
}

/*
 ********************************************************************************
 ***************************** Thread::Ptr::Rep_ ********************************
 ********************************************************************************
 */
Thread::Ptr::Rep_::Rep_ (const function<void ()>& runnable, [[maybe_unused]] const optional<Configuration>& configuration)
    : fRunnable_{runnable}
{
    // @todo - never used anything from configuration (yet) - should!)
#if qPlatform_POSIX
    static bool sDidInit_{false}; // initialize after main() started, but before any threads
    if (not sDidInit_) {
        sDidInit_                               = true;
        kCallInRepThreadAbortProcSignalHandler_ = SignalHandler{Rep_::InterruptionSignalHandler_, SignalHandler::Type::eDirect};
    }
#elif qPlatform_Windows
    if (configuration.has_value () and configuration->fThrowInterruptExceptionInsideUserAPC.has_value ()) {
        fThrowInterruptExceptionInsideUserAPC_ = configuration->fThrowInterruptExceptionInsideUserAPC.value ();
    }
#endif
}

Thread::Ptr::Rep_::~Rep_ ()
{
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
    if (fThreadValid_ and fThread_.joinable ()) {
        fThread_.detach ();
    }
}

void Thread::Ptr::Rep_::Run_ ()
{
    try {
        fRunnable_ ();
    }
    catch (const AbortException&) {
        // Note: intentionally not saved in fSavedException_.
        // See ThrowIfDoneWithException
        throw;
    }
    catch (...) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("in ad::Ptr::Rep_::Run_ () - saving caught exception to repropagate later ({})"_f, current_exception ());
#endif
        fSavedException_ = current_exception ();
        throw;
    }
}

Characters::String Thread::Ptr::Rep_::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    if (fRefCountBumpedInsideThreadMainEvent_.PeekIsSet ()) {
        // If fRefCountBumpedInsideThreadMainEvent_ not yet SET, then this info is bogus
        sb << "id: "sv << GetID () << ", "sv;
        if constexpr (qStroika_Foundation_Debug_Trace_ShowThreadIndex) {
            sb << "index: " << IndexRegistrar::sThe.GetIndex (GetID ()) << ", "sv;
        }
    }
    if (not fThreadName_.empty ()) {
        sb << "name: "sv << fThreadName_ << ", "sv;
    }
    sb << "status: "sv << PeekStatusForToString_ () << ", "sv;
    //sb << "runnable: "sv << fRunnable_ << ", "sv;     // doesn't yet print anything useful
    sb << "abortRequested: "sv << fAbortRequested_.load () << ", "sv;
    sb << "refCountBumpedEvent: "sv << fRefCountBumpedInsideThreadMainEvent_.PeekIsSet () << ", "sv;
    sb << "startReadyToTransitionToRunningEvent_: "sv << fStartReadyToTransitionToRunningEvent_.PeekIsSet () << ", "sv;
    sb << "threadDoneAndCanJoin: "sv << fThreadDoneAndCanJoin_.PeekIsSet () << ", "sv;
    if (fSavedException_.load () != nullptr) [[unlikely]] {
        sb << "savedException: "sv << fSavedException_.load () << ", "sv;
    }
    if (fInitialPriority_.load () != nullopt) [[unlikely]] {
        sb << "initialPriority: "sv << fInitialPriority_.load () << ", "sv;
    }
#if qPlatform_Windows
    sb << "throwInterruptExceptionInsideUserAPC: "sv << fThrowInterruptExceptionInsideUserAPC_;
#endif
    sb << "}"sv;
    return sb;
}

void Thread::Ptr::Rep_::ApplyThreadName2OSThreadObject ()
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
            string          useThreadName = String{fThreadName_}.AsNarrowSDKString (eIgnoreErrors);
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
        string           narrowThreadName = String{fThreadName_}.AsNarrowSDKString (eIgnoreErrors);
        if (narrowThreadName.length () > kMaxNameLen_) {
            narrowThreadName.erase (kMaxNameLen_);
        }
        ::pthread_setname_np (GetNativeHandle (), narrowThreadName.c_str ());
#endif
#endif
    }
}

void Thread::Ptr::Rep_::ApplyPriority (Priority priority)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (
        "Thread::Ptr::Rep_::ApplyPriority", "threads={}, priority={}"_f, Characters::ToString (*this), Characters::ToString (priority))};
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
            DbgTrace ("schedulingPolicy=%d, default-priority=%d, sPriorityMin_=%d, priorityMax=%d", schedulingPolicy, param.sched_priority,
                      priorityMin, priorityMax);
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

void Thread::Ptr::Rep_::ThreadMain_ (const shared_ptr<Rep_> thisThreadRep) noexcept
{
    RequireNotNull (thisThreadRep); // NOTE - since shared_ptr<> is NOT a const reference, this holds the bumped reference count til the end of ThreadMain_ scope
    TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs ("Execution::Thread::Ptr::Rep_::ThreadMain_", "thisThreadRep={}"_f,
                                                                          Characters::ToString (thisThreadRep))};
#if qDebug
    Require (Debug::AppearsDuringMainLifetime ());
    [[maybe_unused]] auto&& cleanupCheckMain = Finally ([] () noexcept { Require (Debug::AppearsDuringMainLifetime ()); });
#endif

    try {
        {
            SuppressInterruptionInContext suppressInterruptionsOfThisThreadCallerKnowsWeHaveItBumpedAndCanProceed;
            // This thread (ThreadMain) cannot possibly get interrupted BEFORE this - because only after this fRefCountBumpedInsideThreadMainEvent_ does the rest of the APP know about our thread ID
            // baring an external process sending us a bogus signal)
            //
            // Note that BOTH the fRefCountBumpedInsideThreadMainEvent_ and the fStartReadyToTransitionToRunningEvent_ wait MUST come inside the try/catch for
            thisThreadRep->fRefCountBumpedInsideThreadMainEvent_.Set ();
        }

        // So inside 'Run' - we will have access to this thread_local variable
        sCurrentThreadRep_ = thisThreadRep;

        [[maybe_unused]] IDType thisThreadID = GetCurrentThreadID (); // NOTE - CANNOT call thisThreadRep->GetID () or in any way touch thisThreadRep->fThread_

#if qStroika_Foundation_Execution_Thread_SupportThreadStatistics
        {
            Require (Debug::AppearsDuringMainLifetime ());
            [[maybe_unused]] lock_guard critSec{sThreadSupportStatsMutex_};
#if qStroika_Foundation_Debug_Trace_ShowThreadIndex
            DbgTrace (
                "Adding thread index {} to sRunningThreads_ ({})"_f, IndexRegistrar::sThe.GetIndex (thisThreadID),
                Traversal::Iterable<IDType>{sRunningThreads_}.Map<vector<int>> ([] (IDType i) { return IndexRegistrar::sThe.GetIndex (i); }));
#else
            DbgTrace ("Adding thread id {} to sRunningThreads_ ({})"_f, thisThreadID, sRunningThreads_);
#endif
            Verify (sRunningThreads_.insert (thisThreadID).second); // .second true if inserted, so checking not already there
        }
        [[maybe_unused]] auto&& cleanup = Finally ([thisThreadID] () noexcept {
            SuppressInterruptionInContext suppressThreadInterrupts; // may not be needed, but safer/harmless
            Require (Debug::AppearsDuringMainLifetime ()); // Note: A crash in this code is FREQUENTLY the result of an attempt to destroy a thread after existing main () has started
            [[maybe_unused]] lock_guard critSec{sThreadSupportStatsMutex_};
#if qStroika_Foundation_Debug_Trace_ShowThreadIndex
            DbgTrace (
                "removing thread index {} from sRunningThreads_ ({})"_f, IndexRegistrar::sThe.GetIndex (thisThreadID),
                Traversal::Iterable<IDType>{sRunningThreads_}.Map<vector<int>> ([] (IDType i) { return IndexRegistrar::sThe.GetIndex (i); }));
#else
            DbgTrace ("removing thread id {} from sRunningThreads_ ({})"_f, thisThreadID, sRunningThreads_);
#endif
            Verify (sRunningThreads_.erase (thisThreadID) == 1); // verify exactly one erased
        });
#endif

        try {
#if qPlatform_POSIX
            {
                // we inherit blocked abort signal given how we are created in DoCreate() - so unblock it -
                // and accept aborts after we've marked reference count as set.
                sigset_t mySet;
                sigemptyset (&mySet);                                      // nb: cannot use :: cuz crapple uses macro --LGP 2016-12-31
                (void)sigaddset (&mySet, SignalUsedForThreadInterrupt ()); // ""
                Verify (::pthread_sigmask (SIG_UNBLOCK, &mySet, nullptr) == 0);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"Just set SIG_UNBLOCK for signal {} in this thread"_f, SignalToName (SignalUsedForThreadInterrupt ()));
#endif
            }
#endif
            thisThreadRep->fStartReadyToTransitionToRunningEvent_.Wait ();

            [[maybe_unused]] auto&& cleanupThreadDoneEventSetter = Finally ([thisThreadRep] () noexcept {
                // whether aborted before we transition state to running, or after, be sure to set this so we can 'join' the thread (also done in catch handlers)
                thisThreadRep->fThreadDoneAndCanJoin_.Set ();
            });

            Assert (thisThreadID == thisThreadRep->GetID ()); // By now we know thisThreadRep->fThread_ has been assigned so it can be accessed

            bool doRun = not thisThreadRep->fAbortRequested_ and not thisThreadRep->IsDone_ ();

#if __cpp_lib_jthread >= 201911
            // If a caller uses the std stop_token mechanism, assure the thread is marked as stopped/aborted
            // But only register this after fRefCountBumpedInsideThreadMainEvent_ (would need to think more carefully to place this earlier)
            // --LGP 2023-10-03
            stop_callback stopCallback{thisThreadRep->fStopToken_, [=] () {
                                           if (doRun) {
                                               DbgTrace ("Something triggered stop_token request stop, so doing abort to make sure we are in an aborting (flag) state."_f);
                                               // Abort () call is is slightly overkill, since frequently already in the aborting state, so check first
                                               if (not thisThreadRep->fAbortRequested_) [[unlikely]] {
                                                   IgnoreExceptionsForCall (Ptr{thisThreadRep}.Abort ());
                                               }
                                           }
                                       }};
#endif
            if (doRun) {
                DbgTrace (L"In Thread::Rep_::ThreadMain_ - set state to RUNNING for thread: {}"_f, thisThreadRep->ToString ());
                thisThreadRep->Run_ ();
                DbgTrace (L"In Thread::Rep_::ThreadProc_ - setting state to COMPLETED for thread: {}"_f, thisThreadRep->ToString ());
            }
        }
        catch (const AbortException&) {
            SuppressInterruptionInContext suppressCtx;
            DbgTrace ("In Thread::Rep_::ThreadProc_ - setting state to COMPLETED (InterruptException) for thread: {}"_f, thisThreadRep->ToString ());
            thisThreadRep->fThreadDoneAndCanJoin_.Set ();
        }
        catch (...) {
            SuppressInterruptionInContext suppressCtx;
            DbgTrace ("In Thread::Rep_::ThreadProc_ - setting state to COMPLETED (due to EXCEPTION) for thread: {}"_f, thisThreadRep->ToString ());
            thisThreadRep->fThreadDoneAndCanJoin_.Set ();
        }
    }
    catch (const AbortException&) {
        DbgTrace ("SERIOUS ERROR in Thread::Rep_::ThreadMain_ () - uncaught InterruptException - see sigsetmask stuff above - somehow not "
                  "working???"_f);
        AssertNotReached (); // This should never happen - but if it does - better a trace message in a tracelog than 'unexpected' being called (with no way out)
    }
    catch (...) {
        DbgTrace ("SERIOUS ERROR in Thread::Rep_::ThreadMain_ () - uncaught exception"_f);
        AssertNotReached (); // This should never happen - but if it does - better a trace message in a tracelog than 'unexpected' being called (with no way out)
    }
}

void Thread::Ptr::Rep_::NotifyOfInterruptionFromAnyThread_ ()
{
    Require (not IsDone_ ());
    Require (fAbortRequested_);
    //TraceContextBumper ctx{"Thread::Rep_::NotifyOfAbortFromAnyThread_"};

    // typically abort from another thread, but in principle this could happen!
    if (GetCurrentThreadID () == GetID ()) [[unlikely]] {
        CheckForInterruption (); // unless suppressed, this will throw
    }

    // Note we fall through here either if we have throws suppressed, or if sending to another thread

    // if fThreadValid_, and can try to cancel it; else start () process early enuf it will cancel
    if (fThreadValid_) {
        /*
         *  Do some platform-specific magic to terminate ongoing system calls
         * 
         *      On POSIX - this is sending a signal which generates EINTR error.
         *      On Windoze - this is QueueUserAPC to enter an alertable state.
         */
#if qPlatform_POSIX
        {
            [[maybe_unused]] lock_guard critSec{sHandlerInstalled_};
            if (not sHandlerInstalled_) {
                SignalHandlerRegistry::Get ().AddSignalHandler (SignalUsedForThreadInterrupt (), kCallInRepThreadAbortProcSignalHandler_);
                sHandlerInstalled_ = true;
            }
        }
        (void)Execution::SendSignal (GetNativeHandle (), SignalUsedForThreadInterrupt ());
#elif qPlatform_Windows
        Verify (::QueueUserAPC (&CalledInRepThreadAbortProc_, GetNativeHandle (), reinterpret_cast<ULONG_PTR> (this)));
#endif
    }
}

#if qPlatform_POSIX
void Thread::Ptr::Rep_::InterruptionSignalHandler_ (SignalID signal) noexcept
{
    //
    //#if USE_NOISY_TRACE_IN_THIS_MODULE_
    // unsafe to call trace code - because called as unsafe (SignalHandler::Type::eDirect) handler
    //TraceContextBumper ctx{"Thread::Ptr::Rep_::InterruptionSignalHandler_"};
    //#endif
    // This doesn't REALLY need to get called. Its enough to have the side-effect of the EINTR from system calls.
    // the TLS variable gets set through the rep poitner in NotifyOfInterruptionFromAnyThread_
    //
    // Note - using SIG_IGN doesn't work, because then the signal doesn't get delivered, and the EINTR doesn't happen
    //
}
#elif qPlatform_Windows
void CALLBACK Thread::Ptr::Rep_::CalledInRepThreadAbortProc_ (ULONG_PTR lpParameter)
{
    TraceContextBumper          ctx{"Thread::Ptr::Rep_::CalledInRepThreadAbortProc_"};
    [[maybe_unused]] Ptr::Rep_* rep = reinterpret_cast<Ptr::Rep_*> (lpParameter);
    Require (GetCurrentThreadID () == rep->GetID ());
    if (rep->fThrowInterruptExceptionInsideUserAPC_) [[unlikely]] {
        CheckForInterruption ();
    }
}
#endif

/*
 ********************************************************************************
 ******************************** Thread::Ptr ***********************************
 ********************************************************************************
 */
namespace {
    Synchronized<Thread::Configuration> sDefaultConfiguration_;
}

namespace {
    Thread::Configuration CombineCFGs_ (const optional<Thread::Configuration>& cfg)
    {
        Thread::Configuration result = Thread::DefaultConfiguration ();
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
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_}; // smart ptr - its the ptr thats const, not the rep
    NativeHandleType nh = GetNativeHandle ();
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
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
    return fRep_ == nullptr ? String{} : fRep_->fThreadName_;
}

void Thread::Ptr::SetThreadName (const String& threadName) const
{
    RequireNotNull (fRep_);
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_}; // smart ptr - its the ptr thats const, not the rep
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx{"Execution::Thread::SetThreadName", "thisThreadID={}, threadName = '{}'"_f, GetID (), threadName};
#endif
    if (fRep_->fThreadName_ != threadName) {
        fRep_->fThreadName_ = threadName.As<wstring> ();
        fRep_->ApplyThreadName2OSThreadObject ();
    }
}

Characters::String Thread::Ptr::ToString () const
{
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
    return fRep_ == nullptr ? "nullptr"sv : fRep_->ToString ();
}

void Thread::Ptr::Start () const
{
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_}; // smart ptr - its the ptr thats const, not the rep
    Debug::TraceContextBumper ctx{"Thread::Start", "*this={}"_f, ToString ()};
    RequireNotNull (fRep_);
    Require (not fRep_->fStartEverInitiated_);
#if qDebug
    {
        auto s = GetStatus ();                                           // @todo - consider - not sure about this
        Require (s == Status::eNotYetRunning or s == Status::eAborting); // if works - document
    }
#endif

    /*
     *  Stroika thread-start choreography:
     * 
     *          CALLING_THREAD                                       |       STATE        CREATED THREAD
     *          Create 'shared_ptr<Rep>'                            <|  eNotYetRunning
     *                  (notes - anytime after state set to eNotYetRunning can transition to eAborting. Only from eRunning or eAborting can it transition to eCompleted )
     *          ENTER CALL TO Thread::Ptr::Start ()                 <|
     *          START SuppressInterruptionInContext                 <|
     *          Create jthread object, giving it                    <|
     *          a 'ThreadMain'                                      <|
     *          WAIT ON fRefCountBumpedInsideThreadMainEvent_       <|>                   ENTER Rep_::ThreadMain_ with BUMPED shared_ptr<Rep> refcount
     *                                                               |>                   thisThreadRep->fRefCountBumpedInsideThreadMainEvent_.Set ()  (NOTE thisThreadRep == fRefCountBumpedInsideThreadMainEvent_)
     *          END SuppressInterruptionInContext                    |       
     *          Setup a few thread properties, name, priority       <|>                   Setup thread-local properties, inside ThreadMain/new thread 
     *          fRep_->fStartReadyToTransitionToRunningEvent_.Set ()<|>                   thisThreadRep->fStartReadyToTransitionToRunningEvent_.Wait ();   -- NOTE CANNOT ACCESS thisThreadRep->fThread_ until this point
     *          return/done                                         <|> eRunning|(etc)    STATE TRANSITION TO RUNNING (MAYBE - COULD HAVE BEEN ALREADY ABORTED)
     */

    {
        /*
         *  Once we have constructed the other thread, its critical it be allowed to run at least to the
         *  point where it's bumped its reference count before we allow aborting this thread.
         */
        SuppressInterruptionInContext suppressInterruptionsOfThisThreadWhileConstructingRepOtherElseLoseSharedPtrEtc;

        fRep_->fStartEverInitiated_ = true; //atomic/publish
        if (fRep_->fAbortRequested_) [[unlikely]] {
            Execution::Throw (Execution::RuntimeErrorException{"Thread aborted during start"}); // check and if aborting now, don't go further
        }

#if __cpp_lib_jthread >= 201911
        fRep_->fStopToken_ = fRep_->fStopSource_.get_token ();
        fRep_->fThread_    = jthread{[this] () -> void { Rep_::ThreadMain_ (fRep_); }};
#else
        fRep_->fThread_ = thread{[this] () -> void { Rep_::ThreadMain_ (fRep_); }};
#endif
        fRep_->fThreadValid_ = true;

        // assure we wait for this, so we don't ever let refcount go to zero before the thread has started.
        fRep_->fRefCountBumpedInsideThreadMainEvent_.Wait ();

        // Once we've gotten here, the ThreadMain is executing, but 'paused' waiting for us to setup more stuff
    }

    // Setup a few thread properties, name, priority
    fRep_->ApplyThreadName2OSThreadObject ();
    if (optional<Priority> p = fRep_->fInitialPriority_.load ()) {
        fRep_->ApplyPriority (*p);
    }
    DbgTrace (L"Requesting transition to running for {}"_f, ToString ());
    fRep_->fStartReadyToTransitionToRunningEvent_.Set ();
}
void Thread::Ptr::Start (WaitUntilStarted) const
{
    Start ();
    for (auto s = GetStatus (); s != Status::eNotYetRunning; s = GetStatus ()) {
        // @todo fix this logic - set expliclt when we do the SET EVENT above (before). But then need to change the threadmain logic to accomodate;
        // low priority since this overload probably not used...
        // --LGP 2023-11-30
        this_thread::yield ();
    }
#if qDebug
    auto s = GetStatus ();
    Ensure (s == Status::eRunning or s == Status::eAborting or s == Status::eCompleted);
#endif
}

void Thread::Ptr::Abort () const
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs ("Thread::Abort", "*this={}"_f, ToString ())};
    Require (*this != nullptr);
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_}; // smart ptr - its the ptr thats const, not the rep

#if __cpp_lib_jthread >= 201911
    bool wasAborted = fRep_->fAbortRequested_;
#endif
    // Abort can be called with status in ANY state, except nullptr (which would mean ever assigned Thread::New());
    fRep_->fAbortRequested_ = true;
    if (fRep_->fStartEverInitiated_) {
#if __cpp_lib_jthread >= 201911
        // If transitioning to aborted state, notify any existing stop_callbacks
        // not needed to check prevState - since https://en.cppreference.com/w/cpp/thread/jthread/request_stop says requst_stop checks if already requested.
        if (not wasAborted) [[likely]] {
            DbgTrace ("Transitioned state to aborting, so calling fThread_.get_stop_source ().request_stop ();"_f);
            fRep_->fStopSource_.request_stop ();
        }
#endif
    }
    else {
        /*
         *  Then mark the thread as completed.
         * 
         *  If we have not yet called start (or gotten to the point where fStartEverInitiated_ gets set), then set tje CanJoin event.
         *  sb safe.
         */
        fRep_->fThreadDoneAndCanJoin_.Set ();
    }
    if (not IsDone ()) [[likely]] {
        // by default - tries to trigger a throw-abort-exception in the right thread using UNIX signals or QueueUserAPC ()
        fRep_->NotifyOfInterruptionFromAnyThread_ ();
    }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"leaving *this = {}"_f, *this);
#endif
}

void Thread::Ptr::AbortAndWaitForDoneUntil (Time::TimePointSeconds timeoutAt) const
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs ("Thread::AbortAndWaitForDoneUntil", "*this={}, timeoutAt={}"_f,
                                                                                 ToString (), Characters::ToString (timeoutAt))};
    RequireNotNull (*this);
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};

    Abort ();
    WaitForDoneUntil (timeoutAt);
}

void Thread::Ptr::ThrowIfDoneWithException () const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"Thread::ThrowIfDoneWithException", "*this={}"_f, *this};
#endif
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
    if (fRep_ and fRep_->IsDone_ () and fRep_->fSavedException_.load () != nullptr) {
        // safe not holding lock cuz code simpler, and cannot transition from savedExcept to none - never cleared
        ReThrow (fRep_->fSavedException_.load (), "Rethrowing exception across threads");
    }
}

void Thread::Ptr::WaitForDoneUntil (Time::TimePointSeconds timeoutAt) const
{
    Debug::TraceContextBumper ctx{"Thread::WaitForDoneUntil", "*this={}, timeoutAt={}"_f, ToString (), timeoutAt};
    if (not WaitForDoneUntilQuietly (timeoutAt)) {
        Throw (TimeOutException::kThe);
    }
}

bool Thread::Ptr::WaitForDoneUntilQuietly (Time::TimePointSeconds timeoutAt) const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs ("Thread::WaitForDoneUntilQuietly",
                                                                                 "*this={}, timeoutAt={}"_f, ToString (), timeoutAt)};
#endif
    Require (*this != nullptr);
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
    CheckForInterruption (); // always a cancelation point
    if (fRep_->fThreadDoneAndCanJoin_.WaitUntilQuietly (timeoutAt) == WaitableEvent::WaitStatus::eTriggered) {
        /*
         *  This is not critical, but has the effect of assuring the COUNT of existing threads is what the caller would expect.
         *  This really only has effect #if qStroika_Foundation_Execution_Thread_SupportThreadStatistics
         *  because that's the only time we have an important side effect of the threads finalizing.
         *
         *  @see http://stroika-bugs.sophists.com/browse/STK-496
         *
         *  NOTE: because we call this join () inside fAccessSTDThreadMutex_, its critical the running thread has terminated to the point where it will no
         *  longer access fThread_ (and therefore not lock fAccessSTDThreadMutex_)
         */
        if (fRep_->fThreadValid_ and fRep_->fThread_.joinable ()) {
            // fThread_.join () will block indefinitely - but since we waited on fRep_->fThreadDoneAndCanJoin_ - it shouldn't really take long
            fRep_->fThread_.join ();
        }
        return true;
    }
    Assert (timeoutAt <= Time::GetTickCount ()); // otherwise we couldn't have timed out
    return false;
}

#if qPlatform_Windows
void Thread::Ptr::WaitForDoneWhilePumpingMessages (Time::DurationSeconds timeout) const
{
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
    Require (*this != nullptr);
    CheckForInterruption ();
    HANDLE thread = fRep_->GetNativeHandle ();
    if (thread == INVALID_HANDLE_VALUE) {
        return;
    }
    Time::TimePointSeconds timeoutAt = Time::GetTickCount () + timeout;
    // CRUDDY impl - but decent enuf for first draft
    while (GetStatus () != Status::eCompleted) {
        Time::DurationSeconds time2Wait = timeoutAt - Time::GetTickCount ();
        if (time2Wait <= 0s) {
            Throw (TimeOutException::kThe);
        }
        Platform::Windows::WaitAndPumpMessages (nullptr, {thread}, time2Wait);
    }
    WaitForDone (); // just to get the qStroika_Foundation_Execution_Thread_SupportThreadStatistics / join ()
}
#endif

/*
 ********************************************************************************
 **************************** Thread::CleanupPtr ********************************
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
    // All Thread::New () overloads vector through this one...
    Ptr ptr = Ptr{make_shared<Ptr::Rep_> (fun2CallOnce, CombineCFGs_ (configuration))};
    if (name) {
        ptr.SetThreadName (*name);
    }
    return ptr;
}

Thread::Configuration Thread::DefaultConfiguration () noexcept
{
    return sDefaultConfiguration_.load ();
}

Thread::Configuration Thread::DefaultConfiguration (const optional<Configuration>& newConfiguration)
{
    auto result = sDefaultConfiguration_.load ();
    if (newConfiguration) {
        sDefaultConfiguration_.store (newConfiguration.value ());
    }
    return result;
}

#if qStroika_Foundation_Execution_Thread_SupportThreadStatistics
Thread::Statistics Thread::GetStatistics ()
{
    [[maybe_unused]] lock_guard critSec{sThreadSupportStatsMutex_};
    return Statistics{Containers::Set<IDType>{sRunningThreads_}};
}
#endif

void Thread::Abort (const Traversal::Iterable<Ptr>& threads)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"Thread::Abort", "threads={}"_f, threads};
#endif
    threads.Apply ([] (Ptr t) { t.Abort (); });
}

void Thread::AbortAndWaitForDoneUntil (const Traversal::Iterable<Ptr>& threads, Time::TimePointSeconds timeoutAt)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs ("Thread::AbortAndWaitForDoneUntil",
                                                                                 "threads={}, timeoutAt={}"_f, threads, timeoutAt)};
#endif
    /*
     *  Before Stroika v3, we would sometimes re-send the abort message, but no need if this is not buggy. One abort sb enuf.
     */
    Abort (threads);
    WaitForDoneUntil (threads, timeoutAt);
}

void Thread::WaitForDoneUntil (const Traversal::Iterable<Ptr>& threads, Time::TimePointSeconds timeoutAt)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"Thread::WaitForDoneUntil", "threads={}, timeoutAt={}"_f, threads, timeoutAt};
#endif
    CheckForInterruption (); // always a cancelation point (even if empty list)
    // consider rewriting so we don't do this sequentially, but 'harvest' the ones completed (much as we did in Stroika v2.1), but perhaps no point.
    // This is probably fine.
    threads.Apply ([timeoutAt] (const Ptr& t) { t.WaitForDoneUntil (timeoutAt); });
}

#if qPlatform_POSIX
namespace {
    SignalID sSignalUsedForThreadInterrupt_ = SIGUSR2;
}
SignalID Thread::SignalUsedForThreadInterrupt () noexcept
{
    return sSignalUsedForThreadInterrupt_;
}
SignalID Thread::SignalUsedForThreadInterrupt (optional<SignalID> signalNumber)
{
    SignalID result = sSignalUsedForThreadInterrupt_;
    if (signalNumber) {
        [[maybe_unused]] lock_guard critSec{sHandlerInstalled_};
        if (sHandlerInstalled_) {
            SignalHandlerRegistry::Get ().RemoveSignalHandler (SignalUsedForThreadInterrupt (), kCallInRepThreadAbortProcSignalHandler_);
            sHandlerInstalled_ = false;
        }
        sSignalUsedForThreadInterrupt_ = signalNumber.value ();
        // install new handler
        if (not sHandlerInstalled_) {
            SignalHandlerRegistry::Get ().AddSignalHandler (SignalUsedForThreadInterrupt (), kCallInRepThreadAbortProcSignalHandler_);
            sHandlerInstalled_ = true;
        }
    }
    return result;
}
#endif

/*
 ********************************************************************************
 ******************** Execution::Thread::FormatThreadID *************************
 ********************************************************************************
 */
wstring Execution::Thread::FormatThreadID (Thread::IDType threadID, const FormatThreadInfo& formatThreadInfo)
{
    return String::FromNarrowSDKString (FormatThreadID_A (threadID, formatThreadInfo)).As<wstring> ();
}

string Execution::Thread::FormatThreadID_A (Thread::IDType threadID, const FormatThreadInfo& formatThreadInfo)
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
        return formatThreadInfo.fIncludeLeadingZeros ? Characters::CString::Format ("0x%016llx", threadIDInt)
                                                     : Characters::CString::Format ("0x%llx", threadIDInt);
    }
    else {
        uint32_t threadIDInt = 0;
        out >> threadIDInt;
        /*
         *  Often, it appears ThreadIDs IDs are < 16bits, so making the printout format shorter makes it a bit more readable.
         *
         *  However, I don't see any reliable way to tell this is the case, so don't bother for now. A trouble with checking on
         *  a per-thread-id basis is that often the MAIN THREAD is 0, which is < 0xffff. Then we get one size and then
         *  on the rest a different size, so the layout in the debug trace log looks funny.
         */
        constexpr bool kUse16BitThreadIDsIfTheyFit_{false};
        const bool     kUse16Bit_ = kUse16BitThreadIDsIfTheyFit_ and threadIDInt <= 0xffff;
        if (kUse16Bit_) {
            return formatThreadInfo.fIncludeLeadingZeros ? Characters::CString::Format ("0x%04x", threadIDInt)
                                                         : Characters::CString::Format ("0x%x", threadIDInt);
        }
        else {
            return formatThreadInfo.fIncludeLeadingZeros ? Characters::CString::Format ("0x%08x", threadIDInt)
                                                         : Characters::CString::Format ("0x%x", threadIDInt);
        }
    }
}

#if qCompilerAndStdLib_ThreadLocalInlineDupSymbol_Buggy
#if __cpp_lib_jthread >= 201911
/*
     ********************************************************************************
     ************************ Thread::GetCurrentThreadStopToken *********************
     ********************************************************************************
     */
optional<stop_token> Thread::GetCurrentThreadStopToken ()
{
    if (Ptr curThread = GetCurrent ()) {
        return curThread.GetStopToken ();
    }
    else {
        return nullopt;
    }
}
#endif
#endif

#if qCompilerAndStdLib_ThreadLocalInlineDupSymbol_Buggy
/*
 ********************************************************************************
 ******************* Thread::IsCurrentThreadInterruptible ***********************
 ********************************************************************************
 */
bool Thread::IsCurrentThreadInterruptible ()
{
    return GetCurrent () != nullptr;
}
#endif

/*
 ********************************************************************************
 ********************** Execution::Thread::CheckForInterruption *****************
 ********************************************************************************
 */
void Execution::Thread::CheckForInterruption ()
{
    /*
     *  NOTE: subtle but important that we use static Thread::InterruptException::kThe so we avoid
     *  re-throw with string operations. Otheriwse we would have to use SuppressInterruptionInContext
     *  just before the actual throw.
     */
    if (shared_ptr<Ptr::Rep_> thisRunningThreadRep = Ptr::sCurrentThreadRep_.lock ()) {
        if (t_InterruptionSuppressDepth_ == 0) [[likely]] {
            if (thisRunningThreadRep->fAbortRequested_) [[unlikely]] {
                Throw (Thread::AbortException::kThe);
            }
        }
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
        else if (thisRunningThreadRep->fAbortRequested_) {
            static atomic<unsigned int> sSuperSuppress_{};
            if (++sSuperSuppress_ <= 1) {
                IgnoreExceptionsForCall (DbgTrace ("Suppressed interrupt throw: t_InterruptionSuppressDepth_={}, t_Interrupting_={}"_f,
                                                   t_InterruptionSuppressDepth_, thisRunningThreadRep->fAbortRequested_.load ()));
                sSuperSuppress_--;
            }
        }
#endif
    }
}

/*
 ********************************************************************************
 **************************** Execution::Thread::Yield **************************
 ********************************************************************************
 */
void Execution::Thread::Yield ()
{
    /*
     *  Check before so we abort more quickly, and after since while we were sleeping we could be interrupted.
     *  And this (yeild) happens at a non-time critical point, so though checking before and after is redudnant,
     *  not importantly
     */
    CheckForInterruption ();
    this_thread::yield ();
    CheckForInterruption ();
}
