/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_InternetAddress_h_
#define _Stroika_Foundation_IO_Network_InternetAddress_h_ 1

#include "../../StroikaPreComp.h"

#include <optional>

#if qPlatform_POSIX
#include <arpa/inet.h>
#include <sys/socket.h> // for AF_INET etc
#elif qPlatform_Windows
#include <WinSock2.h>

#include <in6addr.h>
#include <inaddr.h>
#endif

#include "../../Characters/String.h"
#include "../../Configuration/Common.h"
#include "../../Traversal/Iterable.h"

#include "InternetProtocol/IP.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *      @todo   If possible (I think  yes) have method on InternetAddress() to return Scope (like local , global etc).
 *
 *              enum Scope {
 *                  Interface-Local (0x1)
 *                  Link-Local (0x2)
 *                  Admin-Local (0x4)           // MULTICAST ONLY
 *                  Site-Local (0x5)            (AKA - probably better name 'Unique Local Addresses')
 *                  Organization-Local (0x8)    // MULTICAST ONLY
 *                  Global (0xE)
 *              };
 *
 *              Note - there is the Scope, and then the ScoeeID (not sure?)
 *              IPAddress.ScopeId
 *              http://msdn.microsoft.com/en-us/library/system.net.ipaddress.scopeid(v=vs.110).aspx
 *
 *      @todo   Also add IPAddress.IsIPv4MappedToIPv6 Property
 *              http://msdn.microsoft.com/en-us/library/system.net.ipaddress.isipv4mappedtoipv6(v=vs.110).aspx
 *              IPAddress.IsIPv6LinkLocal
 *              http://msdn.microsoft.com/en-us/library/system.net.ipaddress.isipv6linklocal(v=vs.110).aspx?cs-save-lang=1&cs-lang=csharp#code-snippet-1
 *
 *      @todo   Future versions may support converting from IPV4 address to IPV6 by assigning an
 *              IPV4 and saying As<in6_addr> ()? Or maybe have ToIPV6() method?
 */

namespace Stroika::Foundation::IO::Network {

    using Characters::String;
    using std::byte;

    /**
     *  v4 ip addr as a long.
     */
#if qPlatform_POSIX
    using in_addr_t = ::in_addr_t;
#else
    using in_addr_t = uint32_t;
#endif

    /**
     *  InternetAddress repesents an internet IP address. Its easily convertable to/from
     *  the variety of POSIX/Berkley socket formats.
     *
     *  InternetAddress supports either IPv4 or IPv6 format addresses.
     *
     *  InternetAddress objects can be rendered in either network byte order or host byte order,
     *  but overwhelmingly APIs use network byte order, so that is the default, and the internal
     *  representation.
     *
     *  Also, for IPv6 addresses, since they can be represented as bytes, or shorts, or longs,
     *  its ambiguous what host byte order might mean, so no 'host byte order' API is provided
     *  for IPv6 addresses: just network byte order.
     *
     *  \note Site-Local addresses
     *      This class provides no support for site-local addresses because they have been deprecated
     *      in http://www.ietf.org/rfc/rfc3879.txt
     *
     *  \note   See coding conventions document about operator usage: Compare () and operator<, operator>, etc
     */
    class InternetAddress {
    public:
        /**
         *  @see Configuration::Endian
         *  @see Configuration::EndianConverter
         */
        enum class ByteOrder {
            Network,
            Host,

            eDEFAULT = Network,

            Stroika_Define_Enum_Bounds (Network, Host)
        };

    public:
        /**
         *  This can be V4, V6, or UNKNOWN. The value of this flag is the internet af_family type (e.g. AF_INET).
         */
        enum class AddressFamily {
            UNKNOWN = AF_UNSPEC,
            V4      = AF_INET,
            V6      = AF_INET6,
        };

    public:
        /**
         *  A handy way to access the octets of an IPv4 address without worry about endian stuff.
         *
         *  IPv4AddressOctets[0] is always the high-order (most significant) octet.
         *
         *  \note prior to Stroika v2.1d13, this was tuple<uint8_t,uint8_t,uint8_t,uint8_t>;
         */
        using IPv4AddressOctets = array<uint8_t, 4>;

