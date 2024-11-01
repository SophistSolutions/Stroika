/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "ObjectRequestHandler.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebService;
using namespace Stroika::Frameworks::WebService::Server;

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
    sb << "Response: "sv << fResponse << ","sv;
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
    sb << "DefaultResultMediaType: "sv << fDefaultResultMediaType << ","sv;
    sb << "}"sv;
    return sb;
}
