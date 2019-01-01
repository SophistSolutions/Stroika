/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_SocketAddress_inl_
#define _Stroika_Foundation_IO_Network_SocketAddress_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::IO::Network {

    /*
     ********************************************************************************
     *********************** IO::Network::SocketAddress *****************************
     ********************************************************************************
     */
    inline SocketAddress::SocketAddress ()
        : fSocketAddress_{}
    {
    }
    inline SocketAddress::SocketAddress (const sockaddr& iaddr)
    {
        fSocketAddress_ = iaddr;
    }
    inline SocketAddress::SocketAddress (const sockaddr_in& iaddr)
    {
        fSocketAddress_V4_ = iaddr;
    }
    inline SocketAddress::SocketAddress (const sockaddr_in6& iaddr)
    {
        fSocketAddress_V6_ = iaddr;
    }
    inline SocketAddress::SocketAddress (const sockaddr_storage& iaddr)
        : fSocketAddress_ ()
    {
        fSocketAddressStorage_ = iaddr;
    }
    inline SocketAddress::SocketAddress (const InternetAddress& iaddr, uint16_t portNumber)
    {
        switch (iaddr.GetAddressFamily ()) {
            case InternetAddress::AddressFamily::V4: {
                fSocketAddress_V4_            = sockaddr_in{};
                fSocketAddress_V4_.sin_family = AF_INET;
                DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated\""); // macro uses 'register' - htons not deprecated
                fSocketAddress_V4_.sin_port = htons (portNumber);                                   //NB no '::' cuz some systems use macro
                DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated\"");
                fSocketAddress_V4_.sin_addr = iaddr.As<in_addr> ();
            } break;
            case InternetAddress::AddressFamily::V6: {
                fSocketAddress_V6_             = sockaddr_in6{};
                fSocketAddress_V6_.sin6_family = AF_INET6;
                DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated\""); // macro uses 'register' - htons not deprecated
                fSocketAddress_V6_.sin6_port = htons (portNumber);                                  //NB no ':' cuz some systems use macro
                DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated\"");
                fSocketAddress_V6_.sin6_addr = iaddr.As<in6_addr> ();
            } break;
            default: {
                // just leave blank - no assert?
                fSocketAddress_ = sockaddr{};
            } break;
        }
    }
    inline bool SocketAddress::empty () const
    {
        return fSocketAddress_.sa_family == AF_UNSPEC;
        //constexpr sockaddr kZero_ = { 0 };
        //return ::memcmp (&fSocketAddress_, &kZero_, sizeof (kZero_)) == 0;
    }
    inline void SocketAddress::clear ()
    {
        fSocketAddress_ = sockaddr{};
        Assert (fSocketAddress_.sa_family == AF_UNSPEC);
    }
    inline SocketAddress::FamilyType SocketAddress::GetAddressFamily () const
    {
        return static_cast<FamilyType> (fSocketAddress_.sa_family);
    }
    inline size_t SocketAddress::GetRequiredSize () const
    {
        switch (fSocketAddress_.sa_family) {
            case AF_INET: {
                return sizeof (fSocketAddress_V4_);
            }
            case AF_INET6: {
                return sizeof (fSocketAddress_V6_);
            }
            default: {
                return sizeof (fSocketAddressStorage_);
            }
        }
    }
    inline bool SocketAddress::IsInternetAddress () const
    {
        return fSocketAddress_.sa_family == AF_INET or fSocketAddress_.sa_family == AF_INET6;
    }
    inline InternetAddress SocketAddress::GetInternetAddress () const
    {
        Require (IsInternetAddress ());
        switch (fSocketAddress_.sa_family) {
            case AF_INET: {
                return InternetAddress (fSocketAddress_V4_.sin_addr);
            }
            case AF_INET6: {
                return InternetAddress (fSocketAddress_V6_.sin6_addr);
            }
            default: {
                AssertNotReached ();
                return InternetAddress{};
            }
        }
    }
    inline uint16_t SocketAddress::GetPort () const
    {
        Require (IsInternetAddress ());
        switch (fSocketAddress_.sa_family) {
            case AF_INET: {
                DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated\""); // macro uses 'register' - htons not deprecated
                return ntohs (fSocketAddress_V4_.sin_port);                                         //NB no ':' cuz some systems use macro
                DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated\"");   // macro uses 'register' - htons not deprecated
            }
            case AF_INET6: {
                DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated\""); // macro uses 'register' - htons not deprecated
                return ntohs (fSocketAddress_V6_.sin6_port);                                        //NB no ':' cuz some systems use macro
                DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated\"");   // macro uses 'register' - htons not deprecated
            }
            default: {
                AssertNotReached ();
                return 0;
            }
        }
    }
    template <>
    inline sockaddr SocketAddress::As<sockaddr> () const
    {
        Require (fSocketAddress_.sa_family == AF_INET or fSocketAddress_.sa_family == AF_UNSPEC); // You probably meant to use sockaddr_storage (to support IPv6)
        return fSocketAddress_;
    }
    template <>
    inline sockaddr_storage SocketAddress::As<sockaddr_storage> () const
    {
        return fSocketAddressStorage_;
    }
    template <>
    inline sockaddr_in SocketAddress::As<sockaddr_in> () const
    {
        Require (fSocketAddress_.sa_family == AF_INET or fSocketAddress_.sa_family == AF_UNSPEC);
        return fSocketAddress_V4_;
    }
    template <>
    inline sockaddr_in6 SocketAddress::As<sockaddr_in6> () const
    {
        Require (fSocketAddress_.sa_family == AF_INET6 or fSocketAddress_.sa_family == AF_UNSPEC);
        return fSocketAddress_V6_;
    }

}

#endif /*_Stroika_Foundation_IO_Network_SocketAddress_inl_*/
