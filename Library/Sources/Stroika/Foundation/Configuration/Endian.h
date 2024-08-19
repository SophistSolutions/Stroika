/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Endian_h_
#define _Stroika_Foundation_Configuration_Endian_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <concepts>
#include <cstdint>

#include "Stroika/Foundation/Configuration/Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 *
 * TODO:
 *      @todo   Use new stdc++17 endian feature - http://stroika-bugs.sophists.com/browse/STK-850
 */

namespace Stroika::Foundation::Configuration {

    /**
     *  @see http://en.wikipedia.org/wiki/Endianness
     * 
     *  @todo DOCUMENT CONNECTION TO https://en.cppreference.com/w/cpp/types/endian
     */
    enum class Endian {
        eBigByte, // byte-swapped big-endian
        eBigWord, // word-swapped big-endian

        eLittleByte, // byte-swapped little-endian  (e.g. x86)
        eLittleWord, // word-swapped little-endian

        eBig    = eBigByte,
        eLittle = eLittleByte, // e.g. x86
        ePDP    = eLittleWord,

        eX86 = eLittle // so common, worth an alias
    };

    /**
     * \brief returns Endianness flag. Can be complicated, mixed, etc. But often very simple and for the
     *        simple cases, based on std::endian::native.
     * 
     *   if (endian::native == endian::little) {
     *       return Endian::eLittle;
     *   }
     *   else if (endian::native == endian::big) {
     *       return Endian::eBig;
     *   }
     *   else complicated...
     */
    constexpr Endian GetEndianness ();

    /**
    * @todo redo with byteswap
     *  Utility to convert endianness. Logically this can be defined on any numeric
     *  integer type, but for now is restricted to uint16_t, uint32_t;
     * 
     *  \par Example Usage
     *      \code
     *          EXPECT_EQ (EndianConverter<uint16_t> (0xAABB, Endian::eBig, Endian::eLittle), 0xBBAA);
     *          uint16_t useThisNumber = EndianConverter<uint16_t> (0xAABB, Endian::eBig, GetEndianness ());
     *      \endcode
     */
    template <integral T>
    constexpr T EndianConverter (T value, Endian from, Endian to);
#if 0
    template <>
    constexpr uint16_t EndianConverter (uint16_t value, Endian from, Endian to);
    template <>
    constexpr uint32_t EndianConverter (uint32_t value, Endian from, Endian to);
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Endian.inl"

#endif /*_Stroika_Foundation_Configuration_Endian_h_*/
