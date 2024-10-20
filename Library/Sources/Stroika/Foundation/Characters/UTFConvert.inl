/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#if qPlatform_Windows
#include <windows.h>
#endif

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Memory/Bits.h"
//#include "Stroika/Foundation/Memory/BlockAllocated.h" // causes include embrace problems
#include "Stroika/Foundation/Memory/Span.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"

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
     *************************** Characters::UTFConvert *****************************
     ********************************************************************************
     */
#if qCompilerAndStdLib_DefaultMemberInitializerNeededEnclosingForDefaultFunArg_Buggy
    constexpr UTFConvert::UTFConvert ()
        : UTFConvert{Options{}}
    {
    }
#endif
    constexpr UTFConvert::UTFConvert (const Options& options)
        : fOriginalOptions_{options}
        , fUsingOptions{options}
    {
#if qPlatform_Windows
        if (fUsingOptions.fPreferredImplementation == nullopt and options.fInvalidCharacterReplacement == nullopt) {
            fUsingOptions.fPreferredImplementation = Options::Implementation::eWindowsAPIWide2FromMultibyte;
        }
#endif
        if (fUsingOptions.fPreferredImplementation == nullopt) {
            fUsingOptions.fPreferredImplementation = Options::Implementation::eStroikaPortable;
        }
        if (options.fInvalidCharacterReplacement) {
            // For now, thats all that supports fInvalidCharacterReplacement, but could do others pretty easily - probably
            // Windows Wide2Multipbyte etc doesnt work with this, so dont use there.
            //       - LGP 2023-08-07
            Require (fUsingOptions.fPreferredImplementation == Options::Implementation::eStroikaPortable);
        }
    }
    inline constexpr auto UTFConvert::GetOptions () const -> Options
    {
        return fOriginalOptions_;
    }
    inline constexpr UTFConvert UTFConvert::kThe;
    template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    constexpr optional<size_t> UTFConvert::NextCharacter (span<const CHAR_T> s)
    {
        if (s.empty ()) {
            return optional<size_t>{};
        }
        // Logic based on table from https://en.wikipedia.org/wiki/UTF-8#Encoding
        if constexpr (same_as<CHAR_T, ASCII> or same_as<CHAR_T, Latin1>) {
            return 1;
        }
        else if constexpr (same_as<CHAR_T, char8_t>) {
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
    template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    constexpr optional<size_t> UTFConvert::ComputeCharacterLength (span<const CHAR_T> s)
    {
        if constexpr (sizeof (CHAR_T) == 4) {
            return s.size ();
        }
        else {
            size_t charCount{};
            size_t i = 0;
            while (optional<size_t> nOctets = NextCharacter (s.subspan (i))) {
                ++charCount;
                i += *nOctets;
                if (i == s.size ()) [[unlikely]] {
                    break; // used up all the input span, so we're done
                }
            }
            if (s.size () == i) [[likely]] {
                return charCount;
            }
            else {
                return nullopt; // didn't end evenly at end of span, so something went wrong
            }
        }
    }
    template <IUNICODECanUnambiguouslyConvertTo TO, IUNICODECanUnambiguouslyConvertFrom FROM>
    constexpr size_t UTFConvert::ComputeTargetBufferSize (size_t srcSize)
    {
        if constexpr (sizeof (FROM) == sizeof (TO)) {
            if constexpr (same_as<FROM, TO>) {
                return srcSize; // not super useful to do this conversion, but given how if constexpr works/evaluates, its often important than this code compiles, even if it doesn't execute
            }
            if constexpr (same_as<FROM, Latin1> and same_as<TO, char8_t>) {
                return srcSize * 2; // some latin1 characters - such as 0xb5 -  MICRO SIGN - goto two UTF bytes
            }
            return srcSize; // I think this is right, but less certain than before I noticed the Latin1/UTF issue above
        }
        if constexpr (sizeof (FROM) == 1) {
            // worst case is each src byte is a character: for small buffers, not worth computing tighter limit but for larger, could
            // plausibly avoid a malloc, and even without, more likely to avoid wasted RAM/fragmentation for larger allocations
            return srcSize;
        }
        else if constexpr (sizeof (FROM) == 2) {
            if constexpr (sizeof (TO) == 1) {
                // From https://stackoverflow.com/questions/9533258/what-is-the-maximum-number-of-bytes-for-a-utf-8-encoded-character
                // answer if translating only characters from UTF-16 to UTF-8: 4 bytes
                // @todo fix this is really smaller... I think 3 - look at https://en.wikipedia.org/wiki/UTF-8 more closely

                // also - for larger strings - MIGHT be worth a closer estimate?
                return 4 * srcSize;
            }
            else {
                Require (sizeof (TO) == 4);
                return srcSize; // worst case is no surrogate pairs
            }
        }
        else if constexpr (sizeof (FROM) == 4) {
            if constexpr (sizeof (TO) == 1) {
                // From https://stackoverflow.com/questions/9533258/what-is-the-maximum-number-of-bytes-for-a-utf-8-encoded-character
                // the maximum number of bytes for a character in UTF-8 is ... 4 (really 4 safe now so use that - was 6 bytes)
                return 4 * srcSize;
            }
            else if constexpr (sizeof (TO) == 2) {
                return 2 * srcSize;
            }
            else {
                return srcSize;
            }
        }
        else {
            AssertNotReached ();
            return 0;
        }
    }
    template <IUNICODECanUnambiguouslyConvertTo TO, IUNICODECanUnambiguouslyConvertFrom FROM>
    constexpr size_t UTFConvert::ComputeTargetBufferSize (span<const FROM> src)
        requires (not is_const_v<TO>)
    {
        // NOTE - most of these routines could be (easily) optimized to actually compute the number
        // of characters, instead of using an upper bound, but that would involve walking the source
        // and counting characters. NOT clearly worth while. MAYBE as a hueristic, if src > 50 or so
        // its worth counting, but anything smaller will just end up in a fixed sized buffer, so the exact
        // count doesn't matter
        // @small performance todo!!!!

        // This code just checks cases where we peek at characters, then falls through to ComputeTargetBufferSize (size_t)
        if constexpr (sizeof (FROM) != sizeof (TO)) {
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
                }
            }
        }
        return ComputeTargetBufferSize<TO, FROM> (src.size ());
    }
    template <IUNICODECanUnambiguouslyConvertTo TO, IUNICODECanUnambiguouslyConvertFrom FROM>
    constexpr size_t UTFConvert::ComputeTargetBufferSize (span<FROM> src)
        requires (not is_const_v<TO>)
    {
        return ComputeTargetBufferSize<TO> (Memory::ConstSpan (src));
    }
    template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    constexpr bool UTFConvert::AllFitsInTwoByteEncoding (span<const CHAR_T> s) noexcept
    {
        if constexpr (same_as<CHAR_T, ASCII> or same_as<CHAR_T, Latin1>) {
            return true;
        }
        // see https://en.wikipedia.org/wiki/UTF-16
        // @todo - THIS IS VERY WRONG - and MUCH MORE COMPLEX - but will only return false negatives so OK to start

        // note - tried to simplify with conditional_t but both sides evaluated
        if constexpr (same_as<remove_cv_t<CHAR_T>, Character>) {
            for (Character c : s) {
                if (c.GetCharacterCode () > 0xd7ff) [[unlikely]] {
                    return false;
                }
            }
        }
        else if constexpr (same_as<remove_cv_t<CHAR_T>, Latin1>) {
            return true;
        }
        else if constexpr (same_as<remove_cv_t<CHAR_T>, char8_t>) {
            const char8_t* b = s.data ();
            const char8_t* e = b + s.size ();
            for (const char8_t* i = b; i < e;) {
                auto n = NextCharacter (span<const char8_t>{i, e});
                if (not n.has_value () or *n > 2) [[unlikely]] {
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
    template <IUNICODECanUnambiguouslyConvertFrom SRC_T, IUNICODECanUnambiguouslyConvertTo TRG_T>
    inline auto UTFConvert::Convert (span<const SRC_T> source, span<TRG_T> target) const -> ConversionResult
    {
        Require ((target.size () >= ComputeTargetBufferSize<TRG_T> (source)));
        ConversionResultWithStatus result = ConvertQuietly (source, target);
        ThrowIf_ (result.fStatus, result.fSourceConsumed);
        return result; // slice - no need to return 'status' - we throw on any status but success
    }
    template <IUNICODECanUnambiguouslyConvertFrom SRC_T, IUNICODECanUnambiguouslyConvertTo TRG_T>
    inline auto UTFConvert::Convert (span<SRC_T> source, span<TRG_T> target) const -> ConversionResult
    {
        return Convert (Memory::ConstSpan (source), target);
    }
    template <typename TO, typename FROM>
    inline TO UTFConvert::Convert (const FROM& from) const
        requires ((same_as<TO, string> or same_as<TO, wstring> or same_as<TO, u8string> or same_as<TO, u16string> or same_as<TO, u32string>) and
                  (same_as<FROM, string> or same_as<FROM, wstring> or same_as<FROM, u8string> or same_as<FROM, u16string> or same_as<FROM, u32string>))
    {
        if constexpr (same_as<TO, FROM>) {
            return from;
        }
        else {
            size_t                                       cvtBufSize = ComputeTargetBufferSize<typename TO::value_type> (span{from});
            Memory::StackBuffer<typename TO::value_type> buf{Memory::eUninitialized, cvtBufSize};
            return TO{buf.begin (), get<1> (Convert (span{from}, span{buf}))};
        }
    }
    template <IUNICODECanUnambiguouslyConvertFrom SRC_T, IUNICODECanUnambiguouslyConvertTo TRG_T>
    inline span<TRG_T> UTFConvert::ConvertSpan (span<const SRC_T> source, span<TRG_T> target) const
        requires (not is_const_v<TRG_T>)
    {
        return span{target.data (), Convert (source, target).fTargetProduced};
    }
    template <IUNICODECanUnambiguouslyConvertFrom SRC_T, IUNICODECanUnambiguouslyConvertTo TRG_T>
    inline span<TRG_T> UTFConvert::ConvertSpan (span<SRC_T> source, span<TRG_T> target) const
        requires (not is_const_v<TRG_T>)
    {
        return ConvertSpan (Memory::ConstSpan (source), target);
    }
    template <IUNICODECanUnambiguouslyConvertFrom SRC_T, IUNICODECanUnambiguouslyConvertTo TRG_T>
    inline auto UTFConvert::ConvertQuietly (span<const SRC_T> source, span<TRG_T> target) const -> ConversionResultWithStatus
        requires (not is_const_v<TRG_T>)
    {
        Require ((target.size () >= ComputeTargetBufferSize<TRG_T> (source)));
        using PRIMITIVE_SRC_T = typename decltype (this->ConvertToPrimitiveSpan_ (source))::value_type;
        using PRIMITIVE_TRG_T = typename decltype (this->ConvertToPrimitiveSpan_ (target))::value_type;
        if constexpr (same_as<PRIMITIVE_SRC_T, PRIMITIVE_TRG_T> and sizeof (PRIMITIVE_SRC_T) != 1) {
            Memory::CopySpanData_StaticCast (source, target);
            return ConversionResultWithStatus{{.fSourceConsumed = source.size (), .fTargetProduced = source.size ()}, ConversionStatusFlag::ok};
        }
        else if constexpr (same_as<SRC_T, TRG_T>) {
            Memory::CopySpanData_StaticCast (source, target);
            return ConversionResultWithStatus{{.fSourceConsumed = source.size (), .fTargetProduced = source.size ()}, ConversionStatusFlag::ok};
        }
        else if constexpr (same_as<SRC_T, Latin1>) {
            if constexpr (same_as<TRG_T, char8_t>) {
                // Based on https://stackoverflow.com/questions/4059775/convert-iso-8859-1-strings-to-utf-8-in-c-c
                char8_t* outPtr = target.data ();
                for (const SRC_T ch : source) {
                    if (ch < 0x80) {
                        *outPtr++ = ch;
                    }
                    else {
                        *outPtr++ = 0xc0 | ch >> 6;
                        *outPtr++ = 0x80 | (ch & 0x3f);
                    }
                }
                Assert (outPtr <= target.data () + target.size ());
                return ConversionResultWithStatus{
                    {.fSourceConsumed = source.size (), .fTargetProduced = static_cast<size_t> (outPtr - target.data ())}, ConversionStatusFlag::ok};
            }
            else {
                // ALL TRG_T (but UTF8 and ASCII) have Latin1 as a strict subset so simply copy
                Memory::CopySpanData_StaticCast (source, target);
                return ConversionResultWithStatus{{.fSourceConsumed = source.size (), .fTargetProduced = source.size ()}, ConversionStatusFlag::ok};
            }
        }
        else {
            switch (Private_::ValueOf_ (fUsingOptions.fPreferredImplementation)) {
                case Options::Implementation::eStroikaPortable: {
                    return ConvertQuietly_StroikaPortable_ (fUsingOptions.fInvalidCharacterReplacement, ConvertToPrimitiveSpan_ (source),
                                                            ConvertToPrimitiveSpan_ (target));
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
                    if constexpr (same_as<SRC_T, char8_t> and same_as<TRG_T, char16_t>) {
                        return ConvertQuietly_boost_locale_ (ConvertToPrimitiveSpan_ (source), ConvertToPrimitiveSpan_ (target));
                    }
                }
#endif
                case Options::Implementation::eCodeCVT: {
                    if constexpr ((same_as<SRC_T, char16_t> or same_as<SRC_T, char32_t>)and same_as<TRG_T, char8_t>) {
                        return ConvertQuietly_codeCvt_ (source, target);
                    }
                    if constexpr (same_as<SRC_T, char8_t> and (same_as<TRG_T, char16_t> or same_as<SRC_T, char32_t>)) {
                        return ConvertQuietly_codeCvt_ (source, target);
                    }
                }
            }
            return ConvertQuietly_StroikaPortable_ (fUsingOptions.fInvalidCharacterReplacement, ConvertToPrimitiveSpan_ (source),
                                                    ConvertToPrimitiveSpan_ (target)); // default if preferred not available
        }
    }
    template <IUNICODECanUnambiguouslyConvertTo TRG_T, IUNICODECanUnambiguouslyConvertFrom SRC_T>
    size_t UTFConvert::ConvertOffset (span<const SRC_T> source, size_t srcIndex) const
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
    template <IUNICODECanUnambiguouslyConvertFrom FromT>
    constexpr auto UTFConvert::ConvertToPrimitiveSpan_ (span<FromT> f) -> span<CompatibleT_<FromT>>
    {
        return span{(CompatibleT_<FromT>*)f.data (), f.size ()};
    }
#if qPlatform_Windows
    inline auto UTFConvert::ConvertQuietly_Win32_ (span<const char8_t> source, span<char16_t> target) -> ConversionResultWithStatus
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
    inline auto UTFConvert::ConvertQuietly_Win32_ (span<const char16_t> source, span<char8_t> target) -> ConversionResultWithStatus
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
    inline void UTFConvert::ThrowIf_ (ConversionStatusFlag cr, size_t errorAtSourceOffset)
    {
        switch (cr) {
            case ConversionStatusFlag::ok:;
                break;
            default:
                Throw (cr, errorAtSourceOffset);
        }
    }
#if __has_include("boost/locale/encoding_utf.hpp")
    inline auto UTFConvert::ConvertQuietly_boost_locale_ (span<const char8_t> source, const span<char16_t> target) -> ConversionResultWithStatus
    {
        if (source.empty ()) {
            return ConversionResultWithStatus{{0, 0}, ConversionStatusFlag::ok};
        }
        basic_string<char8_t> src = basic_string<char8_t>{source.data (), source.size ()};
        u16string             r   = boost::locale::conv::utf_to_utf<char16_t> (src.c_str ());
        Memory::CopySpanData (span<char16_t>{r}, target);
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
