/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Debug/Cast.h"

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
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        inherited::Close ();
    }
    inline void ConnectionOrientedStreamSocket::Ptr::Connect (const SocketAddress& sockAddr) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        _ref ().Connect (sockAddr, nullopt);
    }
    inline void ConnectionOrientedStreamSocket::Ptr::Connect (const SocketAddress& sockAddr, const Time::Duration& timeout) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        _ref ().Connect (sockAddr, timeout);
    }
    inline size_t ConnectionOrientedStreamSocket::Ptr::Read (byte* intoStart, byte* intoEnd) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        return _ref ().Read (intoStart, intoEnd);
    }
    inline optional<size_t> ConnectionOrientedStreamSocket::Ptr::ReadNonBlocking (byte* intoStart, byte* intoEnd) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        return _ref ().ReadNonBlocking (intoStart, intoEnd);
    }
    inline void ConnectionOrientedStreamSocket::Ptr::Write (const byte* start, const byte* end) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        _ref ().Write (start, end);
    }
    inline optional<IO::Network::SocketAddress> ConnectionOrientedStreamSocket::Ptr::GetPeerAddress () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        return _cref ().GetPeerAddress ();
    }
    inline optional<Time::DurationSeconds> ConnectionOrientedStreamSocket::Ptr::GetAutomaticTCPDisconnectOnClose () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        return _cref ().GetAutomaticTCPDisconnectOnClose ();
    }
    inline void ConnectionOrientedStreamSocket::Ptr::SetAutomaticTCPDisconnectOnClose (const optional<Time::DurationSeconds>& linger) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        _ref ().SetAutomaticTCPDisconnectOnClose (linger);
    }
    inline auto ConnectionOrientedStreamSocket::Ptr::GetKeepAlives () const -> KeepAliveOptions
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        return _cref ().GetKeepAlives ();
    }
    inline void ConnectionOrientedStreamSocket::Ptr::SetKeepAlives (const KeepAliveOptions& keepAliveOptions) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        _ref ().SetKeepAlives (keepAliveOptions);
    }
    inline shared_ptr<ConnectionOrientedStreamSocket::_IRep> ConnectionOrientedStreamSocket::Ptr::_GetSharedRep () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
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
