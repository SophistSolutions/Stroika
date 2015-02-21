/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#if     qPlatform_POSIX
#include    <poll.h>
#include    <unistd.h>
#elif   qPlatform_Windows
#include    <Windows.h>
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


#if     qPlatform_POSIX && !defined (qUse_ppoll_)
#define qUse_ppoll_                     1
#elif   qPlatform_Windows && !defined (qUse_WaitForMultipleEventsEx_)
#define qUse_WaitForMultipleEventsEx_   1
#endif


#ifndef qUse_ppoll_
#define qUse_ppoll_ 0
#endif
#ifndef qUse_WaitForMultipleEventsEx_
#define qUse_WaitForMultipleEventsEx_   0
#endif




// @todo - support epoll, poll, select, pselect() etc.
// @todo - write variant with WaitForMultipleEventsEx()

// @todo There are a variety of reworks / tricks we could do to make succfessive calls cheaper, depending on the technology.
// @We could move the ADD/REMVOE calls inside the .cpp file, and then incrmeenetlaly update a cached representation handled
// to the OS

// @todo THINK OUT signal flags/params to ppoll()




/*
 ********************************************************************************
 ************************** Execution::WaitForIOReady ***************************
 ********************************************************************************
 */
auto     WaitForIOReady::WaitUntil (Time::DurationSecondsType timeoutAt) -> Set<FileDescriptorType> {
    Set<FileDescriptorType>     fds =   fFDs_.load ();
    size_t                      sz      =   fds.size ();
    Set<FileDescriptorType>     result;
#if     qUse_ppoll_
    DurationSecondsType time2Wait = timeoutAt - Time::GetTickCount ();
    if (time2Wait > 0 and sz > 0)
    {
        SmallStackBuffer<pollfd>    pollData (sz);
        memset (pollData.begin (), 0, sz * sizeof (pollfd));
        size_t i = 0;
        for (FileDescriptorType fd : fds) {
            pollfd* pd = &pollData[i];
            pd->fd = fd;
            pd->events = POLLOUT | POLLWRBAND;
            Assert (pd->revents == 0);
            ++i;
        }
        int timeout_msecs = timeoutAt * 1000;
        int ret = poll(fds, 2, timeout_msecs);
        if (ret > 0) {
            for (size_t i = 0; i < sz; ++i) {
                if (pollData[i].revents != 0) {
                    result.Add (pollData[i].fd);
                }
            }
        }
    }
#elif   qUse_WaitForMultipleEventsEx_
    DurationSecondsType time2Wait = timeoutAt - Time::GetTickCount ();
    if (time2Wait > 0 and sz > 0)
    {
        SmallStackBuffer<HANDLE>    pollData (sz);
        memset (pollData.begin (), 0, sz * sizeof (HANDLE));
        size_t i = 0;
        for (FileDescriptorType fd : fds) {
            pollData[i] = (HANDLE)fd;
            ++i;
        }
        constexpr   bool    kWaitAll_       { false };  // return when any one changes
        constexpr   bool    kAlertable_     { true };   // so it can be interupted via thread abort
        int ret = WaitForMultipleObjectsEx (sz, pollData.begin (), kWaitAll_, Execution::Platform::Windows::Duration2Milliseconds (time2Wait), kAlertable_);
        if (static_cast<int> (WAIT_OBJECT_0)  <= ret and ret <= static_cast<int> (WAIT_OBJECT_0 + sz - 1)) {
            // With this API, we can only find the first
            result.Add (reinterpret_cast<int> (pollData[ret - WAIT_OBJECT_0]));
        }
    }
#else
    AssertNotImplemented ();
#endif
    return result;
}
