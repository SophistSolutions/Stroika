/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Format_h_
#define _Stroika_Foundation_Characters_Format_h_ 1

#include "../StroikaPreComp.h"

#include <cstdarg>

// Various kooky constraints
//      (1) clang++15/16 don't set __cpp_lib_format, so cannot check __cpp_lib_format >= 201907 instead check __has_include(<format>)
//      (2) has_include <format> false positives on some versions of XCode, and no reason to even build qHasFeature_fmtlib unless
//          its needed, so check it first

// NOT SURE WHY ON MACOS XCODE check #if __cpp_lib_format >= 201907 or similar - not working...
#if qHasFeature_fmtlib
#include <fmt/format.h>
#include <fmt/xchar.h>
#elif __has_include(<format>)
#include <format>
#endif
#include <ios>
#include <locale>
#include <sstream> //tmphack for my current formatter

#include "../Configuration/Common.h"

#include "String.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::Characters {

    inline namespace FmtSupport {
#if qHasFeature_fmtlib
#define qStroika_Foundation_Characters_FMT_PREFIX_ fmt
#elif __has_include(<format>)
#define qStroika_Foundation_Characters_FMT_PREFIX_ std
#else
        static_assert (false, "Stroika v3 requires some std::format compatible library - if building with one lacking builtin std::format, "
                              "configure --fmtlib use");
#endif

        /**
         *  To allow interop between std::format and fmt(fmtlib)::format, publish the names into the namespace 'Stroika::Foundation::Characters' and use those.
         *  Lose this once I can fully depend upon std::format... --LGP 2024-03-12
         */
        using qStroika_Foundation_Characters_FMT_PREFIX_::format;
        using qStroika_Foundation_Characters_FMT_PREFIX_::format_error;
        using qStroika_Foundation_Characters_FMT_PREFIX_::format_string;
        using qStroika_Foundation_Characters_FMT_PREFIX_::make_format_args;
        using qStroika_Foundation_Characters_FMT_PREFIX_::vformat;
        using qStroika_Foundation_Characters_FMT_PREFIX_::wformat_string;
    }

    /**
     */
    // EXPERIMENTAL NEW v3d6...
    [[nodiscard]] inline String VFormat (std::string_view fmt, qStroika_Foundation_Characters_FMT_PREFIX_::format_args args)
    {
        // @todo decide if this should ignore errors or not... FOR NOW NO, but document rationale carefully
        // probably std::format - will do same thign as this - but produce eIgnoreErrors SDK string...
        return String{qStroika_Foundation_Characters_FMT_PREFIX_::vformat (qStroika_Foundation_Characters_FMT_PREFIX_::string_view{fmt}, args)};
    }
    [[nodiscard]] inline String VFormat (std::wstring_view fmt, qStroika_Foundation_Characters_FMT_PREFIX_::wformat_args args)
    {
        return String{qStroika_Foundation_Characters_FMT_PREFIX_::vformat (qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view{fmt}, args)};
    }
    [[nodiscard]] inline String VFormat (const std::locale& loc, std::string_view fmt, qStroika_Foundation_Characters_FMT_PREFIX_::format_args args)
    {
        // @todo decide if this should ignore errors or not... FOR NOW NO, but document rationale carefully
        // probably std::format - will do same thign as this - but produce eIgnoreErrors SDK string...
        return String{qStroika_Foundation_Characters_FMT_PREFIX_::vformat (loc, qStroika_Foundation_Characters_FMT_PREFIX_::string_view{fmt}, args)};
    }
    [[nodiscard]] inline String VFormat (const std::locale& loc, std::wstring_view fmt, qStroika_Foundation_Characters_FMT_PREFIX_::wformat_args args)
    {
        return String{qStroika_Foundation_Characters_FMT_PREFIX_::vformat (loc, qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view{fmt}, args)};
    }

    /**
     * 
     * SUPER EARLY EXPERIEMNTAL DRAFT OF c++20 format support
        // Problem with allowing 'string_format' is it generates format_string - which I don't think will handle args of unicode chars right...

            @todo rename this to Format once we've fully removed all references to legacy "Format" API
     */
    template <typename... ARGS>
    [[nodiscard]] inline String Fmt (const qStroika_Foundation_Characters_FMT_PREFIX_::format_string<ARGS...> f, ARGS&&... args)
    {
        return vformat (f.get (), qStroika_Foundation_Characters_FMT_PREFIX_::make_format_args (args...));
    }
    template <typename... ARGS>
    [[nodiscard]] inline String Fmt (const qStroika_Foundation_Characters_FMT_PREFIX_::wformat_string<ARGS...> f, ARGS&&... args)
    {
        return vformat (f.get (), qStroika_Foundation_Characters_FMT_PREFIX_::make_wformat_args (args...));
    }
    template <typename... ARGS>
    [[nodiscard]] inline String Fmt (const std::locale& loc, const qStroika_Foundation_Characters_FMT_PREFIX_::format_string<ARGS...> f, ARGS&&... args)
    {
        return vformat (loc, f.get (), qStroika_Foundation_Characters_FMT_PREFIX_::make_format_args (args...));
    }
    template <typename... ARGS>
    [[nodiscard]] inline String Fmt (const std::locale& loc, const qStroika_Foundation_Characters_FMT_PREFIX_::wformat_string<ARGS...> f, ARGS&&... args)
    {
        return vformat (loc, f.get (), qStroika_Foundation_Characters_FMT_PREFIX_::make_wformat_args (args...));
    }

    // MAYBE CAN MAKE OPERATOR _f stuff wokr!!!
    /**
     * Simple wrapper on basic_string_view, but specifically for the purpose of treating a string_view as a format string.
     * Many of the (c++-20) format APIs just take a ?string_view as argument. But for purposes of overloading and differentiating
     * behavior, using a more specific type in some contexts is helpful.
     * 
     *  For example, when you use the operator"" _f (), it allows its result to unambiguously be treated as a (new format)
     *  format string.
    */
    template <typename CHAR_T>
    struct FormatString {
        basic_string_view<CHAR_T> sv;

        template <class... ARGS>
        [[nodiscard]] inline String operator() (ARGS&&... args)
        {
            using namespace qStroika_Foundation_Characters_FMT_PREFIX_;
            if constexpr (same_as<CHAR_T, char>) {
                // @todo fixup the characterset handling here...
                return vformat (sv, make_format_args (args...));
            }
            else if constexpr (same_as<CHAR_T, wchar_t>) {
                return vformat (sv, make_wformat_args (args...));
            }
        }
    };
    inline FormatString<char> operator"" _f (const char* str, size_t len)
    {
        return FormatString<char>{.sv = string_view{str, len}};
    }
    inline FormatString<wchar_t> operator"" _f (const wchar_t * str, size_t len)
    {
        return FormatString<wchar_t>{.sv = wstring_view{str, len}};
    }

    /*
     * Format is the Stroika wrapper on sprintf().
     * The main differences between sprintf () and Format are:
     *      (1)     No need for buffer management. ANY sized buffer will be automatically allocated internally and returned as a
     *              String.
     *
     *      (2)     This version of format has a SUBTLE - but important difference from std::c++ / sprintf() in the interpretation of
     *              %s format strings in Format(const wchar_t*): %s is assumed to match a const wchar_t* string in the variable
     *              argument list.
     *
     *              This deviation from the c++ standard (technically not a deviation because Stroika::Foundation::Characters::Format() isn't
     *              covered by the stdc++ ;-)) - is because of two reasons:
     *                  a)  Microsoft has this interpretation.
     *                  b)  Its a BETTER interpretation when using wide characters.
     *
     *  \note   The type of 'format' must be - basically  a POD type - not String - due to:
     *          From http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/n4659.pdf
     *
     *              If the parameter parmN is of a reference type, or of a type that is not compatible
     *              with the type that results when passing an argument for which there is no parameter,
     *              the behavior is undefined
     *          
     *          I have to say, I don't understand this. It makes sense for the parameters after paramN, but not for the parameter
     *          to va_start () - and BTW, I've used it with type String for a while, and except for compiler warnings, never saw
     *          a problem.
     */
    String FormatV (const wchar_t* format, va_list argsList);
    String Format (const wchar_t* format, ...);
    // @todo overload of Format/FormatV taking myfmt!!!! - PERFECT BACKWAWRD COMAT STRATEGY... - use Format with _f string to get new behavior, and regualr string to get old!!!
    /// ETC - DO MORE... like this... - then dont need Fmt lowercase anymore!!!
    template <typename... ARGS>
    inline String Format (FormatString<char> f, ARGS&&... args)
    {
        using namespace qStroika_Foundation_Characters_FMT_PREFIX_;
        return VFormat (f, make_format_args (args...));
    }
    template <typename... ARGS>
    inline String Format (FormatString<wchar_t> f, ARGS&&... args)
    {
        using namespace qStroika_Foundation_Characters_FMT_PREFIX_;
        return VFormat (f, make_wformat_args (args...));
    }

}

