/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::Transfer;
using namespace Stroika::Foundation::Memory;

#if qCompiler_cpp17InlineStaticMemberOfTemplateLinkerUndefined_Buggy
/*
 ********************************************************************************
 **************************** Configuration::DefaultNames ***********************
 ********************************************************************************
 */
namespace Stroika::Foundation::Configuration {
    constexpr EnumNames<Foundation::IO::Network::Transfer::Connection::Options::Authentication::Options> DefaultNames<Foundation::IO::Network::Transfer::Connection::Options::Authentication::Options>::k;
}
#endif

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
 **************************** Transfer::Connection ******************************
 ********************************************************************************
 */
Response Connection::GET (const Mapping<String, String>& extraHeaders)
{
    Request r;
    r.fMethod          = HTTP::Methods::kGet;
    r.fOverrideHeaders = extraHeaders;
    return Send (r);
}

Response Connection::POST (const Memory::BLOB& data, const InternetMediaType& contentType, const Mapping<String, String>& extraHeaders)
{
    Request r;
    r.fMethod          = HTTP::Methods::kPost;
    r.fOverrideHeaders = extraHeaders;
    r.fData            = data;
    r.SetContentType (contentType);
    return Send (r);
}

Response Connection::DELETE (const Mapping<String, String>& extraHeaders)
{
    Request r;
    r.fMethod          = HTTP::Methods::kDelete;
    r.fOverrideHeaders = extraHeaders;
    return Send (r);
}

Response Connection::PUT (const Memory::BLOB& data, const InternetMediaType& contentType, const Mapping<String, String>& extraHeaders)
{
    Request r;
    r.fMethod          = HTTP::Methods::kPut;
    r.fOverrideHeaders = extraHeaders;
    r.fData            = data;
    r.SetContentType (contentType);
    return Send (r);
}

Response Connection::OPTIONS (const Mapping<String, String>& extraHeaders)
{
    Request r;
    r.fMethod          = HTTP::Methods::kOptions;
    r.fOverrideHeaders = extraHeaders;
    return Send (r);
}

namespace {
    constexpr bool kDeclareActivitiesFlag_Default_{true};
}

Response Connection::Send (const Request& r)
{
    const LazyEvalActivity activity{[&]() { return L"sending request to " + Characters::ToString (this->GetURL ()); }};
    DeclareActivity        declaredActivity{GetOptions ().fDeclareActivities.value_or (kDeclareActivitiesFlag_Default_) ? &activity : nullptr};
    Response               response = fRep_->Send (r);
    if (not response.GetSucceeded ())
        [[UNLIKELY_ATTR]]
        {
            Throw (Exception (response));
        }
    return response;
}
