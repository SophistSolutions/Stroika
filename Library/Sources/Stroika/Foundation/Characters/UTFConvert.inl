/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_UTFConvert_inl_
#define _Stroika_Foundation_Characters_UTFConvert_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#if qPlatform_Windows
#include <windows.h>
#endif

#include "../Debug/Assertions.h"
#include "../Memory/Bits.h"
//#include "../Memory/BlockAllocated.h" // causes include embrace problems
#include "../Memory/Span.h"
#include "../Memory/StackBuffer.h"

namespace Stroika::Foundation::Characters {

    namespace Private_ {
        // Memory::ValueOf() - but avoid #include
        template <typename T>
        inline T ValueOf_ (const optional<T>& t)
        {
            Require (t);
            return *t;
        }
    }

    /*
     ********************************************************************************
     *************************** Characters::UTFConverter ***************************
     ********************************************************************************
     */
#if qCompilerAndStdLib_DefaultMemberInitializerNeededEnclosingForDefaultFunArg_Buggy
    constexpr UTFConverter::UTFConverter ()
        : UTFConverter{Options{}}
    {
    }
#endif
    constexpr UTFConverter::UTFConverter (const Options& options)
        : fOriginalOptions_{options}
        , fUsingOptions{options}
    {
        if (fUsingOptions.fPreferredImplementation == nullopt) {
#if qPlatform_Windows
            fUsingOptions.fPreferredImplementation = Options::Implementation::eWindowsAPIWide2FromMultibyte;
#else
            fUsingOptions.fPreferredImplementation = Options::Implementation::eStroikaPortable;
#endif
        }
    }
    inline constexpr UTFConverter UTFConverter::kThe;

    template <Character_UNICODECanUnambiguouslyConvertFrom CHAR_T>
    constexpr optional<size_t> UTFConverter::NextCharacter (span<const CHAR_T> s)
    {
        if (s.empty ()) {
            return optional<size_t>{};
        }
        // Logic based on table from https://en.wikipedia.org/wiki/UTF-8#Encoding
        if constexpr (is_same_v<CHAR_T, Character_ASCII> or is_same_v<CHAR_T, Character_Latin1>) {
            return 1;
        }
        else if constexpr (is_same_v<CHAR_T, char8_t>) {
            auto i = s.begin ();
            // starting first byte
            uint8_t firstByte = static_cast<uint8_t> (*i);
            if (Memory::BitSubstring (firstByte, 7, 8) == 0b0) {
                return 1;
            }
            if (i != s.end ()) {
                ++i;
                if (Memory::BitSubstring (firstByte, 5, 8) == 0b110) {
                    return i == s.end () ? optional<size_t>{} : 2;
                }
            }
            if (i != s.end ()) {
                ++i;
                if (Memory::BitSubstring (firstByte, 4, 8) == 0b1110) {
                    return i == s.end () ? optional<size_t>{} : 3;
                }
            }
            if (i != s.end ()) {
                ++i;
                if (Memory::BitSubstring (firstByte, 3, 8) == 0b11110) {
                    return i == s.end () ? optional<size_t>{} : 4;
                }
            }
            return nullopt;
        }
        else if constexpr (sizeof (CHAR_T) == 2) {
            // @todo - must find docs
            auto i = s.begin ();
            // starting first char16_t
            AssertNotImplemented ();
            return nullopt;
        }
        else if constexpr (sizeof (CHAR_T) == 4) {
            return 1;
        }
        AssertNotReached ();
        return nullopt;
    }

    template <Character_UNICODECanUnambiguouslyConvertFrom CHAR_T>
    constexpr optional<size_t> UTFConverter::ComputeCharacterLength (span<const CHAR_T> s)
    {
        if constexpr (sizeof (CHAR_T) == 4) {
            return s.size ();
        }
        else {
            size_t charCount{};
            size_t i = 0;
            while (auto nc = NextCharacter (s.subspan (i))) {
                ++charCount;
                i += *nc;
                if (i == s.size ()) {
                    return charCount;
                }
            }
            return nullopt; // didn't end evenly at end of span, so something went wrong
        }
    }

