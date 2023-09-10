/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/CString/Utilities.h"
#include "../../../Characters/Format.h"
#include "../../../Characters/String2Int.h"

#include "Exception.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::HTTP;

namespace {
    String mkCanBeEmptyReason_ (Status status, const String& reason)
    {
        //http://www.w3.org/Protocols/rfc2616/rfc2616-sec6.html#sec6.1.1
        String effectiveReason = reason;
        if (effectiveReason.empty ()) {
            switch (status) {
                case StatusCodes::kOK:
                    effectiveReason = "OK"sv;
                    break;
                case StatusCodes::kCreated:
                    effectiveReason = "Created"sv;
                    break;
                case StatusCodes::kNoContent:
                    effectiveReason = "No Content"sv;
                    break;
                case StatusCodes::kMovedPermanently:
                    effectiveReason = "Moved Permanently"sv;
                    break;
                case StatusCodes::kNotModified:
                    effectiveReason = "Not Modified"sv;
                    break;
                case StatusCodes::kBadRequest:
                    effectiveReason = "Bad Request"sv;
                    break;
                case StatusCodes::kUnauthorized:
                    effectiveReason = "Unauthorized access"sv;
                    break;
                case 402:
                    effectiveReason = "Payment required"sv;
                    break;
                case 403:
                    effectiveReason = "Forbidden"sv;
                    break;
                case StatusCodes::kNotFound:
                    effectiveReason = "URL not found"sv;
                    break;
                case StatusCodes::kMethodNotAllowed:
                    effectiveReason = "Method Not allowed"sv;
                    break;
                case 410:
                    effectiveReason = "Gone (service has been discontinued)"sv;
                    break;
                case 413:
                    effectiveReason = "Request entity too large"sv;
                    break;
                case 415:
                    effectiveReason = "Unsupported media type"sv;
                    break;
                case StatusCodes::kServiceUnavailable:
                    effectiveReason = "Service temporarily unavailable: try again later"sv;
                    break;
            }
        }
        return effectiveReason;
    }
    String mkReason_ (Status status, const String& reason)
    {
        String effectiveReason = mkCanBeEmptyReason_ (status, reason);
        if (effectiveReason.empty ()) {
            return CString::Format (L"HTTP exception: status %d", status);
        }
        else {
            return effectiveReason;
        }
    }
    String mkExceptionMessage_ (Status status, const String& reason)
    {
        String effectiveReason = mkCanBeEmptyReason_ (status, reason);
        if (effectiveReason.empty ()) {
            return CString::Format (L"HTTP exception: status %d", status);
        }
        else {
            return CString::Format (L"HTTP exception: status %d (%s)", status, effectiveReason.c_str ());
        }
    }
}

/*
 ********************************************************************************
 ************************** IO::Network::HTTP::Exception ************************
 ********************************************************************************
 */
Exception::Exception (Status status, const String& reason)
    : Execution::RuntimeErrorException<>{mkExceptionMessage_ (status, reason)}
    , fStatus_{status}
    , fReason_{reason}
{
}

String Exception::GetReason () const
{
    return mkReason_ (fStatus_, fReason_);
}

String Exception::GetStandardTextForStatus (Status s, bool forceAlwaysFound)
{
    if (forceAlwaysFound) {
        return mkReason_ (s, String{});
    }
    else {
        return mkCanBeEmptyReason_ (s, String{});
    }
}

void Exception::ThrowIfError (Status status, const String& reason)
{
    if (IsHTTPStatusOK (status)) [[likely]] {
        // OK - ignore
    }
    else {
        // not sure we should throw on status 100 etc, but not sure what else todo...
        Execution::Throw (Exception{status, reason});
    }
}

void Exception::ThrowIfError (const String& status, const String& reason)
{
    // Look for ill-formated, number, but ignore any trailing crap after the first three digits (in case some extension allows 404.3 etc,
    // which I think I've seen someplace)
    String ss = status;
    if (ss.length () > 3) {
        ss = ss.substr (0, 3);
    }
    unsigned int s = String2Int<unsigned int> (ss);
    if (s == 0) {
        ThrowIfError (599, "Status: " + status + "; " + reason);
    }
    else {
        ThrowIfError (s, reason);
    }
}
