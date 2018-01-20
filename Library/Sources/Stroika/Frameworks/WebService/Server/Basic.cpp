/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../../Foundation/Characters/StringBuilder.h"
#include "../../../Foundation/Characters/String_Constant.h"
#include "../../../Foundation/Characters/ToString.h"
#include "../../../Foundation/Containers/Mapping.h"
#include "../../../Foundation/DataExchange/Variant/JSON/Reader.h"
#include "../../../Foundation/DataExchange/Variant/JSON/Writer.h"

#include "Basic.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebService;

using Characters::String_Constant;
using Characters::StringBuilder;

/*
 ********************************************************************************
 **************************** WebService::Server::ExpectedMethod ****************
 ********************************************************************************
 */
void WebService::Server::ExpectedMethod (const Request* request, const Set<String>& methods, const Optional<String>& fromInMessage)
{
    String method{request->GetHTTPMethod ()};
    if (not methods.Contains (method)) {
        Execution::Throw (Execution::StringException (
            String_Constant{L"Expected HTTP method "} + Characters::ToString (methods) +
            (fromInMessage ? (L" from " + *fromInMessage) : L"")));
    }
}

void WebService::Server::ExpectedMethod (const Request* request, const WebServiceMethodDescription& wsMethodDescription)
{
    ExpectedMethod (request, wsMethodDescription.fAllowedMethods, wsMethodDescription.fOperation);
}

/*
 ********************************************************************************
 ************************ WebService::Server::WriteDocsPage *********************
 ********************************************************************************
 */
const String WebService::Server::DocsOptions::kDefaultCSSSection =
    L"div.mainDocs {margin-left: .3in; margin-right: .3in; }"
    L"div.mainDocs div { padding-top: 6pt; padding-bottom: 6pt; }"
    L"div.curlExample {margin-left: .3in; margin-top: .1in; margin-bottom:.1in; font-family: \"Courier New\", Courier, \"Lucida Sans Typewriter\", \"Lucida Typewriter\", monospace; font-size: 9pt; font-weight: bold;}"
    L"div.curlExample div { padding-top: 2pt; padding-bottom: 2pt; }";

void WebService::Server::WriteDocsPage (Response* response, const Sequence<WebServiceMethodDescription>& operations, const DocsOptions& docsOptions)
{
    response->writeln (L"<html>");
    response->writeln (L"<style type=\"text/css\">");
    response->writeln (docsOptions.fCSSSection);
    response->writeln (L"</style>");
    response->writeln (L"<body>");
    response->printf (L"<h1>%s</h1>", docsOptions.fH1Text.c_str ());
    response->writeln (L"<ul>");
    auto substVars = [=](const String& origStr) {
        String str = origStr;
        for (auto i : docsOptions.fVariables2Substitute) {
            str = str.ReplaceAll (L"{{" + i.fKey + L"}}", i.fValue);
        }
        return str;
    };
    auto writeDocs = [=](const String& methodName, const String& docs, const String& exampleCall) {
        response->writeln (L"<li>");
        response->printf (L"<a href=\"/%s\">%s</a>", methodName.c_str (), methodName.c_str ());
        response->printf (L"<div class='mainDocs'>%s</div>", docs.c_str ());
        response->printf (L"<div class='curlExample'>%s</div>", exampleCall.c_str ());
        response->writeln (L"</li>");
    };
    for (WebServiceMethodDescription i : operations) {
        StringBuilder tmpDocs;
        if (i.fDetailedDocs) {
            i.fDetailedDocs->Apply ([&](const String& i) { tmpDocs += L"<div>" + substVars (i) + L"</div>"; });
        }
        StringBuilder tmpCurl;
        if (i.fCurlExample) {
            i.fCurlExample->Apply ([&](const String& i) { tmpCurl += L"<div>" + substVars (i) + L"</div>"; });
        }
        writeDocs (i.fOperation, tmpDocs.c_str (), tmpCurl.c_str ());
    }
    response->writeln (L"</ul>");
    response->writeln (L"</body>");
    response->writeln (L"</html>");
    response->SetContentType (DataExchange::PredefinedInternetMediaType::Text_HTML_CT ());
}
