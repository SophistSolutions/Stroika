/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/Format.h"
#include "../../../Characters/StringBuilder.h"
#include "../../../Cryptography/Encoding/Algorithm/Base64.h"
#include "../../../Execution/Activity.h"
#include "../../../Execution/RequiredComponentMissingException.h"
#include "../../../Execution/Throw.h"
#include "../../../Streams/TextReader.h"
#include "../HTTP/Headers.h"
#include "../HTTP/Methods.h"

#include "Connection.h"

#if qHasFeature_LibCurl
#include "Connection_libcurl.h"
#endif
#if qHasFeature_WinHTTP
#include "Connection_WinHTTP.h"
#endif

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::Transfer;
using namespace Stroika::Foundation::Memory;

/*
 ********************************************************************************
 *********************** Connection::Options::Authentication ********************
 ********************************************************************************
 */
String Connection::Options::Authentication::GetAuthToken () const
{
    if (fExplicitAuthToken_) {
        return *fExplicitAuthToken_;
    }
    else if (fUsernamePassword_) {
        // See https://tools.ietf.org/html/rfc2617#section-2
        // This spec says nothing of the character encoding of the username / password (at least not that section) - so assume utf8
        string tmp{fUsernamePassword_->first.AsUTF8 () + ":" + fUsernamePassword_->second.AsUTF8 ()};
        using namespace Stroika::Foundation::Cryptography;
        return String::FromASCII (Encoding::Algorithm::EncodeBase64 (BLOB::Raw (tmp.c_str (), tmp.length ())));
    }
    AssertNotReached ();
    return String{};
}

String Connection::Options::Authentication::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    sb += L"options: " + Characters::ToString (fOptions_);
    if (fExplicitAuthToken_) {
        sb += L", Explicit-Auth-Token: " + Characters::ToString (*fExplicitAuthToken_);
    }
    else if (fUsernamePassword_) {
        sb += L", username: " + fUsernamePassword_->first;
        sb += L", password: " + fUsernamePassword_->second;
    }
    sb += L"}";
    return sb.str ();
}

/*
 ********************************************************************************
 ************************ Transfer::Connection::Ptr *****************************
 ********************************************************************************
 */
Response Connection::Ptr::GET (const URI& l, const Mapping<String, String>& extraHeaders)
{
    if (URI schemeAndAuthority = l.GetSchemeAndAuthority ()) {
        SetSchemeAndAuthority (schemeAndAuthority);
    }
    Request r;
    r.fAuthorityRelativeURL = l.GetAuthorityRelativeResource<URI> ();
    r.fMethod               = HTTP::Methods::kGet;
    r.fOverrideHeaders      = extraHeaders;
    return Send (r);
}

Response Connection::Ptr::PATCH (const URI& l, const Memory::BLOB& data, const InternetMediaType& contentType, const Mapping<String, String>& extraHeaders)
{
    if (URI schemeAndAuthority = l.GetSchemeAndAuthority ()) {
        SetSchemeAndAuthority (schemeAndAuthority);
    }
    Request r;
    r.fAuthorityRelativeURL = l.GetAuthorityRelativeResource<URI> ();
    r.fMethod               = HTTP::Methods::kPatch;
    r.fOverrideHeaders      = extraHeaders;
    r.fData                 = data;
    r.SetContentType (contentType);
    return Send (r);
}

Response Connection::Ptr::POST (const URI& l, const Memory::BLOB& data, const InternetMediaType& contentType, const Mapping<String, String>& extraHeaders)
{
    if (URI schemeAndAuthority = l.GetSchemeAndAuthority ()) {
        SetSchemeAndAuthority (schemeAndAuthority);
    }
    Request r;
    r.fAuthorityRelativeURL = l.GetAuthorityRelativeResource<URI> ();
    r.fMethod               = HTTP::Methods::kPost;
    r.fOverrideHeaders      = extraHeaders;
    r.fData                 = data;
    r.SetContentType (contentType);
    return Send (r);
}

Response Connection::Ptr::DELETE (const URI& l, const Mapping<String, String>& extraHeaders)
{
    if (URI schemeAndAuthority = l.GetSchemeAndAuthority ()) {
        SetSchemeAndAuthority (schemeAndAuthority);
    }
    Request r;
    r.fAuthorityRelativeURL = l.GetAuthorityRelativeResource<URI> ();
    r.fMethod               = HTTP::Methods::kDelete;
    r.fOverrideHeaders      = extraHeaders;
    return Send (r);
}

Response Connection::Ptr::PUT (const URI& l, const Memory::BLOB& data, const InternetMediaType& contentType, const Mapping<String, String>& extraHeaders)
{
    if (URI schemeAndAuthority = l.GetSchemeAndAuthority ()) {
        SetSchemeAndAuthority (schemeAndAuthority);
    }
    Request r;
    r.fMethod               = HTTP::Methods::kPut;
    r.fAuthorityRelativeURL = l.GetAuthorityRelativeResource<URI> ();
    r.fOverrideHeaders      = extraHeaders;
    r.fData                 = data;
    r.SetContentType (contentType);
    return Send (r);
}

Response Connection::Ptr::OPTIONS (const URI& l, const Mapping<String, String>& extraHeaders)
{
    if (URI schemeAndAuthority = l.GetSchemeAndAuthority ()) {
        SetSchemeAndAuthority (schemeAndAuthority);
    }
    Request r;
    r.fMethod               = HTTP::Methods::kOptions;
    r.fAuthorityRelativeURL = l.GetAuthorityRelativeResource<URI> ();
    r.fOverrideHeaders      = extraHeaders;
    return Send (r);
}

namespace {
    constexpr bool kDeclareActivitiesFlag_Default_{true};
}

Response Connection::Ptr::Send (const Request& r)
{
    const LazyEvalActivity activity{[&] () { return L"sending '" + r.fMethod + L"' request to " + Characters::ToString (GetSchemeAndAuthority ().Combine (r.fAuthorityRelativeURL)); }};
    DeclareActivity        declaredActivity{GetOptions ().fDeclareActivities.value_or (kDeclareActivitiesFlag_Default_) ? &activity : nullptr};
    Response               response = fRep_->Send (r);
    if (not response.GetSucceeded ()) [[UNLIKELY_ATTR]] {
        Throw (Exception (response));
    }
    Ensure (response.GetSucceeded ());
    return response;
}

/*
 ********************************************************************************
 **************************** Transfer::Connection ******************************
 ********************************************************************************
 */
Connection::Ptr Transfer::Connection::New (const Connection::Options& options)
{
#if qHasFeature_LibCurl
    return Connection_LibCurl::New (options);
#endif
#if qHasFeature_WinHTTP
    return Connection_WinHTTP::New (options);
#endif
    Execution::Throw (Execution::RequiredComponentMissingException (Execution::RequiredComponentMissingException::kIONetworkClientFactory));
}
