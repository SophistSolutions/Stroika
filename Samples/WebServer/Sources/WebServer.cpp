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
    Router s_Router_ {
        Sequence<Route> {
            Route { RegularExpression (L"", RegularExpression::SyntaxType::eECMAScript), DefaultPage_},
            Route { RegularExpression (L"POST", RegularExpression::SyntaxType::eECMAScript), RegularExpression (L"SetAppState", RegularExpression::SyntaxType::eECMAScript), SetAppState_},
        }
    };
}

int main (int argc, const char* argv[])
{
    Execution::SignalHandlerRegistry::SafeSignalsManager    safeSignals;
    try {
        ConnectionManager   cm; // use??? @todo??

        auto onConnect = [](Socket s) {
            Execution::Thread runConnectionOnAnotherThread ([s]() {
                // now read
                Connection conn (s);
                conn.ReadHeaders ();    // bad API. Must rethink...
                conn.GetResponse ().AddHeader (IO::Network::HTTP::HeaderName::kServer, L"stroika-web-server-demo");

                constexpr bool  kIgnoreSillyCORS_ { true };
                if (kIgnoreSillyCORS_) {
                    conn.GetResponse ().AddHeader (IO::Network::HTTP::HeaderName::kAccessControlAllowOrigin, L"*");
                    conn.GetResponse ().AddHeader (IO::Network::HTTP::HeaderName::kAccessControlAllowHeaders, L"Origin, X-Requested-With, Content-Type, Accept, Authorization");
                }

                String path = conn.GetRequest ().fURL.GetHostRelativePath ();
                DbgTrace (L"Serving page %s", path.c_str ());
                try {
                    Optional<RequestHandler>    handler = s_Router_.Lookup (conn.GetRequest ());
                    if (handler) {
                        (*handler) (&conn.GetRequest (), &conn.GetResponse ());
                    }
                    else {
                        Execution::Throw (IO::Network::HTTP::Exception (HTTP::StatusCodes::kNotFound));
                    }
                }
                catch (const IO::Network::HTTP::Exception& e) {
                    conn.GetResponse ().SetStatus (e.GetStatus (), e.GetReason ());
                    conn.GetResponse ().writeln (L"<html><body><p>OOPS</p></body></html>");
                    conn.GetResponse ().SetContentType (DataExchange::PredefinedInternetMediaType::Text_HTML_CT ());
                }
                catch (...) {
                    conn.GetResponse ().SetStatus (HTTP::StatusCodes::kInternalError);
                    conn.GetResponse ().writeln (L"<html><body><p>OOPS</p></body></html>");
                    conn.GetResponse ().SetContentType (DataExchange::PredefinedInternetMediaType::Text_HTML_CT ());
                }
                conn.GetResponse ().End ();
            });
            runConnectionOnAnotherThread.SetThreadName (L"Connection Thread");  // Could use a fancier name (connection#, from remote address?)
            runConnectionOnAnotherThread.Start ();
            runConnectionOnAnotherThread.WaitForDone ();    // maybe save these in connection mgr so we can force them all shut down...
        };
        Listener l (SocketAddress (Network::V4::kAddrAny, 8080), onConnect);
        Execution::WaitableEvent (Execution::WaitableEvent::eAutoReset).Wait ();    // wait forever - til user hits ctrl-c
    }
    catch (...) {
        cerr << "Exception - terminating..." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
