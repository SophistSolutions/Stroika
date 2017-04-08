/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/TimeOutException.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"
#include "Stroika/Foundation/IO/Network/HTTP/Exception.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/Streams/TextReader.h"

#include "Stroika/Frameworks/WebServer/ConnectionManager.h"
#include "Stroika/Frameworks/WebServer/Router.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Frameworks::WebServer;

using Characters::String;
using Memory::BLOB;

/*
 *  To test this example:
 *      o   Run the service (under the debugger if you wish)
 *      o   curl  http://localhost:8080/ OR
 *      o   curl  http://localhost:8080/FRED OR      (to see error handling)
 *      o   curl -H "Content-Type: application/json" -X POST -d '{"AppState":"Start"}' http://localhost:8080/SetAppState
 */

namespace {
    struct MyWebServer_ {
        ConnectionManager fConnectionMgr_;
        MyWebServer_ (uint16_t portNumber)
            : fConnectionMgr_{SocketAddress (Network::V4::kAddrAny, portNumber), kRouter_}
        {
            fConnectionMgr_.SetServerHeader (String{L"Stroika-Sample-WebServer/1.0"});
        }
        // Can declare arguments as Request*,Response*
        static void DefaultPage_ (Request*, Response* response)
        {
            response->writeln (L"<html><body><p>Hi Mom</p></body></html>");
            response->SetContentType (DataExchange::PredefinedInternetMediaType::Text_HTML_CT ());
        }
        // Can declare arguments as Message* message
        static void SetAppState_ (Message* message)
        {
            String argsAsString = Streams::TextReader (message->PeekRequest ()->GetBody ()).ReadAll ();
            message->PeekResponse ()->writeln (L"<html><body><p>Hi SetAppState (" + argsAsString.As<wstring> () + L")</p></body></html>");
            message->PeekResponse ()->SetContentType (DataExchange::PredefinedInternetMediaType::Text_HTML_CT ());
        }
        static const Router kRouter_;
    };
    const Router MyWebServer_::kRouter_{
        Sequence<Route>{
            Route{RegularExpression (L"", RegularExpression::eECMAScript), DefaultPage_},
            Route{RegularExpression (L"POST", RegularExpression::eECMAScript), RegularExpression (L"SetAppState", RegularExpression::eECMAScript), SetAppState_},
        }};
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
        MyWebServer_ myWebServer{portNumber};                                             // listen and dispatch while this object exists
        Execution::WaitableEvent (Execution::WaitableEvent::eAutoReset).Wait (quitAfter); // wait forever - til user hits ctrl-c
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
