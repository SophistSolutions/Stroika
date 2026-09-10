/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <optional>
#include <version>
#if __cpp_lib_jthread >= 201911
#include <stop_token>
#endif

#if qStroika_Foundation_Common_Platform_POSIX
#include <csignal>
#include <poll.h>
#include <pthread.h>
#include <unistd.h>
#elif qStroika_Foundation_Common_Platform_Windows
#include <Windows.h>

#include <winsock2.h>

#include <ws2tcpip.h>
#endif

#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"
#include "Stroika/Foundation/Time/Realtime.h"

#include "Exceptions.h"
#include "Finally.h"
#include "Thread.h"
#if qStroika_Foundation_Common_Platform_Windows
#include "Platform/Windows/WaitSupport.h"
#endif

#include "Stroika/Foundation/IO/Network/ConnectionOrientedMasterSocket.h"
#include "Stroika/Foundation/IO/Network/ConnectionOrientedStreamSocket.h"

#include "WaitForIOReady.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Execution::WaitForIOReady_Support;
using namespace Stroika::Foundation::IO::Network;

using std::byte;

using Memory::BLOB;
using Memory::StackBuffer;
using Time::DurationSeconds;
using Time::TimePointSeconds;

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

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
        static const inline BLOB sSingleEltDatum{BLOB ({1})};

        EventFD_Based_SocketPair_ ()
        {
            Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs ("EventFD_Based_SocketPair_::CTOR")};
            auto [r, w]   = ConnectionOrientedStreamSocket::NewPair (SocketAddress::FamilyType::INET, Socket::Type::STREAM);
            fReadSocket_  = r;
            fWriteSocket_ = w;
        }
        ConnectionOrientedStreamSocket::Ptr fReadSocket_{nullptr};
        ConnectionOrientedStreamSocket::Ptr fWriteSocket_{nullptr};

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
            while (fReadSocket_.ReadNonBlocking (buf))
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
    Debug::TraceContextBumper ctx{"WaitForIOReady_Support::mkEventFD"};
    // @todo - See https://github.com/SophistSolutions/Stroika/issues/843 (STK-709)
    // to support eventfd and pipe based helper classes
    /// need ifdefs to allow build based on eventfd, or pipe
    return make_unique<EventFD_Based_SocketPair_> ();
}

#if qStroika_Foundation_Execution_WaitForIOReady_UseStopTokenAbortWakeup
namespace {
    /*
     *  The wakeup channel a thread's abort writes to. One per thread, created on first wait and kept for the
     *  life of the thread: a thread can only be inside one of these waits at a time, so one suffices, and
     *  creating one is not cheap - the portable implementation is a loopback socket pair (@see
     *  https://github.com/SophistSolutions/Stroika/issues/843 for the cheaper eventfd/pipe versions).
     */
    EventFD& GetThreadAbortWakeupEventFD_ ()
    {
        static thread_local std::unique_ptr<EventFD> tWakeup_{WaitForIOReady_Support::mkEventFD ()};
        return *tWakeup_;
    }
    /*
     *  Deliberately a named type rather than a lambda: std::stop_callback is a template on its callback
     *  type, so that type must be nameable to hold the registration in an optional<>.
     *
     *  ***This must not throw, and that is not automatic.*** It runs on whichever thread called Abort () -
     *  or, when stop was already requested, on the WAITING thread from inside the stop_callback constructor.
     *  Either of those can itself be mid-abort, and EventFD::Set () writes to a socket, which is a
     *  cancelation point - so without suppressing interruption it throws Thread::AbortException here, and a
     *  throw out of a stop_callback is std::terminate, both by this function's noexcept and by the
     *  standard's own rules for stop_callback. That is not theoretical: it terminated Tests/40 in 4 runs
     *  out of 6 before the suppression below was added.
     *
     *  It also cannot block: EventFD::Set () writes at most one byte, and only when not already set, so it
     *  cannot stall on a full socket buffer.
     */
    struct SetEventFD_ {
        EventFD* fEventFD;
        void     operator() () const noexcept
        {
            Thread::SuppressInterruptionInContext suppressInterruption;
            try {
                fEventFD->Set ();
            }
            catch (...) {
                // Nothing safe to do here but carry on: the waiter then falls back on its timeout, which is
                // bad, but terminating the process is worse.
                DbgTrace ("WARNING: WaitForIOReady abort wakeup Set () failed: {}"_f, Characters::ToString (std::current_exception ()));
            }
        }
    };
}
#endif

/*
 ********************************************************************************
 **************** Execution::WaitForIOReady::WaitForIOReady_Base ****************
 ********************************************************************************
 */
