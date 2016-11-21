/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Containers/Sequence.h"
#include    "../../Debug/Assertions.h"
#include    "../../Execution/Exceptions.h"
#include    "../../Execution/Thread.h"

#include    "Listener.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::IO::Network;
using   namespace   Stroika::Foundation::Traversal;


namespace {
    struct SocketSetPolling_ {
        enum Flag { eRead, eWrite, eExcept };

        fd_set readfds;
        int maxfd;

        Sequence<Socket>    fSockets2Listen;
        SocketSetPolling_ (const Sequence<Socket>& socks)
            : fSockets2Listen (socks)
        {
            FD_ZERO (&readfds);
            maxfd = -1;
            for (int i = 0; i < socks.size (); i++) {
                FD_SET(socks[i].GetNativeSocket (), &readfds);
                if (socks[i].GetNativeSocket () > maxfd)
                    maxfd = socks[i].GetNativeSocket ();
            }
        }
        Sequence<Socket>    Wait ()
        {
            Sequence<Socket>    results;
            int status = select(maxfd + 1, &readfds, NULL, NULL, NULL);
            // throw if < 0
            //if (status < 0)
            //  return INVALID_SOCKET;

            int fd;
            fd = INVALID_SOCKET;
            for (int i = 0; i < fSockets2Listen.size (); i++) {
                if (FD_ISSET(fSockets2Listen[i].GetNativeSocket (), &readfds)) {
                    fd = fSockets2Listen[i].GetNativeSocket ();
                    results.Append (fSockets2Listen[i]);
                }
            }
            return results;
        }
    };

}

/*
 ********************************************************************************
 ************************* IO::Network::Listener::Rep_ **************************
 ********************************************************************************
 */
struct  Listener::Rep_ {
    Rep_ (const Iterable<SocketAddress>& addrs, const Socket::BindFlags& bindFlags, unsigned int backlog, const function<void (Socket newConnection)>& newConnectionAcceptor)
        : fSockAddrs (addrs)
        , fNewConnectionAcceptor (newConnectionAcceptor)
    {
        for (auto addr : addrs) {
            Socket  ms (Socket::SocketKind::STREAM);
            ms.Bind (addr, bindFlags);  // do in CTOR so throw propagated
            ms.Listen (backlog);
            fMasterSockets += ms;
        }
        fListenThread = Execution::Thread ([this]() {
            SocketSetPolling_   sockSetPoller { fMasterSockets };
            while (true) {
                try {
                    for (Socket localSocketToAcceptOn : sockSetPoller.Wait ()) {
                        Socket s = localSocketToAcceptOn.Accept ();
                        fNewConnectionAcceptor (s);
                    }
                }
                catch (const Execution::Thread::AbortException&) {
                    Execution::ReThrow ();
                }
                catch (...) {
                    // unclear what todo with expcetions here
                    // probnably ignore all but for theradabort.
                    // may need virtual fucntions to handle? Or std::function passed in?
                    DbgTrace (L"Exception accepting new coonection - ignored");
                }
            }
        });
        fListenThread.SetThreadName (L"Socket Listener");    // @todo include sockaddr 'pretty print' in name?
        fListenThread.Start ();
    }
    ~Rep_ ()
    {
        // critical we wait for finish of thread cuz it has bare 'this' pointer captured
        Thread::SuppressInterruptionInContext  suppressInterruption;
        IgnoreExceptionsForCall (fListenThread.AbortAndWaitForDone ());
    }

    Sequence<SocketAddress>                 fSockAddrs;
    function<void (Socket newConnection)>   fNewConnectionAcceptor;
    Sequence<Socket>                        fMasterSockets;
    Execution::Thread                       fListenThread;
};





/*
********************************************************************************
**************************** IO::Network::Listener *****************************
********************************************************************************
*/
Listener::Listener (const SocketAddress& addr, const function<void (Socket newConnection)>& newConnectionAcceptor, unsigned int backlog)
    : Listener (Sequence<SocketAddress> { addr }, Socket::BindFlags {}, newConnectionAcceptor, backlog)
{
}

Listener::Listener (const SocketAddress& addr, const Socket::BindFlags& bindFlags, const function<void (Socket newConnection)>& newConnectionAcceptor, unsigned int backlog)
    : Listener (Sequence<SocketAddress> { addr }, bindFlags, newConnectionAcceptor, backlog)
{
}

Listener::Listener (const Traversal::Iterable<SocketAddress>& addrs, const function<void (Socket newConnection)>& newConnectionAcceptor, unsigned int backlog)
    : Listener (addrs, Socket::BindFlags {}, newConnectionAcceptor, backlog)
{
}

Listener::Listener (const Traversal::Iterable<SocketAddress>& addrs, const Socket::BindFlags& bindFlags, const function<void (Socket newConnection)>& newConnectionAcceptor, unsigned int backlog)
    : fRep_ (make_shared<Rep_> (addrs, bindFlags, backlog, newConnectionAcceptor))
{
}

Listener::Listener (const Listener&& rhs)
    : fRep_ (move (rhs.fRep_))
{
}
