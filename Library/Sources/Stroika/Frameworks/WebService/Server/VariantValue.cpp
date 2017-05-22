/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../../Foundation/DataExchange/Variant/JSON/Reader.h"
#include "../../../Foundation/DataExchange/Variant/JSON/Writer.h"

#include "Basic.h"

#include "VariantValue.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebService;
using namespace Stroika::Frameworks::WebService::Server;
using namespace Stroika::Frameworks::WebService::Server;

using Characters::StringBuilder;
using Characters::String_Constant;
using DataExchange::VariantValue;

/*
 ********************************************************************************
 **** WebService::Server::VariantValue::GetWebServiceArgsAsVariantValue *********
 ********************************************************************************
 */
DataExchange::VariantValue Server::VariantValue::GetWebServiceArgsAsVariantValue (Request* request, const Optional<String>& fromInMessage)
{
    String method{request->GetHTTPMethod ()};
    if (method == L"POST") {
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
        Execution::Throw (Execution::StringException (
            String_Constant{L"Expected GET with query-string arguments or POST"} +
            (fromInMessage ? (L" from " + *fromInMessage) : L"")));
    }
}

/*
 ********************************************************************************
 ********** WebService::Server::VariantValue::PickoutParamValues ****************
 ********************************************************************************
 */
Sequence<DataExchange::VariantValue> Server::VariantValue::PickoutParamValues (const Iterable<String>& paramNames, const Mapping<String, DataExchange::VariantValue>& paramValues)
{
    Sequence<DataExchange::VariantValue> vvs;
    for (auto i : paramNames) {
        vvs += paramValues.LookupValue (i);
    }
    return vvs;
}

Sequence<DataExchange::VariantValue> Server::VariantValue::PickoutParamValues (const Iterable<String>& paramNames, Request* request, const Optional<String>& fromInMessage)
{
    return PickoutParamValues (paramNames, GetWebServiceArgsAsVariantValue (request, fromInMessage).As<Mapping<String, DataExchange::VariantValue>> ());
}

/*
 ********************************************************************************
 ***************** WebService::Server::VariantValue::WriteResponse **************
 ********************************************************************************
 */
void Server::VariantValue::WriteResponse (Response* response, const WebServiceMethodDescription& webServiceDescription, const VariantValue& responseValue)
{
    Require (webServiceDescription.fResponseType == DataExchange::PredefinedInternetMediaType::JSON_CT ()); // all we support for now
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
        ExpectedMethod (m->PeekRequest (), webServiceDescription);
        if (webServiceDescription.fResponseType) {
            m->PeekResponse ()->SetContentType (*webServiceDescription.fResponseType);
        }
        m->PeekResponse ()->write (f (m));
    };
}
