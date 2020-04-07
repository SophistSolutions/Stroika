/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_WaitForIOReady_h_
#define _Stroika_Foundation_Execution_WaitForIOReady_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Common.h"
#include "../Containers/Collection.h"
#include "../Containers/Set.h"
#include "../Execution/Synchronized.h"
#include "../IO/Network/Socket.h"
#include "../Time/Duration.h"

/**
 *  \file
 *
 *      WaitForIOReady utility - portably provide facility to check a bunch of file descriptors/sockets
 *      if input is ready (like select, epoll, WaitForMutlipleObjects, etc)
 *
 *  TODO:
 *      @todo   THINK OUT signal flags/params to ppoll()
 *
 *      @todo   Consider using Mapping<> for fPollData_;
 *
 *      @todo   See if some way to make WaitForIOReady work with stuff other than sockets - on windows
 *              (WaitFormUltipleEventsEx didnt work well at all)
 *
 *      @todo   better error handling in WaitForIOReady::WaitUntil () - some errors OK, but others should throw
 */

namespace Stroika::Foundation::Execution {

    /**
     *  \note see https://stroika.atlassian.net/browse/STK-653
     *
     ***** @todo revisit - maybe default off - cuz not using interrupt () anymore (probably still do cuz of abort, but at least do a bit less often)
     Or force teh abort/intererupt to somehow use our write mechanism? NO - that only works with updatbale... Probs can do no better than upping timeout
     *
     *
     *  WSAPoll is not (fully/mostly) alertable, in the Windows API. So for Windows, this trick is needed to make
     *  WaitForIOReady::Wait* a ***Cancelation Point***.
     *
     *  Set qStroika_Foundation_Exececution_WaitForIOReady_BreakWSAPollIntoTimedMillisecondChunks to a number of milliseconds between WSAPoll
     *  forced wakeups. A smaller value means more responsive, but more wasted CPU time.
     */
#ifndef qStroika_Foundation_Exececution_WaitForIOReady_BreakWSAPollIntoTimedMillisecondChunks
#if qPlatform_Windows
#define qStroika_Foundation_Exececution_WaitForIOReady_BreakWSAPollIntoTimedMillisecondChunks 1000
#endif
#endif

    namespace WaitForIOReady_Support {
        /**
         *  This is the underlying native type 'HighLevelType objects must be converted to in order to be used with the OLD poll/select feature.
         */
#if qPlatform_Windows
        using SDKPollableType = SOCKET;
#else
        using SDKPollableType = int;
#endif

        /**
         */
        class WaitForIOReady_Base {
        public:
            using FileDescriptorType [[deprecated ("use SDKPollableType in  in 2.1a5")]] = WaitForIOReady_Support::SDKPollableType;

        public:
            /**
             *  This is the underlying native type 'T' objects mustbe converted to in order to be used with the OLD poll/select feature.
             */
            using SDKPollableType = WaitForIOReady_Support::SDKPollableType;

        public:
            /**
             *  @todo consider adding more params - like out of band flags - but doesn't immediately seem helpful -- LGP 2017-04-16
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
                 *  Error condition.
                 *
                 *  @see http://man7.org/linux/man-pages/man2/poll.2.html - POLLERR
                 */
                eError,

                /**
                 *  stream-oriented connection was either disconnected or aborted.
                 *
                 *  @see http://man7.org/linux/man-pages/man2/poll.2.html - POLLHUP
                 */
                eHUP,

                Stroika_Define_Enum_Bounds (eRead, eHUP)
            };

        public:
            using TypeOfMonitorSet = Containers::Set<TypeOfMonitor>;

        public:
            static const TypeOfMonitorSet kDefaultTypeOfMonitor;

        protected:
            /**
             *  Take an array of pair<SDKPollableType, TypeOfMonitorSet> objects, and return a Set{} with the INDEXES of 'ready' pollable objects.
             *  
             *  \note Design Note: This could have returned a set of pointer to SDKPollableType which would in some sense be simpler and
             *                     clearer, but its easier to validate/assert the returned INDEXES are valid than the returned POINTERS are valid.
             */
            static auto _WaitQuietlyUntil (const pair<SDKPollableType, TypeOfMonitorSet>* start, const pair<SDKPollableType, TypeOfMonitorSet>* end, Time::DurationSecondsType timeoutAt) -> Containers::Set<size_t>;
        };

        template <typename T>
        struct WaitForIOReady_Traits {
            /**
             *  This is the underlying native type 'T' objects mustbe converted to in order to be used with the OLD poll/select feature.
             */
            using HighLevelType = T;

