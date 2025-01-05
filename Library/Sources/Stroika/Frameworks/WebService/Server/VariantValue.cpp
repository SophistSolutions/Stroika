/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Writer.h"
#include "Stroika/Foundation/IO/Network/HTTP/ClientErrorException.h"

#include "Stroika/Frameworks/WebService/Server/Basic.h"

#include "VariantValue.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebService;
using namespace Stroika::Frameworks::WebService::Server;

using IO::Network::HTTP::ClientErrorException;

/*
 ********************************************************************************
 ******** WebService::Server::VariantValue::PickoutParamValuesFromURL ***********
 ********************************************************************************
 */
Mapping<String, DataExchange::VariantValue> Server::VariantValue::PickoutParamValuesFromURL (const URI& url)
{
    return ClientErrorException::TreatExceptionsAsClientError ([&] () {
        Mapping<String, VariantValue> result;
        if (auto query = url.GetQuery ()) {
            Mapping<String, String> unconverted = query->GetMap ();
            unconverted.Apply ([&] (const KeyValuePair<String, String>& kvp) { result.Add (kvp.fKey, VariantValue{kvp.fValue}); });
        }
        return result;
    });
}

/*
 ********************************************************************************
 ******* WebService::Server::VariantValue::PickoutParamValuesFromBody ***********
 ********************************************************************************
 */
Mapping<String, DataExchange::VariantValue> Server::VariantValue::PickoutParamValuesFromBody (const BLOB& body, const optional<InternetMediaType>& bodyContentType)
{
    static const InternetMediaType kDefaultCT_ = DataExchange::InternetMediaTypes::kJSON;
    if (bodyContentType.value_or (kDefaultCT_) == DataExchange::InternetMediaTypes::kJSON) {
        return body.empty () ? Mapping<String, DataExchange::VariantValue>{} : ClientErrorException::TreatExceptionsAsClientError ([&] () {
            return Variant::JSON::Reader{}.Read (body).As<Mapping<String, DataExchange::VariantValue>> ();
        });
    }
    Throw (ClientErrorException{"Unrecognized content-type"sv});
}

/*
 ********************************************************************************
 **** WebService::Server::VariantValue::CombineWebServiceArgsAsVariantValue *****
 ********************************************************************************
 */
DataExchange::VariantValue Server::VariantValue::CombineWebServiceArgsAsVariantValue (Request& request)
{
    return ClientErrorException::TreatExceptionsAsClientError ([&] () {
        Mapping<String, DataExchange::VariantValue> result;
        {
            Memory::BLOB inData = request.GetBody ();
            if (not inData.empty ()) {
                DataExchange::VariantValue bodyObj = Variant::JSON::Reader{}.Read (inData);
                switch (bodyObj.GetType ()) {
                    case DataExchange::VariantValue::eMap:
                        result = bodyObj.As<Mapping<String, DataExchange::VariantValue>> ();
                        break;
                    case DataExchange::VariantValue::eNull:
                        break;
                    default:
                        // Other types cannot be merged with URL data, so just return what we had in the body
                        return bodyObj;
                }
            }
        }
        result.AddAll (PickoutParamValuesFromURL (request));
        return result.empty () ? DataExchange::VariantValue{} : DataExchange::VariantValue{result};
    });
}

/*
 ********************************************************************************
 ************* WebService::Server::VariantValue::PickoutParamValues *************
 ********************************************************************************
 */
Mapping<String, DataExchange::VariantValue> Server::VariantValue::PickoutParamValues (Request& request)
{
    Mapping<String, DataExchange::VariantValue> result = PickoutParamValuesFromURL (request);
    // body params take precedence, if they overlap
    PickoutParamValuesFromBody (request).Apply ([&] (auto i) { result.Add (i.fKey, i.fValue); });
    return result;
}

/*
 ********************************************************************************
 *************** Server::VariantValue::PickOutNamedArguments ********************
 ********************************************************************************
 */
