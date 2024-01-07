/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_ConnectionlessSocket_inl_
#define _Stroika_Foundation_IO_Network_ConnectionlessSocket_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../../Debug/Cast.h"

namespace Stroika::Foundation::IO::Network {

    /*
     ********************************************************************************
     ************ Foundation::IO::Network::ConnectionlessSocket::Ptr ****************
     ********************************************************************************
     */
    inline ConnectionlessSocket::Ptr::Ptr (nullptr_t)
        : Socket::Ptr{nullptr}
    {
    }
    inline ConnectionlessSocket::Ptr::Ptr (const shared_ptr<_IRep>& rep)
        : Socket::Ptr{rep}
    {
    }
    inline ConnectionlessSocket::Ptr::Ptr (shared_ptr<_IRep>&& rep)
        : Socket::Ptr{move (rep)}
    {
    }
    inline uint8_t ConnectionlessSocket::Ptr::GetMulticastTTL () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        return _cref ().GetMulticastTTL ();
    }
    inline void ConnectionlessSocket::Ptr::SetMulticastTTL (uint8_t ttl) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        _ref ().SetMulticastTTL (ttl);
    }
    inline bool ConnectionlessSocket::Ptr::GetMulticastLoopMode () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        return _cref ().GetMulticastLoopMode ();
    }
    inline void ConnectionlessSocket::Ptr::SetMulticastLoopMode (bool loopMode) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        _ref ().SetMulticastLoopMode (loopMode);
    }
    inline void ConnectionlessSocket::Ptr::JoinMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        _ref ().JoinMulticastGroup (iaddr, onInterface);
    }
    inline void ConnectionlessSocket::Ptr::LeaveMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        _ref ().LeaveMulticastGroup (iaddr, onInterface);
    }
    inline void ConnectionlessSocket::Ptr::SendTo (const byte* start, const byte* end, const SocketAddress& sockAddr) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        _ref ().SendTo (start, end, sockAddr);
    }
    inline size_t ConnectionlessSocket::Ptr::ReceiveFrom (byte* intoStart, byte* intoEnd, int flag, SocketAddress* fromAddress,
                                                          Time::DurationSeconds timeout) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        return _ref ().ReceiveFrom (intoStart, intoEnd, flag, fromAddress, timeout);
    }
    inline shared_ptr<ConnectionlessSocket::_IRep> ConnectionlessSocket::Ptr::_GetSharedRep () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        return Debug::UncheckedDynamicPointerCast<ConnectionlessSocket::_IRep> (inherited::_GetSharedRep ());
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
