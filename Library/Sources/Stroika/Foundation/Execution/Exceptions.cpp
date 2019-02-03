/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <cstdio>

#if qPlatform_Windows
#include <Windows.h>
#include <wininet.h> // for error codes
#endif

#include "../Characters/Format.h"
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
 ***************************** SystemException **********************************
 ********************************************************************************
 */
Characters::String SystemException::mkMsg_ (error_code errCode)
{
    return Characters::String::FromNarrowSDKString (errCode.message ());
}

Characters::String SystemException::mkMsg_ (error_code errCode, const Characters::String& message)
{
	if (errCode.category () == system_category ()
#if qPlatform_POSIX
		or errCode.category () == generic_category ()
#endif
		) {
#if qPlatform_POSIX
		return message + L": "sv + Characters::Format (L"{errno %d}", errCode.value ());
#elif qPlatform_Windows
		return message + L": "sv + Characters::Format (L"{Windows error %d}", errCode.value ());
#else
		return message + L": "sv + Characters::Format (L"{system error %d}", errCode.value ());
#endif
	}
	return message + L": "sv + Characters::Format (L"{%s %d}", Characters::String::FromNarrowSDKString (errCode.category ().name ()).c_str (), errCode.value ());
}

#if !qPlatform_POSIX
void SystemException::ThrowPOSIXErrNo (int errNo)
{
    Require (errNo != 0);
    // @todo see errno_ErrorException::Throw () for additional needed mappings
    switch (static_cast<errc> (errNo)) {
        case errc::not_enough_memory: {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("SystemException::ThrowPOSIXErrNo (0x%x) - translating errc::not_enough_memory to bad_alloc", errNo);
#endif
            Throw (bad_alloc ());
        } break;
        case errc::timed_out: {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("SystemException::ThrowPOSIXErrNo (0x%x) - translating errc::not_enough_memory to TimeOutException", errNo);
#endif
            Throw (TimeOutException::kThe);
        }
        default: {
            Throw (SystemException (errNo, generic_category ()));
        } break;
    }
}
#endif

void SystemException::ThrowSystemErrNo (int sysErr)
{
    Require (sysErr != 0);
    // @todo see Execution::Platform::Windows::Exception::Throw - many more translations needed and this one needs testing
    error_code ec{sysErr, system_category ()};
    if (ec == errc::not_enough_memory) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("SystemException::ThrowSystemErrNo (0x%x) - translating errc::not_enough_memory to bad_alloc", sysErr);
#endif
        Throw (bad_alloc ());
    }
    if (ec == errc::timed_out) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("SystemException::ThrowSystemErrNo (0x%x) - translating errc::timed_out to TimeOutException", sysErr);
#endif
        Throw (TimeOutException (ec));
    }
#if qPlatform_Windows && qDebug
    switch (sysErr) {
        // \note - I think the design of error_category is such that the windows system_category class should override equivilent to match multiple
        // error codes to a single error_condition. But I've yet to verify this actually works that way...
        // But got the idea from the first answer to https://stackoverflow.com/questions/32232295/use-cases-for-stderror-code
        case ERROR_NOT_ENOUGH_MEMORY: // errc::not_enough_memory
        case ERROR_OUTOFMEMORY:       // ""
        case WAIT_TIMEOUT:            // errc::timed_out
        case ERROR_INTERNET_TIMEOUT:  // ""
            AssertNotReached ();      // should have been caught above in if (ec == errc::... checks) - so thats not working - maybe need to add this switch or debug
            break;
    }
#endif
    Throw (SystemException (ec));
}
