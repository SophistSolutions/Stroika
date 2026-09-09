/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdio>

#if qStroika_Foundation_Common_Platform_Windows
#include <Windows.h>
#include <wininet.h> // for error codes
#endif

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Linguistics/MessageUtilities.h"

#include "Throw.h"

#include "Exceptions.h"
#include "TimeOutException.h"

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
        auto          tmp = Linguistics::MessageUtilities::Manager::sThe.RemoveTrailingSentencePunctuation (reasonForError);
        sb << tmp.first;
        sb << " while "sv;
        for (auto i = activities.begin (); i != activities.end ();) {
            sb << i->AsString ();
            ++i;
            if (i == activities.end ()) {
                sb << tmp.second.value_or ("."sv);
            }
            else {
                // not clear yet what message will work here
                sb << ", while "sv;
            }
        }
        return sb;
    }
}

/*
 ********************************************************************************
 ****************************** ExceptionStringHelper ***************************
 ********************************************************************************
 */
ExceptionStringHelper::ExceptionStringHelper (const Characters::String& reasonForError, const Containers::Stack<Activity<>>& activities)
    : fActivities_{activities}
    , fRawErrorMessage_{reasonForError}
    , fFullErrorMessage_{mkMessage_ (reasonForError, activities)}
    , fSDKCharString_{fFullErrorMessage_.AsNarrowSDKString (eIgnoreErrors)}
{
}

/*
 ********************************************************************************
 ********************************* NestedException ******************************
 ********************************************************************************
 */
NestedException::NestedException (const exception_ptr& basedOnException)
    : NestedException{Characters::ToString (basedOnException), basedOnException}
{
}

/*
 ********************************************************************************
 ***************** Private_::SystemErrorExceptionPrivate_ ***********************
 ********************************************************************************
 */
#if qStroika_Foundation_Common_Platform_Windows

// for InternetGetConnectedState
#if _MSC_VER
#pragma comment(lib, "Wininet.lib")
#endif

optional<String> TryToOverrideDefaultWindowsSystemCategoryMessage_ (error_code errCode)
{
    if (errCode.category () == system_category ()) {
        switch (errCode.value ()) {
            case ERROR_NOT_ENOUGH_MEMORY:
                return "Not enough memory to complete that operation (ERROR_NOT_ENOUGH_MEMORY)"sv;
            case ERROR_OUTOFMEMORY:
                return "Not enough memory to complete that operation (ERROR_OUTOFMEMORY)"sv;
            case WSAEADDRNOTAVAIL:
                return "Socket address not available (WSAEADDRNOTAVAIL)"sv;
            case ERROR_INTERNET_INVALID_URL:
                return "ERROR_INTERNET_INVALID_URL"sv;
            case ERROR_INTERNET_CANNOT_CONNECT:
                return "Failed to connect to internet URL (ERROR_INTERNET_CANNOT_CONNECT)"sv;
            case ERROR_INTERNET_NAME_NOT_RESOLVED:
                return "ERROR_INTERNET_NAME_NOT_RESOLVED"sv;
            case ERROR_INTERNET_INCORRECT_HANDLE_STATE:
                return "ERROR_INTERNET_INCORRECT_HANDLE_STATE"sv;
            case ERROR_INTERNET_TIMEOUT:
                return "Operation timed out (ERROR_INTERNET_TIMEOUT)"sv;
            case ERROR_INTERNET_CONNECTION_ABORTED:
                return "ERROR_INTERNET_CONNECTION_ABORTED"sv;
            case ERROR_INTERNET_CONNECTION_RESET:
                return "ERROR_INTERNET_CONNECTION_RESET"sv;
            case ERROR_HTTP_INVALID_SERVER_RESPONSE:
                return "Invalid Server Response (ERROR_HTTP_INVALID_SERVER_RESPONSE)"sv;
            case ERROR_INTERNET_PROTOCOL_NOT_FOUND: {
                DWORD r = 0;
                if (::InternetGetConnectedState (&r, 0) and (r & INTERNET_CONNECTION_OFFLINE) == 0) {
                    return "ERROR_INTERNET_PROTOCOL_NOT_FOUND"sv;
                }
                else {
                    return "ERROR_INTERNET_PROTOCOL_NOT_FOUND (offline mode)"sv;
                }
            }
        }
    }
    return nullopt;
}
#endif
Characters::String Execution::Private_::SystemErrorExceptionPrivate_::mkMsg_ (error_code errCode)
{
    /*
     *  errc::timed_out is ETIMEDOUT, which POSIX inherited from BSD sockets, where it really did mean a
     *  connect () timeout - so glibc's strerror () says "Connection timed out". C++ then reused the same value
     *  for timeouts generally, and Stroika reuses it again for lock/thread/event waits which have nothing to do
     *  with connections. Left alone, an ordinary Synchronized<> lock timeout reports "Connection timed out" on
     *  Linux and "timed out" on Windows - confusing on one platform and inconsistent across both.
     *
     *  Only generic_category () is overridden here, and that is the whole point: make_error_code (errc::X)
     *  always produces generic_category (), so this catches exactly the case where STROIKA decided something
     *  timed out. A genuine socket timeout reported by the OS arrives via ThrowPOSIXErrNo (), which tags it
     *  with system_category () on POSIX - that keeps "Connection timed out", where it is accurate.
     */
    if (errCode.category () == generic_category () and errCode == errc::timed_out) {
        return "Operation timed out"sv; // phrasing matches the ERROR_INTERNET_TIMEOUT override below
    }
#if qStroika_Foundation_Common_Platform_Windows
    // for some messages, the default windows implementation does poorly generating messages
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
    sb += " ";
    if (errCode.category () == generic_category ()) {
        sb += "{{errno: {}}}"_f(errCode.value ());
    }
    else if (errCode.category () == system_category ()) {
#if qStroika_Foundation_Common_Platform_POSIX
        sb += "{{errno: {}}}"_f(errCode.value ());
#elif qStroika_Foundation_Common_Platform_Windows
        sb += "{{Windows error: {}}}"_f(errCode.value ());
#else
        sb += "{{system error: {}}}"_f(errCode.value ());
#endif
    }
    else {
        sb += "{{{}: {}}}"_f(Characters::String::FromNarrowSDKString (errCode.category ().name ()), errCode.value ());
    }
    return sb;
}