    public:
        /**
         *  A handy way to access the octets of an IPv6 address without worry about endian stuff.
         *
         *  IPv6AddressOctets[0] is always the high-order (most significant) octet.
         */
        using IPv6AddressOctets = array<uint8_t, 16>;

    public:
        /**
         *  InternetAddress (const string& s, AddressFamily af = AddressFamily::UNKNOWN);
         *  InternetAddress (const String& s, AddressFamily af = AddressFamily::UNKNOWN);
         *      Construct an InternetAddress object from a string (with optionally specified address family).
         *      If the address is unparsable according to the rules specified, an exception will be thrown.
         *
         *      Note - this does NOT lookup hostnames (like www.google.com or localhost). It must be a numeric
         *      form of an internet address.
         *
         *  constexpr InternetAddress (const in_addr_t& i);
         *  InternetAddress (const in_addr_t& i, ByteOrder byteOrder);
         *      Construct an InternetAddress from in_addr_t (v4 ip addr as a long).
         *      Note that provided in_addr must already be in network order (unless explicit byte order provided as argument)
         *
         *  constexpr InternetAddress (const in_addr& i);
         *  InternetAddress (const in_addr& i, ByteOrder byteOrder);
         *      Construct an InternetAddress from in_addr - V4 address.
         *      Note that provided in_addr must already be in network order (unless explicit byte order provided as argument)
         *
         *  constexpr  InternetAddress (byte octet1, byte octet2, byte octet3, byte octet4);
         *  constexpr InternetAddress (uint8_t octet1, uint8_t octet2, uint8_t octet3, uint8_t octet4);
         *      Construct an InternetAddress V4 address in A.B.C.D octet form.
         *
         *  constexpr InternetAddress (const in6_addr& i);
         *      Construct an InternetAddress from in6_addr - V6 address.
         *
         *  InternetAddress (Traversal::Iterable<uint8_t> octets, AddressFamily af);
         *  InternetAddress (Traversal::Iterable<byte> octets, AddressFamily af);
         *      Construct an InternetAddress octets, either 4 or 16 in length, and which must agree with provided address-family
         */
        constexpr InternetAddress ();
        InternetAddress (const string& s, AddressFamily af = AddressFamily::UNKNOWN);
        InternetAddress (const String& s, AddressFamily af = AddressFamily::UNKNOWN);
        constexpr InternetAddress (const in_addr_t& i);
        InternetAddress (const in_addr_t& i, ByteOrder byteOrder);
        constexpr InternetAddress (const in_addr& i);
        InternetAddress (const in_addr& i, ByteOrder byteOrder);
        constexpr InternetAddress (byte octet1, byte octet2, byte octet3, byte octet4);
        constexpr InternetAddress (uint8_t octet1, uint8_t octet2, uint8_t octet3, uint8_t octet4);
        [[deprecated ("array<byte,4> works better so this is deprecated since Stroika v2.1d13")]] constexpr InternetAddress (tuple<uint8_t, uint8_t, uint8_t, uint8_t> octets);
        constexpr InternetAddress (array<uint8_t, 4> octets, AddressFamily af = AddressFamily::V4);
        constexpr InternetAddress (array<byte, 4> octets, AddressFamily af = AddressFamily::V4);
        constexpr InternetAddress (const in6_addr& i);
        constexpr InternetAddress (array<uint8_t, 16> octets, AddressFamily af = AddressFamily::V6);
        constexpr InternetAddress (array<byte, 16> octets, AddressFamily af = AddressFamily::V6);
        template <typename ITERABLE_OF_UINT8OrByte, enable_if_t<Configuration::IsIterableOfT_v<ITERABLE_OF_UINT8OrByte, byte> or Configuration::IsIterableOfT_v<ITERABLE_OF_UINT8OrByte, uint8_t>>* = nullptr>
        InternetAddress (ITERABLE_OF_UINT8OrByte octets, AddressFamily af);

    public:
        /**
         *  Check if unspecified internet address.
         *  @see clear()
         */
        nonvirtual constexpr bool empty () const;

