/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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
#include "../../Memory/Common.h"

namespace Stroika::Foundation::IO::Network {

    /*
     ********************************************************************************
     *********************** IO::Network::InternetAddress ***************************
     ********************************************************************************
     */
    constexpr InternetAddress::InternetAddress ()
        : fAddressFamily_{AddressFamily::UNKNOWN}
        , fV4_{}
    {
    }
    constexpr InternetAddress::InternetAddress (const in_addr_t& i)
        : fAddressFamily_ (AddressFamily::V4)
#if qPlatform_POSIX
        , fV4_
    {
        i
    }
#elif qPlatform_Windows
        , fV4_
    {
        in_addr
        {
            {
                static_cast<uint8_t> (Memory::BitSubstring (i, 0, 8)),
                    static_cast<uint8_t> (Memory::BitSubstring (i, 8, 16)),
                    static_cast<uint8_t> (Memory::BitSubstring (i, 16, 24)),
                    static_cast<uint8_t> (Memory::BitSubstring (i, 24, 32))
            }
        }
    }
#endif
    {
#if qPlatform_Windows
        Assert (fV4_.s_addr == i);
#endif
    }
    inline InternetAddress::InternetAddress (const in_addr_t& i, ByteOrder byteOrder)
        : fAddressFamily_ (AddressFamily::V4)
#if qPlatform_POSIX
        , fV4_
    {
        i
    }
#endif
    {
#if qPlatform_Windows
        fV4_.s_addr = i;
#endif
        if (byteOrder == ByteOrder::Host) {
            fV4_.s_addr = htonl (fV4_.s_addr); //NB no ':' cuz some systems use macro
        }
    }
    inline constexpr InternetAddress::InternetAddress (const in_addr& i)
        : fAddressFamily_{AddressFamily::V4}
        , fV4_{i}
    {
    }
    inline InternetAddress::InternetAddress (const in_addr& i, ByteOrder byteOrder)
        : fAddressFamily_{AddressFamily::V4}
        , fV4_{i}
    {
        if (byteOrder == ByteOrder::Host) {
            fV4_.s_addr = htonl (fV4_.s_addr); //NB no ':' cuz some systems use macro
        }
    }
    constexpr InternetAddress::InternetAddress (byte octet1, byte octet2, byte octet3, byte octet4)
        : InternetAddress (array<byte, 4>{octet1, octet2, octet3, octet4})
    {
    }
    constexpr InternetAddress::InternetAddress (uint8_t octet1, uint8_t octet2, uint8_t octet3, uint8_t octet4)
        : InternetAddress (array<uint8_t, 4>{octet1, octet2, octet3, octet4})
    {
    }
    constexpr InternetAddress::InternetAddress (array<uint8_t, 4> octets, AddressFamily af)
        : fAddressFamily_{af}
        , fArray_4_uint_{octets}
    {
    }
    constexpr InternetAddress::InternetAddress (array<byte, 4> octets, AddressFamily af)
        : fAddressFamily_{af}
        , fArray_4_byte_{octets}
    {
    }
    constexpr InternetAddress::InternetAddress (const in6_addr& i)
        : fAddressFamily_ (AddressFamily::V6)
        , fV6_{i}
    {
    }
    constexpr InternetAddress::InternetAddress (array<uint8_t, 16> octets, AddressFamily af)
        : fAddressFamily_{af}
        , fArray_16_uint_{octets}
    {
    }
    constexpr InternetAddress::InternetAddress (array<byte, 16> octets, AddressFamily af)
        : fAddressFamily_{af}
        , fArray_16_byte_{octets}
    {
    }
    template <typename ITERABLE_OF_UINT8OrByte, enable_if_t<Configuration::IsIterableOfT_v<ITERABLE_OF_UINT8OrByte, byte> or Configuration::IsIterableOfT_v<ITERABLE_OF_UINT8OrByte, uint8_t>>*>
    inline InternetAddress::InternetAddress (ITERABLE_OF_UINT8OrByte octets, AddressFamily af)
        : fAddressFamily_{af}
    {
        Require (af != AddressFamily::V4 or octets.size () == 4);
        Require (af != AddressFamily::V6 or octets.size () == 16);
        size_t i = 0;
        for (auto b : octets) {
            fArray_16_uint_[i++] = static_cast<uint8_t> (b);
        }
    }
    constexpr bool InternetAddress::empty () const
    {
        return fAddressFamily_ == AddressFamily::UNKNOWN;
    }
    inline void InternetAddress::clear ()
    {
        fAddressFamily_ = AddressFamily::UNKNOWN;
    }
    constexpr InternetAddress::AddressFamily InternetAddress::GetAddressFamily () const
    {
        return fAddressFamily_;
    }
    constexpr optional<size_t> InternetAddress::GetAddressSize () const
    {
        switch (GetAddressFamily ()) {
            case AddressFamily::V4:
                return 4;
            case AddressFamily::V6:
                return 16;
            default:
                return nullopt;
        }
    }
    constexpr InternetAddress InternetAddress::min ()
    {
        return InternetAddress{};
    }
    constexpr InternetAddress InternetAddress::max ()
    {
        // @todo - consider if this is really the max (could have longer address at some point in the future - IPV8, 9, 10??)
        return InternetAddress{array<uint8_t, 16>{
            0xff,
            0xff,
            0xff,
            0xff,
            0xff,
            0xff,
            0xff,
            0xff,
            0xff,
            0xff,
            0xff,
            0xff,
            0xff,
            0xff,
            0xff,
            0xff,
        }};
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
    constexpr in_addr InternetAddress::As<in_addr> () const
    {
        Require (fAddressFamily_ == AddressFamily::V4);
        return fV4_;
    }
    template <>
    constexpr array<uint8_t, 4> InternetAddress::As<array<uint8_t, 4>> () const
    {
        Require (GetAddressSize () == 4u);
        return fArray_4_uint_;
    }
    template <>
    constexpr array<byte, 4> InternetAddress::As<array<byte, 4>> () const
    {
        Require (GetAddressSize () == 4u);
        return fArray_4_byte_;
    }
    template <>
    constexpr array<uint8_t, 16> InternetAddress::As<array<uint8_t, 16>> () const
    {
        Require (GetAddressSize () == 16u);
        return fArray_16_uint_;
    }
    template <>
    constexpr array<byte, 16> InternetAddress::As<array<byte, 16>> () const
    {
        Require (GetAddressSize () == 16u);
        return fArray_16_byte_;
    }
    template <>
    inline vector<byte> InternetAddress::As<vector<byte>> () const
    {
        Require (GetAddressSize ().has_value ());
        Assert (*GetAddressSize () <= 16);
        return vector<byte>{fArray_16_byte_.begin (), fArray_16_byte_.begin () + *GetAddressSize ()};
    }
    template <>
    inline vector<uint8_t> InternetAddress::As<vector<uint8_t>> () const
    {
        Require (GetAddressSize ().has_value ());
        Assert (*GetAddressSize () <= 16);
        return vector<uint8_t>{fArray_16_uint_.begin (), fArray_16_uint_.begin () + *GetAddressSize ()};
    }
    template <>
    constexpr in6_addr InternetAddress::As<in6_addr> () const
    {
        Require (fAddressFamily_ == AddressFamily::V6);
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
     ********************** InternetAddress::ThreeWayComparer ***********************
     ********************************************************************************
     */
    constexpr int InternetAddress::ThreeWayComparer::operator() (const InternetAddress& lhs, const InternetAddress& rhs) const
    {
        if (int cmp = Common::ThreeWayCompare (lhs.fAddressFamily_, rhs.fAddressFamily_)) {
            return cmp;
        }
        switch (lhs.fAddressFamily_) {
            case AddressFamily::UNKNOWN: {
                return 0;
            } break;
            case AddressFamily::V4: {
                return Memory::MemCmp (&*lhs.fArray_4_uint_.begin (), &*rhs.fArray_4_uint_.begin (), 4);
            } break;
            case AddressFamily::V6: {
                return Memory::MemCmp (&*lhs.fArray_16_uint_.begin (), &*rhs.fArray_16_uint_.begin (), 16);
            } break;
        }
        //AssertNotReached ();  @todo - this really should be an assertion failure, but tricky cuz constexpr function could fix with template)
        return 0;
    }

    /*
     ********************************************************************************
     ************************* InternetAddress operators ****************************
     ********************************************************************************
     */
    inline bool operator< (const InternetAddress& lhs, const InternetAddress& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) < 0;
    }
    inline bool operator<= (const InternetAddress& lhs, const InternetAddress& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) <= 0;
    }
    inline bool operator== (const InternetAddress& lhs, const InternetAddress& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) == 0;
    }
    inline bool operator!= (const InternetAddress& lhs, const InternetAddress& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) != 0;
    }
    inline bool operator>= (const InternetAddress& lhs, const InternetAddress& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) >= 0;
    }
    inline bool operator> (const InternetAddress& lhs, const InternetAddress& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) > 0;
    }

    namespace V4 {
        constexpr InternetAddress kAddrAny{in_addr{}};
    }
    namespace V6 {
        constexpr InternetAddress kAddrAny{in6_addr{}};
    }
    namespace V4 {
        constexpr InternetAddress kLocalhost{0x7f, 0x0, 0x0, 0x1};
    }
    namespace V6 {
        constexpr InternetAddress kLocalhost{in6_addr{{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}}}};
    }
    namespace V6 {
        constexpr InternetAddress kV4MappedLocalhost{in6_addr{{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff, 0x7f, 0, 0, 1}}}};
    }

}

#endif /*_Stroika_Foundation_IO_Network_InternetAddress_inl_*/
