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
    sb << "MatchedURLArgs: "sv << fMatchedURLArgs << ","sv;
    sb << "Request: "sv << fRequest << ","sv;
    sb << "Response: "sv << fResponse;
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
    //sb << "ObjectMapper: "sv << fObjectMapper ;
    sb << "DefaultResultMediaType: "sv << fDefaultResultMediaType;
    sb << ", AllowedMethods: "sv << fAllowedMethods;
    // sb << ", ExtractVariantValueFromRequest: "sv << fExtractVariantValueFromRequest ;
    sb << ", TreatBodyAsListOfArguments: "sv << fTreatBodyAsListOfArguments;
    //sb << ", JSONWriterOptions: "sv << fJSONWriterOptions;
    sb << "}"sv;
    return sb;
}
