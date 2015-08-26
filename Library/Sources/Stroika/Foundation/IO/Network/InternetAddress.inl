/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_InternetAddress_inl_
#define _Stroika_Foundation_IO_Network_InternetAddress_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Configuration/Endian.h"
#include    "../../Memory/Bits.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {


                /*
                 ********************************************************************************
                 *********************** IO::Network::InternetAddress ***************************
                 ********************************************************************************
                 */
                inline
#if     !qCompilerAndStdLib_constexpr_union_variants_Buggy
                constexpr
#endif
                InternetAddress::InternetAddress ()
                    : fAddressFamily_ (AddressFamily::UNKNOWN)
                    , fV4_ {} {
                }
#if     qPlatform_POSIX
                inline
#if     !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                constexpr
#endif
                InternetAddress::InternetAddress (const in_addr_t& i)
                    : fAddressFamily_ (AddressFamily::V4)
                    , fV4_ { i } {
                }
                inline  InternetAddress::InternetAddress (const in_addr_t& i, ByteOrder byteOrder)
                    : fAddressFamily_ (AddressFamily::V4)
                    , fV4_ { i } {
                    if (byteOrder == ByteOrder::Host)
                    {
                        fV4_.s_addr = htonl (fV4_.s_addr);      //NB no ':' cuz some systems use macro
                    }
                }
#endif
                inline
#if     !qCompilerAndStdLib_constexpr_union_variants_Buggy
                constexpr
#endif
                InternetAddress::InternetAddress (const in_addr& i)
                    : fAddressFamily_ (AddressFamily::V4)
                    , fV4_ (i)
                {
                }
                inline InternetAddress::InternetAddress (const in_addr& i, ByteOrder byteOrder)
                    : fAddressFamily_ (AddressFamily::V4)
                    , fV4_ (i)
                {
                    if (byteOrder == ByteOrder::Host) {
                        fV4_.s_addr = htonl (fV4_.s_addr);      //NB no ':' cuz some systems use macro
                    }
                }
                inline
#if     !qCompilerAndStdLib_union_designators_Buggy && !qCompilerAndStdLib_constexpr_union_variants_Buggy
                constexpr
#endif
                InternetAddress::InternetAddress (uint8_t octet1, uint8_t octet2, uint8_t octet3, uint8_t octet4)
                    : fAddressFamily_ (AddressFamily::V4)
#if     qPlatform_POSIX
                    , fV4_ { Configuration::GetEndianness () == Configuration::Endian::eLittleByte ? static_cast<uint32_t> (octet1 + (octet2 << 8) + (octet3 << 16) + (octet4 << 24)) : static_cast<uint32_t> (octet4 + (octet3 << 8) + (octet2 << 16) + (octet1 << 24)) }
#elif   qPlatform_Windows && !qCompilerAndStdLib_union_designators_Buggy
                    , fV4_ ({{.S_addr = Configuration::GetEndianness () == Configuration::Endian::eLittleByte ? static_cast<uint32_t> (octet1 + (octet2 << 8) + (octet3 << 16) + (octet4 << 24)) : static_cast<uint32_t> (octet4 + (octet3 << 8) + (octet2 << 16) + (octet1 << 24))}})
#endif
                {
                    // @todo - only these cases supported, and cannot static_assert cuz too many compiler bugs.... Soon... --LGP 2015-08-26
                    // static_assert (Configuration::GetEndianness () == Configuration::Endian::eLittle or Configuration::GetEndianness () == Configuration::Endian::eBig, "NYI rare endian");

                    //  a.b.c.d: Each of the four numeric parts specifies a byte of the address
                    // in left-to-right order. Network order is big-endian
#if     qPlatform_Windows && qCompilerAndStdLib_union_designators_Buggy
                    fV4_.s_addr = Configuration::GetEndianness () == Configuration::Endian::eLittleByte ? static_cast<uint32_t> (octet1 + (octet2 << 8) + (octet3 << 16) + (octet4 << 24)) : static_cast<uint32_t> (octet4 + (octet3 << 8) + (octet2 << 16) + (octet1 << 24));
#endif
                }
                inline
#if     !qCompilerAndStdLib_union_designators_Buggy && !qCompilerAndStdLib_constexpr_union_variants_Buggy
                constexpr
#endif
                InternetAddress::InternetAddress (IPv4AddressOctets octets)
                    : InternetAddress (get<0> (octets), get<1> (octets), get<2> (octets), get<3> (octets))
                {
                }
                inline
#if     !qCompilerAndStdLib_constexpr_union_variants_Buggy
                constexpr
#endif
                InternetAddress::InternetAddress (const in6_addr& i)
                    : fAddressFamily_ (AddressFamily::V6)
                    , fV6_ (i)
                {
                }
                inline
#if     !qCompilerAndStdLib_constexpr_union_variants_Buggy
                constexpr
