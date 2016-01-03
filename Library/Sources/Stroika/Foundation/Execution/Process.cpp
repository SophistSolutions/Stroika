/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#if     qPlatform_AIX
#include    <sys/stat.h>
#endif

#include    "Process.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Configuration;
using   namespace   Stroika::Foundation::Execution;



// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1





/*
 ********************************************************************************
 ********************** Execution::IsProcessRunning *****************************
 ********************************************************************************
 */

bool    Execution::IsProcessRunning (pid_t pid)
{
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper traceCtx ("Stroika::Foundation::Execution::IsProcessRunning");
    DbgTrace (L"(pid=%d)", pid);
#endif
#if     qPlatform_AIX
    // sadly getpgid doesnt appear to work on AIX --LGP 2015-11-13
    // From http://stackoverflow.com/questions/9152979/check-if-process-exists-given-its-pid
    struct stat sts;
    char    buf[1024];
    snprintf (buf, NEltsOf(buf), "/proc/%d", pid);
    if (::stat (buf, &sts) == -1 && errno == ENOENT) {
        // process doesn't exist
        return true;
    }
    else {
        return true;
    }
#elif     qPlatform_POSIX
    // http://stackoverflow.com/questions/9152979/check-if-process-exists-given-its-pid
    // http://linux.die.net/man/2/getpgid
    // if not owner, trick of kill (pid, 0) returns error EPERM
    pid_t   tmp { ::getpgid (pid) };
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"getpgid (pid=%d) -> %d, with ernno=%d", pid, tmp, errno);
#endif
    return tmp > 0;
#else
    AssertNotImplemented ();
    return false;
#endif
}
