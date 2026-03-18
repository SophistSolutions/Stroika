/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Containers/Association.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/DataExchange/Variant/FormURLEncoded/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/FormURLEncoded/Writer.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Writer.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Debug/TimingTrace.h"
#include "Stroika/Foundation/IO/Network/Transfer/Connection.h"
#include "Stroika/Foundation/Memory/Optional.h"
#include "Stroika/Foundation/Streams/BinaryToText.h"

#include "Client.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO::Network;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Auth::OAuth;

using Memory::BLOB;

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

/*
 ********************************************************************************
 ************************* Auth::OAuth::TokenRequest ****************************
 ********************************************************************************
 */
String TokenRequest::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "client_id: "sv << client_id;
    if (code) {
        sb << ", code: "sv << code;
        sb << ", grant_type: authorization_code"sv;
    }
    if (refresh_token) {
        sb << ", refresh_token: "sv << refresh_token;
        sb << ", grant_type: refresh_token"sv;
    }
    if (client_secret) {
        sb << ", client_secret: "sv << client_secret;
    }
    if (redirect_uri) {
        sb << ", redirect_uri: "sv << redirect_uri;
    }
    if (code_verifier) {
        sb << ", code_verifier: "sv << code_verifier;
    }
    sb << "}"sv;
    return sb;
}

const ObjectVariantMapper TokenRequest::kMapper = [] () {
    ObjectVariantMapper mapper;
    mapper.AddCommonType<String> ();
    mapper.AddCommonType<optional<String>> ();
    mapper.AddCommonType<optional<URI>> ();
    mapper.AddClass<TokenRequest> ({
        {"client_id"sv, &TokenRequest::client_id},
        {"code"sv, &TokenRequest::code},
        {"refresh_token"sv, &TokenRequest::refresh_token},
        {"client_secret"sv, &TokenRequest::client_secret},
        {"redirect_uri"sv, &TokenRequest::redirect_uri},
        {"code_verifier"sv, &TokenRequest::code_verifier},
    });
    return mapper;
}();

TypedBLOB TokenRequest::ToWireFormat () const
{
    if (not code and not refresh_token) {
        static const auto kExcept_ = RuntimeErrorException{"Missing authorization code/refresh_token"sv};
        Throw (kExcept_);
    }
    if (code and refresh_token) {
        static const auto kExcept_ = RuntimeErrorException{"Cannot combine authorization code/refresh_token"sv};
        Throw (kExcept_);
    }
    if (client_id.empty ()) {
        static const auto kExcept_ = RuntimeErrorException{"Missing client_id"sv};
        Throw (kExcept_);
    }
    BLOB reqBody = [&] () {
        Association<String, String> params{};
        params.Add ({"client_id"sv, client_id});
        if (code) {
            params.Add ({"code"sv, *code});
            params.Add ({"grant_type"sv, "authorization_code"sv});
        }
        else {
            params.Add ({"refresh_token"sv, *refresh_token});
            params.Add ({"grant_type"sv, "refresh_token"sv});
        }
        if (client_secret) {
            params.Add ({"client_secret"sv, *client_secret});
        }
        if (redirect_uri) {
            params.Add ({"redirect_uri"sv, redirect_uri->As<String> ()});
        }
        if (code_verifier) {
            params.Add ({"code_verifier"sv, *code_verifier});
        }
        return Variant::FormURLEncoded::Writer{}.WriteAsBLOB (params);
    }();
    return TypedBLOB{reqBody, InternetMediaTypes::kWWWFormURLEncoded};
}

