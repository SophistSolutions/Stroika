/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_WaitForSocketIOReady_h_
#define _Stroika_Foundation_IO_Network_WaitForSocketIOReady_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Containers/Bijection.h"
#include "../../Execution/WaitForIOReady.h"

#include "Socket.h"

/*
 * TODO:
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 */

namespace Stroika::Foundation::IO::Network {

    /**
     *  Simple portable wrapper on OS select/2, pselect/2, poll/2, epoll (), and/or WaitForMultipleEvents(), for socket class.
     *
     *  \note   @see simple wrapper on Execution::WaitForIOReady
     *
     *  \note   This class is Internally-Synchronized-Thread-Safety. It would not be helpful to use this class with an
     *          extenral 'Synchronized', because then adds would block for the entire time a Wait was going on.
     *
     *  \note   \em Thread-Safety   <a href="thread_safety.html#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     *
     *  TODO:
     *      @todo - INCOMPLETE wrapping of Execution::WaitForIOReady - but enough to start testing...
     */
    template <typename SOCKET_SUBTYPE>
    class WaitForSocketIOReady : public Execution::WaitForIOReady {
    private:
        using inherited = Execution::WaitForIOReady;

    public:
        /**
         *  @see Execution::WaitForIOReady::TypeOfMonitor
         */
        using TypeOfMonitor = Execution::WaitForIOReady::TypeOfMonitor;

    public:
        /**
         *  @see Execution::WaitForIOReady::kDefaultTypeOfMonitor
         */
        using Execution::WaitForIOReady::kDefaultTypeOfMonitor;

    public:
        /**
         */
        WaitForSocketIOReady ()                            = default;
        WaitForSocketIOReady (const WaitForSocketIOReady&) = default;
        WaitForSocketIOReady (const Traversal::Iterable<SOCKET_SUBTYPE>& sockets, const TypeOfMonitorSet& flags = kDefaultTypeOfMonitor);
        WaitForSocketIOReady (const Traversal::Iterable<pair<SOCKET_SUBTYPE, TypeOfMonitorSet>>& fds);
        WaitForSocketIOReady (SOCKET_SUBTYPE fd, const TypeOfMonitorSet& flags = kDefaultTypeOfMonitor);

    public:
        /**
         */
        nonvirtual void Add (SOCKET_SUBTYPE fd, const TypeOfMonitorSet& flags = kDefaultTypeOfMonitor);

    public:
        /**
         */
        nonvirtual void AddAll (const Traversal::Iterable<pair<SOCKET_SUBTYPE, TypeOfMonitorSet>>& fds);
        nonvirtual void AddAll (const Traversal::Iterable<SOCKET_SUBTYPE>& fds, const TypeOfMonitorSet& flags = kDefaultTypeOfMonitor);

    public:
        /**
         *
         *  \note   ***Cancelation Point***
         */
        nonvirtual Traversal::Iterable<SOCKET_SUBTYPE> Wait (Time::DurationSecondsType waitFor = Time::kInfinite);

    public:
        /**
         *
         *  \note   ***Cancelation Point***
         */
        nonvirtual Traversal::Iterable<SOCKET_SUBTYPE> WaitUntil (Time::DurationSecondsType timeoutAt = Time::kInfinite);

    private:
        Containers::Bijection<SOCKET_SUBTYPE, Socket::PlatformNativeHandle> fMapping_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "WaitForSocketIOReady.inl"

#endif /*_Stroika_Foundation_IO_Network_WaitForSocketIOReady_h_*/
