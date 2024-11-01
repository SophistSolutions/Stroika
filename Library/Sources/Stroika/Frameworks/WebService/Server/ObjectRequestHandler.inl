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
        requires (INCLUDE_CONTEXT and not same_as<remove_cvref_t<typename FunctionTraits<CALLBACK_FUNCTION>::template arg<0>::type>, void>)
    Factory<RETURN_TYPE, WEB_METHOD_ARG, INCLUDE_CONTEXT>::Factory (const ObjectVariantMapper& ovm, CALLBACK_FUNCTION&& highLevelHandler,
                                                                    const Options& options)
        : fObjectVariantMapper_{ovm}
        , fHighLevelHandler_{std::function<typename FunctionTraits<CALLBACK_FUNCTION>::result_type (
              remove_cvref_t<typename FunctionTraits<CALLBACK_FUNCTION>::template arg<0>::type>, Context)>{forward<CALLBACK_FUNCTION> (highLevelHandler)}}
        , fOptions_{options}
    {
    }
    template <typename RETURN_TYPE, typename WEB_METHOD_ARG, bool INCLUDE_CONTEXT>
    Factory<RETURN_TYPE, WEB_METHOD_ARG, INCLUDE_CONTEXT>::Factory (const ObjectVariantMapper& ovm,
                                                                    function<RETURN_TYPE (WEB_METHOD_ARG)> highLevelHandler, const Options& options)
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
            Debug::TraceContextBumper ctx{"Factory handler", "m->request = {}"_f, m->request ().ToString ()};
            DbgTrace ("RETURN_TYPE={}"_f, type_index{typeid (RETURN_TYPE)});
            DbgTrace ("WEB_METHOD_ARG={}"_f, type_index{typeid (WEB_METHOD_ARG)});
            DbgTrace ("INCLUDE_CONTEXT={}"_f, INCLUDE_CONTEXT);
            RETURN_TYPE r = [&] () {
                if constexpr (INCLUDE_CONTEXT) {
                    return this->ApplyHandler (Context{.fMatchedURLArgs = matchedArgs, .fRequest = m->rwRequest (), .fResponse = m->rwResponse ()});
                }
                else {
                    return this->ApplyHandler (m->rwRequest ());
                }
            }();
            if constexpr (Common::IAnyOf<RETURN_TYPE, String, DataExchange::VariantValue>) {
                // @todo check accepts content type - and convert result (to JSON or binary json etc)
                if constexpr (same_as<RETURN_TYPE, String>) {
                    m->rwResponse ().contentType = fOptions_.fDefaultResultMediaType.value_or (InternetMediaTypes::kText_PLAIN);
                }
                else {
                    m->rwResponse ().contentType = fOptions_.fDefaultResultMediaType.value_or (InternetMediaTypes::kJSON);
                }
                m->rwResponse ().write (r);
            }
            else {
                m->rwResponse ().contentType = fOptions_.fDefaultResultMediaType.value_or (InternetMediaTypes::kJSON);
                m->rwResponse ().write (Variant::JSON::Writer{}.WriteAsString (fObjectVariantMapper_.FromObject (r)));
            }
        };
    }
    template <typename RETURN_TYPE, typename WEB_METHOD_ARG, bool INCLUDE_CONTEXT>
    RETURN_TYPE Factory<RETURN_TYPE, WEB_METHOD_ARG, INCLUDE_CONTEXT>::ApplyHandler (Request* req) const
        requires (not INCLUDE_CONTEXT)
    {
        if constexpr (same_as<WEB_METHOD_ARG, void>) {
            return fHighLevelHandler_ ();
        }
        else {
            WEB_METHOD_ARG arg{fObjectVariantMapper_.ToObject<WEB_METHOD_ARG> (req->GetBodyVariantValue ())};
            return fHighLevelHandler_ (arg);
        }
    }
    template <typename RETURN_TYPE, typename WEB_METHOD_ARG, bool INCLUDE_CONTEXT>
    RETURN_TYPE Factory<RETURN_TYPE, WEB_METHOD_ARG, INCLUDE_CONTEXT>::ApplyHandler (const Context& c) const
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

}
