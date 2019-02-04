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
#include "../Characters/StringBuilder.h"
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
 ************************* SystemErrorException *********************************
 ********************************************************************************
 */
Characters::String SystemErrorException::mkMsg_ (error_code errCode)
{
    return Characters::String::FromNarrowSDKString (errCode.message ());
}

Characters::String SystemErrorException::mkCombinedMsg_ (error_code errCode, const Characters::String& message)
{
    StringBuilder sb{message};
    sb += L" ";
    if (errCode.category () == generic_category ()) {
        sb += Characters::Format (L"{errno: %d}", errCode.value ());
    }
    else if (errCode.category () == system_category ()) {
#if qPlatform_POSIX
        sb += Characters::Format (L"{errno: %d}", errCode.value ());
#elif qPlatform_Windows
        sb += Characters::Format (L"{Windows error: %d}", errCode.value ());
#else
        sb += Characters::Format (L"{system error: %d}", errCode.value ());
#endif
    }
    else {
        sb += Characters::Format (L"{%s: %d}", Characters::String::FromNarrowSDKString (errCode.category ().name ()).c_str (), errCode.value ());
    }
    return sb.str ();
}

#if !qPlatform_POSIX
void SystemErrorException::ThrowPOSIXErrNo (errno_t errNo)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContenxtBumper tctx (L"SystemErrorException::ThrowPOSIXErrNo (%d)", error);
#endif
    Require (errNo != 0);
    switch (static_cast<errc> (errNo)) {
        case errc::not_enough_memory: {
            Throw (bad_alloc ());
        } break;
        case errc::timed_out: {
            Throw (TimeOutException::kThe);
        }
        default: {
            Throw (SystemErrorException (errNo, generic_category ()));
        } break;
    }
}
#endif

void SystemErrorException::ThrowSystemErrNo (int sysErr)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContenxtBumper tctx (L"SystemErrorException::ThrowSystemErrNo (%d)", error);
#endif
    Require (sysErr != 0);
    // @todo see Execution::Platform::Windows::Exception::Throw - many more translations needed and this one needs testing
    error_code ec{sysErr, system_category ()};
    if (ec == errc::not_enough_memory) {
        Throw (bad_alloc ());
    }
    if (ec == errc::timed_out) {
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
    Throw (SystemErrorException (ec));
}
