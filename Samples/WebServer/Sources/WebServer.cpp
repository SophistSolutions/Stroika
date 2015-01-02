/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "Stroika/Frameworks/StroikaPreComp.h"

#include    <mutex>
#include    <iostream>

#include    "Stroika/Foundation/Execution/CommandLine.h"
#include    "Stroika/Foundation/Execution/SignalHandlers.h"
#include    "Stroika/Foundation/Execution/WaitableEvent.h"
#include    "Stroika/Foundation/Memory/Optional.h"
#include    "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include    "Stroika/Foundation/IO/Network/LinkMonitor.h"
#include    "Stroika/Foundation/IO/Network/Listener.h"

#include    "Stroika/Frameworks/WebServer/ConnectionManager.h"

using   namespace std;

using   namespace Stroika::Foundation;
using   namespace Stroika::Foundation::IO::Network;
using   namespace Stroika::Frameworks::WebServer;

using   Characters::String;
using   Containers::Sequence;
using   Memory::Optional;



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
                String url = conn.GetRequest ().fURL.GetFullURL ();
                DbgTrace (L"Serving page %s", url.c_str ());
                conn.GetResponse ().writeln (L"<html><body><p>Hi Mom</p></body></html>");
                conn.GetResponse ().SetContentType (DataExchange::PredefinedInternetMediaType::Text_HTML_CT ());
                conn.GetResponse ().End ();
            });
            runConnectionOnAnotherThread.SetThreadName (L"Connection Thread");  // SHOULD use a fancier name (connection#)
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
