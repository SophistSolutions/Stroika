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
#include "../Linguistics/MessageUtilities.h"

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

namespace {
    // @todo this message needs lots of linguistic cleanup (punctuation, capitalization etc) - started but quite incomplete
    String mkMessage_ (const Characters::String& reasonForError, const Containers::Stack<Activity<>>& activities)
    {
        //
        // @todo rewrite this using Linguistics::CurrentLocaleMessageUtilities so we capture the 'while' crap there too
        //
        if (activities.empty ()) {
            return reasonForError;
        }
        StringBuilder sb;
        auto          tmp = Linguistics::CurrentLocaleMessageUtilities::RemoveTrailingSentencePunctuation (reasonForError);
        sb += tmp.first;
        sb += L" while ";
        for (auto i = activities.begin (); i != activities.end ();) {
            sb += i->AsString ();
            i++;
            if (i == activities.end ()) {
                sb += tmp.second.value_or (L"."sv);
            }
            else {
                // not clear yet what message will work here
                sb += L", while ";
            }
        }
        return sb.str ();
    }
}

/*
 ********************************************************************************
 ****************************** ExceptionStringHelper ***************************
 ********************************************************************************
 */
ExceptionStringHelper::ExceptionStringHelper (const Characters::String& reasonForError, const Containers::Stack<Activity<>>& activities)
    : fActivities_ (activities)
    , fRawErrorMessage_ (reasonForError)
    , fFullErrorMessage_ (mkMessage_ (reasonForError, activities))
    , fSDKCharString_ (fFullErrorMessage_.AsNarrowSDKString ())
{
}

/*
 ********************************************************************************
 ***************** Private_::SystemErrorExceptionPrivate_ ***********************
 ********************************************************************************
 */
#if qPlatform_Windows

// for InternetGetConnectedState
#if _MSC_VER
#pragma comment(lib, "Wininet.lib")
#endif

optional<String> TryToOverrideDefaultWindowsSystemCategoryMessage_ (error_code errCode)
{
    if (errCode.category () == system_category ()) {
        switch (errCode.value ()) {
            case ERROR_NOT_ENOUGH_MEMORY:
                return L"Not enough memory to complete that operation (ERROR_NOT_ENOUGH_MEMORY)"sv;
            case ERROR_OUTOFMEMORY:
                return L"Not enough memory to complete that operation (ERROR_OUTOFMEMORY)"sv;
            case WSAEADDRNOTAVAIL:
                return L"Socket address not available (WSAEADDRNOTAVAIL)"sv;
            case ERROR_INTERNET_INVALID_URL:
                return L"ERROR_INTERNET_INVALID_URL"sv;
            case ERROR_INTERNET_CANNOT_CONNECT:
                return L"Failed to connect to internet URL (ERROR_INTERNET_CANNOT_CONNECT)"sv;
            case ERROR_INTERNET_NAME_NOT_RESOLVED:
                return L"ERROR_INTERNET_NAME_NOT_RESOLVED"sv;
            case ERROR_INTERNET_INCORRECT_HANDLE_STATE:
                return L"ERROR_INTERNET_INCORRECT_HANDLE_STATE"sv;
            case ERROR_INTERNET_TIMEOUT:
                return L"Operation timed out (ERROR_INTERNET_TIMEOUT)"sv;
            case ERROR_INTERNET_CONNECTION_ABORTED:
                return L"ERROR_INTERNET_CONNECTION_ABORTED"sv;
            case ERROR_INTERNET_CONNECTION_RESET:
                return L"ERROR_INTERNET_CONNECTION_RESET"sv;
            case ERROR_HTTP_INVALID_SERVER_RESPONSE:
                return L"Invalid Server Response (ERROR_HTTP_INVALID_SERVER_RESPONSE)"sv;
            case ERROR_INTERNET_PROTOCOL_NOT_FOUND: {
                DWORD r = 0;
                if (::InternetGetConnectedState (&r, 0) and (r & INTERNET_CONNECTION_OFFLINE) == 0) {
                    return L"ERROR_INTERNET_PROTOCOL_NOT_FOUND"sv;
                }
                else {
                    return L"ERROR_INTERNET_PROTOCOL_NOT_FOUND (offline mode)"sv;
                }
            }
        }
    }
    return {};
}
#endif
Characters::String Execution::Private_::SystemErrorExceptionPrivate_::mkMsg_ (error_code errCode)
{
#if qPlatform_Windows
    // for some messages, the default windows implemation does poorly generating messages
    if (optional<String> o = TryToOverrideDefaultWindowsSystemCategoryMessage_ (errCode)) {
        return *o;
    }
#endif
    // Let the standard C++ library generate the default error message for the given error code - from the category object
    return Characters::String::FromNarrowSDKString (errCode.message ());
}

Characters::String Execution::Private_::SystemErrorExceptionPrivate_::mkCombinedMsg_ (error_code errCode, const Characters::String& message)
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

void Execution::Private_::SystemErrorExceptionPrivate_::TranslateException_ (error_code errCode)
{
    if (errCode == errc::not_enough_memory) {
        Throw (bad_alloc ());
    }
    if (errCode == errc::timed_out or errCode == errc::stream_timeout) {
        Throw (TimeOutException (errCode));
    }
#if qCompilerAndStdLib_Winerror_map_doesnt_map_timeout_Buggy
    if (errCode.category () == system_category ()) {
        switch (errCode.value ()) {
            case WAIT_TIMEOUT:           // errc::timed_out
            case ERROR_INTERNET_TIMEOUT: // ""
                // NOT a good idea becuase then code saying if (errCode==errc::timed_out) will still fail --- Throw (TimeOutException (errCode));
                Throw (TimeOutException ()); // sad to have to lose the original error, but kind of useful so if test against errc::timeout works
        }
    }
#endif

    // double check the compare-with-conditions code working the way I think its supposed to...  matching multiple error codes -- LGP 2019-02-04
#if qPlatform_Windows && qDebug
    if (errCode.category () == system_category ()) {
        switch (errCode.value ()) {
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