auto WaitForIOReady_Base::_WaitQuietlyUntil (const pair<SDKPollableType, TypeOfMonitorSet>* start,
                                             const pair<SDKPollableType, TypeOfMonitorSet>* end, TimePointSeconds timeoutAt) -> Containers::Set<size_t>
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"WaitForIOReady_Base::_WaitQuietlyUntil", "args={}"_f, vector<pair<SDKPollableType, TypeOfMonitorSet>>{start, end}};
#endif
    DurationSeconds time2Wait = Math::AtLeast<DurationSeconds> (timeoutAt - Time::GetTickCount (), 0s);
    Thread::CheckForInterruption ();
    const size_t nMonitored   = static_cast<size_t> (end - start);
    EventFD*     abortWakeup_ = nullptr; // non-null once this wait can be woken by an abort
#if qStroika_Foundation_Execution_WaitForIOReady_UsePPoll
    /*
     *  The SECOND of the three wakeup mechanisms - @see
     *  qStroika_Foundation_Execution_WaitForIOReady_UsePPoll.
     *
     *  Block the interrupt signal FIRST, and check for interruption with it blocked (which catches an abort
     *  that already happened). The original mask then goes to ppoll (), which unblocks the signal atomically
     *  for the duration of the wait, so an abort landing anywhere in the setup window leaves the signal
     *  pending instead of spending it.
     */
    sigset_t originalSignalMask;
    {
        sigset_t blockInterruptSignal;
        ::sigemptyset (&blockInterruptSignal);
        ::sigaddset (&blockInterruptSignal, Thread::SignalUsedForThreadInterrupt ());
        Verify (::pthread_sigmask (SIG_BLOCK, &blockInterruptSignal, &originalSignalMask) == 0);
    }
    // must restore on EVERY exit path - the CheckForInterruption () below throws, and so can the wait
    [[maybe_unused]] auto&& restoreSignalMask =
        Finally ([&originalSignalMask] () noexcept { ::pthread_sigmask (SIG_SETMASK, &originalSignalMask, nullptr); });
#endif
#if qStroika_Foundation_Execution_WaitForIOReady_UseStopTokenAbortWakeup
    /*
     *  Arm the abort wakeup BEFORE building the poll set, and keep it armed across the wait. @see the
     *  qStroika_Foundation_Execution_WaitForIOReady_UseStopTokenAbortWakeup docs for why this closes a window
     *  the interrupt signal alone cannot: the emplace () below runs the callback itself if stop was already
     *  requested, and Thread::Abort () runs it if stop is requested later - so the wakeup descriptor ends up
     *  readable either way, and no ordering of the two can lose it.
     */
    std::optional<std::stop_callback<SetEventFD_>> abortWakeupRegistration;
    if (std::optional<std::stop_token> stopToken = Thread::GetCurrentThreadStopToken ()) {
        /*
         *  If the abort has ALREADY landed - which includes landing in the window between the
         *  CheckForInterruption () above and here, the very window this code exists to close - then there is
         *  no wait to wake: just throw, which is where entering the wait would have had to end up anyway.
         *
         *  Arming in that case would be actively wrong, not merely wasteful: std::stop_callback runs its
         *  callback during CONSTRUCTION when stop is already requested, and that construction happens right
         *  here, on a thread that is itself mid-abort. @see SetEventFD_.
         */
        if (stopToken->stop_requested ()) {
            Thread::CheckForInterruption (); // throws - unless interruption is suppressed in this context,
                                             // in which case arming below is safe for the same reason
        }
        abortWakeup_ = &GetThreadAbortWakeupEventFD_ ();
        abortWakeup_->Clear (); // discard anything left over from an earlier wait on this thread
        abortWakeupRegistration.emplace (*stopToken, SetEventFD_{abortWakeup_});
    }
#endif
    StackBuffer<pollfd> pollData;
    {
        pollData.GrowToSize_uninitialized (nMonitored + (abortWakeup_ == nullptr ? 0 : 1));
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
                    case TypeOfMonitor::ePriority:
                        events |= POLLPRI;
                        break;
                }
            }
            pollData[idx] = pollfd{i->first, events, 0};
            Assert (pollData[idx].revents == 0);
            idx++;
        }
        if (abortWakeup_ != nullptr) {
            Assert (idx == nMonitored);
            // read-monitored: it becomes readable when the abort callback Set ()s it
            pollData[idx] = pollfd{abortWakeup_->GetWaitInfo ().first, POLLIN, 0};
        }
    }
    [[maybe_unused]] int timeoutMilliseconds = Math::Round<int> (time2Wait.count () * 1000);
    Assert (timeoutMilliseconds >= 0);
    int pollResult;
    /*
     *  Exactly ONE wakeup mechanism is compiled in per configuration - @see the three
     *  qStroika_Foundation_Execution_WaitForIOReady_* macros. Stacking them would only let a failure of the
     *  primary one show up as a delay rather than as a failure.
     */
