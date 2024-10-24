/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qPlatform_POSIX
#include <poll.h>
#include <unistd.h>
#elif qPlatform_Windows
#include <Windows.h>

#include <winsock2.h>

#include <ws2tcpip.h>
#endif

#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"
#include "Stroika/Foundation/Time/Realtime.h"

#include "Exceptions.h"
#if qPlatform_Windows
#include "Platform/Windows/Exception.h"
#include "Platform/Windows/WaitSupport.h"
#endif

#include "Stroika/Foundation/IO/Network/ConnectionOrientedMasterSocket.h"
#include "Stroika/Foundation/IO/Network/ConnectionOrientedStreamSocket.h"

#include "WaitForIOReady.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Execution::WaitForIOReady_Support;

using std::byte;

using Memory::StackBuffer;
using Time::DurationSeconds;
using Time::TimePointSeconds;

namespace {

    /*
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     */
    struct EventFD_Based_ : public EventFD {

        EventFD_Based_ () = default;
        virtual bool IsSet () const override
        {
            return fIsSet_;
        }
        virtual void Set () override
        {
            // If already set, nothing todo. To set, we set flag, and write so anybody selecting will wakeup
            if (not IsSet ()) {
                fIsSet_ = true;
                _WriteOne (); // so select calls wake
            }
        }
        virtual void Clear () override
        {
            if (IsSet ()) {
                fIsSet_ = false;
                _ReadAllAvail (); // so select calls don't prematurely wake
            }
        }

    protected:
        virtual void _ReadAllAvail () = 0;
        virtual void _WriteOne ()     = 0;

    private:
        atomic<bool> fIsSet_{false}; // cuz called from multiple threads - sync
    };

    /*
     *  This strategy may not be the most efficient (esp to construct) but it should work
     *  portably, so implemented first.
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     */
    struct EventFD_Based_SocketPair_ : EventFD_Based_ {
        static const inline Memory::BLOB sSingleEltDatum{Memory::BLOB ({1})};

        EventFD_Based_SocketPair_ ()
        {
            Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"EventFD_Based_SocketPair_::CTOR")};
            using namespace IO::Network;

            // Create a Listening master socket, bind it, and get it listening
            // Just needed temporarily to create the socketpair, then it can be closed when it goes out of scope
            auto connectionOrientedMaster = ConnectionOrientedMasterSocket::New (SocketAddress::FamilyType::INET, Socket::Type::STREAM);
            connectionOrientedMaster.Bind (SocketAddress{IO::Network::V4::kLocalhost});
            connectionOrientedMaster.Listen (1);

            // now make a NEW socket, with the bound address and connect;
            fReadSocket_  = ConnectionOrientedStreamSocket::NewConnection (*connectionOrientedMaster.GetLocalAddress ());
            fWriteSocket_ = connectionOrientedMaster.Accept ();
        }
        IO::Network::ConnectionOrientedStreamSocket::Ptr fReadSocket_{nullptr};
        IO::Network::ConnectionOrientedStreamSocket::Ptr fWriteSocket_{nullptr};

        virtual pair<SDKPollableType, WaitForIOReady_Base::TypeOfMonitorSet> GetWaitInfo () override
        {
            // Poll on read FD to see if data available to read
            return pair<SDKPollableType, WaitForIOReady_Base::TypeOfMonitorSet>{
                fReadSocket_.GetNativeSocket (), WaitForIOReady_Base::TypeOfMonitorSet{WaitForIOReady_Base::TypeOfMonitor::eRead}};
        }
        virtual void _ReadAllAvail () override
        {
            // thread safety OK cuz only reading from Ptr (nobody writes) and socket rep internally synchronized
            byte buf[1024];
            while (fReadSocket_.ReadNonBlocking (begin (buf), end (buf)))
                ;
        }
        virtual void _WriteOne () override
        {
            // thread safety OK cuz only reading from Ptr (nobody writes) and socket rep internally synchronized
            fWriteSocket_.Write (sSingleEltDatum);
        }
    };

}

/*
 ********************************************************************************
 *********** Execution::WaitForIOReady::WaitForIOReady_Support::mkEventFD *******
 ********************************************************************************
 */
