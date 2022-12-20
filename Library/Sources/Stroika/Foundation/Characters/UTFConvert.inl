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
#include "../Debug/Assertions.h"
#include "../Memory/Optional.h"

namespace Stroika::Foundation::Characters {

    /*
     ********************************************************************************
     *************************** Characters::UTFConverter ***************************
     ********************************************************************************
     */
    template <>
    inline size_t UTFConverter::QuickComputeConversionOutputBufferSize<char32_t, char16_t> (const char32_t* sourceStart, const char32_t* sourceEnd)
    {
        return (sourceEnd - sourceStart) * 2;
    }
    template <>
    inline size_t UTFConverter::QuickComputeConversionOutputBufferSize<char16_t, char32_t> (const char16_t* sourceStart, const char16_t* sourceEnd)
    {
        return sourceEnd - sourceStart;
    }
    template <>
    inline size_t UTFConverter::QuickComputeConversionOutputBufferSize<char16_t, char> (const char16_t* sourceStart, const char16_t* sourceEnd)
    {
        // From https://stackoverflow.com/questions/9533258/what-is-the-maximum-number-of-bytes-for-a-utf-8-encoded-character
        // answer if translating only characters from UTF-16 to UTF-8: 4 bytes
        return (sourceEnd - sourceStart) * 4;
    }
#if __cpp_char8_t >= 201811L
    template <>
    inline size_t UTFConverter::QuickComputeConversionOutputBufferSize<char16_t, char8_t> (const char16_t* sourceStart, const char16_t* sourceEnd)
    {
        // From https://stackoverflow.com/questions/9533258/what-is-the-maximum-number-of-bytes-for-a-utf-8-encoded-character
        // answer if translating only characters from UTF-16 to UTF-8: 4 bytes
        return (sourceEnd - sourceStart) * 4;
    }
#endif
    template <>
    inline size_t UTFConverter::QuickComputeConversionOutputBufferSize<char, char16_t> (const char* sourceStart, const char* sourceEnd)
    {
        return sourceEnd - sourceStart;
    }
#if __cpp_char8_t >= 201811L
    template <>
    inline size_t UTFConverter::QuickComputeConversionOutputBufferSize<char8_t, char16_t> (const char8_t* sourceStart, const char8_t* sourceEnd)
    {
        return sourceEnd - sourceStart;
    }
#endif
    template <>
    inline size_t UTFConverter::QuickComputeConversionOutputBufferSize<char32_t, char> (const char32_t* sourceStart, const char32_t* sourceEnd)
    {
        // From https://stackoverflow.com/questions/9533258/what-is-the-maximum-number-of-bytes-for-a-utf-8-encoded-character
        // the maximum number of bytes for a character in UTF-8 is ... 6 bytes
        return (sourceEnd - sourceStart) * 6;
    }
#if __cpp_char8_t >= 201811L
    template <>
    inline size_t UTFConverter::QuickComputeConversionOutputBufferSize<char32_t, char8_t> (const char32_t* sourceStart, const char32_t* sourceEnd)
    {
        // From https://stackoverflow.com/questions/9533258/what-is-the-maximum-number-of-bytes-for-a-utf-8-encoded-character
        // the maximum number of bytes for a character in UTF-8 is ... 6 bytes
        return (sourceEnd - sourceStart) * 6;
    }
#endif
    template <>
    inline size_t UTFConverter::QuickComputeConversionOutputBufferSize<char, char32_t> (const char* sourceStart, const char* sourceEnd)
    {
        return sourceEnd - sourceStart;
    }
#if __cpp_char8_t >= 201811L
    template <>
    inline size_t UTFConverter::QuickComputeConversionOutputBufferSize<char8_t, char32_t> (const char8_t* sourceStart, const char8_t* sourceEnd)
    {
        return sourceEnd - sourceStart;
    }
#endif
    template <>
    inline size_t UTFConverter::QuickComputeConversionOutputBufferSize<char, wchar_t> (const char* sourceStart, const char* sourceEnd)
    {
        using ReplaceCharType = conditional_t<sizeof (wchar_t) == sizeof (char16_t), char16_t, char32_t>;
        return QuickComputeConversionOutputBufferSize<char, ReplaceCharType> (sourceStart, sourceEnd);
    }
#if __cpp_char8_t >= 201811L
    template <>
    inline size_t UTFConverter::QuickComputeConversionOutputBufferSize<char8_t, wchar_t> (const char8_t* sourceStart, const char8_t* sourceEnd)
    {
        using ReplaceCharType = conditional_t<sizeof (wchar_t) == sizeof (char16_t), char16_t, char32_t>;
        return QuickComputeConversionOutputBufferSize<char8_t, ReplaceCharType> (sourceStart, sourceEnd);
    }
#endif
    template <>
    inline size_t UTFConverter::QuickComputeConversionOutputBufferSize<char16_t, wchar_t> (const char16_t* sourceStart, const char16_t* sourceEnd)
    {
        using ReplaceCharType = conditional_t<sizeof (wchar_t) == sizeof (char16_t), char16_t, char32_t>;
        return QuickComputeConversionOutputBufferSize<char16_t, ReplaceCharType> (sourceStart, sourceEnd);
    }
    template <>
    inline size_t UTFConverter::QuickComputeConversionOutputBufferSize<char32_t, wchar_t> (const char32_t* sourceStart, const char32_t* sourceEnd)
    {
        using ReplaceCharType = conditional_t<sizeof (wchar_t) == sizeof (char16_t), char16_t, char32_t>;
        return QuickComputeConversionOutputBufferSize<char32_t, ReplaceCharType> (sourceStart, sourceEnd);
    }
    template <>
    inline size_t UTFConverter::QuickComputeConversionOutputBufferSize<wchar_t, char> (const wchar_t* sourceStart, const wchar_t* sourceEnd)
    {
        using ReplaceCharType = conditional_t<sizeof (wchar_t) == sizeof (char16_t), char16_t, char32_t>;
        return QuickComputeConversionOutputBufferSize<ReplaceCharType, char> (reinterpret_cast<const ReplaceCharType*> (sourceStart), reinterpret_cast<const ReplaceCharType*> (sourceEnd));
    }
#if __cpp_char8_t >= 201811L
    template <>
    inline size_t UTFConverter::QuickComputeConversionOutputBufferSize<wchar_t, char8_t> (const wchar_t* sourceStart, const wchar_t* sourceEnd)
    {
        using ReplaceCharType = conditional_t<sizeof (wchar_t) == sizeof (char16_t), char16_t, char32_t>;
        return QuickComputeConversionOutputBufferSize<ReplaceCharType, char8_t> (reinterpret_cast<const ReplaceCharType*> (sourceStart), reinterpret_cast<const ReplaceCharType*> (sourceEnd));
    }
#endif
    template <>
    inline size_t UTFConverter::QuickComputeConversionOutputBufferSize<wchar_t, char16_t> (const wchar_t* sourceStart, const wchar_t* sourceEnd)
    {
        using ReplaceCharType = conditional_t<sizeof (wchar_t) == sizeof (char16_t), char16_t, char32_t>;
        return QuickComputeConversionOutputBufferSize<ReplaceCharType, char16_t> (reinterpret_cast<const ReplaceCharType*> (sourceStart), reinterpret_cast<const ReplaceCharType*> (sourceEnd));
    }
    template <>
    inline size_t UTFConverter::QuickComputeConversionOutputBufferSize<wchar_t, char32_t> (const wchar_t* sourceStart, const wchar_t* sourceEnd)
    {
        using ReplaceCharType = conditional_t<sizeof (wchar_t) == sizeof (char16_t), char16_t, char32_t>;
        return QuickComputeConversionOutputBufferSize<ReplaceCharType, char32_t> (reinterpret_cast<const ReplaceCharType*> (sourceStart), reinterpret_cast<const ReplaceCharType*> (sourceEnd));
    }
    auto UTFConverter::Convert (const span<const char8_t> source, const span<char16_t> target) const -> tuple<size_t, size_t>
    {
        auto result = ConvertQuietly (source, target);
        ThrowIf_ (get<0> (result));
        return make_tuple (get<1> (result), get<2> (result));
    }
    auto UTFConverter::Convert (const span<const char8_t> source, const span<char16_t> target, mbstate_t* multibyteConversionState) const -> tuple<size_t, size_t>
    {
        RequireNotNull (multibyteConversionState);
        auto result = ConvertQuietly (source, target, multibyteConversionState);
        ThrowIf_ (get<0> (result));
        return make_tuple (get<1> (result), get<2> (result));
    }
    auto UTFConverter::ConvertQuietly (const span<const char8_t> source, const span<char16_t> target) const -> tuple<ConversionResults, size_t, size_t>
    {
        switch (Memory::ValueOf (fUsingOptions.fPreferredImplementation)) {
#if qPlatform_Windows
            case Options::Implementation::eWin32Wide2FromMultibyte: {
                return ConvertQuietly_Win32_ (source, target);
            }
#endif
            case Options::Implementation::eStroikaPortable: {
                return ConvertQuietly_StroikaPortable_ (source, target);
            }
            case Options::Implementation::eCodeCVT: {
                return ConvertQuietly_codeCvt_ (source, target, nullptr);
            }
            default: {
                AssertNotReached ();
                return make_tuple (ConversionResults::ok, 0, 0);
            }
        }
    }
    auto UTFConverter::ConvertQuietly (const span<const char8_t> source, const span<char16_t> target, mbstate_t* multibyteConversionState) const -> tuple<ConversionResults, size_t, size_t>
    {
        RequireNotNull (multibyteConversionState);
        switch (Memory::ValueOf (fUsingOptions.fPreferredImplementation)) {
            case Options::Implementation::eCodeCVT: {
                return ConvertQuietly_codeCvt_ (source, target, multibyteConversionState);
            }
            default: {
                AssertNotReached ();
                return make_tuple (ConversionResults::ok, 0, 0);
            }
        }
    }
#if qPlatform_Windows
    inline auto UTFConverter::ConvertQuietly_Win32_ (const span<const char8_t> source, const span<char16_t> target) -> tuple<ConversionResults, size_t, size_t>
    {
        if (source.begin () == source.end ()) {
            return make_tuple (ConversionResults::ok, 0, 0);
        }
        else {
            int srcLen          = static_cast<int> (source.size ());
            int trgLen          = static_cast<int> (target.size ());
            int convertedLength = ::MultiByteToWideChar (CP_UTF8, 0, reinterpret_cast<const char*> (&*source.begin ()), srcLen, reinterpret_cast<WCHAR*> (&*target.begin ()), trgLen);
            return make_tuple (
                convertedLength == 0 ? ConversionResults::ok : ConversionResults::sourceIllegal,
                srcLen, // wag - dont think WideCharToMultiByte tells us how much source consumed
                convertedLength);
        }
    }
    inline auto UTFConverter::ConvertQuietly_Win32_ (const span<const char16_t> source, const span<char8_t> target) -> tuple<ConversionResults, size_t, size_t>
    {
        if (source.begin () == source.end ()) {
            return make_tuple (ConversionResults::ok, 0, 0);
        }
        else {
            int srcLen          = static_cast<int> (source.size ());
            int trgLen          = static_cast<int> (target.size ());
            int convertedLength = ::WideCharToMultiByte (CP_UTF8, 0, reinterpret_cast<const WCHAR*> (&*source.begin ()), srcLen, reinterpret_cast<char*> (&*target.begin ()), trgLen, nullptr, nullptr);
            return make_tuple (
                convertedLength == 0 ? ConversionResults::ok : ConversionResults::sourceIllegal,
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
            case ConversionResults::sourceExhausted: {
                static const auto kException_ = Execution::RuntimeErrorException{L"Invalid UNICODE source string (incomplete UTF character)"sv};
                Execution::Throw (kException_);
            }
            case ConversionResults::sourceIllegal: {
                static const auto kException_ = Execution::RuntimeErrorException{L"Invalid UNICODE source string"sv};
                Execution::Throw (kException_);
            }
            default:
                AssertNotReached ();
        }
    }

}
#endif /*_Stroika_Foundation_Characters_UTFConvert_inl_*/