TokenRequest TokenRequest::FromWireFormat (const TypedBLOB& src)
{
    if (not src.fType or not InternetMediaTypeRegistry::sThe->IsA (InternetMediaTypes::kWWWFormURLEncoded, *src.fType)) {
        static const auto kExcept_ = RuntimeErrorException{"Expected {}"_f(InternetMediaTypes::kWWWFormURLEncoded)};
        Throw (kExcept_);
    }
    Association<String, String> params              = Variant::FormURLEncoded::Reader{}.ReadAssociation (src.fData);
    static const auto           kExcept_clientid_   = RuntimeErrorException{"Missing client_id"sv};
    static const auto           kExcept_authCode_   = RuntimeErrorException{"Missing authentication code"sv};
    static const auto           kExcept_grant_type_ = RuntimeErrorException{"Missing grant_type"sv};
    auto                        code                = params.LookupOne ("code"sv);
    auto                        refresh_token       = params.LookupOne ("refresh_token"sv);
    if (not code and not refresh_token) {
        static const auto kExcept_ = RuntimeErrorException{"Missing authorization code/refresh_token"sv};
        Throw (kExcept_);
    }
    if (code and refresh_token) {
        static const auto kExcept_ = RuntimeErrorException{"Cannot combine authorization code/refresh_token"sv};
        Throw (kExcept_);
    }
    return TokenRequest{.client_id     = params.LookupOneChecked ("client_id"sv, kExcept_clientid_),
                        .code          = code,
                        .refresh_token = refresh_token,
                        .client_secret = params.LookupOne ("client_secret"sv),
                        .redirect_uri  = params.LookupOne ("redirect_uri"sv)};
}

/*
 ********************************************************************************
 ************************* Auth::OAuth::TokenResponse ***************************
 ********************************************************************************
 */
String TokenResponse::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "access_token: "sv << access_token;
    sb << ", expires_at: "sv << expires_at;
    sb << ", scope: "sv << scope;
    if (refresh_token) {
        sb << ", refresh_token: "sv << refresh_token;
    }
    if (id_token) {
        sb << ", id_token: "sv << id_token;
    }
    if (token_type) {
        sb << ", token_type: "sv << token_type;
    }
    sb << "}"sv;
    return sb;
}

const ObjectVariantMapper TokenResponse::kMapper = [] () {
    ObjectVariantMapper mapper;
    using TypeMappingDetails = ObjectVariantMapper::TypeMappingDetails;
    mapper.AddCommonType<String> ();
    mapper.AddCommonType<optional<String>> ();
    mapper.AddCommonType<DateTime> ();
    mapper.AddCommonType<Set<String>> ();
    mapper.AddClass<TokenResponse> ({
        {"access_token"sv, &TokenResponse::access_token},
        // expires_at in wire-format is expires_in seconds into future
        {"expires_in"sv, &TokenResponse::expires_at,
         TypeMappingDetails{ObjectVariantMapper::FromObjectMapperType<DateTime> (
                                [] ([[maybe_unused]] const ObjectVariantMapper& mapper, const DateTime* objOfType) -> VariantValue {
                                    return VariantValue{(objOfType->AsUTC () - DateTime::NowUTC ()).As<int> ()};
                                }),
                            ObjectVariantMapper::ToObjectMapperType<DateTime> (
                                [] ([[maybe_unused]] const ObjectVariantMapper& mapper, const VariantValue& d, DateTime* into) -> void {
                                    *into = DateTime::NowUTC ().AddSeconds (d.As<int> ());
                                })}},
        // scope in wire-format is space separated
        {"scope"sv, &TokenResponse::scope,
         TypeMappingDetails{ObjectVariantMapper::FromObjectMapperType<Set<String>> (
                                [] ([[maybe_unused]] const ObjectVariantMapper& mapper, const Set<String>* objOfType) -> VariantValue {
                                    return objOfType->Join (" "sv);
                                }),
                            ObjectVariantMapper::ToObjectMapperType<Set<String>> (
                                [] ([[maybe_unused]] const ObjectVariantMapper& mapper, const VariantValue& d, Set<String>* into) -> void {
                                    *into = Set<String>{d.As<String> ().Tokenize ()};
                                })}},
        {"refresh_token"sv, &TokenResponse::refresh_token},
        {"id_token"sv, &TokenResponse::id_token},
        {"token_type"sv, &TokenResponse::token_type},
    });
    return mapper;
}();

