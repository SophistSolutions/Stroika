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
#include "Basic.h"
#if 0
namespace std {
    namespace detail {
        template <class F, class Tuple, size_t... I>
        constexpr decltype (auto) apply_impl (F&& f, Tuple&& t, index_sequence<I...>)
        {
            return invoke (forward<F> (f), get<I> (forward<Tuple> (t))...);
        }
    } // namespace detail

    template <class F, class Tuple>
    constexpr decltype (auto) apply (F&& f, Tuple&& t)
    {
        return detail::apply_impl (
            forward<F> (f), forward<Tuple> (t),
            make_index_sequence<tuple_size<decay_t<Tuple>>::value>{});
    }
}
#endif

namespace Stroika::Frameworks::WebService::Server::VariantValue {

    /*
    ********************************************************************************
    **************** WebService::Server::VariantValue::mkRequestHandler ************
    ********************************************************************************
    */
#if 0
    namespace {
        template <typename T, typename... REST_IN_ARGS>
        auto ConvertVariantValuesToTypes_ (const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<VariantValue>& vvs, T, REST_IN_ARGS... args)
        {
            Require (vvs.size () == (sizeof...(args)) + 1);
            return tuple_cat (tuple<T>{objVarMapper.ToObject<T> (vvs.Nth (0))}, ConvertVariantValuesToTypes_ (objVarMapper, vvs.Skip (1), forward<REST_IN_ARGS> (args)...));
        }
        auto ConvertVariantValuesToTypes_ (const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<VariantValue>& vvs)
        {
            Require (vvs.size () == 0);
            return tuple_cat ();
        }
    }
    namespace {
        template < typename T, typename... Ts >
        auto head (const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<VariantValue>& vvs, tuple<T, Ts...> t)
        {
            return  get<0> (t);
        }
        template < size_t... Ns, typename... Ts >
        auto tail_impl (const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<VariantValue>& vvs, index_sequence<Ns...>, tuple<Ts...> t)
        {
            return  make_tuple (get<Ns + 1u> (t)...);
        }
        template < typename... Ts >
        auto tail (const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<VariantValue>& vvs, tuple<Ts...> t)
        {
            return  tail_impl (make_index_sequence<sizeof...(Ts)-1u> (), t);
        }
        template <typename T, typename... REST_IN_ARGS>
        auto ConvertVariantValuesToTypes_x (const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<VariantValue>& vvs, T, tuple<T, REST_IN_ARGS...> args)
        {
            //WRequire (vvs.size () == (sizeof...(args)) + 1);
            return tuple_cat (tuple<T>{objVarMapper.ToObject<T> (vvs.Nth (0))}, ConvertVariantValuesToTypes_x (vvs.Skip (1), forward<REST_IN_ARGS> (args)...));
        }
        auto ConvertVariantValuesToTypes_x (const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<VariantValue>& vvs, tuple<> a)
        {
            Require (vvs.size () == 0);
            return tuple_cat ();
        }
    }
    namespace {
        template <typename RETURN_TYPE, typename... Args>
        struct save_it_for_later {

            DataExchange::ObjectVariantMapper objVarMapper;
            Sequence<VariantValue>            vvs;

            const function<RETURN_TYPE (Args...)>& f;

            tuple<Args...> params;