            /**
             *  To use WaitForIOReady, the high level 'descriptor' objects used mustbe convertible to associated low level
             *  file descriptor objects to use with select/poll/etc...
             */
            static inline WaitForIOReady_Support::SDKPollableType GetSDKPollable (const HighLevelType& t)
            {
                return t;
            }
        };

    }

    /**
     *  Simple portable wrapper on OS select/2, pselect/2, poll/2, epoll (), and/or WaitForMultipleEvents(), etc
     *
     *  \note   pollable2Wakeup specifies an OPTIONAL file descriptor, which, if signalled (written to or whatever signal sent to it
     *          depending on the POLL arg to this field) - any pending waits will return prematurely.
     *
     *          This can be used to trigger premature wakeup (without being treated as a timeout) - like if the list of file descriptors to watch
     *          changes.

     **@TTODO EXPLIAN WHY THIS IS BETTER THAN OLD THJREADE INTERRUPT (stacktrace code slow, and interreupt means if there were real answers mixed with 
     non-nanswer we would miss the real ansers and this way captures them too)

     *
     *  \note   This class is Internally-Synchronized-Thread-Safety. It would not be helpful to use this class with an
     *          externally 'Synchronized', because then adds would block for the entire time a Wait was going on.
     *&&& @todo WRONG UPDTATE -0 C++ NORMAL FOR THIS GUY AND ADD OVERWRITE CHECKS
     &&& AND FIX REGTESTS TO DO USE SMARTPTR - and without mappin to image/ etcc - and use tempate<>
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
     *                  for (auto readyFD : sockSetPoller.WaitQuietly ().Value ()) {
     *                      ConnectionOrientedMasterSocket::Ptr localSocketToAcceptOn = *socket2FDBijection.InverseLookup (readyFD);
     *                      ConnectionOrientedStreamSocket::Ptr s                     = localSocketToAcceptOn.Accept ();
     *                      fNewConnectionAcceptor (s);
     *                  }
     *              }
     *              ...
     *          }
     *      \endcode
     *
     *  \note   WaitForIOReady only works for type SOCKET on Windows
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     */
    template <typename T = WaitForIOReady_Support::SDKPollableType, typename TRAITS = WaitForIOReady_Support::WaitForIOReady_Traits<T>>
    class WaitForIOReady : public WaitForIOReady_Support::WaitForIOReady_Base {
    public:
        using TraitsType = TRAITS;

    public:
        /**
         */
        WaitForIOReady (const WaitForIOReady&) = default;
        WaitForIOReady (const Traversal::Iterable<pair<T, TypeOfMonitorSet>>& fds, optional<pair<SDKPollableType, TypeOfMonitorSet>> pollable2Wakeup = nullopt);
        WaitForIOReady (const Traversal::Iterable<T>& fds, const TypeOfMonitorSet& flags = kDefaultTypeOfMonitor, optional<pair<SDKPollableType, TypeOfMonitorSet>> pollable2Wakeup = nullopt);
        WaitForIOReady (T fd, const TypeOfMonitorSet& flags = kDefaultTypeOfMonitor, optional<pair<SDKPollableType, TypeOfMonitorSet>> pollable2Wakeup = nullopt);

    public:
        ~WaitForIOReady () = default;

    public:
        nonvirtual WaitForIOReady& operator= (const WaitForIOReady&) = default;

    public:
        /**
         */
        nonvirtual Traversal::Iterable<pair<T, TypeOfMonitorSet>> GetDescriptors () const;

    public:
        /*
         *  Waits the given amount of time, and returns as soon as any one (or more) requires service (see TypeOfMonitor).
         *
         *  \note   Throws TimeOutException () on timeout.
         *  
         *  \note   ***Cancelation Point***
         *
         *  @see WaitQuietly
         *  @see WaitUntil
         *  @see WaitQuietlyUntil
         */
        nonvirtual Containers::Set<T> Wait (Time::DurationSecondsType waitFor = Time::kInfinite);
        nonvirtual Containers::Set<T> Wait (const Time::Duration& waitFor);

    public:
        /*
         *  Waits the given amount of time, and returns as soon as any one (or more) requires service (see TypeOfMonitor).
         *
         *   Returns set of file descriptors which are ready, or empty set if timeout.
         *
         *  \note   ***Cancelation Point***
         *
         *  @see Wait
         *  @see WaitUntil
         *  @see WaitQuietlyUntil
         */
        nonvirtual Containers::Set<T> WaitQuietly (Time::DurationSecondsType waitFor = Time::kInfinite);
        nonvirtual Containers::Set<T> WaitQuietly (const Time::Duration& waitFor);

    public:
        /*
         *  Waits unil the given timeoutAt, and returns as soon as any one (or more) requires service (see TypeOfMonitor).
         *
         *  \note   Throws TimeOutException () on timeout.
         *
         *  \note   ***Cancelation Point***
         *
         *  @see Wait
         *  @see WaitQuietly
         *  @see WaitQuietlyUntil
         */
        nonvirtual Containers::Set<T> WaitUntil (Time::DurationSecondsType timeoutAt = Time::kInfinite);

    public:
        /*
         *  Waits unil the given timeoutAt, and returns as soon as any one (or more) requires service (see TypeOfMonitor).
         *
         *  Returns set of file descriptors which are ready, or an empty set if time expired before any became ready.
         *
         *  if timeout is <= 0, this will not wait (but may still find some file desciptors ready).
         *
         *  \note   ***Cancelation Point***
         *
         *  @see Wait
         *  @see WaitQuietly
         *  @see WaitUntil
         */
        nonvirtual Containers::Set<T> WaitQuietlyUntil (Time::DurationSecondsType timeoutAt = Time::kInfinite);

    private:
        // Fill two buffers, one with the data needed to pass to _WaitQuietlyUntil, and the other with
        // corresponding 'T' smart wrapper objects, which we map back to and return as our API result (in same order)
        nonvirtual void FillBuffer_ (vector<pair<SDKPollableType, TypeOfMonitorSet>>* pollBuffer, vector<T>* mappedObjectBuffer);

    private:
        Traversal::Iterable<pair<T, TypeOfMonitorSet>>    fPollData_;
        optional<pair<SDKPollableType, TypeOfMonitorSet>> fPollable2Wakeup_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "WaitForIOReady.inl"

#endif /*_Stroika_Foundation_Execution_WaitForIOReady_h_*/
