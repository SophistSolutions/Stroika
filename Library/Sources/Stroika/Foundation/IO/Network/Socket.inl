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
                 *********************** Foundation::IO::Network::Socket ************************
                 ********************************************************************************
                 */
                inline Socket::Socket (const shared_ptr<_IRep>& src)
                    : fRep_ (src)
                {
                }
                inline Socket::Socket (shared_ptr<_IRep>&& src)
                    : fRep_ (move (src))
                {
                }
                inline shared_ptr<Socket::Ptr::_IRep> Socket::_GetSharedRep () const
                {
                    return fRep_;
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
                inline Optional<IO::Network::SocketAddress> Socket::GetLocalAddress () const
                {
                    return _cref ().GetLocalAddress ();
                }
                inline SocketAddress::FamilyType Socket::GetAddressFamily () const
                {
                    return _cref ().GetAddressFamily ();
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
                 ********************* Foundation::IO::Network::Socket::Ptr *********************
                 ********************************************************************************
                 */
                inline Socket::Ptr::Ptr (const shared_ptr<_IRep>& rep)
                    : inherited (rep)
                {
                }
                inline Socket::Ptr::Ptr (shared_ptr<_IRep>&& rep)
                    : inherited (std::move (rep))
                {
                }
                inline Socket::Ptr::Ptr (Ptr&& s)
                    : inherited (std::move (s.fRep_))
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
            }
        }
    }
}
#endif /*_Stroika_Foundation_IO_Network_Socket_inl_*/
