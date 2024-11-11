/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Writer.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/IO/Network/HTTP/ClientErrorException.h"

#include "Stroika/Frameworks/WebService/Server/Basic.h"

namespace Stroika::Frameworks::WebService::Server::ObjectRequestHandler {

    /*
     ********************************************************************************
     ************************ ObjectRequestHandler::Factory *************************
     ********************************************************************************
     */
    template <typename RETURN_TYPE, typename... ARG_TYPES>
    template <typename /*invocable<ARG_TYPES...>*/ CALLBACK_FUNCTION>
    inline Factory<RETURN_TYPE, ARG_TYPES...>::Factory (const Options& options, CALLBACK_FUNCTION&& highLevelHandler)
        : fHighLevelHandler_{highLevelHandler}
        , fOptions_{options}
    {
    }
    template <typename RETURN_TYPE, typename... ARG_TYPES>
    inline Factory<RETURN_TYPE, ARG_TYPES...>::operator Frameworks::WebServer::RequestHandler () const
    {
        using namespace Characters::Literals;
        using namespace DataExchange;
        using WebServer::Message;
        return [*this] (Message* m, [[maybe_unused]] const Sequence<String>& matchedArgs) {
            Debug::TraceContextBumper ctx{
                Stroika_Foundation_Debug_OptionalizeTraceArgs ("ObjectRequestHandler::Factory handler", "m->request = {}, RETURN_TYPE={}"_f,
                                                               m->request ().ToString (), type_index{typeid (RETURN_TYPE)})};
            Request&  req  = m->rwRequest ();
            Response& resp = m->rwResponse ();
            Context   context{.fMatchedURLArgs = matchedArgs, .fRequest = req, .fResponse = resp};
            if constexpr (same_as<RETURN_TYPE, void>) {
                ApplyHandler (context);
                SendResponse (req, resp);
            }
            else {
                SendResponse (req, resp, ApplyHandler (context));
            }
        };
    }
    template <typename RETURN_TYPE, typename... ARG_TYPES>
    template <typename RET>
    inline tuple<> Factory<RETURN_TYPE, ARG_TYPES...>::mkArgsTuple_ ([[maybe_unused]] const Context&                context,
                                                                     [[maybe_unused]] const Iterable<VariantValue>& variantValueArgs,
                                                                     [[maybe_unused]] const function<RET ()>&       f) const
    {
        Require (variantValueArgs.size () == 0);
        return make_tuple ();
    }
    template <typename RETURN_TYPE, typename... ARG_TYPES>
    template <typename SINGLE_ARG>
    tuple<SINGLE_ARG> Factory<RETURN_TYPE, ARG_TYPES...>::mkArgsTuple_ (const Context&                                 context,
                                                                        [[maybe_unused]] const Iterable<VariantValue>& variantValueArgs,
                                                                        [[maybe_unused]] const function<RETURN_TYPE (SINGLE_ARG)>& f) const
    {
        if constexpr (same_as<remove_cvref_t<SINGLE_ARG>, Context>) {
            Require (variantValueArgs.size () == 0);
            return make_tuple (context);
        }
        else {
            Require (variantValueArgs.size () == 1);
            return make_tuple (ConvertArg2Object<SINGLE_ARG> (variantValueArgs.Nth (0)));
        }
    }
    template <typename RETURN_TYPE, typename... ARG_TYPES>
    template <typename ARG_FIRST, typename... REST_ARG_TYPES>
    auto Factory<RETURN_TYPE, ARG_TYPES...>::mkArgsTuple_ (const Context& context, const Iterable<VariantValue>& variantValueArgs,
                                                           [[maybe_unused]] const function<RETURN_TYPE (ARG_FIRST, REST_ARG_TYPES...)>& f) const
        -> decltype (tuple_cat (make_tuple (declval<remove_cvref_t<ARG_FIRST>> ()), make_tuple (declval<REST_ARG_TYPES...> ())))
    {
        constexpr size_t kTotalArgsRemaining_ = sizeof...(REST_ARG_TYPES) + 1; // +1 cuz still processing ARG_FIRST here
        if constexpr (same_as<remove_cvref_t<ARG_FIRST>, Context>) {
            Require (variantValueArgs.size () == kTotalArgsRemaining_ - 1); // one arg is context, and rest or from variantArgs
            return tuple_cat (mkArgsTuple_ (context, Iterable<VariantValue>{}, function<RETURN_TYPE (ARG_FIRST)>{}),
                              mkArgsTuple_ (context, variantValueArgs, function<RETURN_TYPE (REST_ARG_TYPES...)>{}));
        }
        else {
            Require (variantValueArgs.size () == kTotalArgsRemaining_ or variantValueArgs.size () == kTotalArgsRemaining_ - 1); // need enuf remaining variantargs,except maybe one remaining still a context arg
            return tuple_cat (mkArgsTuple_ (context, variantValueArgs.Take (1), function<RETURN_TYPE (ARG_FIRST)>{}),
                              mkArgsTuple_ (context, variantValueArgs.Skip (1), function<RETURN_TYPE (REST_ARG_TYPES...)>{}));
        }
    }
    template <typename RETURN_TYPE, typename... ARG_TYPES>
    RETURN_TYPE Factory<RETURN_TYPE, ARG_TYPES...>::ApplyHandler (const Context& context) const
    {
        using IO::Network::HTTP::ClientErrorException;
        if (fOptions_.fTreatBodyAsListOfArguments) {
            VariantValue           argVV            = fOptions_.fExtractVariantValueFromRequest (context.fRequest);
            Iterable<VariantValue> variantValueArgs = PickOutNamedArguments (*fOptions_.fTreatBodyAsListOfArguments, argVV);
            Require (variantValueArgs.size () == sizeof...(ARG_TYPES));
            // exceptions parsing args mean ill-formatted arguments to the webservice, so treat as client errors
            auto&& args = ClientErrorException::TreatExceptionsAsClientError (
                [&, this] () { return mkArgsTuple_ (context, variantValueArgs, fHighLevelHandler_); });
            if constexpr (same_as<RETURN_TYPE, void>) {
                apply (fHighLevelHandler_, args);
            }
            else {
                return apply (fHighLevelHandler_, args);
            }
        }
        else {
            if constexpr (sizeof...(ARG_TYPES) == 0) {
                if constexpr (same_as<RETURN_TYPE, void>) {
                    fHighLevelHandler_ ();
                }
                else {
                    return fHighLevelHandler_ ();
                }
            }
            else if constexpr (sizeof...(ARG_TYPES) == 1) {
                using firstArgType = std::tuple_element_t<0, std::tuple<ARG_TYPES...>>;
                if constexpr (same_as<firstArgType, Context>) {
                    if constexpr (same_as<RETURN_TYPE, void>) {
                        fHighLevelHandler_ (context);
                    }
                    else {
                        return fHighLevelHandler_ (context);
                    }
                }
                else {
                    VariantValue argVV = fOptions_.fExtractVariantValueFromRequest (context.fRequest);
                    if constexpr (same_as<RETURN_TYPE, void>) {
                        fHighLevelHandler_ (ConvertArg2Object<firstArgType> (argVV));
                    }
                    else {
                        return fHighLevelHandler_ (ConvertArg2Object<firstArgType> (argVV));
                    }
                }
            }
            else if constexpr (sizeof...(ARG_TYPES) == 2) {
                VariantValue argVV  = fOptions_.fExtractVariantValueFromRequest (context.fRequest);
                using firstArgType  = std::tuple_element_t<0, std::tuple<ARG_TYPES...>>;
                using secondArgType = std::tuple_element_t<1, std::tuple<ARG_TYPES...>>;
                if constexpr (same_as<firstArgType, Context>) {
                    if constexpr (same_as<RETURN_TYPE, void>) {
                        fHighLevelHandler_ (context, ConvertArg2Object<secondArgType> (argVV));
                    }
                    else {
                        return fHighLevelHandler_ (context, ConvertArg2Object<secondArgType> (argVV));
                    }
                }
                else {
                    static_assert (same_as<secondArgType, Context>);
                    if constexpr (same_as<RETURN_TYPE, void>) {
                        fHighLevelHandler_ (ConvertArg2Object<firstArgType> (argVV), context);
                    }
                    else {
                        return fHighLevelHandler_ (ConvertArg2Object<firstArgType> (argVV), context);
                    }
                }
            }
            else {
                static_assert (sizeof...(ARG_TYPES) <= 2);
                AssertNotReached ();
            }
        }
    }
    template <typename RETURN_TYPE, typename... ARG_TYPES>
    inline RETURN_TYPE Factory<RETURN_TYPE, ARG_TYPES...>::ApplyObjectHandler (ARG_TYPES... args) const
    {
        if constexpr (same_as<RETURN_TYPE, void>) {
            fHighLevelHandler_ (args...);
        }
        else {
            return fHighLevelHandler_ (args...);
        }
    }
    template <typename RETURN_TYPE, typename... ARG_TYPES>
    template <typename T>
    inline T Factory<RETURN_TYPE, ARG_TYPES...>::ConvertArg2Object (const VariantValue& v) const
    {
        return fOptions_.fObjectMapper.ToObject<T> (v);
    }
    template <typename RETURN_TYPE, typename... ARG_TYPES>
    template <same_as<RETURN_TYPE> RT>
    // note maybe_unused on request wrong but tmphack to quiet til we check accept headers
    inline void Factory<RETURN_TYPE, ARG_TYPES...>::SendResponse ([[maybe_unused]] const Request& request, Response& response, const RT& r) const
    {
        using namespace DataExchange;
        // @todo check accepts content type - and convert result (to JSON or binary json, xml etc)
        if constexpr (Common::IAnyOf<RETURN_TYPE, String, DataExchange::VariantValue>) {
            if constexpr (same_as<RETURN_TYPE, String>) {
                response.contentType = fOptions_.fDefaultResultMediaType.value_or (InternetMediaTypes::kText_PLAIN);
            }
            else {
                response.contentType = fOptions_.fDefaultResultMediaType.value_or (InternetMediaTypes::kJSON);
            }
            response.write (r);
        }
        else {
            response.contentType = fOptions_.fDefaultResultMediaType.value_or (InternetMediaTypes::kJSON);
            response.write (Variant::JSON::Writer{}.WriteAsString (fOptions_.fObjectMapper.FromObject (r)));
        }
    }
    template <typename RETURN_TYPE, typename... ARG_TYPES>
    inline void Factory<RETURN_TYPE, ARG_TYPES...>::SendResponse ([[maybe_unused]] const Request& request, [[maybe_unused]] Response& response) const
        requires (same_as<RETURN_TYPE, void>)
    {
        // @todo - not sure anything todo here???
    }

}
