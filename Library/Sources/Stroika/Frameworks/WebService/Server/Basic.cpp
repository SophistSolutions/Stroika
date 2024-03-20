/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../../Foundation/Characters/Format.h"
#include "../../../Foundation/Characters/StringBuilder.h"
#include "../../../Foundation/Characters/ToString.h"
#include "../../../Foundation/Containers/Mapping.h"
#include "../../../Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "../../../Foundation/DataExchange/Variant/JSON/Reader.h"
#include "../../../Foundation/DataExchange/Variant/JSON/Writer.h"
#include "../../../Foundation/IO/Network/HTTP/ClientErrorException.h"

#include "Basic.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebService;

using Characters::CompareOptions;
using Characters::StringBuilder;
using IO::Network::HTTP::ClientErrorException;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ************************ WebService::Server::ExpectedMethod ********************
 ********************************************************************************
 */
void WebService::Server::ExpectedMethod (const Request& request, const Iterable<String>& methods, const optional<String>& fromInMessage)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (
        L"WebService::Server::ExpectedMethod", L"request=%s, methods=%s, fromInMessage=%s", Characters::ToString (request).c_str (),
        Characters::ToString (methods).c_str (), Characters::ToString (fromInMessage).c_str ())};
#endif
    if (not Set<String> (String::EqualsComparer{CompareOptions::eCaseInsensitive}, methods).Contains (request.httpMethod ())) {
        Execution::Throw (ClientErrorException (
            Characters::Format (L"Received HTTP method '%s'%s, but expected one from %s", request.httpMethod ().As<wstring> ().c_str (),
                                (fromInMessage ? (L" from '" + *fromInMessage + L"'").As<wstring> ().c_str () : L""),
                                Characters::ToString (methods).As<wstring> ().c_str ())));
    }
}

void WebService::Server::ExpectedMethod (const Request& request, const WebServiceMethodDescription& wsMethodDescription)
{
    if (wsMethodDescription.fAllowedMethods) {
        // @todo - dont see why cast to iterable<string> needed on windoze??? Seems a mistake
        ExpectedMethod (request, (Iterable<String>)*wsMethodDescription.fAllowedMethods, wsMethodDescription.fOperation);
    }
}

/*
 ********************************************************************************
 ************************ WebService::Server::WriteDocsPage *********************
 ********************************************************************************
 */
const String WebService::Server::DocsOptions::kDefaultCSSSection =
    "div.mainDocs {margin-left: .3in; margin-right: .3in; }\n"
    "div.mainDocs div { padding-top: 6pt; padding-bottom: 6pt; }\n"
    "div.OpenAPI { padding-top: 12pt; padding-bottom: 12pt; }\n"
    "div.curlExample {margin-left: .3in; margin-top: .1in; margin-bottom:.1in; font-family: \"Courier New\", Courier, \"Lucida Sans "
    "Typewriter\", \"Lucida Typewriter\", monospace; font-size: 9pt; font-weight: bold;}\n"
    "div.curlExample div { padding-top: 2pt; padding-bottom: 2pt; }\n"
    "div.introduction div { padding-top: 2pt; padding-bottom: 2pt; }\n"sv;

void WebService::Server::WriteDocsPage (Response* response, const Sequence<WebServiceMethodDescription>& operations, const DocsOptions& docsOptions)
{
    response->contentType = DataExchange::InternetMediaTypes::kHTML;
    response->writeln (L"<html>");
    response->writeln (L"<style type=\"text/css\">");
    response->writeln (docsOptions.fCSSSection);
    response->writeln (L"</style>");
    response->writeln (L"<body>");
    response->printf (L"<h1>%s</h1>", docsOptions.fH1Text.As<wstring> ().c_str ());
    response->printf (L"<div class='introduction'>%s</div>\n", docsOptions.fIntroductoryText.As<wstring> ().c_str ());

    if (docsOptions.fOpenAPISpecificationURI) {
        response->printf (L"<div class='OpenAPI'>Download <a href=%s>OpenAPI File</a></div>\n",
                          docsOptions.fOpenAPISpecificationURI->As<String> ().As<wstring> ().c_str ());
    }
    response->writeln (L"<ul>");
    auto substVars = [=] (const String& origStr) {
        String str = origStr;
        for (const auto& i : docsOptions.fVariables2Substitute) {
            str = str.ReplaceAll ("{{" + i.fKey + "}}", i.fValue);
        }
        return str;
    };
    auto writeDocs = [=] (const String& methodName, const String& docs, const String& exampleCall) {
        response->writeln (L"<li>");
        response->printf (L"<a href=\"/%s\">%s</a>", methodName.As<wstring> ().c_str (), methodName.As<wstring> ().c_str ());
        response->printf (L"<div class='mainDocs'>%s</div>", docs.As<wstring> ().c_str ());
        response->printf (L"<div class='curlExample'>%s</div>", exampleCall.As<wstring> ().c_str ());
        response->writeln (L"</li>");
    };
    for (const WebServiceMethodDescription& i : operations) {
        StringBuilder tmpDocs;
        if (i.fDetailedDocs) {
            i.fDetailedDocs->Apply ([&] (const String& i) { tmpDocs << "<div>" << substVars (i) << "</div>"; });
        }
        StringBuilder tmpCurl;
        if (i.fCurlExample) {
            i.fCurlExample->Apply ([&] (const String& i) { tmpCurl << "<div>" << substVars (i) << "</div>"; });
        }
        writeDocs (i.fOperation, tmpDocs.str (), tmpCurl.str ());
    }
    response->writeln (L"</ul>");
    response->writeln (L"</body>");
    response->writeln (L"</html>");
}
