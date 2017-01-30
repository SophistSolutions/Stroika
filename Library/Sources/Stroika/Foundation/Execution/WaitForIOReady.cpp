/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
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
WaitForIOReady::WaitForIOReady (const Traversal::Iterable<FileDescriptorType>& fds)
{
    for (auto i : fds) {
        Add (i);
    }
}

void WaitForIOReady::Add (FileDescriptorType fd, TypeOfMonitor flags)
{
    fPollData_.rwget ()->Add (pair<FileDescriptorType, TypeOfMonitor>{fd, flags});
}

void WaitForIOReady::Remove (FileDescriptorType fd)
{
    AssertNotImplemented ();
}

void WaitForIOReady::Remove (FileDescriptorType fd, TypeOfMonitor flags)
{
    AssertNotImplemented ();
}

void WaitForIOReady::RemoveAll (const Traversal::Iterable<FileDescriptorType>& fds)
{
    AssertNotImplemented ();
}

void WaitForIOReady::RemoveAll (const Traversal::Iterable<pair<FileDescriptorType, TypeOfMonitor>>& fds)
{
    AssertNotImplemented ();
}

void WaitForIOReady::SetDescriptors (const Traversal::Iterable<pair<FileDescriptorType, TypeOfMonitor>>& fds)
{
    fPollData_.store (fds);
}

auto WaitForIOReady::WaitUntil (Time::DurationSecondsType timeoutAt) -> Set<FileDescriptorType>
{
    Set<FileDescriptorType> result;
    DurationSecondsType     time2Wait = timeoutAt - Time::GetTickCount ();
    CheckForThreadInterruption ();
    if (time2Wait > 0) {
        SmallStackBuffer<pollfd> pollData (0);
        {
            auto   lockedPollData = fPollData_.cget ();
            size_t sz             = lockedPollData->size ();
            pollData.GrowToSize (sz);
            size_t idx = 0;
            for (auto i : lockedPollData.cref ()) {
                short events  = (i.second == TypeOfMonitor::eRead) ? POLLIN : 0;
                pollData[idx] = pollfd{i.first, events, 0};
                Assert (pollData[idx].revents == 0);
                idx++;
            }
        }
        // USE ppoll? Also verify meaning of timeout, as docs on http://linux.die.net/man/2/poll seem to suggest
        // I ahve this wrong but I susepct docs wrong (says "The timeout argument specifies the minimum number of milliseconds that poll() will block"
        // which sounds backward...
        int timeout_msecs = static_cast<int> (::round (timeoutAt * 1000));
#if qPlatform_Windows
        if (::WSAPoll (pollData.begin (), pollData.GetSize (), timeout_msecs) == SOCKET_ERROR) {
            Platform::Windows::Exception::Throw (::WSAGetLastError ());
        }
#else
        Handle_ErrNoResultInterruption ([&]() { return ::poll (pollData.begin (), pollData.GetSize (), timeout_msecs); });
#endif
        for (size_t i = 0; i < pollData.GetSize (); ++i) {
            if (pollData[i].revents != 0) {
                result.Add (pollData[i].fd);
            }
        }
    }
    return result;
}
