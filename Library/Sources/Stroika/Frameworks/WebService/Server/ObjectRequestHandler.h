/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_WebService_Server_ObjectRequestHandler_h_
#define _Stroika_Framework_WebService_Server_ObjectRequestHandler_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <tuple>

#include "Stroika/Foundation/Common/Concepts.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/DataExchange/InternetMediaType.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"

#include "Stroika/Frameworks/WebServer/RequestHandler.h"

#include "Stroika/Frameworks/WebService/Server/Basic.h"

/*
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 * 
 * 
 *  @todo PROBABLY will replace mkRequestHandler
 */

namespace Stroika::Frameworks::WebService::Server::ObjectRequestHandler {

    using namespace Stroika::Foundation;

    using Characters::String;
    using Common::Case;
    using Common::FunctionTraits;
    using Common::Select_t;
    using Containers::Sequence;
    using DataExchange::InternetMediaType;
    using DataExchange::ObjectVariantMapper;
    using DataExchange::VariantValue;
    using IO::Network::URI;
    using Memory::BLOB;
    using Traversal::Iterable;

    using WebServer::Request;
    using WebServer::Response;

    /**
     *  \brief map a list of argument names, and a Mapping<String,VariantValue> (named arguments list), to a Sequence<VariantValue> - argument values.
     *  for overload with VariantValue argumentValueMap - throw if not GetType() == VariantValue::eMap (or null) - for no arguments.
     * 
     *  Sometimes callers will wish to treat the Body (or/possibly plus query url args) as a single object, and sometimes
     *  as multiple named parameters. This function serves that later scenario.
     */
    Iterable<VariantValue> PickOutNamedArguments (const Iterable<String>& argNames, const Mapping<String, VariantValue>& argumentValueMap);
    Iterable<VariantValue> PickOutNamedArguments (const Iterable<String>& argNames, const VariantValue& argumentValueMap);

    /**
     *   \note data (like request) etc only valid until end of call - don't copy/save
     */
    struct Context {
        /**
         *  \note - the size of the fMatchedURLArgs is a function of the regexp matched in the Route
         *          rule, and has nothing todo with the data in the request (the value depends on the
         *          data but not the size). So you can index fMatchedURLArgs[2] - and have it checked
         *          with assertions safely.
         */
        const Sequence<String>& fMatchedURLArgs;
        Request&                fRequest;
        Response&               fResponse;

        /**
         *  \note since Context is not copyable, you must explicitly call .ToString() on it to use it with _f strings (std::format).
         */
        String ToString () const;
    };
    static_assert (not copyable<Context>);
    static_assert (not movable<Context>);

    /**
     * values returned typically Mapping<String,VariantValue> - but can be other - often also null-value
     * 
     * \note - each of these - if they throw - they throw a subtype of ClientErrorException
     */
    struct ExtractArgumentsAsVariantValue {
        /**
         *  Looks at request content type, and tries to convert body data accordingly. If no content in body - OK
         *  returns empty VariantValue. If its text/plain - OK - returns a string. Otherwise if some sort of json, it parses
         *  it and returns it as VariantValue. Similarly for future types (xml etc).
         * 
         *  Failure to parse incoming data will result in exception being thrown, but always ClientErrorException
         */
        static VariantValue FromRequestBody (Request& request);

        /**
         *  \brief - extracts Query args from request url into a Mapping<String,String> (converted to VariantValue), or empty variant-value if no query args
         * 
         *  Any kind of failure will produce ClientErrorException
         */
        static VariantValue FromRequestURL (Request& request);

        /**
         *  First invoke FromRequestBody, and then FromRequestURL. Combine their results. If either null, return the other.
         *  If both non-null, both must of of type Mapping<String,VariantValue> - and then query-arguments take precedence.
         * 
         *  Any format or other errors, results in ClientErrorException
         */
        static VariantValue FromRequest (Request& request);
    };

    /**
     *  \brief Options for ObjectRequestHandler - mostly the ObjectVariantMapper, but also a few others depending on situation
     */
    struct Options {

        /**
         */
        ObjectVariantMapper fObjectMapper;

        /**
         * This is the default media type for the content type of the result message. If missing, it will be inferred based on data type produced.
         * regardless, it maybe overriden based on (eventually) http accept headers.
         */
        optional<InternetMediaType> fDefaultResultMediaType;

        /**
         */
        function<VariantValue (Request&)> fExtractVariantValueFromRequest{ExtractArgumentsAsVariantValue::FromRequestBody};

