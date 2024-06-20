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
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Frameworks::WebServer;

using Characters::String;
using Common::ConstantProperty;
using Execution::CommandLine;
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
        return FileSystemRequestHandler::Options{"Files"_k, Sequence<String>{"index.html"_k}, nullopt, cacheControlSettings_};
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
         *  Routes specify the 'handlers' for the various web urls your webserver will support.
         */
        const Sequence<Route> kRoutes_;

        /**
         *  The connectionMgr specifies parameters that govern the procedural behavior of your webserver.
         *  For example, caching settings go here, thread pool settings, network bindings, and so on.
         */
        ConnectionManager fConnectionMgr_;

        MyWebServer_ (uint16_t portNumber)
            : kRoutes_{Route{""_RegEx, DefaultPage_}, Route{HTTP::MethodsRegEx::kPost, "SetAppState"_RegEx, SetAppState_},
                       Route{"FRED"_RegEx,
                             [] (Request*, Response* response) {
                                 response->contentType = DataExchange::InternetMediaTypes::kText_PLAIN;
                                 response->write (L"FRED");
                             }},
                       Route{"Files/.*"_RegEx, FileSystemRequestHandler{Execution::GetEXEDir () / "html", kFileSystemRouterOptions_}}}
            , fConnectionMgr_{SocketAddresses (InternetAddresses_Any (), portNumber), kRoutes_,
                              ConnectionManager::Options{.fBindFlags = Socket::BindFlags{}, .fDefaultResponseHeaders = kDefaultResponseHeaders_}}
        {
        }
        // Can declare arguments as Request*,Response*
        static void DefaultPage_ (Request*, Response* response)
        {
            constexpr bool kUseTransferCoding_ = true;
            //constexpr bool kUseTransferCoding_ = false;
            if (kUseTransferCoding_) {
                response->rwHeaders ().transferEncoding = HTTP::TransferEncoding::kChunked;
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
    CommandLine                                          cmdLine{argc, argv};
    Debug::TraceContextBumper                            ctx{"main", "argv={}"_f, cmdLine};
    Execution::SignalHandlerRegistry::SafeSignalsManager safeSignals;
#if qPlatform_POSIX
    Execution::SignalHandlerRegistry::Get ().SetSignalHandlers (SIGPIPE, Execution::SignalHandlerRegistry::kIGNORED);
#endif
    const Execution::CommandLine::Option kPortO_{.fLongName = "port"sv, .fSupportsArgument = true};
    const Execution::CommandLine::Option kQuitAfterO_{.fLongName = "quit-after"sv, .fSupportsArgument = true};

    try {
        uint16_t              portNumber = 8080;
        Time::DurationSeconds quitAfter  = Time::kInfinity;
        cmdLine.Validate ({kPortO_, kQuitAfterO_});
        if (auto o = cmdLine.GetArgument (kPortO_)) {
            portNumber = Characters::String2Int<uint16_t> (*o);
        }
        if (auto o = cmdLine.GetArgument (kQuitAfterO_)) {
            quitAfter = Time::DurationSeconds{Characters::FloatConversion::ToFloat<Time::DurationSeconds::rep> (*o)};
        }
        MyWebServer_ myWebServer{portNumber};        // listen and dispatch while this object exists
        Execution::WaitableEvent{}.Wait (quitAfter); // wait quitAfter seconds, or til user hits ctrl-c
    }
    catch (const Execution::TimeOutException&) {
        cerr << "Timed out - so - exiting..." << endl;
        return EXIT_SUCCESS;
    }
    catch (...) {
        cerr << "Error encountered: " << Characters::ToString (current_exception ()).AsNarrowSDKString () << " - terminating..." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
