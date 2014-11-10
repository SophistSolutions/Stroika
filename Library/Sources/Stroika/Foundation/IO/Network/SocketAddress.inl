/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
                    static_assert (sizeof (iaddr) <= sizeof (fSocketAddress_), "sizeof (iaddr) <= sizeof (fSocketAddress_)");
                    (void)::memcpy (&fSocketAddress_, &iaddr, sizeof (iaddr));
                }
                inline  SocketAddress::SocketAddress (const sockaddr_in& iaddr)
                    : fSocketAddress_ ()
                {
                    static_assert (sizeof (iaddr) <= sizeof (fSocketAddress_), "sizeof (iaddr) <= sizeof (fSocketAddress_)");
                    (void)::memcpy (&fSocketAddress_, &iaddr, sizeof (iaddr));
                }
                inline  SocketAddress::SocketAddress (const sockaddr_storage& iaddr)
                    : fSocketAddress_ ()
                {
                    static_assert (sizeof (iaddr) == sizeof (fSocketAddress_), "sizeof (iaddr) <= sizeof (fSocketAddress_)");
                    (void)::memcpy (&fSocketAddress_, &iaddr, sizeof (iaddr));
                }
#if     qPlatform_Windows
                inline  SocketAddress::SocketAddress (const SOCKET_ADDRESS& sockaddr)
                    : fSocketAddress_ ()
                {
                    if (sockaddr.iSockaddrLength > sizeof (fSocketAddress_)) {
                        Execution::DoThrow (Execution::StringException (L"bad socket address size"));
                    }
                    (void)::memcpy (&fSocketAddress_, sockaddr.lpSockaddr, sockaddr.iSockaddrLength);
                }