unique_ptr<EventFD> WaitForIOReady_Support::mkEventFD ()
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"WaitForIOReady_Support::mkEventFD")};
    // @todo - See http://stroika-bugs.sophists.com/browse/STK-709
    // to support eventfd and pipe based helper classes
    /// need ifdefs to allow build based on eventfd, or pipe
    return make_unique<EventFD_Based_SocketPair_> ();
}

/*
 ********************************************************************************
 **************** Execution::WaitForIOReady::WaitForIOReady_Base ****************
 ********************************************************************************
 */
auto WaitForIOReady_Base::_WaitQuietlyUntil (const pair<SDKPollableType, TypeOfMonitorSet>* start,
                                             const pair<SDKPollableType, TypeOfMonitorSet>* end, TimePointSeconds timeoutAt) -> Containers::Set<size_t>
{
    DurationSeconds time2Wait = Math::AtLeast<DurationSeconds> (timeoutAt - Time::GetTickCount (), 0s);
    Thread::CheckForInterruption ();
    StackBuffer<pollfd> pollData;
    {
        pollData.GrowToSize_uninitialized (end - start);
        size_t idx = 0;
        for (auto i = start; i != end; ++i) {
            short events = 0;
            for (TypeOfMonitor ii : i->second) {
                switch (ii) {
                    case TypeOfMonitor::eRead:
                        events |= POLLIN;
                        break;
                    case TypeOfMonitor::eWrite:
                        events |= POLLOUT;
                        break;
                    case TypeOfMonitor::eError:
                        events |= POLLERR;
                        break;
                    case TypeOfMonitor::eHUP:
                        events |= POLLHUP;
                        break;
                }
            }
            pollData[idx] = pollfd{i->first, events, 0};
            Assert (pollData[idx].revents == 0);
            idx++;
        }
    }
    // USE ppoll? Also verify meaning of timeout, as docs on http://linux.die.net/man/2/poll seem to suggest
    // I have this wrong but I suspect docs wrong (says "The timeout argument specifies the minimum number of milliseconds that poll() will block"
    // which sounds backward...
    [[maybe_unused]] int timeoutMilliseconds = Math::Round<int> (time2Wait.count () * 1000);
    Assert (timeoutMilliseconds >= 0);
    int pollResult;
#if qPlatform_Windows
#if qStroika_Foundation_Exececution_WaitForIOReady_BreakWSAPollIntoTimedMillisecondChunks > 0
    while (true) {
        Thread::CheckForInterruption ();
        DurationSeconds timeLeft2Wait     = Math::AtLeast<DurationSeconds> (timeoutAt - Time::GetTickCount (), 0s);
        DurationSeconds time2WaitThisLoop = Math::AtLeast<DurationSeconds> (
            Math::AtMost<DurationSeconds> (
                timeLeft2Wait, DurationSeconds{qStroika_Foundation_Exececution_WaitForIOReady_BreakWSAPollIntoTimedMillisecondChunks / 1000.0}),
            0s);
        int time2WaitMillisecondsThisLoop = static_cast<int> (time2WaitThisLoop.count () * 1000);
        if ((pollResult = ::WSAPoll (pollData.begin (), static_cast<ULONG> (pollData.GetSize ()), time2WaitMillisecondsThisLoop)) == SOCKET_ERROR) {
            Execution::ThrowSystemErrNo (::WSAGetLastError ());
        }
        if (pollResult != 0 or Time::GetTickCount () >= timeoutAt) {
            break;
        }
    }
#else
    if ((pollResult = ::WSAPoll (pollData.begin (), static_cast<ULONG> (pollData.GetSize ()), timeoutMilliseconds)) == SOCKET_ERROR) {
        Platform::Windows::Exception::Throw (::WSAGetLastError ());
    }
#endif
#else
    pollResult = Handle_ErrNoResultInterruption ([&] () { return ::poll (pollData.begin (), pollData.GetSize (), timeoutMilliseconds); });
#endif
    Set<size_t> result;
    if (pollResult != 0) {
        for (size_t i = 0; i < pollData.GetSize (); ++i) {
            if (pollData[i].revents != 0) {
                result.Add (i);
            }
        }
    }
    return result;
}
