/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
                inline  Socket::Socket (const Socket& s)
                    : fRep_ (s.fRep_)
                {
                }
                inline  Socket::Socket (Socket&&  s)
                    : fRep_ (std::move (s.fRep_))
                {
                }
                inline  Socket::~Socket ()
                {
                }
                inline  Socket& Socket::operator= (const Socket& s)
                {
                    if (fRep_ != s.fRep_) {
                        fRep_ = s.fRep_;
                    }
                    return *this;
                }
                inline  Socket& Socket::operator= (Socket && s)
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
                inline  void    Socket::Listen (unsigned int backlog)
                {
                    fRep_->Listen (backlog);
                }
                inline  Socket  Socket::Accept ()
                {
                    return fRep_->Accept ();
                }
                inline  uint8_t     Socket::GetMulticastTTL ()
                {
                    return fRep_->GetMulticastTTL ();
                }
                inline  void        Socket::SetMulticastTTL (uint8_t ttl)
                {
                    fRep_->SetMulticastTTL (ttl);
                }
                inline  bool        Socket::GetMulticastLoopMode ()
                {
                    return fRep_->GetMulticastLoopMode ();
                }
                inline  void        Socket::SetMulticastLoopMode (bool loopMode)
                {
                    fRep_->SetMulticastLoopMode (loopMode);
                }
                inline  void    Socket::JoinMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface)
                {
                    fRep_->JoinMulticastGroup (iaddr, onInterface);
                }
                inline  void    Socket::LeaveMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface)
                {
                    fRep_->LeaveMulticastGroup (iaddr, onInterface);
                }
                inline  size_t  Socket::Read (Byte* intoStart, Byte* intoEnd)
                {
                    return fRep_->Read (intoStart, intoEnd);
                }
                inline  void    Socket::Write (const Byte* start, const Byte* end)
                {
                    fRep_->Write (start, end);
                }
                inline  void    Socket::SendTo (const Byte* start, const Byte* end, const SocketAddress& sockAddr)
                {
                    fRep_->SendTo (start, end, sockAddr);
                }
                inline  size_t    Socket::ReceiveFrom (Byte* intoStart, Byte* intoEnd, int flag, SocketAddress* fromAddress)
                {
                    return fRep_->ReceiveFrom (intoStart, intoEnd, flag, fromAddress);
                }
                template    <typename RESULT_TYPE>
                inline  RESULT_TYPE Socket::getsockopt (int level, int optname)
                {
                    RESULT_TYPE r {};
                    socklen_t   roptlen = sizeof (r);
                    fRep_->getsockopt (level, optname, &r, &roptlen);
                    return r;
                }



            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_Network_Socket_inl_*/
