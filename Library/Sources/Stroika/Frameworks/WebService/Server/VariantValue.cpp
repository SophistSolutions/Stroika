/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../../Foundation/DataExchange/Variant/JSON/Reader.h"
#include "../../../Foundation/DataExchange/Variant/JSON/Writer.h"

#include "../../WebServer/ClientErrorException.h"

#include "Basic.h"

#include "VariantValue.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebService;
using namespace Stroika::Frameworks::WebService::Server;

using Characters::String_Constant;
using Characters::StringBuilder;
using WebServer::ClientErrorException;

/*
 ********************************************************************************
 ******** WebService::Server::VariantValue::PickoutParamValuesFromURL ***********
 ********************************************************************************
 */
Mapping<String, DataExchange::VariantValue> Server::VariantValue::PickoutParamValuesFromURL (Request* request, const Optional<Iterable<String>>& namedParameters)
{
    RequireNotNull (request);
    return PickoutParamValuesFromURL (request->GetURL (), namedParameters);
}

Mapping<String, DataExchange::VariantValue> Server::VariantValue::PickoutParamValuesFromURL (const URL& url, const Optional<Iterable<String>>& namedParameters)
{
    Mapping<String, VariantValue> result;
    Mapping<String, String>       unconverted = url.GetQuery ().GetMap ();
    if (namedParameters) {
        namedParameters->Apply ([&](const String& paramName) {
            if (auto o = unconverted.Lookup (paramName)) {
                result.Add (paramName, VariantValue{*o});
            }
        });
    }
    else {
        unconverted.Apply ([&](const KeyValuePair<String, String>& kvp) {
            result.Add (kvp.fKey, VariantValue{kvp.fValue});
        });
    }
    return result;
}

/*
 ********************************************************************************
 ******* WebService::Server::VariantValue::PickoutParamValuesFromBody ***********
 ********************************************************************************
 */
Mapping<String, DataExchange::VariantValue> Server::VariantValue::PickoutParamValuesFromBody (Request* request, const Optional<Iterable<String>>& namedParameters)
{
    Memory::BLOB inData = request->GetBody ();
    return PickoutParamValuesFromBody (request->GetBody (), request->GetContentType (), namedParameters);
}

Mapping<String, DataExchange::VariantValue> Server::VariantValue::PickoutParamValuesFromBody (const BLOB& body, const Optional<InternetMediaType>& bodyContentType, const Optional<Iterable<String>>& namedParameters)
{
    static const InternetMediaType kDefaultCT_ = DataExchange::PredefinedInternetMediaType::kJSON;
    if (bodyContentType.Value (kDefaultCT_) == DataExchange::PredefinedInternetMediaType::kJSON) {
        Mapping<String, DataExchange::VariantValue> tmp = Variant::JSON::Reader ().Read (body).As<Mapping<String, DataExchange::VariantValue>> ();
        if (namedParameters) {
            tmp.RetainAll (*namedParameters);
        }
        return tmp;
    }
    Execution::Throw (DataExchange::BadFormatException (String_Constant (L"Unrecognized content-type")));
}

/*
 ********************************************************************************
 ***** WebService::Server::VariantValue::GetWebServiceArgsAsVariantValue ********
 ********************************************************************************
 */
DataExchange::VariantValue Server::VariantValue::GetWebServiceArgsAsVariantValue (Request* request, const Optional<String>& fromInMessage)
{
    String method{request->GetHTTPMethod ()};
    if (method == L"POST" or method == L"PUT") {
        // Allow missing (content-size: 0) for args to method - and dont fail it as invalid json
        // @also - @todo - check ContentType ebfore reading as JSON!!!
        Memory::BLOB inData = request->GetBody ();
        return inData.empty () ? VariantValue{} : Variant::JSON::Reader ().Read (inData);
    }
    else if (method == L"GET") {
        IO::Network::URL url = request->GetURL ();
        // get query args
        // For now - only support String values of query-string args
        Mapping<String, VariantValue> result;
        url.GetQuery ().GetMap ().Apply ([&result](const KeyValuePair<String, String>& kvp) { result.Add (kvp.fKey, kvp.fValue); });
        return VariantValue{result};
    }
    else {
        Execution::Throw (ClientErrorException (
            String_Constant{L"Expected GET with query-string arguments or PUT or POST"} +
            (fromInMessage ? (L" from " + *fromInMessage) : L"")));
    }
}

/*
 ********************************************************************************
 ********** WebService::Server::VariantValue::PickoutParamValues ****************
 ********************************************************************************
 */
Mapping<String, DataExchange::VariantValue> Server::VariantValue::PickoutParamValues (Request* request, const Optional<Iterable<String>>& namedURLParams, const Optional<Iterable<String>>& namedBodyParams)
{
    Mapping<String, DataExchange::VariantValue> result = PickoutParamValuesFromURL (request, namedURLParams);
    // body params take precedence, if they overlap
    PickoutParamValuesFromBody (request, namedBodyParams).Apply ([&](auto i) { result.Add (i.fKey, i.fValue); });
    return result;
}

/*
 ********************************************************************************
 ************ WebService::Server::VariantValue::OrderParamValues ****************
 ********************************************************************************
 */
Sequence<DataExchange::VariantValue> Server::VariantValue::OrderParamValues (const Iterable<String>& paramNames, const Mapping<String, DataExchange::VariantValue>& paramValues)
{
    Sequence<DataExchange::VariantValue> vvs;
    for (auto i : paramNames) {
        vvs += paramValues.LookupValue (i);
    }
    return vvs;
}

/*
 ********************************************************************************
 ***************** WebService::Server::VariantValue::WriteResponse **************
 ********************************************************************************
 */
void Server::VariantValue::WriteResponse (Response* response, const WebServiceMethodDescription& webServiceDescription)
{
    // nothing todo to write empty (void) response
}

void Server::VariantValue::WriteResponse (Response* response, const WebServiceMethodDescription& webServiceDescription, const VariantValue& responseValue)
{
    Require (webServiceDescription.fResponseType == DataExchange::PredefinedInternetMediaType::kJSON ()); // all we support for now
    response->write (Variant::JSON::Writer ().WriteAsBLOB (responseValue));
    if (webServiceDescription.fResponseType) {
        response->SetContentType (*webServiceDescription.fResponseType);
    }
}

/*
 ********************************************************************************
 **************** WebService::Server::VariantValue::mkRequestHandler ************
 ********************************************************************************
 */
WebServer::RequestHandler Server::VariantValue::mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<Memory::BLOB (WebServer::Message* m)>& f)
{
    return [=](WebServer::Message* m) {
        ExpectedMethod (*m->PeekRequest (), webServiceDescription);
        if (webServiceDescription.fResponseType) {
            m->PeekResponse ()->SetContentType (*webServiceDescription.fResponseType);
        }
        m->PeekResponse ()->write (f (m));
    };
}
