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
 ************ ObjectRequestHandler::PickOutNamedArguments ***********************
 ********************************************************************************
 */
Iterable<VariantValue> ObjectRequestHandler::PickOutNamedArguments (const Iterable<String>& argNames, const Mapping<String, VariantValue>& argumentValueMap)
{
    return argNames.Map<Iterable<VariantValue>> ([=] (const String& i) -> VariantValue { return argumentValueMap.LookupValue (i); });
}

Iterable<VariantValue> ObjectRequestHandler::PickOutNamedArguments (const Iterable<String>& argNames, const VariantValue& argumentValueMap)
{
    return PickOutNamedArguments (argNames, argumentValueMap.As<Mapping<String, VariantValue>> ());
}

/*
 ********************************************************************************
 ********* ObjectRequestHandler::ExtractArgumentsAsVariantValue *****************
 ********************************************************************************
 */
VariantValue ExtractArgumentsAsVariantValue::FromRequestBody (Request& request)
{
    return ClientErrorException::TreatExceptionsAsClientError ([&] () { return request.GetBodyVariantValue (); });
}

VariantValue ExtractArgumentsAsVariantValue::FromRequestURL (Request& request)
{
    return ClientErrorException::TreatExceptionsAsClientError ([&] () {
        Mapping<String, VariantValue> result;
        if (auto query = request.url ().GetQuery ()) {
            Mapping<String, String> unconverted = query->GetMap ();
            unconverted.Apply ([&] (const KeyValuePair<String, String>& kvp) { result.Add (kvp.fKey, VariantValue{kvp.fValue}); });
        }
        if (result.empty ()) {
            return VariantValue{};
        }
        return VariantValue{result};
    });
}

VariantValue ExtractArgumentsAsVariantValue::FromRequest (Request& request)
{
    return ClientErrorException::TreatExceptionsAsClientError ([&] () {
        VariantValue requestBody = FromRequestBody (request);
        VariantValue urlBody     = FromRequestURL (request);
        if (requestBody == VariantValue{}) {
            return urlBody;
        }
        if (urlBody == VariantValue{}) {
            return requestBody;
        }
        Assert (requestBody != VariantValue{} and urlBody != VariantValue{});
        if (requestBody.GetType () != VariantValue::eMap or urlBody.GetType () != VariantValue::eMap) {
            Execution::Throw (ClientErrorException{"Expected url and body to both be structured VariantValue type"sv});
        }
        Mapping<String, VariantValue> rr = requestBody.As<Mapping<String, VariantValue>> ();
        // merge - with url values taking precedence
        rr.AddAll (urlBody.As<Mapping<String, VariantValue>> ());
        return VariantValue{rr};
    });
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
