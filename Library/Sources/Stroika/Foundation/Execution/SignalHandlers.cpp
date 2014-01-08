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

#include    "SignalHandlers.h"


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
    bool    IsSigIgnore_ (const Set<SignalHandler>& sigSet)
    {
        return sigSet.size () == 1 and sigSet.Contains (SignalHandlerRegistry::kIGNORED);
    }
}








/*
 ********************************************************************************
 *********** Execution::SignalHandlerRegistry::SafeSignalsManager ***************
 ********************************************************************************
 */
SignalHandlerRegistry::SafeSignalsManager*  SignalHandlerRegistry::SafeSignalsManager::sThe =   nullptr;

namespace {
    Queue<SignalID> mkQ_ ()
    {
        Containers::Concrete::Queue_Array<SignalID> signalQ;
        signalQ.SetCapacity (100);  // quite arbitrary - @todo make configurable somehow...
        return signalQ;
    }
}

SignalHandlerRegistry::SafeSignalsManager::SafeSignalsManager ()
    : fIncomingSafeSignals_ (mkQ_ ())
    , fBlockingQueuePusherThread_ ()
{
    sThe = this;
    Thread watcherThread ([this] () {
        // This is a safe context
        Debug::TraceContextBumper trcCtx (SDKSTR ("Stroika::Foundation::Execution::Signals::{}::fBlockingQueueDelegatorThread_"));
        while (true) {
            Debug::TraceContextBumper trcCtx (SDKSTR ("waiting for next signal"));
            SignalID    i   =   fIncomingSafeSignals_.RemoveHead ();
            DbgTrace (L"got signal: %s; ... delegating to safe handlers...", SignalToName (i).c_str ());
            for (SignalHandler sh : SignalHandlerRegistry::Get ().GetSignalHandlers (i)) {
                if (sh.GetType () == SignalHandler::Type::eSafe) {
                    IgnoreExceptionsExceptThreadAbortForCall (sh (i));
                }
            }
        }
    });
    watcherThread.SetThreadName (L"Signal Handler Safe Execution Thread");
    watcherThread.Start ();
    fBlockingQueuePusherThread_ = std::move (watcherThread);
}

SignalHandlerRegistry::SafeSignalsManager::~SafeSignalsManager ()
{
    fBlockingQueuePusherThread_.AbortAndWaitForDone ();
    sThe = nullptr;
}





/*
 ********************************************************************************
 ******************** Execution::SignalHandlerRegistry **************************
 ********************************************************************************
 */
const   SignalHandler   SignalHandlerRegistry::kIGNORED =   SIG_IGN;

SignalHandlerRegistry&  SignalHandlerRegistry::Get ()
{
    static  SignalHandlerRegistry   sThe_;
    return sThe_;
}

SignalHandlerRegistry::SignalHandlerRegistry ()
    : fDirectSignalHandlers_CritSection_ ()
    , fDirectSignalHandlers_ ()
{
}

Set<SignalID>   SignalHandlerRegistry::GetHandledSignals () const
{
    // @todo redo using Mapping<>::Keys () when implemented...
    Set<SignalID>   result;
    for (auto i : fHandlers_) {
        result.Add (i.fKey);
    }
    return result;
}

Set<SignalHandler>  SignalHandlerRegistry::GetSignalHandlers (SignalID signal) const
{
    return fHandlers_.LookupValue (signal);
}

void    SignalHandlerRegistry::SetSignalHandlers (SignalID signal)
{
    SetSignalHandlers (signal, Set<SignalHandler> ());
}

void    SignalHandlerRegistry::SetSignalHandlers (SignalID signal, SignalHandler handler)
{
    SetSignalHandlers (signal, Set<SignalHandler> ({handler}));
}

void    SignalHandlerRegistry::SetSignalHandlers (SignalID signal, const Set<SignalHandler>& handlers)
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

void    SignalHandlerRegistry::AddSignalHandler (SignalID signal, SignalHandler handler)
{
    Set<SignalHandler>  s   =   GetSignalHandlers (signal);
    s.Add (handler);
    SetSignalHandlers (signal, s);
}

void    SignalHandlerRegistry::RemoveSignalHandler (SignalID signal, SignalHandler handler)
{
    Set<SignalHandler>  s   =   GetSignalHandlers (signal);
    Require (s.Contains (handler));
    s.Remove (handler);
    SetSignalHandlers (signal, s);
}

void    SignalHandlerRegistry::DefaultCrashSignalHandler (SignalID signal)
{
    DbgTrace (L"Serious Signal Error trapped: %s ... Aborting", SignalToName (signal).c_str ());
    abort ();
}

