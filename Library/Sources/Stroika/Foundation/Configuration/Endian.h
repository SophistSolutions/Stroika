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
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Configuration {

    /**
     *  \brief in principle complicated question of correspondence between bit and byte and word numbering, but often fairly simple - similar to but subtler version of std::endian
     * 
     *  @see http://en.wikipedia.org/wiki/Endianness
     *  @see https://en.cppreference.com/w/cpp/types/endian
     * 
     *  \note cannot use values from std::endian for matching cases (eBig/eLittle) because these values are impl defined
     */
    enum class Endian {
        eBigByte, // byte-swapped big-endian
        eBigWord, // word-swapped big-endian

        eLittleByte, // byte-swapped little-endian  (e.g. x86)
        eLittleWord, // word-swapped little-endian

        eBig    = eBigByte,    // (std::endian::big)
        eLittle = eLittleByte, // e.g. x86  (std::endian::little)
        ePDP    = eLittleWord,

        eX86 = eLittle, // so common, worth an alias
        eARM = eLittle  // ""
    };

    /**
     * \brief returns native (std::endian::native) Endianness flag. Can be complicated (mixed, etc). But often very simple (e.g. Endian::eLittle) and for the
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
     *  Utility to convert endianness. Logically this can be defined on any numeric
     *  integer type, but for now is restricted to uint16_t, uint32_t;
     * 
     *  \par Example Usage
     *      \code
     *          EXPECT_EQ (EndianConverter<uint16_t> (0xAABB, Endian::eBig, Endian::eLittle), 0xBBAA);
     *          uint16_t useThisNumber = EndianConverter<uint16_t> (0xAABB, Endian::eBig, GetEndianness ());
     *      \endcode
     * 
     *  @see https://en.cppreference.com/w/cpp/numeric/byteswap
     */
    template <integral T>
    constexpr T EndianConverter (T value, Endian from, Endian to);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Endian.inl"

#endif /*_Stroika_Foundation_Configuration_Endian_h_*/
