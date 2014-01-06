/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <mutex>

#include    "../Characters/CString/Utilities.h"
#include    "../Characters/Format.h"
#include    "../Containers/Mapping.h"
#include    "../Debug/Trace.h"
#include    "../Containers/Concrete/Queue_Array.h"

#include    "Signals.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Memory;

using   Containers::Mapping;
using   Containers::Queue;
using   Containers::Set;



// maybe useful while debugging signal code, but VERY unsafe
// and could easily be the source of bugs/deadlocks!
#ifndef qDoDbgTraceOnSignalHandlers_
#define qDoDbgTraceOnSignalHandlers_    0
#endif



namespace   {
    bool    IsSigIgnore_ (const Set<SignalHandlerType>& sigSet)
    {
        return sigSet.size () == 1 and sigSet.Contains (SignalHandlerRegistry::kIGNORED);
    }
}





/*
 ********************************************************************************
 ******************** Execution::SignalHandlerRegistry **************************
 ********************************************************************************
 */
const   SignalHandlerType   SignalHandlerRegistry::kIGNORED =   SIG_IGN;

mutex                                           SignalHandlerRegistry::sDirectSignalHandlers_CritSection_;
vector<pair<SignalIDType, SignalHandlerType>>   SignalHandlerRegistry::sDirectSignalHandlers_;
SignalHandlerRegistry                           SignalHandlerRegistry::sThe_;

SignalHandlerRegistry&  SignalHandlerRegistry::Get ()
{
    return sThe_;
}

namespace {
    Queue<SignalIDType> mkQ_ ()
    {
        Containers::Concrete::Queue_Array<SignalIDType> signalQ;
        signalQ.SetCapacity (100);  // quite arbitrary - @todo make configurable somehow...
        return signalQ;
    }
}

SignalHandlerRegistry::SignalHandlerRegistry ()
    : fHandlers_ ()
    , fIncomingSafeSignals_ (mkQ_ ())
{
}

SignalHandlerRegistry::~SignalHandlerRegistry ()
{
    Shutdown ();
}

void    SignalHandlerRegistry::Shutdown ()
{
    fBlockingQueuePusherThread_.Abort ();   // so stops processing while we remove stuff - not critical
    // important to vector through this code so we reset signal handlers to not point to stale pointers.
    for (SignalIDType si : GetHandledSignals ()) {
        SetSignalHandlers (si);
    }
    Assert (fHandlers_.empty ());
    Assert (sDirectSignalHandlers_.empty ());
    fBlockingQueuePusherThread_.AbortAndWaitForDone ();
}

Set<SignalIDType>   SignalHandlerRegistry::GetHandledSignals () const
{
    // @todo redo using Mapping<>::Keys () when implemented...
    Set<SignalIDType>   result;
    for (auto i : fHandlers_) {
        result.Add (i.fKey);
    }
    return result;
}

Set<SignalHandlerType>  SignalHandlerRegistry::GetSignalHandlers (SignalIDType signal) const
{
    return fHandlers_.LookupValue (signal);
}

void    SignalHandlerRegistry::SetSignalHandlers (SignalIDType signal)
{
    SetSignalHandlers (signal, Set<SignalHandlerType> ());
}

void    SignalHandlerRegistry::SetSignalHandlers (SignalIDType signal, SignalHandlerType handler)
{
    SetSignalHandlers (signal, Set<SignalHandlerType> ({handler}));
}

void    SignalHandlerRegistry::SetSignalHandlers (SignalIDType signal, const Set<SignalHandlerType>& handlers)
{
    Debug::TraceContextBumper trcCtx (SDKSTR ("Stroika::Foundation::Execution::Signals::{}::SetSignalHandlers"));
    DbgTrace (L"(signal = %s, handlers.size () = %d, ....)", SignalToName (signal).c_str (), handlers.size ());
    if (handlers.empty ()) {
        // save memory and remove empty items from list
        fHandlers_.Remove (signal);
    }
    else {
        fHandlers_.Add (signal, handlers);
    }
    UpdateDirectSignalHandlers_ (signal);
}

