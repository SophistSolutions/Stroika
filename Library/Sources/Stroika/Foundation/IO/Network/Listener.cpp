/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_Platform_POSIX
#include <sys/socket.h>
#elif qStroika_Platform_Windows
#include <winsock2.h>
#endif

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/Execution/WaitForIOReady.h"

#include "Stroika/Foundation/IO/Network/ConnectionOrientedMasterSocket.h"

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
struct Listener::Rep_ final {
    Rep_ (const Iterable<SocketAddress>& addrs, const ConnectionOrientedMasterSocket::BindFlags& bindFlags, unsigned int backlog,
          const function<void (const ConnectionOrientedStreamSocket::Ptr& newConnection)>& newConnectionAcceptor)
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("Listener::Rep_::CTOR (address={}, ...)"_f, addrs);
#endif
        // Bind each address to a master socket, and start a listener loop on all of them.
        Sequence<ConnectionOrientedMasterSocket::Ptr> masterSockets;
        for (const auto& addr : addrs) {
            ConnectionOrientedMasterSocket::Ptr ms = ConnectionOrientedMasterSocket::New (addr.GetAddressFamily (), Socket::STREAM);
            ms.Bind (addr, bindFlags); // do in CTOR (not thread) so throw propagated
            ms.Listen (backlog);
            masterSockets += ms;
        }
        fListenThread =
            Execution::Thread::New ([masterSockets, newConnectionAcceptor] () { ListenLoop_ (masterSockets, newConnectionAcceptor); },
                                    Thread::eAutoStart, "Socket Listener: "sv + Characters::ToString (addrs));
    }
#if qStroika_Foundation_Debug_DefaultTracingOn
    ~Rep_ ()
    {
        DbgTrace ("Starting destructor for Listener::Rep_ ({})"_f, static_cast<const void*> (this));
    }
#endif

    // The listening thread: accepts each connection on masterSockets and hands it to newConnectionAcceptor, until aborted.
    static void ListenLoop_ (const Sequence<ConnectionOrientedMasterSocket::Ptr>&                             masterSockets,
                             const function<void (const ConnectionOrientedStreamSocket::Ptr& newConnection)>& newConnectionAcceptor)
    {
        constexpr Time::DurationSeconds                     kFirstRetryWait_{10ms};
        constexpr Time::DurationSeconds                     kMaxRetryWait_{500ms};
        Debug::TraceContextBumper                           ctx{"Listener::Rep_::ListenLoop_"};
        Time::DurationSeconds                               retryWait{0s};
        WaitForIOReady<ConnectionOrientedMasterSocket::Ptr> sockSetPoller{masterSockets};
        while (true) {
            try {
                // wait on ALL the sockets from all the bound addresses, and accept on EACH that is ready
                for (const auto& readyMasterSocket : sockSetPoller.WaitQuietly ()) {
                    ConnectionOrientedStreamSocket::Ptr s = readyMasterSocket.Accept ();
                    retryWait                             = 0s;
                    try {
                        newConnectionAcceptor (s);
                    }
                    catch (const Execution::Thread::AbortException&) {
                        Execution::ReThrow ();
                    }
                    catch (...) {
                        // its connection is off the queue, so nothing to wait for
                        DbgTrace ("Exception from newConnectionAcceptor: {} - ignored"_f, current_exception ());
                    }
                }
            }
            catch (const Execution::Thread::AbortException&) {
                Execution::ReThrow ();
            }
            catch (...) {
                // WaitQuietly or Accept failed (an exception from newConnectionAcceptor is caught above). Two kinds of cause:
                //   - one-off: the client reset its connection before the accept (ECONNABORTED) - the next try works;
                //   - lasting: the process or system is out of file descriptors (EMFILE, ENFILE) or kernel memory (ENOBUFS) - on
                //     Linux the connection stays queued (macOS's accept drops it), so the listening socket stays ready, and retrying
                //     at once would spin at full CPU until the cause goes.
                // Telling them apart takes a per-platform list of errors, and a lasting cause missing from it would still spin - so
                // every retry waits: briefly after one failure (all a one-off costs), doubling while they continue, up to half a
                // second (nginx's pause after EMFILE). A successful Accept resets it.
                retryWait = retryWait == 0s ? kFirstRetryWait_ : min (retryWait * 2, kMaxRetryWait_);
                DbgTrace ("Exception accepting new connection: {} - retrying in {}"_f, current_exception (), retryWait);
                Execution::Sleep (retryWait);
            }
        }
    }

    Execution::Thread::CleanupPtr fListenThread{Execution::Thread::CleanupPtr::eAbortBeforeWaiting};
};

/*
********************************************************************************
**************************** IO::Network::Listener *****************************
********************************************************************************
*/
const unsigned int Listener::kDefaultBacklog = SOMAXCONN;

Listener::Listener (const SocketAddress& addr,
                    const function<void (const ConnectionOrientedStreamSocket::Ptr& newConnection)>& newConnectionAcceptor, unsigned int backlog)
    : Listener{Sequence<SocketAddress>{addr}, ConnectionOrientedMasterSocket::BindFlags{}, newConnectionAcceptor, backlog}
{
}

Listener::Listener (const SocketAddress& addr, const ConnectionOrientedMasterSocket::BindFlags& bindFlags,
                    const function<void (const ConnectionOrientedStreamSocket::Ptr& newConnection)>& newConnectionAcceptor, unsigned int backlog)
    : Listener{Sequence<SocketAddress>{addr}, bindFlags, newConnectionAcceptor, backlog}
{
}

Listener::Listener (const Traversal::Iterable<SocketAddress>& addrs,
                    const function<void (const ConnectionOrientedStreamSocket::Ptr& newConnection)>& newConnectionAcceptor, unsigned int backlog)
    : Listener{addrs, ConnectionOrientedMasterSocket::BindFlags{}, newConnectionAcceptor, backlog}
{
}

Listener::Listener (const Traversal::Iterable<SocketAddress>& addrs, const ConnectionOrientedMasterSocket::BindFlags& bindFlags,
                    const function<void (const ConnectionOrientedStreamSocket::Ptr& newConnection)>& newConnectionAcceptor, unsigned int backlog)
    : fRep_{Memory::MakeSharedPtr<Rep_> (addrs, bindFlags, backlog, newConnectionAcceptor)}
{
}

Listener::Listener (Listener&& rhs) noexcept
    : fRep_{move (rhs.fRep_)}
{
}
