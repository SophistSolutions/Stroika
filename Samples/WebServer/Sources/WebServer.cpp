/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/Module.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/TimeOutException.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"
#include "Stroika/Foundation/IO/Network/HTTP/Exception.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/Streams/TextReader.h"

#include "Stroika/Frameworks/WebServer/ConnectionManager.h"
#include "Stroika/Frameworks/WebServer/FileSystemRouter.h"
#include "Stroika/Frameworks/WebServer/Router.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Frameworks::WebServer;

using Characters::String;
using Memory::BLOB;

/*
 *  To test this example: (make sure you run with 'current directory == top level directory of this sample else you wont find sample-html-folder)
 *
 *      o   Run the service (under the debugger if you wish)
 *      o   curl  http://localhost:8080/ OR
 *      o   curl  http://localhost:8080/FRED OR      (to see error handling)
 *      o   curl -H "Content-Type: application/json" -X POST -d '{"AppState":"Start"}' http://localhost:8080/SetAppState
 *      o   curl  http://localhost:8080/Files/foo.html -v
 */

namespace {
    /*
     *  It's often helpful to structure together, routes, special interceptors, with your connection manager, to package up
     *  all the logic /options for HTTP interface.
     *
     *  This particular organization also makes it easy to save instance variables with the webserver (like a pointer to a handler)
     *  and accesss them from the Route handler functions.
     */
    struct MyWebServer_ {
        const Router      kRouter_;
        ConnectionManager fConnectionMgr_;
        MyWebServer_ (uint16_t portNumber)
            : kRouter_{
                  Sequence<Route>{
                      Route{RegularExpression (L""), DefaultPage_},
                      Route{RegularExpression (L"POST"), RegularExpression (L"SetAppState"), SetAppState_},
                      Route{
                          RegularExpression (L"Files/.*"),
                          FileSystemRouter{Execution::GetEXEDir () + L"html", String (L"Files"), Sequence<String>{L"index.html"}},
                      },
                  }}
            , fConnectionMgr_{SocketAddresses (InternetAddresses_Any (), portNumber), kRouter_, ConnectionManager::Options{{}, Socket::BindFlags{}, String{L"Stroika-Sample-WebServer/1.0"}}}
        {
        }
        // Can declare arguments as Request*,Response*
        static void DefaultPage_ (Request*, Response* response)
        {
            response->writeln (L"<html><body>");
            response->writeln (L"<p>Hi Mom</p>");
            response->writeln (L"<ul>");
            response->writeln (L"Run the service (under the debugger if you wish)");
            response->writeln (L"<li>curl http://localhost:8080/ OR</li>");
            response->writeln (L"<li>curl http://localhost:8080/FRED OR      (to see error handling)</li>");
            response->writeln (L"<li>curl -H \"Content-Type: application/json\" -X POST -d '{\"AppState\":\"Start\"}' http://localhost:8080/SetAppState</li>");
            response->writeln (L"<li>curl http://localhost:8080/Files/foo.html -v</li>");
            response->writeln (L"</ul>");
            response->writeln (L"</body></html>");

            response->SetContentType (DataExchange::PredefinedInternetMediaType::kText_HTML);
        }
        // Can declare arguments as Message* message
        static void SetAppState_ (Message* message)
        {
            String argsAsString = Streams::TextReader::New (message->PeekRequest ()->GetBody ()).ReadAll ();
            message->PeekResponse ()->writeln (L"<html><body><p>Hi SetAppState (" + argsAsString.As<wstring> () + L")</p></body></html>");
            message->PeekResponse ()->SetContentType (DataExchange::PredefinedInternetMediaType::kText_HTML);
        }
    };
}

int main (int argc, const char* argv[])
{
    Execution::SignalHandlerRegistry::SafeSignalsManager safeSignals;
#if qPlatform_POSIX
    Execution::SignalHandlerRegistry::Get ().SetSignalHandlers (SIGPIPE, Execution::SignalHandlerRegistry::kIGNORED);
#endif
    uint16_t                  portNumber = 8080;
    Time::DurationSecondsType quitAfter  = numeric_limits<Time::DurationSecondsType>::max ();

    Sequence<String> args = Execution::ParseCommandLine (argc, argv);
    for (auto argi = args.begin (); argi != args.end (); ++argi) {
        if (Execution::MatchesCommandLineArgument (*argi, L"port")) {
            ++argi;
            if (argi != args.end ()) {
                portNumber = Characters::String2Int<uint16_t> (*argi);
            }
            else {
                cerr << "Expected arg to -port" << endl;
                return EXIT_FAILURE;
            }
        }
        else if (Execution::MatchesCommandLineArgument (*argi, L"quit-after")) {
            ++argi;
            if (argi != args.end ()) {
                quitAfter = Characters::String2Float<Time::DurationSecondsType> (*argi);
            }
            else {
                cerr << "Expected arg to -quit-after" << endl;
                return EXIT_FAILURE;
            }
        }
    }

    try {
        MyWebServer_ myWebServer{portNumber};        // listen and dispatch while this object exists
        Execution::WaitableEvent{}.Wait (quitAfter); // wait forever - til user hits ctrl-c
    }
    catch (const Execution::TimeOutException&) {
        cerr << "Timed out - so - terminating..." << endl;
        return EXIT_FAILURE;
    }
    catch (...) {
        cerr << "Error encountered: " << Characters::ToString (current_exception ()).AsNarrowSDKString () << " - terminating..." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
