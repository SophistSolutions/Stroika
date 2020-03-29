/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
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

#include "../Memory/SmallStackBuffer.h"
#include "../Time/Realtime.h"

#include "Exceptions.h"
#if qPlatform_Windows
#include "Platform/Windows/Exception.h"
#include "Platform/Windows/WaitSupport.h"
#endif

#include "WaitForIOReady.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Execution::WaitForIOReady_Support;

using Memory::SmallStackBuffer;
using Time::DurationSecondsType;

/*
 ********************************************************************************
 **************** Execution::WaitForIOReady::WaitForIOReady_Base ****************
 ********************************************************************************
 */
const WaitForIOReady_Base::TypeOfMonitorSet WaitForIOReady_Base::kDefaultTypeOfMonitor{WaitForIOReady_Base::TypeOfMonitor::eRead};

auto WaitForIOReady_Base::_WaitQuietlyUntil (const pair<SDKPollableType, TypeOfMonitorSet>* start, const pair<SDKPollableType, TypeOfMonitorSet>* end, Time::DurationSecondsType timeoutAt) -> Containers::Set<size_t>
{
    DurationSecondsType time2Wait = Math::AtLeast<Time::DurationSecondsType> (timeoutAt - Time::GetTickCount (), 0);
    CheckForThreadInterruption ();
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
        Execution::CheckForThreadInterruption ();
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
    pollResult = ThrowPOSIXErrNoIfNegative (Handle_ErrNoResultInterruption ([&] () { return ::poll (pollData.begin (), pollData.GetSize (), timeout_msecs); }));
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
