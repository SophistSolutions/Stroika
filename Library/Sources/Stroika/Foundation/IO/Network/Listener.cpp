/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Debug/Assertions.h"
#include    "../../Execution/Exceptions.h"
#include    "../../Execution/Thread.h"

#include    "Listener.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::IO::Network;




/*
********************************************************************************
************************* IO::Network::Listener::Rep_ **************************
********************************************************************************
*/
struct  Listener::Rep_ {
    Rep_ (const SocketAddress& addr, const function<void (Socket newConnection)>& newConnectionAcceptor)
        : fSockAddr (addr)
        , fNewConnectionAcceptor (newConnectionAcceptor)
        , fMasterSocket (Socket::SocketKind::STREAM)
    {
        fMasterSocket.Bind (addr);  // do in CTOR so throw propagated
        fMasterSocket.Listen (10);//need param

        fListenThread = Execution::Thread ([this]() {
            while (true) {
                try {
                    Socket s = fMasterSocket.Accept ();
                    fNewConnectionAcceptor (s);
                }
                catch (const Execution::Thread::AbortException&) {
                    Execution::DoReThrow ();
                }
                catch (...) {
                    // unclear what todo with expcetions here
                    // probnably ignore all but for theradabort.
                    // may need virtual fucntions to handle? Or std::function passed in?
                    DbgTrace (L"Exception accepting new coonection - ignored");
                }
            }
        });
        fListenThread.SetThreadName (L"WebServer Listener");    // @todo include sockaddr 'pretty print' in name?
        fListenThread.Start ();

    }
    ~Rep_ ()
    {
        // critical we wait for finish of thread cuz it has bare 'this' pointer captured
        Thread::SuppressInterruptionInContext  suppressInterruption;
        IgnoreExceptionsForCall (fListenThread.AbortAndWaitForDone ());
    }

    SocketAddress                           fSockAddr;
    function<void (Socket newConnection)>   fNewConnectionAcceptor;
    Socket                                  fMasterSocket;
    Execution::Thread                       fListenThread;
};



/*
********************************************************************************
**************************** IO::Network::Listener *****************************
********************************************************************************
*/
Listener::Listener (const SocketAddress& addr, const function<void (Socket newConnection)>& newConnectionAcceptor)
    : fRep_ (new Rep_ (addr, newConnectionAcceptor))
{
}

Listener::Listener (const Listener&& rhs)
    : fRep_ (move (rhs.fRep_))
{
}
