/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_TextConvert_inl_
#define _Stroika_Foundation_Characters_TextConvert_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Memory/Common.h"

namespace Stroika::Foundation::Characters {

    /*
     ********************************************************************************
     ************************** Characters::GetByteOrderMark ************************
     ********************************************************************************
     */
    constexpr span<const byte> GetByteOrderMark (UnicodeExternalEncodings e) noexcept
    {
        // Values from https://en.wikipedia.org/wiki/Byte_order_mark
        using namespace Memory;
        switch (e) {
            case UnicodeExternalEncodings::eUTF8: {
                constexpr byte r[] = {0xEF_b, 0xBB_b, 0xBF_b};
                return span<const byte>{r, sizeof (r)};
            }
            case UnicodeExternalEncodings::eUTF16_BE: {
                constexpr byte r[] = {0xFE_b, 0xFF_b};
                return span<const byte>{r, sizeof (r)};
            }
            case UnicodeExternalEncodings::eUTF16_LE: {
                constexpr byte r[] = {0xFF_b, 0xFE_b};
                return span<const byte>{r, sizeof (r)};
            }
            case UnicodeExternalEncodings::eUTF32_BE: {
                constexpr byte r[] = {0xFE_b, 0xFF_b, 0x00_b};
                return span<const byte>{r, sizeof (r)};
            }
            case UnicodeExternalEncodings::eUTF32_LE: {
                constexpr byte r[] = {0xFF_b, 0xFE_b, 0x00_b};
                return span<const byte>{r, sizeof (r)};
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

#endif /*_Stroika_Foundation_Characters_TextConvert_inl_*/