#if qStroika_Foundation_Common_Platform_Windows
#if !qStroika_Foundation_Execution_WaitForIOReady_UseStopTokenAbortWakeup
    Require (sPollWaitChunkTime > 0s);
    while (true) {
        Thread::CheckForInterruption ();
        DurationSeconds timeLeft2Wait                 = Math::AtLeast<DurationSeconds> (timeoutAt - Time::GetTickCount (), 0s);
        DurationSeconds time2WaitThisLoop             = clamp<DurationSeconds> (timeLeft2Wait, 0s, sPollWaitChunkTime);
        int             time2WaitMillisecondsThisLoop = static_cast<int> (time2WaitThisLoop.count () * 1000);
        if ((pollResult = ::WSAPoll (pollData.begin (), static_cast<ULONG> (pollData.GetSize ()), time2WaitMillisecondsThisLoop)) == SOCKET_ERROR) {
            ThrowSystemErrNo (::WSAGetLastError ());
        }
        if (pollResult != 0 or Time::GetTickCount () >= timeoutAt) {
            break;
        }
    }
#else
    // abortWakeup_ is in this poll set, so one WSAPoll spanning the whole timeout wakes just as promptly
    if ((pollResult = ::WSAPoll (pollData.begin (), static_cast<ULONG> (pollData.GetSize ()), timeoutMilliseconds)) == SOCKET_ERROR) {
        // NB: this called Platform::Windows::Exception::Throw () - a name that has not existed since v3 -
        // until the stop_token wakeup made this branch the live one. It was dead code, so nothing caught it.
        ThrowSystemErrNo (::WSAGetLastError ());
    }
#endif
#elif qStroika_Foundation_Execution_WaitForIOReady_UsePPoll
    // the mask argument is what makes this race-free - @see the signal-blocking code above
    struct timespec timeoutSpec{};
    timeoutSpec.tv_sec  = timeoutMilliseconds / 1000;
    timeoutSpec.tv_nsec = (timeoutMilliseconds % 1000) * 1000000L;
    pollResult          = Handle_ErrNoResultInterruption (
        [&] () { return ::ppoll (pollData.begin (), pollData.GetSize (), &timeoutSpec, &originalSignalMask); });
#elif !qStroika_Foundation_Execution_WaitForIOReady_UseStopTokenAbortWakeup
    Require (sPollWaitChunkTime > 0s);
    while (true) {
        DurationSeconds timeLeft2Wait        = Math::AtLeast<DurationSeconds> (timeoutAt - Time::GetTickCount (), 0s);
        DurationSeconds time2WaitThisLoop    = clamp<DurationSeconds> (timeLeft2Wait, 0s, sPollWaitChunkTime);
        int             thisLoopMilliseconds = static_cast<int> (time2WaitThisLoop.count () * 1000);
        // NB: Handle_ErrNoResultInterruption () both retries on EINTR and is itself a cancelation point
        pollResult = Handle_ErrNoResultInterruption ([&] () { return ::poll (pollData.begin (), pollData.GetSize (), thisLoopMilliseconds); });
        if (pollResult != 0 or Time::GetTickCount () >= timeoutAt) {
            break;
        }
    }
#else
    // abortWakeup_ is in this poll set, so one poll () spanning the whole timeout wakes just as promptly
    pollResult = Handle_ErrNoResultInterruption ([&] () { return ::poll (pollData.begin (), pollData.GetSize (), timeoutMilliseconds); });
#endif
    /*
     *  Where a wakeup becomes an abort - and also where an abort that arrived before the wait even started
     *  gets noticed, since arming the registration above will already have made the descriptor readable.
     */
    Thread::CheckForInterruption ();
    if (abortWakeup_ != nullptr) {
        // No-op unless the wakeup fired. If it did fire and we still got here, interruption is suppressed, so
        // drain it - otherwise every later wait on this thread would return immediately.
        abortWakeup_->Clear ();
    }
    Set<size_t> result;
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Sequence<SDKPollableType> dbgResult;
#endif
    if (pollResult != 0) {
        // nMonitored, NOT pollData.GetSize (): the wakeup channel is ours, not one of the caller's fds
        for (size_t i = 0; i < nMonitored; ++i) {
            if (pollData[i].revents != 0) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                dbgResult += start[i].first;
#endif
                result.Add (i);
            }
        }
    }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace ("returning {}"_f, dbgResult);
#endif
    return result;
}
