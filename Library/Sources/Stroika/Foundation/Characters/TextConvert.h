/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_TextConvert_h_
#define _Stroika_Foundation_Characters_TextConvert_h_ 1

#include "../StroikaPreComp.h"

#include <bit>
#include <optional>
#include <span>

#include "../Configuration/Endian.h"

#include "CodeCvt.h"
#include "UTFConvert.h"

/**
 *  \file
 *      Wrappers and extensions to the CodeCvt/std::codecvt<> logic. Often when reading/writing files (or a series of bytes)
 *      you have no, or limited knowlege of the code page. This contains logic to help with that case.
 * 
 *  @todo - this will replace the (probably to be deprecated) CodePage module - at least much/most of it.
 */

namespace Stroika::Foundation::Characters {

    using namespace std;

    /**
     *  \brief list of external UNICODE character encodings, for file IO (eDefault = eUTF8)
     */
    enum class UnicodeExternalEncodings {
        eUTF7,
        eUTF8,
        eUTF16_BE,
        eUTF16_LE,
        eUTF16 = std::endian::native == std::endian::big ? eUTF16_BE : eUTF16_LE,
        eUTF32_BE,
        eUTF32_LE,
        eUTF32 = std::endian::native == std::endian::big ? eUTF32_BE : eUTF32_LE,

        eDefault = eUTF8,
    };

    /**
     *  returns the byte order mark for the given unicode encoding.
     */
    constexpr span<const byte> GetByteOrderMark (UnicodeExternalEncodings e) noexcept;

    /**
     *  returns guessed encoding, and number of bytes consumed. If 'd' doesn't contain
     *  BOM (possible cuz not large enuf) - returns nullopt
     */
    constexpr optional<tuple<UnicodeExternalEncodings, size_t>> ReadByteOrderMark (span<const byte> d) noexcept;

    /**
     *  \req into.size () >= SizeOfByteOrderMark (e)
     * 
     *  returns remaining span to write into (basically just into.subspan(SizeOfByteOrderMark (e))
     *  so caller can continue writing
     */
    span<byte> WriteByteOrderMark (UnicodeExternalEncodings e, span<byte> into);

    /**
     *  Construct CodeCvt (codecvt<> like object) to allow converting of UNICODE CHAR_T to/from bytes, either taking argument UNICODE
     *  encoding, or a locale (if not specified, the current locale).
     *
     *  One overload takes a UnicodeExternalEncodings saying what kind of converter to produce.
     *  One overload takes a locale, saying what kind of converter to produce.
     *  One overload takes an initial/partial span of initial text (at least 10 bytes a good idea)
     *  and the system will GUESS the format to use, and also return an indicate of how many (BOM) bytes
     *  skipped in the input.
     * 
     *  Note - when the guesses fail, this still returns a guess at CodeCvt<CHAR_T>.
     */
    template <typename CHAR_T>
    CodeCvt<CHAR_T> ConstructCodeCvt (UnicodeExternalEncodings useEncoding);
    template <typename CHAR_T>
    CodeCvt<CHAR_T> ConstructCodeCvt (const locale& l);
    template <typename CHAR_T>
    tuple<CodeCvt<CHAR_T>, size_t> ConstructCodeCvt (span<const byte> from);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TextConvert.inl"

#endif /*_Stroika_Foundation_Characters_TextConvert_h_*/
