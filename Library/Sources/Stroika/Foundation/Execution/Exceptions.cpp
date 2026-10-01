/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdio>

#if qStroika_Platform_Windows
#include <Windows.h>
#include <wininet.h> // for error codes
#endif

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Common/TemplateUtilities.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/Linguistics/MessageUtilities.h"

#include "Exceptions.h"

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
ExceptionStringHelper::ExceptionStringHelper (const Characters::String& reasonForError, const optional<Containers::Stack<Activity<>>>& activities)
    : fActivities_{activities}
    , fRawErrorMessage_{reasonForError}
{
}

ExceptionStringHelper::ExceptionStringHelper (const ExceptionStringHelper& src)
    : fActivities_{src.fActivities_}
    , fRawErrorMessage_{src.fRawErrorMessage_}
{
    lock_guard lk{src.fBuildMutex_};
    fBuilt_            = src.fBuilt_;
    fFullErrorMessage_ = src.fFullErrorMessage_;
    fSDKCharString_    = src.fSDKCharString_;
}

bool Execution::Private_::ShouldImbueActivities_ (const ExceptionStringHelper& e) noexcept
{
    // cheap first: an exception which specified activities is never overwritten. AnyCurrentActivities () is
    // inexpensive, and is only consulted when there is actually a decision to make.
    return e.GetActivities () == nullopt and AnyCurrentActivities ();
}

void Execution::Private_::ImbueCurrentActivities_ (ExceptionStringHelper* e)
{
    RequireNotNull (e);
    e->ImbueActivities (CaptureCurrentActivities ());
}

void ExceptionStringHelper::ImbueActivities (const optional<Containers::Stack<Activity<>>>& activities)
{
    lock_guard lk{fBuildMutex_};
    fActivities_ = activities;
    fBuilt_      = false; // the message merges these in, so anything already built is stale and gets rebuilt
}

