/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

#if qPlatform_POSIX
#include <poll.h>
#include <unistd.h>
#elif qPlatform_Windows
#include <Windows.h>

#include <winsock2.h>

#include <ws2tcpip.h>
#endif

#include "../Memory/BLOB.h"
#include "../Memory/SmallStackBuffer.h"
#include "../Time/Realtime.h"

#include "Exceptions.h"
#if qPlatform_Windows
#include "Platform/Windows/Exception.h"
#include "Platform/Windows/WaitSupport.h"
#endif

#include "../IO/Network/ConnectionOrientedMasterSocket.h"
#include "../IO/Network/ConnectionOrientedStreamSocket.h"

#include "WaitForIOReady.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Execution::WaitForIOReady_Support;

using Memory::SmallStackBuffer;
using Time::DurationSecondsType;

namespace {
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
        bool fIsSet_{false};
    };

    /*
     *  This strategy may not be the most efficient (esp to construct) but it should work
     *  portably, so implemented first.
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
            return pair<SDKPollableType, WaitForIOReady_Base::TypeOfMonitorSet>{fReadSocket_.GetNativeSocket (), WaitForIOReady_Base::TypeOfMonitorSet{WaitForIOReady_Base::TypeOfMonitor::eRead}};
        }
        virtual void _ReadAllAvail () override
        {
            std::byte buf[1024];
            while (fReadSocket_.ReadNonBlocking (begin (buf), end (buf)))
                ;
        }
        virtual void _WriteOne () override
        {
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
    // @todo - See https://stroika.atlassian.net/browse/STK-709
    // to support eventfd and pipe based helper classes
    /// need ifdefs to allow build based on eventfd, or pipe
    return make_unique<EventFD_Based_SocketPair_> ();
}

/*
 ********************************************************************************
 **************** Execution::WaitForIOReady::WaitForIOReady_Base ****************
 ********************************************************************************
 */
#if qCompiler_cpp17InlineStaticMemberOfClassDoubleDeleteAtExit_Buggy
const WaitForIOReady_Base::TypeOfMonitorSet WaitForIOReady_Base::kDefaultTypeOfMonitor{TypeOfMonitor::eRead};
#endif
auto WaitForIOReady_Base::_WaitQuietlyUntil (const pair<SDKPollableType, TypeOfMonitorSet>* start, const pair<SDKPollableType, TypeOfMonitorSet>* end, Time::DurationSecondsType timeoutAt) -> Containers::Set<size_t>
{
    DurationSecondsType time2Wait = Math::AtLeast<Time::DurationSecondsType> (timeoutAt - Time::GetTickCount (), 0);
    Thread::CheckForInterruption ();
    SmallStackBuffer<pollfd> pollData;
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
    [[maybe_unused]] int timeout_msecs = Math::Round<int> (time2Wait * 1000);
    Assert (timeout_msecs >= 0);
    int pollResult;
#if qPlatform_Windows
#if qStroika_Foundation_Exececution_WaitForIOReady_BreakWSAPollIntoTimedMillisecondChunks > 0
    while (true) {
        Thread::CheckForInterruption ();
        DurationSecondsType timeLeft2Wait                 = Math::AtLeast<Time::DurationSecondsType> (timeoutAt - Time::GetTickCount (), 0);
        DurationSecondsType time2WaitThisLoop             = Math::AtLeast<Time::DurationSecondsType> (Math::AtMost<Time::DurationSecondsType> (timeLeft2Wait, qStroika_Foundation_Exececution_WaitForIOReady_BreakWSAPollIntoTimedMillisecondChunks / 1000.0), 0);
        int                 time2WaitMillisecondsThisLoop = static_cast<int> (time2WaitThisLoop * 1000);
        if ((pollResult = ::WSAPoll (pollData.begin (), static_cast<ULONG> (pollData.GetSize ()), time2WaitMillisecondsThisLoop)) == SOCKET_ERROR) {
            Execution::ThrowSystemErrNo (::WSAGetLastError ());
        }
        if (pollResult != 0 or Time::GetTickCount () >= timeoutAt) {
            break;
        }
    }
#else
    if ((pollResult = ::WSAPoll (pollData.begin (), static_cast<ULONG> (pollData.GetSize ()), timeout_msecs)) == SOCKET_ERROR) {
        Platform::Windows::Exception::Throw (::WSAGetLastError ());
    }
#endif
#else
    pollResult = Handle_ErrNoResultInterruption ([&] () { return ::poll (pollData.begin (), pollData.GetSize (), timeout_msecs); });
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
