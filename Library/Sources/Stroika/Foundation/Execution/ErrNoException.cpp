/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../IO/FileAccessException.h"

#include "Exceptions.h"

#include "ErrNoException.h"

using namespace Stroika;
using namespace Stroika::Foundation;

using namespace Characters;
using namespace Execution;

using Debug::TraceContextBumper;

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
    SDKString justErrnoNumberMessage;
    {
        SDKChar justNumBuf[2048];
        justNumBuf[0] = '\0';
#if qPlatform_Windows
        (void)::_stprintf_s (justNumBuf, SDKSTR ("errno: %d"), e);
#else
        (void)::snprintf (justNumBuf, NEltsOf (justNumBuf), SDKSTR ("errno: %d"), e);
#endif
        justErrnoNumberMessage = justNumBuf;
    }
    SDKChar buf[2048];
    buf[0] = '\0';
#if qPlatform_Windows
    if (::_tcserror_s (buf, e) == 0) {
        return buf + SDKString (SDKSTR (" (") + justErrnoNumberMessage + SDKSTR (")"));
    }
#elif qPlatform_POSIX
/*
     * A bit quirky - gcc and POSIX handle this API fairly differently. Hopefully I have both cases correct??? 
      * https://linux.die.net/man/3/strerror_r - in one case returns int and 0 means worked, and other case 0 means didnt work
     */
#if (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && !_GNU_SOURCE
    if (::strerror_r (e, buf, NEltsOf (buf)) == 0) {
        return buf + SDKString (SDKSTR (" (") + justErrnoNumberMessage + SDKSTR (")"));
        ;
    }
#else
    if (::strerror_r (e, buf, NEltsOf (buf)) != 0) {
        return buf + SDKString (SDKSTR (" (") + justErrnoNumberMessage + SDKSTR (")"));
        ;
    }
#endif
#else
    AssertNotImplemented ();
#endif
    return justErrnoNumberMessage;
}

[[noreturn]] void errno_ErrorException::Throw (Execution::errno_t error)
{
    //REVIEW EXCPETIONS ANMD MPAPING - THIS IS NOT GOOD - NOT EVEN CLOSE!!! -- LGP 2011-09-29
    switch (error) {
        case ENOMEM: {
            Execution::Throw (bad_alloc (), "errno_ErrorException::Throw (ENOMEM) - throwing bad_alloc");
        }
        case ENOENT: {
            Execution::Throw (IO::FileAccessException ()); // don't know if they were reading or writing at this level..., and don't know file name...
        }
        case EACCES: {
            Execution::Throw (IO::FileAccessException ()); // don't know if they were reading or writing at this level..., and don't know file name...
        }
// If I decide to pursue mapping, this maybe a good place to start
//  http://aplawrence.com/Unixart/errors.html
//      -- LGP 2009-01-02
#if 0
        case    EPERM: {
                // not sure any point in this unification. Maybe if I added my OWN private 'access denied' exception
                // the mapping/unification would make sense.
                //      -- LGP 2009-01-02
                DbgTrace ("errno_ErrorException::Throw (EPERM) - throwing ERROR_ACCESS_DENIED");
                throw Win32Exception (ERROR_ACCESS_DENIED);
            }
#endif
    }
    DbgTrace (L"errno_ErrorException (0x%x - %s) - throwing errno_ErrorException", error, SDKString2Wide (LookupMessage (error)).c_str ());
    throw errno_ErrorException (error);
}