    public:
        /**
         *  Make it empty().
         *  @see empty()
         */
        nonvirtual void clear ();

    public:
        /**
         *  This can be V4, V6, or UNKNOWN, and iff UNKNOWN, then empty () will return true.
         */
        nonvirtual constexpr AddressFamily GetAddressFamily () const;

    public:
        /**
         *  Converts - if possible - to the given address family. So for example, to IPv6.
         *
         *  \note - there are many ways to convert from IPv4 to IPv6, and this uses "6to4" - https://en.wikipedia.org/wiki/6to4
         */
        nonvirtual optional<InternetAddress> AsAddressFamily (AddressFamily family) const;

    public:
        /**
         *  The size in bytes of the raw address.
         */
        nonvirtual constexpr optional<size_t> GetAddressSize () const;

    public:
        /**
         *  Only specifically specialized variants are supported. As<T> supported variants include:
         *      As<String> ();
         *      As<in_addr_t> ();                               // qPlatform_POSIX ONLY
         *      As<in_addr> ();                                 // GetAddressFamily () == V4 only
         *      As<array<byte,4>> ();                           // GetAddressFamily () == V4 only
         *      As<array<uint8_t,4>> ();                        // GetAddressFamily () == V4 only
         *      As<IPv4AddressOctets>                           // GetAddressFamily () == V4 only (alias)
         *      As<in6_addr> ();                                // GetAddressFamily () == V6 only
         *      As<array<byte,16>> ();                          // GetAddressFamily () == V6 only
         *      As<array<uint8_t,16>> ();                       // GetAddressFamily () == V6 only
         *      As<IPv6AddressOctets>                           // GetAddressFamily () == V6 only (alias)
         *      As<vector<byte>> ();                            // AddressFamily must be V4 or V6
         *      As<vector<uint8_t>> ();                         // AddressFamily must be V4 or V6
         *      As<vector<bool>> ();                            // AddressFamily must be V4 or V6  (returns 'bits' mapped into array indexes - high order bit of high order byte first)
         *
         *  Note that returned in_addr, in_addr_t addresses already in network byte order (for the no-arg overload).
         *
         *  As<T> (ByteOrder) is only defined for T==in_addr, and then the byte order is determined by the parameter.
         *
         *  As<array<byte,4>> () returns the high order (in this case 'network') byte
         *  in the first part of the array, so
         *      Assert (InternetAddress { 1, 2, 3, 4 }.As<array<byte,4>> ()[0] == 1);
         *
         *  \note   As<String> () will always produce a numerical representation, whereas ToString () - will sometimes produce
         *          a textual shortcut, like "INADDR_ANY".
         */
        template <typename T>
        nonvirtual T As () const;
        template <typename T>
        nonvirtual T As (ByteOrder byteOrder) const;

    public:
        /**
         *  @see Characters::ToString ()
         *
         *  \note   As<String> () will always produce a numerical representation, whereas ToString () - will sometimes produce
         *          a textual shortcut, like "INADDR_ANY".
         */
        nonvirtual String ToString () const;

    public:
        /**
         *  \req not empty ()
         *
         *  Return true iff the given address is a localhost IP address (typically 127.0.0.1, but can be anything
         *  in that class C range, or ::1, for IPv6 or that range).
         *
         *  This might be better called 'loopback' address. This is NOT for link-local addresses.
         */
        nonvirtual bool IsLocalhostAddress () const;

    public:
        /**
         *  Returns true iff the address is an 'link local' address. In IPv4 this means 169.254.0.1
         *  through 169.254.255.254, and in IPv6 these are fe80::/64. These are used for autoconf
         *  when there is no DHCP server, as in http://tools.ietf.org/html/rfc3927
         */
        nonvirtual bool IsLinkLocalAddress () const;

    public:
        /**
         *  \req not empty ()
         *  Return true iff the given address is a private IP address (non-routable).
         *  This is sometimes also called Unique Local Addresses (especially in IPv6).
         */
        nonvirtual bool IsPrivateAddress () const;