Iterable<DataExchange::VariantValue> Server::VariantValue::PickOutNamedArguments (const Iterable<String>&              argNames,
                                                                                  const Mapping<String, VariantValue>& argumentValueMap)
{
    return argNames.Map<Iterable<VariantValue>> ([=] (const String& i) -> VariantValue { return argumentValueMap.LookupValue (i); });
}

Iterable<DataExchange::VariantValue> Server::VariantValue::PickOutNamedArguments (const Iterable<String>& argNames, const VariantValue& argumentValueMap)
{
    return PickOutNamedArguments (argNames, argumentValueMap.As<Mapping<String, VariantValue>> ());
}

/*
 ********************************************************************************
 ********* Server::VariantValue::ExtractArgumentsAsVariantValue *****************
 ********************************************************************************
 */
DataExchange::VariantValue Server::VariantValue::ExtractArgumentsAsVariantValue::FromRequestBody (Request& request)
{
    return ClientErrorException::TreatExceptionsAsClientError ([&] () { return request.GetBodyVariantValue (); });
}

DataExchange::VariantValue Server::VariantValue::ExtractArgumentsAsVariantValue::FromRequestURL (Request& request)
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

DataExchange::VariantValue Server::VariantValue::ExtractArgumentsAsVariantValue::FromRequest (Request& request)
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
            Throw (ClientErrorException{"Expected url and body to both be structured VariantValue type"sv});
        }
        Mapping<String, VariantValue> rr = requestBody.As<Mapping<String, VariantValue>> ();
        // merge - with url values taking precedence
        rr.AddAll (urlBody.As<Mapping<String, VariantValue>> ());
        return VariantValue{rr};
    });
}

/*
 ********************************************************************************
 ************ WebService::Server::VariantValue::OrderParamValues ****************
 ********************************************************************************
 */
Sequence<DataExchange::VariantValue> Server::VariantValue::OrderParamValues (const Iterable<String>&                            paramNames,
                                                                             const Mapping<String, DataExchange::VariantValue>& paramValues)
{
    Sequence<DataExchange::VariantValue> result;
    paramNames.Apply ([&] (const String& name) {
        if (auto o = paramValues.Lookup (name)) {
            result += DataExchange::VariantValue{*o};
        }
        else {
            result += DataExchange::VariantValue{};
        }
    });
    return result;
}

/*
 ********************************************************************************
 ***************** WebService::Server::VariantValue::WriteResponse **************
 ********************************************************************************
 */
void Server::VariantValue::WriteResponse (Response& response, const WebServiceMethodDescription& webServiceDescription, const Memory::BLOB& responseValue)
{
    if (webServiceDescription.fResponseType) {
        response.contentType = *webServiceDescription.fResponseType;
        response.write (responseValue);
    }
    else {
        WeakAssert (responseValue.empty ()); // if you returned a value you probably meant to have it written!
    }
}

void Server::VariantValue::WriteResponse (Response& response, const WebServiceMethodDescription& webServiceDescription, const VariantValue& responseValue)
{
    InternetMediaTypeRegistry registry = InternetMediaTypeRegistry::sThe;
    Require (not webServiceDescription.fResponseType.has_value () or
             (webServiceDescription.fResponseType == DataExchange::InternetMediaTypes::kJSON or
              webServiceDescription.fResponseType == DataExchange::InternetMediaTypes::kText_PLAIN)); // all we support for now
    if (webServiceDescription.fResponseType) {
        if (registry.IsA (InternetMediaTypes::kJSON, *webServiceDescription.fResponseType)) {
            response.contentType = *webServiceDescription.fResponseType;
            response.write (Variant::JSON::Writer{}.WriteAsBLOB (responseValue));
        }
        else if (registry.IsA (InternetMediaTypes::Wildcards::kText, *webServiceDescription.fResponseType)) {
            response.contentType = *webServiceDescription.fResponseType;
            response.write (Variant::JSON::Writer{}.WriteAsBLOB (responseValue));
        }
        else {
            RequireNotReached ();
        }
    }
    else {
        WeakAssert (responseValue == nullptr); // if you returned a value you probably meant to have it written!
    }
}