            template <size_t... I>
            RETURN_TYPE call_func (index_sequence<I...>)
            {
                //return f (ConvertVariantValuesToTypes_ (objVarMapper, vvs, get<I> (params)...));
            //  using T = decltype (get<I> (params)...);
                //using T = tuple_element_t<I, tuple<Args...>>;
                return f (objVarMapper.ToObject<tuple_element_t<I..., decltype(params)>> (vvs.Nth (I))...);
            }
            RETURN_TYPE delayed_dispatch ()
            {
                return call_func (index_sequence_for<Args...>{});
            }
        };
    }
    template <typename RETURN_TYPE, typename... IN_ARGS>
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (IN_ARGS...)>& f)
    {
        return [=](WebServer::Message* m) {
            using namespace Containers;
            ExpectedMethod (m->GetRequestReference (), webServiceDescription);
            VariantValue allArgsAsVariantValue = GetWebServiceArgsAsVariantValue (m->PeekRequest (), {});
            Mapping<String, VariantValue> allArgsMap = allArgsAsVariantValue.As<Mapping<String, VariantValue>> ();
            Sequence<VariantValue> vvs;
            for (auto i : paramNames) {
                vvs += allArgsMap.LookupValue (i);
            }
            //  tuple<IN_ARGS...> junk;

            save_it_for_later<RETURN_TYPE, IN_ARGS...> aaa{ objVarMapper, vvs, f };

            //tuple<IN_ARGS...> args2Forward = ConvertVariantValuesToTypes_ (objVarMapper, vvs, forward<IN_ARGS...> (junk)...);
            //tuple<IN_ARGS...> args2Forward = apply (ConvertVariantValuesToTypes_, tuple_cat (objVarMapper, vvs, forward<IN_ARGS> (junk)...));
            //RETURN_TYPE           response2Send = f (forward<IN_ARGS> (args2Forward)...);
            RETURN_TYPE response2Send = aaa.delayed_dispatch ();
            WriteResponse (m->PeekResponse (), webServiceDescription, objVarMapper.FromObject (response2Send));
        };
    }
    template <typename RETURN_TYPE, typename IN_ARGS>
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<RETURN_TYPE (IN_ARGS)>& f)
    {
        return [=](WebServer::Message* m) {
            ExpectedMethod (m->GetRequestReference (), webServiceDescription);
            WriteResponse (m->PeekResponse (), webServiceDescription, objVarMapper.FromObject (f (objVarMapper.ToObject<IN_ARGS> (GetWebServiceArgsAsVariantValue (m->PeekRequest (), {})))));
        };
    }
