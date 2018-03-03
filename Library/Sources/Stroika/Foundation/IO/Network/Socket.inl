/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
                inline Socket::Ptr::Ptr (nullptr_t)
                {
                }
                inline Socket::Ptr::Ptr (const shared_ptr<_IRep>& rep)
                    : fRep_ (rep)
                {
                }
                inline Socket::Ptr::Ptr (shared_ptr<_IRep>&& rep)
                    : fRep_ (std::move (rep))
                {
                }
                inline Socket::Ptr& Socket::Ptr::operator= (Ptr&& s)
                {
                    fRep_ = move (s.fRep_);
                    return *this;
                }
                inline Socket::Ptr& Socket::Ptr::operator= (const Ptr& s)
                {
                    fRep_ = s.fRep_;
                    return *this;
                }
                inline void Socket::Ptr::reset () noexcept
                {
                    fRep_.reset ();
                }
                inline shared_ptr<Socket::_IRep> Socket::Ptr::_GetSharedRep () const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    return fRep_;
                }
                inline Socket::_IRep& Socket::Ptr::_ref () const
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
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    return _cref ().GetNativeSocket ();
                }
                inline Optional<IO::Network::SocketAddress> Socket::Ptr::GetLocalAddress () const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    return _cref ().GetLocalAddress ();
                }
                inline SocketAddress::FamilyType Socket::Ptr::GetAddressFamily () const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    return _cref ().GetAddressFamily ();
                }
                inline void Socket::Ptr::Shutdown (ShutdownTarget shutdownTarget)
                {
                    // not important to null-out, but may as well...
                    if (fRep_ != nullptr) {
                        _ref ().Shutdown (shutdownTarget);
                    }
                }
                inline void Socket::Ptr::Close () const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    // not important to null-out, but may as well...
                    if (fRep_ != nullptr) {
                        fRep_->Close ();
                        //fRep_.reset ();  changed in v2.0a209
                    }
                }
                template <typename RESULT_TYPE>
                inline RESULT_TYPE Socket::Ptr::getsockopt (int level, int optname) const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    RESULT_TYPE                                         r{};
                    socklen_t                                           roptlen = sizeof (r);
                    _cref ().getsockopt (level, optname, &r, &roptlen);
                    return r;
                }
                template <typename ARG_TYPE>
                inline void Socket::Ptr::setsockopt (int level, int optname, ARG_TYPE arg) const
                {
                    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                    socklen_t                                          optvallen = sizeof (arg);
                    _ref ().setsockopt (level, optname, &arg, optvallen);
                }
                inline bool Socket::Ptr::Equals (const Socket::Ptr& rhs) const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    /* 
                     *  Used to check GetNativeSocket () == rhs.GetNativeSocket ();
                     *  but this is better. It practically always amounts to the same thing (since one typically constructs
                     *  a Socket object, and copies that as a Ref - thought it CAN be differnt if you manually attach
                     *  the same low level socket to another Stroika socket object). And comparing with GetNativeSocket () - requires
                     *  being careful about null ptrs.
                     */
                    return _GetSharedRep () == rhs._GetSharedRep ();
                }
                inline int Socket::Ptr::Compare (const Socket::Ptr& rhs) const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    /* 
                     *  Used to check Common::CompareNormalizer (GetNativeSocket (), rhs.GetNativeSocket ());
                     *  but this is better. It practically always amounts to the same thing (since one typically constructs
                     *  a Socket object, and copies that as a Ref - thought it CAN be differnt if you manually attach
                     *  the same low level socket to another Stroika socket object). And comparing with GetNativeSocket () - requires
                     *  being careful about null ptrs.
                     */
                    return Common::CompareNormalizer (_GetSharedRep (), rhs._GetSharedRep ());
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
