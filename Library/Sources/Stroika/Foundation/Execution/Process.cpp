/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

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
#if     qPlatform_POSIX
    // http://stackoverflow.com/questions/9152979/check-if-process-exists-given-its-pid
    // http://linux.die.net/man/2/getpgid
    // if not owner, trick of kill (pid, 0) returns error EPERM
    return ::getpgid (pid) > 0;
#else
    AssertNotImplemented ();
    return false;
#endif
}
