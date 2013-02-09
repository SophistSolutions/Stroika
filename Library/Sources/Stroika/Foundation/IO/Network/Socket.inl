/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Socket_inl_
#define _Stroika_Foundation_IO_Network_Socket_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {


                /*
                 ********************************************************************************
                 *************** Foundation::IO::Network::Socket::BindFlags *********************
                 ********************************************************************************
                 */
                inline  Socket::BindFlags::BindFlags (bool reUseAddr)
                    : fReUseAddr (reUseAddr)
                {
                }


                /*
                 ********************************************************************************
                 ********************* Foundation::IO::Network::Socket **************************
                 ********************************************************************************
                 */
                inline  Socket::Socket ()
                    : fRep_ ()
                {
                }
                inline  Socket::Socket (const shared_ptr<_Rep>& rep)
                    : fRep_ (rep)
                {
                }
                inline  Socket::Socket (shared_ptr<_Rep> && rep)
                    : fRep_ (std::move (rep))
                {
                }
                inline  Socket::Socket (const Socket& s)
                    : fRep_ (s.fRep_)
                {
                }
                inline  Socket::Socket (Socket && s)
                    : fRep_ (std::move (s.fRep_))
                {
                }
                inline  Socket::~Socket ()
                {
                }
                inline  const Socket& Socket::operator= (const Socket& s)
                {
                    if (fRep_ != s.fRep_) {
                        fRep_ = s.fRep_;
                    }
                    return *this;
                }
                inline  const Socket& Socket::operator= (Socket && s)
                {
                    if (fRep_ != s.fRep_) {
                        fRep_ = std::move (s.fRep_);
                    }
                    return *this;
                }
                inline  Socket::PlatformNativeHandle    Socket::GetNativeSocket () const
                {
                    return fRep_->GetNativeSocket ();
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_Network_Socket_inl_*/
