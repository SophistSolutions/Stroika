/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Framework_WebService_Server_VariantValue_h_
#define _Stroika_Framework_WebService_Server_VariantValue_h_ 1

#include "../../StroikaPreComp.h"

#include "../../../Foundation/Containers/Mapping.h"
#include "../../../Foundation/Containers/Sequence.h"
#include "../../../Foundation/DataExchange/InternetMediaType.h"
#include "../../../Foundation/DataExchange/ObjectVariantMapper.h"
#include "../../../Foundation/DataExchange/VariantValue.h"
#include "../../../Foundation/IO/Network/URL.h"

#include "../../WebServer/Request.h"
#include "../../WebServer/RequestHandler.h"
#include "../../WebServer/Response.h"

#include "../Basic.h"

/*
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *      @todo get mkRequestHandler () overloads that take ARG_TYPE_1...N working with variadic templates. Got a FAILED start at that
 *            going in the .inl file, but I still need to read more about how to use variadic templates.
 */

namespace Stroika::Frameworks::WebService::Server::VariantValue {

    using namespace Stroika::Foundation;

    using Containers::Mapping;
    using Containers::Sequence;
    using DataExchange::InternetMediaType;
    using DataExchange::VariantValue;
    using IO::Network::URL;
    using Memory::BLOB;
    using Traversal::Iterable;

    using WebServer::Request;
    using WebServer::Response;

    /**
     * Convert URL query arguments to a mapping of name to value pairs (so they can be mapped to objects)
     *
     *  \par Example Usage
     *      \code
     *      \endcode
     *
     *  @see PickoutParamValuesFromBody () to just pickout params from Body
     *  @see PickoutParamValues () to pickout params from both url arg and body
     */
    Mapping<String, VariantValue> PickoutParamValuesFromURL (Request* request);
    Mapping<String, VariantValue> PickoutParamValuesFromURL (const URL& url);

    /**
     * Convert body to a mapping of name to value pairs (so they can be mapped to objects)
     *
     *  @see PickoutParamValuesFromURL () to just pickout params from URL
     *  @see PickoutParamValues () to pickout params from both url arg and body
     */
    Mapping<String, VariantValue> PickoutParamValuesFromBody (Request* request);
    Mapping<String, VariantValue> PickoutParamValuesFromBody (const BLOB& body, const optional<InternetMediaType>& bodyContentType);

    /**
     * Combine params from URL (@see PickoutParamValuesFromURL) and PickoutParamValuesFromBody - optionally restricting which params we grab from URL/body.
     *
     *  If parameters given in both places, use the BODY -provided value.
     *
     *  @see PickoutParamValuesFromURL () to just pickout params from URL
     *  @see PickoutParamValuesFromBody () to just pickout params from Body
     *  @see OrderParamValues () to produce a Sequence<VariantValue> {} from the mapping.
     */
    Mapping<String, VariantValue> PickoutParamValues (Request* request);

    /**
     */
    VariantValue GetWebServiceArgsAsVariantValue (Request* request, const optional<String>& fromInMessage = {});

    /**
     *  Take the ordered list of param names, and produce an ordered list of variant values (with the same ordering).
     *  This is useful if you know the order of named parameters, and just want to pass them as ordered parameters.
     *
     *  \par Example Usage
     *      \code
     *          Sequence<VariantValue> tmp = OrderParamValues ( Iterable<String>{L"page", L"xxx"}, PickoutParamValuesFromURL (URL (L"http://www.sophist.com?page=5", URL::eFlexiblyAsUI)));
     *          Assert (tmp.size () == 2);
     *          Assert (tmp[0] == 5);
     *          Assert (tmp[1] == nullptr);
     *      \endcode
     */
    Sequence<VariantValue> OrderParamValues (const Iterable<String>& paramNames, const Mapping<String, VariantValue>& paramValues);
    Sequence<VariantValue> OrderParamValues (const Iterable<String>& paramNames, Request* request);

