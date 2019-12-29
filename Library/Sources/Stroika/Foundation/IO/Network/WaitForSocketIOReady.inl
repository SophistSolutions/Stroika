/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_WaitForSocketIOReady_inl_
#define _Stroika_Foundation_IO_Network_WaitForSocketIOReady_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::IO::Network {

    /*
     ********************************************************************************
     ********************** IO::Network::WaitForSocketIOReady ***********************
     ********************************************************************************
     */
    template <typename SOCKET_SUBTYPE>
    WaitForSocketIOReady<SOCKET_SUBTYPE>::WaitForSocketIOReady (const Traversal::Iterable<SOCKET_SUBTYPE>& sockets, const TypeOfMonitorSet& flags)
        : inherited (sockets.template Select<Socket::PlatformNativeHandle> ([] (const Socket::Ptr& s) { return s.GetNativeSocket (); }), flags)
    {
        for (SOCKET_SUBTYPE i : sockets) {
            fMapping_.Add (i, i.GetNativeSocket ());
        }
    }
    template <typename SOCKET_SUBTYPE>
    WaitForSocketIOReady<SOCKET_SUBTYPE>::WaitForSocketIOReady (const Traversal::Iterable<pair<SOCKET_SUBTYPE, TypeOfMonitorSet>>& fds)
        : inherited (fds.template Select<pair<Socket::PlatformNativeHandle, TypeOfMonitorSet>> ([] (const pair<SOCKET_SUBTYPE, TypeOfMonitorSet>& p) { return pair<Socket::PlatformNativeHandle, TypeOfMonitorSet>{p.first.GetNativeSocket (), p.second}; }))
    {
        for (SOCKET_SUBTYPE i : fds) {
            fMapping_.Add (i, i.GetNativeSocket ());
        }
    }
    template <typename SOCKET_SUBTYPE>
    WaitForSocketIOReady<SOCKET_SUBTYPE>::WaitForSocketIOReady (SOCKET_SUBTYPE fd, const TypeOfMonitorSet& flags)
        : inherited (fd.GetNativeSocket (), flags)
    {
        fMapping_.Add (fd, fd.GetNativeSocket ());
    }
    template <typename SOCKET_SUBTYPE>
    inline void WaitForSocketIOReady<SOCKET_SUBTYPE>::Add (SOCKET_SUBTYPE fd, const TypeOfMonitorSet& flags)
    {
        inherited::Add (fd.GetNativeSocket (), flags);
        fMapping_.Add (fd, fd.GetNativeSocket ());
    }
    template <typename SOCKET_SUBTYPE>
    void WaitForSocketIOReady<SOCKET_SUBTYPE>::AddAll (const Traversal::Iterable<pair<SOCKET_SUBTYPE, TypeOfMonitorSet>>& fds)
    {
        for (auto i : fds) {
            Add (i.first, i.second);
        }
    }
    template <typename SOCKET_SUBTYPE>
    void WaitForSocketIOReady<SOCKET_SUBTYPE>::AddAll (const Traversal::Iterable<SOCKET_SUBTYPE>& fds, const TypeOfMonitorSet& flags)
    {
        for (auto i : fds) {
            Add (i, flags);
        }
    }
    template <typename SOCKET_SUBTYPE>
    inline Traversal::Iterable<SOCKET_SUBTYPE> WaitForSocketIOReady<SOCKET_SUBTYPE>::Wait (Time::DurationSecondsType waitFor)
    {
        return fMapping_.InverseMap (inherited::Wait (waitFor));
    }
    template <typename SOCKET_SUBTYPE>
    inline Traversal::Iterable<SOCKET_SUBTYPE> WaitForSocketIOReady<SOCKET_SUBTYPE>::Wait (const Time::Duration& waitFor)
    {
        return fMapping_.InverseMap (inherited::Wait (waitFor));
    }
    template <typename SOCKET_SUBTYPE>
    inline Traversal::Iterable<SOCKET_SUBTYPE> WaitForSocketIOReady<SOCKET_SUBTYPE>::WaitQuietly (const Time::Duration& waitFor)
    {
        return fMapping_.InverseMap (inherited::WaitQuietly (waitFor));
    }
    template <typename SOCKET_SUBTYPE>
    inline Traversal::Iterable<SOCKET_SUBTYPE> WaitForSocketIOReady<SOCKET_SUBTYPE>::WaitUntil (Time::DurationSecondsType timeoutAt)
    {
        return fMapping_.InverseMap (inherited::WaitUntil (timeoutAt));
    }
    template <typename SOCKET_SUBTYPE>
    inline Traversal::Iterable<SOCKET_SUBTYPE> WaitForSocketIOReady<SOCKET_SUBTYPE>::WaitQuietlyUntil (Time::DurationSecondsType timeoutAt)
    {
        return fMapping_.InverseMap (inherited::WaitQuietlyUntil (timeoutAt));
    }

}

#endif /*_Stroika_Foundation_IO_Network_WaitForSocketIOReady_inl_*/
