/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_StdCompat_h_
#define _Stroika_Foundation_Configuration_StdCompat_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <bit>
#include <cmath>
#include <compare>
#include <cstdarg>
#include <ranges>

// Various kooky constraints
//      (1) clang++15/16 don't set __cpp_lib_format, so cannot check __cpp_lib_format >= 201907 instead check __has_include(<format>)
//      (2) has_include <format> false positives on some versions of XCode, and no reason to even build qHasFeature_fmtlib unless
//          its needed, so check it first

#if qHasFeature_fmtlib
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/xchar.h>
#elif __has_include(<format>)
#include <format>
#endif

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Configuration::StdCompat {

    /**
     *  By default include all of std, but throw in selected missing things from some implementations.
     */
    using namespace std;

#if qHasFeature_fmtlib
#define qStroika_Foundation_Characters_FMT_PREFIX_ fmt
#elif __has_include(<format>)
#define qStroika_Foundation_Characters_FMT_PREFIX_ std
#else
    static_assert (false, "Stroika v3 requires some std::format compatible library - if building with one lacking builtin std::format, "
                          "configure --fmtlib use");
#endif

    /**
     *  To allow interop between std::format and fmt(fmtlib)::format, publish the names into the namespace 'Stroika::Foundation::Configuration::StdCompat' and use those.
     *  Lose this once I can fully depend upon std::format... --LGP 2024-03-12
     */
    using qStroika_Foundation_Characters_FMT_PREFIX_::basic_format_parse_context;
    using qStroika_Foundation_Characters_FMT_PREFIX_::format;
    using qStroika_Foundation_Characters_FMT_PREFIX_::format_args;
    using qStroika_Foundation_Characters_FMT_PREFIX_::format_error;
    using qStroika_Foundation_Characters_FMT_PREFIX_::format_string;
    using qStroika_Foundation_Characters_FMT_PREFIX_::format_to;
    using qStroika_Foundation_Characters_FMT_PREFIX_::make_format_args;
    using qStroika_Foundation_Characters_FMT_PREFIX_::make_wformat_args;
    using qStroika_Foundation_Characters_FMT_PREFIX_::vformat;
    using qStroika_Foundation_Characters_FMT_PREFIX_::wformat_args;
    using qStroika_Foundation_Characters_FMT_PREFIX_::wformat_string;

#if __cplusplus >= kStrokia_Foundation_Configuration_cplusplus_23 || _HAS_CXX23 /*vis studio uses _HAS_CXX23 */
    template <class T, class CharT>
    concept formattable = std::formattable<T, CharT>;
#else
    namespace Private_ {
        template <class _CharT>
        struct _Phony_fmt_iter_for {
            using difference_type = ptrdiff_t;
            _CharT&              operator* () const;
            _Phony_fmt_iter_for& operator++ ();
            _Phony_fmt_iter_for  operator++ (int);
        };
        // _Formatter = typename _Context needed for clang++-15 (compiler bug but no BWA declaration for now...BWA in middle of BWA ;-))
        template <class _Ty, class _Context, class _Formatter = typename _Context::template formatter_type<remove_const_t<_Ty>>>
        concept _Formattable_with = semiregular<_Formatter> && requires (_Formatter& __f, const _Formatter& __cf, _Ty&& __t, _Context __fc,
                                                                         basic_format_parse_context<typename _Context::char_type> __pc) {
            {
                __f.parse (__pc)
            } -> same_as<typename decltype (__pc)::iterator>;
            {
                __cf.format (__t, __fc)
            } -> same_as<typename _Context::iterator>;
        };
    }
    template <class T, class CharT>
    concept formattable =
        Private_::_Formattable_with<remove_reference_t<T>, qStroika_Foundation_Characters_FMT_PREFIX_::basic_format_context<Private_::_Phony_fmt_iter_for<CharT>, CharT>>;
#endif

    /**
     *  Workaround absence of bit_cast in MacOS XCode 14 (which we support with Stroika v3)
     */
#if __cpp_lib_bit_cast >= 201806L
    using std::bit_cast;
#else
    template <class To, class From>
    inline To bit_cast (const From& src) noexcept
        requires (sizeof (To) == sizeof (From) && std::is_trivially_copyable_v<From> && std::is_trivially_copyable_v<To>)
    {
        static_assert (std::is_trivially_constructible_v<To>, "This implementation additionally requires "
                                                              "destination type to be trivially constructible");
        To dst;
        std::memcpy (&dst, &src, sizeof (To));
        return dst;
    }
#endif

    /**
     *  Workaround absence of byteswap gcc up to version 12, and clang (up to 14).
     */
#if __cpp_lib_byteswap >= 202110L
    using std::byteswap;
#else
    template <class T>
    inline T byteswap (T n) noexcept
    {
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Warray-bounds\"");
        static_assert (std::has_unique_object_representations_v<T>, "T may not have padding bits");
        auto value_representation = bit_cast<array<byte, sizeof (T)>> (n);
        for (size_t i = 0; i < value_representation.size () / 2; ++i) {
            swap (value_representation[i], value_representation[value_representation.size () - i]);
        }
        return bit_cast<T> (value_representation);
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Warray-bounds\"");
    }
#endif

    /**
     *  workaround qCompilerAndStdLib_fpclasifyEtcOfInteger_Buggy
     */
    template <typename T>
#if __cplusplus >= kStrokia_Foundation_Configuration_cplusplus_23 || _MSVC_LANG >= kStrokia_Foundation_Configuration_cplusplus_23
    constexpr
#else
    inline
#endif
        bool
        isinf (T v) noexcept
    {
#if qCompilerAndStdLib_fpclasifyEtcOfInteger_Buggy
        if constexpr (integral<T>) {
            return false; // needed for vis stud
        }
        else
#endif
            return std::isinf (v);
    }

    /**
     *  workaround qCompilerAndStdLib_fpclasifyEtcOfInteger_Buggy
     */
    template <typename T>
#if __cplusplus >= kStrokia_Foundation_Configuration_cplusplus_23
    constexpr
#else
    inline
#endif
        bool
        isnan (T v) noexcept
    {
#if qCompilerAndStdLib_fpclasifyEtcOfInteger_Buggy
        if constexpr (integral<T>) {
            return false; // needed for vis stud
        }
        else
#endif
            return std::isnan (v);
    }

#if qCompilerAndStdLib_stdlib_compare_three_way_present_but_Buggy
    struct compare_three_way {
        // NOTE - this workaround is GENERALLY INADEQUATE, but is adequate for my current use in Stroika -- LGP 2022-11-01
        template <typename LT, typename RT>
        constexpr auto operator() (LT&& lhs, RT&& rhs) const
        {
            using CT = common_type_t<LT, RT>;
            if (equal_to<CT>{}(forward<LT> (lhs), forward<RT> (rhs))) {
                return strong_ordering::equal;
            }
            return less<CT>{}(forward<LT> (lhs), forward<RT> (rhs)) ? strong_ordering::less : strong_ordering::greater;
        }
        using is_transparent = void;
    };
#else
    using compare_three_way = std::compare_three_way;
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StdCompat.inl"

#endif /*_Stroika_Foundation_Configuration_StdCompat_h_*/