namespace {
    /*
     *  Shared by both TranslateException_ overloads - message is nullptr when the caller had none.
     *  The set of promotions implemented here is the one DOCUMENTED as guaranteed in Exceptions.h
     *  (@see ThrowError) - keep the two in sync.
     */
    void TranslateException_Impl_ (error_code errCode, const String* message)
    {
#if qCompilerAndStdLib_Winerror_map_doesnt_map_timeout_Buggy
        // Normalize FIRST, so every condition test below - and the caller's own e.code () == errc::timed_out -
        // sees a code which actually compares equal. MSVC's system_category does not map these onto
        // errc::timed_out, so without this a genuine timeout satisfies no timeout test at all.
        // The raw Windows value is dropped from code (), but survives in the message text; and unlike the
        // pre-v3.0d25 workaround (which threw a shared static TimeOutException) a caller-supplied message
        // survives too.
        if (errCode.category () == system_category ()) {
            switch (errCode.value ()) {
                case WAIT_TIMEOUT:           // errc::timed_out
                case ERROR_INTERNET_TIMEOUT: // ""
                    errCode = make_error_code (errc::timed_out);
                    break;
            }
        }
#endif
        if (errCode == errc::not_enough_memory) {
            Throw (bad_alloc{}); // NB: bad_alloc carries no message, so 'message' is necessarily dropped here
        }
        DISABLE_COMPILER_MSC_WARNING_START (4996);
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
        if (errCode == errc::timed_out) {
            // DEPRECATED promotion (v3.0d25): kept ONLY so existing catch (const TimeOutException&) clauses keep
            // matching. Remove this together with the TimeOutException class - and they MUST go together:
            // keeping the class while dropping this would leave those catch clauses compiling and silently
            // never firing, which is strictly worse than a compile error. @see TimeOutException.
            if (message == nullptr) {
                Throw (TimeOutException{errCode});
            }
            else {
                Throw (TimeOutException{errCode, *message});
            }
        }
        DISABLE_COMPILER_MSC_WARNING_END (4996);
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

        // double check the compare-with-conditions code working the way I think its supposed to...  matching multiple error codes -- LGP 2019-02-04
#if qStroika_Foundation_Common_Platform_Windows && qStroika_Foundation_Debug_AssertionsChecked
        if (errCode.category () == system_category ()) {
            switch (errCode.value ()) {
                case ERROR_NOT_ENOUGH_MEMORY: // errc::not_enough_memory
                case ERROR_OUTOFMEMORY:       // ""
                case WAIT_TIMEOUT:            // errc::timed_out
                case ERROR_INTERNET_TIMEOUT:  // ""
                    AssertNotReached (); // should have been caught above in if (ec == errc::... checks) - so thats not working - maybe need to add this switch or debug
                    // qCompilerAndStdLib_Winerror_map_doesnt_map_timeout_Buggy???
                    break;
            }
        }
#endif
    }
}

void Execution::Private_::SystemErrorExceptionPrivate_::TranslateException_ (error_code errCode)
{
    TranslateException_Impl_ (errCode, nullptr);
}

void Execution::Private_::SystemErrorExceptionPrivate_::TranslateException_ (error_code errCode, const String& message)
{
    TranslateException_Impl_ (errCode, &message);
}

/*
 ********************************************************************************
 *********************** Execution::GetAssociatedErrorCode **********************
 ********************************************************************************
 */
optional<error_code> Execution::GetAssociatedErrorCode (const exception_ptr& e) noexcept
{
    try {
        rethrow_exception (e);
    }
    catch (const system_error& se) {
        return se.code ();
    }
    catch (...) {
        return nullopt;
    }
}
