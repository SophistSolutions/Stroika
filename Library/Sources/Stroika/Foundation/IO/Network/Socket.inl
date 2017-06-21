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
            }
        }
    }
}
#endif /*_Stroika_Foundation_IO_Network_Socket_inl_*/
