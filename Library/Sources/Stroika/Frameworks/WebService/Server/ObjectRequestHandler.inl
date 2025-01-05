/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
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
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (invocable<ARG_TYPES...>) CALLBACK_FUNCTION>
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
        return [*this] (Message& m, [[maybe_unused]] const Sequence<String>& matchedArgs) {
            Debug::TraceContextBumper ctx{
                Stroika_Foundation_Debug_OptionalizeTraceArgs ("ObjectRequestHandler::Factory handler", "m->request = {}, RETURN_TYPE={}"_f,
                                                               m.request ().ToString (), type_index{typeid (RETURN_TYPE)})};
            Request&  req  = m.rwRequest ();
            Response& resp = m.rwResponse ();
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
        return make_tuple ();
    }
    template <typename RETURN_TYPE, typename... ARG_TYPES>
    template <typename SINGLE_ARG>
    tuple<SINGLE_ARG> Factory<RETURN_TYPE, ARG_TYPES...>::mkArgsTuple_ (const Context&                                 context,
                                                                        [[maybe_unused]] const Iterable<VariantValue>& variantValueArgs,
                                                                        [[maybe_unused]] const function<RETURN_TYPE (SINGLE_ARG)>& f) const
    {
        if constexpr (same_as<remove_cvref_t<SINGLE_ARG>, Context>) {
            return make_tuple (context);
        }
        else {
            Require (variantValueArgs.size () >= 1);
            return make_tuple (ConvertArg2Object<SINGLE_ARG> (variantValueArgs.Nth (0)));
        }
    }
    template <typename RETURN_TYPE, typename... ARG_TYPES>
    template <typename ARG_FIRST, typename... REST_ARG_TYPES>
    auto Factory<RETURN_TYPE, ARG_TYPES...>::mkArgsTuple_ (const Context& context, const Iterable<VariantValue>& variantValueArgs,
                                                           [[maybe_unused]] const function<RETURN_TYPE (ARG_FIRST, REST_ARG_TYPES...)>& f) const
        -> decltype (tuple_cat (make_tuple (declval<remove_cvref_t<ARG_FIRST>> ()), make_tuple (declval<REST_ARG_TYPES...> ())))
    {
        [[maybe_unused]] constexpr size_t kTotalArgsRemaining_ = sizeof...(REST_ARG_TYPES) + 1; // +1 cuz still processing ARG_FIRST here
        Require (variantValueArgs.size () >= kTotalArgsRemaining_);
        if constexpr (same_as<remove_cvref_t<ARG_FIRST>, Context>) {
            return tuple_cat (mkArgsTuple_ (context, Iterable<VariantValue>{}, function<RETURN_TYPE (ARG_FIRST)>{}),
                              mkArgsTuple_ (context, variantValueArgs, function<RETURN_TYPE (REST_ARG_TYPES...)>{}));
        }
        else {
            return tuple_cat (mkArgsTuple_ (context, variantValueArgs.Take (1), function<RETURN_TYPE (ARG_FIRST)>{}),
                              mkArgsTuple_ (context, variantValueArgs.Skip (1), function<RETURN_TYPE (REST_ARG_TYPES...)>{}));
        }
    }
    template <typename RETURN_TYPE, typename... ARG_TYPES>
    RETURN_TYPE Factory<RETURN_TYPE, ARG_TYPES...>::ApplyHandler (const Context& context) const
    {
        using IO::Network::HTTP::ClientErrorException;
        using Server::VariantValue::PickOutNamedArguments;
        if (fOptions_.fAllowedMethods) {
            ExpectedMethod (context.fRequest, *fOptions_.fAllowedMethods);
        }
        Iterable<VariantValue> variantValueArgs = [&] () {
            VariantValue argVV = fOptions_.fExtractVariantValueFromRequest (context.fRequest);
            if (fOptions_.fTreatBodyAsListOfArguments) {
                Iterable<VariantValue> variantValueArgs = PickOutNamedArguments (*fOptions_.fTreatBodyAsListOfArguments, argVV);
                Require (variantValueArgs.size () >= sizeof...(ARG_TYPES) - 1); // assuming one is context, else >= sizeof(argstype)
                return variantValueArgs;
            }
            else {
                Iterable<VariantValue> variantValueArgs{argVV};
                Require (variantValueArgs.size () == 1);
                return variantValueArgs;
            }
        }();

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
    inline void Factory<RETURN_TYPE, ARG_TYPES...>::SendResponse ([[maybe_unused]] const Request& request, Response& response, const RT& r) const
    {
        using namespace DataExchange;
        // note maybe_unused on request wrong but tmphack to quiet til we check accept headers
        if (not response.contentType ().has_value ()) {
            // @todo check accept headers for the default...
            response.contentType = fOptions_.fDefaultResultMediaType.value_or (InternetMediaTypes::kJSON);
        }
        auto ct = Memory::ValueOf (response.contentType ());

        // @todo check accepts content type - and convert result (to JSON or binary json, xml etc)
        VariantValue vv2Write = fOptions_.fObjectMapper.FromObject (r);
        if (InternetMediaTypeRegistry::sThe->IsA (InternetMediaTypes::kJSON, ct)) {
            using Variant::JSON::Writer;
            response.write (Writer{fOptions_.fJSONWriterOptions.value_or (Writer::Options{})}.WriteAsString (vv2Write));
        }
        else if (InternetMediaTypeRegistry::sThe->IsA (InternetMediaTypes::kText_PLAIN, ct)) {
            response.write (vv2Write.As<String> ()); // may throw if cannot convert to String, like accept: text/plain on content that was a map - should throw!
        }
        else {
            RequireNotReached (); // that type not yet supported... - @todo binary json, xml, etc...
        }
    }
    template <typename RETURN_TYPE, typename... ARG_TYPES>
    inline void Factory<RETURN_TYPE, ARG_TYPES...>::SendResponse ([[maybe_unused]] const Request& request, [[maybe_unused]] Response& response) const
        requires (same_as<RETURN_TYPE, void>)
    {
        // @todo - not sure anything todo here???
    }
    template <typename RETURN_TYPE, typename... ARG_TYPES>
    template <same_as<RETURN_TYPE> RT>
    inline void Factory<RETURN_TYPE, ARG_TYPES...>::SendStringResponse ([[maybe_unused]] const Request& request, Response& response, const RT& r) const
    {
        // @todo maybe respect accept headers - to a degree?
        using namespace DataExchange;
        response.contentType = fOptions_.fDefaultResultMediaType.value_or (InternetMediaTypes::kText_PLAIN);
        if constexpr (Characters::IConvertibleToString<RT>) {
            response.write (r);
        }
        else {
            response.write (r.template As<String> ()); // e.g. for Common::GUID, or URL, etc...
        }
    }

}
