/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_InternetAddress_h_
#define _Stroika_Foundation_IO_Network_InternetAddress_h_    1

#include    "../../StroikaPreComp.h"

#if     qPlatform_POSIX
#include    <arpa/inet.h>
#elif   qPlatform_Windows
#include    <WinSock2.h>
#include    <inaddr.h>
#include    <in6addr.h>
#endif

#include    "../../Characters/String.h"
#include    "../../Configuration/Common.h"



/**
 * TODO:
 *
 *      @todo   IPV6 code not fully implemented on windows (pre-windows-vista)
 *
 *      @todo   Future versions may support converting from IPV4 address to IPV6 by assigning an
 *              IPV4 and saying As<in6_addr> ()? Or maybe have ToIPV6() method?
 *
 *      @todo   Check impl of IsMulticastAddress/IsLocalhostAddress - esp for IPV4 -
 *              and endianness. Now about 80% sure right, and have some review code,
 *              but could use more review.
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {


                using   Characters::String;


                /**
                 *  InternetAddress repesents an internet IP address. Its easily convertable to/from
                 *  the variety of POSIX/Berkley socket formats.
                 *
                 *  InternetAddress supports either IPv4 or IPv6 format addresses.
                 *
                 *  InternetAddress objects can be rendered in either network byte order or host byte order, but overwhelmingly
                 *  APIs use network byte order, so that is the default, and the internal representation.
                 *
                 *  Also, for IPv6 addresses, since they can be represented as bytes, or shorts, or longs, its ambiguous what
                 *  host byte order might mean, so no 'host byte order' API is provided for IPv6 addresses: just network byte order.
                 */
                class   InternetAddress {
                public:
                    /**
                     */
                    enum    class  ByteOrder {
                        Network,
                        Host,

                        Stroika_Define_Enum_Bounds(Network, Host)
                        eDEFAULT = Network,
                    };

                public:
                    /**
                     *  This can be V4, V6, or UNKNOWN. The value of this flag is the internet af_family type (e.g. AF_INET).
                     */
                    enum    class  AddressFamily {
                        UNKNOWN = AF_UNSPEC,
                        V4 = AF_INET,
                        V6 = AF_INET6,
                    };

                public:
#if     !qCompilerAndStdLib_constexpr_union_variants_Buggy
                    constexpr
#endif
                    InternetAddress ();
                    /**
                     *  Construct an InternetAddress object from a string (with optionally specified address family).
                     *  If the address is unparsable according to the rules specified, an exception will be thrown.
                     */
                    InternetAddress (const string& s, AddressFamily af = AddressFamily::UNKNOWN);
                    InternetAddress (const String& s, AddressFamily af = AddressFamily::UNKNOWN);
#if     qPlatform_POSIX
                    /**
                     *  Construct an InternetAddress from in_addr_t (v4 ip addr as a long).
                     */
#if     !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                    constexpr
#endif
                    InternetAddress (const in_addr_t& i);
                    InternetAddress (const in_addr_t& i, ByteOrder byteOrder);
#endif
                    /**
                     *  Construct an InternetAddress from in_addr - V4 address.
                     *  Note that provided in_addr must already be in network order.
                     */
#if     !qCompilerAndStdLib_constexpr_union_variants_Buggy
                    constexpr
#endif
                    InternetAddress (const in_addr& i);
                    InternetAddress (const in_addr& i, ByteOrder byteOrder);
                    /**
                     *  Construct an InternetAddress from in6_addr - V6 address.
                     */
#if     !qCompilerAndStdLib_constexpr_union_variants_Buggy
                    constexpr
#endif
                    InternetAddress (const in6_addr& i);

                public:
                    /**
                     *  Check if unspecified internet address.
                     *  @see clear()
                     */
                    nonvirtual  bool    empty () const;

                public:
                    /**
                     *  Make it empty().
                     *  @see empty()
                     */
                    nonvirtual  void    clear ();

                public:
                    /**
                     *  This can be V4, V6, or UNKNOWN, and iff UNKNOWN, then empty () will return true.
                     */
                    nonvirtual  AddressFamily   GetAddressFamily () const;

                public:
                    /**
                     *  As<T> supported variants include:
                     *      As<String> ();
                     *      As<in_addr_t> ();       // qPlatform_POSIX ONLY
                     *      As<in_addr> ();         // GetAddressFamily () == V4 only
                     *      As<in6_addr> ();        // GetAddressFamily () == V6 only
                     *
                     *  Note that returned in_addr, in_addr_t addresses already in network byte order (for the no-arg overload).
                     *
                     *  As<T> (ByteOrder) is only defined for T==in_addr, and then the byte order is determinted by the parameter.
                     */
                    template    <typename T>
                    nonvirtual  T   As () const;
                    template    <typename T>
                    nonvirtual  T   As (ByteOrder byteOrder) const;

                public:
                    /**
                     *  \req not empty ()
                     *
                     *  Return true iff the given address is a localhost IP address (typically 127.0.0.1, but can be anything
                     *  in that class C range, or ::1, for IPv6 or that range).
                     *
                     *  This might be better called 'loopback' address. This is NOT for link-local addresses.
                     */
                    nonvirtual  bool    IsLocalhostAddress () const;

                public:
                    /**
                     *  Returns true iff the address is an 'link local' address. In IPv4 this means 169.254.0.1
                     *  through 169.254.255.254, and in IPv6 these are fe80::/64. These are used for autoconf
                     *  when there is no DHCP server, as in http://tools.ietf.org/html/rfc3927
                     */
                    nonvirtual  bool    IsLinkLocalAddress () const;

                public:
                    /**
                     *  \req not empty ()
                     *  Return true iff the given address is a private IP address (non-routable).
                     */
                    nonvirtual  bool    IsPrivateAddress () const;

                public:
                    /**
                     *  \req not empty ()
                     *  Return true iff the given address is a mutlicast IP address.
                     */
                    nonvirtual  bool    IsMulticastAddress () const;

                public:
                    /**
                     *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
                     *  Treat any in_addr4 < in_addr6 (in other words fAddressFamily_ is high-order of compare).
                     */
                    nonvirtual  int Compare (const InternetAddress& rhs) const;

                public:
                    /**
                     *  Basic operator overloads with the obivous meaning, and simply indirect to @Compare (const InternetAddress& rhs)
                     */
                    nonvirtual  bool    operator< (const InternetAddress& rhs) const;
                    nonvirtual  bool    operator<= (const InternetAddress& rhs) const;
                    nonvirtual  bool    operator> (const InternetAddress& rhs) const;
                    nonvirtual  bool    operator>= (const InternetAddress& rhs) const;
                    nonvirtual  bool    operator== (const InternetAddress& rhs) const;
                    nonvirtual  bool    operator!= (const InternetAddress& rhs) const;

                private:
                    AddressFamily   fAddressFamily_;
                    union   {
                        in_addr         fV4_;
                        in6_addr        fV6_;
                    };
                };


                namespace V4 {
#if     qCompilerAndStdLib_constexpr_union_variants_Buggy
                    extern  const   InternetAddress kAddrAny;
                    extern  const   InternetAddress kLocalhost;
#elif   qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy
                    //we cannot 'forward declare'
#else
                    /**
                     *  Declared const, but defined constexpr
                     */
                    const   InternetAddress kAddrAny;
                    const   InternetAddress kLocalhost;
#endif
                }
                namespace V6 {
#if     qCompilerAndStdLib_constexpr_union_variants_Buggy
                    extern  const   InternetAddress kAddrAny;
                    extern  const   InternetAddress kLocalhost;
#elif   qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy
                    //we cannot 'forward declare'
#else
                    /**
                     *  Declared const, but defined constexpr
                     */
                    const   InternetAddress kAddrAny;
                    const   InternetAddress kLocalhost;
#endif
                }


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "InternetAddress.inl"

#endif  /*_Stroika_Foundation_IO_Network_InternetAddress_h_*/
