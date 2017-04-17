/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_WaitForIOReady_h_
#define _Stroika_Foundation_Execution_WaitForIOReady_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Common.h"
#include "../Containers/Collection.h"
#include "../Containers/Set.h"
#include "../Execution/Synchronized.h"
#include "../IO/Network/Socket.h"

/**
 *  \file
 *
 *      WaitForIOReady utility - portably provide facility to check a bunch of file descriptors/sockets
 *      if input is ready (like select, epoll, WaitForMutlipleObjects, etc)
 *
 *  TODO:
 *
 *      @todo   THINK OUT signal flags/params to ppoll()
 *
 *      @todo   Consider using Mapping<> for fPollData_;
 *
 *      @todo   See if some way to make WaitForIOReady work with stuff other than sockets - on windows
 *              (WaitFormUltipleEventsEx didnt work well at all)
 *
 *      @todo   NYI Remove/RemoveAll - easy but not used yet, so low priority
 *
 *      @todo   better error handling in WaitForIOReady::WaitUntil () - some errors OK, but others should throw
 */

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            /**
             *  Simple portable wrapper on OS select/2, pselect/2, poll/2, epoll (), and/or WaitForMultipleEvents(), etc
             *
             *  \note   Calls to Add/Remove/clear/SetDescriptors () doesn't affect already running calls to Wait()
             *
             *  \note   This class is Automatically-Synchronized-Thread-Safety. It would not be helpful to use this class with an
             *          extenral 'Synchronized', because then adds would block for the entire time a Wait was going on.
             *
             *  \note   WaitForIOReady only works for type SOCKET on Windows
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             */
            class WaitForIOReady {
            public:
#if qPlatform_Windows
                using FileDescriptorType = SOCKET;
#else
                using FileDescriptorType = int;
#endif

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

            public:
                /**
                 */
                WaitForIOReady ()                      = default;
                WaitForIOReady (const WaitForIOReady&) = default;
                WaitForIOReady (const Traversal::Iterable<FileDescriptorType>& fds, const TypeOfMonitorSet& flags = kDefaultTypeOfMonitor);
                WaitForIOReady (const Traversal::Iterable<pair<FileDescriptorType, TypeOfMonitorSet>>& fds);
                WaitForIOReady (FileDescriptorType fd, const TypeOfMonitorSet& flags);

            public:
                ~WaitForIOReady () = default;

            public:
                nonvirtual WaitForIOReady& operator= (const WaitForIOReady&) = default;

            public:
                /**
                 */
                nonvirtual void Add (FileDescriptorType fd, const TypeOfMonitorSet& flags = kDefaultTypeOfMonitor);

            public:
                /**
                 */
                nonvirtual void AddAll (const Traversal::Iterable<pair<FileDescriptorType, TypeOfMonitorSet>>& fds);
                nonvirtual void AddAll (const Traversal::Iterable<FileDescriptorType>& fds, const TypeOfMonitorSet& flags = kDefaultTypeOfMonitor);

            public:
                /*
                 *  If no flags specified, remove all occurences of fd.
                 */
                nonvirtual void Remove (FileDescriptorType fd);
                nonvirtual void Remove (FileDescriptorType fd, const TypeOfMonitorSet& flags);

            public:
                /**
                 */
                nonvirtual void RemoveAll (const Traversal::Iterable<FileDescriptorType>& fds);
                nonvirtual void RemoveAll (const Traversal::Iterable<pair<FileDescriptorType, TypeOfMonitorSet>>& fds);

            public:
                /**
                 */
                nonvirtual Containers::Collection<pair<FileDescriptorType, TypeOfMonitorSet>> GetDescriptors () const;

            public:
                /**
                 */
                nonvirtual void SetDescriptors (const Traversal::Iterable<pair<FileDescriptorType, TypeOfMonitorSet>>& fds);

            public:
                /**
                 */
                nonvirtual void clear ();

            public:
                /*
                 *  Waits the given amount of time, and returns as soon as any one (or more) requires service (see TypeOfMonitor).
                 *
                 *  \note   Throws TimeOutException () on timeout.
                 *  
                 *  @see WaitQuietly
                 *  @see WaitUntil
                 *  @see WaitQuietlyUntil
                 */
                nonvirtual Containers::Set<FileDescriptorType> Wait (Time::DurationSecondsType waitFor = Time::kInfinite);

            public:
                /*
                 *  Waits the given amount of time, and returns as soon as any one (or more) requires service (see TypeOfMonitor).
                 *
                 *  Returns set of file descriptors which are ready, or 'Missing' if timeout.
                 *
                 *  @see Wait
                 *  @see WaitUntil
                 *  @see WaitQuietlyUntil
                 */
                nonvirtual Memory::Optional<Containers::Set<FileDescriptorType>> WaitQuietly (Time::DurationSecondsType waitFor = Time::kInfinite);

            public:
                /*
                 *  Waits unil the given timeoutAt, and returns as soon as any one (or more) requires service (see TypeOfMonitor).
                 *
                 *  \note   Throws TimeOutException () on timeout.
                 *
                 *  @see Wait
                 *  @see WaitQuietly
                 *  @see WaitQuietlyUntil
                 */
                nonvirtual Containers::Set<FileDescriptorType> WaitUntil (Time::DurationSecondsType timeoutAt = Time::kInfinite);

            public:
                /*
                 *  Waits unil the given timeoutAt, and returns as soon as any one (or more) requires service (see TypeOfMonitor).
                 *
                 *  Returns set of file descriptors which are ready, or 'Missing' if timeout.
                 *
                 *  @see Wait
                 *  @see WaitQuietly
                 *  @see WaitUntil
                 */
                nonvirtual Memory::Optional<Containers::Set<FileDescriptorType>> WaitQuietlyUntil (Time::DurationSecondsType timeoutAt = Time::kInfinite);

            private:
                Execution::Synchronized<Containers::Collection<pair<FileDescriptorType, TypeOfMonitorSet>>> fPollData_;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "WaitForIOReady.inl"

#endif /*_Stroika_Foundation_Execution_WaitForIOReady_h_*/
