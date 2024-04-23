/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include <tuple>

#include "../../../Foundation/IO/Network/HTTP/ClientErrorException.h"

#include "Basic.h"

namespace Stroika::Frameworks::WebService::Server::VariantValue {

    /*
     ********************************************************************************
     ****************** WebService::Server::VariantValue::ApplyArgs *****************
     ********************************************************************************
     */
    namespace STROIKA_PRIVATE_ {
        // use tuple_cat to put all the args together (but in a tuple) and then apply on the function to expand the args to call f
        template <typename IGNORED_F_RETURN_TYPE>
        inline auto mkArgsTuple_ (const Iterable<VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper,
                                  [[maybe_unused]] const function<IGNORED_F_RETURN_TYPE (void)>& f)
        {
            Require (variantValueArgs.size () == 0);
            return make_tuple ();
        }
        template <typename IGNORED_F_RETURN_TYPE, typename SINGLE_ARG>
        inline auto mkArgsTuple_ (const Iterable<VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper,
                                  [[maybe_unused]] const function<IGNORED_F_RETURN_TYPE (SINGLE_ARG)>& f)
        {
            Require (variantValueArgs.size () == 1);
            return make_tuple (objVarMapper.ToObject<SINGLE_ARG> (variantValueArgs.Nth (0)));
        }
        template <typename IGNORED_F_RETURN_TYPE, typename ARG_FIRST, typename... REST_ARG_TYPES>
        inline auto mkArgsTuple_ (const Iterable<VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper,
                                  [[maybe_unused]] const function<IGNORED_F_RETURN_TYPE (ARG_FIRST, REST_ARG_TYPES...)>& f)
        {
            Require (variantValueArgs.size () == sizeof...(REST_ARG_TYPES) + 1);
            return tuple_cat (mkArgsTuple_ (variantValueArgs.Take (1), objVarMapper, function<IGNORED_F_RETURN_TYPE (ARG_FIRST)>{}),
                              mkArgsTuple_ (variantValueArgs.Skip (1), objVarMapper, function<IGNORED_F_RETURN_TYPE (REST_ARG_TYPES...)>{}));
        }
    }
    template <typename RETURN_TYPE, typename... ARG_TYPES>
    VariantValue ApplyArgs (const Sequence<VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper,
                            const function<RETURN_TYPE (ARG_TYPES...)>& f)
    {
        using IO::Network::HTTP::ClientErrorException;
        Require (variantValueArgs.size () == sizeof...(ARG_TYPES));
        // exceptions parsing args mean ill-formatted arguments to the webservice, so treat as client errors
        auto&& args = ClientErrorException::TreatExceptionsAsClientError (
            [&] () { return STROIKA_PRIVATE_::mkArgsTuple_ (variantValueArgs, objVarMapper, f); });
        if constexpr (is_same_v<RETURN_TYPE, void>) {
            apply (f, args);
            return VariantValue{};
        }
        else {
            return objVarMapper.FromObject (apply (f, args));
        }
    }
    template <typename RETURN_TYPE, typename... ARG_TYPES>
    VariantValue ApplyArgs (const Mapping<String, VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper,
                            const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (ARG_TYPES...)>& f)
    {
        Sequence<VariantValue> vvs;
        for (const auto& i : paramNames) {
            vvs += variantValueArgs.LookupValue (i);
        }
        return ApplyArgs (vvs, objVarMapper, f);
    }

    /*
     ********************************************************************************
     ******* WebService::Server::VariantValue::PickoutParamValuesFromBody ***********
     ********************************************************************************
     */
    inline Mapping<String, VariantValue> PickoutParamValuesFromBody (Request* request)
    {
        RequireNotNull (request);
        return PickoutParamValuesFromBody (request->GetBody (), request->contentType ());
    }

    /*
     ********************************************************************************
     ******** WebService::Server::VariantValue::PickoutParamValuesFromURL ***********
     ********************************************************************************
     */
    inline Mapping<String, VariantValue> PickoutParamValuesFromURL (const Request* request)
    {
        RequireNotNull (request);
        return PickoutParamValuesFromURL (request->url ());
    }

    /*
     ********************************************************************************
     ************ WebService::Server::VariantValue::OrderParamValues ****************
     ********************************************************************************
     */
    inline Sequence<VariantValue> OrderParamValues (const Iterable<String>& paramNames, Request* request)
    {
        return OrderParamValues (paramNames, PickoutParamValues (request));
    }

    /*
     ********************************************************************************
     ***************** WebService::Server::VariantValue::WriteResponse **************
     ********************************************************************************
     */
    inline void WriteResponse ([[maybe_unused]] Response* response, [[maybe_unused]] const WebServiceMethodDescription& webServiceDescription)
    {
        // nothing todo to write empty (void) response
    }