        /** 
         *  Sometimes you will want to treat the body as the sole input object for a webservice call. Sometimes
         * best to treat it as an array of parameters. If treated as an array of parameters (possible from mix of sources with fExtra... above)
         * then need their names and ordering to map to the arguments to the callback function.
         * 
         *  \see PickOutNamedArguments
         */
        optional<Iterable<String>> fTreatBodyAsListOfArguments;

        /**
         */
        nonvirtual String ToString () const;
    };
    static_assert (copyable<Options>);

    /**
     *  \brief Build Frameworks::WebServer::RequestHandler out of ObjectVariantMapper, a few options/clues, and a object-based Route callback function
     * 
     *  \par Example Usage
     *      \code
     *          Route{"api/objs/?"_RegEx,
     *                  ObjectRequestHandler::Factory{
     *                      {kMapper},
     *                      [] () -> Sequence<GUID> {
     *                          return Sequence<GUID>{};
     *                      }}}
     *      \endcode
     * 
     *  \par Example Usage
     *      \code
     *          Route{"api/(v1/)?recordings/(.+)"_RegEx,
     *                 ObjectRequestHandler::Factory{{kMapper}, [this] (const ObjectRequestHandler::Context& c) -> Recording {
     *                     String id = c.fMatchedURLArgs[1];
     *                     return fWSImpl_->recordings_GET (id);
     *                 }}}
     *      \endcode
     * 
     *  \brief ObjectRequestHandler::Factory is a way to construct a WebServer::RequestHandler from an ObjectVariantMapper object and a lambda taking in/out params of objects.
     * 
     *  \todo check acceptsContentType and return result as JSON, binary json, or xml (etc) accordingly - take OPTIONS param saying default
     */
    template <typename RETURN_TYPE, typename... ARG_TYPES>
    class Factory {
    public:
        static_assert (not is_reference_v<RETURN_TYPE>);
        //static_assert (conjunction<(not is_reference_v<ARG_TYPES>) && ...>, "");  // todo something close to this

    public:
        /**
         *  \brief Build Frameworks::WebServer::RequestHandler out of ObjectVariantMapper, a few options/clues, and a object-based Route callback function
         */
        template <typename /*invocable<ARG_TYPES...>*/ CALLBACK_FUNCTION>
        Factory (const Options& options, CALLBACK_FUNCTION&& highLevelHandler);

    public:
        /**
         *  This is the whole point of this class - to produce a RequestHandler that can be used in a Stroika WebServer Route.
         */
        nonvirtual operator Frameworks::WebServer::RequestHandler () const;

    public:
        /**
         *  This is 1/2 the guts of the RequestHandler - taking the request calling the handler with it, and producing
         *  the 'RESULT_TYPE' object.
         * 
         *  Note this is broken out as a callable method so it can be used from a straight custom WebServer::RequestHandler
         *  and just parts of the functionality used.
         * 
         *  \par Example Usage
         *      \code
         *          , Route{IO::Network::HTTP::MethodsRegEx::kPost, "api/(v1/)?recordings/?"_RegEx,
         *                [this] (Message* m) {
         *                    // use ObjectRequestHandler::Factory indirectly so can support POST raw data and arguments as query-args!
         *                    ObjectRequestHandler::Factory f{{kMapper}, [this] (const Recording& r) { return fWSImpl_->recordings_POST (r); }};
         *                    Recording                     arg = [&] () {
         *                        InternetMediaType requestCt =
         *                            Memory::ValueOfOrThrow (m->request ().contentType (), ClientErrorException{"missing request content type"sv});
         *                        auto ctChecker = InternetMediaTypeRegistry::sThe.load ();
         *                        if (ctChecker.IsA (InternetMediaTypes::kJSON, requestCt)) {
         *                            return Recording{kMapper.ToObject<Recording> (m->rwRequest ().GetBodyVariantValue ())};
         *                        }
         *                        else if (ctChecker.IsA (InternetMediaTypes::kAudio, requestCt)) {
         *                            auto r = Recording{.fData = make_tuple (requestCt, m->rwRequest ().GetBody ())};
         *                            // also can grab some parameters, like user, etc from query args - @todo
         *                            return r;
         *                        }
         *                        else {
         *                            Throw (ClientErrorException{"unsupported request content type"sv});
         *                        }
         *                    }();
         *                    auto rr = f.ApplyHandler (arg);
         *                    f.SendResponse (m->request (), m->rwResponse (), rr);
         *                }},
         *      \endcode
         */
        nonvirtual RETURN_TYPE ApplyHandler (const Context& context) const;

