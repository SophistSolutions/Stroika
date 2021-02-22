/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
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
using Memory::BLOB;
using Stroika::Frameworks::WebServer::FileSystemRequestHandler;
using Stroika::Frameworks::WebServer::Request;
using Stroika::Frameworks::WebServer::Response;
using Time::Duration;

namespace {
    Sequence<pair<RegularExpression, CacheControl>> kCacheControlSettings_{
#if __cpp_designated_initializers
        pair<RegularExpression, CacheControl>{RegularExpression{L".*\\.gif", CompareOptions::eCaseInsensitive}, CacheControl{.fMaxAge = Duration{24h}.As<double> ()}}
#else
        pair<RegularExpression, CacheControl>{RegularExpression{L".*\\.gif", CompareOptions::eCaseInsensitive}, CacheControl{nullopt, nullopt, false, nullopt, Duration{24h}.As<double> ()}}
#endif
    };

    FileSystemRequestHandler::Options kFSHandlersOptions_{L"Files"_k, Sequence<String>{L"index.html"_k}, nullopt, kCacheControlSettings_};

    /*
     *  It's often helpful to structure together, routes, special interceptors, with your connection manager, to package up
     *  all the logic /options for HTTP interface.
     *
     *  This particular organization also makes it easy to save instance variables with the webserver (like a pointer to a handler)
     *  and accesss them from the Route handler functions.
     */
    struct MyWebServer_ {
        const Sequence<Route> kRoutes_;
        ConnectionManager     fConnectionMgr_;
        MyWebServer_ (uint16_t portNumber)
            : kRoutes_
        {
            Route{L""_RegEx, DefaultPage_},
                Route{HTTP::MethodsRegEx::kPost, L"SetAppState"_RegEx, SetAppState_},
                Route{L"FRED"_RegEx, [] (Request*, Response* response) {
                          response->write (L"FRED");
                          response->contentType = DataExchange::InternetMediaTypes::kText_PLAIN;
                      }},
                Route
            {
                L"Files/.*"_RegEx, FileSystemRequestHandler { Execution::GetEXEDir () / L"html", kFSHandlersOptions_ }
            }
        }
#if __cpp_designated_initializers
        , fConnectionMgr_
        {
            SocketAddresses (InternetAddresses_Any (), portNumber), kRoutes_, ConnectionManager::Options { .fBindFlags = Socket::BindFlags{}, .fServerHeader = L"Stroika-Sample-WebServer/"_k + AppVersion::kVersion.AsMajorMinorString () }
        }
#else
        , fConnectionMgr_
        {
            SocketAddresses (InternetAddresses_Any (), portNumber), kRoutes_, ConnectionManager::Options { nullopt, nullopt, Socket::BindFlags{}, L"Stroika-Sample-WebServer/"_k + AppVersion::kVersion.AsMajorMinorString () }
        }
#endif
        {
        }
        // Can declare arguments as Request*,Response*
        static void DefaultPage_ (Request*, Response* response)
        {
            //constexpr bool kUseTransferCoding_ = true;
            constexpr bool kUseTransferCoding_ = false;
            if (kUseTransferCoding_) {
                response->rwHeaders ().transferEncoding = HTTP::TransferEncoding::eChunked;
            }
            response->contentType = DataExchange::InternetMediaTypes::kHTML;
            response->writeln (L"<html><body>");
            response->writeln (L"<p>Hi Mom</p>");
            response->writeln (L"<ul>");
            response->writeln (L"Run the service (under the debugger if you wish)");
            response->writeln (L"<li>curl http://localhost:8080/ OR</li>");
            response->writeln (L"<li>curl http://localhost:8080/FRED OR      (to see error handling)</li>");
            response->writeln (L"<li>curl -H \"Content-Type: application/json\" -X POST -d '{\"AppState\":\"Start\"}' http://localhost:8080/SetAppState</li>");
            response->writeln (L"<li>curl http://localhost:8080/Files/index.html -v</li>");
            response->writeln (L"</ul>");
            response->writeln (L"</body></html>");
        }
        // Can declare arguments as Message* message
        static void SetAppState_ (Message* message)
        {
            String argsAsString = Streams::TextReader::New (message->rwRequest ().GetBody ()).ReadAll ();
            message->rwResponse ().writeln (L"<html><body><p>Hi SetAppState (" + argsAsString.As<wstring> () + L")</p></body></html>");
            message->rwResponse ().contentType = DataExchange::InternetMediaTypes::kHTML;
        }
    };
}

int main (int argc, const char* argv[])
{
    Debug::TraceContextBumper                            ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"main", L"argv=%s", Characters::ToString (vector<const char*>{argv, argv + argc}).c_str ())};
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
        cerr << "Timed out - so - exiting..." << endl;
        return EXIT_SUCCESS;
    }
    catch (...) {
        cerr << "Error encountered: " << Characters::ToString (current_exception ()).AsNarrowSDKString () << " - terminating..." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
