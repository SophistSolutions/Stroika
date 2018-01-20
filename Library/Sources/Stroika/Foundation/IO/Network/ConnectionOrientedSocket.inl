/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_ConnectionOrientedSocket_inl_
#define _Stroika_Foundation_IO_Network_ConnectionOrientedSocket_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {

                /*
                 ********************************************************************************
                 ************ Foundation::IO::Network::ConnectionOrientedSocket::Ptr ************
                 ********************************************************************************
                 */
                inline ConnectionOrientedSocket::Ptr::Ptr (nullptr_t)
                    : inherited (nullptr)
                {
                }
                inline ConnectionOrientedSocket::Ptr::Ptr (shared_ptr<_IRep>&& rep)
                    : inherited (move (rep))
                {
                }
                inline ConnectionOrientedSocket::Ptr::Ptr (const shared_ptr<_IRep>& rep)
                    : inherited (rep)
                {
                }
                inline void ConnectionOrientedSocket::Ptr::Close () const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    inherited::Close ();
                }
                inline void ConnectionOrientedSocket::Ptr::Connect (const SocketAddress& sockAddr) const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    return _ref ().Connect (sockAddr);
                }
                inline size_t ConnectionOrientedSocket::Ptr::Read (Byte* intoStart, Byte* intoEnd) const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    return _ref ().Read (intoStart, intoEnd);
                }
                inline Memory::Optional<size_t> ConnectionOrientedSocket::Ptr::ReadNonBlocking (Byte* intoStart, Byte* intoEnd) const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    return _ref ().ReadNonBlocking (intoStart, intoEnd);
                }
                inline void ConnectionOrientedSocket::Ptr::Write (const Byte* start, const Byte* end) const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    _ref ().Write (start, end);
                }
                inline Optional<IO::Network::SocketAddress> ConnectionOrientedSocket::Ptr::GetPeerAddress () const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    return _cref ().GetPeerAddress ();
                }
                inline Optional<Time::DurationSecondsType> ConnectionOrientedSocket::Ptr::GetAutomaticTCPDisconnectOnClose () const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    return _cref ().GetAutomaticTCPDisconnectOnClose ();
                }
                inline void ConnectionOrientedSocket::Ptr::SetAutomaticTCPDisconnectOnClose (const Optional<Time::DurationSecondsType>& linger) const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    _ref ().SetAutomaticTCPDisconnectOnClose (linger);
                }
                inline auto ConnectionOrientedSocket::Ptr::GetKeepAlives () const -> KeepAliveOptions
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    return _cref ().GetKeepAlives ();
                }
                inline void ConnectionOrientedSocket::Ptr::SetKeepAlives (const KeepAliveOptions& keepAliveOptions) const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    _ref ().SetKeepAlives (keepAliveOptions);
                }
                inline shared_ptr<ConnectionOrientedSocket::_IRep> ConnectionOrientedSocket::Ptr::_GetSharedRep () const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    return dynamic_pointer_cast<ConnectionOrientedSocket::_IRep> (inherited::_GetSharedRep ());
                }
                inline ConnectionOrientedSocket::_IRep& ConnectionOrientedSocket::Ptr::_ref () const
                {
                    AssertMember (&inherited::_ref (), _IRep);
                    return *reinterpret_cast<_IRep*> (&inherited::_ref ());
                }
                inline const ConnectionOrientedSocket::_IRep& ConnectionOrientedSocket::Ptr::_cref () const
                {
                    AssertMember (&inherited::_cref (), _IRep);
                    return *reinterpret_cast<const _IRep*> (&inherited::_cref ());
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_IO_Network_ConnectionOrientedSocket_inl_*/