    template <Character_UNICODECanUnambiguouslyConvertFrom TO, Character_UNICODECanUnambiguouslyConvertFrom FROM>
    constexpr size_t UTFConverter::ComputeTargetBufferSize (span<const FROM> src)
        requires (not is_const_v<TO>)
    {
        // NOTE - most of these routines could be (easily) optimized to actually compute the number
        // of characters, instead of using an upper bound, but that would involve walking the source
        // and counting characters. NOT clearly worth while. MAYBE as a hueristic, if src > 50 or so
        // its worth counting, but anything smaller will just end up in a fixed sized buffer, so the exact
        // count doesn't matter
        // @small performance todo!!!!
        if constexpr (sizeof (FROM) == sizeof (TO)) {
            return src.size (); // not super useful to do this conversion, but given how if constexpr works/evaluates, its often important than this code compiles, even if it doesn't execute
        }
        if constexpr (sizeof (FROM) == 1) {
            // worst case is each src byte is a character: for small buffers, not worth computing tighter limit but for larger, could
            // plausibly avoid a malloc, and even without, more likely to avoid wasted RAM/fragmentation for larger allocations
            if constexpr (sizeof (TO) == 4) {
                if (src.size () * sizeof (TO) > Memory::kStackBuffer_TargetInlineByteBufferSize) {
                    if (auto i = ComputeCharacterLength (src)) {
                        return *i;
                    }
                }
            }
            return src.size ();
        }
        else if constexpr (sizeof (FROM) == 2) {
            if constexpr (sizeof (TO) == 1) {
                // From https://stackoverflow.com/questions/9533258/what-is-the-maximum-number-of-bytes-for-a-utf-8-encoded-character
                // answer if translating only characters from UTF-16 to UTF-8: 4 bytes
                // @todo fix this is really smaller... I think 3 - look at https://en.wikipedia.org/wiki/UTF-8 more closely

                // also - for larger strings - MIGHT be worth a closer estimate?
                return 4 * src.size ();
            }
            else {
                Require (sizeof (TO) == 4);
                return src.size (); // worst case is no surrogate pairs
            }
        }
        else if constexpr (sizeof (FROM) == 4) {
            if constexpr (sizeof (TO) == 1) {
                // From https://stackoverflow.com/questions/9533258/what-is-the-maximum-number-of-bytes-for-a-utf-8-encoded-character
                // the maximum number of bytes for a character in UTF-8 is ... 4 (really 4 safe now so use that - was 6 bytes)

                // @todo this is probably worth walking the characters and doing a better estimate
                if (src.size () * 4 > Memory::kStackBuffer_TargetInlineByteBufferSize) {
                    // walk the characters, and see how much space each will use when encoded
                    size_t sz{};
                    for (auto c : src) {
                        if (isascii (static_cast<char32_t> (c))) {
                            ++sz;
                        }
                        else {
                            sz += 4; // @todo look at cases/ranges - not too hard to do better than this - very frequently just two bytes
                        }
                    }
                    return sz;
                }
                return 4 * src.size ();
            }
            else if constexpr (sizeof (TO) == 2) {
                return 2 * src.size ();
            }
            else {
                return src.size ();
            }
        }
        else {
            AssertNotReached ();
            return 0;
        }
    }
    template <Character_UNICODECanUnambiguouslyConvertFrom TO, Character_UNICODECanUnambiguouslyConvertFrom FROM>
    constexpr size_t UTFConverter::ComputeTargetBufferSize (span<FROM> src)
        requires (not is_const_v<TO>)
    {
        return ComputeTargetBufferSize<TO> (Memory::ConstSpan (src));
    }

