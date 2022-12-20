/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_UTFConvert_h_
#define _Stroika_Foundation_Characters_UTFConvert_h_ 1

#include "../StroikaPreComp.h"

#include <span>

/**
 *  \file
 *      This module is designed to provide mappings between various UTF encodings of UNICODE characters.</p>
 */

namespace Stroika::Foundation::Characters {

    /**
     * \brief UTFConverter is designed to provide mappings between various UTF encodings of UNICODE characters.
     * 
     *  This area of C++ is a confusingly broken cluster-fuck. Its pretty simple, and well defined. It's been standardized in C++
     *  as of C++11 (though poorly, but better poor than none)
     * 
     *  Available (sensible) implementations:
     *      o   std C++ code_cvt    (deprecated, and on windows, slow, but DOES support mbstate_t)
     *      o   Boost no-wide       (untested so not sure about this)
     *      o   Windows API         (appears most performant, but doesn't support mbstate_t)
     *      o   Stroika portable implementation, based on libutfxx (slow but portable, and works, NOT supporting mbstate_t
     */
    class UTFConverter {
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

#if 0
                // unteseted/unimplmeneted so far
                eBoostNoWide,
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
        constexpr UTFConverter (const Options& options = Options{})
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

    private:
        Options fOriginalOptions_;
        Options fUsingOptions;

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
         */
        template <typename FROM, typename TO>
        static size_t QuickComputeConversionOutputBufferSize (const FROM* sourceStart, const FROM* sourceEnd);

    public:
        enum class ConversionResults {
            ok,              /* conversion successful */
            sourceExhausted, /* partial character in source, but hit end */
            sourceIllegal    /* source sequence is illegal/malformed */
        };

    public:
        /**
         *  \brief Convert UTF8 -> UTF16, throw on failure, return tuple of n source/target elements consumed.
         * 
         *  \req size of target span large enuf  (SEE XXX FOR REQUIREMNET)
         * 
         *  \note overload taking mbstate_t maybe used if converting a large stream in parts which don't necesarily fall on multibyte boundaries.
         */
        nonvirtual tuple<size_t, size_t> Convert (const span<const char8_t> source, const span<char16_t> target) const;
        nonvirtual tuple<size_t, size_t> Convert (const span<const char8_t> source, const span<char16_t> target, mbstate_t* multibyteConversionState) const;

    public:
        /**
         *  \brief Convert UTF8 -> UTF16, returning flag of success/failure, and amount of input/output converted
         * 
         *  \note overload taking mbstate_t maybe used if converting a large stream in parts which don't necesarily fall on multibyte boundaries.
         */
        nonvirtual tuple<ConversionResults, size_t, size_t> ConvertQuietly (const span<const char8_t> source, const span<char16_t> target) const;
        nonvirtual tuple<ConversionResults, size_t, size_t> ConvertQuietly (const span<const char8_t> source, const span<char16_t> target, mbstate_t* multibyteConversionState) const;

    public:
        static const UTFConverter kThe;

    private:
#if qPlatform_Windows
        static tuple<ConversionResults, size_t, size_t> ConvertQuietly_Win32_ (const span<const char8_t> source, const span<char16_t> target);
        static tuple<ConversionResults, size_t, size_t> ConvertQuietly_Win32_ (const span<const char16_t> source, const span<char8_t> target);
#endif
    private:
        static tuple<ConversionResults, size_t, size_t> ConvertQuietly_StroikaPortable_ (const span<const char8_t> source, const span<char16_t> target);
        static tuple<ConversionResults, size_t, size_t> ConvertQuietly_StroikaPortable_ (const span<const char16_t> source, const span<char8_t> target);

    private:
        // this API allows multibyteConversionState == nullptr, even though public APIs don't
        static tuple<ConversionResults, size_t, size_t> ConvertQuietly_codeCvt_ (const span<const char8_t> source, const span<char16_t> target, mbstate_t* multibyteConversionState);
        static tuple<ConversionResults, size_t, size_t> ConvertQuietly_codeCvt_ (const span<const char16_t> source, const span<char8_t> target);

    private:
        static void ThrowIf_ (ConversionResults cr);
    };
    inline constexpr UTFConverter UTFConverter::kThe;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "UTFConvert.inl"

#endif /*_Stroika_Foundation_Characters_UTFConvert_h_*/
