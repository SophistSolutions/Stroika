/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/IO/Network/HTTP/ClientErrorException.h"

#include "ObjectRequestHandler.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebService;
using namespace Stroika::Frameworks::WebService::Server;
using namespace Stroika::Frameworks::WebService::Server::ObjectRequestHandler;

using IO::Network::HTTP::ClientErrorException;

/*
 ********************************************************************************
 ********* ObjectRequestHandler::ExtractArgumentsAsVariantValue *****************
 ********************************************************************************
 */
VariantValue ExtractArgumentsAsVariantValue::FromRequestBody (Request* request)
{
    RequireNotNull (request);
    return ClientErrorException::TreatExceptionsAsClientError ([&] () { return request->GetBodyVariantValue (); });
}

VariantValue ExtractArgumentsAsVariantValue::FromRequestURL (Request* request)
{
    RequireNotNull (request);
    return ClientErrorException::TreatExceptionsAsClientError ([&] () {
        Mapping<String, VariantValue> result;
        if (auto query = request->url ().GetQuery ()) {
            Mapping<String, String> unconverted = query->GetMap ();
            unconverted.Apply ([&] (const KeyValuePair<String, String>& kvp) { result.Add (kvp.fKey, VariantValue{kvp.fValue}); });
        }
        if (result.empty ()) {
            return VariantValue{};
        }
        return VariantValue{result};
    });
}

VariantValue ExtractArgumentsAsVariantValue::FromRequest (Request* request)
{
    RequireNotNull (request);
    VariantValue r = FromRequestBody (request);
    // @todo merge - @todo handle/document handling of bad body arg types

    return r;
}

/*
 ********************************************************************************
 *********************** ObjectRequestHandler::Context **************************
 ********************************************************************************
 */
String ObjectRequestHandler::Context::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "MatchedURLArgs: "sv << fMatchedURLArgs << ","sv;
    sb << "Request: "sv << fRequest << ","sv;
    sb << "Response: "sv << fResponse;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 *********************** ObjectRequestHandler::Options **************************
 ********************************************************************************
 */
String ObjectRequestHandler::Options::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "DefaultResultMediaType: "sv << fDefaultResultMediaType;
    sb << "}"sv;
    return sb;
}
