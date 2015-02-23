/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_WaitForIOReady_h_
#define _Stroika_Foundation_Execution_WaitForIOReady_h_  1

#include    "../StroikaPreComp.h"


#include    "../Containers/Set.h"
#include    "../Configuration/Common.h"
#include    "../Execution/Synchronized.h"
#include    "../IO/Network/Socket.h"



/**
 *  \file
 *
 *      WaitForIOReady utility - portably provide select, epoll, WaitForMutlipleObjects, etc.
 *
 *  TODO:
 *      @todo   Must figure out relationship on windows between HANDLE and 'int' descriptors! Not done correctly here.
 *              Probably also add overload to ADD taking HANDLE.
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
             *          extenral 'Synchonized', because then adds would block for the entire time a Wait was going on.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             */
            class   WaitForIOReady {
            public:
#if     qPlatform_Windows
                using   FileDescriptorType  =   HANDLE;
#else
                using   FileDescriptorType  =   int;
#endif

            public:
                using   Socket = IO::Network::Socket;

            public:
                template    <typename T>
                using   Set = Containers::Set<T>;

            public:
                WaitForIOReady () = default;
                WaitForIOReady (const WaitForIOReady&) = default;
                WaitForIOReady (const Set<Socket>& s);
                WaitForIOReady (const Set<FileDescriptorType>& fds);

            public:
                ~WaitForIOReady() = default;

            public:
                nonvirtual  WaitForIOReady& operator= (const WaitForIOReady&) = default;

            public:
                nonvirtual  void    Add (FileDescriptorType fd);
                nonvirtual  void    Add (Socket s);

            public:
                nonvirtual  void    AddAll (const Set<FileDescriptorType>& fds);
                nonvirtual  void    AddAll (const Set<Socket>& s);

            public:
                nonvirtual  void    Remove (FileDescriptorType fd);
                nonvirtual  void    Remove (Socket s);

            public:
                nonvirtual  void    RemoveAll (const Set<FileDescriptorType>& fds);
                nonvirtual  void    RemoveAll (const Set<Socket>& s);

            public:
                nonvirtual  Set<FileDescriptorType> GetDescriptors () const;

            public:
                nonvirtual  void        SetDescriptors (const Set<FileDescriptorType>& fds);
                nonvirtual  void        SetDescriptors (const Set<Socket>& s);

            public:
                nonvirtual  void        clear ();

            public:
                /*
                 *  Waits the given amount of time, and returns as soon as any one (or more) requires service (read or write).
                 *  Unlike other wait functions, this just returns an empty set on timeout.
                 */
                nonvirtual  Set<FileDescriptorType>     Wait (Time::DurationSecondsType waitFor = Time::kInfinite);

            public:
                /*
                 *  Waits unil the given timeoutAt, and returns as soon as any one (or more) requires service (read or write).
                 *  Unlike other wait functions, this just returns an empty set on timeout.
                 */
                nonvirtual  Set<FileDescriptorType>     WaitUntil (Time::DurationSecondsType timeoutAt = Time::kInfinite);

            private:
                static  FileDescriptorType  cvt_ (Socket::PlatformNativeHandle pnh);

            private:
                Execution::Synchronized<Set<FileDescriptorType>>    fFDs_;
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