#endif
                inline  SocketAddress::SocketAddress (const InternetAddress& iaddr, uint16_t portNumber)
                    : fSocketAddress_ ()
                {
                    (void)::memset (&fSocketAddress_, 0, sizeof (fSocketAddress_));
                    switch (iaddr.GetAddressFamily ()) {
                        case    InternetAddress::AddressFamily::V4: {
                                Assert (sizeof (sockaddr_in) == sizeof (sockaddr));
                                sockaddr_in& as = reinterpret_cast<sockaddr_in&> (fSocketAddress_);
                                as.sin_family = AF_INET;
                                DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Wdeprecated\"");  // macro uses 'register' - htons not deprecated
                                as.sin_port = htons (portNumber);
                                DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Wdeprecated\"");
                                as.sin_addr = iaddr.As<in_addr> ();
                            }
                            break;
                        case    InternetAddress::AddressFamily::V6: {
                                Assert (sizeof (sockaddr_in) == sizeof (sockaddr));
                                sockaddr_in6& as = reinterpret_cast<sockaddr_in6&> (fSocketAddress_);
                                as.sin6_family = AF_INET6;
                                DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Wdeprecated\"");  // macro uses 'register' - htons not deprecated
                                as.sin6_port = htons (portNumber);
                                DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Wdeprecated\"");
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
                    return fSocketAddress_.ss_family == AF_UNSPEC;
                    //constexpr sockaddr kZero_ = { 0 };
                    //return ::memcmp (&fSocketAddress_, &kZero_, sizeof (kZero_)) == 0;
                }
                inline  void    SocketAddress::clear ()
                {
                    fSocketAddress_.ss_family = AF_UNSPEC;
                    //::memset (&fSocketAddress_, 0, sizeof (fSocketAddress_));
                }
                inline  SocketAddress::FamilyType    SocketAddress::GetAddressFamily () const
                {
                    return fSocketAddress_.ss_family;
                }
                inline  bool    SocketAddress::IsInternetAddress () const
                {
                    return fSocketAddress_.ss_family == AF_INET or fSocketAddress_.ss_family == AF_INET6;
                }
                inline  InternetAddress    SocketAddress::GetInternetAddress () const
                {
                    Require (IsInternetAddress ());
                    switch (fSocketAddress_.ss_family) {
                        case AF_INET: {
                                Require (fSocketAddress_.ss_family == AF_INET or fSocketAddress_.ss_family == AF_UNSPEC);
                                static_assert (sizeof (sockaddr_in) <= sizeof (fSocketAddress_), "sizeof (sockaddr_in) <= sizeof (fSocketAddress_)");
                                const sockaddr_in& as = reinterpret_cast<const sockaddr_in&> (fSocketAddress_);
                                return InternetAddress (as.sin_addr);
                            }
                        case AF_INET6: {
                                Require (fSocketAddress_.ss_family == AF_INET6 or fSocketAddress_.ss_family == AF_UNSPEC);
                                static_assert (sizeof (sockaddr_in6) <= sizeof (fSocketAddress_), "sizeof (sockaddr_in6) <= sizeof (fSocketAddress_)");
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
                    switch (fSocketAddress_.ss_family) {
                        case AF_INET: {
                                Require (fSocketAddress_.ss_family == AF_INET or fSocketAddress_.ss_family == AF_UNSPEC);
                                static_assert (sizeof (sockaddr_in) <= sizeof (fSocketAddress_), "sizeof (sockaddr_in) <= sizeof (fSocketAddress_)");
                                const sockaddr_in& as = reinterpret_cast<const sockaddr_in&> (fSocketAddress_);
                                DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Wdeprecated\"");  // macro uses 'register' - htons not deprecated
                                return ntohs (as.sin_port);
                                DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Wdeprecated\"");  // macro uses 'register' - htons not deprecated
                            }
                        case AF_INET6: {
                                Require (fSocketAddress_.ss_family == AF_INET6 or fSocketAddress_.ss_family == AF_UNSPEC);
                                static_assert (sizeof (sockaddr_in6) <= sizeof (fSocketAddress_), "sizeof (sockaddr_in6) <= sizeof (fSocketAddress_)");
                                const sockaddr_in6& as = reinterpret_cast<const sockaddr_in6&> (fSocketAddress_);
                                DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Wdeprecated\"");  // macro uses 'register' - htons not deprecated
                                return ntohs (as.sin6_port);
                                DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Wdeprecated\"");  // macro uses 'register' - htons not deprecated
                            }
                        default: {
                                AssertNotReached ();
                                return 0;
                            }
                    }
                }
                template    <typename T>
                T   SocketAddress::As () const
                {
#if     qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy
                    RequireNotReached ();
#else
                    static_assert (false, "Only specifically specialized variants are supported");
#endif
                }
                template    <>
                inline  sockaddr   SocketAddress::As<sockaddr> () const
                {
                    Require (fSocketAddress_.ss_family == AF_INET or fSocketAddress_.ss_family == AF_UNSPEC);
                    static_assert (sizeof (sockaddr) <= sizeof (fSocketAddress_), "sizeof (sockaddr_in6) <= sizeof (fSocketAddress_)");
                    return *reinterpret_cast<const sockaddr*> (&fSocketAddress_);
                }
                template    <>
                inline  sockaddr_in   SocketAddress::As<sockaddr_in> () const
                {
                    Require (fSocketAddress_.ss_family == AF_INET or fSocketAddress_.ss_family == AF_UNSPEC);
                    static_assert (sizeof (sockaddr_in) <= sizeof (fSocketAddress_), "sizeof (sockaddr_in6) <= sizeof (fSocketAddress_)");
                    const sockaddr_in& as = reinterpret_cast<const sockaddr_in&> (fSocketAddress_);
                    return as;
                }
                template    <>
                inline  sockaddr_in6   SocketAddress::As<sockaddr_in6> () const
                {
                    Require (fSocketAddress_.ss_family == AF_INET6 or fSocketAddress_.ss_family == AF_UNSPEC);
                    static_assert (sizeof (sockaddr_in6) <= sizeof (fSocketAddress_), "sizeof (sockaddr_in6) <= sizeof (fSocketAddress_)");
                    const sockaddr_in6& as = reinterpret_cast<const sockaddr_in6&> (fSocketAddress_);
                    return as;
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_Network_SocketAddress_inl_*/
