/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Writer.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "Stroika/Frameworks/WebService/Server/Basic.h"

namespace Stroika::Frameworks::WebService::Server::ObjectRequestHandler {

    /*
     ********************************************************************************
     *********************** ObjectRequestHandler::Factory **************************
     ********************************************************************************
     */
    template <typename RETURN_TYPE, typename WEB_METHOD_ARG, bool INCLUDE_CONTEXT>
    Factory<RETURN_TYPE, WEB_METHOD_ARG, INCLUDE_CONTEXT>::Factory (const ObjectVariantMapper& ovm,
                                                                    function<RETURN_TYPE (Context)> highLevelHandler, const Options& options)
        : fObjectVariantMapper_{ovm}
        , fHighLevelHandler_{highLevelHandler}
        , fOptions_{options}
    {
    }
    template <typename RETURN_TYPE, typename WEB_METHOD_ARG, bool INCLUDE_CONTEXT>
    template <Private_::IsFunctionOfOneArgPlusContext_ CALLBACK_FUNCTION>
        requires (INCLUDE_CONTEXT)
    Factory<RETURN_TYPE, WEB_METHOD_ARG, INCLUDE_CONTEXT>::Factory (const ObjectVariantMapper& ovm, CALLBACK_FUNCTION&& highLevelHandler,
                                                                    const Options& options)
        : fObjectVariantMapper_{ovm}
        , fHighLevelHandler_{std::function<typename FunctionTraits<CALLBACK_FUNCTION>::result_type (
              remove_cvref_t<typename FunctionTraits<CALLBACK_FUNCTION>::template arg<0>::type>, Context)>{forward<CALLBACK_FUNCTION> (highLevelHandler)}}
        , fOptions_{options}
    {
    }
    template <typename RETURN_TYPE, typename WEB_METHOD_ARG, bool INCLUDE_CONTEXT>
    template <Private_::IsFunctionOfOneArgNoContext_ CALLBACK_FUNCTION>
    Factory<RETURN_TYPE, WEB_METHOD_ARG, INCLUDE_CONTEXT>::Factory (const ObjectVariantMapper& ovm,
                                                                    CALLBACK_FUNCTION&& highLevelHandler, const Options& options)
        requires (not INCLUDE_CONTEXT)
        : fObjectVariantMapper_{ovm}
        , fHighLevelHandler_{highLevelHandler}
        , fOptions_{options}
    {
    }
    template <typename RETURN_TYPE, typename WEB_METHOD_ARG, bool INCLUDE_CONTEXT>
    Factory<RETURN_TYPE, WEB_METHOD_ARG, INCLUDE_CONTEXT>::operator Frameworks::WebServer::RequestHandler () const
    {
        using namespace Characters::Literals;
        using namespace DataExchange;
        using WebServer::Message;
        return [*this] (Message* m, [[maybe_unused]] const Sequence<String>& matchedArgs) {
            Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (
                "ObjectRequestHandler::Factory handler", "m->request = {}, RETURN_TYPE={}, WEB_METHOD_ARG={}, INCLUDE_CONTEXT={}"_f,
                m->request ().ToString (), type_index{typeid (RETURN_TYPE)}, type_index{typeid (WEB_METHOD_ARG)}, INCLUDE_CONTEXT)};
            Request&                  req  = m->rwRequest ();
            Response&                 resp = m->rwResponse ();
            if constexpr (same_as<RETURN_TYPE, void>) {
                if constexpr (INCLUDE_CONTEXT) {
                    ApplyHandler (Context{.fMatchedURLArgs = matchedArgs, .fRequest = req, .fResponse = resp});
                }
                else {
                    ApplyHandler (req);
                }
                SendResponse (req, resp);
            }
            else {
                RETURN_TYPE r = [&] () {
                    if constexpr (INCLUDE_CONTEXT) {
                        return ApplyHandler (Context{.fMatchedURLArgs = matchedArgs, .fRequest = req, .fResponse = resp});
                    }
                    else {
                        return ApplyHandler (req);
                    }
                }();
                SendResponse (req, resp, r);
            }
        };
    }
    template <typename RETURN_TYPE, typename WEB_METHOD_ARG, bool INCLUDE_CONTEXT>
    inline RETURN_TYPE Factory<RETURN_TYPE, WEB_METHOD_ARG, INCLUDE_CONTEXT>::ApplyHandler (Request& req) const
        requires (not INCLUDE_CONTEXT)
    {
        if constexpr (same_as<WEB_METHOD_ARG, void>) {
            return fHighLevelHandler_ ();
        }
        else {
            WEB_METHOD_ARG arg{fObjectVariantMapper_.ToObject<WEB_METHOD_ARG> (req.GetBodyVariantValue ())};
            return fHighLevelHandler_ (arg);
        }
    }
    template <typename RETURN_TYPE, typename WEB_METHOD_ARG, bool INCLUDE_CONTEXT>
    inline RETURN_TYPE Factory<RETURN_TYPE, WEB_METHOD_ARG, INCLUDE_CONTEXT>::ApplyHandler (const Context& c) const
        requires (INCLUDE_CONTEXT)
    {
        if constexpr (same_as<WEB_METHOD_ARG, void>) {
            return fHighLevelHandler_ (c);
        }
        else {
            WEB_METHOD_ARG arg{fObjectVariantMapper_.ToObject<WEB_METHOD_ARG> (c.fRequest.GetBodyVariantValue ())};
            return fHighLevelHandler_ (arg, c);
        }
    }
    template <typename RETURN_TYPE, typename WEB_METHOD_ARG, bool INCLUDE_CONTEXT>
    inline void Factory<RETURN_TYPE, WEB_METHOD_ARG, INCLUDE_CONTEXT>::SendResponse ([[maybe_unused]] const Request& request,
                                                                                     [[maybe_unused]] Response&      response) const
        requires (same_as<RETURN_TYPE, void>)
    {
        // @todo - set status? Maybe no need - just do nothing??? and OK
    }
    template <typename RETURN_TYPE, typename WEB_METHOD_ARG, bool INCLUDE_CONTEXT>
    // note maybe_unused on request wrong but tmphack to quiet til we check accept headers
    inline void Factory<RETURN_TYPE, WEB_METHOD_ARG, INCLUDE_CONTEXT>::SendResponse ([[maybe_unused]] const Request& request,
                                                                                     Response& response, const RETURN_TYPE& r) const
        requires (not same_as<RETURN_TYPE, void>)
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
            response.write (Variant::JSON::Writer{}.WriteAsString (fObjectVariantMapper_.FromObject (r)));
        }
    }

}