// SUPER PRIMITIVE ROUGH FIRST DRAFT
template <>
struct qStroika_Foundation_Characters_FMT_PREFIX_::formatter<Stroika::Foundation::Characters::String, wchar_t> {
    bool quoted = false;

    template <typename ParseContext>
    constexpr typename ParseContext::iterator parse (ParseContext& ctx)
    {
        auto it = ctx.begin ();
        if (it == ctx.end ())
            return it;

        if (*it == '#') {
            quoted = true;
            ++it;
        }
        if (*it != '}')
            throw format_error{"Invalid format args for QuotableString."};

        return it;
    }

    template <typename FmtContext>
    typename FmtContext::iterator format (Stroika::Foundation::Characters::String s, FmtContext& ctx) const
    {
        std::wstringstream out;
        out << s;
        return std::ranges::copy (std::move (out).str (), ctx.out ()).out;
    }
};
template <>
struct qStroika_Foundation_Characters_FMT_PREFIX_::formatter<Stroika::Foundation::Characters::String, char> {
    bool ignoreerrors{true}; // maybe set from thread-local variable, or parse() settings, or both

    template <typename ParseContext>
    constexpr typename ParseContext::iterator parse (ParseContext& ctx)
    {
        auto it = ctx.begin ();
        if (it == ctx.end ())
            return it;

        if (*it == '#') {
            //quoted = true;
            ++it;
        }
        if (*it != '}')
            throw format_error{"Invalid format args for QuotableString."};

        return it;
    }

    template <typename FmtContext>
    typename FmtContext::iterator format (Stroika::Foundation::Characters::String s, FmtContext& ctx) const
    {
        using namespace Stroika::Foundation::Characters;
        //  wformat_context delegateCTX;
        String dr{s}; // really want to delegate to wchar_t version (with vformat) but no documented easy way to extract format_args from ctx (though its in there)
        if (ignoreerrors) {
            return std::ranges::copy (dr.AsNarrowSDKString (eIgnoreErrors), ctx.out ()).out;
        }
        else {
            return std::ranges::copy (dr.AsNarrowSDKString (), ctx.out ()).out;
        }
    }
};

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Format.inl"

#endif /*_Stroika_Foundation_Characters_Format_h_*/
