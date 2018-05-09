/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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

#include "../Execution/TimeOutException.h"
#include "../Memory/SmallStackBuffer.h"
#include "../Time/Realtime.h"

#if qPlatform_POSIX
#include "ErrNoException.h"
#elif qPlatform_Windows
#include "Platform/Windows/Exception.h"
#include "Platform/Windows/WaitSupport.h"
#endif

#include "WaitForIOReady.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;

using Memory::SmallStackBuffer;
using Time::DurationSecondsType;

/*
 ********************************************************************************
 ************************** Execution::WaitForIOReady ***************************
 ********************************************************************************
 */
const WaitForIOReady::TypeOfMonitorSet WaitForIOReady::kDefaultTypeOfMonitor{WaitForIOReady::TypeOfMonitor::eRead};

WaitForIOReady::WaitForIOReady (const Traversal::Iterable<FileDescriptorType>& fds, const TypeOfMonitorSet& flags)
{
    AddAll (fds, flags);
}

WaitForIOReady::WaitForIOReady (const Traversal::Iterable<pair<FileDescriptorType, TypeOfMonitorSet>>& fds)
{
    AddAll (fds);
}

WaitForIOReady::WaitForIOReady (FileDescriptorType fd, const TypeOfMonitorSet& flags)
{
    Add (fd, flags);
}

void WaitForIOReady::Add (FileDescriptorType fd, const TypeOfMonitorSet& flags)
{
    fPollData_.rwget ()->Add (pair<FileDescriptorType, TypeOfMonitorSet>{fd, flags});
}

void WaitForIOReady::Remove (FileDescriptorType fd)
{
    AssertNotImplemented ();
}

void WaitForIOReady::Remove (FileDescriptorType fd, const TypeOfMonitorSet& flags)
{
    AssertNotImplemented ();
}

void WaitForIOReady::RemoveAll (const Traversal::Iterable<FileDescriptorType>& fds)
{
    AssertNotImplemented ();
}

void WaitForIOReady::RemoveAll (const Traversal::Iterable<pair<FileDescriptorType, TypeOfMonitorSet>>& fds)
{
    AssertNotImplemented ();
}

void WaitForIOReady::SetDescriptors (const Traversal::Iterable<pair<FileDescriptorType, TypeOfMonitorSet>>& fds)
{
    fPollData_.store (fds);
}

auto WaitForIOReady::WaitUntil (Time::DurationSecondsType timeoutAt) -> Set<FileDescriptorType>
{
    Set<FileDescriptorType> result = WaitQuietlyUntil (timeoutAt);
    if (result.empty ()) {
        Execution::Throw (Execution::TimeOutException ());
    }
    return result;
}

auto WaitForIOReady::WaitQuietlyUntil (Time::DurationSecondsType timeoutAt) -> Containers::Set<FileDescriptorType>
{
    DurationSecondsType time2Wait = Math::AtLeast<Time::DurationSecondsType> (timeoutAt - Time::GetTickCount (), 0);
    CheckForThreadInterruption ();
    SmallStackBuffer<pollfd> pollData;
    {
        auto   lockedPollData = fPollData_.cget ();
        size_t sz             = lockedPollData->size ();
        pollData.GrowToSize (sz);
        size_t idx = 0;
        for (auto i : lockedPollData.cref ()) {
            short events = 0;
            for (TypeOfMonitor ii : i.second) {
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
            pollData[idx] = pollfd{i.first, events, 0};
            Assert (pollData[idx].revents == 0);
            idx++;
        }
    }
    // USE ppoll? Also verify meaning of timeout, as docs on http://linux.die.net/man/2/poll seem to suggest
    // I have this wrong but I suspect docs wrong (says "The timeout argument specifies the minimum number of milliseconds that poll() will block"
    // which sounds backward...
    int timeout_msecs = Math::Round<int> (time2Wait * 1000);
    Assert (timeout_msecs >= 0);
    int pollResult;
#if qPlatform_Windows
    if ((pollResult = ::WSAPoll (pollData.begin (), static_cast<ULONG> (pollData.GetSize ()), timeout_msecs)) == SOCKET_ERROR) {
        Platform::Windows::Exception::Throw (::WSAGetLastError ());
    }
#else
    pollResult = ThrowErrNoIfNegative (Handle_ErrNoResultInterruption ([&]() { return ::poll (pollData.begin (), pollData.GetSize (), timeout_msecs); }));
#endif
    Set<FileDescriptorType> result;
    if (pollResult != 0) {
        for (size_t i = 0; i < pollData.GetSize (); ++i) {
            if (pollData[i].revents != 0) {
                result.Add (pollData[i].fd);
            }
        }
    }
    return result;
}
