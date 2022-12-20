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
     *          So - instead - have ComputeOutputBufferSize () API, which quickly computes a reasonable buffer size, and just
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
         *  This will frequently (greatly) over-estimate the amount of space needed.
         *
         *  FROM and TO can be
         *      char
         *      char8_t
         *      char16_t
         *      char32_t
         *      wchar_t
         *
         *  @See ConvertQuietly ()
         *  @See Convert ()
         *  \Alias used to be called ComputeOutputBufferSize
         */
        template <typename FROM, typename TO>
        static constexpr size_t ComputeOutputBufferSize (span<FROM> src);
        template <typename FROM, typename TO>
        static constexpr size_t ComputeOutputBufferSize (const FROM* sourceStart, const FROM* sourceEnd);

    public:
        /**
         *  \brief Convert UTF8 -> UTF16, throw on failure, return tuple of n source/target elements consumed.
         * 
         *  \req size of target span must be at least as large as specified by ComputeOutputBufferSize
         * 
         *  \note overload taking mbstate_t maybe used if converting a large stream in parts which don't necesarily fall on multibyte boundaries.
         * 
         *  Wrapper on ConvertQuietly, that throws when bad source data input, and asserts out when bad target size (insuffiient for buffer).
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
         *          size_t                    cvtBufSize = UTFConverter::ComputeOutputBufferSize<char8_t, wchar_t> (src);
         *          StackBuffer<wchar_t>      buf{Memory::eUninitialized, cvtBufSize};
         *          auto result = UTFConverter::kThe.Convert (src, span<char8_t>{buf});
         *          return String{buf.begin (), buf.begin () + get<2> (result)};
         *      \endcode
         *
         *  @see ConvertQuietly
         */
        nonvirtual tuple<size_t, size_t> Convert (span<const char8_t> source, span<char16_t> target) const;
        nonvirtual tuple<size_t, size_t> Convert (span<const char16_t> source, span<char8_t> target) const;
        nonvirtual tuple<size_t, size_t> Convert (span<const char8_t> source, span<char32_t> target) const;
        nonvirtual tuple<size_t, size_t> Convert (span<const char32_t> source, span<char8_t> target) const;
        nonvirtual tuple<size_t, size_t> Convert (span<const char16_t> source, span<char32_t> target) const;
        nonvirtual tuple<size_t, size_t> Convert (span<const char32_t> source, span<char16_t> target) const;
        nonvirtual tuple<size_t, size_t> Convert (span<const char8_t> source, span<char16_t> target, mbstate_t* multibyteConversionState) const;
        nonvirtual tuple<size_t, size_t> Convert (span<const char8_t> source, span<char32_t> target, mbstate_t* multibyteConversionState) const;
        template <typename SRC_T, typename TRG_T>
        nonvirtual tuple<size_t, size_t> Convert (span<const SRC_T> source, span<TRG_T> target) const
            requires (sizeof (SRC_T) != sizeof (TRG_T));

    public:
        /**
         *  \brief used for ConvertQuietly
         */
        enum class ConversionResults {
            ok,              /* conversion successful */
            sourceExhausted, /* partial character in source, but hit end */
            sourceIllegal    /* source sequence is illegal/malformed */
        };

    public:
        /**
         *  \brief Convert UTF encoded (char8_t, char16_t, char32_t, char, wchar_t) characters to from each other
         * 
         *  \see Convert () above for details. This only differs from Convert, in that it returns a result flag instead
         *       of throwing on errors.
         */
        nonvirtual tuple<ConversionResults, size_t, size_t> ConvertQuietly (span<const char8_t> source, span<char16_t> target) const;
        nonvirtual tuple<ConversionResults, size_t, size_t> ConvertQuietly (span<const char16_t> source, span<char8_t> target) const;
        nonvirtual tuple<ConversionResults, size_t, size_t> ConvertQuietly (span<const char8_t> source, span<char32_t> target) const;
        nonvirtual tuple<ConversionResults, size_t, size_t> ConvertQuietly (span<const char32_t> source, span<char8_t> target) const;
        nonvirtual tuple<ConversionResults, size_t, size_t> ConvertQuietly (span<const char16_t> source, span<char32_t> target) const;
        nonvirtual tuple<ConversionResults, size_t, size_t> ConvertQuietly (span<const char32_t> source, span<char16_t> target) const;
        nonvirtual tuple<ConversionResults, size_t, size_t> ConvertQuietly (span<const char8_t> source, span<char16_t> target, mbstate_t* multibyteConversionState) const;
        nonvirtual tuple<ConversionResults, size_t, size_t> ConvertQuietly (span<const char8_t> source, span<char32_t> target, mbstate_t* multibyteConversionState) const;
        template <typename SRC_T, typename TRG_T>
        nonvirtual tuple<ConversionResults, size_t, size_t> ConvertQuietly (span<const SRC_T> source, span<TRG_T> target) const
            requires (sizeof (SRC_T) != sizeof (TRG_T));

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

    private:
#if qPlatform_Windows
        static tuple<ConversionResults, size_t, size_t> ConvertQuietly_Win32_ (span<const char8_t> source, span<char16_t> target);
        static tuple<ConversionResults, size_t, size_t> ConvertQuietly_Win32_ (span<const char16_t> source, span<char8_t> target);
#endif
    private:
        static tuple<ConversionResults, size_t, size_t> ConvertQuietly_StroikaPortable_ (span<const char8_t> source, const span<char16_t> target);
        static tuple<ConversionResults, size_t, size_t> ConvertQuietly_StroikaPortable_ (span<const char8_t> source, const span<char32_t> target);
        static tuple<ConversionResults, size_t, size_t> ConvertQuietly_StroikaPortable_ (span<const char16_t> source, const span<char32_t> target);
        static tuple<ConversionResults, size_t, size_t> ConvertQuietly_StroikaPortable_ (span<const char32_t> source, const span<char16_t> target);
        static tuple<ConversionResults, size_t, size_t> ConvertQuietly_StroikaPortable_ (span<const char32_t> source, const span<char8_t> target);
        static tuple<ConversionResults, size_t, size_t> ConvertQuietly_StroikaPortable_ (span<const char16_t> source, const span<char8_t> target);

#if __has_include("boost/locale/encoding_utf.hpp")
    private:
        static tuple<ConversionResults, size_t, size_t> ConvertQuietly_boost_locale_ (span<const char8_t> source, const span<char16_t> target);
#endif

    private:
        // this API allows multibyteConversionState == nullptr, even though public APIs don't
        static tuple<ConversionResults, size_t, size_t> ConvertQuietly_codeCvt_ (span<const char8_t> source, span<char16_t> target, mbstate_t* multibyteConversionState);
        static tuple<ConversionResults, size_t, size_t> ConvertQuietly_codeCvt_ (span<const char8_t> source, span<char32_t> target, mbstate_t* multibyteConversionState);
        static tuple<ConversionResults, size_t, size_t> ConvertQuietly_codeCvt_ (span<const char16_t> source, span<char8_t> target);
        static tuple<ConversionResults, size_t, size_t> ConvertQuietly_codeCvt_ (span<const char32_t> source, span<char8_t> target);

    private:
        static void ThrowIf_ (ConversionResults cr);
        static void Throw_ (ConversionResults cr);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "UTFConvert.inl"

#endif /*_Stroika_Foundation_Characters_UTFConvert_h_*/
