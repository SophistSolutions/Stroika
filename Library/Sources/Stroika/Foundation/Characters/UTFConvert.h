/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_UTFConvert_h_
#define _Stroika_Foundation_Characters_UTFConvert_h_ 1

#include "../StroikaPreComp.h"

#include <optional>
#include <span>

#if __has_include("boost/locale/encoding_utf.hpp")
#include <boost/locale/encoding_utf.hpp>
#endif

/**
 *  \file
 *      This module is designed to provide mappings between various UTF encodings of UNICODE characters.</p>
 * 
 *  TODO:
 *      @todo   Cleanup the option 'lenient' support - not respected in thourough way, and the bad-character / missing
 *              character support not thoroughly supported or documented when not. ALL VERY MINOR though.
 */

namespace Stroika::Foundation::Characters {

    /**
     * \brief UTFConverter is designed to provide mappings between various UTF encodings of UNICODE characters.
     * 
     *  This area of C++ is a confusingly broken cluster-fuck. Its pretty simple, and well defined. It's been standardized in C++
     *  as of C++11 (though poorly, but better poor than none). But then that standardized code was deprecated in C++17, and
     *  I BELIEVE still not replaced functionlity as of C++23 (I've searched, but no luck).
     * 
     *  Available (sensible) implementations:
     *      o   std C++ code_cvt    (deprecated, and on windows, slow, but DOES support mbstate_t)
     *      o   Boost no-wide       (untested so not sure about this)
     *      o   Windows API         (appears most performant, but doesn't support mbstate_t)
     *      o   Stroika portable implementation, based on libutfxx (slow but portable, and works, NOT supporting mbstate_t
     * 
     *  Design Choices:
     *      o   Could have API to COMPUTE size of output buffer. But thats as much work to compute as actually doing the conversion (generally close).
     *          So - instead - have ComputeTargetBufferSize () API, which quickly computes a reasonable buffer size, and just
     *          assert we never run out of space. Not a great plan, but probably pretty good, most of the time.
     * 
     *  Though you can construct your own UTFConverter with different options, a typical application will just use
     *      \code
     *          UTFConverter::kThe
     *      \endcode
     */
    class UTFConverter final {
    public:
        /**
         */
        struct Options {
            /**
             *  if fStrictMode, then throw exceptions for bad data (if the underlying algorithm allows). If not, just
             *  silently skip or use xxx character.
             */
            bool fStrictMode{false};

            /**
             *  Different implementations of UTF character conversion
             */
            enum class Implementation {

                // Based on libutfxx
                eStroikaPortable,

#if __has_include("boost/locale/encoding_utf.hpp")
                // unteseted/unimplmeneted so far
                eBoost_Locale,
#endif

#if qPlatform_Windows
                // Seems fastest for windows
                eWin32Wide2FromMultibyte,
#endif

                // Deprecated by stdc++, and windows implementation appears quite slow, but only one supporting mbstate_t
                eCodeCVT,
            };

            /**
             *  \note that since not all implementations support all APIs, this is just a hint. Other implementations maybe
             *        used as needed.
             */
            optional<Implementation> fPreferredImplementation;
        };

    public:
        /**
         */
#if qCompilerAndStdLib_DefaultMemberInitializerNeededEnclosingForDefaultFunArg_Buggy
        constexpr UTFConverter ();
        constexpr UTFConverter (const Options& options);
#else
        constexpr UTFConverter (const Options& options = Options{});
#endif

    public:
        /*
         *  Quickly compute the buffer size needed for a call to Convert
         *
         *  This will frequently (greatly) over-estimate the amount of space needed but it will always produce a sufficient answer without much computation.
         *
         *  FROM and TO can be
         *      char
         *      char8_t
         *      char16_t
         *      char32_t
         *      wchar_t
         * 
         *  \note buffer size NOT in 'bytes' but in units of 'TO' - so char32_t, or char8_t, or whatever.
         * 
         *  \note future implementations might do more work to compute a sometimes smaller, less wasteful buffer size. Maybe
         *        look at size, and if small just overestimate, but if input span is large, maybe worth the trouble and count
         *        multibyte characters?
         *
         *  @See ConvertQuietly ()
         *  @See Convert ()
         *  \Alias used to be called QuickComputeConversionOutputBufferSize
         */
        template <typename TO, typename FROM>
        static constexpr size_t ComputeTargetBufferSize (span<const FROM> src);