void    SignalHandlerRegistry::AddSignalHandler (SignalIDType signal, SignalHandlerType handler)
{
    Set<SignalHandlerType>  s   =   GetSignalHandlers (signal);
    s.Add (handler);
    SetSignalHandlers (signal, s);
}

void    SignalHandlerRegistry::RemoveSignalHandler (SignalIDType signal, SignalHandlerType handler)
{
    Set<SignalHandlerType>  s   =   GetSignalHandlers (signal);
    Require (s.Contains (handler));
    s.Remove (handler);
    SetSignalHandlers (signal, s);
}

void    SignalHandlerRegistry::DefaultCrashSignalHandler (SignalIDType signal)
{
    DbgTrace (L"Serious Signal Error trapped: %s ... Aborting", SignalToName (signal).c_str ());
    abort ();
}

void    SignalHandlerRegistry::SetStandardCrashHandlerSignals (SignalHandlerType handler, const Set<SignalIDType>& excludedSignals)
{
    if (not excludedSignals.Contains (SIGINT))      {  SetSignalHandlers (SIGINT, handler);        }
    if (not excludedSignals.Contains (SIGILL))      {  SetSignalHandlers (SIGILL, handler);        }
    if (not excludedSignals.Contains (SIGFPE))      {  SetSignalHandlers (SIGFPE, handler);        }
    if (not excludedSignals.Contains (SIGSEGV))     {  SetSignalHandlers (SIGSEGV, handler);       }
    if (not excludedSignals.Contains (SIGTERM))     {  SetSignalHandlers (SIGTERM, handler);       }
#if     qPlatform_POSIX
    if (not excludedSignals.Contains (SIGSYS))      {  SetSignalHandlers (SIGSYS, handler);        }
    if (not excludedSignals.Contains (SIGBUS))      {  SetSignalHandlers (SIGBUS, handler);        }
    if (not excludedSignals.Contains (SIGQUIT))     {  SetSignalHandlers (SIGQUIT, handler);       }
    if (not excludedSignals.Contains (SIGPIPE))     {  SetSignalHandlers (SIGPIPE, handler);       }
    if (not excludedSignals.Contains (SIGHUP))      {  SetSignalHandlers (SIGHUP, handler);        }
    if (not excludedSignals.Contains (SIGXCPU))     {  SetSignalHandlers (SIGXCPU, handler);       }
    if (not excludedSignals.Contains (SIGXFSZ))     {  SetSignalHandlers (SIGXFSZ, handler);       }
#endif
}