    public:
        /**
         *  \req not empty ()
         *  Return true iff the given address is a mutlicast IP address.
         */
        nonvirtual bool IsMulticastAddress () const;

    public:
        /**
         *  Return true iff the InternetAddresses are the same.
         *
         *  This is like Compare() == 0.
         */
        nonvirtual bool Equals (const InternetAddress& rhs) const;

    public:
        /**
         *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
         *  Treat any in_addr4 < in_addr6 (in other words fAddressFamily_ is high-order of compare).
         *  For IPv4 compares, compare logically in the form the ip addr would appear as text.
         *  For IPv6, compare as a byte string.
         */
        nonvirtual int Compare (const InternetAddress& rhs) const;

    private:
        AddressFamily fAddressFamily_;
        union {
            // EACH Stored in network byte order
            in_addr            fV4_;
            in6_addr           fV6_;
            array<uint8_t, 4>  fArray_4_uint_;
            array<byte, 4>     fArray_4_byte_;
            array<uint8_t, 16> fArray_16_uint_;
            array<byte, 16>    fArray_16_byte_;
        };
    };

    /**
     *  operator indirects to BLOB::Compare()
     */
    bool operator< (const InternetAddress& lhs, const InternetAddress& rhs);

    /**
     *  operator indirects to InternetAddress::Compare()
     */
    bool operator<= (const InternetAddress& lhs, const InternetAddress& rhs);

    /**
     *  operator indirects to InternetAddress::Equals()
     */
    bool operator== (const InternetAddress& lhs, const InternetAddress& rhs);

    /**
     *  operator indirects to InternetAddress::Equals()
     */
    bool operator!= (const InternetAddress& lhs, const InternetAddress& rhs);

    /**
     *  operator indirects to InternetAddress::Compare()
     */
    bool operator>= (const InternetAddress& lhs, const InternetAddress& rhs);

    /**
     *  operator indirects to InternetAddress::Compare()
     */
    bool operator> (const InternetAddress& lhs, const InternetAddress& rhs);

    /**
     *  IN_ADDR_ANY
     *      This address is a wildcard, matching any address.
     *
     *  \note   Cannot declare here cuz then declaration and definition differ, and cannot define here because we have
     *          the constexpr CTOR methods in the .inl file.
     *
     *      namespace V4 { constexpr InternetAddress kAddrAny{in_addr{}}; }
     *      namespace V6 { constexpr InternetAddress kAddrAny{in6_addr{}}; }
     */

    /**
     *  LOCALHOST address
     *      this address is for the current machine on its loopback interface.
     *
     *  \note   Cannot declare here cuz then declaration and definition differ, and cannot define here because we have
     *          the constexpr CTOR methods in the .inl file.
     *
     *      namespace V4 { constexpr InternetAddress kLocalhost{0x7f, 0x0, 0x0, 0x1}; }
     *      namespace V6 { constexpr InternetAddress kLocalhost{in6_addr{{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}}}}; }
     */

    /**
     *  v4 localhost address rendered as v6 (mapped).
     *
     *  \note   Cannot declare here cuz then declaration and definition differ, and cannot define here because we have
     *          the constexpr CTOR methods in the .inl file.
     *
     *      namespace V6 { constexpr InternetAddress kV4MappedLocalhost{in6_addr{{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff, 0x7f, 0, 0, 1}}}}; }
     */

    /**
     *  Return kAddrAny - both the IPv4 and IPv6 variants (depending on ipSupport argument) - which defaults to both.
     */
    Traversal::Iterable<InternetAddress> InternetAddresses_Any (InternetProtocol::IP::IPVersionSupport ipSupport = InternetProtocol::IP::IPVersionSupport::eDEFAULT);

    /**
     *  Return kLocalhost - both the IPv4 and IPv6 variants (depending on ipSupport argument) - which defaults to both.
     */
    Traversal::Iterable<InternetAddress> InternetAddresses_Localhost (InternetProtocol::IP::IPVersionSupport ipSupport = InternetProtocol::IP::IPVersionSupport::eDEFAULT);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InternetAddress.inl"

#endif /*_Stroika_Foundation_IO_Network_InternetAddress_h_*/
