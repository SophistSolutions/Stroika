/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
        constexpr byte kUTF32_BE_bom[] = {0xFE_b, 0xFF_b, 0x00_b};
        constexpr byte kUTF32_LE_bom[] = {0xFF_b, 0xFE_b, 0x00_b};
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
                return span<const byte>{Private_ ::kUTF32_BE_bom, sizeof (Private_ ::kUTF32_BE_bom)};
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
        auto check1 = [&d] (auto bom) -> optional<tuple<UnicodeExternalEncodings, size_t>> {
            if (d.size () >= bom.size () and std::memcmp (bom.data (), d.data (), bom.size ()) == 0) {
                return make_tuple (UnicodeExternalEncodings::eUTF8, bom.size ());
            }
            return nullopt;
        };
        // UTF-8 unambiguous and most likely, so check it first
        if (auto r = check1 (GetByteOrderMark (UnicodeExternalEncodings::eUTF8))) {
            return r;
        }
        // next check UTF32 BOM, only because needed to do before char16 BOM, cuz they overlap
        if (auto r = check1 (GetByteOrderMark (UnicodeExternalEncodings::eUTF32_BE))) {
            return r;
        }
        if (auto r = check1 (GetByteOrderMark (UnicodeExternalEncodings::eUTF32_LE))) {
            return r;
        }
        // next check UTF16
        if (auto r = check1 (GetByteOrderMark (UnicodeExternalEncodings::eUTF16_BE))) {
            return r;
        }
        if (auto r = check1 (GetByteOrderMark (UnicodeExternalEncodings::eUTF16_LE))) {
            return r;
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
