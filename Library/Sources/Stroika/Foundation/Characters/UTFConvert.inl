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
            fUsingOptions.fPreferredImplementation = Options::Implementation::eWin32Wide2FromMultibyte;
#else
            fUsingOptions.fPreferredImplementation = Options::Implementation::eStroikaPortable;
#endif
        }
    }
    inline constexpr UTFConverter UTFConverter::kThe;

    template <Character_Compatible TO, Character_Compatible FROM>
    constexpr size_t UTFConverter::ComputeTargetBufferSize (span<const FROM> src)
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
                    for (char32_t c : src) {
                        if (isascii (c)) {
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

    inline auto UTFConverter::Convert (span<const char8_t> source, span<char16_t> target) const -> ConversionResult
    {
        Require ((target.size () >= ComputeTargetBufferSize<char16_t> (source)));
        auto result = ConvertQuietly (source, target);
        ThrowIf_ (result.fStatus);
        return result; // slice
    }
    inline auto UTFConverter::Convert (span<const char16_t> source, span<char8_t> target) const -> ConversionResult
    {
        Require ((target.size () >= ComputeTargetBufferSize<char8_t> (source)));
        auto result = ConvertQuietly (source, target);
        ThrowIf_ (result.fStatus);
        return result; // slice
    }
    inline auto UTFConverter::Convert (span<const char8_t> source, span<char32_t> target) const -> ConversionResult
    {
        Require ((target.size () >= ComputeTargetBufferSize<char32_t> (source)));
        auto result = ConvertQuietly (source, target);
        ThrowIf_ (result.fStatus);
        return result; // slice
    }
    inline auto UTFConverter::Convert (span<const char32_t> source, span<char8_t> target) const -> ConversionResult
    {
        Require ((target.size () >= ComputeTargetBufferSize<char8_t> (source)));
        auto result = ConvertQuietly (source, target);
        ThrowIf_ (result.fStatus);
        return result; // slice
    }
    inline auto UTFConverter::Convert (span<const char16_t> source, span<char32_t> target) const -> ConversionResult
    {
        Require ((target.size () >= ComputeTargetBufferSize<char32_t> (source)));
        auto result = ConvertQuietly (source, target);
        ThrowIf_ (result.fStatus);
        return result; // slice
    }
    inline auto UTFConverter::Convert (span<const char32_t> source, span<char16_t> target) const -> ConversionResult
    {
        Require ((target.size () >= ComputeTargetBufferSize<char16_t> (source)));
        auto result = ConvertQuietly (source, target);
        ThrowIf_ (result.fStatus);
        return result; // slice
    }
    inline auto UTFConverter::Convert (span<const char8_t> source, span<char16_t> target, mbstate_t* multibyteConversionState) const -> ConversionResult
    {
        Require ((target.size () >= ComputeTargetBufferSize<char16_t> (source)));
        RequireNotNull (multibyteConversionState);
        auto result = ConvertQuietly (source, target, multibyteConversionState);
        ThrowIf_ (result.fStatus);
        return result; // slice
    }
    inline auto UTFConverter::Convert (span<const char8_t> source, span<char32_t> target, mbstate_t* multibyteConversionState) const -> ConversionResult
    {
        Require ((target.size () >= ComputeTargetBufferSize<char32_t> (source)));
        RequireNotNull (multibyteConversionState);
        auto result = ConvertQuietly (source, target, multibyteConversionState);
        ThrowIf_ (result.fStatus);
        return result; // slice
    }
    template <Character_Compatible SRC_T, Character_Compatible TRG_T>
    inline auto UTFConverter::Convert (span<const SRC_T> source, span<TRG_T> target) const -> ConversionResult
        requires (not is_const_v<TRG_T>) {
            Require ((target.size () >= ComputeTargetBufferSize<TRG_T> (source)));
            if constexpr (sizeof (SRC_T) == sizeof (TRG_T)) {
                copy (source, target, source.size ()); // pointless conversion, but if requested...
                return source.size ();
            }
            return Convert (ConvertCompatibleSpan_ (source), ConvertCompatibleSpan_ (target));
        } template <typename TO, typename FROM>
        inline TO UTFConverter::Convert (const FROM& from) const
        requires (
            (is_same_v<TO, string> or is_same_v<TO, wstring> or is_same_v<TO, u8string> or is_same_v<TO, u16string> or is_same_v<TO, u32string>) and
            (is_same_v<FROM, string> or is_same_v<FROM, wstring> or is_same_v<FROM, u8string> or is_same_v<FROM, u16string> or is_same_v<FROM, u32string>))
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

    inline auto UTFConverter::ConvertQuietly (span<const char8_t> source, span<char16_t> target) const -> ConversionResultWithStatus
    {
        Require ((target.size () >= ComputeTargetBufferSize<char16_t> (source)));
        switch (Private_::ValueOf_ (fUsingOptions.fPreferredImplementation)) {
#if qPlatform_Windows
            case Options::Implementation::eWin32Wide2FromMultibyte: {
                return ConvertQuietly_Win32_ (source, target);
            }
#endif
#if __has_include("boost/locale/encoding_utf.hpp")
            case Options::Implementation::eBoost_Locale: {
                return ConvertQuietly_boost_locale_ (source, target);
            }
#endif
            case Options::Implementation::eStroikaPortable: {
                return ConvertQuietly_StroikaPortable_ (source, target);
            }
            case Options::Implementation::eCodeCVT: {
                return ConvertQuietly_codeCvt_ (source, target, nullptr);
            }
            default: {
                return ConvertQuietly_StroikaPortable_ (source, target); // default if preferred not available
            }
        }
    }
    inline auto UTFConverter::ConvertQuietly (span<const char16_t> source, span<char8_t> target) const -> ConversionResultWithStatus
    {
        Require ((target.size () >= ComputeTargetBufferSize<char8_t> (source)));
        switch (Private_::ValueOf_ (fUsingOptions.fPreferredImplementation)) {
#if qPlatform_Windows
            case Options::Implementation::eWin32Wide2FromMultibyte: {
                return ConvertQuietly_Win32_ (source, target);
            }
#endif
            case Options::Implementation::eStroikaPortable: {
                return ConvertQuietly_StroikaPortable_ (source, target);
            }
            case Options::Implementation::eCodeCVT: {
                return ConvertQuietly_codeCvt_ (source, target);
            }
            default: {
                return ConvertQuietly_StroikaPortable_ (source, target); // default if preferred not available
            }
        }
    }
    inline auto UTFConverter::ConvertQuietly (span<const char8_t> source, span<char32_t> target) const -> ConversionResultWithStatus
    {
        Require ((target.size () >= ComputeTargetBufferSize<char32_t> (source)));
        switch (Private_::ValueOf_ (fUsingOptions.fPreferredImplementation)) {
            case Options::Implementation::eStroikaPortable: {
                return ConvertQuietly_StroikaPortable_ (source, target);
            }
            case Options::Implementation::eCodeCVT: {
                return ConvertQuietly_codeCvt_ (source, target, nullptr);
            }
            default: {
                return ConvertQuietly_StroikaPortable_ (source, target); // default if preferred not available
            }
        }
    }
    inline auto UTFConverter::ConvertQuietly (span<const char32_t> source, span<char8_t> target) const -> ConversionResultWithStatus
    {
        Require ((target.size () >= ComputeTargetBufferSize<char8_t> (source)));
        switch (Private_::ValueOf_ (fUsingOptions.fPreferredImplementation)) {
            case Options::Implementation::eStroikaPortable: {
                return ConvertQuietly_StroikaPortable_ (source, target);
            }
            case Options::Implementation::eCodeCVT: {
                return ConvertQuietly_codeCvt_ (source, target);
            }
            default: {
                return ConvertQuietly_StroikaPortable_ (source, target); // default if preferred not available
            }
        }
    }
    inline auto UTFConverter::ConvertQuietly (span<const char16_t> source, span<char32_t> target) const -> ConversionResultWithStatus
    {
        Require ((target.size () >= ComputeTargetBufferSize<char32_t> (source)));
        switch (Private_::ValueOf_ (fUsingOptions.fPreferredImplementation)) {
            case Options::Implementation::eStroikaPortable: {
                return ConvertQuietly_StroikaPortable_ (source, target);
            }
#if 0
            case Options::Implementation::eCodeCVT: {
                return ConvertQuietly_codeCvt_ (source, target, nullptr);
            }
#endif
            default: {
                return ConvertQuietly_StroikaPortable_ (source, target); // default if preferred not available
            }
        }
    }
    inline auto UTFConverter::ConvertQuietly (span<const char32_t> source, span<char16_t> target) const -> ConversionResultWithStatus
    {
        Require ((target.size () >= ComputeTargetBufferSize<char16_t> (source)));
        switch (Private_::ValueOf_ (fUsingOptions.fPreferredImplementation)) {
            case Options::Implementation::eStroikaPortable: {
                return ConvertQuietly_StroikaPortable_ (source, target);
            }
#if 0
            case Options::Implementation::eCodeCVT: {
                return ConvertQuietly_codeCvt_ (source, target);
            }
#endif
            default: {
                return ConvertQuietly_StroikaPortable_ (source, target); // default if preferred not available
            }
        }
    }
    inline auto UTFConverter::ConvertQuietly (span<const char8_t> source, span<char16_t> target, mbstate_t* multibyteConversionState) const -> ConversionResultWithStatus
    {
        Require ((target.size () >= ComputeTargetBufferSize<char16_t> (source)));
        RequireNotNull (multibyteConversionState);
        switch (Private_::ValueOf_ (fUsingOptions.fPreferredImplementation)) {
            case Options::Implementation::eCodeCVT: {
                return ConvertQuietly_codeCvt_ (source, target, multibyteConversionState);
            }
            default: {
                return ConvertQuietly_StroikaPortable_ (source, target); // default if preferred not available
            }
        }
    }
    inline auto UTFConverter::ConvertQuietly (span<const char8_t> source, span<char32_t> target, mbstate_t* multibyteConversionState) const -> ConversionResultWithStatus
    {
        Require ((target.size () >= ComputeTargetBufferSize<char32_t> (source)));
        RequireNotNull (multibyteConversionState);
        switch (Private_::ValueOf_ (fUsingOptions.fPreferredImplementation)) {
            case Options::Implementation::eCodeCVT: {
                return ConvertQuietly_codeCvt_ (source, target, multibyteConversionState);
            }
            default: {
                return ConvertQuietly_StroikaPortable_ (source, target); // default if preferred not available
            }
        }
    }
    template <Character_Compatible SRC_T, Character_Compatible TRG_T>
    inline auto UTFConverter::ConvertQuietly (span<const SRC_T> source, span<TRG_T> target) const -> ConversionResultWithStatus
        requires (not is_const_v<TRG_T>) {
            if constexpr (sizeof (SRC_T) == sizeof (TRG_T)) {
                copy (source, target, source.size ()); // pointless conversion, but if requested...
                return source.size ();
            }
            return ConvertQuietly (ConvertCompatibleSpan_ (source), ConvertCompatibleSpan_ (target));
        }

    template <Character_IsUnicodeCodePoint TRG_T, Character_IsUnicodeCodePoint SRC_T>
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

    template <Character_IsUnicodeCodePointOrPlainChar CHAR_T>
    constexpr optional<size_t> UTFConverter::NextCharacter (span<const CHAR_T> s)
    {
        // Logic based on table from https://en.wikipedia.org/wiki/UTF-8#Encoding
        if (is_same_v<CHAR_T, char>) {
            return s.empty () ? optional<size_t>{} : 1;
        }
        else if constexpr (is_same_v<CHAR_T, char8_t>) {
            auto i = s.begin ();
            // starting first byte
            if (i != s.end ()) {
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
        }
        else if constexpr (sizeof (CHAR_T) == 2) {
            // @todo - must find docs
            auto i = s.begin ();
            // starting first char16_t
            if (i != s.end ()) {
                AssertNotImplemented ();
            }
            return nullopt;
        }
        else if constexpr (sizeof (CHAR_T) == 4) {
            return s.size () >= 4 ? 4 : optional<size_t>{};
        }
        AssertNotReached ();
        return nullopt;
    }

    template <Character_IsUnicodeCodePointOrPlainChar CHAR_T>
    constexpr optional<size_t> UTFConverter::ComputeCharacterLength (span<const CHAR_T> s)
    {
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

    template <Character_Compatible FromT>
    constexpr auto UTFConverter::ConvertCompatibleSpan_ (span<FromT> f) -> span<CompatibleT_<FromT>>
    {
        return span{(CompatibleT_<FromT>*)f.data (), f.size ()};
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
            int convertedLength = ::MultiByteToWideChar (CP_UTF8, 0, reinterpret_cast<const char*> (&*source.begin ()), srcLen, reinterpret_cast<WCHAR*> (&*target.begin ()), trgLen);
            return ConversionResultWithStatus{
                {static_cast<size_t> (srcLen), // wag - dont think WideCharToMultiByte tells us how much source consumed
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
            int convertedLength = ::WideCharToMultiByte (CP_UTF8, 0, reinterpret_cast<const WCHAR*> (&*source.begin ()), srcLen, reinterpret_cast<char*> (&*target.begin ()), trgLen, nullptr, nullptr);
            return ConversionResultWithStatus{
                {static_cast<size_t> (srcLen), // wag - dont think WideCharToMultiByte tells us how much source consumed
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
        basic_string<char8_t> src = basic_string<char8_t>{reinterpret_cast<const char8_t*> (&*source.begin ()), reinterpret_cast<const char8_t*> (&*source.begin ()) + source.size ()};
        u16string             r   = boost::locale::conv::utf_to_utf<char16_t> (src.c_str ());
        copy (r.begin (), r.end (), target.begin ());
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
