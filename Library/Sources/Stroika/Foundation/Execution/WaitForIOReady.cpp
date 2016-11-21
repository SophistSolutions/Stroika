/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#if     qPlatform_POSIX
#include    <poll.h>
#include    <unistd.h>
#elif   qPlatform_Windows
#include    <Windows.h>
#include    <winsock2.h>
#include    <ws2tcpip.h>
#endif

#include    "../Memory/SmallStackBuffer.h"
#include    "../Time/Realtime.h"

#if     qPlatform_Windows
#include    "Platform/Windows/WaitSupport.h"
#endif

#include    "WaitForIOReady.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;

using   Memory::SmallStackBuffer;
using   Time::DurationSecondsType;







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

void    WaitForIOReady::Add (FileDescriptorType fd, TypeOfMonitor flags)
{
    //tmphack - fix events
    short   events   =  flags == TypeOfMonitor::eRead ? POLLIN : 0;
	fPollData_.rwget ()->Add (pair<FileDescriptorType, short> { fd, events });
}

void    WaitForIOReady::AddAll (const Traversal::Iterable<FileDescriptorType>& fds, TypeOfMonitor flags)
{
    for (auto i : fds) {
        Add (i, flags);
    }
}

void    WaitForIOReady::Remove (FileDescriptorType fd)
{
    // fFDs_.rwget ()->Remove (fd);
}

void    WaitForIOReady::RemoveAll (const Traversal::Iterable<FileDescriptorType>& fds)
{
    // fFDs_.rwget ()->RemoveAll (fds);
}

auto WaitForIOReady::GetDescriptors () const -> Set<FileDescriptorType> {
    Set<FileDescriptorType> result;
    auto    lockedPollData      { fPollData_.cget () };
    for (auto i : lockedPollData.cref ())
    {
        result.Add (i.first);
    }
    return result;
}

void        WaitForIOReady::SetDescriptors (const Traversal::Iterable<FileDescriptorType>& fds)
{
//   fFDs_ = Set<FileDescriptorType> { fds };
}

auto     WaitForIOReady::WaitUntil (Time::DurationSecondsType timeoutAt) -> Set<FileDescriptorType> {
    //Set<FileDescriptorType>     fds     { fFDs_.load () };
    //size_t                      sz      { fds.size () };
    Set<FileDescriptorType>     result;
    DurationSecondsType time2Wait = timeoutAt - Time::GetTickCount ();
    if (time2Wait > 0)
    {
        SmallStackBuffer<pollfd>    pollData (0);
        {
            auto    lockedPollData      { fPollData_.cget () };
            size_t  sz                  { lockedPollData->size () };
            pollData.GrowToSize (sz);
            size_t  idx = 0;
            for (auto i : lockedPollData.cref ()) {
                pollData[idx] = pollfd { i.first, i.second, 0 };
                Assert (pollData[idx].revents == 0);
            }
        }
        // USE ppoll? Also verify meaning of timeout, as docs on http://linux.die.net/man/2/poll seem to suggest
        // I ahve this wrong but I susepct docs wrong (says "The timeout argument specifies the minimum number of milliseconds that poll() will block"
        // which sounds backward...
        int timeout_msecs = timeoutAt * 1000;
#if qPlatform_Windows
        int ret = ::WSAPoll (pollData.begin (), pollData.GetSize (), timeout_msecs);
#else
        int ret = ::poll (pollData.begin (), pollData.GetSize (), timeout_msecs);
#endif
        if (ret > 0) {
            for (size_t i = 0; i < pollData.GetSize (); ++i) {
                if (pollData[i].revents != 0) {
                    result.Add (pollData[i].fd);
                }
            }
        }
    }
    return result;
}
