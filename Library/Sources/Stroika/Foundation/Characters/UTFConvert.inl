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

    template <typename TO, typename FROM>
    constexpr size_t UTFConverter::ComputeOutputBufferSize (span<const FROM> src)
    {
        if constexpr (sizeof (FROM) == sizeof (TO)) {
            return src.size (); // not super useful to do this conversion, but given how if constexpr works/evaluates, its often important than this code compiles, even if it doesn't execute
        }
        if constexpr (sizeof (FROM) == 1) {
            // worst case is each src byte is a character
            return src.size ();
        }
        else if constexpr (sizeof (FROM) == 2) {
            if constexpr (sizeof (TO) == 1) {
                // From https://stackoverflow.com/questions/9533258/what-is-the-maximum-number-of-bytes-for-a-utf-8-encoded-character
                // answer if translating only characters from UTF-16 to UTF-8: 4 bytes
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

    inline auto UTFConverter::Convert (span<const char8_t> source, span<char16_t> target) const -> tuple<size_t, size_t>
    {
        Require ((target.size () >= ComputeOutputBufferSize<char16_t> (source)));
        auto result = ConvertQuietly (source, target);
        ThrowIf_ (get<0> (result));
        return make_tuple (get<1> (result), get<2> (result));
    }
    inline auto UTFConverter::Convert (span<const char16_t> source, span<char8_t> target) const -> tuple<size_t, size_t>
    {
        Require ((target.size () >= ComputeOutputBufferSize<char8_t> (source)));
        auto result = ConvertQuietly (source, target);
        ThrowIf_ (get<0> (result));
        return make_tuple (get<1> (result), get<2> (result));
    }
    inline auto UTFConverter::Convert (span<const char8_t> source, span<char32_t> target) const -> tuple<size_t, size_t>
    {
        Require ((target.size () >= ComputeOutputBufferSize<char32_t> (source)));
        auto result = ConvertQuietly (source, target);
        ThrowIf_ (get<0> (result));
        return make_tuple (get<1> (result), get<2> (result));
    }
    inline auto UTFConverter::Convert (span<const char32_t> source, span<char8_t> target) const -> tuple<size_t, size_t>
    {
        Require ((target.size () >= ComputeOutputBufferSize<char8_t> (source)));
        auto result = ConvertQuietly (source, target);
        ThrowIf_ (get<0> (result));
        return make_tuple (get<1> (result), get<2> (result));
    }
    inline auto UTFConverter::Convert (span<const char16_t> source, span<char32_t> target) const -> tuple<size_t, size_t>
    {
        Require ((target.size () >= ComputeOutputBufferSize<char32_t> (source)));
        auto result = ConvertQuietly (source, target);
        ThrowIf_ (get<0> (result));
        return make_tuple (get<1> (result), get<2> (result));
    }
    inline auto UTFConverter::Convert (span<const char32_t> source, span<char16_t> target) const -> tuple<size_t, size_t>
    {
        Require ((target.size () >= ComputeOutputBufferSize<char16_t> (source)));
        auto result = ConvertQuietly (source, target);
        ThrowIf_ (get<0> (result));
        return make_tuple (get<1> (result), get<2> (result));
    }
    inline auto UTFConverter::Convert (span<const char8_t> source, span<char16_t> target, mbstate_t* multibyteConversionState) const -> tuple<size_t, size_t>
    {
        Require ((target.size () >= ComputeOutputBufferSize<char16_t> (source)));
        RequireNotNull (multibyteConversionState);
        auto result = ConvertQuietly (source, target, multibyteConversionState);
        ThrowIf_ (get<0> (result));
        return make_tuple (get<1> (result), get<2> (result));
    }
    inline auto UTFConverter::Convert (span<const char8_t> source, span<char32_t> target, mbstate_t* multibyteConversionState) const -> tuple<size_t, size_t>
    {
        Require ((target.size () >= ComputeOutputBufferSize<char32_t> (source)));
        RequireNotNull (multibyteConversionState);
        auto result = ConvertQuietly (source, target, multibyteConversionState);
        ThrowIf_ (get<0> (result));
        return make_tuple (get<1> (result), get<2> (result));
    }
    template <typename SRC_T, typename TRG_T>
    inline tuple<size_t, size_t> UTFConverter::Convert (span<const SRC_T> source, span<TRG_T> target) const
        requires (not is_const_v<TRG_T>)
    {
        Require ((target.size () >= ComputeOutputBufferSize<TRG_T> (source)));
        if constexpr (sizeof (SRC_T) == sizeof (TRG_T)) {
            copy (source, target, source.size ()); // pointless conversion, but if requested...
            return source.size ();
        }
        return Convert (ConvertCompatibleSpan_ (source), ConvertCompatibleSpan_ (target));
    }
    template <typename TO, typename FROM>
    inline TO UTFConverter::Convert (const FROM& from) const
        requires (
            (is_same_v<TO, string> or is_same_v<TO, wstring> or is_same_v<TO, u8string> or is_same_v<TO, u16string> or is_same_v<TO, u32string>) and
            (is_same_v<FROM, string> or is_same_v<FROM, wstring> or is_same_v<FROM, u8string> or is_same_v<FROM, u16string> or is_same_v<FROM, u32string>))
    {
        if constexpr (is_same_v<TO, FROM>) {
            return from;
        }
        else {
            size_t                                       cvtBufSize = ComputeOutputBufferSize<typename TO::value_type> (span{from});
            Memory::StackBuffer<typename TO::value_type> buf{Memory::eUninitialized, cvtBufSize};
            return TO{buf.begin (), get<1> (Convert (span{from}, span{buf}))};
        }
    }

    inline auto UTFConverter::ConvertQuietly (span<const char8_t> source, span<char16_t> target) const -> tuple<ConversionResults, size_t, size_t>
    {
        Require ((target.size () >= ComputeOutputBufferSize<char16_t> (source)));
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
    inline auto UTFConverter::ConvertQuietly (span<const char16_t> source, span<char8_t> target) const -> tuple<ConversionResults, size_t, size_t>
    {
        Require ((target.size () >= ComputeOutputBufferSize<char8_t> (source)));
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
    inline auto UTFConverter::ConvertQuietly (span<const char8_t> source, span<char32_t> target) const -> tuple<ConversionResults, size_t, size_t>
    {
        Require ((target.size () >= ComputeOutputBufferSize<char32_t> (source)));
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
    inline auto UTFConverter::ConvertQuietly (span<const char32_t> source, span<char8_t> target) const -> tuple<ConversionResults, size_t, size_t>
    {
        Require ((target.size () >= ComputeOutputBufferSize<char8_t> (source)));
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
    inline auto UTFConverter::ConvertQuietly (span<const char16_t> source, span<char32_t> target) const -> tuple<ConversionResults, size_t, size_t>
    {
        Require ((target.size () >= ComputeOutputBufferSize<char32_t> (source)));
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
    inline auto UTFConverter::ConvertQuietly (span<const char32_t> source, span<char16_t> target) const -> tuple<ConversionResults, size_t, size_t>
    {
        Require ((target.size () >= ComputeOutputBufferSize<char16_t> (source)));
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
    inline auto UTFConverter::ConvertQuietly (span<const char8_t> source, span<char16_t> target, mbstate_t* multibyteConversionState) const -> tuple<ConversionResults, size_t, size_t>
    {
        Require ((target.size () >= ComputeOutputBufferSize<char16_t> (source)));
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
    inline auto UTFConverter::ConvertQuietly (span<const char8_t> source, span<char32_t> target, mbstate_t* multibyteConversionState) const -> tuple<ConversionResults, size_t, size_t>
    {
        Require ((target.size () >= ComputeOutputBufferSize<char32_t> (source)));
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
    template <typename SRC_T, typename TRG_T>
    inline auto UTFConverter::ConvertQuietly (span<const SRC_T> source, span<TRG_T> target) const -> tuple<ConversionResults, size_t, size_t>
        requires (not is_const_v<TRG_T>)
    {
        if constexpr (sizeof (SRC_T) == sizeof (TRG_T)) {
            copy (source, target, source.size ()); // pointless conversion, but if requested...
            return source.size ();
        }
        return ConvertQuietly (ConvertCompatibleSpan_ (source), ConvertCompatibleSpan_ (target));
    }

    template <typename FromT>
    constexpr auto UTFConverter::ConvertCompatibleSpan_ (span<FromT> f) -> span<CompatibleT_<FromT>>
    {
        if (f.empty ()) {
            return span<CompatibleT_<FromT>>{}; // avoid the appearance of dereferencing an empty span (important for msvc runtime)
        }
        return span{(CompatibleT_<FromT>*)&*f.begin (), f.size ()};
    }

#if qPlatform_Windows
    inline auto UTFConverter::ConvertQuietly_Win32_ (span<const char8_t> source, span<char16_t> target) -> tuple<ConversionResults, size_t, size_t>
    {
        if (source.begin () == source.end ()) {
            return make_tuple (ConversionResults::ok, 0, 0);
        }
        else {
            int srcLen          = static_cast<int> (source.size ());
            int trgLen          = static_cast<int> (target.size ());
            int convertedLength = ::MultiByteToWideChar (CP_UTF8, 0, reinterpret_cast<const char*> (&*source.begin ()), srcLen, reinterpret_cast<WCHAR*> (&*target.begin ()), trgLen);
            return make_tuple (
                convertedLength == 0 ? ConversionResults::sourceIllegal : ConversionResults::ok,
                srcLen, // wag - dont think WideCharToMultiByte tells us how much source consumed
                convertedLength);
        }
    }
    inline auto UTFConverter::ConvertQuietly_Win32_ (span<const char16_t> source, span<char8_t> target) -> tuple<ConversionResults, size_t, size_t>
    {
        if (source.begin () == source.end ()) {
            return make_tuple (ConversionResults::ok, 0, 0);
        }
        else {
            int srcLen          = static_cast<int> (source.size ());
            int trgLen          = static_cast<int> (target.size ());
            int convertedLength = ::WideCharToMultiByte (CP_UTF8, 0, reinterpret_cast<const WCHAR*> (&*source.begin ()), srcLen, reinterpret_cast<char*> (&*target.begin ()), trgLen, nullptr, nullptr);
            return make_tuple (
                convertedLength == 0 ? ConversionResults::sourceIllegal : ConversionResults::ok,
                srcLen, // wag - dont think WideCharToMultiByte tells us how much source consumed
                convertedLength);
        }
    }
#endif
    inline void UTFConverter::ThrowIf_ (ConversionResults cr)
    {
        switch (cr) {
            case ConversionResults::ok:;
                break;
            default:
                Throw_ (cr);
        }
    }
#if __has_include("boost/locale/encoding_utf.hpp")
    inline auto UTFConverter::ConvertQuietly_boost_locale_ (span<const char8_t> source, const span<char16_t> target) -> tuple<ConversionResults, size_t, size_t>
    {
        if (source.empty ()) {
            return make_tuple (ConversionResults::ok, 0, 0);
        }
        basic_string<char8_t> src = basic_string<char8_t>{reinterpret_cast<const char8_t*> (&*source.begin ()), reinterpret_cast<const char8_t*> (&*source.begin ()) + source.size ()};
        u16string             r   = boost::locale::conv::utf_to_utf<char16_t> (src.c_str ());
        copy (r.begin (), r.end (), target.begin ());
        return make_tuple (ConversionResults::ok, source.size (), r.size ());
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
        return make_tuple (ConversionResults::ok, 0, 0);
    }
#endif

}
#endif /*_Stroika_Foundation_Characters_UTFConvert_inl_*/
