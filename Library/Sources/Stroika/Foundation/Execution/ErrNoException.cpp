/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <cstdio>

#include "../IO/FileAccessException.h"

#include "Throw.h"
#include "TimeOutException.h"

#include "ErrNoException.h"

using namespace Stroika;
using namespace Stroika::Foundation;

using namespace Characters;
using namespace Execution;

using Debug::TraceContextBumper;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ***************************** errno_ErrorException *****************************
 ********************************************************************************
 */
errno_ErrorException::errno_ErrorException (Execution::errno_t e)
    : Execution::Exception<> (SDKString2Wide (LookupMessage (e)))
    , fError (e)
{
}

SDKString errno_ErrorException::LookupMessage (Execution::errno_t e)
{
    SDKString justErrnoNumberMessage;
    {
        SDKChar justNumBuf[2048];
        justNumBuf[0] = '\0';
#if qPlatform_Windows
        (void)::_stprintf_s (justNumBuf, SDKSTR ("errno: %d"), e);
#else
        (void)snprintf (justNumBuf, NEltsOf (justNumBuf), SDKSTR ("errno: %d"), e);
#endif
        justErrnoNumberMessage = justNumBuf;
    }
    SDKChar buf[2048];
    buf[0] = '\0';
#if qPlatform_Windows
    if (::_tcserror_s (buf, e) == 0) {
        return buf + SDKString (SDKSTR (" (") + justErrnoNumberMessage + SDKSTR (")"));
    }
#elif qPlatform_Linux
/*
     * A bit quirky - GNU-specific and POSIX handle this API fairly differently.
     * https://linux.die.net/man/3/strerror_r - in one case returns int and 0 means worked, 
     * and other case always returns string to use (not in buf)
     */
#if (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && !_GNU_SOURCE
    // The XSI-compliant strerror_r() function returns 0 on success
    if (::strerror_r (e, buf, NEltsOf (buf)) == 0) {
        return buf + SDKString (SDKSTR (" (") + justErrnoNumberMessage + SDKSTR (")"));
    }
#else
    // the GNU-specific strerror_r() functions return the appropriate error description string
    // NOTE - this version MAY or MAY NOT modify buf - it sometimes returns static strings!
    return ::strerror_r (e, buf, NEltsOf (buf)) + SDKString (SDKSTR (" (") + justErrnoNumberMessage + SDKSTR (")"));
#endif
#elif qPlatform_POSIX
    // The XSI-compliant strerror_r() function returns 0 on success
    if (::strerror_r (e, buf, NEltsOf (buf)) == 0) {
        return buf + SDKString (SDKSTR (" (") + justErrnoNumberMessage + SDKSTR (")"));
    }
#else
    AssertNotImplemented ();
#endif
    return justErrnoNumberMessage;
}

[[noreturn]] void errno_ErrorException::Throw (Execution::errno_t error)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{L"errno_ErrorException::Throw", L"error = %d", error};
#endif

    SystemException::ThrowPOSIXErrNo (error);
}
