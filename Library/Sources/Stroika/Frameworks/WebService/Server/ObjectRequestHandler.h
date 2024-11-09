/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_WebService_Server_ObjectRequestHandler_h_
#define _Stroika_Framework_WebService_Server_ObjectRequestHandler_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

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
     */
    struct Options {

        // todo add objevrainatmapper here (move it)
        ObjectVariantMapper fObjectMapper;

        /**
         * This is the default media type for the content type of the result message. If missing, it will be inferred based on data type produced.
         * regardless, it maybe overriden based on (eventually) http accept headers.
         */
        optional<InternetMediaType> fDefaultResultMediaType;

        function<VariantValue (Request&)> fExtractVariantValueFromRequest{ExtractArgumentsAsVariantValue::FromRequestBody};

        /** 
         *  Sometimesyou will want to treat the body as the sole input object for a webservice call. Sometimes
         * best to treat it as an array of parameters. If treated as an array of parameters (possible from mix of sources with fExtra... above)
         * then need their names and ordering to map to the arguments to the callback function.
         */
        optional<Iterable<String>> fTreatBodyAsListOfArguments;

        /**
         */
        nonvirtual String ToString () const;
    };
    static_assert (copyable<Options>);

    namespace Private_ {
        template <typename CALLBACK_FUNCTION>
        concept IsFunctionOfOneArgNoContext_ =
            FunctionTraits<CALLBACK_FUNCTION>::kArity == 1 and
            invocable<CALLBACK_FUNCTION, typename FunctionTraits<CALLBACK_FUNCTION>::template arg<0>::type> and
            not same_as<remove_cvref_t<typename FunctionTraits<CALLBACK_FUNCTION>::template arg<0>::type>, Context>;

        template <typename CALLBACK_FUNCTION>
        concept IsFunctionOfOneArgPlusContext_ =
            FunctionTraits<CALLBACK_FUNCTION>::kArity == 2 and
            invocable<CALLBACK_FUNCTION, typename FunctionTraits<CALLBACK_FUNCTION>::template arg<0>::type, Context>;

        // nb: would be nice if could use using instead of subclass, or better yet, if whole issue
        // of using (void) for arg wasn't so crazy inconvenient --LGP 2024-11-02
        template <typename RETURN_TYPE, typename WEB_METHOD_ARG>
        struct MagicRemoveVoidArgAddContext_ : function<RETURN_TYPE (WEB_METHOD_ARG, Context)> {};
        template <typename RETURN_TYPE>
        struct MagicRemoveVoidArgAddContext_<RETURN_TYPE, void> : function<RETURN_TYPE (Context)> {};
        template <typename RETURN_TYPE, typename WEB_METHOD_ARG>
        struct MagicRemoveVoidArgNoContext_ : function<RETURN_TYPE (WEB_METHOD_ARG)> {};
        template <typename RETURN_TYPE>
        struct MagicRemoveVoidArgNoContext_<RETURN_TYPE, void> : function<RETURN_TYPE ()> {};
    }

    /**
     * &&& EARLY DRAFT &&&&
     * 
     *  \par Example Usage
     *      \code
     *          Route{"api/objs/?"_RegEx,
     *                  ObjectRequestHandler::Factory{
     *                      kMapper,
     *                      [] () -> Sequence<GUID> {
     *                          return Sequence<GUID>{};
     *                      }}}
     *      \endcode
     * 
     *  \par Example Usage
     *      \code
     *          Route{"api/(v1/)?recordings/(.+)"_RegEx,
     *                 ObjectRequestHandler::Factory{kMapper, [this] (const ObjectRequestHandler::Context& c) -> Recording {
     *                     String id = c.fMatchedURLArgs[1];
     *                     return fWSImpl_->recordings_GET (id);
     *                 }}}
     *      \endcode
     * 
     *  \brief ObjectRequestHandler::Factory is a way to construct a WebServer::RequestHandler from an ObjectVariantMapper object and a lambda taking in/out params of objects.
     * 
     *  \todo check acceptsContentType and return result as JSON, binary json, or xml (etc) accordingly - take OPTIONS param saying default
     */
    template <typename RETURN_TYPE, typename WEB_METHOD_ARG, bool INCLUDE_CONTEXT>
    class Factory {
    public:
        static_assert (not is_reference_v<RETURN_TYPE>);
        static_assert (not is_reference_v<WEB_METHOD_ARG>);

    public:
        /**
         */
        template <invocable<Context> CALLBACK_FUNCTION>
        Factory (const ObjectVariantMapper& ovm, CALLBACK_FUNCTION&& highLevelHandler, const Options& options = {})
            requires (INCLUDE_CONTEXT);
        template <invocable<> CALLBACK_FUNCTION>
        Factory (const ObjectVariantMapper& ovm, CALLBACK_FUNCTION&& highLevelHandler, const Options& options = {})
            requires (not INCLUDE_CONTEXT);
        template <Private_::IsFunctionOfOneArgPlusContext_ CALLBACK_FUNCTION>
        Factory (const ObjectVariantMapper& ovm, CALLBACK_FUNCTION&& highLevelHandler, const Options& options = {})
            requires (INCLUDE_CONTEXT);
        template <Private_::IsFunctionOfOneArgNoContext_ CALLBACK_FUNCTION>
        Factory (const ObjectVariantMapper& ovm, CALLBACK_FUNCTION&& highLevelHandler, const Options& options = {})
            requires (not INCLUDE_CONTEXT);

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
         *                    ObjectRequestHandler::Factory f{kMapper, [this] (const Recording& r) { return fWSImpl_->recordings_POST (r); }};
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
        nonvirtual RETURN_TYPE ApplyHandler (Request& req, const Options& options = {}) const
            requires (not INCLUDE_CONTEXT);
        nonvirtual RETURN_TYPE ApplyHandler (const Context& c, const Options& options = {}) const
            requires (INCLUDE_CONTEXT);
        template <same_as<WEB_METHOD_ARG> WMA>
        nonvirtual RETURN_TYPE ApplyHandler (const WMA& arg, const Options& options = {}) const
            requires (not INCLUDE_CONTEXT);
        template <same_as<WEB_METHOD_ARG> WMA>
        nonvirtual RETURN_TYPE ApplyHandler (const WMA& arg, const Context& c, const Options& options = {}) const
            requires (INCLUDE_CONTEXT);

    public:
        /**
         *  Given the packaged up response 'r' - send it as a result, in the appropriate format (based on request headers etc)
         */
        template <same_as<RETURN_TYPE> RT>
        nonvirtual void SendResponse (const Request& request, Response& response, const RT& r) const;
        nonvirtual void SendResponse (const Request& request, Response& response) const
            requires (same_as<RETURN_TYPE, void>);

    private:
        ObjectVariantMapper fObjectVariantMapper_;
        using HandlerType_ = Select_t<Case<INCLUDE_CONTEXT, Private_::MagicRemoveVoidArgAddContext_<RETURN_TYPE, WEB_METHOD_ARG>>,
                                      Case<not INCLUDE_CONTEXT, Private_::MagicRemoveVoidArgNoContext_<RETURN_TYPE, WEB_METHOD_ARG>>>;
        HandlerType_ fHighLevelHandler_;
        Options      fOptions_;
    };
    template <invocable<Context> CALLBACK_FUNCTION, typename... IGNORED>
    Factory (const ObjectVariantMapper&, CALLBACK_FUNCTION&&, IGNORED...)
        -> Factory<typename FunctionTraits<CALLBACK_FUNCTION>::result_type, void, true>;
    template <invocable<> CALLBACK_FUNCTION, typename... IGNORED>
    Factory (const ObjectVariantMapper&, CALLBACK_FUNCTION&&, IGNORED...) -> Factory<invoke_result_t<CALLBACK_FUNCTION>, void, false>;
    template <Private_::IsFunctionOfOneArgPlusContext_ CALLBACK_FUNCTION, typename RT_ = typename FunctionTraits<CALLBACK_FUNCTION>::result_type,
              typename AT_ = remove_cvref_t<typename FunctionTraits<CALLBACK_FUNCTION>::template ArgOrVoid<0>::type>>
    Factory (const ObjectVariantMapper&, CALLBACK_FUNCTION&&) -> Factory<RT_, AT_, true>;
    template <Private_::IsFunctionOfOneArgNoContext_ CALLBACK_FUNCTION, typename... IGNORED>
    Factory (const ObjectVariantMapper&, CALLBACK_FUNCTION&&, IGNORED...)
        -> Factory<typename FunctionTraits<CALLBACK_FUNCTION>::result_type, remove_cvref_t<typename FunctionTraits<CALLBACK_FUNCTION>::template ArgOrVoid<0>::type>, false>;

    // @todo something which is a cross between
    //  template <typename RETURN_TYPE, typename... ARG_TYPES>
    //VariantValue ApplyArgs (const Sequence<VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper,
    //                        const function<RETURN_TYPE (ARG_TYPES...)>& f)
    // and Factory BELIW

    // We already have code to do most of it. Just change teh sole 'VariantValue' with a 'vector' of them, and do template magic over
    // args (as we do in ApplyArgs above) to call the resulting function.

    // maybe approach by creating new 'Factory2' - and only diff - is no conctext support.
    // THEN - make it variadic in number of args to function<>
    // THEN - special case ApplyArgs on a single arg type 'Context' - not to lookup argname - but just to add the context....

    template <typename RETURN_TYPE, typename... ARG_TYPES>
    class Factory2 {
    public:
        static_assert (not is_reference_v<RETURN_TYPE>);
        // static_assert (not is_reference_v<WEB_METHOD_ARG>);

    public:
        template <invocable<ARG_TYPES...> CALLBACK_FUNCTION>
        Factory2 (const Options& options, CALLBACK_FUNCTION&& highLevelHandler);

    public:
        nonvirtual operator Frameworks::WebServer::RequestHandler () const;

    public:
        nonvirtual RETURN_TYPE ApplyHandler (const Context& context) const;

    public:
        nonvirtual RETURN_TYPE ApplyObjectHandler (const Context& context, ARG_TYPES... args) const;

    public:
        template <typename T>
        nonvirtual T ConvertArg2Object (const VariantValue& v) const;

    private:
        // use tuple_cat to put all the args together (but in a tuple) and then apply on the function to expand the args to call f
        nonvirtual tuple<> mkArgsTuple_ (const Context& context, const Iterable<VariantValue>& variantValueArgs,
                                         [[maybe_unused]] const function<RETURN_TYPE (void)>& f);
        template <typename SINGLE_ARG>
        nonvirtual tuple<SINGLE_ARG> mkArgsTuple_ (const Context& context, const Iterable<VariantValue>& variantValueArgs,
                                                   [[maybe_unused]] const function<RETURN_TYPE (SINGLE_ARG)>& f);
        template <typename ARG_FIRST, typename... REST_ARG_TYPES>
        nonvirtual auto mkArgsTuple_ (const Context& context, const Iterable<VariantValue>& variantValueArgs,
                                      [[maybe_unused]] const function<RETURN_TYPE (ARG_FIRST, REST_ARG_TYPES...)>& f)
            -> decltype (tuple_cat (declval<ARG_FIRST> (), declval<REST_ARG_TYPES...> ()));

    public:
        /**
         */
        template <same_as<RETURN_TYPE> RT>
        nonvirtual void SendResponse (const Request& request, Response& response, const RT& r) const;
        nonvirtual void SendResponse (const Request& request, Response& response) const
            requires (same_as<RETURN_TYPE, void>);

    private:
        function<RETURN_TYPE (ARG_TYPES...)> fHighLevelHandler_;
        Options                              fOptions_;
    };
    template <typename CALLBACK_FUNCTION, typename RETURN_TYPE, typename... ARG_TYPES>
        requires (convertible_to<CALLBACK_FUNCTION, function<RETURN_TYPE (ARG_TYPES...)>>)
    Factory2 (const Options&, CALLBACK_FUNCTION&&) -> Factory2<RETURN_TYPE, ARG_TYPES...>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ObjectRequestHandler.inl"

#endif /*_Stroika_Framework_WebService_Server_ObjectRequestHandler_h_*/
