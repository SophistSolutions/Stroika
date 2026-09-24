/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_WaitForIOReady_h_
#define _Stroika_Foundation_Execution_WaitForIOReady_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <version>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedChecker.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/IO/Network/Socket.h"
#include "Stroika/Foundation/Time/Duration.h"

/**
 *  \file
 *
 *      WaitForIOReady utility - portably provide facility to check a bunch of file descriptors/sockets
 *      if input is ready (like select, epoll, WaitForMutlipleObjects, etc)
 *
 *  \note   ***Windows takes sockets and nothing else.*** The underlying WSAPoll () accepts no other kind
 *          of handle, and WaitForMultipleObjectsEx () was tried instead and worked poorly. This is not just
 *          a restriction on callers: it is why the wakeup channel EventFD hands out has to be a (loopback
 *          UDP) socket there, where an event HANDLE would be cheaper. @see mkEventFD () and
 *          https://github.com/SophistSolutions/Stroika/issues/843
 */

namespace Stroika::Foundation::Execution {

/**
 *  Wake a pending wait through the waiting thread's std::stop_token, instead of depending on the
 *  thread-interrupt signal (POSIX) or APC (Windows) to interrupt the underlying poll () call.
 *
 *  The interrupt signal cannot carry this reliably. Its whole effect is the EINTR it causes in a call that
 *  is already in flight, so a signal delivered while the thread is still setting up its poll set runs the
 *  (deliberately empty) handler and is spent, having interrupted nothing - and the thread then sleeps out
 *  its entire timeout. A std::stop_callback has the property the signal lacks: it is guaranteed to run, and
 *  to run DURING ITS OWN CONSTRUCTION if stop was already requested. Registering one to Set () a pollable
 *  EventFD, with that EventFD in the poll set, therefore leaves no window - however the abort and the wait
 *  are ordered, the descriptor ends up readable and poll () returns.
 *
 *  Thread::Abort () already calls stop_source::request_stop (), so nothing on the aborting side changes.
 *
 *  FIRST choice of three, because it is the only one that is portable - it covers Windows and macOS as well
 *  as Linux. @see qStroika_Foundation_Execution_WaitForIOReady_UsePPoll for the fallback used where the
 *  standard library has no jthread, and _BreakPollIntoTimedMillisecondChunks for the last resort.
 *
 *  @see https://github.com/SophistSolutions/Stroika/issues/1165
 */
#ifndef qStroika_Foundation_Execution_WaitForIOReady_UseStopTokenAbortWakeup
#if __cpp_lib_jthread >= 201911
#define qStroika_Foundation_Execution_WaitForIOReady_UseStopTokenAbortWakeup 1
#else
#define qStroika_Foundation_Execution_WaitForIOReady_UseStopTokenAbortWakeup 0
#endif
#endif

/**
 *  SECOND choice, where the stop_token wakeup is unavailable: use ppoll () rather than poll (), blocking the
 *  thread-interrupt signal across the setup window and handing the original mask to the wait, which unblocks
 *  it atomically for exactly the duration of the wait. An abort arriving anywhere in the window then leaves
 *  the signal PENDING rather than spending it, and it fires the instant ppoll () unblocks it.
 *
 *  This closes the race just as completely as the stop_token wakeup does, and it is worth having as well
 *  because the two are available in DIFFERENT configurations: ppoll () is glibc, so it does not care which
 *  C++ standard library is in use, while __cpp_lib_jthread is undefined for every libc++ before LLVM 20. So
 *  the clang++-NN-*-libc++ configurations get this one and not the stop_token path. @see the note on which
 *  standard libraries have jthread/stop_token, in Thread.h.
 *
 *  Defined as 0 whenever the stop_token wakeup is available, so that exactly one mechanism is in play.
 */
#ifndef qStroika_Foundation_Execution_WaitForIOReady_UsePPoll
#if !qStroika_Foundation_Execution_WaitForIOReady_UseStopTokenAbortWakeup && defined(__linux__)
#define qStroika_Foundation_Execution_WaitForIOReady_UsePPoll 1
#else
#define qStroika_Foundation_Execution_WaitForIOReady_UsePPoll 0
#endif
#endif