    template <Character_UNICODECanUnambiguouslyConvertFrom CHAR_T>
    constexpr bool UTFConverter::AllFitsInTwoByteEncoding (span<const CHAR_T> s) noexcept
    {
        if constexpr (is_same_v<CHAR_T, Character_ASCII> or is_same_v<CHAR_T, Character_Latin1>) {
            return true;
        }
        // see https://en.wikipedia.org/wiki/UTF-16
        // @todo - THIS IS VERY WRONG - and MUCH MORE COMPLEX - but will only return false negatives so OK to start

        // note - tried to simplify with conditional_t but both sides evaluated
        if constexpr (is_same_v<remove_cv_t<CHAR_T>, Character>) {
            for (Character c : s) {
                if (c.GetCharacterCode () > 0xd7ff) [[unlikely]] {
                    return false;
                }
            }
        }
        else if constexpr (is_same_v<remove_cv_t<CHAR_T>, Character_Latin1>) {
            return true;
        }
        else if constexpr (is_same_v<remove_cv_t<CHAR_T>, char8_t>) {
            const char8_t* b = s.data ();
            const char8_t* e = b + s.size ();
            for (const char8_t* i = b; i < e;) {
                auto n = NextCharacter (span<const char8_t>{i, e});
                if (not n.has_value () or n > 2) [[unlikely]] {
                    return false;
                }
                i += *n;
            }
        }
        else {
            for (CHAR_T c : s) {
                if (static_cast<make_unsigned_t<CHAR_T>> (c) > 0xd7ff) [[unlikely]] {
                    return false;
                }
            }
        }
        return true;
    }

    template <Character_UNICODECanUnambiguouslyConvertFrom SRC_T, Character_UNICODECanUnambiguouslyConvertFrom TRG_T>
    inline auto UTFConverter::Convert (span<const SRC_T> source, span<TRG_T> target) const -> ConversionResult
    {
        Require ((target.size () >= ComputeTargetBufferSize<TRG_T> (source)));
        mbstate_t ignored{};
        auto      result = ConvertQuietly (source, target, &ignored);
        ThrowIf_ (result.fStatus);
        return result; // slice - no need to return 'status' - we throw on any status but success
    }
    template <Character_UNICODECanUnambiguouslyConvertFrom SRC_T, Character_UNICODECanUnambiguouslyConvertFrom TRG_T>
    inline auto UTFConverter::Convert (span<SRC_T> source, span<TRG_T> target) const -> ConversionResult
    {
        return Convert (Memory::ConstSpan (source), target);
    }
    template <typename TO, typename FROM>
    inline TO UTFConverter::Convert (const FROM& from) const
        requires ((is_same_v<TO, string> or is_same_v<TO, wstring> or is_same_v<TO, u8string> or is_same_v<TO, u16string> or is_same_v<TO, u32string>) and
                  (is_same_v<FROM, string> or is_same_v<FROM, wstring> or is_same_v<FROM, u8string> or is_same_v<FROM, u16string> or
                   is_same_v<FROM, u32string>))
    {
        if constexpr (is_same_v<TO, FROM>) {
            return from;
        }
        else {
            size_t                                       cvtBufSize = ComputeTargetBufferSize<typename TO::value_type> (span{from});
            Memory::StackBuffer<typename TO::value_type> buf{Memory::eUninitialized, cvtBufSize};
            return TO{buf.begin (), get<1> (Convert (span{from}, span{buf}))};
        }
    }

    template <Character_UNICODECanUnambiguouslyConvertFrom SRC_T, Character_UNICODECanUnambiguouslyConvertFrom TRG_T>
    inline span<TRG_T> UTFConverter::ConvertSpan (span<const SRC_T> source, span<TRG_T> target) const
        requires (not is_const_v<TRG_T>)
    {
        return span{target.data (), Convert (source, target).fTargetProduced};
    }
    template <Character_UNICODECanUnambiguouslyConvertFrom SRC_T, Character_UNICODECanUnambiguouslyConvertFrom TRG_T>
    inline span<TRG_T> UTFConverter::ConvertSpan (span<SRC_T> source, span<TRG_T> target) const
        requires (not is_const_v<TRG_T>)
    {
        return ConvertSpan (Memory::ConstSpan (source), target);
    }

