/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_ConnectionOrientedStreamSocket_inl_
#define _Stroika_Foundation_IO_Network_ConnectionOrientedStreamSocket_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::IO::Network {

    /*
     ********************************************************************************
     ******* Foundation::IO::Network::ConnectionOrientedStreamSocket::Ptr ***********
     ********************************************************************************
     */
    inline ConnectionOrientedStreamSocket::Ptr::Ptr (nullptr_t)
        : inherited (nullptr)
    {
    }
    inline ConnectionOrientedStreamSocket::Ptr::Ptr (shared_ptr<_IRep>&& rep)
        : inherited (move (rep))
    {
    }
    inline ConnectionOrientedStreamSocket::Ptr::Ptr (const shared_ptr<_IRep>& rep)
        : inherited (rep)
    {
    }
    inline void ConnectionOrientedStreamSocket::Ptr::Close () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        inherited::Close ();
    }
    inline void ConnectionOrientedStreamSocket::Ptr::Connect (const SocketAddress& sockAddr) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return _ref ().Connect (sockAddr);
    }
    inline size_t ConnectionOrientedStreamSocket::Ptr::Read (Byte* intoStart, Byte* intoEnd) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return _ref ().Read (intoStart, intoEnd);
    }
    inline optional<size_t> ConnectionOrientedStreamSocket::Ptr::ReadNonBlocking (Byte* intoStart, Byte* intoEnd) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return _ref ().ReadNonBlocking (intoStart, intoEnd);
    }
    inline void ConnectionOrientedStreamSocket::Ptr::Write (const Byte* start, const Byte* end) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        _ref ().Write (start, end);
    }
    inline optional<IO::Network::SocketAddress> ConnectionOrientedStreamSocket::Ptr::GetPeerAddress () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return _cref ().GetPeerAddress ();
    }
    inline optional<Time::DurationSecondsType> ConnectionOrientedStreamSocket::Ptr::GetAutomaticTCPDisconnectOnClose () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return _cref ().GetAutomaticTCPDisconnectOnClose ();
    }
    inline void ConnectionOrientedStreamSocket::Ptr::SetAutomaticTCPDisconnectOnClose (const optional<Time::DurationSecondsType>& linger) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        _ref ().SetAutomaticTCPDisconnectOnClose (linger);
    }
    inline auto ConnectionOrientedStreamSocket::Ptr::GetKeepAlives () const -> KeepAliveOptions
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return _cref ().GetKeepAlives ();
    }
    inline void ConnectionOrientedStreamSocket::Ptr::SetKeepAlives (const KeepAliveOptions& keepAliveOptions) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        _ref ().SetKeepAlives (keepAliveOptions);
    }
    inline shared_ptr<ConnectionOrientedStreamSocket::_IRep> ConnectionOrientedStreamSocket::Ptr::_GetSharedRep () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return dynamic_pointer_cast<ConnectionOrientedStreamSocket::_IRep> (inherited::_GetSharedRep ());
    }
    inline ConnectionOrientedStreamSocket::_IRep& ConnectionOrientedStreamSocket::Ptr::_ref () const
    {
        AssertMember (&inherited::_ref (), _IRep);
        return *reinterpret_cast<_IRep*> (&inherited::_ref ());
    }
    inline const ConnectionOrientedStreamSocket::_IRep& ConnectionOrientedStreamSocket::Ptr::_cref () const
    {
        AssertMember (&inherited::_cref (), _IRep);
        return *reinterpret_cast<const _IRep*> (&inherited::_cref ());
    }

}

#endif /*_Stroika_Foundation_IO_Network_ConnectionOrientedStreamSocket_inl_*/
