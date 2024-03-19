/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_WebService_Server_Basic_h_
#define _Stroika_Framework_WebService_Server_Basic_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Frameworks/WebServer/Request.h"
#include "Stroika/Frameworks/WebServer/RequestHandler.h"
#include "Stroika/Frameworks/WebServer/Response.h"

#include "../Basic.h"
#include "../OpenAPI/Specification.h"

/*
 */

/*
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 *
 *       SUPER DUPER ROUGH DRAFT
 */

namespace Stroika::Frameworks::WebService::Server {

    using namespace Stroika::Foundation;

    using Characters::String;
    using Containers::Mapping;
    using Containers::Sequence;
    using Containers::Set;
    using Traversal::Iterable;

    using WebServer::Request;
    using WebServer::Response;

    /**
     */
    void ExpectedMethod (const Request& request, const Iterable<String>& methods, const optional<String>& fromInMessage = nullopt);
    void ExpectedMethod (const Request& request, const WebServiceMethodDescription& wsMethodDescription);

    /**
     */
    struct DocsOptions {
        String                           fH1Text           = "Operations";
        String                           fIntroductoryText = ""; // div section of introductory text about operations
        Mapping<String, String>          fVariables2Substitute;
        String                           fCSSSection = kDefaultCSSSection;
        static const String              kDefaultCSSSection;
        optional<OpenAPI::Specification> fOpenAPISpecification;
        optional<IO::Network::URI>       fOpenAPISpecificationURI;
    };

    /**
     */
    void WriteDocsPage (Response* response, const Sequence<WebServiceMethodDescription>& operations, const DocsOptions& docsOptions = {});

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Basic.inl"

#endif /*_Stroika_Framework_WebService_Server_Basic_h_*/
