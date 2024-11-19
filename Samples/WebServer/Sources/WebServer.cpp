/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/Module.h"
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#include "Stroika/Foundation/Execution/TimeOutException.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"
#include "Stroika/Foundation/IO/Network/HTTP/Exception.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/IO/Network/HTTP/Methods.h"
#include "Stroika/Foundation/Streams/TextReader.h"
#include "Stroika/Foundation/Time/Duration.h"

#include "Stroika/Frameworks/WebServer/ConnectionManager.h"
#include "Stroika/Frameworks/WebServer/FileSystemRequestHandler.h"
#include "Stroika/Frameworks/WebServer/Router.h"

#include "AppVersion.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Frameworks::WebServer;

using Characters::String;
using Common::ConstantProperty;
using Memory::BLOB;

using Stroika::Frameworks::WebServer::FileSystemRequestHandler;
using Stroika::Frameworks::WebServer::Request;
using Stroika::Frameworks::WebServer::Response;
using Time::Duration;

namespace {

    /**
     *  You don't need to specify any of this, but it maybe helpful to specify caching control policies to
     *  get the best web-server performance.
     */
    const ConstantProperty<FileSystemRequestHandler::Options> kFileSystemRouterOptions_{[] () {
        Sequence<pair<RegularExpression, CacheControl>> cacheControlSettings_{
            {RegularExpression{".*\\.gif", CompareOptions::eCaseInsensitive}, CacheControl{.fMaxAge = Duration{24h}.As<int32_t> ()}}};
        return FileSystemRequestHandler::Options{.fURLPrefix2Strip       = "/Files/"_k,
                                                 .fDefaultIndexFileNames = Sequence<String>{"index.html"_k},
                                                 .fCacheControlSettings  = cacheControlSettings_};
    }};

    /**
     *  You don't need to specify any of this, but its a good idea to properly identify your application.
     */
    const ConstantProperty<Headers> kDefaultResponseHeaders_{[] () {
        Headers h;
        h.server = "Stroika-Sample-WebServer/"_k + AppVersion::kVersion.AsMajorMinorString ();
        return h;
    }};

    /*
     *  It's often helpful to structure together, routes, special interceptors, with your connection manager, to package up
     *  all the logic /options for HTTP interface.
     *
     *  This particular organization also makes it easy to save instance variables with the webserver (like a pointer to a handler)
     *  and access them from the Route handler functions.
     */
    struct MyWebServer_ {

        /**
         *  Routes specify the 'handlers' for the various web URLs your webserver will support.
         */
        const Sequence<Route> kRoutes_;

        /**
         *  The connectionMgr specifies parameters that govern the procedural behavior of your webserver.
         *  For example, caching settings go here, thread pool settings, network bindings, and so on.
         */
        ConnectionManager fConnectionMgr_;

        MyWebServer_ (uint16_t portNumber)
            : kRoutes_{
                /*
                 *  Define the 'routes' for your webserver - the dispatch of URLs to callbacks (usually lambdas).
                 *  But - sometimes the callbacks are complex objects, like the filesystem handler.
                 * 
                 *  \see the ../../../WebService, or ../../../HTMLUI samples for more complex examples of routers.
                 */

                Route{""_RegEx, DefaultPage_}
                
                , Route{HTTP::MethodsRegEx::kPost, "SetAppState"_RegEx, SetAppState_}
                
                , Route{"FRED/?"_RegEx,
                        [] (Request*, Response* response) {
                            response->contentType = DataExchange::InternetMediaTypes::kText_PLAIN;
                            response->write ("FRED"sv);
                        }}
                
                , Route{"Files/.*"_RegEx, FileSystemRequestHandler{GetEXEDir () / "html", kFileSystemRouterOptions_}}}
        , fConnectionMgr_{SocketAddresses (InternetAddresses_Any (), portNumber), kRoutes_,
                              ConnectionManager::Options{.fBindFlags = Socket::BindFlags{}, .fDefaultResponseHeaders = kDefaultResponseHeaders_}}
        {
            cerr << "Listening on {}..."_f(fConnectionMgr_.bindings ()) << endl;
        }

