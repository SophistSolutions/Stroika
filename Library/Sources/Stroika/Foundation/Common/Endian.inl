/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include <bit>
#include <cstdint>

#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Foundation::Common {

    /*
     ********************************************************************************
     ****************************** Common::GetEndianness ***************************
     ********************************************************************************
     */
#if !qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy
    namespace Private_ {
        union EndianTester_ {
            uint32_t sdat;
            uint8_t  cdat[4];
        };
        static constexpr EndianTester_ kMix_{0x01020304};
    }
#endif
    inline constexpr Endian GetEndianness ()
    {
        if constexpr (endian::native == endian::little) {
            return Endian::eLittle;
        }
        if constexpr (endian::native == endian::big) {
            return Endian::eBig;
        }
#if !qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy
        return (Private_::kMix_.cdat[0] == 4) ? Endian::eLittleByte : // aka little endian
                   (Private_::kMix_.cdat[0] == 1) ? Endian::eBigByte
                                                  : // aka big endian
                   (Private_::kMix_.cdat[0] == 2) ? Endian::eLittleWord
                                                  : // aka little PDP
                   Endian::eBigWord;
#else
#if (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) ||      \
    defined(__BIG_ENDIAN__) || defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__) || defined(_MIBSEB) ||                 \
    defined(__MIBSEB) || defined(__MIBSEB__)
        return Endian::eBigByte;
#endif
#if (defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) || \
    defined(__LITTLE_ENDIAN__) || defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__) || defined(_MIPSEL) ||               \
    defined(__MIPSEL) || defined(__MIPSEL__) || defined(_M_IX86) || defined(_M_X64) || defined(_M_IA64) || defined(_M_ARM)
        return Endian::eLittle;
#endif
#endif
    }

    /*
     ********************************************************************************
     **************************** Common::EndianConverter ***************************
     ********************************************************************************
     */
    template <integral T>
    constexpr inline T EndianConverter (T value, Endian from, Endian to)
    {
        if (from == to) {
            return value;
        }
        Require (from == Endian::eBig or from == Endian::eLittle); // just cuz that's all that's implemented
        Require (to == Endian::eBig or to == Endian::eLittle);     // ""
        // @todo re-implement some cases using https://en.cppreference.com/w/cpp/numeric/byteswap
        // Require ((from == Endian::eBig or from == Endian::eLittle) and (to == Endian::eBig or to == Endian::eLittle));
        // return std::byteswap (value) ;; double check this is right for all integral sizes... and use my stdcompat code for this
        if constexpr (sizeof (T) == 1) {
            return value;
        }
        if constexpr (sizeof (T) == 2) {
            // since we only support big endian and little endian and from != to - swap is easy
            return ((value & 0xFF) << 8) | ((value >> 8) & 0xFF);
        }
        if constexpr (sizeof (T) == 4) {
            // since we only support big endian and little endian and from != to - I THINK This is right...
            return ((value & 0xFF) << 24) | ((value & 0xFF00) << 8) | ((value >> 8) & 0xFF00) | ((value >> 24) & 0xFF);
        }
        AssertNotImplemented ();
        return value;
    }

}
