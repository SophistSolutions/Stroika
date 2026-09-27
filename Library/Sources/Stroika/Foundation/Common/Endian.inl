/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include <array>
#include <bit>
#include <cstdint>

#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Foundation::Common {

    /*
     ********************************************************************************
     ****************************** Common::GetEndianness ***************************
     ********************************************************************************
     */
    inline constexpr Endian GetEndianness ()
    {
        // The byte order of a known value. bit_cast may look at an object's bytes in a constant expression; reading a
        // union member other than the one last written may not (in any C++ version). std::endian alone cannot tell the
        // two word-swapped orders apart.
        constexpr auto kBytes_ = bit_cast<array<uint8_t, 4>> (uint32_t{0x01020304});
        return kBytes_[0] == 4   ? Endian::eLittleByte
               : kBytes_[0] == 1 ? Endian::eBigByte
               : kBytes_[0] == 2 ? Endian::eLittleWord
                                 : Endian::eBigWord;
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
