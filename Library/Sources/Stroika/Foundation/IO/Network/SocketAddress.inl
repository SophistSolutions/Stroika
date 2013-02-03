/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_SocketAddress_inl_
#define _Stroika_Foundation_IO_Network_SocketAddress_inl_  1


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
                 *********************** IO::Network::SocketAddress *****************************
                 ********************************************************************************
                 */
                inline  SocketAddress::SocketAddress ()
                    : fSocketAddress_ ()
                {
                    ::memset (&fSocketAddress_, 0, sizeof (fSocketAddress_));
                }
                inline  SocketAddress::SocketAddress (const sockaddr& iaddr)
                    : fSocketAddress_ ()
                {
                    Assert  (sizeof (fSocketAddress_) == sizeof (sockaddr));
                    ::memcpy (&fSocketAddress_, &iaddr, sizeof (fSocketAddress_));
                }
                inline  SocketAddress::SocketAddress (const sockaddr_in& iaddr)
                    : fSocketAddress_ ()
                {
                    Assert  (sizeof (fSocketAddress_) == sizeof (sockaddr));
                    Assert  (sizeof (fSocketAddress_) == sizeof (sockaddr_in));
                    ::memcpy (&fSocketAddress_, &iaddr, sizeof (fSocketAddress_));
                }
                inline  SocketAddress::SocketAddress (const InternetAddress& iaddr, uint16_t portNumber)
                    : fSocketAddress_ ()
                {
                    ::memset (&fSocketAddress_, 0, sizeof (fSocketAddress_));
                    switch (iaddr.GetAddressFamily ()) {
                        case    AF_INET: {
                                Assert (sizeof (sockaddr_in) == sizeof (sockaddr));
                                sockaddr_in& as = reinterpret_cast<sockaddr_in&> (fSocketAddress_);
                                as.sin_family = AF_INET;
                                as.sin_port = htons (portNumber);
                                as.sin_addr = iaddr.As<in_addr> ();
                            }
                            break;
                        case    AF_INET6: {
                                Assert (sizeof (sockaddr_in) == sizeof (sockaddr));
                                sockaddr_in6& as = reinterpret_cast<sockaddr_in6&> (fSocketAddress_);
                                as.sin6_family = AF_INET6;
                                as.sin6_port = htons (portNumber);
                                as.sin6_addr = iaddr.As<in6_addr> ();
                            }
                            break;
                        default: {
                                // just leave blank - no assert?
                            }
                            break;
                    }
                }
                inline  bool    SocketAddress::empty () const
                {
                    return fSocketAddress_.sa_family == AF_UNSPEC;
                    //constexpr sockaddr kZero_ = { 0 };
                    //return ::memcmp (&fSocketAddress_, &kZero_, sizeof (kZero_)) == 0;
                }
                inline  void    SocketAddress::clear ()
                {
                    fSocketAddress_.sa_family = AF_UNSPEC;
                    //::memset (&fSocketAddress_, 0, sizeof (fSocketAddress_));
                }
                inline  SocketAddress::FamilyType    SocketAddress::GetAddressFamily () const
                {
                    return fSocketAddress_.sa_family;
                }
                inline  bool    SocketAddress::IsInternetAddress () const
                {
                    return fSocketAddress_.sa_family == AF_INET or fSocketAddress_.sa_family == AF_INET6;
                }
                inline  InternetAddress    SocketAddress::GetInternetAddress () const
                {
                    Require (IsInternetAddress ());
                    switch (fSocketAddress_.sa_family) {
                        case AF_INET: {
                                Assert (sizeof (sockaddr_in) == sizeof (sockaddr));
                                const sockaddr_in& as = reinterpret_cast<const sockaddr_in&> (fSocketAddress_);
                                return InternetAddress (as.sin_addr);
                            }
                        case AF_INET6: {
                                Assert (sizeof (sockaddr_in6) == sizeof (sockaddr));
                                const sockaddr_in6& as = reinterpret_cast<const sockaddr_in6&> (fSocketAddress_);
                                return InternetAddress (as.sin6_addr);
                            }
                        default: {
                                AssertNotReached ();
                                return InternetAddress ();
                            }
                    }
                }
                inline  uint16_t    SocketAddress::GetPort () const
                {
                    Require (IsInternetAddress ());
                    switch (fSocketAddress_.sa_family) {
                        case AF_INET: {
                                Assert (sizeof (sockaddr_in) == sizeof (sockaddr));
                                const sockaddr_in& as = reinterpret_cast<const sockaddr_in&> (fSocketAddress_);
                                return ntohs (as.sin_port);
                            }
                        case AF_INET6: {
                                Assert (sizeof (sockaddr_in6) == sizeof (sockaddr));
                                const sockaddr_in6& as = reinterpret_cast<const sockaddr_in6&> (fSocketAddress_);
                                return ntohs (as.sin6_port);
                            }
                        default: {
                                AssertNotReached ();
                                return 0;
                            }
                    }
                }
#if     !qCompilerAndStdLib_FailsStaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded
                template    <typename T>
                T   SocketAddress::As () const
                {
                    static_assert (false, "Only specifically specialized variants are supported");
                }
#endif
                template    <>
                inline  sockaddr   SocketAddress::As<sockaddr> () const
                {
                    return fSocketAddress_;
                }
                template    <>
                inline  sockaddr_in   SocketAddress::As<sockaddr_in> () const
                {
                    Assert (sizeof (sockaddr_in6) == sizeof (sockaddr));
                    const sockaddr_in& as = reinterpret_cast<const sockaddr_in&> (fSocketAddress_);
                    return as;
                }
                template    <>
                inline  sockaddr_in6   SocketAddress::As<sockaddr_in6> () const
                {
                    Assert (sizeof (sockaddr_in6) == sizeof (sockaddr));
                    const sockaddr_in6& as = reinterpret_cast<const sockaddr_in6&> (fSocketAddress_);
                    return as;
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_Network_SocketAddress_inl_*/