    template <Character_UNICODECanUnambiguouslyConvertFrom SRC_T, Character_UNICODECanUnambiguouslyConvertFrom TRG_T>
    inline auto UTFConverter::ConvertQuietly (span<const SRC_T> source, span<TRG_T> target, mbstate_t* multibyteConversionState) const
        -> ConversionResultWithStatus
        requires (not is_const_v<TRG_T>)
    {
        Require ((target.size () >= ComputeTargetBufferSize<TRG_T> (source)));
        Require (multibyteConversionState != nullptr); // despite the fact its rarely used, to accomodate the cases where it can be
        using PRIMITIVE_SRC_T = typename decltype (this->ConvertToPrimitiveSpan_ (source))::value_type;
        using PRIMITIVE_TRG_T = typename decltype (this->ConvertToPrimitiveSpan_ (target))::value_type;
        if constexpr (is_same_v<PRIMITIVE_SRC_T, PRIMITIVE_TRG_T>) {
            Memory::CopySpanData_StaticCast (source, target);
            return ConversionResultWithStatus{{.fSourceConsumed = source.size (), .fTargetProduced = source.size ()}, ConversionStatusFlag::ok};
        }
        else if constexpr (is_same_v<SRC_T, Character_Latin1>) {
            // ALL TRG_T (but maybe ASCII?) have Character_Latin1 as a strict subset so simply copy
            Memory::CopySpanData_StaticCast (source, target);
            return ConversionResultWithStatus{{.fSourceConsumed = source.size (), .fTargetProduced = source.size ()}, ConversionStatusFlag::ok};
        }
        else if constexpr (is_same_v<TRG_T, Character_Latin1>) {
            // ALL TRG_T (but maybe ASCII?) have Character_Latin1 as a strict subset so simply copy
            Memory::CopySpanData_StaticCast (source, target);
            return ConversionResultWithStatus{{.fSourceConsumed = source.size (), .fTargetProduced = source.size ()}, ConversionStatusFlag::ok};
        }
        else {
            switch (Private_::ValueOf_ (fUsingOptions.fPreferredImplementation)) {
                case Options::Implementation::eStroikaPortable: {
                    return ConvertQuietly_StroikaPortable_ (ConvertToPrimitiveSpan_ (source), ConvertToPrimitiveSpan_ (target));
                }
#if qPlatform_Windows
                case Options::Implementation::eWindowsAPIWide2FromMultibyte: {
                    if constexpr ((sizeof (SRC_T) == 1 and sizeof (TRG_T) == 2) or (sizeof (SRC_T) == 2 and sizeof (TRG_T) == 1)) {
                        return ConvertQuietly_Win32_ (ConvertToPrimitiveSpan_ (source), ConvertToPrimitiveSpan_ (target));
                    }
                }
#endif
#if __has_include("boost/locale/encoding_utf.hpp")
                case Options::Implementation::eBoost_Locale: {
                    if constexpr (is_same_v<SRC_T, char8_t> and is_same_v<TRG_T, char16_t>) {
                        return ConvertQuietly_boost_locale_ (ConvertToPrimitiveSpan_ (source), ConvertToPrimitiveSpan_ (target));
                    }
                }
#endif
                case Options::Implementation::eCodeCVT: {
                    if constexpr ((is_same_v<SRC_T, char16_t> and is_same_v<SRC_T, char32_t>)and is_same_v<TRG_T, char8_t>) {
                        return ConvertQuietly_codeCvt_ (source, target, multibyteConversionState);
                    }
                }
            }
            return ConvertQuietly_StroikaPortable_ (ConvertToPrimitiveSpan_ (source), ConvertToPrimitiveSpan_ (target)); // default if preferred not available
        }
    }

    template <Character_Compatible TRG_T, Character_Compatible SRC_T>
    size_t UTFConverter::ConvertOffset (span<const SRC_T> source, size_t srcIndex) const
    {
        static_assert (not is_const_v<TRG_T>);
        // needlessly costly way to compute, but hopefully adequate for now -- LGP 2022-12-27
        // @todo redo using CountCharacters...
        Require (srcIndex <= source.size ());
        span<const SRC_T>          fakeSrc{source.begin (), srcIndex};
        Memory::StackBuffer<TRG_T> fakeOut{ComputeTargetBufferSize<TRG_T> (fakeSrc)};
        ConversionResult           r = Convert (fakeSrc, span{fakeOut});
        return r.fTargetProduced;
    }

    template <Character_UNICODECanUnambiguouslyConvertFrom FromT>
    constexpr auto UTFConverter::ConvertToPrimitiveSpan_ (span<FromT> f) -> span<CompatibleT_<FromT>>
    {
        return span{(CompatibleT_<FromT>*)f.data (), f.size ()};
    }