    namespace WaitForIOReady_Support {

#if !qStroika_Foundation_Execution_WaitForIOReady_UseStopTokenAbortWakeup && !qStroika_Foundation_Execution_WaitForIOReady_UsePPoll
        /**
         *  LAST choice of the three wakeup mechanisms, used only where neither the stop_token wakeup nor
         *  ppoll () is available. There, the wait is broken into chunks this long, re-checking for thread
         *  interruption between them.
         *
         *  This is NOT hypothetical, despite being the fallback: macOS has no ppoll () at all (the UsePPoll
         *  default requires __linux__), and Apple's libc++ had no jthread before Xcode 26 - measured
         *  2026-09-10, Xcode 15 and 16 lack __cpp_lib_jthread while 26.3 has it. So every XCode <= 16 build
         *  lands here, which includes the MacOS-15-XCode-16.4 CI job that runs on every push. Verified
         *  independently by forcing both macros to 0 on Linux/g++: all 54 regression tests pass.
         *
         *  Unlike the other two, this only BOUNDS a lost wakeup rather than preventing one: an abort whose
         *  signal was spent before the wait began costs one chunk instead of the whole timeout. Nothing
         *  better is available in that configuration. It is deliberately NOT used when either other
         *  mechanism is in play - there, waking early could only mask a failure of the mechanism that is
         *  supposed to be doing the work, which is exactly how such a failure goes unnoticed.
         *
         *  Smaller means an abort is noticed sooner, at the cost of more wakeups. Must be > 0.
         *
         *  \note   Deliberately not even DECLARED where one of the other mechanisms is available - which is
         *          every platform Stroika currently builds on except old XCode - so that it cannot be set
         *          somewhere it would have no effect.
         *
         *  \note   ***Set this before creating any threads.*** It is a variable rather than a macro on
         *          purpose: a macro could only be changed by rebuilding Stroika, whereas this lets an
         *          application tune it. But it is not synchronized, so changing it once threads are running
         *          is a data race.
         *
         *  @see ConditionVariable's sConditionVariableWaitChunkTime, which this mirrors.
         */
        static inline Time::DurationSeconds sPollWaitChunkTime{3s};
#endif

        /**
         *  This is the underlying native type 'HighLevelType objects must be converted to in order to
         *  be used with the operating-system poll/select feature.
         */
#if qStroika_Foundation_Common_Platform_Windows
        using SDKPollableType = SOCKET;
#else
        using SDKPollableType = int;
#endif

        /**
         */
        class WaitForIOReady_Base {
        public:
            /**
             *  This is the underlying native type 'T' objects must be converted to in order to be used with the OLD poll/select feature.
             */
            using SDKPollableType = WaitForIOReady_Support::SDKPollableType;

        public:
            /**
             *  @todo consider adding more params - like out of band flags - but doesn't immediately seem helpful -- LGP 2017-04-16
             *
             *  \note   Common::DefaultNames<> supported
             */
            enum class TypeOfMonitor {
                /**
                 *  There is data to read.
                 *
                 *  @see http://man7.org/linux/man-pages/man2/poll.2.html - POLLIN
                 */
                eRead,

                /**
                 *  Writing is now possible.
                 *
                 *  @see http://man7.org/linux/man-pages/man2/poll.2.html - POLLOUT
                 */
                eWrite,

                /**
                 *  Poll Priority/Poll Urgent data (out-of-band data) is available for read - POLLPRI
                 *
                 *  @see https://man7.org/linux/man-pages/man2/poll.2.html - It is used to detect "out-of-band" (OOB) data on sockets or high-priority, non-zero priority band data in STREAMS.
                 */
                ePriority,

