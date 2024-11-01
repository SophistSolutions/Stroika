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
    }

    /**
    * &&& EARLY DRAFT &&&&
    * 
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
        Factory (const ObjectVariantMapper& ovm, function<RETURN_TYPE (Context)> highLevelHandler, const Options& options = {});
        template <Private_::IsFunctionOfOneArgPlusContext_ CALLBACK_FUNCTION>
            requires (INCLUDE_CONTEXT and not same_as<remove_cvref_t<typename FunctionTraits<CALLBACK_FUNCTION>::template arg<0>::type>, void>)
        Factory (const ObjectVariantMapper& ovm, CALLBACK_FUNCTION&& highLevelHandler, const Options& options = {});
        Factory (const ObjectVariantMapper& ovm, function<RETURN_TYPE (WEB_METHOD_ARG)> highLevelHandler, const Options& options = {})
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
         */
        nonvirtual RETURN_TYPE ApplyHandler (Request* req) const
            requires (not INCLUDE_CONTEXT);
        nonvirtual RETURN_TYPE ApplyHandler (const Context& c) const
            requires (INCLUDE_CONTEXT);

    private:
        ObjectVariantMapper fObjectVariantMapper_;
        template <typename WA = WEB_METHOD_ARG>
        struct MagicRemoveVoidArg_ : function<RETURN_TYPE (WEB_METHOD_ARG, Context)> {};
        template <>
        struct MagicRemoveVoidArg_<void> : function<RETURN_TYPE (Context)> {};
        using HandlerType_ = Select_t<Case<INCLUDE_CONTEXT and same_as<WEB_METHOD_ARG, void>, function<RETURN_TYPE (Context)>>,
                                      Case<INCLUDE_CONTEXT and not same_as<WEB_METHOD_ARG, void>, MagicRemoveVoidArg_<WEB_METHOD_ARG>>,
                                      Case<not INCLUDE_CONTEXT, function<RETURN_TYPE (WEB_METHOD_ARG)>>>;
        HandlerType_ fHighLevelHandler_;
        Options      fOptions_;
    };
    template <invocable<Context> CALLBACK_FUNCTION, typename... IGNORED>
    Factory (const ObjectVariantMapper&, CALLBACK_FUNCTION&&, IGNORED...)
        -> Factory<typename FunctionTraits<CALLBACK_FUNCTION>::result_type, void, true>;
    template <invocable<> CALLBACK_FUNCTION>
    Factory (const ObjectVariantMapper&, CALLBACK_FUNCTION&&) -> Factory<typename FunctionTraits<CALLBACK_FUNCTION>::result_type, void, false>;
    template <Private_::IsFunctionOfOneArgNoContext_ CALLBACK_FUNCTION>
    Factory (const ObjectVariantMapper&, CALLBACK_FUNCTION&&)
        -> Factory<typename FunctionTraits<CALLBACK_FUNCTION>::result_type, remove_cvref_t<typename FunctionTraits<CALLBACK_FUNCTION>::template arg<0>::type>, false>;
    template <Private_::IsFunctionOfOneArgPlusContext_ CALLBACK_FUNCTION>
    Factory (const ObjectVariantMapper&, CALLBACK_FUNCTION&&)
        -> Factory<typename FunctionTraits<CALLBACK_FUNCTION>::result_type, remove_cvref_t<typename FunctionTraits<CALLBACK_FUNCTION>::template arg<0>::type>, true>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ObjectRequestHandler.inl"

#endif /*_Stroika_Framework_WebService_Server_ObjectRequestHandler_h_*/
