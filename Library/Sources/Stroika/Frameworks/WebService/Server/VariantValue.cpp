/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../../Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "../../../Foundation/DataExchange/Variant/JSON/Reader.h"
#include "../../../Foundation/DataExchange/Variant/JSON/Writer.h"
#include "../../../Foundation/IO/Network/HTTP/ClientErrorException.h"

#include "Basic.h"

#include "VariantValue.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;

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
    using namespace Characters;
    static const InternetMediaType kDefaultCT_ = DataExchange::InternetMediaTypes::kJSON;
    if (bodyContentType.value_or (kDefaultCT_) == DataExchange::InternetMediaTypes::kJSON) {
        return body.empty () ? Mapping<String, DataExchange::VariantValue>{} : ClientErrorException::TreatExceptionsAsClientError ([&] () {
            return Variant::JSON::Reader ().Read (body).As<Mapping<String, DataExchange::VariantValue>> ();
        });
    }
    Execution::Throw (ClientErrorException{"Unrecognized content-type"sv});
}

/*
 ********************************************************************************
 *** WebService::Server::VariantValue::CombineWebServiceArgsAsVariantValue ******
 ********************************************************************************
 */
DataExchange::VariantValue Server::VariantValue::CombineWebServiceArgsAsVariantValue (Request* request)
{
    RequireNotNull (request);
    return ClientErrorException::TreatExceptionsAsClientError ([&] () {
        Mapping<String, DataExchange::VariantValue> result;
        {
            Memory::BLOB inData = request->GetBody ();
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
 ********** WebService::Server::VariantValue::PickoutParamValues ****************
 ********************************************************************************
 */
Mapping<String, DataExchange::VariantValue> Server::VariantValue::PickoutParamValues (Request* request)
{
    Mapping<String, DataExchange::VariantValue> result = PickoutParamValuesFromURL (request);
    // body params take precedence, if they overlap
    PickoutParamValuesFromBody (request).Apply ([&] (auto i) { result.Add (i.fKey, i.fValue); });
    return result;
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
void Server::VariantValue::WriteResponse (Response* response, const WebServiceMethodDescription& webServiceDescription, const Memory::BLOB& responseValue)
{
    if (webServiceDescription.fResponseType) {
        response->contentType = *webServiceDescription.fResponseType;
        response->write (responseValue);
    }
    else {
        WeakAssert (responseValue.empty ()); // if you returned a value you probably meant to have it written!
    }
}

void Server::VariantValue::WriteResponse (Response* response, const WebServiceMethodDescription& webServiceDescription, const VariantValue& responseValue)
{
    Require (not webServiceDescription.fResponseType.has_value () or
             (webServiceDescription.fResponseType == DataExchange::InternetMediaTypes::kJSON or
              webServiceDescription.fResponseType == DataExchange::InternetMediaTypes::kText_PLAIN)); // all we support for now
    if (webServiceDescription.fResponseType) {
        if (webServiceDescription.fResponseType == DataExchange::InternetMediaTypes::kJSON) {
            response->contentType = *webServiceDescription.fResponseType;
            response->write (Variant::JSON::Writer{}.WriteAsBLOB (responseValue));
        }
        else if (webServiceDescription.fResponseType == DataExchange::InternetMediaTypes::kText_PLAIN) {
            response->contentType = *webServiceDescription.fResponseType;
            response->write (Variant::JSON::Writer{}.WriteAsBLOB (responseValue));
        }
        else {
            RequireNotReached ();
        }
    }
    else {
        WeakAssert (responseValue == nullptr); // if you returned a value you probably meant to have it written!
    }
}

/*
 ********************************************************************************
 **************** WebService::Server::VariantValue::mkRequestHandler ************
 ********************************************************************************
 */
WebServer::RequestHandler Server::VariantValue::mkRequestHandler (const WebServiceMethodDescription& webServiceDescription,
                                                                  const function<Memory::BLOB (WebServer::Message* m)>& f)
{
    return [=] (WebServer::Message* m) {
        RequireNotNull (m);
        ExpectedMethod (m->request (), webServiceDescription);
        if (webServiceDescription.fResponseType) {
            m->rwResponse ().contentType = *webServiceDescription.fResponseType;
        }
        m->rwResponse ().write (f (m));
    };
}