TypedBLOB TokenResponse::ToWireFormat () const
{
    return TypedBLOB{Variant::JSON::Writer{}.WriteAsBLOB (kMapper.FromObject (*this)), InternetMediaTypes::kJSON};
}

TokenResponse TokenResponse::FromWireFormat (const TypedBLOB& src)
{
    if (not src.fType or not InternetMediaTypeRegistry::sThe->IsA (InternetMediaTypes::kJSON, *src.fType)) {
        static const auto kExcept_ = RuntimeErrorException{"Expected JSON"sv};
        Throw (kExcept_);
    }
    return kMapper.ToObject<TokenResponse> (Variant::JSON::Reader{}.Read (src.fData));
}

/*
 ********************************************************************************
 ********************* Auth::OAuth::TokenRevocationRequest **********************
 ********************************************************************************
 */
String TokenRevocationRequest::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "access_token: "sv << access_token;
    if (refresh_token) {
        sb << ", refresh_token: "sv << *refresh_token;
    }
    if (client_id) {
        sb << ", client_id: "sv << *client_id;
    }
    if (client_secret) {
        sb << ", client_secret: "sv << *client_secret;
    }
    sb << "}"sv;
    return sb;
}

const ObjectVariantMapper TokenRevocationRequest::kMapper = [] () {
    ObjectVariantMapper mapper;
    mapper.AddCommonType<String> ();
    mapper.AddCommonType<optional<String>> ();
    mapper.AddClass<TokenRevocationRequest> ({
        {"access_token"sv, &TokenRevocationRequest::access_token},
        {"refresh_token"sv, &TokenRevocationRequest::refresh_token},
        {"client_id"sv, &TokenRevocationRequest::client_id},
        {"client_secret"sv, &TokenRevocationRequest::client_secret},
    });
    return mapper;
}();

TypedBLOB TokenRevocationRequest::ToWireFormat () const
{
    if (access_token.empty ()) {
        static const auto kExcept_ = RuntimeErrorException{"Missing access_token"sv};
        Throw (kExcept_);
    }
    BLOB reqBody = [&] () {
        Association<String, String> params{};
        if (refresh_token) {
            params.Add ({"token_type_hint"sv, "refresh_token"sv});
            params.Add ({"token"sv, *refresh_token});
        }
        else {
            params.Add ({"token_type_hint"sv, "access_token"sv});
            params.Add ({"token"sv, access_token});
        }
        if (client_id) {
            params.Add ({"client_id"sv, *client_id});
        }
        if (client_secret) {
            params.Add ({"client_secret"sv, *client_secret});
        }
        return Variant::FormURLEncoded::Writer{}.WriteAsBLOB (params);
    }();
    return TypedBLOB{reqBody, InternetMediaTypes::kWWWFormURLEncoded};
}

/*
 ********************************************************************************
 ******************** Auth::OAuth::TokenIntrospectionResponse *******************
 ********************************************************************************
 */
String TokenIntrospectionResponse::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << ", expires_at: "sv << expires_at;
    sb << "}"sv;
    return sb;
}

const ObjectVariantMapper TokenIntrospectionResponse::kMapper = [] () {
    ObjectVariantMapper mapper;
    using TypeMappingDetails = ObjectVariantMapper::TypeMappingDetails;
    mapper.AddCommonType<String> ();
    mapper.AddCommonType<optional<String>> ();
    mapper.AddCommonType<DateTime> ();
    mapper.AddCommonType<Set<String>> ();
    // @todo Introspection API INCOMPLETE.... - but need to test with provider that supports it to flesh this out usefully
    mapper.AddClass<TokenIntrospectionResponse> ({
        // expires_at in wire-format is expires_in seconds into future
        {"expires_in"sv, &TokenIntrospectionResponse::expires_at,
         TypeMappingDetails{ObjectVariantMapper::FromObjectMapperType<DateTime> (
                                [] ([[maybe_unused]] const ObjectVariantMapper& mapper, const DateTime* objOfType) -> VariantValue {
                                    return VariantValue{(objOfType->AsUTC () - DateTime::NowUTC ()).As<int> ()};
                                }),
                            ObjectVariantMapper::ToObjectMapperType<DateTime> (
                                [] ([[maybe_unused]] const ObjectVariantMapper& mapper, const VariantValue& d, DateTime* into) -> void {
                                    *into = DateTime::NowUTC ().AddSeconds (d.As<int> ());
                                })}},
    });
    return mapper;
}();