// So all signal() calls (setting up FirstPassSignalHandler_) - and setup its array of direct handlers
void    SignalHandlerRegistry::UpdateDirectSignalHandlers_ (SignalIDType forSignal)
{
    Set<SignalHandlerType>  handlers    =   GetSignalHandlers (forSignal);
    if (handlers.empty ()) {
        // nothing todo - empty list treated as not in sHandlers_ list
        lock_guard<mutex> critSec (sDirectSignalHandlers_CritSection_);
        for (auto i = sDirectSignalHandlers_.begin (); i != sDirectSignalHandlers_.end (); ) {
            if (i->first == forSignal) {
                i = sDirectSignalHandlers_.erase (i);
            }
            else {
                i++;
            }
        }
        (void)::signal (forSignal, SIG_DFL);
    }
    else if (IsSigIgnore_ (handlers)) {
        lock_guard<mutex> critSec (sDirectSignalHandlers_CritSection_);
        for (auto i = sDirectSignalHandlers_.begin (); i != sDirectSignalHandlers_.end (); ) {
            if (i->first == forSignal) {
                i = sDirectSignalHandlers_.erase (i);
            }
            else {
                i++;
            }
        }
        (void)::signal (forSignal, SIG_IGN);
    }
    else {
        bool    anyDirect   =   handlers.AnyWith ([] (const SignalHandlerType & sh) -> bool { return sh.GetType () == SignalHandlerType::Type::eDirect;});
        bool    anyIndirect =   handlers.AnyWith ([] (const SignalHandlerType & sh) -> bool { return sh.GetType () == SignalHandlerType::Type::eSafe;});

        // @todo
        // OPTIMIZE this code - so if anyDirect == false, and anyIndirect unchanged, we can avoid any upadates to the list

        // Directly copy in 'direct' signal handlers, and for indirect ones, list our indirect signal handler in the 'direct' list
        lock_guard<mutex> critSec (sDirectSignalHandlers_CritSection_);
        for (auto i = sDirectSignalHandlers_.begin (); i != sDirectSignalHandlers_.end (); ) {
            if (i->first == forSignal) {
                i = sDirectSignalHandlers_.erase (i);
            }
            else {
                i++;
            }
        }
        if (anyDirect) {
            // add them explicitly
            for (SignalHandlerType i : handlers) {
                if (i.GetType () == SignalHandlerType::Type::eDirect) {
                    sDirectSignalHandlers_.push_back (pair<SignalIDType, SignalHandlerType> (forSignal, i));
                }
            }
        }
        if (anyIndirect) {
            sDirectSignalHandlers_.push_back (pair<SignalIDType, SignalHandlerType> (forSignal, SignalHandlerType (SecondPassDelegationSignalHandler_, SignalHandlerType::Type::eDirect)));
        }
        (void)::signal (forSignal, FirstPassSignalHandler_);

        // @todo REALLY NEED MUTEX TO MAKE THIS START SAFE!
        // BUT LOW PRIIORTY
        if (anyIndirect and fBlockingQueuePusherThread_.GetStatus () == Thread::Status::eNull) {
            Thread watcherThread ([this] () {
                // This is a safe context
                Debug::TraceContextBumper trcCtx (SDKSTR ("Stroika::Foundation::Execution::Signals::{}::fBlockingQueueDelegatorThread_"));
                while (true) {
                    Debug::TraceContextBumper trcCtx (SDKSTR ("waiting for next signal"));
                    SignalIDType    i   =   fIncomingSafeSignals_.RemoveHead ();
                    DbgTrace (L"got signal: %s; ... delegating to safe handlers...", SignalToName (i).c_str ());
                    for (SignalHandlerType sh : GetSignalHandlers (i)) {
                        if (sh.GetType () == SignalHandlerType::Type::eSafe) {
                            IgnoreExceptionsExceptThreadAbortForCall (sh (i));
                        }
                    }
                }
            });
            watcherThread.SetThreadName (L"Signal Handler Safe Execution Thread");
            watcherThread.Start ();
            fBlockingQueuePusherThread_ = std::move (watcherThread);
        }
    }
}

void    SignalHandlerRegistry::FirstPassSignalHandler_ (SignalIDType signal)
{
#if     qDoDbgTraceOnSignalHandlers_
    Debug::TraceContextBumper trcCtx (SDKSTR ("Stroika::Foundation::Execution::Signals::{}::FirstPassSignalHandler_"));
    DbgTrace (L"(signal = %s)", SignalToName (signal).c_str ());
#endif
    /*
     * sDirectSignalHandlers_ may contain multiple matching signal handlers. We want to avoid deadlocks, so dont keep locked while
     * calling that handler, But the list could change out from under us. If that happens, we could mis some handlers. The alterantive
     * would  be to copy the list first. However, that might involve memory allocations, which could itself cause deadlock.
     * Its unlikely this will be and issue, so just go with this simple strategy.
     *
     *    Note - its OK to copy SignalHandlerType - even thoguh it contains a function() - which woudlnt be safe to copy - but
     *    its wrapped in a shared_ptr<> (so the copy just ups reference count whcih dooesnt allocate memory).
     */
    sDirectSignalHandlers_CritSection_.lock ();
    try {
        for (size_t i = 0; i < sDirectSignalHandlers_.size (); ++i) {
            pair<SignalIDType, SignalHandlerType>    si =    sDirectSignalHandlers_[i];
            if (si.first == signal) {
                sDirectSignalHandlers_CritSection_.unlock ();
                si.second (signal);
                sDirectSignalHandlers_CritSection_.lock ();
            }
        }
    }
    catch (...) {
    }
    sDirectSignalHandlers_CritSection_.unlock ();
}