    /**
     *  \brief Apply the arguments in Mapping<String,VariantValue> in the order specified by paramNames, to function f, using objVariantMapper to transform them, and return the result
     *
     *  @todo - figure out how to rewrite this using variadic template
     */
    template <typename RETURN_TYPE>
    RETURN_TYPE ApplyArgs (const Mapping<String, VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (void)>& f);
    template <typename RETURN_TYPE, typename ARG_TYPE_0>
    RETURN_TYPE ApplyArgs (const Mapping<String, VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (ARG_TYPE_0)>& f);
    template <typename RETURN_TYPE, typename ARG_TYPE_0, typename ARG_TYPE_1>
    RETURN_TYPE ApplyArgs (const Mapping<String, VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (ARG_TYPE_0, ARG_TYPE_1)>& f);
    template <typename RETURN_TYPE, typename ARG_TYPE_0, typename ARG_TYPE_1, typename ARG_TYPE_2>
    RETURN_TYPE ApplyArgs (const Mapping<String, VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (ARG_TYPE_0, ARG_TYPE_1, ARG_TYPE_2)>& f);
    template <typename RETURN_TYPE, typename ARG_TYPE_0, typename ARG_TYPE_1, typename ARG_TYPE_2, typename ARG_TYPE_3>
    RETURN_TYPE ApplyArgs (const Mapping<String, VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (ARG_TYPE_0, ARG_TYPE_1, ARG_TYPE_2, ARG_TYPE_3)>& f);

    /**
     */
    template <typename RETURN_TYPE, typename... IN_ARGS>
    void CallFAndWriteConvertedResponse (Response* response, const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<RETURN_TYPE (IN_ARGS...)>& f, IN_ARGS... inArgs);

    /**
     */
    void WriteResponse (Response* response, const WebServiceMethodDescription& webServiceDescription);
    void WriteResponse (Response* response, const WebServiceMethodDescription& webServiceDescription, const VariantValue& responseValue);

    /**
     *  The variants of mkRequestHandler () which take paramNames will eventually be replaced with variadic templates. So the calls should remain
     *  unchanged, but the exact parameters passed to the template instantiation will.
     *
     *  Each named parameter (in paramNames) must correspond to the JSON param arg to the funtion and must correspond exactly in type and
     *  order to the parameters of the function.
     *
     *  \par Example Usage
     *      \code
     *           Route{
     *              RegularExpression{L"plus"},
     *              mkRequestHandler (
     *                  kPlusWSDescriptor_,
     *                  kObjectVariantMapper,
     *                  Sequence<String>{L"lhs", L"rhs"},
     *                  function<float (float, float)>{[=](float lhs, float rhs) { return lhs + rhs }})},
     *      \endcode
     *
     *  The overload with f (void) as argument, takes no arguments (and so omits paramNames), and just returns the given result.
     *
     *  The overload with no 'f' just takes a simple message handler (and wraps a bit of WebServiceMethodDescription checking around its call).
     *
     *
     *  @todo REWRITE USING PickoutParamValues, and ApplyArgs (allowing intermeidate varsions 
     */
    template <typename RETURN_TYPE, typename ARG_TYPE_COMBINED>
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<RETURN_TYPE (ARG_TYPE_COMBINED)>& f);
    template <typename RETURN_TYPE, typename ARG_TYPE_0>
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (ARG_TYPE_0)>& f);
    template <typename RETURN_TYPE, typename ARG_TYPE_0, typename ARG_TYPE_1>
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (ARG_TYPE_0, ARG_TYPE_1)>& f);
    template <typename RETURN_TYPE, typename ARG_TYPE_0, typename ARG_TYPE_1, typename ARG_TYPE_2>
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (ARG_TYPE_0, ARG_TYPE_1, ARG_TYPE_2)>& f);
    template <typename RETURN_TYPE, typename ARG_TYPE_0, typename ARG_TYPE_1, typename ARG_TYPE_2, typename ARG_TYPE_3>
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (ARG_TYPE_0, ARG_TYPE_1, ARG_TYPE_2, ARG_TYPE_3)>& f);

#if 0
    // @todo eventually find a way to make this owrk with JSON or XML in/ out and in can be GET query args (depending on WebServiceMethodDescription properties)
    template <typename RETURN_TYPE, typename... IN_ARGS>
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<RETURN_TYPE (IN_ARGS...)>& f);
    template <typename RETURN_TYPE, typename... IN_ARGS>
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (IN_ARGS...)>& f);
#endif
    template <typename RETURN_TYPE>
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<RETURN_TYPE (void)>& f);

    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<BLOB (WebServer::Message* m)>& f);
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "VariantValue.inl"

#endif /*_Stroika_Framework_WebService_Server_VariantValue_h_*/