#endif

    template <typename RETURN_TYPE>
    RETURN_TYPE ApplyArgs (const Mapping<String, VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (void)>& f)
    {
        Require (paramNames.size () == 0);
        return f ();
    }
    template <typename RETURN_TYPE, typename ARG_TYPE_0>
    RETURN_TYPE ApplyArgs (const Mapping<String, VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (ARG_TYPE_0)>& f)
    {
        Require (paramNames.size () == 1);
        Sequence<VariantValue> vvs;
        for (auto i : paramNames) {
            vvs += variantValueArgs.LookupValue (i);
        }
        return f (objVarMapper.ToObject<ARG_TYPE_0> (vvs[0]));
    }
    template <typename RETURN_TYPE, typename ARG_TYPE_0, typename ARG_TYPE_1>
    RETURN_TYPE ApplyArgs (const Mapping<String, VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (ARG_TYPE_0, ARG_TYPE_1)>& f)
    {
        Require (paramNames.size () == 2);
        Sequence<VariantValue> vvs;
        for (auto i : paramNames) {
            vvs += variantValueArgs.LookupValue (i);
        }
        return objVarMapper.FromObject (f (objVarMapper.ToObject<ARG_TYPE_0> (vvs[0]), objVarMapper.ToObject<ARG_TYPE_1> (vvs[1])));
    }
    template <typename RETURN_TYPE, typename ARG_TYPE_0, typename ARG_TYPE_1, typename ARG_TYPE_2>
    RETURN_TYPE ApplyArgs (const Mapping<String, VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (ARG_TYPE_0, ARG_TYPE_1, ARG_TYPE_2)>& f)
    {
        Require (paramNames.size () == 3);
        Sequence<VariantValue> vvs;
        for (auto i : paramNames) {
            vvs += variantValueArgs.LookupValue (i);
        }
        return objVarMapper.FromObject (f (objVarMapper.ToObject<ARG_TYPE_0> (vvs[0]), objVarMapper.ToObject<ARG_TYPE_1> (vvs[1]), objVarMapper.ToObject<ARG_TYPE_2> (vvs[2])));
    }
    template <typename RETURN_TYPE, typename ARG_TYPE_0, typename ARG_TYPE_1, typename ARG_TYPE_2, typename ARG_TYPE_3>
    RETURN_TYPE ApplyArgs (const Mapping<String, VariantValue>& variantValueArgs, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (ARG_TYPE_0, ARG_TYPE_1, ARG_TYPE_2, ARG_TYPE_3)>& f)
    {
        Require (paramNames.size () == 4);
        Sequence<VariantValue> vvs;
        for (auto i : paramNames) {
            vvs += variantValueArgs.LookupValue (i);
        }
        return objVarMapper.FromObject (f (objVarMapper.ToObject<ARG_TYPE_0> (vvs[0]), objVarMapper.ToObject<ARG_TYPE_1> (vvs[1]), objVarMapper.ToObject<ARG_TYPE_2> (vvs[2]), objVarMapper.ToObject<ARG_TYPE_3> (vvs[3])));
    }
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
    // WORKAROUND FACT I CANNOT GET VARIADIC TEMPLATES WORKING...
    template <typename RETURN_TYPE, typename ARG_TYPE_COMBINED>
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<RETURN_TYPE (ARG_TYPE_COMBINED)>& f)
    {
        using namespace Containers;
        return [=](WebServer::Message* m) {
            ExpectedMethod (m->GetRequestReference (), webServiceDescription);
            VariantValue allArgsAsVariantValue = GetWebServiceArgsAsVariantValue (m->PeekRequest (), {});
            CallFAndWriteConvertedResponse (m->PeekResponse (), webServiceDescription, objVarMapper, f, objVarMapper.ToObject<ARG_TYPE_COMBINED> (allArgsAsVariantValue));
        };
    }
    template <typename RETURN_TYPE, typename ARG_TYPE_0>
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (ARG_TYPE_0)>& f)
    {
        using namespace Containers;
        Require (paramNames.size () == 1);
        return [=](WebServer::Message* m) {
            ExpectedMethod (m->GetRequestReference (), webServiceDescription);
            VariantValue                  allArgsAsVariantValue = GetWebServiceArgsAsVariantValue (m->PeekRequest (), {});
            Mapping<String, VariantValue> allArgsMap            = allArgsAsVariantValue.As<Mapping<String, VariantValue>> ();
            Sequence<VariantValue>        vvs;
            for (auto i : paramNames) {
                vvs += allArgsMap.LookupValue (i);
            }
            CallFAndWriteConvertedResponse (m->PeekResponse (), webServiceDescription, objVarMapper, f, objVarMapper.ToObject<ARG_TYPE_0> (vvs[0]));
        };
    }
    template <typename RETURN_TYPE, typename ARG_TYPE_0, typename ARG_TYPE_1>
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (ARG_TYPE_0, ARG_TYPE_1)>& f)
    {
        using namespace Containers;
        Require (paramNames.size () == 2);
        return [=](WebServer::Message* m) {
            ExpectedMethod (m->GetRequestReference (), webServiceDescription);
            VariantValue                  allArgsAsVariantValue = GetWebServiceArgsAsVariantValue (m->PeekRequest (), {});
            Mapping<String, VariantValue> allArgsMap            = allArgsAsVariantValue.As<Mapping<String, VariantValue>> ();
            Sequence<VariantValue>        vvs;
            for (auto i : paramNames) {
                vvs += allArgsMap.LookupValue (i);
            }
            CallFAndWriteConvertedResponse (m->PeekResponse (), webServiceDescription, objVarMapper, f, objVarMapper.ToObject<ARG_TYPE_0> (vvs[0]), objVarMapper.ToObject<ARG_TYPE_1> (vvs[1]));
        };
    }
    template <typename RETURN_TYPE, typename ARG_TYPE_0, typename ARG_TYPE_1, typename ARG_TYPE_2>
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (ARG_TYPE_0, ARG_TYPE_1, ARG_TYPE_2)>& f)
    {
        using namespace Containers;
        Require (paramNames.size () == 3);
        return [=](WebServer::Message* m) {
            ExpectedMethod (m->GetRequestReference (), webServiceDescription);
            VariantValue                  allArgsAsVariantValue = GetWebServiceArgsAsVariantValue (m->PeekRequest (), {});
            Mapping<String, VariantValue> allArgsMap            = allArgsAsVariantValue.As<Mapping<String, VariantValue>> ();
            Sequence<VariantValue>        vvs;
            for (auto i : paramNames) {
                vvs += allArgsMap.LookupValue (i);
            }
            CallFAndWriteConvertedResponse (m->PeekResponse (), webServiceDescription, objVarMapper, f, objVarMapper.ToObject<ARG_TYPE_0> (vvs[0]), objVarMapper.ToObject<ARG_TYPE_1> (vvs[1]), objVarMapper.ToObject<ARG_TYPE_2> (vvs[2]));
        };
    }
    template <typename RETURN_TYPE, typename ARG_TYPE_0, typename ARG_TYPE_1, typename ARG_TYPE_2, typename ARG_TYPE_3>
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const Traversal::Iterable<String>& paramNames, const function<RETURN_TYPE (ARG_TYPE_0, ARG_TYPE_1, ARG_TYPE_2, ARG_TYPE_3)>& f)
    {
        using namespace Containers;
        Require (paramNames.size () == 4);
        return [=](WebServer::Message* m) {
            ExpectedMethod (m->GetRequestReference (), webServiceDescription);
            VariantValue                  allArgsAsVariantValue = GetWebServiceArgsAsVariantValue (m->PeekRequest (), {});
            Mapping<String, VariantValue> allArgsMap            = allArgsAsVariantValue.As<Mapping<String, VariantValue>> ();
            Sequence<VariantValue>        vvs;
            for (auto i : paramNames) {
                vvs += allArgsMap.LookupValue (i);
            }
            CallFAndWriteConvertedResponse (m->PeekResponse (), webServiceDescription, objVarMapper, f, objVarMapper.ToObject<ARG_TYPE_0> (vvs[0]), objVarMapper.ToObject<ARG_TYPE_1> (vvs[1]), objVarMapper.ToObject<ARG_TYPE_2> (vvs[2]), objVarMapper.ToObject<ARG_TYPE_3> (vvs[3]));
        };
    }