#endif
                bool    InternetAddress::empty () const
                {
                    return fAddressFamily_ == AddressFamily::UNKNOWN;
                }
                inline  void    InternetAddress::clear ()
                {
                    fAddressFamily_ = AddressFamily::UNKNOWN;
                }
                inline
#if     !qCompilerAndStdLib_constexpr_union_variants_Buggy
                constexpr
#endif
                InternetAddress::AddressFamily  InternetAddress::GetAddressFamily () const
                {
                    return fAddressFamily_;
                }
                template    <>
                String InternetAddress::As<String> () const;
                template    <typename T>
                T   InternetAddress::As () const
                {
#if     qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy
                    RequireNotReached ();
#else
                    static_assert (false, "Only specifically specialized variants are supported");
#endif
                }
#if     qPlatform_POSIX
                template    <>
                inline  in_addr_t   InternetAddress::As<in_addr_t> () const
                {
                    Require (fAddressFamily_ == AddressFamily::V4);
                    return fV4_.s_addr;
                }
#endif
                template    <>
                inline
#if     !qCompilerAndStdLib_constexpr_union_variants_Buggy && !qCompilerAndStdLib_constexpr_after_template_decl_constexpr_Buggy
                constexpr
#endif
                in_addr InternetAddress::As<in_addr> () const
                {
#if     !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                    Require (fAddressFamily_ == AddressFamily::V4);
#endif
                    return fV4_;
                }
                template    <>
                inline  tuple<uint8_t, uint8_t, uint8_t, uint8_t> InternetAddress::As<tuple<uint8_t, uint8_t, uint8_t, uint8_t>> () const
                {
                    Require (fAddressFamily_ == AddressFamily::V4);
                    return make_tuple<uint8_t, uint8_t, uint8_t, uint8_t> (
                               static_cast<uint8_t> (Memory::BitSubstring (fV4_.s_addr, 0, 8)),
                               static_cast<uint8_t> (Memory::BitSubstring (fV4_.s_addr, 8, 8)),
                               static_cast<uint8_t> (Memory::BitSubstring (fV4_.s_addr, 16, 8)),
                               static_cast<uint8_t> (Memory::BitSubstring (fV4_.s_addr, 24, 8))
                           );
                }
                template    <>
                inline
#if     !qCompilerAndStdLib_constexpr_union_variants_Buggy && !qCompilerAndStdLib_constexpr_after_template_decl_constexpr_Buggy
                constexpr
#endif
                in6_addr    InternetAddress::As<in6_addr> () const
                {
#if     !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                    Require (fAddressFamily_ == AddressFamily::V6);
#endif
                    return fV6_;
                }
                template    <>
                inline  in_addr InternetAddress::As<in_addr> (ByteOrder byteOrder) const
                {
                    Require (fAddressFamily_ == AddressFamily::V4);
                    if (byteOrder == ByteOrder::Network) {
                        return fV4_;
                    }
                    else {
                        in_addr tmp =   fV4_;
                        tmp.s_addr = ntohl (tmp.s_addr);
                        return tmp;
                    }
                }
                template    <typename T>
                T   InternetAddress::As (ByteOrder byteOrder) const
                {
#if     qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy
                    RequireNotReached ();
#else
                    static_assert (false, "Only specifically specialized variants are supported");
#endif
                }


                /*
                 ********************************************************************************
                 ************************* InternetAddress operators ****************************
                 ********************************************************************************
                 */
                inline  bool    operator< (const InternetAddress& lhs, const InternetAddress& rhs)
                {
                    return lhs.Compare (rhs) < 0;
                }
                inline  bool    operator<= (const InternetAddress& lhs, const InternetAddress& rhs)
                {
                    return lhs.Compare (rhs) <= 0;
                }
                inline  bool    operator== (const InternetAddress& lhs, const InternetAddress& rhs)
                {
                    return lhs.Equals (rhs);
                }
                inline  bool    operator!= (const InternetAddress& lhs, const InternetAddress& rhs)
                {
                    return not lhs.Equals (rhs);
                }
                inline  bool    operator>= (const InternetAddress& lhs, const InternetAddress& rhs)
                {
                    return lhs.Compare (rhs) >= 0;
                }
                inline  bool    operator> (const InternetAddress& lhs, const InternetAddress& rhs)
                {
                    return lhs.Compare (rhs) > 0;
                }


#if     !qCompilerAndStdLib_constexpr_union_variants_Buggy
                namespace V4 {
                    constexpr   InternetAddress kAddrAny    { in_addr { } };
                    constexpr   InternetAddress kLocalhost  { 0x7f, 0x0, 0x0, 0x1 };
                }
                namespace V6 {
                    constexpr   InternetAddress kAddrAny    { in6_addr { } };
                    constexpr   InternetAddress kLocalhost  { in6_addr IN6ADDR_LOOPBACK_INIT };
                }
#endif


            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_Network_InternetAddress_inl_*/
