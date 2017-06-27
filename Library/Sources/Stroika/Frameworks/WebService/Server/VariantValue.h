/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
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
 */

/*
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *      @todo get mkRequestHandler () overloads that take ARG_TYPE_1...N working with variadic templates. Got a FAILED start at that
 *            going in the .inl file, but I still need to read more about how to use variadic templates.
 */

namespace Stroika {
    namespace Frameworks {
        namespace WebService {
            namespace Server {
                namespace VariantValue {

                    using namespace Stroika::Foundation;

                    using Containers::Mapping;
                    using Containers::Sequence;
                    using DataExchange::InternetMediaType;
                    using DataExchange::VariantValue;
                    using IO::Network::URL;
                    using Memory::BLOB;
                    using Memory::Optional;
                    using Traversal::Iterable;

                    using WebServer::Request;
                    using WebServer::Response;

                    /**
                     * Convert URL query argumetns to a mapping of name to value pairs (so they can be mapped to objects)
                     */
                    Mapping<String, VariantValue> PickoutParamValuesFromURL (Request* request, const Optional<Iterable<String>>& namedParameters = {});
                    Mapping<String, VariantValue> PickoutParamValuesFromURL (const URL& url, const Optional<Iterable<String>>& namedParameters = {});

                    /**
                     * Convert body to a mapping of name to value pairs (so they can be mapped to objects)
                     */
                    Mapping<String, VariantValue> PickoutParamValuesBody (Request* request, const Optional<Iterable<String>>& namedParameters);
                    Mapping<String, VariantValue> PickoutParamValuesBody (const BLOB& body, const Optional<InternetMediaType>& bodyContentType, const Optional<Iterable<String>>& namedParameters);

                    /**
                     */
                    VariantValue GetWebServiceArgsAsVariantValue (Request* request, const Optional<String>& fromInMessage = {});

                    /**
                     &&& considering deprecating these &&&
                     */
                    Sequence<VariantValue> PickoutParamValues (const Iterable<String>& paramNames, const Mapping<String, VariantValue>& paramValues);
                    Sequence<VariantValue> PickoutParamValues (const Iterable<String>& paramNames, Request* request, const Optional<String>& fromInMessage = {});

                    //// NEW
                    /**
                     * Combine params from URL (@see PickoutParamValuesFromURL) and PickoutParamValuesBody - optionally restricting which params we grab from URL/body.
                     *
                     *  If parameters given in both places, use the BODY -provided value.
                     */
                    Mapping<String, VariantValue> PickoutParamValues_NEW (Request* request, const Optional<Iterable<String>>& namedURLParams = {}, const Optional<Iterable<String>>& namedBodyParams = {});

                    /**
                     *  \brief Apply the arguments in Mapping<String,VariantValue> in the order specified by paramNames, to function f, using objVariantMapper to transform them, and return the result
                     *
                     *  @todo - figure out how to rewrite this using variadic template
                     */
                    template <typename OUT_ARGS, typename ARG_TYPE_0>
                    OUT_ARGS ApplyArgs (const Mapping<String, VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<OUT_ARGS (ARG_TYPE_0)>& f);
                    template <typename OUT_ARGS, typename ARG_TYPE_0, typename ARG_TYPE_1>
                    OUT_ARGS ApplyArgs (const Mapping<String, VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<OUT_ARGS (ARG_TYPE_0, ARG_TYPE_1)>& f);
                    template <typename OUT_ARGS, typename ARG_TYPE_0, typename ARG_TYPE_1, typename ARG_TYPE_2>
                    OUT_ARGS ApplyArgs (const Mapping<String, VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<OUT_ARGS (ARG_TYPE_0, ARG_TYPE_1, ARG_TYPE_2)>& f);
                    template <typename OUT_ARGS, typename ARG_TYPE_0, typename ARG_TYPE_1, typename ARG_TYPE_2, typename ARG_TYPE_3>
                    OUT_ARGS ApplyArgs (const Mapping<String, VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<OUT_ARGS (ARG_TYPE_0, ARG_TYPE_1, ARG_TYPE_2, ARG_TYPE_3)>& f);

                    /**
                     */
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
                    template <typename OUT_ARGS, typename ARG_TYPE_0>
                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<OUT_ARGS (ARG_TYPE_0)>& f);
                    template <typename OUT_ARGS, typename ARG_TYPE_0, typename ARG_TYPE_1>
                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<OUT_ARGS (ARG_TYPE_0, ARG_TYPE_1)>& f);
                    template <typename OUT_ARGS, typename ARG_TYPE_0, typename ARG_TYPE_1, typename ARG_TYPE_2>
                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<OUT_ARGS (ARG_TYPE_0, ARG_TYPE_1, ARG_TYPE_2)>& f);
                    template <typename OUT_ARGS, typename ARG_TYPE_0, typename ARG_TYPE_1, typename ARG_TYPE_2, typename ARG_TYPE_3>
                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<OUT_ARGS (ARG_TYPE_0, ARG_TYPE_1, ARG_TYPE_2, ARG_TYPE_3)>& f);

#if 0
                    // @todo eventually find a way to make this owrk with JSON or XML in/ out and in can be GET query args (depending on WebServiceMethodDescription properties)
                    template <typename OUT_ARG, typename... IN_ARGS>
                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<OUT_ARG (IN_ARGS...)>& f);
                    template <typename OUT_ARG, typename... IN_ARGS>
                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<OUT_ARG (IN_ARGS...)>& f);
#endif
                    template <typename OUT_ARG>
                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<OUT_ARG (void)>& f);

                    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<BLOB (WebServer::Message* m)>& f);
                }
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "VariantValue.inl"

#endif /*_Stroika_Framework_WebService_Server_VariantValue_h_*/
