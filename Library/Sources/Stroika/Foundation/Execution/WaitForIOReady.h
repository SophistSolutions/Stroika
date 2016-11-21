/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_WaitForIOReady_h_
#define _Stroika_Foundation_Execution_WaitForIOReady_h_  1

#include    "../StroikaPreComp.h"

#include    "../Containers/Collection.h"
#include    "../Containers/Set.h"
#include    "../Configuration/Common.h"
#include    "../Execution/Synchronized.h"
#include    "../IO/Network/Socket.h"



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
 *      @todo   See if some way to make WaitForIOReady work with stuff other than sockets - on windows
 *              (WaitFormUltipleEventsEx didnt work well at all)
 *
 *		@todo	NYI Remove/RemoveAll - easy but not used yet, so low priority
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


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
            class   WaitForIOReady {
            public:
#if     qPlatform_Windows
                using   FileDescriptorType  =   SOCKET;
#else
                using   FileDescriptorType  =   int;
#endif

            public:
                WaitForIOReady () = default;
                WaitForIOReady (const WaitForIOReady&) = default;
                WaitForIOReady (const Traversal::Iterable<FileDescriptorType>& fds);

            public:
                ~WaitForIOReady() = default;

            public:
                nonvirtual  WaitForIOReady& operator= (const WaitForIOReady&) = default;

            public:
                enum    class   TypeOfMonitor {
                    eRead,              // @see http://man7.org/linux/man-pages/man2/poll.2.html - POLLIN

                    eDEFAULT = eRead,
                };
            public:
                nonvirtual  void    Add (FileDescriptorType fd, TypeOfMonitor flags = TypeOfMonitor::eDEFAULT);

            public:
                nonvirtual  void    AddAll (const Traversal::Iterable<pair<FileDescriptorType, TypeOfMonitor>>& fds);
                nonvirtual  void    AddAll (const Traversal::Iterable<FileDescriptorType>& fds, TypeOfMonitor flags = TypeOfMonitor::eDEFAULT);

            public:
                /*
                 *  If no flags specified, remove all occurences of fd.
                 */
                nonvirtual  void    Remove (FileDescriptorType fd);
                nonvirtual  void    Remove (FileDescriptorType fd, TypeOfMonitor flags);

            public:
                nonvirtual  void    RemoveAll (const Traversal::Iterable<FileDescriptorType>& fds);
                nonvirtual  void    RemoveAll (const Traversal::Iterable<pair<FileDescriptorType, TypeOfMonitor>>& fds);

            public:
                nonvirtual  Containers::Collection<pair<FileDescriptorType, TypeOfMonitor>> GetDescriptors () const;

            public:
                nonvirtual  void        SetDescriptors (const Traversal::Iterable<pair<FileDescriptorType, TypeOfMonitor>>& fds);

            public:
                nonvirtual  void        clear ();

            public:
                /*
                 *  Waits the given amount of time, and returns as soon as any one (or more) requires service (see TypeOfMonitor).
                 *
                 *  \note   Unlike other wait functions, this just returns an empty set on timeout (or EINTR)
                 */
                nonvirtual  Containers::Set<FileDescriptorType>     Wait (Time::DurationSecondsType waitFor = Time::kInfinite);

            public:
                /*
                 *  Waits unil the given timeoutAt, and returns as soon as any one (or more) requires service (see TypeOfMonitor).
                 *
                 *  \note   Unlike other wait functions, this just returns an empty set on timeout (or EINTR)
                 */
                nonvirtual  Containers::Set<FileDescriptorType>     WaitUntil (Time::DurationSecondsType timeoutAt = Time::kInfinite);

            private:
                Execution::Synchronized<Containers::Collection<pair<FileDescriptorType, TypeOfMonitor>>> fPollData_;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "WaitForIOReady.inl"

#endif  /*_Stroika_Foundation_Execution_WaitForIOReady_h_*/