    /*
     ********************************************************************************
     **************** WebService::Server::VariantValue::mkRequestHandler ************
     ********************************************************************************
     */
    template <typename RETURN_TYPE, typename ARG_TYPE_COMBINED>
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription&               webServiceDescription,
                                                const DataExchange::ObjectVariantMapper&         objVarMapper,
                                                const function<RETURN_TYPE (ARG_TYPE_COMBINED)>& f)
    {
        return [=] (WebServer::Message* m) {
            ExpectedMethod (m->request (), webServiceDescription);
            if constexpr (is_same_v<RETURN_TYPE, void>) {
                f (objVarMapper.ToObject<ARG_TYPE_COMBINED> (CombineWebServiceArgsAsVariantValue (&m->rwRequest ())));
                WriteResponse (&m->rwResponse (), webServiceDescription);
            }
            else {
                WriteResponse (&m->rwResponse (), webServiceDescription,
                               f (objVarMapper.ToObject<ARG_TYPE_COMBINED> (CombineWebServiceArgsAsVariantValue (&m->rwRequest ()))));
            }
        };
    }
    template <typename RETURN_TYPE, typename... IN_ARGS>
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription&       webServiceDescription,
                                                const DataExchange::ObjectVariantMapper& objVarMapper,
                                                const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (IN_ARGS...)>& f)
    {
        Require (paramNames.size () == sizeof...(IN_ARGS));
        return [=] (WebServer::Message* m) {
            ExpectedMethod (m->request (), webServiceDescription);
            Sequence<VariantValue> args = OrderParamValues (paramNames, &m->rwRequest ());
            Assert (args.size () == paramNames.size ());
            if constexpr (is_same_v<RETURN_TYPE, void>) {
                (void)ApplyArgs (args, objVarMapper, f);
                WriteResponse (&m->rwResponse (), webServiceDescription);
            }
            else {
                WriteResponse (&m->rwResponse (), webServiceDescription, ApplyArgs (args, objVarMapper, f));
            }
        };
    }
    template <typename RETURN_TYPE>
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription,
                                                const DataExchange::ObjectVariantMapper& objVarMapper, const function<RETURN_TYPE (void)>& f)
    {
        return [=] (WebServer::Message* m) {
            ExpectedMethod (m->request (), webServiceDescription);
            if constexpr (is_same_v<RETURN_TYPE, void>) {
                f ();
                WriteResponse (&m->rwResponse (), webServiceDescription);
            }
            else {
                WriteResponse (&m->rwResponse (), webServiceDescription, objVarMapper.FromObject (f ()));
            }
        };
    }

    /**
     */
    // template <typename RETURN_TYPE, typename... IN_ARGS>
    // void CallFAndWriteConvertedResponse (Response* response, const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<RETURN_TYPE (IN_ARGS...)>& f, IN_ARGS... inArgs);
    namespace PRIVATE_ {
        template <typename RETURN_TYPE>
        inline void CallFAndWriteConvertedResponse_ (Response* response, const WebServiceMethodDescription& webServiceDescription,
                                                     const DataExchange::ObjectVariantMapper& objVarMapper,
                                                     const function<RETURN_TYPE ()>& f, enable_if_t<!is_same_v<RETURN_TYPE, void>>* = 0)
        {
            WriteResponse (response, webServiceDescription, objVarMapper.FromObject (forward<RETURN_TYPE> (f ())));
        }
        template <typename RETURN_TYPE>
        inline void CallFAndWriteConvertedResponse_ (Response* response, const WebServiceMethodDescription& webServiceDescription,
                                                     const DataExchange::ObjectVariantMapper& objVarMapper,
                                                     const function<RETURN_TYPE ()>& f, enable_if_t<is_same_v<RETURN_TYPE, void>>* = 0)
        {
            f ();
            WriteResponse (response, webServiceDescription);
        }
    }
    template <typename RETURN_TYPE, typename... IN_ARGS>
    inline void CallFAndWriteConvertedResponse (Response* response, const WebServiceMethodDescription& webServiceDescription,
                                                const DataExchange::ObjectVariantMapper&  objVarMapper,
                                                const function<RETURN_TYPE (IN_ARGS...)>& f, IN_ARGS... inArgs)
    {
        PRIVATE_::CallFAndWriteConvertedResponse_ (response, webServiceDescription, objVarMapper,
                                                   function<RETURN_TYPE ()>{bind<RETURN_TYPE> (f, forward<IN_ARGS> (inArgs)...)});
    }

}
