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

    // maybe call Factory => ObjectRequestHandler
    // Stroika WebServer RequestHanlder looks like:
    //      void (Message* message, const Containers::Sequence<Characters::String>& matchedArgs
    //
    // together with ObjectVariantMapper, we replace this with
    //      RESULT_TYPE (MESSAGE_ARGUMENT_TYPE, optional<CONTEXT> c)
    //      context contains matched URL args, and reference to original message (or at least request) in case we need to do more,
    // andmaby also response (so maybe just message)

    // sepearate issue - is mapping query-args to be part of original message
    // making this work with PATCH methods

    // todo use template guides so dont need to specify RETURN_TYPE etc args...

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
     */
    struct Options {
        /**
         * This is the default media type for the content type of the result message. If missing, it will be inferred based on data type produced.
         * regardless, it maybe overriden based on (eventually) http accept headers.
         */
        optional<InternetMediaType> fDefaultResultMediaType;

        /**
         */
        String ToString () const;
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
        nonvirtual RETURN_TYPE ApplyHandler (Request& req) const
            requires (not INCLUDE_CONTEXT);
        nonvirtual RETURN_TYPE ApplyHandler (const Context& c) const
            requires (INCLUDE_CONTEXT);
        template <same_as<WEB_METHOD_ARG> WMA>
        nonvirtual RETURN_TYPE ApplyHandler (const WMA& arg) const
            requires (not INCLUDE_CONTEXT);
        template <same_as<WEB_METHOD_ARG> WMA>
        nonvirtual RETURN_TYPE ApplyHandler (const WMA& arg, const Context& c) const
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

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ObjectRequestHandler.inl"

#endif /*_Stroika_Framework_WebService_Server_ObjectRequestHandler_h_*/
