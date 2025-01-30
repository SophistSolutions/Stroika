/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Containers/Association.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/DataExchange/Variant/FormURLEncoded/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/FormURLEncoded/Writer.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Writer.h"
#include "Stroika/Foundation/IO/Network/Transfer/Connection.h"

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
    sb << ", code: "sv << code;
    sb << ", grant_type: "sv << grant_type;
    if (client_secret) {
        sb << ", client_secret: "sv << client_secret;
    }
    if (redirect_uri) {
        sb << ", redirect_uri: "sv << redirect_uri;
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
        {"grant_type"sv, &TokenRequest::grant_type},
        {"client_secret"sv, &TokenRequest::client_secret},
        {"redirect_uri"sv, &TokenRequest::redirect_uri},
    });
    return mapper;
}();

TypedBLOB TokenRequest::ToWireFormat () const
{
    if (code.empty ()) {
        static const auto kExcept_ = RuntimeErrorException{"Missing authorization code"sv};
        Throw (kExcept_);
    }
    if (client_id.empty ()) {
        static const auto kExcept_ = RuntimeErrorException{"Missing client_id"sv};
        Throw (kExcept_);
    }
    if (grant_type.empty ()) {
        static const auto kExcept_ = RuntimeErrorException{"Missing grant_type"sv};
        Throw (kExcept_);
    }
    BLOB reqBody = [&] () {
        Association<String, String> params{};
        params.Add ({"client_id", client_id});
        params.Add ({"code", code});
        if (client_secret) {
            params.Add ({"client_secret", *client_secret});
        }
        if (redirect_uri) {
            params.Add ({"redirect_uri", redirect_uri->As<String> ()});
        }
        params.Add ({"grant_type", grant_type});
        return Variant::FormURLEncoded::Writer{}.WriteAsBLOB (params);
    }();
    return TypedBLOB{reqBody, InternetMediaTypes::kWWWFormURLEncoded};
}

TokenRequest TokenRequest::FromWireFormat (const TypedBLOB& src)
{
    // not  sure we want to be this strict
    if (src.fType != InternetMediaTypes::kWWWFormURLEncoded) {
        static const auto kExcept_ = RuntimeErrorException{"Expected {}"_f(InternetMediaTypes::kWWWFormURLEncoded)};
        Throw (kExcept_);
    }
    Association<String, String> params              = Variant::FormURLEncoded::Reader{}.ReadAssociation (src.fData);
    static const auto           kExcept_clientid_   = RuntimeErrorException{"Missing client_id"sv};
    static const auto           kExcept_authCode_   = RuntimeErrorException{"Missing authentication code"sv};
    static const auto           kExcept_grant_type_ = RuntimeErrorException{"Missing grant_type"sv};
    return TokenRequest{.client_id     = params.LookupOneChecked ("client_id"sv, kExcept_clientid_),
                        .code          = params.LookupOneChecked ("code"sv, kExcept_authCode_),
                        .grant_type    = params.LookupOneChecked ("grant_type"sv, kExcept_grant_type_),
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
                                    return VariantValue{(*objOfType - DateTime::Now ()).As<int> ()};
                                }),
                            ObjectVariantMapper::ToObjectMapperType<DateTime> (
                                [] ([[maybe_unused]] const ObjectVariantMapper& mapper, const VariantValue& d, DateTime* into) -> void {
                                    *into = DateTime::Now ().AddSeconds (d.As<int> ());
                                })}},
        // scope in wire-format is space separated
        {"scope"sv, &TokenResponse::scope,
         TypeMappingDetails{ObjectVariantMapper::FromObjectMapperType<Set<String>> (
                                [] ([[maybe_unused]] const ObjectVariantMapper& mapper, const Set<String>* objOfType) -> VariantValue {
                                    return objOfType->Join (" "sv);
                                }),
                            ObjectVariantMapper::ToObjectMapperType<Set<String>> ([] ([[maybe_unused]] const ObjectVariantMapper& mapper, const VariantValue& d, Set<String>* into) -> void {
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
    // not  sure we want to be this strict
    if (src.fType != InternetMediaTypes::kJSON) {
        static const auto kExcept_ = RuntimeErrorException{"Expected JSON"sv};
        Throw (kExcept_);
    }
    return kMapper.ToObject<TokenResponse> (Variant::JSON::Reader{}.Read (src.fData));
}

/*
 ********************************************************************************
 ***************************** Auth::OAuth::Fetcher *****************************
 ********************************************************************************
 */
Fetcher::Fetcher (const ProviderConfiguration& providerConfiguration, const ClientConfiguration& clientConfig)
    : fProviderConfiguration_{providerConfiguration}
    , fClientConfig_{clientConfig}
{
}

TokenResponse Fetcher::Token (const TokenRequest& tr) const
{
    URI  tokenRequestURI = fProviderConfiguration_.token_uri;
    auto connection      = IO::Network::Transfer::Connection::New ();

    try {
        //DbgTrace ("sedning={}"_f, Streams::BinaryToText::Reader::New (reqBody).ReadAll ());
        IO::Network::Transfer::Response r = connection.POST (tokenRequestURI, tr.ToWireFormat ());
        Assert (r.GetSucceeded ());
        //DbgTrace ("respraw={}"_f, Streams::BinaryToText::Reader::New (r.GetData ()).ReadAll ());
        return TokenResponse::FromWireFormat (r.GetTypedData ());
    }
    catch (...) {
        DbgTrace ("Fetcher::Token: exception={}"_f, current_exception ());
        Execution::ReThrow ();
    }
}
