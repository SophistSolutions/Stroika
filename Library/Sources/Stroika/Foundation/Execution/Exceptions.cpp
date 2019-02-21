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

#include "Exceptions.h"

using namespace Stroika;
using namespace Stroika::Foundation;

using namespace Characters;
using namespace Execution;

using Debug::TraceContextBumper;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ******************************** ExceptionStringHelper *************************
 ********************************************************************************
 */
ExceptionStringHelper::ExceptionStringHelper (const Characters::String& reasonForError, const Containers::Stack<Activity<>>& activities)
    : fActivities_ (activities)
	, fRawErrorMessage_ (reasonForError)
    , fErrorMessage_ (reasonForError)
    , fSDKCharString_ (reasonForError.AsNarrowSDKString ())
{
}

/*
 ********************************************************************************
 ***************** Private_::SystemErrorExceptionPrivate_ ***********************
 ********************************************************************************
 */
Characters::String Private_::SystemErrorExceptionPrivate_::mkMsg_ (error_code errCode)
{
    return Characters::String::FromNarrowSDKString (errCode.message ());
}

Characters::String Private_::SystemErrorExceptionPrivate_::mkCombinedMsg_ (error_code errCode, const Characters::String& message)
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

void Private_::SystemErrorExceptionPrivate_::TranslateException_ (error_code errCode)
{
    if (errCode == errc::not_enough_memory) {
        Throw (bad_alloc ());
    }
    if (errCode == errc::timed_out) {
        Throw (TimeOutException (errCode));
    }
    // double check the compare-with-conditions code working the way I think its supposed to...  matching multiple error codes -- LGP 2019-02-04
#if qPlatform_Windows && qDebug
    if (errCode.category () == system_category ()) {
        switch (errCode.value ()) {
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
    }
#endif
}