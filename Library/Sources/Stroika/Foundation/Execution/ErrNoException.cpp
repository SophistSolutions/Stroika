/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "Exceptions.h"

#include    "ErrNoException.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;




using   namespace   Characters;
using   namespace   Execution;

using   Debug::TraceContextBumper;






/*
 ********************************************************************************
 ***************************** errno_ErrorException *****************************
 ********************************************************************************
 */
errno_ErrorException::errno_ErrorException (Execution::errno_t e)
    : StringException (SDKString2Wide (LookupMessage (e)))
    , fError (e)
{
}

SDKString errno_ErrorException::LookupMessage (Execution::errno_t e)
{
    SDKChar   buf[2048];
    buf[0] = '\0';
#if     qPlatform_Windows
    if (::_tcserror_s (buf, e) != 0) {
        return buf;
    }
    (void)::_stprintf_s (buf, SDKSTR ("errno_t error code: 0x%x"), e);
#elif   qPlatform_POSIX
    /*
     * A bit quirky - gcc and POSIX handle this API fairly differently. Hopefully I have both cases correct???
     */
#if (defined (_XOPEN_SOURCE) && _XOPEN_SOURCE >= 600) && !defined (_GNU_SOURCE)
    if (::strerror_r (e, buf, NEltsOf (buf)) == 0) {
        return buf;
    }
#else
    char* res   =   nullptr;
    if ((res = ::strerror_r (e, buf, NEltsOf (buf))) != nullptr) {
        return res;
    }
#endif
    (void) ::snprintf (buf, NEltsOf (buf), SDKSTR ("errno_t error code: 0x%x"), e);
#else
    AssertNotImplemented ();
#endif
    return buf;
}

[[noreturn]]    void    errno_ErrorException::Throw (Execution::errno_t error)
{
}
