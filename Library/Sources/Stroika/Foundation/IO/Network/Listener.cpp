/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/ToString.h"
#include "../../Containers/Bijection.h"
#include "../../Containers/Sequence.h"
#include "../../Debug/Assertions.h"
#include "../../Execution/Thread.h"
#include "../../Execution/Throw.h"
#include "../../Execution/WaitForIOReady.h"

#include "ConnectionOrientedMasterSocket.h"

#include "Listener.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::Traversal;

/*
 ********************************************************************************
 ************************* IO::Network::Listener::Rep_ **************************
 ********************************************************************************
 */
struct Listener::Rep_ {
    Rep_ (const Iterable<SocketAddress>& addrs, const Socket::BindFlags& bindFlags, unsigned int backlog, const function<void(const ConnectionOrientedStreamSocket::Ptr& newConnection)>& newConnectionAcceptor)
        : fNewConnectionAcceptor (newConnectionAcceptor)
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"Listener::Rep_::CTOR (addres=%s, ...)", Characters::ToString (addrs).c_str ());
#endif
        for (auto addr : addrs) {
            ConnectionOrientedMasterSocket::Ptr ms = ConnectionOrientedMasterSocket::New (addr.GetAddressFamily (), Socket::STREAM);
            ms.Bind (addr, bindFlags); // do in CTOR so throw propagated
            ms.Listen (backlog);
            fMasterSockets += ms;
        }
        fListenThread = Execution::Thread::New (
            [this]() {
                Containers::Bijection<ConnectionOrientedMasterSocket::Ptr, WaitForIOReady::FileDescriptorType> socket2FDBijection;
                for (auto&& s : fMasterSockets) {
                    socket2FDBijection.Add (s, s.GetNativeSocket ());
                }
                WaitForIOReady sockSetPoller{socket2FDBijection.Image ()};
                while (true) {
                    try {
                        for (auto readyFD : sockSetPoller.WaitQuietly ().value_or (Set<WaitForIOReady::FileDescriptorType>{})) {
                            ConnectionOrientedMasterSocket::Ptr localSocketToAcceptOn = *socket2FDBijection.InverseLookup (readyFD);
                            ConnectionOrientedStreamSocket::Ptr s                     = localSocketToAcceptOn.Accept ();
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
                        DbgTrace (L"Exception accepting new connection: %s - ignored", Characters::ToString (current_exception ()).c_str ());
                    }
                }
            },
            Thread::eAutoStart,
            L"Socket Listener: " + Characters::ToString (addrs));
    }

    function<void(const ConnectionOrientedStreamSocket::Ptr& newConnection)> fNewConnectionAcceptor;
    Sequence<ConnectionOrientedMasterSocket::Ptr>                            fMasterSockets;
    Execution::Thread::CleanupPtr                                            fListenThread{Execution::Thread::CleanupPtr::eAbortBeforeWaiting};
};

/*
********************************************************************************
**************************** IO::Network::Listener *****************************
********************************************************************************
*/
Listener::Listener (const SocketAddress& addr, const function<void(const ConnectionOrientedStreamSocket::Ptr& newConnection)>& newConnectionAcceptor, unsigned int backlog)
    : Listener (Sequence<SocketAddress>{addr}, Socket::BindFlags{}, newConnectionAcceptor, backlog)
{
}

Listener::Listener (const SocketAddress& addr, const Socket::BindFlags& bindFlags, const function<void(const ConnectionOrientedStreamSocket::Ptr& newConnection)>& newConnectionAcceptor, unsigned int backlog)
    : Listener (Sequence<SocketAddress>{addr}, bindFlags, newConnectionAcceptor, backlog)
{
}

Listener::Listener (const Traversal::Iterable<SocketAddress>& addrs, const function<void(const ConnectionOrientedStreamSocket::Ptr& newConnection)>& newConnectionAcceptor, unsigned int backlog)
    : Listener (addrs, Socket::BindFlags{}, newConnectionAcceptor, backlog)
{
}

Listener::Listener (const Traversal::Iterable<SocketAddress>& addrs, const Socket::BindFlags& bindFlags, const function<void(const ConnectionOrientedStreamSocket::Ptr& newConnection)>& newConnectionAcceptor, unsigned int backlog)
    : fRep_ (make_shared<Rep_> (addrs, bindFlags, backlog, newConnectionAcceptor))
{
}

Listener::Listener (const Listener&& rhs)
    : fRep_ (move (rhs.fRep_))
{
}