TypedBLOB TokenIntrospectionResponse::ToWireFormat () const
{
    return TypedBLOB{Variant::JSON::Writer{}.WriteAsBLOB (kMapper.FromObject (*this)), InternetMediaTypes::kJSON};
}

TokenIntrospectionResponse TokenIntrospectionResponse::FromWireFormat (const TypedBLOB& src)
{
    if (not src.fType or not InternetMediaTypeRegistry::sThe->IsA (InternetMediaTypes::kJSON, *src.fType)) {
        static const auto kExcept_ = RuntimeErrorException{"Expected JSON"sv};
        Throw (kExcept_);
    }
    return kMapper.ToObject<TokenIntrospectionResponse> (Variant::JSON::Reader{}.Read (src.fData));
}

/*
 ********************************************************************************
 ****************************** Auth::OAuth::UserInfo ***************************
 ********************************************************************************
 */
String UserInfo::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    if (name) {
        sb << "name: "sv << name;
    }
    if (given_name) {
        sb << ", given_name: "sv << given_name;
    }
    if (family_name) {
        sb << ", family_name: "sv << family_name;
    }
    if (email) {
        sb << ", email: "sv << email;
    }
    if (picture) {
        sb << ", picture: "sv << picture;
    }
    sb << "}"sv;
    return sb;
}

const ObjectVariantMapper UserInfo::kMapper = [] () {
    ObjectVariantMapper mapper;
    mapper.AddCommonType<String> ();
    mapper.AddCommonType<optional<String>> ();
    mapper.AddCommonType<URI> ();
    mapper.AddCommonType<optional<URI>> ();
    mapper.AddClass<UserInfo> ({
        {"name"sv, &UserInfo::name},
        {"given_name"sv, &UserInfo::given_name},
        {"family_name"sv, &UserInfo::family_name},
        {"email"sv, &UserInfo::email},
        {"picture"sv, &UserInfo::picture},
    });
    return mapper;
}();

UserInfo UserInfo::FromWireFormat (const TypedBLOB& src)
{
    if (not src.fType or not InternetMediaTypeRegistry::sThe->IsA (InternetMediaTypes::kJSON, *src.fType)) {
        static const auto kExcept_ = RuntimeErrorException{"Expected JSON"sv};
        Throw (kExcept_);
    }
    return kMapper.ToObject<UserInfo> (Variant::JSON::Reader{}.Read (src.fData));
}

/*
 ********************************************************************************
 ***************************** Auth::OAuth::Fetcher *****************************
 ********************************************************************************
 */
Fetcher::Fetcher (const ProviderConfiguration& providerConfiguration, const Options& options)
    : fProviderConfiguration_{providerConfiguration}
    , fOptions_{options}
    , fCache_{options.fCaching ? make_unique<Cache_> () : nullptr}
{
}

Fetcher::Fetcher (const Fetcher& src)
    : fProviderConfiguration_{src.fProviderConfiguration_}
    , fOptions_{src.fOptions_}
    , fCache_{src.fCache_ ? make_unique<Cache_> () : nullptr}
{
}

