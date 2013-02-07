/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <map>
#include    <mutex>

#include    "../Characters/Format.h"
#include    "../Debug/Trace.h"

#include    "Signals.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;



namespace   {
    mutex sCritSection_;

    map<SignalIDType, set<SignalHandlerType>>    sHandlers_;

    bool    IsSigIgnore_ (const set<SignalHandlerType>& sigSet)
    {
        return sigSet.size () == 1 and * sigSet.begin () == SignalHandlerRegistry::kIGNORED;
    }

    void    MyHandler_ (int signal)
    {
        Debug::TraceContextBumper trcCtx (TSTR ("Stroika::Foundation::Execution::Signals::{}::MyHandler_"));
        DbgTrace (L"(signal = %s)", SignalToName (signal).c_str ());
        set<SignalHandlerType>  handlers;
        {
            lock_guard<mutex> critSec (sCritSection_);
            map<SignalIDType, set<SignalHandlerType>>::const_iterator i = sHandlers_.find (signal);
            Assert (i != sHandlers_.end ());
            handlers = i->second;
        }
        for (auto i = handlers.begin (); i != handlers.end (); ++i) {
            if (*i != SignalHandlerRegistry::kIGNORED) {
                (*i) (signal);
            }
        }
    }
}





/*
 ********************************************************************************
 ******************** Execution::SignalHandlerRegistry **************************
 ********************************************************************************
 */

const   SignalHandlerType   SignalHandlerRegistry::kIGNORED =   SIG_IGN;

SignalHandlerRegistry&  SignalHandlerRegistry::Get ()
{
    static  SignalHandlerRegistry   sThe_;
    return sThe_;
}

SignalHandlerRegistry::SignalHandlerRegistry ()
{
}

set<SignalIDType>   SignalHandlerRegistry::GetHandledSignals () const
{
    set<SignalIDType>   result;
    {
        lock_guard<mutex> critSec (sCritSection_);
        for (auto i = sHandlers_.begin (); i != sHandlers_.end (); ++i) {
            result.insert (i->first);
        }
    }
    return result;
}

set<SignalHandlerType>  SignalHandlerRegistry::GetSignalHandlers (SignalIDType signal) const
{
    lock_guard<mutex> critSec (sCritSection_);
    map<SignalIDType, set<SignalHandlerType>>::const_iterator i = sHandlers_.find (signal);
    if (i == sHandlers_.end ()) {
        return set<SignalHandlerType> ();
    }
    else {
        return i->second;
    }
}

void    SignalHandlerRegistry::SetSignalHandlers (SignalIDType signal)
{
    SetSignalHandlers (signal, set<SignalHandlerType> ());
}

void    SignalHandlerRegistry::SetSignalHandlers (SignalIDType signal, SignalHandlerType handler)
{
    SetSignalHandlers (signal, set<SignalHandlerType> (&handler, &handler + 1));
}

void    SignalHandlerRegistry::SetSignalHandlers (SignalIDType signal, const set<SignalHandlerType>& handlers)
{
    Debug::TraceContextBumper trcCtx (TSTR ("Stroika::Foundation::Execution::Signals::{}::SetSignalHandlers"));
    DbgTrace (L"(signal = %s, handlers.size () = %d, ....)", SignalToName (signal).c_str (), handlers.size ());
    lock_guard<mutex> critSec (sCritSection_);
    map<SignalIDType, set<SignalHandlerType>>::iterator i = sHandlers_.find (signal);
    if (i == sHandlers_.end ()) {
        if (not handlers.empty ()) {
            sHandlers_.insert (map<SignalIDType, set<SignalHandlerType>>::value_type (signal, handlers));
        }
    }
    else {
        i->second = handlers;
    }
    if (handlers.empty ()) {
        // nothing todo - empty list treated as not in sHandlers_ list
        (void)::signal (signal, SIG_DFL);
    }
    else if (IsSigIgnore_ (handlers)) {
        (void)::signal (signal, SIG_IGN);
    }
    else {
        (void)::signal (signal, MyHandler_);
    }
}

void    SignalHandlerRegistry::AddSignalHandler (SignalIDType signal, SignalHandlerType handler)
{
    set<SignalHandlerType>  s   =   GetSignalHandlers (signal);
    s.insert (handler);
    SetSignalHandlers (signal, s);
}

void    SignalHandlerRegistry::RemoveSignalHandler (SignalIDType signal, SignalHandlerType handler)
{
    set<SignalHandlerType>  s   =   GetSignalHandlers (signal);
    Require (s.find (handler) != s.end ());
    s.erase (handler);
    SetSignalHandlers (signal, s);
}

void    SignalHandlerRegistry::DefaultCrashSignalHandler (SignalIDType signal)
{
    DbgTrace (L"Serious Signal Error trapped: %s ... Aborting", SignalToName (signal).c_str ());
    abort ();
}

void    SignalHandlerRegistry::SetStandardCrashHandlerSignals (SignalHandlerType handler, const set<SignalIDType>& excludedSignals)
{
    if (excludedSignals.find (SIGINT) == excludedSignals.end ())         {  SetSignalHandlers (SIGINT, handler);        }
    if (excludedSignals.find (SIGILL) == excludedSignals.end ())         {  SetSignalHandlers (SIGILL, handler);        }
    if (excludedSignals.find (SIGFPE) == excludedSignals.end ())         {  SetSignalHandlers (SIGFPE, handler);        }
    if (excludedSignals.find (SIGSEGV) == excludedSignals.end ())        {  SetSignalHandlers (SIGSEGV, handler);       }
    if (excludedSignals.find (SIGTERM) == excludedSignals.end ())        {  SetSignalHandlers (SIGTERM, handler);       }
#if     qPlatform_POSIX
    if (excludedSignals.find (SIGSYS) == excludedSignals.end ())         {  SetSignalHandlers (SIGSYS, handler);        }
    if (excludedSignals.find (SIGBUS) == excludedSignals.end ())         {  SetSignalHandlers (SIGBUS, handler);        }
    if (excludedSignals.find (SIGQUIT) == excludedSignals.end ())        {  SetSignalHandlers (SIGQUIT, handler);       }
    if (excludedSignals.find (SIGPIPE) == excludedSignals.end ())        {  SetSignalHandlers (SIGPIPE, handler);       }
    if (excludedSignals.find (SIGHUP) == excludedSignals.end ())         {  SetSignalHandlers (SIGHUP, handler);        }
    if (excludedSignals.find (SIGXCPU) == excludedSignals.end ())        {  SetSignalHandlers (SIGXCPU, handler);       }
    if (excludedSignals.find (SIGXFSZ) == excludedSignals.end ())        {  SetSignalHandlers (SIGXFSZ, handler);       }
#endif
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
            return Characters::Format (L"Signal# %d", signal);
    }
}






/*
 ********************************************************************************
 **************************** Execution::SendSignal *****************************
 ********************************************************************************
 */
void    Execution::SendSignal (Thread::NativeHandleType h, SignalIDType signal)
{
    Debug::TraceContextBumper trcCtx (TSTR ("Stroika::Foundation::Execution::Signals::Execution::SendSignal"));
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
