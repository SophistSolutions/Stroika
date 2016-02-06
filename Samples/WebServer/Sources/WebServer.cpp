/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "Stroika/Frameworks/StroikaPreComp.h"

#include    <iostream>

#include    "Stroika/Foundation/Execution/CommandLine.h"
#include    "Stroika/Foundation/Execution/SignalHandlers.h"
#include    "Stroika/Foundation/Execution/WaitableEvent.h"
#include    "Stroika/Foundation/IO/Network/HTTP/Exception.h"
#include    "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include    "Stroika/Foundation/IO/Network/LinkMonitor.h"
#include    "Stroika/Foundation/IO/Network/Listener.h"
#include    "Stroika/Foundation/Streams/TextReader.h"

#include    "Stroika/Frameworks/WebServer/ConnectionManager.h"
#include    "Stroika/Frameworks/WebServer/Router.h"

using   namespace std;

using   namespace Stroika::Foundation;
using   namespace Stroika::Foundation::IO::Network;
using   namespace Stroika::Frameworks::WebServer;

using   Characters::String;
using   Memory::BLOB;



/*
 *  To test this example:
 *      o   Run the service (under the debugger if you wish)
 *      o   curl  http://localhost:8080/ OR
 *      o   curl -H "Content-Type: application/json" -X POST -d '{"AppState":"Start"}' http://localhost:8080/SetAppState
 */


#if     !qCompilerAndStdLib_regex_Buggy
namespace {
    void    DefaultPage_ (Request* request, Response* response)
    {
        response->writeln (L"<html><body><p>Hi Mom</p></body></html>");
        response->SetContentType (DataExchange::PredefinedInternetMediaType::Text_HTML_CT ());
    }
    void    SetAppState_ (Request* request, Response* response)
    {
        BLOB    setAppState2    =   request->GetBody ();
        String  interpretAsString = Streams::TextReader (setAppState2).ReadAll ();
        response->writeln (L"<html><body><p>Hi SetAppState (" + interpretAsString.As<wstring> () + L")</p></body></html>");
        response->SetContentType (DataExchange::PredefinedInternetMediaType::Text_HTML_CT ());
    }
    const   Router kRouter_ {
        Sequence<Route> {
            Route { RegularExpression (L"", RegularExpression::SyntaxType::eECMAScript), DefaultPage_ },
            Route { RegularExpression (L"POST", RegularExpression::SyntaxType::eECMAScript), RegularExpression (L"SetAppState", RegularExpression::SyntaxType::eECMAScript), SetAppState_ },
        }
    };
}
#endif

int main (int argc, const char* argv[])
{
    Execution::SignalHandlerRegistry::SafeSignalsManager    safeSignals;
    try {
#if     qCompilerAndStdLib_regex_Buggy
        AssertNotReached ();
#else
        ConnectionManager   cm { SocketAddress (Network::V4::kAddrAny, 8080), kRouter_ } ;  // listen and dispatch while this object exists
        cm.SetServerHeader (String { L"Stroika-Sample-WebServer" });
        Execution::WaitableEvent (Execution::WaitableEvent::eAutoReset).Wait ();    // wait forever - til user hits ctrl-c
#endif
    }
    catch (...) {
        cerr << "Exception - terminating..." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
