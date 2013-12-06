/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>
#include    <cstdlib>

#include    "../../Foundation/Characters/Tokenize.h"
#include    "../../Foundation/Containers/Common.h"
#include    "../../Foundation/DataExchange/BadFormatException.h"
#include    "../../Foundation/Debug/Assertions.h"
#include    "../../Foundation/Execution/Exceptions.h"
#include    "../../Foundation/Execution/Thread.h"
#include    "../../Foundation/Memory/SmallStackBuffer.h"

#include    "Listener.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::WebServer;



struct Listener::Rep_ {
    Rep_ (const SocketAddress& addr, const function<void (Socket newConnection)>& newConnectionAcceptor)
        : fSockAddr (addr)
        , fNewConnectionAcceptor (newConnectionAcceptor)
        , fMasterSocket ()
    {
        fMasterSocket.Bind (addr);  // do in CTOR so throw propagated
        fMasterSocket.Listen (10);//need param

        fListenThread = Execution::Thread ([this]() {
            while (true) {
                // unclear what todo with expcetions here
                // probnably ignore all but for theradabort.
                // may need virtual fucntions to handle? Or std::function passed in?
                Socket s = fMasterSocket.Accept ();
                fNewConnectionAcceptor (s);
            }
        });
        fListenThread.SetThreadName (L"WebServer Listener");    // @todo include sockaddr 'pretty print' in name?
        fListenThread.Start ();

    }
    ~Rep_ ()
    {
        // critical we wait for finish of thread cuz it has bare 'this' pointer captured
        IgnoreExceptionsForCall (fListenThread.AbortAndWaitForDone ());
    }

    SocketAddress fSockAddr;
    function<void (Socket newConnection)> fNewConnectionAcceptor;
    Socket  fMasterSocket;
    Execution::Thread fListenThread;
};



Listener::Listener (const SocketAddress& addr, const function<void (Socket newConnection)>& newConnectionAcceptor)
    : fRep_ (new Rep_ (addr, newConnectionAcceptor))
{
}