    namespace Private_ {
        template <typename CHAR_T>
        auto HandleShortTargetBuffer_ (const UTFConverter& utfcvt, auto from, auto to)
        {
            // one mismatch between the UTFConverter apis and ConvertQuietly, is ConvertQuietly REQUIRES
            // the data fit in targetbuf. Since there is no requirement to use up all the source text, just reduce source text size
            // to fit (and you can avoid this performance loss by using a larger output buffer)
            while (size_t requiredTargetBufSize = utfcvt.ComputeTargetBufferSize (*from) > to->size ()) {
                // could be smarter leveraging requiredTargetBufSize, but KISS for now
                if (from->empty ()) {
                    *to = decltype (*to){}; // say nothing output, but no change to input
                    return CodeCvt<CHAR_T>::partial;
                }
                *from = from->front (from->size () - 1); // shorten input til it fits safely (could be much faster if off by alot if we estimate and divide etc)
            }
            return CodeCvt<CHAR_T>::ok;
        };
    }

    template <Character_IsUnicodeCodePoint CHAR_T, Character_IsUnicodeCodePoint SERIALIZED_CHAR_T>
    CodeCvt<CHAR_T> UTFConverter::AsCodeCvt ()
    {
        // @todo: nice to use public Memory::BlockAllocationUseHelper<Rep_>, Memory::MakeSharedPtr<Rep_> (*this);
        // but the #include creates deadly embrace not worth solving now --LGP 2023-02-11
        struct Rep_ : CodeCvt<CHAR_T>::IRep {
            UTFConverter fCodeConverter_;
            using result      = typename CodeCvt<CHAR_T>::result;
            using MBState     = typename CodeCvt<CHAR_T>::MBState;
            using extern_type = SERIALIZED_CHAR_T;
            Rep_ (const UTFConverter& utfCodeCvt)
                : fCodeConverter_{utfCodeCvt}
            {
            }
            virtual result Bytes2Characters (span<const extern_type>* from, span<CHAR_T>* to, MBState* state) const override
            {
                RequireNotNull (state);
                RequireNotNull (from);
                RequireNotNull (to);
                if (auto preflightResult = Private_::HandleShortTargetBuffer_<CHAR_T> (fCodeConverter_, from, to) != CodeCvt<CHAR_T>::ok) {
                    return preflightResult; // HandleShortTargetBuffer_ patched from/to accordingly for the error
                }
                ConversionResultWithStatus r = fCodeConverter_.ConvertQuietly (*from, *to, state);
                from                         = from->subspan (r.fSourceConsumed);  // point to remaining to use data - typically none
                *to                          = to->subspan (0, r.fTargetProduced); // point ACTUAL copied data
                return cvtR_ (r.fStatus);
            }
            virtual result Characters2Bytes (span<const CHAR_T>* from, span<extern_type>* to, MBState* state) const override
            {
                if (auto preflightResult = Private_::HandleShortTargetBuffer_<CHAR_T> (fCodeConverter_, from, to) != CodeCvt<CHAR_T>::ok) {
                    return preflightResult; // HandleShortTargetBuffer_ patched from/to accordingly for the error
                }
                ConversionResultWithStatus r = fCodeConverter_.ConvertQuietly (*from, *to, state);
                from                         = from->subspan (r.fSourceConsumed);  // point to remaining to use data - typically none
                *to                          = to->subspan (0, r.fTargetProduced); // point ACTUAL copied data
                return cvtR_ (r.fStatus);
            }
            static result cvtR_ (ConversionStatusFlag status)
            {
                switch (status) {
                    case ConversionStatusFlag::ok:
                        return CodeCvt<CHAR_T>::ok;
                    case ConversionStatusFlag::sourceExhausted:
                        return CodeCvt<CHAR_T>::partial;
                    case ConversionStatusFlag::sourceIllegal:
                        return CodeCvt<CHAR_T>::error;
                }
            }
        };
        return make_shared<Rep_> (*this);
    }

#if qPlatform_Windows
    inline auto UTFConverter::ConvertQuietly_Win32_ (span<const char8_t> source, span<char16_t> target) -> ConversionResultWithStatus
    {
        if (source.begin () == source.end ()) {
            return ConversionResultWithStatus{{0, 0}, ConversionStatusFlag::ok};
        }
        else {
            int srcLen          = static_cast<int> (source.size ());
            int trgLen          = static_cast<int> (target.size ());
            int convertedLength = ::MultiByteToWideChar (CP_UTF8, 0, reinterpret_cast<const char*> (source.data ()), srcLen,
                                                         reinterpret_cast<WCHAR*> (&*target.begin ()), trgLen);
            return ConversionResultWithStatus{{static_cast<size_t> (srcLen), // wag - dont think WideCharToMultiByte tells us how much source consumed
                                               static_cast<size_t> (convertedLength)},
                                              convertedLength == 0 ? ConversionStatusFlag::sourceIllegal : ConversionStatusFlag::ok};
        }
    }
    inline auto UTFConverter::ConvertQuietly_Win32_ (span<const char16_t> source, span<char8_t> target) -> ConversionResultWithStatus
    {
        if (source.begin () == source.end ()) {
            return ConversionResultWithStatus{{0, 0}, ConversionStatusFlag::ok};
        }
        else {
            int srcLen          = static_cast<int> (source.size ());
            int trgLen          = static_cast<int> (target.size ());
            int convertedLength = ::WideCharToMultiByte (CP_UTF8, 0, reinterpret_cast<const WCHAR*> (source.data ()), srcLen,
                                                         reinterpret_cast<char*> (target.data ()), trgLen, nullptr, nullptr);
            return ConversionResultWithStatus{{static_cast<size_t> (srcLen), // wag - dont think WideCharToMultiByte tells us how much source consumed
                                               static_cast<size_t> (convertedLength)},
                                              convertedLength == 0 ? ConversionStatusFlag::sourceIllegal : ConversionStatusFlag::ok};
        }
    }
#endif