        // Can declare arguments as Request*,Response*
        static void DefaultPage_ (Request*, Response* response)
        {
            constexpr bool kUseChunkedTransferCoding_ = true;
            //constexpr bool kUseChunkedTransferCoding_ = false;
            if (kUseChunkedTransferCoding_) {
                response->automaticTransferChunkSize = 25;
            }
            else {
                response->automaticTransferChunkSize = Response::kNoChunkedTransfer;
            }
            response->contentType = DataExchange::InternetMediaTypes::kHTML;
            response->writeln ("<html><body>"sv);
            response->writeln ("<p>Hi Mom</p>"sv);
            response->writeln ("<ul>"sv);
            response->writeln ("Run the service (under the debugger if you wish)"sv);
            response->writeln ("<li>curl http://localhost:8080/ OR</li>"sv);
            response->writeln ("<li>curl http://localhost:8080/FRED OR      (to see error handling)</li>"sv);
            response->writeln ("<li>curl -H \"Content-Type: application/json\" -X POST -d '{\"AppState\":\"Start\"}' http://localhost:8080/SetAppState</li>"sv);
            response->writeln ("<li>curl http://localhost:8080/Files/index.html -v</li>"sv);
            response->writeln ("</ul>"sv);
            response->writeln ("</body></html>"sv);
        }
        // Can declare arguments as Message* message
        static void SetAppState_ (Message* message)
        {
            message->rwResponse ().contentType = DataExchange::InternetMediaTypes::kHTML;
            String argsAsString                = Streams::TextReader::New (message->rwRequest ().GetBody ()).ReadAll ();
            message->rwResponse ().writeln ("<html><body><p>Hi SetAppState ("sv + argsAsString + ")</p></body></html>");
        }
    };
}

int main (int argc, const char* argv[])
{
    CommandLine                               cmdLine{argc, argv};
    Debug::TraceContextBumper                 ctx{"main", "argv={}"_f, cmdLine};
    SignalHandlerRegistry::SafeSignalsManager safeSignals;
#if qStroika_Foundation_Common_Platform_POSIX
    SignalHandlerRegistry::Get ().SetSignalHandlers (SIGPIPE, SignalHandlerRegistry::kIGNORED);
#endif

    uint16_t portNumber = 8080;

    const CommandLine::Option kPortO_{
        .fLongName = "port"sv, .fSupportsArgument = true, .fHelpOptionText = "specify webserver listen port (default {})"_f(portNumber)};
    const CommandLine::Option kQuitAfterO_{
        .fLongName = "quit-after"sv, .fSupportsArgument = true, .fHelpOptionText = "automatically quit after <argument> seconds"sv};
    const Sequence<CommandLine::Option> kAllOptions_{StandardCommandLineOptions::kHelp, kPortO_, kQuitAfterO_};

    try {
        cmdLine.Validate (kAllOptions_);
    }
    catch (const InvalidCommandLineArgument&) {
        cerr << Characters::ToString (current_exception ()).AsNarrowSDKString () << endl;
        cerr << cmdLine.GenerateUsage (kAllOptions_).AsNarrowSDKString () << endl;
        return EXIT_FAILURE;
    }
    if (cmdLine.Has (StandardCommandLineOptions::kHelp)) {
        cerr << cmdLine.GenerateUsage (kAllOptions_).AsNarrowSDKString () << endl;
        return EXIT_SUCCESS;
    }

    try {
        Time::DurationSeconds quitAfter = Time::kInfinity;
        if (auto o = cmdLine.GetArgument (kPortO_)) {
            portNumber = Characters::String2Int<uint16_t> (*o);
        }
        if (auto o = cmdLine.GetArgument (kQuitAfterO_)) {
            quitAfter = Time::DurationSeconds{Characters::FloatConversion::ToFloat<Time::DurationSeconds::rep> (*o)};
        }
        MyWebServer_ myWebServer{portNumber}; // listen and dispatch while this object exists
        WaitableEvent{}.Wait (quitAfter);     // wait quitAfter seconds, or til user hits ctrl-c
    }
    catch (const TimeOutException&) {
        cerr << "Timed out - so - exiting..." << endl;
        return EXIT_SUCCESS;
    }
    catch (...) {
        cerr << "Error encountered: " << Characters::ToString (current_exception ()).AsNarrowSDKString () << " - terminating..." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
