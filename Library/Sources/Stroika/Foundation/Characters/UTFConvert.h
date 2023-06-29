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

#include "Character.h"

/**
 *  \file
 *      This module is designed to provide mappings between various UTF encodings of UNICODE characters.
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
     *  I BELIEVE still not replaced functionlity as of C++23 (I've searched, but no luck). (MAYBE WRONG - just renamed char to char8_t
     *  but docs in cppreference dont make that clear; anyhow still a mess).
     * 
     *  Available (plausible) implementations:
     *      o   std C++ code_cvt        (deprecated, and on windows, slow)
     *      o   Boost locale utf_to_utf (untested so not sure about this)
     *      o   Windows API             (appears most performant)
     *      o   Stroika portable implementation, based on libutfxx (slow but portable, and works)
     *      o   nemtrif/utfcpp (haven't tried yet)
     *      o   simdutf (allegedly fastest, but haven't tried yet)
     * 
     *  Design Choices:
     *      o   Could have API to COMPUTE size of output buffer. But thats as much work to compute as actually doing the conversion (generally close).
     *          So - instead - have ComputeTargetBufferSize () API, which quickly computes a reasonable buffer size, and just
     *          assert we never run out of space. Not a great plan, but probably pretty good, most of the time.
     * 
     *          API setup so the compute-buf-size routine COULD walk the source and compute the exact needed size, without changing API.
     * 
     *  \note Byte Order Markers
     *      UTFConverter does NOT support byte order marks (BOM) - for that - see Streams::TextReader, and Streams::TextWriter
     *
     *  \notes about mbstate_t
     *      mbstate_t is used by the std::codecvt apis and nothing else, and seems opaque and not any obvious use, so just
     *      not used in this API, and faked when needed for codecvt.
     * 
     *  Web Pages/ Specs:
     *      o   https://en.wikipedia.org/wiki/UTF-8
     *      o   https://en.wikipedia.org/wiki/UTF-16
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
                eWindowsAPIWide2FromMultibyte,
#endif

                // CPPREFERENCE docs on std::codecvt somewhat confusing about what is deprected, but it appears the interface as a whole is still viable as of C++20
                // and just several sub-interfaces are deprecated -- LGP 2023-06-28
                eCodeCVT,

                // @todo LIBS TO LOOK AT
                //      https://github.com/nemtrif/utfcpp
                //      https://github.com/simdutf/simdutf      ((probably best/fastest - so try))
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
         *  Quickly compute the buffer size needed for a call to Convert (or ConvertSpan)
         *
         *  This will frequently (greatly) over-estimate the amount of space needed but it will always produce a sufficient answer without much computation.
         *
         *  \note buffer size NOT in 'bytes' but in units of 'TO' - so char32_t, or char8_t, or whatever.
         * 
         *  \note future implementations might do more work to compute a sometimes smaller, less wasteful buffer size. Maybe
         *        look at size, and if small just overestimate, but if input span is large, maybe worth the trouble and count
         *        multibyte characters?
         *
         *  For the overload taking a size, and not the actual FROM span, this computes the upper bound size required.
         * 
         *  @See ConvertQuietly ()
         *  @See Convert ()
         *  \Alias used to be called QuickComputeConversionOutputBufferSize
         */
        template <IUNICODECanUnambiguouslyConvertFrom TO, IUNICODECanUnambiguouslyConvertFrom FROM>
        static constexpr size_t ComputeTargetBufferSize (span<const FROM> src)
            requires (not is_const_v<TO>);
        template <IUNICODECanUnambiguouslyConvertFrom TO, IUNICODECanUnambiguouslyConvertFrom FROM>
        static constexpr size_t ComputeTargetBufferSize (span<FROM> src)
            requires (not is_const_v<TO>);
        template <IUNICODECanUnambiguouslyConvertFrom TO, IUNICODECanUnambiguouslyConvertFrom FROM>
        static constexpr size_t ComputeTargetBufferSize (size_t srcSize);

    public:
        /**
         *  Check if each character in the span fits in a 2-byte encoding (ie no UTF-16 surrogate pairs)
         */
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        static constexpr bool AllFitsInTwoByteEncoding (span<const CHAR_T> s) noexcept;

    public:
        /**
         *  \brief Convert between UTF-N encoded (including the special case of ASCII, and Latin1) character spans (e.g. UTF8 to UTF32), throw on failure, resulting span<>.
         * 
         *  Compared with the Convert () API, this loses information (number of source characters consumed). 
         *  Not a general purpose API. But very frequently this is all you need, for the next stage, a new span, 
         *  and for that case, this saves a little typing.
         * 
         *  NOTE - the returned span is ALWAYS (not necessarily proper) sub-span of its 'target' argument
         *
         *  \par Example Usage
         *      \code
         *          StackBuffer<wchar_t>      buf{Memory::eUninitialized, UTFConverter::ComputeTargetBufferSize<wchar_t> (src)};
         *          span<wchar_t> spanOfTargetBufferUsed = UTFConverter::kThe.ConvertSpan (src, span{buf});
         *          return String{spanOfTargetBufferUsed};
         *      \endcode
         */
        template <IUNICODECanUnambiguouslyConvertFrom SRC_T, IUNICODECanUnambiguouslyConvertFrom TRG_T>
        nonvirtual span<TRG_T> ConvertSpan (span<const SRC_T> source, span<TRG_T> target) const
            requires (not is_const_v<TRG_T>);
        template <IUNICODECanUnambiguouslyConvertFrom SRC_T, IUNICODECanUnambiguouslyConvertFrom TRG_T>
        nonvirtual span<TRG_T> ConvertSpan (span<SRC_T> source, span<TRG_T> target) const
            requires (not is_const_v<TRG_T>);

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
         *  \brief Convert between UTF-N encoded strings/spans (including the special case of ASCII, and Latin1) (e.g. UTF8 to UTF32), throw on failure
         * 
         *  \note Failures
         *      Failures can be because of illegal input code-point or source exhuasted. If you want to deal with partial characters,
         *      you must use the ConvertQuietly API.
         * 
         *  For overloads taking a target span:
         *      \req size of target span must be at least as large as specified by ComputeTargetBufferSize
         * 
         *  Wrapper on ConvertQuietly, that throws when bad source data input, and asserts out when bad target size (insuffient for buffer).
         *
         *  Variations from char8_t are overloaded to optionally take a multibyteConversionState parameter.
         * 
         *  The types
         *      o   char
         *      o   wchar_t
         *  are mapped to the appropriate above type.
         * 
         *  Source and target spans can be of any IUNICODECanUnambiguouslyConvertFrom character type (but source const and target non-const)
         *  (or basic_string of said)...
         * 
         *  \par Example Usage
         *      \code
         *          StackBuffer<wchar_t>      buf{Memory::eUninitialized, UTFConverter::ComputeTargetBufferSize<wchar_t> (src)};
         *          auto result = UTFConverter::kThe.Convert (src, span{buf});
         *          return String{buf.begin (), buf.begin () + result.fTargetProduced}; // OR better yet see ConvertSpan
         *      \endcode
         *
         *  @see ConvertQuietly for span overloads
         * 
         *  String overloads are simple wrappers on the span code but with simpler to use arguments:
         *  \par Example Usage
         *      \code
         *          wstring wide_fred = UTFConverter::kThe.Convert<wstring> (u8"fred");
         *          u16string u16_fred = UTFConverter::kThe.Convert<u16string> (U"fred");
         *      \endcode
         */
        template <IUNICODECanUnambiguouslyConvertFrom SRC_T, IUNICODECanUnambiguouslyConvertFrom TRG_T>
        nonvirtual ConversionResult Convert (span<const SRC_T> source, span<TRG_T> target) const;
        template <IUNICODECanUnambiguouslyConvertFrom SRC_T, IUNICODECanUnambiguouslyConvertFrom TRG_T>
        nonvirtual ConversionResult Convert (span<SRC_T> source, span<TRG_T> target) const;
        template <typename TO, typename FROM>
        nonvirtual TO Convert (const FROM& from) const
            requires ((is_same_v<TO, string> or is_same_v<TO, wstring> or is_same_v<TO, u8string> or is_same_v<TO, u16string> or is_same_v<TO, u32string>) and
                      (is_same_v<FROM, string> or is_same_v<FROM, wstring> or is_same_v<FROM, u8string> or is_same_v<FROM, u16string> or
                       is_same_v<FROM, u32string>));

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
         *  \brief Convert UTF encoded (char8_t, char16_t, char32_t, char, wchar_t, ASCII, Latin1) characters to from each other without format exceptions (still may raise memory exceptions if not enuf space)
         * 
         *  \see Convert () above for details. This only differs from Convert, in that it returns a result flag instead
         *       of throwing on errors.
         * 
         *  \note - possible error status values include 'illegal source', and 'source exhausted'. Source exhausted ins't always an
         *          error, but it is more often than not, so its treated as an error, and you must special case handling if you want
         *          to treat otherwise.
         * 
         *          So - ConvertQuietly () of many characters, but where the LAST character is complete WILL convert all the data up to the last
         *          character, return the number of characters consumed and produced, but ALSO indicate the source exhausted status - not OK.
         * 
         *  Source and target spans can be of any IUNICODECanUnambiguouslyConvertFrom character type (but source const and target non-const)
         * 
         *  \req target.size () >= ComputeTargetBufferSize<TRG_T> (source)
         * 
         *  \note, if given illegal UTF-8, or illegal ascii (ASCII) source input, the function should (perhaps peramterized)
         *         detect the error and return ConversionStatusFlag::sourceIllegal (depending on Options::fStrictMode).
         *         // @todo RECONSIDER fStrictMode!!!
         * 
         *  \note multibyteConversionState is often ignored, but since some implementations may use it, it is required (to allow
         *        interface as a whole to always work without knowing which implementations require it).
         */
        template <IUNICODECanUnambiguouslyConvertFrom SRC_T, IUNICODECanUnambiguouslyConvertFrom TRG_T>
        nonvirtual ConversionResultWithStatus ConvertQuietly (span<const SRC_T> source, span<TRG_T> target) const
            requires (not is_const_v<TRG_T>);

    public:
        /**
         *  See what the given offset in the source text translates to in the target text
         * 
         *  For example, if you are translating UTF32 text to UTF8 text, the 3rd character in
         *  UTF32 text would start at offset 3, but in the corresponding UTF8 text it might
         *  start at offset 6.
         */
        template <IUNICODECanUnambiguouslyConvertFrom TRG_T, IUNICODECanUnambiguouslyConvertFrom SRC_T>
        nonvirtual size_t ConvertOffset (span<const SRC_T> source, size_t srcIndex) const;

    public:
        /**
         *  Given a span, return the number of code-point units in the (full UNICODE) character, or return nullopt if the span of characters is invalid or incomplete
         * 
         *  if argument span empty, or insufficient to complete a character, this returns nullopt.
         * 
         *  For example, if CHAR_T == char32_t, or Character, this returns 1.
         *  If CHAR_T is ASCII, this returns 1.
         */
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        static constexpr optional<size_t> NextCharacter (span<const CHAR_T> s);

    public:
        /**
         *  Given a span of UTF-encoded characters, return the number of characters (unicode code points) in the span, or nullopt if any character is incomplete/invalid
         *  (should we throw or skip or ???) - not sure
         * 
         *  \note for 'char' - the characters are ASSUMED/REQUIRED to be ASCII
         */
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        static constexpr optional<size_t> ComputeCharacterLength (span<const CHAR_T> s);

    public:
        /**
         *  \brief Nearly always use this default UTFConverter.
         */
        static const UTFConverter kThe;

    public:
        static void Throw (ConversionStatusFlag cr);

    private:
        // find same size, and then remove_const, and then add back const
        template <typename SRC_OF_CONSTNESS_T, typename TYPE_T>
        using AddConstIfMatching_ = conditional_t<is_const_v<SRC_OF_CONSTNESS_T>, add_const_t<TYPE_T>, TYPE_T>;
        template <typename TYPE_T>
        using MapSizes_ =
            conditional_t<sizeof (TYPE_T) == 1, conditional_t<is_same_v<TYPE_T, Latin1>, TYPE_T, char8_t>, conditional_t<sizeof (TYPE_T) == 2, char16_t, char32_t>>;
        template <typename TYPE_T>
        using CompatibleT_ = AddConstIfMatching_<TYPE_T, MapSizes_<TYPE_T>>;

    private:
        // need generic way to convert char to char8_t, and wchar_t to char16_t or char32_t, Character etc
        template <IUNICODECanUnambiguouslyConvertFrom FromT>
        static constexpr span<CompatibleT_<FromT>> ConvertToPrimitiveSpan_ (span<FromT> f);

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
        static ConversionResultWithStatus ConvertQuietly_codeCvt_ (span<const char8_t> source, span<char16_t> target);
        static ConversionResultWithStatus ConvertQuietly_codeCvt_ (span<const char8_t> source, span<char32_t> target);
        static ConversionResultWithStatus ConvertQuietly_codeCvt_ (span<const char16_t> source, span<char8_t> target);
        static ConversionResultWithStatus ConvertQuietly_codeCvt_ (span<const char32_t> source, span<char8_t> target);

    private:
        static void ThrowIf_ (ConversionStatusFlag cr);
    };

    /**
     *  This is a function that takes a span of bytes, and an OPTIONAL mbstate_t (TBD), and targetBuffer, translates into targetBuffer, and returns the changes.
     *  This utility wrapper funciton is meant to capture what you can easily put together from a (configured or default) UTFConverter,
     *  but in a form more easily used/consumed by a the TextReader code.
     * 
     *  @todo NEED EXAMPLE OR TO LOSE THIS...
     */
    template <typename OUTPUT_CHAR_T>
    using UTFCodeConverter = function<UTFConverter::ConversionResult (span<const std::byte> source, span<OUTPUT_CHAR_T> targetBuffer)>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "UTFConvert.inl"

#endif /*_Stroika_Foundation_Characters_UTFConvert_h_*/
