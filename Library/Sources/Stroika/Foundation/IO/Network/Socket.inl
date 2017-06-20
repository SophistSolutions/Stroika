/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Socket_inl_
#define _Stroika_Foundation_IO_Network_Socket_inl_ 1

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
                 *************** Foundation::IO::Network::Socket::BindFlags *********************
                 ********************************************************************************
                 */
                inline constexpr Socket::BindFlags::BindFlags (bool reUseAddr)
                    : fReUseAddr (reUseAddr)
                {
                }

                /*
                 ********************************************************************************
                 ********************* Foundation::IO::Network::Socket::Ptr *********************
                 ********************************************************************************
                 */
                inline Socket::Ptr::Ptr (Ptr&& s)
                    : fRep_ (std::move (s.fRep_))
                {
                }
                inline Socket::Ptr& Socket::Ptr::operator= (const Ptr& s)
                {
                    if (fRep_ != s.fRep_) {
                        fRep_ = s.fRep_;
                    }
                    return *this;
                }
                inline Socket::Ptr& Socket::Ptr::operator= (Ptr&& s)
                {
                    if (fRep_ != s.fRep_) {
                        fRep_ = std::move (s.fRep_);
                    }
                    return *this;
                }
                inline shared_ptr<Socket::Ptr::_IRep> Socket::Ptr::_GetSharedRep () const
                {
                    return fRep_;
                }
                inline Socket::_IRep& Socket::Ptr::_ref ()
                {
                    RequireNotNull (fRep_);
                    return *fRep_;
                }
                inline const Socket::_IRep& Socket::Ptr::_cref () const
                {
                    RequireNotNull (fRep_);
                    return *fRep_;
                }
                inline Socket::PlatformNativeHandle Socket::Ptr::GetNativeSocket () const
                {
                    return _cref ().GetNativeSocket ();
                }
                inline Optional<IO::Network::SocketAddress> Socket::Ptr::GetLocalAddress () const
                {
                    return _cref ().GetLocalAddress ();
                }
                inline SocketAddress::FamilyType Socket::Ptr::GetAddressFamily () const
                {
                    return _cref ().GetAddressFamily ();
                }
                inline void Socket::Ptr::Shutdown (ShutdownTarget shutdownTarget)
                {
                    // not important to null-out, but may as well...
                    if (fRep_ != nullptr) {
                        _ref ().Shutdown (shutdownTarget);
                    }
                }
                inline void Socket::Ptr::Close ()
                {
                    // not important to null-out, but may as well...
                    if (fRep_ != nullptr) {
                        fRep_->Close ();
                        fRep_.reset ();
                    }
                }
                template <typename RESULT_TYPE>
                inline RESULT_TYPE Socket::Ptr::getsockopt (int level, int optname) const
                {
                    RESULT_TYPE r{};
                    socklen_t   roptlen = sizeof (r);
                    _cref ().getsockopt (level, optname, &r, &roptlen);
                    return r;
                }
                template <typename ARG_TYPE>
                inline void Socket::Ptr::setsockopt (int level, int optname, ARG_TYPE arg)
                {
                    socklen_t optvallen = sizeof (arg);
                    _ref ().setsockopt (level, optname, &arg, optvallen);
                }
                inline bool Socket::Ptr::Equals (const Ptr& rhs) const
                {
                    return GetNativeSocket () == rhs.GetNativeSocket ();
                }
                inline int Socket::Ptr::Compare (const Ptr& rhs) const
                {
                    return Common::CompareNormalizer (GetNativeSocket (), rhs.GetNativeSocket ());
                }

                /*
                 ********************************************************************************
                 ****************************** Socket operators ********************************
                 ********************************************************************************
                 */
                inline bool operator< (const Socket::Ptr& lhs, const Socket::Ptr& rhs)
                {
                    return lhs.Compare (rhs) < 0;
                }
                inline bool operator<= (const Socket::Ptr& lhs, const Socket::Ptr& rhs)
                {
                    return lhs.Compare (rhs) <= 0;
                }
                inline bool operator== (const Socket::Ptr& lhs, const Socket::Ptr& rhs)
                {
                    return lhs.Equals (rhs);
                }
                inline bool operator!= (const Socket::Ptr& lhs, const Socket::Ptr& rhs)
                {
                    return not lhs.Equals (rhs);
                }
                inline bool operator>= (const Socket::Ptr& lhs, const Socket::Ptr& rhs)
                {
                    return lhs.Compare (rhs) >= 0;
                }
                inline bool operator> (const Socket::Ptr& lhs, const Socket::Ptr& rhs)
                {
                    return lhs.Compare (rhs) > 0;
                }

                /*
                 ********************************************************************************
                 ************** Foundation::IO::Network::ConnectionlessSocket *******************
                 ********************************************************************************
                 */
                inline ConnectionlessSocket& ConnectionlessSocket::operator= (ConnectionlessSocket&& s)
                {
                    inherited::operator= (move (s));
                    return *this;
                }
                inline shared_ptr<ConnectionlessSocket::Ptr::_IRep> ConnectionlessSocket::_GetSharedRep () const
                {
                    return dynamic_pointer_cast<ConnectionlessSocket::_IRep> (inherited::_GetSharedRep ());
                }
                inline ConnectionlessSocket::_IRep& ConnectionlessSocket::_ref ()
                {
                    AssertMember (&inherited::_ref (), _IRep);
                    return *reinterpret_cast<_IRep*> (&inherited::_ref ());
                }
                inline const ConnectionlessSocket::_IRep& ConnectionlessSocket::_cref () const
                {
                    AssertMember (&inherited::_cref (), _IRep);
                    return *reinterpret_cast<const _IRep*> (&inherited::_cref ());
                }
                inline uint8_t ConnectionlessSocket::GetMulticastTTL () const
                {
                    return _cref ().GetMulticastTTL ();
                }
                inline void ConnectionlessSocket::SetMulticastTTL (uint8_t ttl)
                {
                    _ref ().SetMulticastTTL (ttl);
                }
                inline bool ConnectionlessSocket::GetMulticastLoopMode ()
                {
                    return _cref ().GetMulticastLoopMode ();
                }
                inline void ConnectionlessSocket::SetMulticastLoopMode (bool loopMode)
                {
                    _ref ().SetMulticastLoopMode (loopMode);
                }
                inline void ConnectionlessSocket::JoinMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface)
                {
                    _ref ().JoinMulticastGroup (iaddr, onInterface);
                }
                inline void ConnectionlessSocket::LeaveMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface)
                {
                    _ref ().LeaveMulticastGroup (iaddr, onInterface);
                }
                inline void ConnectionlessSocket::SendTo (const Byte* start, const Byte* end, const SocketAddress& sockAddr)
                {
                    _ref ().SendTo (start, end, sockAddr);
                }
                inline size_t ConnectionlessSocket::ReceiveFrom (Byte* intoStart, Byte* intoEnd, int flag, SocketAddress* fromAddress, Time::DurationSecondsType timeout)
                {
                    return _ref ().ReceiveFrom (intoStart, intoEnd, flag, fromAddress, timeout);
                }

                /*
                 ********************************************************************************
                 ************ Foundation::IO::Network::ConnectionlessSocket::Ptr ****************
                 ********************************************************************************
                 */
                inline ConnectionlessSocket::Ptr::Ptr (nullptr_t)
                    : inherited (shared_ptr<inherited::_IRep>{})
                {
                }
                inline ConnectionlessSocket::Ptr::Ptr (const ConnectionlessSocket& src)
                    : inherited (src._GetSharedRep ())
                {
                }
                inline ConnectionlessSocket::Ptr::Ptr (const Ptr& src)
                    : inherited (src._GetSharedRep ())
                {
                }
                inline ConnectionlessSocket::Ptr::Ptr (Ptr&& src)
                    : inherited (src._GetSharedRep ())
                {
                }

                /*
                 ********************************************************************************
                 ************ Foundation::IO::Network::ConnectionOrientedSocket *****************
                 ********************************************************************************
                 */
                inline ConnectionOrientedSocket& ConnectionOrientedSocket::operator= (ConnectionOrientedSocket&& s)
                {
                    inherited::operator= (move (s));
                    return *this;
                }
                inline shared_ptr<ConnectionOrientedSocket::Ptr::_IRep> ConnectionOrientedSocket::_GetSharedRep () const
                {
                    return dynamic_pointer_cast<ConnectionOrientedSocket::_IRep> (inherited::_GetSharedRep ());
                }
                inline ConnectionOrientedSocket::_IRep& ConnectionOrientedSocket::_ref ()
                {
                    AssertMember (&inherited::_ref (), _IRep);
                    return *reinterpret_cast<_IRep*> (&inherited::_ref ());
                }
                inline const ConnectionOrientedSocket::_IRep& ConnectionOrientedSocket::_cref () const
                {
                    AssertMember (&inherited::_cref (), _IRep);
                    return *reinterpret_cast<const _IRep*> (&inherited::_cref ());
                }
                inline void ConnectionOrientedSocket::Close ()
                {
                    inherited::Close ();
                }
                inline void ConnectionOrientedSocket::Connect (const SocketAddress& sockAddr)
                {
                    return _ref ().Connect (sockAddr);
                }
                inline size_t ConnectionOrientedSocket::Read (Byte* intoStart, Byte* intoEnd)
                {
                    return _ref ().Read (intoStart, intoEnd);
                }
                inline void ConnectionOrientedSocket::Write (const Byte* start, const Byte* end)
                {
                    _ref ().Write (start, end);
                }
                inline Optional<IO::Network::SocketAddress> ConnectionOrientedSocket::GetPeerAddress () const
                {
                    return _cref ().GetPeerAddress ();
                }
                inline Optional<Time::DurationSecondsType> ConnectionOrientedSocket::GetAutomaticTCPDisconnectOnClose () const
                {
                    return _cref ().GetAutomaticTCPDisconnectOnClose ();
                }
                inline void ConnectionOrientedSocket::SetAutomaticTCPDisconnectOnClose (const Optional<Time::DurationSecondsType>& linger)
                {
                    _ref ().SetAutomaticTCPDisconnectOnClose (linger);
                }
                inline auto ConnectionOrientedSocket::GetKeepAlives () -> KeepAliveOptions
                {
                    return _cref ().GetKeepAlives ();
                }
                inline void ConnectionOrientedSocket::SetKeepAlives (const KeepAliveOptions& keepAliveOptions)
                {
                    _ref ().SetKeepAlives (keepAliveOptions);
                }

                /*
                 ********************************************************************************
                 ************ Foundation::IO::Network::ConnectionOrientedSocket::Ptr ****************
                 ********************************************************************************
                 */
                inline ConnectionOrientedSocket::Ptr::Ptr (nullptr_t)
                    : inherited (shared_ptr<inherited::_IRep>{})
                {
                }
                inline ConnectionOrientedSocket::Ptr::Ptr (const ConnectionOrientedSocket& src)
                    : inherited (src._GetSharedRep ())
                {
                }
                inline ConnectionOrientedSocket::Ptr::Ptr (const Ptr& src)
                    : inherited (src._GetSharedRep ())
                {
                }
                inline ConnectionOrientedSocket::Ptr::Ptr (Ptr&& src)
                    : inherited (src._GetSharedRep ())
                {
                }

                /*
                 ********************************************************************************
                 ********** Foundation::IO::Network::ConnectionOrientedMasterSocket *************
                 ********************************************************************************
                 */
                inline ConnectionOrientedMasterSocket& ConnectionOrientedMasterSocket::operator= (ConnectionOrientedMasterSocket&& s)
                {
                    inherited::operator= (move (s));
                    return *this;
                }
                inline shared_ptr<ConnectionOrientedMasterSocket::Ptr::_IRep> ConnectionOrientedMasterSocket::_GetSharedRep () const
                {
                    return dynamic_pointer_cast<ConnectionOrientedMasterSocket::_IRep> (inherited::_GetSharedRep ());
                }
                inline ConnectionOrientedMasterSocket::_IRep& ConnectionOrientedMasterSocket::_ref ()
                {
                    AssertMember (&inherited::_ref (), _IRep);
                    return *reinterpret_cast<_IRep*> (&inherited::_ref ());
                }
                inline const ConnectionOrientedMasterSocket::_IRep& ConnectionOrientedMasterSocket::_cref () const
                {
                    AssertMember (&inherited::_cref (), _IRep);
                    return *reinterpret_cast<const _IRep*> (&inherited::_cref ());
                }
                inline void ConnectionOrientedMasterSocket::Listen (unsigned int backlog)
                {
                    _ref ().Listen (backlog);
                }
                inline ConnectionOrientedSocket::Ptr ConnectionOrientedMasterSocket::Accept ()
                {
                    return _ref ().Accept ();
                }

                /*
                 ********************************************************************************
                 ********** Foundation::IO::Network::ConnectionOrientedMasterSocket::Ptr ********
                 ********************************************************************************
                 */
                inline ConnectionOrientedMasterSocket::Ptr::Ptr (nullptr_t)
                    : inherited (shared_ptr<inherited::_IRep>{})
                {
                }
                inline ConnectionOrientedMasterSocket::Ptr::Ptr (const ConnectionOrientedMasterSocket& src)
                    : inherited (src._GetSharedRep ())
                {
                }
                inline ConnectionOrientedMasterSocket::Ptr::Ptr (const Ptr& src)
                    : inherited (src._GetSharedRep ())
                {
                }
                inline ConnectionOrientedMasterSocket::Ptr::Ptr (Ptr&& src)
                    : inherited (src._GetSharedRep ())
                {
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_IO_Network_Socket_inl_*/