#if 0
    template <typename RETURN_TYPE, typename IN_ARGS>
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<RETURN_TYPE (IN_ARGS)>& f)
    {
        return [=](WebServer::Message* m) {
            ExpectedMethod (m->GetRequestReference (), webServiceDescription);
            WriteResponse (m->PeekResponse (), webServiceDescription, objVarMapper.FromObject (f (objVarMapper.ToObject<IN_ARGS> (GetWebServiceArgsAsVariantValue (m->PeekRequest (), {})))));
        };
    }
#endif
    template <typename RETURN_TYPE>
    WebServer::RequestHandler mkRequestHandler (const WebServiceMethodDescription& webServiceDescription, const DataExchange::ObjectVariantMapper& objVarMapper, const function<RETURN_TYPE (void)>& f)
    {
        return [=](WebServer::Message* m) {
            ExpectedMethod (m->GetRequestReference (), webServiceDescription);
            CallFAndWriteConvertedResponse (m->PeekResponse (), webServiceDescription, objVarMapper, f);
        };
    }

    ////////////////DEPRECATED STUFF
    inline [[deprecated ("since Stroika v2.1d13 - use one arg version")]] Mapping<String, VariantValue> PickoutParamValuesFromURL (Request* request, const optional<Iterable<String>>& namedParameters)
    {
        Assert (not namedParameters.has_value ()); // cuz never supported - you can manually call retainall if you want
        return PickoutParamValuesFromURL (request);
    }
    inline [[deprecated ("since Stroika v2.1d13 - use one arg version")]] Mapping<String, VariantValue> PickoutParamValuesFromURL (const URL& url, const optional<Iterable<String>>& namedParameters)
    {
        Assert (not namedParameters.has_value ()); // cuz never supported - you can manually call retainall if you want
        return PickoutParamValuesFromURL (url);
    }
    inline [[deprecated ("since Stroika v2.1d13 - use one arg version")]] Mapping<String, VariantValue> PickoutParamValuesFromBody (Request* request, const optional<Iterable<String>>& namedParameters)
    {
        Assert (not namedParameters.has_value ()); // cuz never supported - you can manually call retainall if you want
        return PickoutParamValuesFromBody (request);
    }
    inline [[deprecated ("since Stroika v2.1d13 - use one arg version")]] Mapping<String, VariantValue> PickoutParamValuesFromBody (const BLOB& body, const optional<InternetMediaType>& bodyContentType, const optional<Iterable<String>>& namedParameters)
    {
        Assert (not namedParameters.has_value ()); // cuz never supported - you can manually call retainall if you want
        return PickoutParamValuesFromBody (body, bodyContentType);
    }
    inline [[deprecated ("since Stroika v2.1d13 - use one arg version")]] Mapping<String, VariantValue> PickoutParamValues (Request* request, const optional<Iterable<String>>& namedURLParams, const optional<Iterable<String>>& namedBodyParams)
    {
        Assert (not namedURLParams.has_value ());  // cuz never supported - you can manually call retainall if you want
        Assert (not namedBodyParams.has_value ()); // cuz never supported - you can manually call retainall if you want
        return PickoutParamValues (request);
    }

}
#endif /*_Stroika_Framework_WebService_Server_ObjectVariantMapper_inl_*/