TokenResponse Fetcher::GetToken (const TokenRequest& tr) const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"OAuth::Fetcher::GetToken", "tr={}"_f, tr};
#endif
    auto nonCachingFetcher = [&] () -> TokenResponse {
        using namespace IO::Network::Transfer;
        URI  tokenRequestURI = Memory::ValueOfOrThrow (fProviderConfiguration_.token_uri, RuntimeErrorException{"no token_uri"sv});
        auto connection      = Connection::New ();
        try {
            //DbgTrace ("Sending={}"_f, Streams::BinaryToText::Convert (tr.ToWireFormat ().fData));
            Response r = connection.POST (tokenRequestURI, tr.ToWireFormat ());
            //DbgTrace ("rawResponse={}"_f, Streams::BinaryToText::Convert (r.GetData ()));
            return TokenResponse::FromWireFormat (r.GetTypedData ());
        }
        catch (...) {
            DbgTrace ("Fetcher::Token: exception={}"_f, current_exception ());
            Execution::ReThrow ();
        }
    };
    auto r = nonCachingFetcher ();
    if (fCache_) {
        optional<UserInfo> uInfo = nullopt;
        if (r.id_token) {
            // @todo
            // NOTE - ID_token doesnt contain EXACTLY same info as user_info endpoint - may need to update API to reflect this difference
            // No, a decoded ID token may not contain the exact same information as the userinfo endpoint response
            // . The information can overlap significantly, but there are key differences:

            // cache ID_Token return from TOKEN API (since that has the expiry and userinfo information)
            // This maybe best! Avoids whole API call, and I'm not sure we have the right URL todo this with facebook
            // as identity manager...
            // @todo if we got access token AND id token - parse out of ID token the user info and cache in
            // ...
        }
        fCache_->fAccessToken2UserInfo.Add (r.access_token, uInfo, r.expires_at.As<Time::TimePointSeconds> ());
    }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace ("returning: {}"_f, r);
#endif
    return r;
}

void Fetcher::RevokeTokens (const TokenRevocationRequest& tr) const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"OAuth::Fetcher::RevokeTokens", "tr={}"_f, tr};
#endif
    if (fCache_) {
        // remove references to the argument access_token (we dont cache refresh tokens currently)
        fCache_->fAccessToken2UserInfo.Remove (tr.access_token);
    }
    if (optional<URI> revokeURI = fProviderConfiguration_.revocation_endpoint) {
        using namespace IO::Network::Transfer;
        auto connection = Connection::New ();
        try {
            //DbgTrace ("Sending={}"_f, Streams::BinaryToText::Convert (tr.ToWireFormat ().fData));
            [[maybe_unused]] Response r = connection.POST (*revokeURI, tr.ToWireFormat ());
        }
        catch (...) {
            DbgTrace ("Fetcher::RevokeTokens: exception={}"_f, current_exception ());
            Execution::ReThrow ();
        }
    }
    else {
        DbgTrace ("Fetcher::RevokeTokens: skipping due to missing revocation_endpoint"_f);
    }
}

UserInfo Fetcher::GetUserInfo (const String& accessToken) const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"OAuth::Fetcher::GetUserInfo", "accessToken={}"_f, accessToken};
#endif
    if (fCache_) {
        // AccessToken can be in cache, but with no user info (if we got back GetToken result, but no ID-Token).
        if (optional<optional<UserInfo>> oou = fCache_->fAccessToken2UserInfo.Lookup (accessToken); oou and *oou) {
            return **oou;
        }
    }
    auto nonCachingFetcher = [&] () -> UserInfo {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx2{"upstream oauth provider fetcher"};
#endif
        using namespace IO::Network::Transfer;
        URI userInfoRequestURI = Memory::ValueOfOrThrow (fProviderConfiguration_.userinfo_endpoint, RuntimeErrorException{"no userinfo_endpoint"sv});
        auto authInfo   = Connection::Options::Authentication{"Bearer "sv + accessToken};
        auto connection = Connection::New (Connection::Options{.fAuthentication = authInfo});
        try {
            Response r = connection.GET (userInfoRequestURI);
            //DbgTrace ("rawResponse={}"_f, Streams::BinaryToText::Convert (r.GetData ()));
            return UserInfo::FromWireFormat (r.GetTypedData ());
        }
        catch (...) {
            DbgTrace ("Fetcher::UserInfo: exception={}"_f, current_exception ());
            Execution::ReThrow ();
        }
    };
    UserInfo userInfo = nonCachingFetcher ();
    if (fCache_) {
        optional<Time::TimePointSeconds> accessTokenExpiresAt = fCache_->fAccessToken2UserInfo.GetExpiration (accessToken);
        if (accessTokenExpiresAt == nullopt) {
            // if this is first time we've seen the access_code (e.g. load balancing situation where another server generates access_code and we dont see it)
            // we still need to know how long the user_info is valid for - so ask, and if we cannot tell, make a conservative guess
            if (optional<TokenIntrospectionResponse> o = FetchTokenIntrospectionQueitly_ (accessToken)) {
                accessTokenExpiresAt = o->expires_at.As<Time::TimePointSeconds> ();
            }
            else {
                // @todo this should be option/configurable behavior
                static constexpr auto kWAG_ = 30s;
                Time::DateTime        t     = Time::DateTime::NowUTC () + kWAG_;
                accessTokenExpiresAt        = t.As<Time::TimePointSeconds> ();
            }
        }
        Assert (accessTokenExpiresAt);
        fCache_->fAccessToken2UserInfo.Add (accessToken, userInfo, *accessTokenExpiresAt);
    }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace ("returning: {}"_f, userInfo);
