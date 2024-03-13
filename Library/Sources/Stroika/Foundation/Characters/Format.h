/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Format_h_
#define _Stroika_Foundation_Characters_Format_h_ 1

#include "../StroikaPreComp.h"

#include <cstdarg>
#if __cpp_lib_format >= 201907
#include <format>
#elif qHasFeature_fmtlib
#include <fmt/format.h>
#include <fmt/xchar.h>
#endif
#include <ios>
#include <locale>
#include <sstream> //tmphack for my current formatter

#include "../Configuration/Common.h"

#include "String.h"

/**
 * TODO:
 *
 *      @todo   Consdier if we should have variants of these funtions taking a locale, or
 *              always using C/currnet locale. For the most part - I find it best to use the C locale.
 *              But DOCUMENT in all cases!!! And maybe implement variants...
 */

namespace Stroika::Foundation::Characters {

    inline namespace FmtSupport {
#if __cpp_lib_format >= 201907
#define qStroika_Foundation_Characters_FMT_PREFIX_ std
#elif qHasFeature_fmtlib
#define qStroika_Foundation_Characters_FMT_PREFIX_ fmt
#else
        static_assert (false, "Stroika v3 requires some std::format compatible library - if building with one lacking builtin std::format, "
                              "configure --fmtlib use");
#endif

        /**
 *  To allow interop between std::format and fmt(fmtlib)::format, publish the names into the namespace 'Stroika::Foundation::Characters' and use those.
 *  Lose this once I can fully depend upon std::format... --LGP 2024-03-12
 */
        using qStroika_Foundation_Characters_FMT_PREFIX_::format;
        using qStroika_Foundation_Characters_FMT_PREFIX_::format_string;
        using qStroika_Foundation_Characters_FMT_PREFIX_::make_format_args;
        using qStroika_Foundation_Characters_FMT_PREFIX_::vformat;
        using qStroika_Foundation_Characters_FMT_PREFIX_::wformat_string;
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

    // EXPERIMENTAL NEW v3d6...
    [[nodiscard]] inline String VFormat (std::string_view fmt, std::format_args args)
    {
        // @todo decide if this should ignore errors or not... FOR NOW NO, but document rationale carefully
        // probably std::format - will do same thign as this - but produce eIgnoreErrors SDK string...
        return String{vformat (fmt, args)};
    }
    [[nodiscard]] inline String VFormat (std::wstring_view fmt, std::wformat_args args)
    {
        return String{vformat (fmt, args)};
    }
    [[nodiscard]] inline String VFormat (const std::locale& loc, std::string_view fmt, std::format_args args)
    {
        // @todo decide if this should ignore errors or not... FOR NOW NO, but document rationale carefully
        // probably std::format - will do same thign as this - but produce eIgnoreErrors SDK string...
        return String{vformat (loc, fmt, args)};
    }
    [[nodiscard]] inline String VFormat (const std::locale& loc, std::wstring_view fmt, std::wformat_args args)
    {
        return String{vformat (loc, fmt, args)};
    }

    /**
     * 
     * SUPER EARLY EXPERIEMNTAL DRAFT OF c++20 format support
        // Problem with allowing 'string_format' is it generates format_string - which I don't think will handle args of unicode chars right...

            @todo rename this to Format once we've fully removed all references to legacy "Format" API
     */
    template <typename... ARGS>
    [[nodiscard]] inline String Fmt (const format_string<ARGS...> f, ARGS&&... args)
    {
        return VFormat (f.get (), make_format_args (args...));
    }
    template <typename... ARGS>
    [[nodiscard]] inline String Fmt (const wformat_string<ARGS...> f, ARGS&&... args)
    {
        return VFormat (f.get (), make_wformat_args (args...));
    }
    template <typename... ARGS>
    [[nodiscard]] inline String Fmt (const std::locale& loc, const format_string<ARGS...> f, ARGS&&... args)
    {
        return VFormat (loc, f.get (), make_format_args (args...));
    }
    template <typename... ARGS>
    [[nodiscard]] inline String Fmt (const std::locale& loc, const wformat_string<ARGS...> f, ARGS&&... args)
    {
        return VFormat (loc, f.get (), make_wformat_args (args...));
    }

    // MAYBE CAN MAKE OPERATOR _f stuff wokr!!!
    template <typename CHAR_T>
    struct myfmt {
        basic_string_view<CHAR_T> sv;

        template <class... ARGS>
        [[nodiscard]] inline String operator() (ARGS&&... _Args)
        {
            if constexpr (same_as<CHAR_T, char>) {
                return vformat (sv, make_format_args (_Args...));
            }
            else if constexpr (same_as<CHAR_T, wchar_t>) {
                return vformat (sv, make_wformat_args (_Args...));
            }
        }
    };
    inline myfmt<char> operator"" _f (const char* str, size_t len)
    {
        return myfmt<char>{.sv = string_view{str, len}};
    }
    inline myfmt<wchar_t> operator"" _f (const wchar_t * str, size_t len)
    {
        return myfmt<wchar_t>{.sv = wstring_view{str, len}};
    }

}

// SUPER PRIMITIVE ROUGH FIRST DRAFT
template <>
struct qStroika_Foundation_Characters_FMT_PREFIX_::formatter<Stroika::Foundation::Characters::String, wchar_t> {
    bool quoted = false;

    template <typename ParseContext>
    constexpr ParseContext::iterator parse (ParseContext& ctx)
    {
        auto it = ctx.begin ();
        if (it == ctx.end ())
            return it;

        if (*it == '#') {
            quoted = true;
            ++it;
        }
        if (*it != '}')
            throw std::format_error ("Invalid format args for QuotableString.");

        return it;
    }

    template <typename FmtContext>
    FmtContext::iterator format (Stroika::Foundation::Characters::String s, FmtContext& ctx) const
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
    constexpr ParseContext::iterator parse (ParseContext& ctx)
    {
        auto it = ctx.begin ();
        if (it == ctx.end ())
            return it;

        if (*it == '#') {
            //quoted = true;
            ++it;
        }
        if (*it != '}')
            throw std::format_error ("Invalid format args for QuotableString.");

        return it;
    }

    template <typename FmtContext>
    FmtContext::iterator format (Stroika::Foundation::Characters::String s, FmtContext& ctx) const
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