void    SignalHandlerRegistry::SecondPassDelegationSignalHandler_ (SignalIDType signal)
{
    /*
     * This is still an unsafe context, so CAREFULLY push the signal onto the blocking queue!
     */
#if     qDoDbgTraceOnSignalHandlers_
    Debug::TraceContextBumper trcCtx (SDKSTR ("Stroika::Foundation::Execution::Signals::{}::SecondPassDelegationSignalHandler_"));
    DbgTrace (L"(signal = %s)", SignalToName (signal).c_str ());
#endif
    Get ().fIncomingSafeSignals_.AddTail (signal);
}





/*
 ********************************************************************************
 ************************** Execution::SignalToName *****************************
 ********************************************************************************
 */
wstring Execution::SignalToName (SignalIDType signal)
{
    switch (signal) {
        case    SIGINT:
            return L"SIGINT";
        case    SIGILL:
            return L"SIGILL";
        case    SIGSEGV:
            return L"SIGSEGV";
        case    SIGABRT:
            return L"SIGABRT";
#if     defined (SIGSYS)
        case    SIGSYS:
            return L"SIGSYS";
#endif
#if     defined (SIGBUS)
        case    SIGBUS:
            return L"SIGBUS";
#endif
#if     defined (SIGUSR1)
        case    SIGUSR1:
            return L"SIGUSR1";
#endif
#if     defined (SIGUSR2)
        case    SIGUSR2:
            return L"SIGUSR2";
#endif
#if     defined (SIGHUP)
        case    SIGHUP:
            return L"SIGHUP";
#endif
        case    SIGTERM:
            return L"SIGTERM";
#if     defined (SIGPIPE)
        case    SIGPIPE:
            return L"SIGPIPE";
#endif
#if     defined (SIGQUIT)
        case    SIGQUIT:
            return L"SIGQUIT";
#endif
#if     defined (SIGXCPU)
        case    SIGXCPU:
            return L"SIGXCPU";
#endif
#if     defined (SIGXFSZ)
        case    SIGXFSZ:
            return L"SIGXFSZ";
#endif

        default:
            return Characters::CString::Format (L"Signal# %d", signal);
    }
}






/*
 ********************************************************************************
 **************************** Execution::SendSignal *****************************
 ********************************************************************************
 */
void    Execution::SendSignal (Thread::NativeHandleType h, SignalIDType signal)
{
    Debug::TraceContextBumper trcCtx (SDKSTR ("Stroika::Foundation::Execution::Signals::Execution::SendSignal"));
#if     qPlatform_POSIX
    DbgTrace (L"(signal = %s, 0x%lx)", SignalToName (signal).c_str (), static_cast<unsigned long> (h));
#else
    DbgTrace (L"(signal = %s)", SignalToName (signal).c_str ());
#endif

#if     qPlatform_POSIX
    Verify (pthread_kill (h, signal) == 0);
#else
    AssertNotImplemented ();
#endif
}





#if     qPlatform_POSIX
/*
 ********************************************************************************
 ************* Execution::ScopedBlockCurrentThreadSignal ************************
 ********************************************************************************
 */
ScopedBlockCurrentThreadSignal::ScopedBlockCurrentThreadSignal (SignalIDType signal)
    : fSignal_ (signal)
    , fRestoreMask_ ()
{
    //DbgTrace (L"ScopedBlockCurrentThreadSignal blocking signals for %s", SignalToName (signal).c_str ());
    sigset_t    mySet;
    sigemptyset (&mySet);
    (void)::sigaddset (&mySet, signal);
    sigemptyset (&fRestoreMask_);           // Unclear if this emptyset call is needed?
    Verify (pthread_sigmask (SIG_BLOCK,  &mySet, &fRestoreMask_) == 0);
}

ScopedBlockCurrentThreadSignal::~ScopedBlockCurrentThreadSignal ()
{
    //DbgTrace (L"ScopedBlockCurrentThreadSignal restoriing signals for %s", SignalToName (fSignal_).c_str ());
    Verify (pthread_sigmask (SIG_SETMASK,  &fRestoreMask_, nullptr) == 0);
}
#endif
