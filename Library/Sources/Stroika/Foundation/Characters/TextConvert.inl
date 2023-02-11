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
#include "../Memory/Common.h"

namespace Stroika::Foundation::Characters {

    /*
     ********************************************************************************
     ************************** Characters::GetByteOrderMark ************************
     ********************************************************************************
     */
    constexpr span<const byte> GetByteOrderMark (UnicodeExternalEncodings e) noexcept
    {
        // Values from https://en.wikipedia.org/wiki/Byte_order_mark (except utf7)
        using namespace Memory;
        switch (e) {
            case UnicodeExternalEncodings::eUTF7: {
                // no idea where I found this, but its in old Stroika code
                constexpr byte r[] = {0x2b_b, 0x2f_b, 0x76_b, 0x38_b, 0x2d_b};
                return span<const byte>{&r[0], sizeof (r)};
            }
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

    /*
     ********************************************************************************
     ************************ Characters::ConstructCodeCvt **************************
     ********************************************************************************
     */
    namespace Private_ {

        template <typename SERIALIZED_CHAR_T>
        struct Rep_ : CodeCvt<Character>::IRep {
            UTFConverter fCodeConverter_;
            using result                     = typename CodeCvt<Character>::result;
            using MBState                    = typename CodeCvt<Character>::MBState;
            using ConversionResultWithStatus = UTFConverter::ConversionResultWithStatus;
            using ConversionStatusFlag       = UTFConverter::ConversionStatusFlag;

            Rep_ (const UTFConverter& utfCodeCvt)
                : fCodeConverter_{utfCodeCvt}
            {
            }
            virtual result Bytes2Characters (span<const byte>* from, span<Character>* to, MBState* state) const override
            {
                RequireNotNull (state);
                RequireNotNull (from);
                RequireNotNull (to);

                // essentially 'cast' from bytes to from SERIALIZED_CHAR_T (could be char8_t, char16_t or whatever works with UTFConverter)
                span<const SERIALIZED_CHAR_T> serializedFrom{reinterpret_cast<const SERIALIZED_CHAR_T*> (from->data ()),
                                                             from->size () / sizeof (SERIALIZED_CHAR_T)};
                Assert (serializedFrom.size_bytes () <= from->size ()); // note - serializedFrom could be smaller than from in bytespan

                auto handleShortTargetBuffer = [&] (span<const SERIALIZED_CHAR_T>* from, span<Character>* to) {
                    // one mismatch between the UTFConverter apis and ConvertQuietly, is ConvertQuietly REQUIRES
                    // the data fit in targetbuf. Since there is no requirement to use up all the source text, just reduce source text size
                    // to fit (and you can avoid this performance loss by using a larger output buffer)
                    while (size_t requiredTargetBufSize = fCodeConverter_.ComputeTargetBufferSize<Character> (*from) > to->size ()) {
                        // could be smarter leveraging requiredTargetBufSize, but KISS for now
                        if (from->empty ()) {
                            *to = span<Character>{}; // say nothing output, but no change to input
                            return CodeCvt<Character>::partial;
                        }
                        *from = from->subspan (0, from->size () - 1); // shorten input til it fits safely (could be much faster if off by alot if we estimate and divide etc)
                    }
                    return CodeCvt<Character>::ok;
                };
                if (auto preflightResult = handleShortTargetBuffer (&serializedFrom, to) != CodeCvt<Character>::ok) {
                    return preflightResult; // HandleShortTargetBuffer_ patched from/to accordingly for the error
                }
                ConversionResultWithStatus r = fCodeConverter_.ConvertQuietly (serializedFrom, *to, state);
                *from = as_bytes (serializedFrom.subspan (r.fSourceConsumed)); // point to remaining to use data - typically none
                *to   = to->subspan (0, r.fTargetProduced);                    // point ACTUAL copied data
                return cvtR_ (r.fStatus);
            }
            virtual result Characters2Bytes (span<const Character>* from, span<byte>* to, MBState* state) const override
            {
                RequireNotNull (state);
                RequireNotNull (from);
                RequireNotNull (to);

                // essentially 'cast' from bytes to from SERIALIZED_CHAR_T (could be char8_t, char16_t or whatever works with UTFConverter)
                span<SERIALIZED_CHAR_T> serializedTo{reinterpret_cast<SERIALIZED_CHAR_T*> (to->data ()), to->size () / sizeof (SERIALIZED_CHAR_T)};
                Assert (serializedTo.size_bytes () <= to->size ()); // note - serializedTo could be smaller than to in bytespan

                auto handleShortTargetBuffer = [&] (span<const Character>* from, span<SERIALIZED_CHAR_T>* to) {
                    // one mismatch between the UTFConverter apis and ConvertQuietly, is ConvertQuietly REQUIRES
                    // the data fit in targetbuf. Since there is no requirement to use up all the source text, just reduce source text size
                    // to fit (and you can avoid this performance loss by using a larger output buffer)
                    while (size_t requiredTargetBufSize = fCodeConverter_.ComputeTargetBufferSize<SERIALIZED_CHAR_T> (*from) > to->size ()) {
                        // could be smarter leveraging requiredTargetBufSize, but KISS for now
                        if (from->empty ()) {
                            *to = span<SERIALIZED_CHAR_T>{}; // say nothing output, but no change to input
                            return CodeCvt<Character>::partial;
                        }
                        *from = from->subspan (0, from->size () - 1); // shorten input til it fits safely (could be much faster if off by alot if we estimate and divide etc)
                    }
                    return CodeCvt<Character>::ok;
                };
                if (auto preflightResult = handleShortTargetBuffer (from, &serializedTo) != CodeCvt<Character>::ok) {
                    return preflightResult; // HandleShortTargetBuffer_ patched from/to accordingly for the error
                }
                ConversionResultWithStatus r = fCodeConverter_.ConvertQuietly (*from, serializedTo, state);
                *from                        = from->subspan (r.fSourceConsumed); // point to remaining to use data - typically none
                *to                          = to->subspan (0, r.fTargetProduced * sizeof (SERIALIZED_CHAR_T)); // point ACTUAL copied data
                return cvtR_ (r.fStatus);
            }
            static result cvtR_ (UTFConverter::ConversionStatusFlag status)
            {
                switch (status) {
                    case ConversionStatusFlag::ok:
                        return CodeCvt<Character>::ok;
                    case ConversionStatusFlag::sourceExhausted:
                        return CodeCvt<Character>::partial;
                    case ConversionStatusFlag::sourceIllegal:
                        return CodeCvt<Character>::error;
                    default:
                        AssertNotReached ();
                        return CodeCvt<Character>::error;
                }
            }
        };
    }

    inline CodeCvt<Character> ConstructCodeCvt (UnicodeExternalEncodings useEncoding)
    {
        // @todo: nice to use public Memory::BlockAllocationUseHelper<Rep_>, Memory::MakeSharedPtr<Rep_> (*this);
        // but the #include creates deadly embrace not worth solving now --LGP 2023-02-11
        switch (useEncoding) {
            case Characters::UnicodeExternalEncodings::eUTF8:
                return CodeCvt<Character>{make_shared<Private_::Rep_<char8_t>> (UTFConverter::kThe)};
            case Characters::UnicodeExternalEncodings::eUTF16:
                return CodeCvt<Character>{make_shared<Private_::Rep_<char16_t>> (UTFConverter::kThe)};
            case Characters::UnicodeExternalEncodings::eUTF32:
                return CodeCvt<Character>{make_shared<Private_::Rep_<char32_t>> (UTFConverter::kThe)};
            default:
                AssertNotReached ();
                return *((CodeCvt<Character>*)nullptr); //tmphack - mostly use UTFConverer::kThe.AsCodeCvt - maybe thats it?
        }
    }
    inline CodeCvt<Character> ConstructCodeCvt (const locale& l)
    {
        return *((CodeCvt<Character>*)nullptr); //tmphack - mostly use UTFConverer::kThe.AsCodeCvt - maybe thats it?
#if 0
        if constexpr (is_same_v <CHAR_T, wchar_t>) {
            return CodeCvt<wchar_t>{l}; // provided by std-c++ library
        }
        else {
            // must chain conversions
            return ConstructCodeCvt<CHAR_T> (UnicodeExternalEncodings::eUTF16); //tmphack
        }
#endif
    }
    inline tuple<CodeCvt<Character>, size_t> ConstructCodeCvt (span<const byte> from)
    {
        if (optional<tuple<UnicodeExternalEncodings, size_t>> o = ReadByteOrderMark (from)) {
            return make_tuple (ConstructCodeCvt (get<0> (*o)), get<1> (*o));
        }
        else {
            return make_tuple (ConstructCodeCvt (UnicodeExternalEncodings::eDefault), 0);
        }
    }

}

#endif /*_Stroika_Foundation_Characters_TextConvert_inl_*/
