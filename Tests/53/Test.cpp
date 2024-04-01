/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Frameworks::WebServer
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/Module.h"
#include "Stroika/Foundation/IO/Network/Transfer/Connection.h"
#include "Stroika/Foundation/Streams/TextReader.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"
#include "Stroika/Frameworks/WebServer/ConnectionManager.h"
#include "Stroika/Frameworks/WebServer/FileSystemRequestHandler.h"
#include "Stroika/Frameworks/WebServer/Router.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Characters;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;

using Common::ConstantProperty;
using Time::Duration;

namespace {
    /*
     *  It's often helpful to structure together, routes, special interceptors, with your connection manager, to package up
     *  all the logic /options for HTTP interface.
     *
     *  This particular organization also makes it easy to save instance variables with the webserver (like a pointer to a handler)
     *  and accesss them from the Route handler functions.
     */
    struct MyWebServer_ {

        /**
         *  Routes specify the 'handlers' for the various web urls your webserver will support.
         */
        const Sequence<Route> kRoutes_;

        /**
         *  The connectionMgr specifies parameters that govern the behavior of your webserver.
         *  For example, caching settings go here, thread pool settings, network bindings, and so on.
         */
        ConnectionManager fConnectionMgr_;

        MyWebServer_ (uint16_t portNumber)
            : kRoutes_{Route{""_RegEx, DefaultPage_}, Route{HTTP::MethodsRegEx::kPost, "SetAppState"_RegEx, SetAppState_},
                       Route{"FRED"_RegEx,
                             [] (Request*, Response* response) {
                                 response->contentType = DataExchange::InternetMediaTypes::kText_PLAIN;
                                 response->write (L"FRED");
                             }}}
            , fConnectionMgr_{SocketAddresses (InternetAddresses_Any (), portNumber), kRoutes_}
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

#if qHasFeature_GoogleTest
namespace {
    GTEST_TEST (Frameworks_WebServer, SimpleStartStopServerTest)
    {
        const auto   portNumber = 8082;
        const auto   quitAfter  = 1s;
        MyWebServer_ myWebServer{portNumber};               // listen and dispatch while this object exists
        Execution::WaitableEvent{}.WaitQuietly (quitAfter); // leave it running for a bit
    }
}

namespace {
    GTEST_TEST (Frameworks_WebServer, SimpleCurlTestTalk2Server)
    {
        const IO::Network::PortType     portNumber = 8082;
        MyWebServer_                    myWebServer{portNumber}; // listen and dispatch while this object exists
        auto                            c = IO::Network::Transfer::Connection::New ();
        IO::Network::Transfer::Response r = c.GET (URI{"http", URI::Authority{URI::Host{"localhost"}, portNumber}});
        EXPECT_TRUE (r.GetSucceeded ());
        EXPECT_TRUE (r.GetData ().size () > 1);
        String response = r.GetDataTextInputStream ().ReadAll ();
        DbgTrace (L"response={}"_f, response);
        EXPECT_TRUE (response.StartsWith ("<html>"));
        EXPECT_TRUE (response.EndsWith ("</html>\r\n"));
    }
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qHasFeature_GoogleTest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