                Stroika_Define_Enum_Bounds (eRead, ePriority)
            };

        public:
            using TypeOfMonitorSet = Containers::Set<TypeOfMonitor>;

        public:
            /**
             * default is to watch for read, error, and HUP (close) events (all but write).
             */
            // static inline const TypeOfMonitorSet kDefaultTypeOfMonitor{TypeOfMonitor::eRead, TypeOfMonitor::eError, TypeOfMonitor::eHUP};
            static inline const TypeOfMonitorSet kDefaultTypeOfMonitor{TypeOfMonitor::eRead};

        protected:
            /**
             *  Take an array of pair<SDKPollableType, TypeOfMonitorSet> objects, and return a Set{} with the INDEXES of 'ready' pollable objects.
             *  
             *  \note Design Note: This could have returned a set of pointer to SDKPollableType which would in some sense be simpler and
             *                     clearer, but its easier to validate/assert the returned INDEXES are valid than the returned POINTERS are valid.
             */
            static auto _WaitQuietlyUntil (const pair<SDKPollableType, TypeOfMonitorSet>* start, const pair<SDKPollableType, TypeOfMonitorSet>* end,
                                           Time::TimePointSeconds timeoutAt) -> Containers::Set<size_t>;
        };

        /**
         *  (Private) utility to allow select() to wakeup without sending EINTR such signals...
         *  
         *  \note idea originally from https://stackoverflow.com/questions/12050072/how-to-wake-up-a-thread-being-blocked-by-select-poll-poll-function-from-anothe/22239521
         * 
         *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
         */
        class EventFD {
        public:
            EventFD () = default;

        public:
            virtual ~EventFD () = default;

        public:
            virtual bool IsSet () const = 0;

        public:
            virtual void Set () = 0;

        public:
            virtual void Clear () = 0;

        public:
            // return low level FD + set of poll events
            virtual pair<SDKPollableType, WaitForIOReady_Base::TypeOfMonitorSet> GetWaitInfo () = 0;
        };
        unique_ptr<EventFD> mkEventFD ();

        template <typename T>
        struct WaitForIOReady_Traits {
            /**
             *  This is the type of object which is being wrapped (around a SDKPollableType object) and used with WaitForIOReady
             */
            using HighLevelType = T;

            /**
             *  To use WaitForIOReady, the high level 'descriptor' objects used must be convertible to associated low level
             *  file descriptor objects to use with select/poll/etc...
             */
            static inline WaitForIOReady_Support::SDKPollableType GetSDKPollable (const HighLevelType& t)
            {
                return t;
            }
        };

    }

    /**
     *  Simple portable wrapper on OS select/2, pselect/2, poll/2, epoll (), and/or WaitForMultipleEvents(), etc, 
     *  except you construct it with the 'select' arguments (file descriptors to watch), and then call wait
     *  on the 'waitable' object.
     *
     *  \note   An optional pollable2Wakeup descriptor lets another thread make a pending wait return early, without
     *          timing out and without ending the thread - @see the constructor.
     *
     *  \par Example Usage
     *      \code
     *          Execution::WaitForIOReady waiter{fd};
     *          bool                      eof = false;
     *          while (not eof) {
     *              waiter.WaitQuietly (1);
     *              readALittleFromProcess (fd, stream, write2StdErrCache, &eof);
     *          }
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          Execution::WaitForIOReady sockSetPoller{socket2FDBijection.Image ()};
     *          while (true) {
     *              try {
     *                  for (const auto& readyFD : sockSetPoller.WaitQuietly ()) {
     *                      ConnectionOrientedMasterSocket::Ptr localSocketToAcceptOn = *socket2FDBijection.InverseLookup (readyFD);
     *                      ConnectionOrientedStreamSocket::Ptr s                     = localSocketToAcceptOn.Accept ();
     *                      fNewConnectionAcceptor (s);
     *                  }
     *              }
     *              ...
     *          }
     *      \endcode
     *
     *  \note   WaitForIOReady internally uses SDKPollableType, which is a UNIX file descriptor or Windows SOCKET. TRAITS
     *          must be provided to map 'T' objects to that SDKPollableType. These are provided by default for most appropriate types.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note pointless to ever create a WaitForIOReady and not use it, so [[nodiscard]] appropriate
     */
    template <typename T = WaitForIOReady_Support::SDKPollableType, typename TRAITS = WaitForIOReady_Support::WaitForIOReady_Traits<T>>
    class [[nodiscard]] WaitForIOReady : public WaitForIOReady_Support::WaitForIOReady_Base {
    public:
        using TraitsType = TRAITS;

