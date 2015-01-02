/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "../../../Characters/CString/Utilities.h"
#include    "../../../Characters/Format.h"
#include    "../../../Characters/String2Int.h"
#include    "../../../Execution/Exceptions.h"

#include    "Exception.h"

using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;
using   namespace   Stroika::Foundation::IO::Network::HTTP;


namespace   {
    wstring mkCanBeEmptyReason_ (Status status, const wstring& reason)
    {
        //http://www.w3.org/Protocols/rfc2616/rfc2616-sec6.html#sec6.1.1
        wstring effectiveReason =   reason;
        if (effectiveReason.empty ()) {
            switch (status) {
                case    StatusCodes::kOK:
                    effectiveReason = L"OK";
                    break;
                case    StatusCodes::kMovedPermanently:
                    effectiveReason = L"Moved Permanently";
                    break;
                case    StatusCodes::kUnauthorized:
                    effectiveReason = L"Unauthorized access";
                    break;
                case    402:
                    effectiveReason = L"Payment required";
                    break;
                case    403:
                    effectiveReason = L"Forbidden";
                    break;
                case    StatusCodes::kNotFound:
                    effectiveReason = L"URL not found";
                    break;
                case    405:
                    effectiveReason = L"Method Nnt allowed";
                    break;
                case    410:
                    effectiveReason = L"Gone (service has been discontinued)";
                    break;
                case    413:
                    effectiveReason = L"Request entity too large";
                    break;
                case    415:
                    effectiveReason = L"Unsupported media type";
                    break;
                case    StatusCodes::kServiceUnavailable:
                    effectiveReason = L"Service temporarily unavailable: try again later";
                    break;
            }
        }
        return effectiveReason;
    }
    wstring mkReason_ (Status status, const wstring& reason)
    {
        wstring effectiveReason =   mkCanBeEmptyReason_ (status, reason);;
        if (effectiveReason.empty ()) {
            return CString::Format (L"HTTP exception: status %d", status);
        }
        else {
            return effectiveReason;
        }
    }
    wstring mkExceptionMessage (Status status, const wstring& reason)
    {
        wstring effectiveReason =   mkCanBeEmptyReason_ (status, reason);;
        if (effectiveReason.empty ()) {
            return CString::Format (L"HTTP exception: status %d", status);
        }
        else {
            return CString::Format (L"HTTP exception: status %d (%s)", status, effectiveReason.c_str ());
        }
    }
}


Exception::Exception (Status status, const wstring& reason)
    : StringException (mkExceptionMessage (status, reason))
    , fStatus_ (status)
    , fReason_ (reason)
{
}

wstring Exception::GetReason () const
{
    return mkReason_ (fStatus_, fReason_);
}

wstring Exception::GetStandardTextForStatus (Status s, bool forceAlwaysFound)
{
    if (forceAlwaysFound) {
        return mkReason_ (s, wstring ());
    }
    else {
        return mkCanBeEmptyReason_ (s, wstring ());
    }
}

void    Exception::DoThrowIfError (Status status, const wstring& reason)
{
    if (IsHTTPStatusOK (status)) {
        // OK - ignore
    }
    else {
        // not sure we should throw on status 100 etc, but not sure what else todo...
        Execution::DoThrow (Exception (status, reason));
    }
}

void    Exception::DoThrowIfError (const wstring& status, const wstring& reason)
{
    // Look for ill-formated, number, but ignore any trailing crap after the first three digits (in case some extension allows 404.3 etc,
    // which I think I've seen someplace)
    wstring ss  =   status;
    if (ss.length () > 3) {
        ss = ss.substr (0, 3);
    }
    unsigned int    s   =   String2Int<unsigned int> (ss);
    if (s == 0) {
        DoThrowIfError (599, L"Status: " + status + L"; " + reason);
    }
    else {
        DoThrowIfError (s, reason);
    }
}


