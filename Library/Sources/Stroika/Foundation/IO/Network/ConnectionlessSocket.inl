/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_ConnectionlessSocket_inl_
#define _Stroika_Foundation_IO_Network_ConnectionlessSocket_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::IO::Network {

    /*
     ********************************************************************************
     ************ Foundation::IO::Network::ConnectionlessSocket::Ptr ****************
     ********************************************************************************
     */
    inline ConnectionlessSocket::Ptr::Ptr (nullptr_t)
        : Socket::Ptr (nullptr)
    {
    }
    inline ConnectionlessSocket::Ptr::Ptr (const shared_ptr<_IRep>& rep)
        : Socket::Ptr (rep)
    {
    }
    inline ConnectionlessSocket::Ptr::Ptr (shared_ptr<_IRep>&& rep)
        : Socket::Ptr (move (rep))
    {
    }
    inline uint8_t ConnectionlessSocket::Ptr::GetMulticastTTL () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return _cref ().GetMulticastTTL ();
    }
    inline void ConnectionlessSocket::Ptr::SetMulticastTTL (uint8_t ttl) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        _ref ().SetMulticastTTL (ttl);
    }
    inline bool ConnectionlessSocket::Ptr::GetMulticastLoopMode () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return _cref ().GetMulticastLoopMode ();
    }
    inline void ConnectionlessSocket::Ptr::SetMulticastLoopMode (bool loopMode) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        _ref ().SetMulticastLoopMode (loopMode);
    }
    inline void ConnectionlessSocket::Ptr::JoinMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        _ref ().JoinMulticastGroup (iaddr, onInterface);
    }
    inline void ConnectionlessSocket::Ptr::LeaveMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        _ref ().LeaveMulticastGroup (iaddr, onInterface);
    }
    inline void ConnectionlessSocket::Ptr::SendTo (const Byte* start, const Byte* end, const SocketAddress& sockAddr) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        _ref ().SendTo (start, end, sockAddr);
    }
    inline size_t ConnectionlessSocket::Ptr::ReceiveFrom (Byte* intoStart, Byte* intoEnd, int flag, SocketAddress* fromAddress, Time::DurationSecondsType timeout) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return _ref ().ReceiveFrom (intoStart, intoEnd, flag, fromAddress, timeout);
    }
    inline shared_ptr<ConnectionlessSocket::_IRep> ConnectionlessSocket::Ptr::_GetSharedRep () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return dynamic_pointer_cast<ConnectionlessSocket::_IRep> (inherited::_GetSharedRep ());
    }
    inline ConnectionlessSocket::_IRep& ConnectionlessSocket::Ptr::_ref () const
    {
        AssertMember (&inherited::_ref (), _IRep);
        return *reinterpret_cast<_IRep*> (&inherited::_ref ());
    }
    inline const ConnectionlessSocket::_IRep& ConnectionlessSocket::Ptr::_cref () const
    {
        AssertMember (&inherited::_cref (), _IRep);
        return *reinterpret_cast<const _IRep*> (&inherited::_cref ());
    }

}

#endif /*_Stroika_Foundation_IO_Network_ConnectionlessSocket_inl_*/
