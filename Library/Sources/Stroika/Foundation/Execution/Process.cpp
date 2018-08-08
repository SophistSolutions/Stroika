/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "Finally.h"

#include "Process.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Configuration;
using namespace Stroika::Foundation::Execution;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ********************** Execution::IsProcessRunning *****************************
 ********************************************************************************
 */

bool Execution::IsProcessRunning (pid_t pid)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper traceCtx (L"Stroika::Foundation::Execution::IsProcessRunning", L"pid=%d", pid);
#endif
#if qPlatform_POSIX
    // http://stackoverflow.com/questions/9152979/check-if-process-exists-given-its-pid
    // http://linux.die.net/man/2/getpgid
    // if not owner, trick of kill (pid, 0) returns error EPERM
    pid_t tmp{::getpgid (pid)};
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"getpgid (pid=%d) -> %d, with ernno=%d", pid, tmp, errno);
#endif
    return tmp > 0;
#elif qPlatform_Windows
    HANDLE process = ::OpenProcess (SYNCHRONIZE, FALSE, pid);
    if (process == nullptr) {
        // This can fail for a variety of reasons, including permissions, and because the process died a long time ago.
        // Probably best answer here is just to return no, that its not running
        return false;
    }
    [[maybe_unused]] auto&& cleanup = Execution::Finally ([&]() {
        ::CloseHandle (process);
    });
    constexpr bool          kUseGetExitCodeProcess_{false}; // GetExitCodeProcess can get confused by bad error code so WFSO probably better
    if constexpr (kUseGetExitCodeProcess_) {
        DWORD exitCode{};
        return ::GetExitCodeProcess (process, &exitCode) and exitCode == STILL_ACTIVE;
    }
    else {
        return ::WaitForSingleObject (process, 0) == WAIT_TIMEOUT;
    }
#else
    AssertNotImplemented ();
    return false;
#endif
}
