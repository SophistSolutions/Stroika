/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Framework_WebService_Server_ObjectVariantMapper_inl_
#define _Stroika_Framework_WebService_Server_ObjectVariantMapper_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <tuple>

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
        inline auto mkArgsTuple_ (const Iterable<VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper, const function<IGNORED_F_RETURN_TYPE (void)>& f)
        {
            Require (variantValueArgs.size () == 0);
            return make_tuple ();
        }
        template <typename IGNORED_F_RETURN_TYPE, typename SINGLE_ARG>
        inline auto mkArgsTuple_ (const Iterable<VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper, const function<IGNORED_F_RETURN_TYPE (SINGLE_ARG)>& f)
        {
            Require (variantValueArgs.size () == 1);
            return make_tuple (objVarMapper.ToObject<SINGLE_ARG> (variantValueArgs.Nth (0)));
        }
        template <typename IGNORED_F_RETURN_TYPE, typename ARG_FIRST, typename... REST_ARG_TYPES>
        inline auto mkArgsTuple_ (const Iterable<VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper, const function<IGNORED_F_RETURN_TYPE (ARG_FIRST, REST_ARG_TYPES...)>& f)
        {
            Require (variantValueArgs.size () == sizeof...(REST_ARG_TYPES) + 1);
            return tuple_cat (
                mkArgsTuple_ (variantValueArgs.Take (1), objVarMapper, function<IGNORED_F_RETURN_TYPE (ARG_FIRST)>{}),
                mkArgsTuple_ (variantValueArgs.Skip (1), objVarMapper, function<IGNORED_F_RETURN_TYPE (REST_ARG_TYPES...)>{}));
        }
    }
    template <typename RETURN_TYPE, typename... ARG_TYPES>
    VariantValue ApplyArgs (const Sequence<VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper, const function<RETURN_TYPE (ARG_TYPES...)>& f)
    {
        Require (variantValueArgs.size () == sizeof...(ARG_TYPES));
        if constexpr (is_same_v<RETURN_TYPE, void>) {
            apply (f, STROIKA_PRIVATE_::mkArgsTuple_ (variantValueArgs, objVarMapper, f));
            return VariantValue{};
        }
        else {
            return objVarMapper.FromObject (apply (f, STROIKA_PRIVATE_::mkArgsTuple_ (variantValueArgs, objVarMapper, f)));
        }
    }
    template <typename RETURN_TYPE, typename... ARG_TYPES>
    VariantValue ApplyArgs (const Mapping<String, VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (ARG_TYPES...)>& f)
    {
        Sequence<VariantValue> vvs;
        for (auto i : paramNames) {
            vvs += variantValueArgs.LookupValue (i);
        }
        ApplyArgs (vvs, objVarMapper, f);
    }

    /*
     ********************************************************************************
     ******* WebService::Server::VariantValue::PickoutParamValuesFromBody ***********
     ********************************************************************************
     */
    inline Mapping<String, VariantValue> PickoutParamValuesFromBody (Request* request)
    {
        RequireNotNull (request);
        return PickoutParamValuesFromBody (request->GetBody (), request->GetContentType ());
    }

    /*
     ********************************************************************************
     ******** WebService::Server::VariantValue::PickoutParamValuesFromURL ***********
     ********************************************************************************
     */
    inline Mapping<String, VariantValue> PickoutParamValuesFromURL (Request* request)
    {
        RequireNotNull (request);
        return PickoutParamValuesFromURL (request->GetURL ());
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
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<RETURN_TYPE (ARG_TYPE_COMBINED)>& f)
    {
        return [=](WebServer::Message* m) {
            ExpectedMethod (m->GetRequestReference (), webServiceDescription);
            if constexpr (is_same_v<RETURN_TYPE, void>) {
                f (objVarMapper.ToObject<ARG_TYPE_COMBINED> (CombineWebServiceArgsAsVariantValue (m->PeekRequest ())));
                WriteResponse (m->PeekResponse (), webServiceDescription);
            }
            else {
                WriteResponse (m->PeekResponse (), webServiceDescription, f (objVarMapper.ToObject<ARG_TYPE_COMBINED> (CombineWebServiceArgsAsVariantValue (m->PeekRequest ()))));
            }
        };
    }
    template <typename RETURN_TYPE, typename... IN_ARGS>
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (IN_ARGS...)>& f)
    {
        Require (paramNames.size () == sizeof...(IN_ARGS));
        return [=](WebServer::Message* m) {
            ExpectedMethod (m->GetRequestReference (), webServiceDescription);
            Sequence<VariantValue> args = OrderParamValues (paramNames, m->PeekRequest ());
            Assert (args.size () == paramNames.size ());
            if constexpr (is_same_v<RETURN_TYPE, void>) {
                ApplyArgs (args, objVarMapper, f);
                WriteResponse (m->PeekResponse (), webServiceDescription);
            }
            else {
                WriteResponse (m->PeekResponse (), webServiceDescription, ApplyArgs (args, objVarMapper, f));
            }
        };
    }
    template <typename RETURN_TYPE>
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<RETURN_TYPE (void)>& f)
    {
        return [=](WebServer::Message* m) {
            ExpectedMethod (m->GetRequestReference (), webServiceDescription);
            if constexpr (is_same_v<RETURN_TYPE, void>) {
                f ();
                WriteResponse (m->PeekResponse (), webServiceDescription);
            }
            else {
                WriteResponse (m->PeekResponse (), webServiceDescription, objVarMapper.FromObject (f ()));
            }
        };
    }

    ////////////////DEPRECATED STUFF
    [[deprecated ("since Stroika v2.1d13 - use one arg version")]] inline Mapping<String, VariantValue> PickoutParamValuesFromURL (Request* request, [[maybe_unused]] const optional<Iterable<String>>& namedParameters)
    {
        Assert (not namedParameters.has_value ()); // cuz never supported - you can manually call retainall if you want
        return PickoutParamValuesFromURL (request);
    }
    [[deprecated ("since Stroika v2.1d13 - use one arg version")]] inline Mapping<String, VariantValue> PickoutParamValuesFromURL (const URL& url, [[maybe_unused]] const optional<Iterable<String>>& namedParameters)
    {
        Assert (not namedParameters.has_value ()); // cuz never supported - you can manually call retainall if you want
        return PickoutParamValuesFromURL (url);
    }
    [[deprecated ("since Stroika v2.1d13 - use one arg version")]] inline Mapping<String, VariantValue> PickoutParamValuesFromBody (Request* request, [[maybe_unused]] const optional<Iterable<String>>& namedParameters)
    {
        Assert (not namedParameters.has_value ()); // cuz never supported - you can manually call retainall if you want
        return PickoutParamValuesFromBody (request);
    }
    [[deprecated ("since Stroika v2.1d13 - use one arg version")]] inline Mapping<String, VariantValue> PickoutParamValuesFromBody (const BLOB& body, const optional<InternetMediaType>& bodyContentType, [[maybe_unused]] const optional<Iterable<String>>& namedParameters)
    {
        Assert (not namedParameters.has_value ()); // cuz never supported - you can manually call retainall if you want
        return PickoutParamValuesFromBody (body, bodyContentType);
    }
    [[deprecated ("since Stroika v2.1d13 - use one arg version")]] inline Mapping<String, VariantValue> PickoutParamValues (Request* request, [[maybe_unused]] const optional<Iterable<String>>& namedURLParams, [[maybe_unused]] const optional<Iterable<String>>& namedBodyParams)
    {
        Assert (not namedURLParams.has_value ());  // cuz never supported - you can manually call retainall if you want
        Assert (not namedBodyParams.has_value ()); // cuz never supported - you can manually call retainall if you want
        return PickoutParamValues (request);
    }

    [[deprecated ("since Stroika v2.1d13 - use PickoutParamValues or OrderParamValues or CombineWebServiceArgsAsVariantValue")]] VariantValue GetWebServiceArgsAsVariantValue (Request* request, const optional<String>& fromInMessage = {});

    [[deprecated ("since Stroika v2.1d13 - use mkRequestHandler withotu objVariantMapper argument")]] inline WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper&, const function<BLOB (WebServer::Message* m)>& f)
    {
        return mkRequestHandler (webServiceDescription, f);
    }

    /**
     */
    // template <typename RETURN_TYPE, typename... IN_ARGS>
    // void CallFAndWriteConvertedResponse (Response* response, const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<RETURN_TYPE (IN_ARGS...)>& f, IN_ARGS... inArgs);
    namespace PRIVATE_ {
        template <typename RETURN_TYPE>
        inline void CallFAndWriteConvertedResponse_ (Response* response, const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<RETURN_TYPE ()>& f, enable_if_t<!is_same_v<RETURN_TYPE, void>>* = 0)
        {
            WriteResponse (response, webServiceDescription, objVarMapper.FromObject (forward<RETURN_TYPE> (f ())));
        }
        template <typename RETURN_TYPE>
        inline void CallFAndWriteConvertedResponse_ (Response* response, const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<RETURN_TYPE ()>& f, enable_if_t<is_same_v<RETURN_TYPE, void>>* = 0)
        {
            f ();
            WriteResponse (response, webServiceDescription);
        }
    }
    template <typename RETURN_TYPE, typename... IN_ARGS>
    inline void CallFAndWriteConvertedResponse (Response* response, const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<RETURN_TYPE (IN_ARGS...)>& f, IN_ARGS... inArgs)
    {
        PRIVATE_::CallFAndWriteConvertedResponse_ (response, webServiceDescription, objVarMapper, function<RETURN_TYPE ()>{bind<RETURN_TYPE> (f, forward<IN_ARGS> (inArgs)...)});
    }

}
#endif /*_Stroika_Framework_WebService_Server_ObjectVariantMapper_inl_*/
