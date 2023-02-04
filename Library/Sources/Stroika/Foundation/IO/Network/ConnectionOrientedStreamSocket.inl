/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_ConnectionOrientedStreamSocket_inl_
#define _Stroika_Foundation_IO_Network_ConnectionOrientedStreamSocket_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../../Debug/Cast.h"

namespace Stroika::Foundation::IO::Network {

    /*
     ********************************************************************************
     ******* Foundation::IO::Network::ConnectionOrientedStreamSocket::Ptr ***********
     ********************************************************************************
     */
    inline ConnectionOrientedStreamSocket::Ptr::Ptr (nullptr_t)
        : inherited{nullptr}
    {
    }
    inline ConnectionOrientedStreamSocket::Ptr::Ptr (shared_ptr<_IRep>&& rep)
        : inherited{move (rep)}
    {
    }
    inline ConnectionOrientedStreamSocket::Ptr::Ptr (const shared_ptr<_IRep>& rep)
        : inherited{rep}
    {
    }
    inline void ConnectionOrientedStreamSocket::Ptr::Close () const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        inherited::Close ();
    }
    inline void ConnectionOrientedStreamSocket::Ptr::Connect (const SocketAddress& sockAddr) const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        _ref ().Connect (sockAddr, nullopt);
    }
    inline void ConnectionOrientedStreamSocket::Ptr::Connect (const SocketAddress& sockAddr, const Time::Duration& timeout) const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        _ref ().Connect (sockAddr, timeout);
    }
    inline size_t ConnectionOrientedStreamSocket::Ptr::Read (byte* intoStart, byte* intoEnd) const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return _ref ().Read (intoStart, intoEnd);
    }
    inline optional<size_t> ConnectionOrientedStreamSocket::Ptr::ReadNonBlocking (byte* intoStart, byte* intoEnd) const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return _ref ().ReadNonBlocking (intoStart, intoEnd);
    }
    inline void ConnectionOrientedStreamSocket::Ptr::Write (const byte* start, const byte* end) const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        _ref ().Write (start, end);
    }
    inline optional<IO::Network::SocketAddress> ConnectionOrientedStreamSocket::Ptr::GetPeerAddress () const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return _cref ().GetPeerAddress ();
    }
    inline optional<Time::DurationSecondsType> ConnectionOrientedStreamSocket::Ptr::GetAutomaticTCPDisconnectOnClose () const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return _cref ().GetAutomaticTCPDisconnectOnClose ();
    }
    inline void ConnectionOrientedStreamSocket::Ptr::SetAutomaticTCPDisconnectOnClose (const optional<Time::DurationSecondsType>& linger) const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        _ref ().SetAutomaticTCPDisconnectOnClose (linger);
    }
    inline auto ConnectionOrientedStreamSocket::Ptr::GetKeepAlives () const -> KeepAliveOptions
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return _cref ().GetKeepAlives ();
    }
    inline void ConnectionOrientedStreamSocket::Ptr::SetKeepAlives (const KeepAliveOptions& keepAliveOptions) const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        _ref ().SetKeepAlives (keepAliveOptions);
    }
    inline shared_ptr<ConnectionOrientedStreamSocket::_IRep> ConnectionOrientedStreamSocket::Ptr::_GetSharedRep () const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return Debug::UncheckedDynamicPointerCast<ConnectionOrientedStreamSocket::_IRep> (inherited::_GetSharedRep ());
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

    /*
     ********************************************************************************
     ********* Foundation::IO::Network::ConnectionOrientedStreamSocket **************
     ********************************************************************************
     */
    inline ConnectionOrientedStreamSocket::Ptr ConnectionOrientedStreamSocket::NewConnection (const SocketAddress& sockAddr)
    {
        ConnectionOrientedStreamSocket::Ptr s = ConnectionOrientedStreamSocket::New (sockAddr.GetAddressFamily (), Socket::STREAM);
        s.Connect (sockAddr);
        return s;
    }

}

#endif /*_Stroika_Foundation_IO_Network_ConnectionOrientedStreamSocket_inl_*/
