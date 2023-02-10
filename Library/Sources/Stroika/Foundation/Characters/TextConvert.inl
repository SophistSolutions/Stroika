/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_TextConvert_inl_
#define _Stroika_Foundation_Characters_TextConvert_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Characters {

    /*
     ********************************************************************************
     ************************** Characters::GetByteOrderMark ************************
     ********************************************************************************
     */
    constexpr span<const byte> GetByteOrderMark (UnicodeExternalEncodings e) noexcept
    {
        // Values from https://en.wikipedia.org/wiki/Byte_order_mark

        // NOTE - the combination of std::array and byte - rules with C++, are insanely inconvenient -LGP 2023-02-09
        using b = byte;
        switch (e) {
            case UnicodeExternalEncodings::eUTF7: {
                // no idea where I found this, but its in old Stroika code
                constexpr byte r[] = {b (0x2b), b (0x2f), b (0x76), b (0x38), b (0x2d)};
                return span<const byte>{&r[0], sizeof (r)};
            }
            case UnicodeExternalEncodings::eUTF8: {
                constexpr byte r[] = {b (0xEF), b (0xBB), b (0xBF)};
                return span<const byte>{&r[0], sizeof (r)};
            }
            case UnicodeExternalEncodings::eUTF16Wide_BE: {
                constexpr byte r[] = {b (0xFE), b (0xFF)};
                return span<const byte>{&r[0], sizeof (r)};
            }
            case UnicodeExternalEncodings::eUTF16Wide_LE: {
                constexpr byte r[] = {b (0xFF), b (0xFE)};
                return span<const byte>{&r[0], sizeof (r)};
            }
            case UnicodeExternalEncodings::eUTF32Wide_BE: {
                constexpr byte r[] = {b (0xFE), b (0xFF), b (0x00)};
                return span<const byte>{&r[0], sizeof (r)};
            }
            case UnicodeExternalEncodings::eUTF32Wide_LE: {
                constexpr byte r[] = {b (0xFF), b (0xFE), b (0x00)};
                return span<const byte>{&r[0], sizeof (r)};
            }
            default:
                AssertNotReached ();
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
        if (auto r = check1 (GetByteOrderMark (UnicodeExternalEncodings::eUTF32Wide_BE))) {
            return r;
        }
        if (auto r = check1 (GetByteOrderMark (UnicodeExternalEncodings::eUTF32Wide_LE))) {
            return r;
        }
        // next check UTF16
        if (auto r = check1 (GetByteOrderMark (UnicodeExternalEncodings::eUTF16Wide_BE))) {
            return r;
        }
        if (auto r = check1 (GetByteOrderMark (UnicodeExternalEncodings::eUTF16Wide_LE))) {
            return r;
        }
        // finally UTF-7
        if (auto r = check1 (GetByteOrderMark (UnicodeExternalEncodings::eUTF7))) {
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