void ExceptionStringHelper::EnsureBuilt_ () const noexcept
{
    try {
        lock_guard lk{fBuildMutex_};
        if (not fBuilt_) {
            fFullErrorMessage_ = mkMessage_ (fRawErrorMessage_, fActivities_.value_or (Containers::Stack<Activity<>>{}));
            fSDKCharString_    = fFullErrorMessage_.AsNarrowSDKString (eIgnoreErrors);
            fBuilt_            = true;
        }
    }
    catch (...) {
        // Only reachable if formatting an error message itself fails (allocation). Leave the cache empty rather
        // than propagate - callers are noexcept, and an empty what () beats std::terminate.
    }
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
namespace {
    // the category the OS's own error numbers arrive in: on Windows Stroika tags them with Win32_error_category (), but the
    // standard library (and others) still use system_category ()
    bool IsNativeSystemCategory_ (const error_category& c) noexcept
    {
#if qStroika_Platform_Windows
        if (c == Execution::Platform::Windows::Win32_error_category ()) {
            return true;
        }
#endif
        return c == system_category ();
    }
}

#if qStroika_Platform_Windows

// for InternetGetConnectedState
#if _MSC_VER
#pragma comment(lib, "Wininet.lib")
#endif

optional<String> TryToOverrideDefaultWindowsSystemCategoryMessage_ (error_code errCode)
{
    if (IsNativeSystemCategory_ (errCode.category ())) {
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
#if qStroika_Platform_Windows
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
    else if (IsNativeSystemCategory_ (errCode.category ())) {
#if qStroika_Platform_POSIX
        sb += "{{errno: {}}}"_f(errCode.value ());
#elif qStroika_Platform_Windows
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

void Execution::Private_::SystemErrorExceptionPrivate_::ThrowTranslatedExceptionIfNeeded_ (error_code errCode, [[maybe_unused]] const String* message)
{
    // Windows codes are NOT rewritten here (as WAIT_TIMEOUT / ERROR_INTERNET_TIMEOUT once were): the conditions Microsoft's
    // system_category leaves out come from Win32_error_category (), so code () keeps the raw value.
    // @see https://github.com/SophistSolutions/Stroika/issues/1192
    if (errCode == errc::not_enough_memory) {
        // Deliberately lossy - 'message' and the Activity stack are both dropped. @see ThrowError () for why
        // enriching this one is a bad trade.
        Throw (bad_alloc{});
    }
    // double check the compare-with-conditions code working the way I think its supposed to...  matching multiple error codes -- LGP 2019-02-04
#if qStroika_Platform_Windows && qStroika_Foundation_Debug_AssertionsChecked
    if (IsNativeSystemCategory_ (errCode.category ())) {
        switch (errCode.value ()) {
            case ERROR_NOT_ENOUGH_MEMORY: // errc::not_enough_memory
            case ERROR_OUTOFMEMORY:       // ""
                AssertNotReached (); // should have been caught above in if (ec == errc::... checks) - so thats not working - maybe need to add this switch or debug
                break;
        }
    }
#endif
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

/*
 ********************************************************************************
 ******************************** Execution::IsA ********************************
 ********************************************************************************
 */
bool Execution::IsA (const error_code& ec, error_condition cond) noexcept
{
#if qStroika_Platform_Windows
    // A Win32 code in std::system_category () - from the standard library, or another library - is asked as Stroika's
    // Win32_error_category (), so it gets the conditions Microsoft's leaves out. @see Execution::Platform::Windows::Win32_error_category
    if (ec.category () == system_category ()) {
        return error_code{ec.value (), Execution::Platform::Windows::Win32_error_category ()} == cond;
    }
#endif
    return ec == cond;
}

bool Execution::IsA (const system_error& e, error_condition cond) noexcept
{
    return IsA (e.code (), cond);
}

bool Execution::IsA (const exception& e, error_condition cond) noexcept
{
    if (const system_error* se = dynamic_cast<const system_error*> (&e)) {
        return IsA (se->code (), cond);
    }
    return false;
}

bool Execution::IsA (const exception_ptr& e, error_condition cond) noexcept
{
    //  No way to ask an exception_ptr anything without rethrowing it - @see GetAssociatedErrorCode,
    //  which is where that (comparatively costly) dance lives, so it exists once rather than at each
    //  call site.
    if (e == nullptr) {
        return false;
    }
    optional<error_code> ec = GetAssociatedErrorCode (e);
    return ec.has_value () and IsA (*ec, cond);
}

#if qStroika_Platform_Windows
/*
 ********************************************************************************
 *************** Execution::Platform::Windows::Win32_error_category *************
 ********************************************************************************
 */
namespace {
    // Win32 codes std::system_category () maps onto no condition (or not onto all the ones they mean). A code may be listed more
    // than once: its FIRST row is its default_error_condition (). @see Win32_error_category and
    // https://github.com/SophistSolutions/Stroika/issues/1192 (and the earlier report to Microsoft about their _Winerror_map,
    // https://developercommunity.visualstudio.com/content/problem/484206/const-int-posv-winerror-map-errval-should-probably.html)
    struct Win32ConditionMapping_ {
        int  fWin32;
        errc fCondition;
    };
    constexpr Win32ConditionMapping_ kWin32ConditionMappings_[] = {
        // winerror.h's own network errors - std maps only their Winsock (WSAE*) twins
        {ERROR_NETNAME_DELETED, errc::connection_reset},
        {ERROR_NETNAME_DELETED, errc::connection_aborted},
        {ERROR_CANCELLED, errc::operation_canceled},
        {ERROR_CONNECTION_REFUSED, errc::connection_refused},
        {ERROR_PORT_UNREACHABLE, errc::connection_refused}, // as Boost.Asio maps it (UDP: the peer's port is closed)
        {ERROR_NETWORK_UNREACHABLE, errc::network_unreachable},
        {ERROR_HOST_UNREACHABLE, errc::host_unreachable},
        {ERROR_CONNECTION_ABORTED, errc::connection_aborted},

        // WinINet - WinHTTP's ERROR_WINHTTP_* share these numbers (ERROR_WINHTTP_CONNECTION_ERROR is "reset or terminated")
        {ERROR_INTERNET_TIMEOUT, errc::timed_out},
        {ERROR_INTERNET_NAME_NOT_RESOLVED, errc::no_such_device}, // as DNS maps EAI_NONAME - there is no 'name not found' errc
        {ERROR_INTERNET_CANNOT_CONNECT, errc::connection_refused},
        {ERROR_INTERNET_CONNECTION_ABORTED, errc::connection_aborted},
        {ERROR_INTERNET_CONNECTION_ABORTED, errc::connection_reset},
        {ERROR_INTERNET_CONNECTION_RESET, errc::connection_reset},
        {ERROR_HTTP_INVALID_SERVER_RESPONSE, errc::protocol_error},
    };

    // WinINet / WinHTTP keep their message text in their own module, so FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM) - all
    // std's system_category ().message () uses - says "unknown error" for them
    optional<string> GetModuleMessage_ (int ev)
    {
        static const HMODULE kModules_[] = {
            ::LoadLibraryExW (L"winhttp.dll", nullptr, LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_SEARCH_SYSTEM32),
            ::LoadLibraryExW (L"wininet.dll", nullptr, LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_SEARCH_SYSTEM32),
        }; // loaded once (as data), and never freed - like the category itself
        for (HMODULE m : kModules_) {
            if (m == nullptr) {
                continue;
            }
            char*  buf = nullptr;
            DWORD  n   = ::FormatMessageA (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS, m,
                                           static_cast<DWORD> (ev), 0, reinterpret_cast<char*> (&buf), 0, nullptr);
            string result{buf == nullptr ? string{} : string{buf, n}};
            ::LocalFree (buf);
            while (not result.empty () and (result.back () == '\n' or result.back () == '\r' or result.back () == ' ')) {
                result.pop_back ();
            }
            if (not result.empty ()) {
                return result;
            }
        }
        return nullopt;
    }

    class Win32_error_category_ : public error_category {
    public:
        virtual const char* name () const noexcept override
        {
            return system_category ().name (); // same name as Microsoft's - @see Win32_error_category
        }
        virtual string message (int ev) const override
        {
            if (INTERNET_ERROR_BASE <= ev and ev < INTERNET_ERROR_BASE + 1000) { // the WinINet / WinHTTP block (WinHTTP runs past INTERNET_ERROR_LAST)
                if (optional<string> m = GetModuleMessage_ (ev)) {
                    return *m;
                }
            }
            return system_category ().message (ev);
        }
        virtual error_condition default_error_condition (int ev) const noexcept override
        {
            for (const Win32ConditionMapping_& i : kWin32ConditionMappings_) {
                if (i.fWin32 == ev and ev != 0) {
                    return make_error_condition (i.fCondition);
                }
            }
            return system_category ().default_error_condition (ev);
        }
        virtual bool equivalent (int ev, const error_condition& cond) const noexcept override
        {
            for (const Win32ConditionMapping_& i : kWin32ConditionMappings_) {
                if (i.fWin32 == ev and ev != 0 and make_error_condition (i.fCondition) == cond) {
                    return true;
                }
            }
            return system_category ().equivalent (ev, cond);
        }
    };
}

const error_category& Execution::Platform::Windows::Win32_error_category () noexcept
{
    return Common::Immortalize<Win32_error_category_> ();
}

bool Execution::Platform::Windows::IsWin32Error (const error_code& ec, int win32Err) noexcept
{
    return ec.value () == win32Err and (ec.category () == Win32_error_category () or ec.category () == system_category ());
}
#endif
