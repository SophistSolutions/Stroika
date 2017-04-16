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
                inline Socket::BindFlags::BindFlags (bool reUseAddr)
                    : fReUseAddr (reUseAddr)
                {
                }

                /*
                 ********************************************************************************
                 ********************* Foundation::IO::Network::Socket **************************
                 ********************************************************************************
                 */
                inline Socket::Socket (Socket&& s)
                    : fRep_ (std::move (s.fRep_))
                {
                }
                inline Socket& Socket::operator= (const Socket& s)
                {
                    if (fRep_ != s.fRep_) {
                        fRep_ = s.fRep_;
                    }
                    return *this;
                }
                inline Socket& Socket::operator= (Socket&& s)
                {
                    if (fRep_ != s.fRep_) {
                        fRep_ = std::move (s.fRep_);
                    }
                    return *this;
                }
                inline Socket::_IRep& Socket::_ref ()
                {
                    RequireNotNull (fRep_);
                    return *fRep_;
                }
                inline const Socket::_IRep& Socket::_cref () const
                {
                    RequireNotNull (fRep_);
                    return *fRep_;
                }
                inline Socket::PlatformNativeHandle Socket::GetNativeSocket () const
                {
                    return _cref ().GetNativeSocket ();
                }
                inline uint8_t Socket::GetMulticastTTL () const
                {
                    return _cref ().GetMulticastTTL ();
                }
                inline void Socket::SetMulticastTTL (uint8_t ttl)
                {
                    _ref ().SetMulticastTTL (ttl);
                }
                inline bool Socket::GetMulticastLoopMode ()
                {
                    return _cref ().GetMulticastLoopMode ();
                }
                inline void Socket::SetMulticastLoopMode (bool loopMode)
                {
                    _ref ().SetMulticastLoopMode (loopMode);
                }
                inline Optional<IO::Network::SocketAddress> Socket::GetLocalAddress () const
                {
                    return _cref ().GetLocalAddress ();
                }
                inline void Socket::JoinMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface)
                {
                    _ref ().JoinMulticastGroup (iaddr, onInterface);
                }
                inline void Socket::LeaveMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface)
                {
                    _ref ().LeaveMulticastGroup (iaddr, onInterface);
                }
                inline void Socket::SendTo (const Byte* start, const Byte* end, const SocketAddress& sockAddr)
                {
                    _ref ().SendTo (start, end, sockAddr);
                }
                inline size_t Socket::ReceiveFrom (Byte* intoStart, Byte* intoEnd, int flag, SocketAddress* fromAddress, Time::DurationSecondsType timeout)
                {
                    return _ref ().ReceiveFrom (intoStart, intoEnd, flag, fromAddress, timeout);
                }
                inline void Socket::Shutdown (ShutdownTarget shutdownTarget)
                {
                    // not important to null-out, but may as well...
                    if (fRep_ != nullptr) {
                        _ref ().Shutdown (shutdownTarget);
                    }
                }
                inline void Socket::Close ()
                {
                    // not important to null-out, but may as well...
                    if (fRep_ != nullptr) {
                        fRep_->Close ();
                        fRep_.reset ();
                    }
                }
                template <typename RESULT_TYPE>
                inline RESULT_TYPE Socket::getsockopt (int level, int optname) const
                {
                    RESULT_TYPE r{};
                    socklen_t   roptlen = sizeof (r);
                    _cref ().getsockopt (level, optname, &r, &roptlen);
                    return r;
                }
                template <typename ARG_TYPE>
                inline void Socket::setsockopt (int level, int optname, ARG_TYPE arg)
                {
                    socklen_t optvallen = sizeof (arg);
                    _ref ().setsockopt (level, optname, &arg, optvallen);
                }
                inline bool Socket::Equals (const Socket& rhs) const
                {
                    return GetNativeSocket () == rhs.GetNativeSocket ();
                }
                inline int Socket::Compare (const Socket& rhs) const
                {
                    return Common::CompareNormalizer (GetNativeSocket (), rhs.GetNativeSocket ());
                }

                /*
                 ********************************************************************************
                 ****************************** Socket operators ********************************
                 ********************************************************************************
                 */
                inline bool operator< (const Socket& lhs, const Socket& rhs)
                {
                    return lhs.Compare (rhs) < 0;
                }
                inline bool operator<= (const Socket& lhs, const Socket& rhs)
                {
                    return lhs.Compare (rhs) <= 0;
                }
                inline bool operator== (const Socket& lhs, const Socket& rhs)
                {
                    return lhs.Equals (rhs);
                }
                inline bool operator!= (const Socket& lhs, const Socket& rhs)
                {
                    return not lhs.Equals (rhs);
                }
                inline bool operator>= (const Socket& lhs, const Socket& rhs)
                {
                    return lhs.Compare (rhs) >= 0;
                }
                inline bool operator> (const Socket& lhs, const Socket& rhs)
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
                 ********** Foundation::IO::Network::ConnectionOrientedMasterSocket *************
                 ********************************************************************************
                 */
                inline ConnectionOrientedMasterSocket& ConnectionOrientedMasterSocket::operator= (ConnectionOrientedMasterSocket&& s)
                {
                    inherited::operator= (move (s));
                    return *this;
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
                inline ConnectionOrientedSocket ConnectionOrientedMasterSocket::Accept ()
                {
                    return _ref ().Accept ();
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_IO_Network_Socket_inl_*/
