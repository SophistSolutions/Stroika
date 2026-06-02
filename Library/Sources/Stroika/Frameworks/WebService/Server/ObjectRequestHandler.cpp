/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/IO/Network/HTTP/ClientErrorException.h"

#include "ObjectRequestHandler.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebService;
using namespace Stroika::Frameworks::WebService::Server;
using namespace Stroika::Frameworks::WebService::Server::ObjectRequestHandler;

using IO::Network::HTTP::ClientErrorException;

/*
 ********************************************************************************
 *********************** ObjectRequestHandler::Context **************************
 ********************************************************************************
 */
String ObjectRequestHandler::Context::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "matchedURLArgs: "sv << fMatchedURLArgs << ","sv;
    sb << "request: "sv << fRequest << ","sv;
    sb << "response: "sv << fResponse;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 *********************** ObjectRequestHandler::Options **************************
 ********************************************************************************
 */
String ObjectRequestHandler::Options::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    //sb << "objectMapper: "sv << fObjectMapper ;
    sb << "defaultResultMediaType: "sv << fDefaultResultMediaType;
    sb << ", allowedMethods: "sv << fAllowedMethods;
    // sb << ", extractVariantValueFromRequest: "sv << fExtractVariantValueFromRequest ;
    sb << ", treatBodyAsListOfArguments: "sv << fTreatBodyAsListOfArguments;
    //sb << ", JSONWriterOptions: "sv << fJSONWriterOptions;
    sb << "}"sv;
    return sb;
}