    public:
        /**
         *  \brief Watch each of fds for the events paired with it - or, in the other overloads, all of fds (or the one fd) for flags.
         *
         *  fds is copied, so later changes to the container it came from have no effect here. For a descriptor set
         *  that can change while a wait is in progress, @see UpdatableWaitForIOReady.
         *
         *  \par pollable2Wakeup
         *  An optional extra descriptor, and the events to watch it for (normally just eRead), whose becoming ready
         *  makes a pending Wait* return early. That is how another thread gets a wait to return WITHOUT ending it -
         *  to pick up a changed descriptor set, say. Aborting the waiting thread needs no such help: every Wait* is
         *  a cancelation point, and Thread::Ptr::Abort () ends it by itself.
         *
         *  It is a raw SDKPollableType, not a T, so TRAITS does not map it and it never appears in a result: a wait
         *  it ends returns just the fds that happened to be ready too - often none. So:
         *      -   WaitQuietly/WaitQuietlyUntil can return an empty set BEFORE the timeout, which from the result
         *          alone looks just like a timeout. To tell them apart, ask the wakeup (e.g. EventFD::IsSet ()).
         *      -   Wait/WaitUntil throw only once the timeout has really passed, so they too can return an empty set.
         *
         *  This class only ever polls it - it never reads, resets, or closes it. And poll () is level-triggered, so
         *  once ready it stays ready until its owner resets it, and until then EVERY wait returns at once. So:
         *      -   Reset it BEFORE each wait, not after. Then a wakeup that races with the reset is not lost - at
         *          worst it makes the wait return at once.
         *      -   Keep it open for as long as any Wait* on this object can run - including on copies, which share
         *          it rather than duplicate it.
         *
         *  The usual choice is an EventFD, from WaitForIOReady_Support::mkEventFD (): pass its GetWaitInfo (),
         *  Set () it to wake the wait, and Clear () it to reset. That is what UpdatableWaitForIOReady does.
         *
         *  \par Example Usage
         *      \code
         *          auto wakeup = WaitForIOReady_Support::mkEventFD (); // other threads call wakeup->Set () to end the wait early
         *          while (...) {
         *              wakeup->Clear (); // before the wait, not after - @see above
         *              WaitForIOReady waiter{fds, WaitForIOReady_Base::kDefaultTypeOfMonitor, wakeup->GetWaitInfo ()};
         *              for (auto fd : waiter.WaitQuietly (30s)) {
         *                  ...
         *              }
         *              // if wakeup->IsSet (), a wakeup ended the wait - so recheck what fds should be
         *          }
         *      \endcode
         *
         *  \note   On Windows, like fds, pollable2Wakeup must be a socket - @see the note at the top of this file.
         */
        WaitForIOReady (const Traversal::Iterable<pair<T, TypeOfMonitorSet>>& fds,
                        optional<pair<SDKPollableType, TypeOfMonitorSet>>     pollable2Wakeup = nullopt);
        WaitForIOReady (const Traversal::Iterable<T>& fds, const TypeOfMonitorSet& flags = kDefaultTypeOfMonitor,
                        optional<pair<SDKPollableType, TypeOfMonitorSet>> pollable2Wakeup = nullopt); ///< \brief Watch all of fds for flags
        WaitForIOReady (T fd, const TypeOfMonitorSet& flags = kDefaultTypeOfMonitor,
                        optional<pair<SDKPollableType, TypeOfMonitorSet>> pollable2Wakeup = nullopt); ///< \brief Watch just fd, for flags
        WaitForIOReady (WaitForIOReady&&) noexcept = default; ///< \brief Copies and moves share the descriptors - none is duplicated
        WaitForIOReady (const WaitForIOReady&)     = default; ///< \brief Copies and moves share the descriptors - none is duplicated