    public:
        /**
         *  Result of Convert() call - saying how much of the source was consumed, and how many units of the target were produced.
         *  units depend on the call, char8_ts, or char16_ts, or char32_ts.
         */
        struct ConversionResult {
            size_t fSourceConsumed{};
            size_t fTargetProduced{};
        };

    public:
        /**
         *  \brief Convert UTFN -> UTFM (e.g. UTF8 to UTF32), throw on failure, return tuple of n source/target elements consumed/produced.
         * 
         *  \req size of target span must be at least as large as specified by ComputeTargetBufferSize
         * 
         *  \note overload taking mbstate_t maybe used if converting a large stream in parts which don't necesarily fall on multibyte boundaries.
         * 
         *  Wrapper on ConvertQuietly, that throws when bad source data input, and asserts out when bad target size (insuffient for buffer).
         *
         *  Convert these combinations:
         *      o   char8_t
         *      o   char16_t
         *      o   char32_t
         *  to/from each other (NOT char8_t to char8_t).
         * 
         *  Variations from char8_t are overloaded to optionally take a multibyteConversionState parameter.
         * 
         *  The types
         *      o   char
         *      o   wchar_t
         *  are mapped to the appropriate above type.
         * 
         *  \par Example Usage
         *      \code
         *          size_t                    cvtBufSize = UTFConverter::ComputeTargetBufferSize<wchar_t> (src);
         *          StackBuffer<wchar_t>      buf{Memory::eUninitialized, cvtBufSize};
         *          auto result = UTFConverter::kThe.Convert (src, span<wchar_t>{buf});
         *          return String{buf.begin (), buf.begin () + result.fTargetProduced};
         *      \endcode
         *
         *  @see ConvertQuietly for span overloads
         * 
         *  String overloads are simple wrappers on the span code but with simpler to use arguments
         * 
         *  \par Example Usage
         *      \code
         *          wstring wide_fred = UTFConverter::kThe.Convert<wstring> (u8"fred");
         *          u16string u16_fred = UTFConverter::kThe.Convert<u16string> (u32"fred");
         *      \endcode
         *
         */
        nonvirtual ConversionResult Convert (span<const char8_t> source, span<char16_t> target) const;
        nonvirtual ConversionResult Convert (span<const char16_t> source, span<char8_t> target) const;
        nonvirtual ConversionResult Convert (span<const char8_t> source, span<char32_t> target) const;
        nonvirtual ConversionResult Convert (span<const char32_t> source, span<char8_t> target) const;
        nonvirtual ConversionResult Convert (span<const char16_t> source, span<char32_t> target) const;
        nonvirtual ConversionResult Convert (span<const char32_t> source, span<char16_t> target) const;
        nonvirtual ConversionResult Convert (span<const char8_t> source, span<char16_t> target, mbstate_t* multibyteConversionState) const;
        nonvirtual ConversionResult Convert (span<const char8_t> source, span<char32_t> target, mbstate_t* multibyteConversionState) const;
        template <typename SRC_T, typename TRG_T>
        nonvirtual ConversionResult Convert (span<const SRC_T> source, span<TRG_T> target) const
            requires (not is_const_v<TRG_T>);
        template <typename TO, typename FROM>
        nonvirtual TO Convert (const FROM& from) const
            requires (
                (is_same_v<TO, string> or is_same_v<TO, wstring> or is_same_v<TO, u8string> or is_same_v<TO, u16string> or is_same_v<TO, u32string>) and
                (is_same_v<FROM, string> or is_same_v<FROM, wstring> or is_same_v<FROM, u8string> or is_same_v<FROM, u16string> or is_same_v<FROM, u32string>));

    public:
        /**
         *  \brief used for ConvertQuietly
         * 
         *  \note no need to have status code for 'targetExhausted' because we assert error in that case. DONT DO IT.
         */
        enum class ConversionStatusFlag {
            ok,              /* conversion successful */
            sourceExhausted, /* partial character in source, but hit end */
            sourceIllegal    /* source sequence is illegal/malformed */
        };

    public:
        /**
         */
        struct ConversionResultWithStatus : ConversionResult {
            ConversionStatusFlag fStatus{};
        };

