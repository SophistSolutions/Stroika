/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "Signals.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;

/*
 ********************************************************************************
 ************************** Execution::SignalToName *****************************
 ********************************************************************************
 */
Characters::String Execution::SignalToName (SignalID signal)
{
    switch (signal) {
        case SIGINT:
            return "SIGINT"sv;
        case SIGILL:
            return "SIGILL"sv;
        case SIGSEGV:
            return "SIGSEGV"sv;
        case SIGABRT:
            return "SIGABRT"sv;
        case SIGFPE:
            return "SIGFPE"sv;
#if defined(SIGSYS)
        case SIGSYS:
            return "SIGSYS"sv;
#endif
#if defined(SIGBUS)
        case SIGBUS:
            return "SIGBUS"sv;
#endif
#if defined(SIGUSR1)
        case SIGUSR1:
            return "SIGUSR1"sv;
#endif
#if defined(SIGUSR2)
        case SIGUSR2:
            return "SIGUSR2"sv;
#endif
#if defined(SIGHUP)
        case SIGHUP:
            return "SIGHUP"sv;
#endif
        case SIGTERM:
            return "SIGTERM"sv;
#if defined(SIGPIPE)
        case SIGPIPE:
            return "SIGPIPE"sv;
#endif
#if defined(SIGQUIT)
        case SIGQUIT:
            return "SIGQUIT"sv;
#endif
#if defined(SIGXCPU)
        case SIGXCPU:
            return "SIGXCPU"sv;
#endif
#if defined(SIGXFSZ)
        case SIGXFSZ:
            return "SIGXFSZ"sv;
#endif

        default:
            return Characters::Format ("Signal# {}"_f, signal);
    }
}

/*
 ********************************************************************************
 **************************** Execution::SendSignal *****************************
 ********************************************************************************
 */
errno_t Execution::SendSignal ([[maybe_unused]] std::thread::native_handle_type target, [[maybe_unused]] SignalID signal)
{
#if qStroika_Foundation_Common_Platform_POSIX
    Debug::TraceContextBumper ctx{
        Stroika_Foundation_Debug_OptionalizeTraceArgs ("Stroika::Foundation::Execution::Signals::Execution::SendSignal",
                                                       "target = {:x} signal = {}"_f, (unsigned long long)(target), SignalToName (signal))};
#else
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (
        "Stroika::Foundation::Execution::Signals::Execution::SendSignal", "signal = {}"_f, SignalToName (signal))};
#endif
#if qStroika_Foundation_Common_Platform_POSIX
    errno_t e = ::pthread_kill (target, signal);
    Verify (e == 0 or e == ESRCH);
    if (e != 0) {
        DbgTrace ("pthread_kill returned error {}"_f, e); // ESRCH can be OK, for example if abort sent to thread that already terminated
    }
    return e;
#else
    AssertNotImplemented ();
    return 0;
#endif
}
