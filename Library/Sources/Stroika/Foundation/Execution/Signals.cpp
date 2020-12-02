/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/Format.h"
#include "../Debug/Trace.h"

#include "Signals.h"

using namespace Stroika::Foundation;
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
            return L"SIGINT"sv;
        case SIGILL:
            return L"SIGILL"sv;
        case SIGSEGV:
            return L"SIGSEGV"sv;
        case SIGABRT:
            return L"SIGABRT"sv;
        case SIGFPE:
            return L"SIGFPE"sv;
#if defined(SIGSYS)
        case SIGSYS:
            return L"SIGSYS"sv;
#endif
#if defined(SIGBUS)
        case SIGBUS:
            return L"SIGBUS"sv;
#endif
#if defined(SIGUSR1)
        case SIGUSR1:
            return L"SIGUSR1"sv;
#endif
#if defined(SIGUSR2)
        case SIGUSR2:
            return L"SIGUSR2"sv;
#endif
#if defined(SIGHUP)
        case SIGHUP:
            return L"SIGHUP"sv;
#endif
        case SIGTERM:
            return L"SIGTERM"sv;
#if defined(SIGPIPE)
        case SIGPIPE:
            return L"SIGPIPE"sv;
#endif
#if defined(SIGQUIT)
        case SIGQUIT:
            return L"SIGQUIT"sv;
#endif
#if defined(SIGXCPU)
        case SIGXCPU:
            return L"SIGXCPU"sv;
#endif
#if defined(SIGXFSZ)
        case SIGXFSZ:
            return L"SIGXFSZ"sv;
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
errno_t Execution::SendSignal ([[maybe_unused]] std::thread::native_handle_type target, [[maybe_unused]] SignalID signal)
{
#if qPlatform_POSIX
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"Stroika::Foundation::Execution::Signals::Execution::SendSignal", L"target = 0x%llx, signal = %s", (unsigned long long)(target), SignalToName (signal).c_str ())};
#else
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"Stroika::Foundation::Execution::Signals::Execution::SendSignal", L"signal = %s", SignalToName (signal).c_str ())};
#endif
#if qPlatform_POSIX
    errno_t e = ::pthread_kill (target, signal);
    Verify (e == 0 or e == ESRCH);
    if (e != 0) {
        DbgTrace ("pthread_kill returned error %d", e); // ESRCH can be OK, for example if abort sent to thread that already terminated
    }
    return e;
#else
    AssertNotImplemented ();
    return 0;
#endif
}
