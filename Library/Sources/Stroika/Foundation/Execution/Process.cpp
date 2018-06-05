/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

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

bool Execution::IsProcessRunning ([[maybe_unused]] pid_t pid)
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
#else
    AssertNotImplemented ();
    return false;
#endif
}
