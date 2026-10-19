/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Memory/Common.h"

namespace Stroika::Foundation::Characters {

    /*
     ********************************************************************************
     ************************** Characters::GetByteOrderMark ************************
     ********************************************************************************
     */
    namespace Private_ {
        using namespace Memory;
        constexpr byte kUTF8_bom[]     = {0xEF_b, 0xBB_b, 0xBF_b};
        constexpr byte kUTF16_BE_bom[] = {0xFE_b, 0xFF_b};
        constexpr byte kUTF16_LE_bom[] = {0xFF_b, 0xFE_b};
        constexpr byte kUTF32_BE_bom[] = {0x00_b, 0x00_b, 0xFE_b, 0xFF_b};
        constexpr byte kUTF32_LE_bom[] = {0xFF_b, 0xFE_b, 0x00_b, 0x00_b};
    }
    constexpr span<const byte> GetByteOrderMark (UnicodeExternalEncodings e) noexcept
    {
        // Values from https://en.wikipedia.org/wiki/Byte_order_mark
        switch (e) {
            case UnicodeExternalEncodings::eUTF8: {
#if qCompilerAndStdLib_span_requires_explicit_type_for_BLOBCVT_Buggy
                return span<const byte>{Private_ ::kUTF8_bom, sizeof (Private_ ::kUTF8_bom)};
#else
                return span{Private_ ::kUTF8_bom, sizeof (Private_ ::kUTF8_bom)};
#endif
            }
            case UnicodeExternalEncodings::eUTF16_BE: {
#if qCompilerAndStdLib_span_requires_explicit_type_for_BLOBCVT_Buggy
                return span<const byte>{Private_ ::kUTF16_BE_bom, sizeof (Private_ ::kUTF16_BE_bom)};
#else
                return span{Private_ ::kUTF16_BE_bom, sizeof (Private_ ::kUTF16_BE_bom)};
#endif
            }
            case UnicodeExternalEncodings::eUTF16_LE: {
#if qCompilerAndStdLib_span_requires_explicit_type_for_BLOBCVT_Buggy
                return span<const byte>{Private_ ::kUTF16_LE_bom, sizeof (Private_ ::kUTF16_LE_bom)};
#else
                return span{Private_ ::kUTF16_LE_bom, sizeof (Private_ ::kUTF16_LE_bom)};
#endif
            }
            case UnicodeExternalEncodings::eUTF32_BE: {
#if qCompilerAndStdLib_span_requires_explicit_type_for_BLOBCVT_Buggy
                return span<const byte>{Private_ ::kUTF32_BE_bom, sizeof (Private_ ::kUTF32_BE_bom)};
#else
                return span{Private_ ::kUTF32_BE_bom, sizeof (Private_ ::kUTF32_BE_bom)};
#endif
            }
            case UnicodeExternalEncodings::eUTF32_LE: {
#if qCompilerAndStdLib_span_requires_explicit_type_for_BLOBCVT_Buggy
                return span<const byte>{Private_ ::kUTF32_LE_bom, sizeof (Private_ ::kUTF32_LE_bom)};
#else
                return span{Private_ ::kUTF32_LE_bom, sizeof (Private_ ::kUTF32_LE_bom)};
#endif
            }
            default:
                AssertNotReached ();
                return span<const byte>{};
        }
    }

    /*
     ********************************************************************************
     *********************** Characters::ReadByteOrderMark **************************
     ********************************************************************************
     */
    constexpr optional<tuple<UnicodeExternalEncodings, size_t>> ReadByteOrderMark (span<const byte> d) noexcept
    {
        auto check1 = [&d] (UnicodeExternalEncodings e) -> optional<tuple<UnicodeExternalEncodings, size_t>> {
            span<const byte> bom = GetByteOrderMark (e);
            if (d.size () >= bom.size () and std::memcmp (bom.data (), d.data (), bom.size ()) == 0) {
                return make_tuple (e, bom.size ());
            }
            return nullopt;
        };
        // UTF-8 unambiguous and most likely, so check it first; UTF-32LE's before UTF-16LE's, which it begins with (so UTF-16LE
        // text whose first character is U+0000 reads as UTF-32LE - the one ambiguity byte order marks have)
        for (UnicodeExternalEncodings e : {UnicodeExternalEncodings::eUTF8, UnicodeExternalEncodings::eUTF32_BE, UnicodeExternalEncodings::eUTF32_LE,
                                           UnicodeExternalEncodings::eUTF16_BE, UnicodeExternalEncodings::eUTF16_LE}) {
            if (auto r = check1 (e)) {
                return r;
            }
        }
        return nullopt;
    }

    /*
     ********************************************************************************
     *********************** Characters::WriteByteOrderMark *************************
     ********************************************************************************
     */
    inline span<byte> WriteByteOrderMark (UnicodeExternalEncodings e, span<byte> into)
    {
        Require (into.size () >= GetByteOrderMark (e).size ());
        auto bom = GetByteOrderMark (e);
        std::memcpy (into.data (), bom.data (), bom.size ());
        return into.subspan (bom.size ());
    }

}