void    SignalHandlerRegistry::SetStandardCrashHandlerSignals (SignalHandler handler, const Set<SignalID>& excludedSignals)
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
void    SignalHandlerRegistry::UpdateDirectSignalHandlers_ (SignalID forSignal)
{
    Set<SignalHandler>  handlers    =   GetSignalHandlers (forSignal);
    if (handlers.empty ()) {
        // nothing todo - empty list treated as not in sHandlers_ list
        lock_guard<mutex> critSec (fDirectSignalHandlers_CritSection_);
        for (auto i = fDirectSignalHandlers_.begin (); i != fDirectSignalHandlers_.end (); ) {
            if (i->first == forSignal) {
                i = fDirectSignalHandlers_.erase (i);
            }
            else {
                i++;
            }
        }
        (void)::signal (forSignal, SIG_DFL);
    }
    else if (IsSigIgnore_ (handlers)) {
        lock_guard<mutex> critSec (fDirectSignalHandlers_CritSection_);
        for (auto i = fDirectSignalHandlers_.begin (); i != fDirectSignalHandlers_.end (); ) {
            if (i->first == forSignal) {
                i = fDirectSignalHandlers_.erase (i);
            }
            else {
                i++;
            }
        }
        (void)::signal (forSignal, SIG_IGN);
    }
    else {
        bool    anyDirect   =   handlers.ContainsWith ([] (const SignalHandler & sh) -> bool { return sh.GetType () == SignalHandler::Type::eDirect;});
        bool    anyIndirect =   handlers.ContainsWith ([] (const SignalHandler & sh) -> bool { return sh.GetType () == SignalHandler::Type::eSafe;});

        Assert (anyDirect or anyIndirect);

        // @todo
        // OPTIMIZE this code - so if anyDirect == false, and anyIndirect unchanged, we can avoid any upadates to the list

        // Directly copy in 'direct' signal handlers, and for indirect ones, list our indirect signal handler in the 'direct' list
        lock_guard<mutex> critSec (fDirectSignalHandlers_CritSection_);
        for (auto i = fDirectSignalHandlers_.begin (); i != fDirectSignalHandlers_.end (); ) {
            if (i->first == forSignal) {
                i = fDirectSignalHandlers_.erase (i);
            }
            else {
                i++;
            }
        }
        if (anyDirect) {
            // add them explicitly
            for (SignalHandler i : handlers) {
                if (i.GetType () == SignalHandler::Type::eDirect) {
                    fDirectSignalHandlers_.push_back (pair<SignalID, SignalHandler> (forSignal, i));
                }
                else {
                    AssertNotNull (SafeSignalsManager::sThe);/////smart/weakptr
                    Set<SignalHandler>  s   =   GetSignalHandlers (forSignal);
                    s.Add (i);
                    SafeSignalsManager::sThe->fHandlers_.Add (forSignal, s);
                }
            }
        }
        if (anyIndirect) {
            fDirectSignalHandlers_.push_back (pair<SignalID, SignalHandler> (forSignal, SignalHandler (SecondPassDelegationSignalHandler_, SignalHandler::Type::eDirect)));
        }
        (void)::signal (forSignal, FirstPassSignalHandler_);
    }
}

void    SignalHandlerRegistry::FirstPassSignalHandler_ (SignalID signal)
{
#if     qDoDbgTraceOnSignalHandlers_
    Debug::TraceContextBumper trcCtx (SDKSTR ("Stroika::Foundation::Execution::Signals::{}::FirstPassSignalHandler_"));
    DbgTrace (L"(signal = %s)", SignalToName (signal).c_str ());
#endif

    SignalHandlerRegistry&  SHR =   Get ();

    /*
     * sDirectSignalHandlers_ may contain multiple matching signal handlers. We want to avoid deadlocks, so dont keep locked while
     * calling that handler, But the list could change out from under us. If that happens, we could mis some handlers. The alterantive
     * would  be to copy the list first. However, that might involve memory allocations, which could itself cause deadlock.
     * Its unlikely this will be and issue, so just go with this simple strategy.
     *
     *    Note - its OK to copy SignalHandler - even thoguh it contains a function() - which woudlnt be safe to copy - but
     *    its wrapped in a shared_ptr<> (so the copy just ups reference count whcih dooesnt allocate memory).
     */
    SHR.fDirectSignalHandlers_CritSection_.lock ();
    try {
        for (size_t i = 0; i < SHR.fDirectSignalHandlers_.size (); ++i) {
            pair<SignalID, SignalHandler>    si =    SHR.fDirectSignalHandlers_[i];
            if (si.first == signal) {
                SHR.fDirectSignalHandlers_CritSection_.unlock ();
                si.second (signal);
                SHR.fDirectSignalHandlers_CritSection_.lock ();
            }
        }
    }
    catch (...) {
    }
    SHR.fDirectSignalHandlers_CritSection_.unlock ();
}

void    SignalHandlerRegistry::SecondPassDelegationSignalHandler_ (SignalID signal)
{
    /*
     * This is still an unsafe context, so CAREFULLY push the signal onto the blocking queue!
     */
#if     qDoDbgTraceOnSignalHandlers_
    Debug::TraceContextBumper trcCtx (SDKSTR ("Stroika::Foundation::Execution::Signals::{}::SecondPassDelegationSignalHandler_"));
    DbgTrace (L"(signal = %s)", SignalToName (signal).c_str ());
#endif
    AssertNotNull (SafeSignalsManager::sThe);/////smart/weakptr
    SafeSignalsManager::sThe->fIncomingSafeSignals_.AddTail (signal);
}





#if     qPlatform_POSIX
/*
 ********************************************************************************
 ************* Execution::ScopedBlockCurrentThreadSignal ************************
 ********************************************************************************
 */
ScopedBlockCurrentThreadSignal::ScopedBlockCurrentThreadSignal (SignalID signal)
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
