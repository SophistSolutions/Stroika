/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/ToString.h"
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
using namespace Stroika::Foundation::Characters::Literals;
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
    Rep_ (const Iterable<SocketAddress>& addrs, const Socket::BindFlags& bindFlags, unsigned int backlog,
          const function<void (const ConnectionOrientedStreamSocket::Ptr& newConnection)>& newConnectionAcceptor)
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("Listener::Rep_::CTOR (addres={}, ...)"_f, addrs);
#endif
        Sequence<ConnectionOrientedMasterSocket::Ptr> masterSockets;
        for (const auto& addr : addrs) {
            ConnectionOrientedMasterSocket::Ptr ms = ConnectionOrientedMasterSocket::New (addr.GetAddressFamily (), Socket::STREAM);
            ms.Bind (addr, bindFlags); // do in CTOR (not thread) so throw propagated
            ms.Listen (backlog);
            masterSockets += ms;
        }
        fListenThread = Execution::Thread::New (
            [masterSockets, newConnectionAcceptor] () {
                WaitForIOReady<ConnectionOrientedMasterSocket::Ptr> sockSetPoller{masterSockets};
                while (true) {
                    try {
                        for (const auto& readyMasterSocket : sockSetPoller.WaitQuietly ()) {
                            ConnectionOrientedStreamSocket::Ptr s = readyMasterSocket.Accept ();
                            newConnectionAcceptor (s);
                        }
                    }
                    catch (const Execution::Thread::AbortException&) {
                        Execution::ReThrow ();
                    }
                    catch (...) {
                        // unclear what todo with exceptions here
                        // probably ignore all but for thread abort.
                        // may need virtual functions to handle? Or std::function passed in?
                        DbgTrace ("Exception accepting new connection: {} - ignored"_f, current_exception ());
                    }
                }
            },
            Thread::eAutoStart, "Socket Listener: "sv + Characters::ToString (addrs));
    }
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
    ~Rep_ ()
    {
        DbgTrace ("Starting destructor for Listener::Rep_ ({})"_f, static_cast<const void*> (this));
    }
#endif

    Execution::Thread::CleanupPtr fListenThread{Execution::Thread::CleanupPtr::eAbortBeforeWaiting};
};

/*
********************************************************************************
**************************** IO::Network::Listener *****************************
********************************************************************************
*/
Listener::Listener (const SocketAddress& addr,
                    const function<void (const ConnectionOrientedStreamSocket::Ptr& newConnection)>& newConnectionAcceptor, unsigned int backlog)
    : Listener{Sequence<SocketAddress>{addr}, Socket::BindFlags{}, newConnectionAcceptor, backlog}
{
}

Listener::Listener (const SocketAddress& addr, const Socket::BindFlags& bindFlags,
                    const function<void (const ConnectionOrientedStreamSocket::Ptr& newConnection)>& newConnectionAcceptor, unsigned int backlog)
    : Listener{Sequence<SocketAddress>{addr}, bindFlags, newConnectionAcceptor, backlog}
{
}

Listener::Listener (const Traversal::Iterable<SocketAddress>& addrs,
                    const function<void (const ConnectionOrientedStreamSocket::Ptr& newConnection)>& newConnectionAcceptor, unsigned int backlog)
    : Listener{addrs, Socket::BindFlags{}, newConnectionAcceptor, backlog}
{
}

Listener::Listener (const Traversal::Iterable<SocketAddress>& addrs, const Socket::BindFlags& bindFlags,
                    const function<void (const ConnectionOrientedStreamSocket::Ptr& newConnection)>& newConnectionAcceptor, unsigned int backlog)
    : fRep_{make_shared<Rep_> (addrs, bindFlags, backlog, newConnectionAcceptor)}
{
}

Listener::Listener (Listener&& rhs) noexcept
    : fRep_{move (rhs.fRep_)}
{
}