    inline void UTFConverter::ThrowIf_ (ConversionStatusFlag cr)
    {
        switch (cr) {
            case ConversionStatusFlag::ok:;
                break;
            default:
                Throw_ (cr);
        }
    }

#if __has_include("boost/locale/encoding_utf.hpp")
    inline auto UTFConverter::ConvertQuietly_boost_locale_ (span<const char8_t> source, const span<char16_t> target) -> ConversionResultWithStatus
    {
        if (source.empty ()) {
            return ConversionResultWithStatus{{0, 0}, ConversionStatusFlag::ok};
        }
        basic_string<char8_t> src = basic_string<char8_t>{source.data (), source.size ()};
        u16string             r   = boost::locale::conv::utf_to_utf<char16_t> (src.c_str ());
#if qCompilerAndStdLib_spanOfContainer_Buggy
        Memory::CopySpanData (span<char16_t>{r.data (), r.size ()}, target);
#else
        Memory::CopySpanData (span<char16_t>{r}, target);
#endif
        return ConversionResultWithStatus{{source.size (), r.size ()}, ConversionStatusFlag::ok};
#if 0
         utf::code_point c;
         char16_t* p = target.begin ();
        while(begin!=end) {
            c=utf::utf_traits<char8_t>::template decode<char8_t const *>(begin,end);
            if(c==utf::illegal || c==utf::incomplete) {
                // throw or ignore 

            }
            else {
                utf::utf_traits<char16_t>::template encode(c,p);
            }
       }

        // do something more like this loop/inserter stuff
        std::basic_string<CharOut> result;
                result.reserve(end-begin);
                typedef std::back_insert_iterator<std::basic_string<CharOut> > inserter_type;
                inserter_type inserter(result);
                utf::code_point c;
                while(begin!=end) {
                    c=utf::utf_traits<CharIn>::template decode<CharIn const *>(begin,end);
                    if(c==utf::illegal || c==utf::incomplete) {
                        if(how==stop)
                            throw conversion_error();
                    }
                    else {
                        utf::utf_traits<CharOut>::template encode<inserter_type>(c,inserter);
                    }
                }
                return result;
#endif
        //tmphack to test
        return ConversionResultWithStatus{{0, 0}, ConversionStatusFlag::ok};
    }
#endif

}
#endif /*_Stroika_Foundation_Characters_UTFConvert_inl_*/
