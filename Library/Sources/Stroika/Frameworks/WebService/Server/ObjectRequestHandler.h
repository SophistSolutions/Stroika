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
#include "Stroika/Frameworks/WebService/Server/VariantValue.h"

/*
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
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
     *  \brief not directly instantiated, but to receive context arguments in callbacks.
     * 
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
        nonvirtual String ToString () const;
    };
    static_assert (not copyable<Context>);
    static_assert (not movable<Context>);

    /**
     *  \brief Options for ObjectRequestHandler - mostly the ObjectVariantMapper, but also a few others depending on situation
     */
    struct Options {

        /**
         */
        ObjectVariantMapper fObjectMapper;

        /**
         *  if set specified, any web-method not in the set will be rejected.
         *      \note These are compared case-sensitive, and are typically upper case.
         */
        optional<Set<String>> fAllowedMethods; // e.g. GET

        /**
         * This is the default media type for the content type of the result message. If missing, it will be inferred based on data type produced.
         * regardless, it maybe overriden based on (eventually) http accept headers.
         */
        optional<InternetMediaType> fDefaultResultMediaType;

        /**
         */
        function<VariantValue (Request&)> fExtractVariantValueFromRequest{Server::VariantValue::ExtractArgumentsAsVariantValue::FromRequestBody};

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
     *  \brief ObjectRequestHandler::Factory is a way to construct a WebServer::RequestHandler from an ObjectVariantMapper object and a lambda taking in/out params of objects.
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
     * 
     *  \todo check acceptsContentType and return result as JSON, binary json, or xml (etc) accordingly - take OPTIONS param saying default
     */
    template <typename RETURN_TYPE, typename... ARG_TYPES>
    class Factory {
    public:
        static_assert (not is_reference_v<RETURN_TYPE>);
        //static_assert (conjunction<((not is_reference_v<ARG_TYPES>)  ...)>, ""); // todo something close to this

    public:
        /**
         *  \brief Build Frameworks::WebServer::RequestHandler out of ObjectVariantMapper, a few options/clues, and a object-based Route callback function
         */
        template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<ARG_TYPES...>) CALLBACK_FUNCTION>
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
         *                [this] (Message& m) {
         *                    // use ObjectRequestHandler::Factory indirectly so can support POST raw data and arguments as query-args!
         *                    ObjectRequestHandler::Factory f{{kMapper}, [this] (const Recording& r) { return fWSImpl_->recordings_POST (r); }};
         *                    Recording                     arg = [&] () {
         *                        InternetMediaType requestCt =
         *                            Memory::ValueOfOrThrow (m.request ().contentType (), ClientErrorException{"missing request content type"sv});
         *                        auto ctChecker = InternetMediaTypeRegistry::sThe.load ();
         *                        if (ctChecker.IsA (InternetMediaTypes::kJSON, requestCt)) {
         *                            return Recording{kMapper.ToObject<Recording> (m.rwRequest ().GetBodyVariantValue ())};
         *                        }
         *                        else if (ctChecker.IsA (InternetMediaTypes::kAudio, requestCt)) {
         *                            auto r = Recording{.fData = make_tuple (requestCt, m.rwRequest ().GetBody ())};
         *                            // also can grab some parameters, like user, etc from query args - @todo
         *                            return r;
         *                        }
         *                        else {
         *                            Throw (ClientErrorException{"unsupported request content type"sv});
         *                        }
         *                    }();
         *                    auto rr = f.ApplyHandler (arg);
         *                    f.SendResponse (m.request (), m.rwResponse (), rr);
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
        // \require nArgs in variantValueArgs >= count(ARGS_TYPE...)
        template <typename RET = RETURN_TYPE>
        nonvirtual tuple<> mkArgsTuple_ (const Context& context, const Iterable<VariantValue>& variantValueArgs,
                                         [[maybe_unused]] const function<RET ()>& f) const;
        template <typename SINGLE_ARG>
        nonvirtual tuple<SINGLE_ARG> mkArgsTuple_ (const Context& context, const Iterable<VariantValue>& variantValueArgs,
                                                   [[maybe_unused]] const function<RETURN_TYPE (SINGLE_ARG)>& f) const;
        template <typename ARG_FIRST, typename... REST_ARG_TYPES>
        nonvirtual auto mkArgsTuple_ (const Context& context, const Iterable<VariantValue>& variantValueArgs,
                                      [[maybe_unused]] const function<RETURN_TYPE (ARG_FIRST, REST_ARG_TYPES...)>& f) const
            -> decltype (tuple_cat (make_tuple (declval<remove_cvref_t<ARG_FIRST>> ()), make_tuple (declval<REST_ARG_TYPES...> ())));

    public:
        /**
         *  Given the packaged up response 'r' - send it as a result, in the appropriate format (based on request accept headers etc)
         * 
         *  \see also SendStringResponse, if the response should be sent as a string
         */
        template <same_as<RETURN_TYPE> RT>
        nonvirtual void SendResponse (const Request& request, Response& response, const RT& r) const;
        nonvirtual void SendResponse (const Request& request, Response& response) const
            requires (same_as<RETURN_TYPE, void>);

    public:
        /**
         *  Send the given response, but assure content type is 'text' or textish, and don't encode 'r' as JSON.
         *  This is primarily useful for a POST method, where you want to return the ID, not "ID".
         */
        template <same_as<RETURN_TYPE> RT>
        nonvirtual void SendStringResponse (const Request& request, Response& response, const RT& r) const;

    private:
        function<RETURN_TYPE (ARG_TYPES...)> fHighLevelHandler_;
        Options                              fOptions_;
    };

    namespace Private_ {
        // just to shorten stuff below...
        template <typename CALLBACK_FUNCTION, size_t i>
        using CBArg_t_ = remove_cvref_t<typename FunctionTraits<CALLBACK_FUNCTION>::template ArgOrVoid_t<i>>;
    }

    // hopefully adequate approach for now, but there must be some way to generalize this - perhaps with folds?
    // --LGP 2024-11-10
    template <typename CALLBACK_FUNCTION>
    Factory (const Options&, CALLBACK_FUNCTION&&) -> Factory<invoke_result_t<CALLBACK_FUNCTION>>;
    template <typename CALLBACK_FUNCTION>
    Factory (const Options&, CALLBACK_FUNCTION&&)
        -> Factory<invoke_result_t<CALLBACK_FUNCTION, Private_::CBArg_t_<CALLBACK_FUNCTION, 0>>, Private_::CBArg_t_<CALLBACK_FUNCTION, 0>>;
    template <typename CALLBACK_FUNCTION>
    Factory (const Options&, CALLBACK_FUNCTION&&)
        -> Factory<invoke_result_t<CALLBACK_FUNCTION, Private_::CBArg_t_<CALLBACK_FUNCTION, 0>, Private_::CBArg_t_<CALLBACK_FUNCTION, 1>>,
                   Private_::CBArg_t_<CALLBACK_FUNCTION, 0>, Private_::CBArg_t_<CALLBACK_FUNCTION, 1>>;
    template <typename CALLBACK_FUNCTION>
    Factory (const Options&, CALLBACK_FUNCTION&&)
        -> Factory<invoke_result_t<CALLBACK_FUNCTION, Private_::CBArg_t_<CALLBACK_FUNCTION, 0>, Private_::CBArg_t_<CALLBACK_FUNCTION, 1>, Private_::CBArg_t_<CALLBACK_FUNCTION, 2>>,
                   Private_::CBArg_t_<CALLBACK_FUNCTION, 0>, Private_::CBArg_t_<CALLBACK_FUNCTION, 1>, Private_::CBArg_t_<CALLBACK_FUNCTION, 2>>;
    template <typename CALLBACK_FUNCTION>
    Factory (const Options&, CALLBACK_FUNCTION&&)
        -> Factory<invoke_result_t<CALLBACK_FUNCTION, Private_::CBArg_t_<CALLBACK_FUNCTION, 0>, Private_::CBArg_t_<CALLBACK_FUNCTION, 1>,
                                   Private_::CBArg_t_<CALLBACK_FUNCTION, 2>, Private_::CBArg_t_<CALLBACK_FUNCTION, 3>>,
                   Private_::CBArg_t_<CALLBACK_FUNCTION, 0>, Private_::CBArg_t_<CALLBACK_FUNCTION, 1>,
                   Private_::CBArg_t_<CALLBACK_FUNCTION, 2>, Private_::CBArg_t_<CALLBACK_FUNCTION, 3>>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ObjectRequestHandler.inl"

#endif /*_Stroika_Framework_WebService_Server_ObjectRequestHandler_h_*/
