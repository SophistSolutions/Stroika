/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_WebService_Server_VariantValue_h_
#define _Stroika_Framework_WebService_Server_VariantValue_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/DataExchange/InternetMediaType.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/DataExchange/VariantValue.h"
#include "Stroika/Foundation/IO/Network/URI.h"

#include "Stroika/Frameworks/WebServer/Request.h"
#include "Stroika/Frameworks/WebServer/RequestHandler.h"
#include "Stroika/Frameworks/WebServer/Response.h"

#include "Stroika/Frameworks/WebService/Server/Basic.h"

/*
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Frameworks::WebService::Server::VariantValue {

    using namespace Stroika::Foundation;

    using Containers::Mapping;
    using Containers::Sequence;
    using DataExchange::InternetMediaType;
    using DataExchange::VariantValue;
    using IO::Network::URI;
    using Memory::BLOB;
    using Traversal::Iterable;

    using WebServer::Request;
    using WebServer::Response;

    /**
     * Convert URL query arguments to a mapping of name to value pairs (so they can be mapped to objects)
     *
     *  \par Example Usage
     *      \code
     *         static const String                         kValueParamName_ = "value"sv;
     *         Mapping<String, DataExchange::VariantValue> args             = PickoutParamValuesFromURL (m->request ());
     *         number                                                       = Model::kMapper.ToObject<Number> (args.LookupValue (kValueParamName_));
     *      \endcode
     *
     *  @see PickoutParamValuesFromBody () to just pickout params from Body
     *  @see PickoutParamValues () to pickout params from both url arg and body
     *
     *  \note - PickoutParamValuesFromURL maps exceptions returned to IO::Network::HTTP::ClientErrorException
     */
    Mapping<String, VariantValue> PickoutParamValuesFromURL (const Request* request);
    Mapping<String, VariantValue> PickoutParamValuesFromURL (const URI& url);

    /**
     * Convert body to a mapping of name to value pairs (so they can be mapped to objects)
     * 
     *  This API presumes its argument is a 'request' (or BLOB with the body), which consists of a javascript object (Mapping<String,VariantValue>)
     *  where the key in the mapping picks out parameter names, and the values in the mapping are the values of the parameters.
     * 
     *  This is a VERY common case.
     *
     *  \par Example Usage
     *      \code
     *         Mapping<String, DataExchange::VariantValue> args             = PickoutParamValuesFromBody (m->rwRequest ());
     *         number                                                       = Model::kMapper.ToObject<Number> (args.LookupValue ("value"sv));
     *      \endcode
     *
     *  Supported BODY formats:
     *      o   DataExchange::InternetMediaTypes::kJSON
     *      o   application/x-www-form-urlencoded           *** @todo NOT YET IMPLEMENTED - BUT SHOULD BE SUPPORTED ***
     *
     *  @see PickoutParamValuesFromURL () to just pick out params from URL
     *  @see PickoutParamValues () to pickout params from both url arg and body
     *
     *  \note - PickoutParamValuesFromBody map exceptions returned to IO::Network::HTTP::ClientErrorException
     */
    Mapping<String, VariantValue> PickoutParamValuesFromBody (Request* request);
    Mapping<String, VariantValue> PickoutParamValuesFromBody (Request& request);
    Mapping<String, VariantValue> PickoutParamValuesFromBody (const BLOB& body, const optional<InternetMediaType>& bodyContentType);

    /**
     * Combine params from URL (@see PickoutParamValuesFromURL) and PickoutParamValuesFromBody - optionally restricting which params we grab from URL/body.
     *
     *  @todo FIX to be smarter about if to check body/url, and how to combine. 
     *        THIS WILL CHANGE TO BE SMATER - bUT NOT SURE HOW YET.
     *  If parameters given in both places, use the BODY -provided value.
     *
     *  \par Example Usage
     *      \code
     *         static const String                         kValueParamName_ = "value"sv;
     *         Mapping<String, DataExchange::VariantValue> args             = PickoutParamValues (m->rwRequest ());
     *         number                                                       = Model::kMapper.ToObject<Number> (args.LookupValue (kValueParamName_));
     *      \endcode
     *
     *  @see PickoutParamValuesFromURL () to just pickout params from URL
     *  @see PickoutParamValuesFromBody () to just pickout params from Body
     *  @see OrderParamValues () to produce a Sequence<VariantValue> {} from the mapping.
     *
     *  \note - PickoutParamValues maps exceptions returned to IO::Network::HTTP::ClientErrorException
     */
    Mapping<String, VariantValue> PickoutParamValues (Request* request);

    /**
      * Take the Body of the request, and if its missing, or an object, add in any query parameters (overriding body values) in the resulting object.
      *
      * \note this CAN return an array or simple object like string from the body, but if that is present, then the URL arguments will be ignored.
      *
      * \note older GetWebServiceArgsAsVariantValue () used to look at METHOD, and just grab BODY from PUT/POST and just grab url args from GET. Not sure
      *       if that was better (changed in Stroika v2.1d13).
      *
      * \note could parameterize some of these choices? But pretty easy to just explicitly choose yourself in your own route handler.
      *
      * \note THIS IS RARELY used - but just if you want to have a single mapper that all your arguments and converts them to a single object.
      */
    VariantValue CombineWebServiceArgsAsVariantValue (Request* request);

    /**
     *  Take the ordered list of param names, and produce an ordered list of variant values (with the same ordering).
     *  This is useful if you know the order of named parameters, and just want to pass them as ordered parameters.
     *
     *  \par Example Usage
     *      \code
     *          Sequence<VariantValue> tmp = OrderParamValues ( Iterable<String>{"page", "xxx"}, PickoutParamValuesFromURL (URI {"http://www.sophist.com?page=5"}));
     *          Assert (tmp.size () == 2);
     *          Assert (tmp[0] == 5);
     *          Assert (tmp[1] == nullptr);
     *      \endcode
     *
     */
    Sequence<VariantValue> OrderParamValues (const Iterable<String>& paramNames, const Mapping<String, VariantValue>& paramValues);
    Sequence<VariantValue> OrderParamValues (const Iterable<String>& paramNames, Request* request);

    /**
     *  \brief Apply the arguments in Sequence<VariantValue> or Mapping<String,VariantValue> in the order specified by paramNames, to function f, using objVariantMapper to transform them, and return the result
     *
     *  \note we only use the overload taking Sequence<VariantValue>, and MAY want to lose the Mapping<> overload.
     */
    template <typename RETURN_TYPE, typename... ARG_TYPES>
    VariantValue ApplyArgs (const Sequence<VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper,
                            const function<RETURN_TYPE (ARG_TYPES...)>& f);
    template <typename RETURN_TYPE, typename... ARG_TYPES>
    VariantValue ApplyArgs (const Mapping<String, VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper,
                            const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (ARG_TYPES...)>& f);

    /**
     *  Send the argument value as a web-service response. If no argument (response value) response is empty. If resposne is a VariantValue,
     *  its written as the format in the webServiceDescription.fResponseType.
     *  IF fResponseType !has_value, then no respose is written.
     *
     *  \note   Supported Response Types (For the VariantValue response type overload)
     *          o  nullopt (no response written)
     *          o  DataExchange::InternetMediaTypes::JSON_CT
     *          o  DataExchange::InternetMediaTypes::kText_PLAIN
     */
    void WriteResponse (Response* response, const WebServiceMethodDescription& webServiceDescription);
    void WriteResponse (Response* response, const WebServiceMethodDescription& webServiceDescription, const Memory::BLOB& responseValue);
    void WriteResponse (Response* response, const WebServiceMethodDescription& webServiceDescription, const VariantValue& responseValue);

    /**
     *  \brief mkRequestHandler () is a series of overloaded helpers that first call ExpectedMethod to validate and then the argument function 'f' and then use the objMapper to format/return the result.
     *
     *  All the overloads of mkRequestHandler () take as the first argument a WebServiceMethodDescription, used to validate.
     *
     *  All (except the 'const function<BLOB (WebServer::Message* m)>& f' overload) take an ObjectVariantMapper used to map the arguments (if any) and results.
     *
     *  In the variadic overload with 'paramNames', each named parameter must correspond to the JSON param arg to the funtion and must correspond exactly in type and
     *  order to the parameters of the function. They will be automatically extracted from the URL params or body (with @see OrderParamValues)
     *
     *  @see WriteResponse () for supported webServiceDescription.fResponseType values.
     *
     *  \par Example Usage
     *      \code
     *           Route{
     *              RegularExpression{"plus"},
     *              mkRequestHandler (
     *                  kPlusWSDescriptor_,
     *                  kObjectVariantMapper,
     *                  Sequence<String>{"lhs", "rhs"},
     *                  function<float (float, float)>{[=](float lhs, float rhs) { return lhs + rhs }})},
     *      \endcode
     *
     *  The overload with f (void) as argument, takes no arguments (and so omits paramNames), and just returns the given result.
     */
    template <typename RETURN_TYPE, typename ARG_TYPE_COMBINED>
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription&               webServiceDescription,
                                                const DataExchange::ObjectVariantMapper&         objVarMapper,
                                                const function<RETURN_TYPE (ARG_TYPE_COMBINED)>& f);
    template <typename RETURN_TYPE, typename... IN_ARGS>
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription&       webServiceDescription,
                                                const DataExchange::ObjectVariantMapper& objVarMapper,
                                                const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (IN_ARGS...)>& f);
    template <typename RETURN_TYPE>
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription,
                                                const DataExchange::ObjectVariantMapper& objVarMapper, const function<RETURN_TYPE (void)>& f);
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription&            webServiceDescription,
                                                const function<BLOB (WebServer::Message* m)>& f);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "VariantValue.inl"

#endif /*_Stroika_Framework_WebService_Server_VariantValue_h_*/
