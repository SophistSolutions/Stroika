/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"
#include    "../Characters/String_Constant.h"
#include    "../Debug/Trace.h"
#include    "../Containers/Concrete/Queue_Array.h"

#include    "Signals.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Memory;



using   Characters::String_Constant;



/*
 ********************************************************************************
 ************************** Execution::SignalToName *****************************
 ********************************************************************************
 */
Characters::String Execution::SignalToName (SignalID signal)
{
    switch (signal) {
        case    SIGINT:
            return String_Constant (L"SIGINT");
        case    SIGILL:
            return String_Constant (L"SIGILL");
        case    SIGSEGV:
            return String_Constant (L"SIGSEGV");
        case    SIGABRT:
            return String_Constant (L"SIGABRT");
#if     defined (SIGSYS)
        case    SIGSYS:
            return String_Constant (L"SIGSYS");
#endif
#if     defined (SIGBUS)
        case    SIGBUS:
            return String_Constant (L"SIGBUS");
#endif
#if     defined (SIGUSR1)
        case    SIGUSR1:
            return String_Constant (L"SIGUSR1");
#endif
#if     defined (SIGUSR2)
        case    SIGUSR2:
            return String_Constant (L"SIGUSR2");
#endif
#if     defined (SIGHUP)
        case    SIGHUP:
            return String_Constant (L"SIGHUP");
#endif
        case    SIGTERM:
            return String_Constant (L"SIGTERM");
#if     defined (SIGPIPE)
        case    SIGPIPE:
            return String_Constant (L"SIGPIPE");
#endif
#if     defined (SIGQUIT)
        case    SIGQUIT:
            return String_Constant (L"SIGQUIT");
#endif
#if     defined (SIGXCPU)
        case    SIGXCPU:
            return String_Constant (L"SIGXCPU");
#endif
#if     defined (SIGXFSZ)
        case    SIGXFSZ:
            return String_Constant (L"SIGXFSZ");
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
errno_t    Execution::SendSignal (thread::native_handle_type h, SignalID signal)
{
    Debug::TraceContextBumper trcCtx (SDKSTR ("Stroika::Foundation::Execution::Signals::Execution::SendSignal"));
#if     qPlatform_POSIX
    DbgTrace (L"(signal = %s, 0x%lx)", SignalToName (signal).c_str (), static_cast<unsigned long> (h));
#else
    DbgTrace (L"(signal = %s)", SignalToName (signal).c_str ());
#endif
#if     qPlatform_POSIX
    errno_t e = pthread_kill (h, signal);
    Verify (e == 0 or e == ESRCH);
    return e;
#else
    AssertNotImplemented ();
#endif
}

