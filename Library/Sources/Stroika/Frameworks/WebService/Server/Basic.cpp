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
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebService;

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
    response->writeln ("<html>"sv);
    response->writeln ("<style type=\"text/css\">"sv);
    response->writeln (docsOptions.fCSSSection);
    response->writeln ("</style>"sv);
    response->writeln ("<body>"sv);
    response->write ("<h1>{}</h1>"_f, docsOptions.fH1Text);
    response->write ("<div class='introduction'>{}</div>\n"_f, docsOptions.fIntroductoryText);

    if (docsOptions.fOpenAPISpecificationURI) {
        response->write ("<div class='OpenAPI'>Download <a href={}>OpenAPI File</a></div>\n"_f, *docsOptions.fOpenAPISpecificationURI);
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
        response->writeln ("<li>"sv);
        response->write ("<a href=\"/{}\">{}</a>"_f, methodName, methodName);
        response->write ("<div class='mainDocs'>{}</div>"_f, docs);
        response->write ("<div class='curlExample'>{}</div>"_f, exampleCall);
        response->writeln ("</li>"sv);
    };
    for (const WebServiceMethodDescription& i : operations) {
        StringBuilder tmpDocs;
        if (i.fDetailedDocs) {
            i.fDetailedDocs->Apply ([&] (const String& i) { tmpDocs << "<div>"sv << substVars (i) << "</div>"sv; });
        }
        StringBuilder tmpCurl;
        if (i.fCurlExample) {
            i.fCurlExample->Apply ([&] (const String& i) { tmpCurl << "<div>"sv << substVars (i) << "</div>"sv; });
        }
        writeDocs (i.fOperation, tmpDocs, tmpCurl);
    }
    response->writeln ("</ul>"sv);
    response->writeln ("</body>"sv);
    response->writeln ("</html>"sv);
}