#endif
    return userInfo;
}

optional<TokenIntrospectionResponse> Fetcher::FetchTokenIntrospectionQueitly_ (const String& accessToken) const
{
    if (fProviderConfiguration_.introspection_endpoint) {
        // NYI, but no biggie cuz google doesn't either
        // https://datatracker.ietf.org/doc/html/rfc7662
        AssertNotImplemented ();
    }
    if (fProviderConfiguration_.tokeninfo_endpoint) {
        // non-standard approach, but the only one that works with google, the only provider I currently support!
        using namespace IO::Network::Transfer;
        auto authInfo   = Connection::Options::Authentication{"Bearer "sv + accessToken};
        auto connection = Connection::New (Connection::Options{.fAuthentication = authInfo});
        try {
            //
            // A successful request returns a JSON object containing information about the token, such as:
            // issued_to: The client ID to whom the token was issued.
            // audience: The intended audience for the token.
            // user_id: The obfuscated unique identifier for the user.
            // scope: The space-separated list of scopes granted to the token.
            // expires_in: The number of seconds left until the token expires.
            // email: The user's email address.
            // verified_email: A boolean indicating if the email address is verified.
            // hd: The hosted domain of the user if they belong to a Google Workspace account.
            //
            // Google TokenInfo Endpoint
            // You can use this endpoint to "introspect" an access token by sending a GET request:
            // Endpoint: https://oauth2.googleapis.com/tokeninfo
            // Parameter: access_token
            // Example Request
            // http
            // GET https://oauth2.googleapis.com
            // Use code with caution.
            //
            // Expected JSON Response
            // If the token is valid, Google returns metadata including the expiration time:
            // json
            // {
            // "azp": "123456789-example.apps.googleusercontent.com",
            // "aud": "123456789-example.apps.googleusercontent.com",
            // "sub": "111222333444555",
            // "scope": "https://www.googleapis.com/auth/userinfo.email openid",
            // "exp": "1710275200",   // Expiration time in Unix epoch format
            // "expires_in": "3599",  // Seconds remaining until expiration
            // "email": "user@example.com",
            // "email_verified": "true"
            // }
            //
            // CLOSE to same as UserInfo - but all I use this for is the expiration info, so good enuf for that...
            //
            Response r = connection.GET (*fProviderConfiguration_.tokeninfo_endpoint);
            // DbgTrace ("rawResponse={}"_f, Streams::BinaryToText::Convert (r.GetData ()));
            return TokenIntrospectionResponse::FromWireFormat (r.GetTypedData ());
        }
        catch (...) {
            DbgTrace ("Fetcher::FetchTokenIntrospectionQueitly_: exception={} - BEING IGNORED"_f, current_exception ());
            return nullopt;
        }
    }
    return nullopt;
}