    public:
        ~WaitForIOReady () = default;

    public:
        nonvirtual WaitForIOReady& operator= (WaitForIOReady&&) noexcept = default;
        nonvirtual WaitForIOReady& operator= (const WaitForIOReady&)     = default;

    public:
        /**
         */
        nonvirtual Traversal::Iterable<pair<T, TypeOfMonitorSet>> GetDescriptors () const;

    public:
        /**
         *  Waits the given amount of time, and returns as soon as any one (or more) requires service (see TypeOfMonitor), or pollable2Wakeup signaled (in which case may return empty set).
         *
         *  \note   Throws a timeout on timeout - @see Execution::ThrowError (errc::timed_out).
         *  
         *  \note   ***Cancelation Point***
         *
         *  @see WaitQuietly
         *  @see WaitUntil
         *  @see WaitQuietlyUntil
         */
        nonvirtual Containers::Set<T> Wait (Time::DurationSeconds waitFor = Time::kInfinity);

    public:
        /**
         *  Waits the given amount of time, and returns as soon as any one (or more) requires service (see TypeOfMonitor).
         *
         *   Returns set of file descriptors which are ready, or empty set if timeout, or if signaled by pollable2Wakeup.
         *
         *  \note   ***Cancelation Point***
         *
         *  @see Wait
         *  @see WaitUntil
         *  @see WaitQuietlyUntil
         */
        nonvirtual Containers::Set<T> WaitQuietly (Time::DurationSeconds waitFor = Time::kInfinity);

    public:
        /**
         *  Waits until the given timeoutAt, and returns as soon as any one (or more) requires service (see TypeOfMonitor), or pollable2Wakeup signaled (in which case may return empty set)..
         *
         *  \note   Throws a timeout on timeout - @see Execution::ThrowError (errc::timed_out).
         *
         *  \note   ***Cancelation Point***
         *
         *  @see Wait
         *  @see WaitQuietly
         *  @see WaitQuietlyUntil
         */
        nonvirtual Containers::Set<T> WaitUntil (Time::TimePointSeconds timeoutAt = Time::TimePointSeconds{Time::kInfinity});

    public:
        /**
         *  Waits until the given timeoutAt, and returns as soon as any one (or more) requires service (see TypeOfMonitor), or pollable2Wakeup signaled (in which case may return empty set)..
         *
         *  Returns set of file descriptors which are ready, or an empty set if time expired before any became ready.
         *
         *  if timeout is <= 0, this will not wait (but may still find some file descriptors ready).
         *
         *  \note   ***Cancelation Point***
         *
         *  @see Wait
         *  @see WaitQuietly
         *  @see WaitUntil
         */
        nonvirtual Containers::Set<T> WaitQuietlyUntil (Time::TimePointSeconds timeoutAt = Time::TimePointSeconds{Time::kInfinity});

    private:
        qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE Debug::AssertExternallySynchronizedChecker fThisAssertExternallySynchronized_;
        // @todo   Consider Mapping<T, TypeOfMonitorSet> here instead of an Iterable of pairs
        const Traversal::Iterable<pair<T, TypeOfMonitorSet>>    fPollData_;
        const optional<pair<SDKPollableType, TypeOfMonitorSet>> fPollable2Wakeup_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "WaitForIOReady.inl"

#endif /*_Stroika_Foundation_Execution_WaitForIOReady_h_*/