    public:
        /**
         *  \brief Convert UTF encoded (char8_t, char16_t, char32_t, char, wchar_t) characters to from each other
         * 
         *  \see Convert () above for details. This only differs from Convert, in that it returns a result flag instead
         *       of throwing on errors.
         */
        nonvirtual ConversionResultWithStatus ConvertQuietly (span<const char8_t> source, span<char16_t> target) const;
        nonvirtual ConversionResultWithStatus ConvertQuietly (span<const char16_t> source, span<char8_t> target) const;
        nonvirtual ConversionResultWithStatus ConvertQuietly (span<const char8_t> source, span<char32_t> target) const;
        nonvirtual ConversionResultWithStatus ConvertQuietly (span<const char32_t> source, span<char8_t> target) const;
        nonvirtual ConversionResultWithStatus ConvertQuietly (span<const char16_t> source, span<char32_t> target) const;
        nonvirtual ConversionResultWithStatus ConvertQuietly (span<const char32_t> source, span<char16_t> target) const;
        nonvirtual ConversionResultWithStatus ConvertQuietly (span<const char8_t> source, span<char16_t> target, mbstate_t* multibyteConversionState) const;
        nonvirtual ConversionResultWithStatus ConvertQuietly (span<const char8_t> source, span<char32_t> target, mbstate_t* multibyteConversionState) const;
        template <typename SRC_T, typename TRG_T>
        nonvirtual ConversionResultWithStatus ConvertQuietly (span<const SRC_T> source, span<TRG_T> target) const
            requires (not is_const_v<TRG_T>);

    public:
        /**
         *  \brief Nearly always use this default UTFConverter.
         */
        static const UTFConverter kThe;

    private:
        // find same size, and then remove_const, and then add back const
        template <typename SRC_OF_CONSTNESS_T, typename TYPE_T>
        using AddConstIfMatching_ = conditional_t<is_const_v<SRC_OF_CONSTNESS_T>, add_const_t<TYPE_T>, TYPE_T>;
        template <typename SRC_T>
        using MapSizes_ = conditional_t<sizeof (SRC_T) == 1, char8_t, conditional_t<sizeof (SRC_T) == 2, char16_t, char32_t>>;
        template <typename SRC_T>
        using CompatibleT_ = AddConstIfMatching_<SRC_T, MapSizes_<SRC_T>>;

    private:
        // need generic way to convert char to char8_t, and wchar_t to char16_t or char32_t
        template <typename FromT>
        static constexpr span<CompatibleT_<FromT>> ConvertCompatibleSpan_ (span<FromT> f);

    private:
        Options fOriginalOptions_;
        Options fUsingOptions;

#if qPlatform_Windows
    private:
        static ConversionResultWithStatus ConvertQuietly_Win32_ (span<const char8_t> source, span<char16_t> target);
        static ConversionResultWithStatus ConvertQuietly_Win32_ (span<const char16_t> source, span<char8_t> target);
#endif
    private:
        static ConversionResultWithStatus ConvertQuietly_StroikaPortable_ (span<const char8_t> source, span<char16_t> target);
        static ConversionResultWithStatus ConvertQuietly_StroikaPortable_ (span<const char8_t> source, span<char32_t> target);
        static ConversionResultWithStatus ConvertQuietly_StroikaPortable_ (span<const char16_t> source, span<char32_t> target);
        static ConversionResultWithStatus ConvertQuietly_StroikaPortable_ (span<const char32_t> source, span<char16_t> target);
        static ConversionResultWithStatus ConvertQuietly_StroikaPortable_ (span<const char32_t> source, span<char8_t> target);
        static ConversionResultWithStatus ConvertQuietly_StroikaPortable_ (span<const char16_t> source, span<char8_t> target);

#if __has_include("boost/locale/encoding_utf.hpp")
    private:
        static ConversionResultWithStatus ConvertQuietly_boost_locale_ (span<const char8_t> source, const span<char16_t> target);
#endif

    private:
        // this API allows multibyteConversionState == nullptr, even though public APIs don't
        static ConversionResultWithStatus ConvertQuietly_codeCvt_ (span<const char8_t> source, span<char16_t> target, mbstate_t* multibyteConversionState);
        static ConversionResultWithStatus ConvertQuietly_codeCvt_ (span<const char8_t> source, span<char32_t> target, mbstate_t* multibyteConversionState);
        static ConversionResultWithStatus ConvertQuietly_codeCvt_ (span<const char16_t> source, span<char8_t> target);
        static ConversionResultWithStatus ConvertQuietly_codeCvt_ (span<const char32_t> source, span<char8_t> target);

    private:
        static void ThrowIf_ (ConversionStatusFlag cr);
        static void Throw_ (ConversionStatusFlag cr);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "UTFConvert.inl"

#endif /*_Stroika_Foundation_Characters_UTFConvert_h_*/
