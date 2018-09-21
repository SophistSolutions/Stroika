/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/CString/Utilities.h"
#include "../../../Characters/Format.h"
#include "../../../Characters/String2Int.h"
#include "../../../Execution/Exceptions.h"

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
                    effectiveReason = L"OK";
                    break;
                case StatusCodes::kMovedPermanently:
                    effectiveReason = L"Moved Permanently";
                    break;
                case StatusCodes::kUnauthorized:
                    effectiveReason = L"Unauthorized access";
                    break;
                case 402:
                    effectiveReason = L"Payment required";
                    break;
                case 403:
                    effectiveReason = L"Forbidden";
                    break;
                case StatusCodes::kNotFound:
                    effectiveReason = L"URL not found";
                    break;
                case StatusCodes::kMethodNotAllowed:
                    effectiveReason = L"Method Not allowed";
                    break;
                case 410:
                    effectiveReason = L"Gone (service has been discontinued)";
                    break;
                case 413:
                    effectiveReason = L"Request entity too large";
                    break;
                case 415:
                    effectiveReason = L"Unsupported media type";
                    break;
                case StatusCodes::kServiceUnavailable:
                    effectiveReason = L"Service temporarily unavailable: try again later";
                    break;
            }
        }
        return effectiveReason;
    }
    String mkReason_ (Status status, const String& reason)
    {
        String effectiveReason = mkCanBeEmptyReason_ (status, reason);
        ;
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
        ;
        if (effectiveReason.empty ()) {
            return CString::Format (L"HTTP exception: status %d", status);
        }
        else {
            return CString::Format (L"HTTP exception: status %d (%s)", status, effectiveReason.c_str ());
        }
    }
}

Exception::Exception (Status status, const String& reason)
    : StringException (mkExceptionMessage_ (status, reason))
    , fStatus_ (status)
    , fReason_ (reason)
{
}

String Exception::GetReason () const
{
    return mkReason_ (fStatus_, fReason_);
}

String Exception::GetStandardTextForStatus (Status s, bool forceAlwaysFound)
{
    if (forceAlwaysFound) {
        return mkReason_ (s, String ());
    }
    else {
        return mkCanBeEmptyReason_ (s, String ());
    }
}

void Exception::ThrowIfError (Status status, const String& reason)
{
    if (IsHTTPStatusOK (status))
        [[LIKELY_ATTR]]
        {
            // OK - ignore
        }
    else {
        // not sure we should throw on status 100 etc, but not sure what else todo...
        Execution::Throw (Exception (status, reason));
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
        ThrowIfError (599, L"Status: " + status + L"; " + reason);
    }
    else {
        ThrowIfError (s, reason);
    }
}