    public:
        /**
         */
        nonvirtual RETURN_TYPE ApplyObjectHandler (ARG_TYPES... args) const;

    public:
        /**
         */
        template <typename T>
        nonvirtual T ConvertArg2Object (const VariantValue& v) const;

    private:
        // use tuple_cat to put all the args together (but in a tuple) and then apply on the function to expand the args to call f
        template <typename RET = RETURN_TYPE>
        nonvirtual tuple<> mkArgsTuple_ (const Context& context, const Iterable<VariantValue>& variantValueArgs,
                                         [[maybe_unused]] const function<RET ()>& f) const;
        template <typename SINGLE_ARG>
        nonvirtual tuple<SINGLE_ARG> mkArgsTuple_ (const Context& context, const Iterable<VariantValue>& variantValueArgs,
                                                   [[maybe_unused]] const function<RETURN_TYPE (SINGLE_ARG)>& f) const;
        template <typename ARG_FIRST, typename... REST_ARG_TYPES>
        nonvirtual auto mkArgsTuple_ (const Context& context, const Iterable<VariantValue>& variantValueArgs,
                                      [[maybe_unused]] const function<RETURN_TYPE (ARG_FIRST, REST_ARG_TYPES...)>& f) const
            -> decltype (tuple_cat (declval<remove_cvref_t<ARG_FIRST>> (), declval<REST_ARG_TYPES...> ()));

    public:
        /**
         *  Given the packaged up response 'r' - send it as a result, in the appropriate format (based on request headers etc)
         */
        template <same_as<RETURN_TYPE> RT>
        nonvirtual void SendResponse (const Request& request, Response& response, const RT& r) const;
        nonvirtual void SendResponse (const Request& request, Response& response) const
            requires (same_as<RETURN_TYPE, void>);

    private:
        function<RETURN_TYPE (ARG_TYPES...)> fHighLevelHandler_;
        Options                              fOptions_;
    };

    // hopefully adequate approach for now, but there must be some way to generalize this - perhaps with folds?
    // --LGP 2024-11-10
    template <typename CALLBACK_FUNCTION>
    Factory (const Options&, CALLBACK_FUNCTION&&) -> Factory<invoke_result_t<CALLBACK_FUNCTION>>;
    template <typename CALLBACK_FUNCTION>
    Factory (const Options&, CALLBACK_FUNCTION&&)
        -> Factory<invoke_result_t<CALLBACK_FUNCTION, typename FunctionTraits<CALLBACK_FUNCTION>::template ArgOrVoid_t<0>>,
                   remove_cvref_t<typename FunctionTraits<CALLBACK_FUNCTION>::template ArgOrVoid_t<0>>>;
    template <typename CALLBACK_FUNCTION>
    Factory (const Options&, CALLBACK_FUNCTION&&)
        -> Factory<invoke_result_t<CALLBACK_FUNCTION, typename FunctionTraits<CALLBACK_FUNCTION>::template ArgOrVoid_t<0>, typename FunctionTraits<CALLBACK_FUNCTION>::template ArgOrVoid_t<1>>,
                   remove_cvref_t<typename FunctionTraits<CALLBACK_FUNCTION>::template ArgOrVoid_t<0>>,
                   remove_cvref_t<typename FunctionTraits<CALLBACK_FUNCTION>::template ArgOrVoid_t<1>>>;
    template <typename CALLBACK_FUNCTION>
    Factory (const Options&, CALLBACK_FUNCTION&&)
        -> Factory<invoke_result_t<CALLBACK_FUNCTION, typename FunctionTraits<CALLBACK_FUNCTION>::template ArgOrVoid_t<0>,
                                   typename FunctionTraits<CALLBACK_FUNCTION>::template ArgOrVoid_t<1>, typename FunctionTraits<CALLBACK_FUNCTION>::template ArgOrVoid_t<2>>,
                   remove_cvref_t<typename FunctionTraits<CALLBACK_FUNCTION>::template ArgOrVoid_t<0>>,
                   remove_cvref_t<typename FunctionTraits<CALLBACK_FUNCTION>::template ArgOrVoid_t<1>>,
                   remove_cvref_t<typename FunctionTraits<CALLBACK_FUNCTION>::template ArgOrVoid_t<2>>>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ObjectRequestHandler.inl"

#endif /*_Stroika_Framework_WebService_Server_ObjectRequestHandler_h_*/
