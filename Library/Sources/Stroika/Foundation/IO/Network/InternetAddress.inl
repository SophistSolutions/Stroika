/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_InternetAddress_inl_
#define _Stroika_Foundation_IO_Network_InternetAddress_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Configuration/Endian.h"
#include "../../Memory/Bits.h"

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {

                /*
                 ********************************************************************************
                 *********************** IO::Network::InternetAddress ***************************
                 ********************************************************************************
                 */
                inline constexpr InternetAddress::InternetAddress ()
                    : fAddressFamily_ (AddressFamily::UNKNOWN)
                    , fV4_{}
                {
                }
                inline constexpr InternetAddress::InternetAddress (const in_addr_t& i)
                    : fAddressFamily_ (AddressFamily::V4)
                    , fV4_
                {
#if qPlatform_POSIX
                    i
#elif qPlatform_Windows
                    in_addr
                    {
                        {
                            static_cast<uint8_t> (Memory::BitSubstring (i, 0, 8)),
                                static_cast<uint8_t> (Memory::BitSubstring (i, 8, 16)),
                                static_cast<uint8_t> (Memory::BitSubstring (i, 16, 24)),
                                static_cast<uint8_t> (Memory::BitSubstring (i, 24, 32))
                        }
                    }
#endif
                }
                {
#if qPlatform_Windows && !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                    Assert (fV4_.s_addr == i);
#endif
                }
                inline InternetAddress::InternetAddress (const in_addr_t& i, ByteOrder byteOrder)
                    : fAddressFamily_ (AddressFamily::V4)
                    , fV4_
                {
#if qPlatform_POSIX
                    i
#endif
                }
                {
#if qPlatform_Windows
                    fV4_.s_addr = i;
#endif
                    if (byteOrder == ByteOrder::Host) {
                        fV4_.s_addr = htonl (fV4_.s_addr); //NB no ':' cuz some systems use macro
                    }
                }
                inline constexpr InternetAddress::InternetAddress (const in_addr& i)
                    : fAddressFamily_ (AddressFamily::V4)
                    , fV4_ (i)
                {
                }
                inline InternetAddress::InternetAddress (const in_addr& i, ByteOrder byteOrder)
                    : fAddressFamily_ (AddressFamily::V4)
                    , fV4_ (i)
                {
                    if (byteOrder == ByteOrder::Host) {
                        fV4_.s_addr = htonl (fV4_.s_addr); //NB no ':' cuz some systems use macro
                    }
                }
                inline
#if __cpp_designators
                    constexpr
#endif
                    InternetAddress::InternetAddress (uint8_t octet1, uint8_t octet2, uint8_t octet3, uint8_t octet4)
                    : fAddressFamily_ (AddressFamily::V4)
#if qPlatform_POSIX
                    , fV4_
                {
                    Configuration::GetEndianness () == Configuration::Endian::eLittleByte ? static_cast<uint32_t> (octet1 + (octet2 << 8) + (octet3 << 16) + (octet4 << 24)) : static_cast<uint32_t> (octet4 + (octet3 << 8) + (octet2 << 16) + (octet1 << 24))
                }
#elif qPlatform_Windows && __cpp_designators
                    , fV4_ ({{.S_addr = Configuration::GetEndianness () == Configuration::Endian::eLittleByte ? static_cast<uint32_t> (octet1 + (octet2 << 8) + (octet3 << 16) + (octet4 << 24)) : static_cast<uint32_t> (octet4 + (octet3 << 8) + (octet2 << 16) + (octet1 << 24))}})
#endif
                {
// @todo - only these cases supported, and cannot static_assert cuz too many compiler bugs.... Soon... --LGP 2015-08-26
// static_assert (Configuration::GetEndianness () == Configuration::Endian::eLittle or Configuration::GetEndianness () == Configuration::Endian::eBig, "NYI rare endian");

//  a.b.c.d: Each of the four numeric parts specifies a byte of the address
// in left-to-right order. Network order is big-endian
#if qPlatform_Windows && !__cpp_designators
                    fV4_.s_addr = Configuration::GetEndianness () == Configuration::Endian::eLittleByte ? static_cast<uint32_t> (octet1 + (octet2 << 8) + (octet3 << 16) + (octet4 << 24)) : static_cast<uint32_t> (octet4 + (octet3 << 8) + (octet2 << 16) + (octet1 << 24));
#endif
                }
                inline
#if __cpp_designators
                    constexpr
#endif
                    InternetAddress::InternetAddress (IPv4AddressOctets octets)
                    : InternetAddress (get<0> (octets), get<1> (octets), get<2> (octets), get<3> (octets))
                {
                }
                inline constexpr InternetAddress::InternetAddress (const in6_addr& i)
                    : fAddressFamily_ (AddressFamily::V6)
                    , fV6_ (i)
                {
                }
                inline constexpr bool InternetAddress::empty () const
                {
                    return fAddressFamily_ == AddressFamily::UNKNOWN;
                }
                inline void InternetAddress::clear ()
                {
                    fAddressFamily_ = AddressFamily::UNKNOWN;
                }
                inline constexpr InternetAddress::AddressFamily InternetAddress::GetAddressFamily () const
                {
                    return fAddressFamily_;
                }
                inline Memory::Optional<size_t> InternetAddress::GetAddressSize () const
                {
                    switch (this->GetAddressFamily ()) {
                        case AddressFamily::V4:
                            return 4;
                        case AddressFamily::V6:
                            return 32;
                        default:
                            return {};
                    }
                }
                template <>
                String InternetAddress::As<String> () const;
#if qPlatform_POSIX
                template <>
                inline in_addr_t InternetAddress::As<in_addr_t> () const
                {
                    Require (fAddressFamily_ == AddressFamily::V4);
                    return fV4_.s_addr;
                }
#endif
                template <>
                inline constexpr in_addr InternetAddress::As<in_addr> () const
                {
#if !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                    Require (fAddressFamily_ == AddressFamily::V4);
#endif
                    return fV4_;
                }
                template <>
                inline tuple<uint8_t, uint8_t, uint8_t, uint8_t> InternetAddress::As<tuple<uint8_t, uint8_t, uint8_t, uint8_t>> () const
                {
                    Require (fAddressFamily_ == AddressFamily::V4);
                    switch (Configuration::GetEndianness ()) {
                        case Configuration::Endian::eLittleByte:
                            return make_tuple<uint8_t, uint8_t, uint8_t, uint8_t> (
                                static_cast<uint8_t> (Memory::BitSubstring (fV4_.s_addr, 0, 8)),
                                static_cast<uint8_t> (Memory::BitSubstring (fV4_.s_addr, 8, 16)),
                                static_cast<uint8_t> (Memory::BitSubstring (fV4_.s_addr, 16, 24)),
                                static_cast<uint8_t> (Memory::BitSubstring (fV4_.s_addr, 24, 32)));
                        case Configuration::Endian::eBigByte:
                            return make_tuple<uint8_t, uint8_t, uint8_t, uint8_t> (
                                static_cast<uint8_t> (Memory::BitSubstring (fV4_.s_addr, 24, 32)),
                                static_cast<uint8_t> (Memory::BitSubstring (fV4_.s_addr, 16, 24)),
                                static_cast<uint8_t> (Memory::BitSubstring (fV4_.s_addr, 8, 16)),
                                static_cast<uint8_t> (Memory::BitSubstring (fV4_.s_addr, 0, 8)));
                        default:
                            AssertNotImplemented ();
                            return tuple<uint8_t, uint8_t, uint8_t, uint8_t>{};
                    }
                }
                template <>
                inline constexpr in6_addr InternetAddress::As<in6_addr> () const
                {
#if !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                    Require (fAddressFamily_ == AddressFamily::V6);
#endif
                    return fV6_;
                }
                template <>
                inline in_addr InternetAddress::As<in_addr> (ByteOrder byteOrder) const
                {
                    Require (fAddressFamily_ == AddressFamily::V4);
                    if (byteOrder == ByteOrder::Network) {
                        return fV4_;
                    }
                    else {
                        in_addr tmp = fV4_;
                        tmp.s_addr  = ntohl (tmp.s_addr);
                        return tmp;
                    }
                }

                /*
                 ********************************************************************************
                 ************************* InternetAddress operators ****************************
                 ********************************************************************************
                 */
                inline bool operator< (const InternetAddress& lhs, const InternetAddress& rhs)
                {
                    return lhs.Compare (rhs) < 0;
                }
                inline bool operator<= (const InternetAddress& lhs, const InternetAddress& rhs)
                {
                    return lhs.Compare (rhs) <= 0;
                }
                inline bool operator== (const InternetAddress& lhs, const InternetAddress& rhs)
                {
                    return lhs.Equals (rhs);
                }
                inline bool operator!= (const InternetAddress& lhs, const InternetAddress& rhs)
                {
                    return not lhs.Equals (rhs);
                }
                inline bool operator>= (const InternetAddress& lhs, const InternetAddress& rhs)
                {
                    return lhs.Compare (rhs) >= 0;
                }
                inline bool operator> (const InternetAddress& lhs, const InternetAddress& rhs)
                {
                    return lhs.Compare (rhs) > 0;
                }

#if !qCompilerAndStdLib_constexpr_union_variants_Buggy
                namespace V4 {
                    constexpr InternetAddress kAddrAny{in_addr{}};
                    constexpr InternetAddress kLocalhost{0x7f, 0x0, 0x0, 0x1};
                }
                namespace V6 {
                    constexpr InternetAddress kAddrAny{in6_addr{}};
                    constexpr InternetAddress kLocalhost{in6_addr IN6ADDR_LOOPBACK_INIT};
                    constexpr InternetAddress kV4MappedLocalhost{in6_addr{{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff, 0x7f, 0, 0, 1}}}};
                }
#endif
            }
        }
    }
}
#endif /*_Stroika_Foundation_IO_Network_InternetAddress_inl_*/
