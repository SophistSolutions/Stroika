/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"
#include    "../Debug/Trace.h"
#include    "../Containers/Concrete/Queue_Array.h"

#include    "Signals.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Memory;







/*
 ********************************************************************************
 ************************** Execution::SignalToName *****************************
 ********************************************************************************
 */
Characters::String Execution::SignalToName (SignalIDType signal)
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
void    Execution::SendSignal (thread::native_handle_type h, SignalIDType signal)
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


