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
     */
    enum class UnicodeExternalEncodings {
        eUTF7,
        eUTF8,
        eUTF16Wide_BE,
        eUTF16Wide_LE,
        eUTF16Wide = std::endian::native == std::endian::big ? eUTF16Wide_BE : eUTF16Wide_LE,
        eUTF32Wide_BE,
        eUTF32Wide_LE,
        eUTF32Wide = std::endian::native == std::endian::big ? eUTF32Wide_BE : eUTF32Wide_LE,
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
     *  Construct codecvt<> object to allow converting of bytes into a UNICODE CHAR_T>, based on
     *  sampling the existing bytes, checking for BOM, and as a fallback, using the current locale.
     *  Also returns number of bytes consumed 'from' - for BOM, if any.
     */
    template <typename CHAR_T>
    tuple<CodeCvt<CHAR_T>, size_t> ConstructCodeCvtToUnicode (span<const byte> from);

    /**
     *  Construct codecvt<> object to allow converting of UNICODE CHAR_T to bytes, either taking argument UNICODE
     *  encoding, or a locale (if not specified, the current locale).
     */
    template <typename CHAR_T>
    tuple<CodeCvt<CHAR_T>, size_t> ConstructCodeCvtUnicodeToBytes (span<const CHAR_T> from, UnicodeExternalEncodings e);
    template <typename CHAR_T>
    tuple<CodeCvt<CHAR_T>, size_t> ConstructCodeCvtUnicodeToBytes (span<const CHAR_T> from, const locale& l = locale{});

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TextConvert.inl"

#endif /*_Stroika_Foundation_Characters_TextConvert_h_*/
