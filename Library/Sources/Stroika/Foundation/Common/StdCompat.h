/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_StdCompat_h_
#define _Stroika_Foundation_Common_StdCompat_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <bit>
#include <cmath>
#include <compare>
#include <cstdarg>
#include <ranges>

#if __cpp_lib_expected
#include <expected>
#else
#include <variant>
#endif

// Various kooky constraints
//      (1) clang++15/16 don't set __cpp_lib_format, so cannot check __cpp_lib_format >= 201907 instead check __has_include(<format>)
//      (2) has_include <format> false positives on some versions of XCode, and no reason to even build qStroika_HasComponent_fmtlib unless
//          its needed, so check it first

#if qStroika_HasComponent_fmtlib
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/xchar.h>
#elif __has_include(<format>)
#include <format>
#endif

/**
 *  \file
 * 
 *  The purpose of this module is to define any std c++ functions/classes etc - which may not be provided by the
 *  current std c++ library (often because not compiled with appropriate --std=... flag) - and/or because its
 *  an old compiler.
 * 
 *  This also includes 'named requirements' - rendered as concepts (probably SHOULD be part of CPP standard but are not).
 *
 *  This doesn't strictly violate any rules about sticking stuff into namespace std - cuz we don't. That's why we use a
 *  separate namespace (that often just indirects to the namespace std - where the function/class is defined already).
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Common::StdCompat {

    /**
     *  By default include all of std, but throw in selected missing things from some implementations.
     */
    using namespace std;

    /**
     * \brief Logically the C++ standard BasicLockable named requirement, but that was not included in std c++ library
     * 
     * \see https://en.cppreference.com/w/cpp/named_req/BasicLockable.html
     */
    template <typename T>
    concept BasicLockable = requires (T lo) {
        // Requires a public lock() member function
        { lo.lock () } -> std::same_as<void>;
        // Requires a public unlock() member function
        { lo.unlock () } -> std::same_as<void>;
    };

    /**
     * \brief Logically the C++ standard Lockable named requirement, but that was not included in std c++ library
     * 
     * \see https://en.cppreference.com/w/cpp/named_req/Lockable.html
     */
    template <typename T>
    concept Lockable = BasicLockable<T> and requires (T lo) {
        { lo.try_lock () } -> std::same_as<bool>;
    };

#if qStroika_HasComponent_fmtlib
#define qStroika_Foundation_Characters_FMT_PREFIX_ fmt
#elif __has_include(<format>)
#define qStroika_Foundation_Characters_FMT_PREFIX_ std
#else
    static_assert (false, "Stroika v3 requires some std::format compatible library - if building with one lacking builtin std::format, "
                          "configure --fmtlib use");
#endif

    /**
     *  To allow interop between std::format and fmt(fmtlib)::format, publish the names into the namespace 'Stroika::Foundation::Common::StdCompat' and use those.
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

#if __cplusplus >= kStrokia_Foundation_Common_cplusplus_23 || _HAS_CXX23 /*vis studio uses _HAS_CXX23 */
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
            { __f.parse (__pc) } -> same_as<typename decltype (__pc)::iterator>;
            { __cf.format (__t, __fc) } -> same_as<typename _Context::iterator>;
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
#if __cplusplus >= kStrokia_Foundation_Common_cplusplus_23 || _MSVC_LANG >= kStrokia_Foundation_Common_cplusplus_23
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
#if __cplusplus >= kStrokia_Foundation_Common_cplusplus_23
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

    /**
     *  Wrap a simplified version of std::unexpected, cuz handy even if c++23 not present
     */
#if __cpp_lib_expected
    template <typename T>
    using unexpected = std::unexpected<T>;
    template <typename T, typename E>
    using expected = std::expected<T, E>;
#else
    template <typename T>
    class unexpected {
    public:
        template <typename _UError = T>
            requires (!is_same_v<remove_cvref_t<_UError>, unexpected> && !is_same_v<remove_cvref_t<_UError>, in_place_t> && is_constructible_v<T, _UError>)
        constexpr explicit unexpected (_UError&& _Unex)
            : _Unexpected (forward<_UError> (_Unex))
        {
        }
        template <typename... _Args>
            requires is_constructible_v<T, _Args...>
        constexpr explicit unexpected (in_place_t, _Args&&... _Vals)
            : _Unexpected (forward<_Args> (_Vals)...)
        {
        }
        template <typename _Uty, typename... _Args>
            requires is_constructible_v<T, initializer_list<_Uty>&, _Args...>
        constexpr explicit unexpected (in_place_t, initializer_list<_Uty> _Ilist, _Args&&... _Vals)
            : _Unexpected (_Ilist, forward<_Args> (_Vals)...)
        {
        }

        constexpr const T& error () const& noexcept
        {
            return _Unexpected;
        }
        constexpr T& error () & noexcept
        {
            return _Unexpected;
        }
        constexpr const T&& error () const&& noexcept
        {
            return std::move (_Unexpected);
        }
        constexpr T&& error () && noexcept
        {
            return std::move (_Unexpected);
        }

        constexpr void swap (unexpected& _Other)
        {
            using std::swap;
            swap (_Unexpected, _Other._Unexpected); // intentional ADL
        }

        friend constexpr void swap (unexpected& _Left, unexpected& _Right)
            requires is_swappable<T>::value // TRANSITION, /permissive needs ::value
        {
            _Left.swap (_Right);
        }

        template <class _UErr>
        friend constexpr bool operator== (const unexpected& _Left, const unexpected<_UErr>& _Right)
        {
            return _Left._Unexpected == _Right.error ();
        }

    private:
        T _Unexpected;
    };
    template <typename T>
    unexpected (T) -> unexpected<T>;

    /**
     *  Wrap a simplified version of std::expected, cuz handy even if c++23 not present
     * 
     *      \note STILL VERY ROUGH DRAFT - IN USE - BUT NOT SURE ITS RIGHT (cuz mine works but delegating to
     *            MSFT one not working, so maybe mine wrong).
     */
    template <typename T, typename E>
    class expected;
    template <typename T, typename E>
    class expected {
    public:
        using value_type      = T;
        using error_type      = E;
        using unexpected_type = unexpected<E>;

        constexpr expected () noexcept                = default;
        constexpr expected (const expected&) noexcept = default;
        constexpr expected (T v)
            : fData_{v}
        {
        }
        constexpr expected (const unexpected_type& e)
            : fData_{e.error ()}
        {
        }
        template <typename T1, typename E1>
        constexpr expected (const expected<T1, E1>& e)
        {
            if (e) {
                fData_ = e.value ();
            }
            else {
                fData_ = e.error ();
            }
        }
        explicit operator bool () const noexcept
        {
            return std::get_if<T> (&fData_) != nullptr;
        }
        T operator* () const
        {
            return value ();
        }
        T value () const
        {
            return get<T> (fData_);
        }
        E error () const
        {
            return get<E> (fData_);
        }

    private:
        variant<T, E> fData_;
    };
#endif

    /**
     * [[indeterminate]]
     * https://en.cppreference.com/w/cpp/language/attributes/indeterminate.html
     * 
     *  \par Example Usage
     *      \code
     *         qStroika_ATTRIBUTE_INDETERMINATE byte r[1024]; // don't initialize explicitly cuz filled in below before used (performance)
     *      \endcode
     */
#if __has_cpp_attribute(indeterminate)
#define qStroika_ATTRIBUTE_INDETERMINATE [[indeterminate]]
#else
#define qStroika_ATTRIBUTE_INDETERMINATE
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StdCompat.inl"

#endif /*_Stroika_Foundation_Common_StdCompat_h_*/
