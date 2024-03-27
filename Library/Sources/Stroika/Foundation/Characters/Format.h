/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Format_h_
#define _Stroika_Foundation_Characters_Format_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdarg>
#include <ios>
#include <locale>
#include <ranges>
#include <sstream> //tmphack for my current formatter

#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Configuration/StdCompat.h"

#include "String.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::Characters {

#if 0
    /**
     */
    // EXPERIMENTAL NEW v3d6...
    [[nodiscard]] inline String VFormat (std::string_view fmt, Configuration::StdCompat::format_args args)
    {
        // @todo decide if this should ignore errors or not... FOR NOW NO, but document rationale carefully
        // probably std::format - will do same thign as this - but produce eIgnoreErrors SDK string...
        return String{Configuration::StdCompat::vformat (qStroika_Foundation_Characters_FMT_PREFIX_::string_view{fmt}, args)};
    }
    [[nodiscard]] inline String VFormat (std::wstring_view fmt, Configuration::StdCompat::wformat_args args)
    {
        return String{Configuration::StdCompat::vformat (qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view{fmt}, args)};
    }
    [[nodiscard]] inline String VFormat (const std::locale& loc, std::string_view fmt, Configuration::StdCompat::format_args args)
    {
        // @todo decide if this should ignore errors or not... FOR NOW NO, but document rationale carefully
        // probably std::format - will do same thign as this - but produce eIgnoreErrors SDK string...
        return String{Configuration::StdCompat::vformat (loc, qStroika_Foundation_Characters_FMT_PREFIX_::string_view{fmt}, args)};
    }
    [[nodiscard]] inline String VFormat (const std::locale& loc, std::wstring_view fmt, Configuration::StdCompat::wformat_args args)
    {
        return String{Configuration::StdCompat::vformat (loc, qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view{fmt}, args)};
    }
#endif

    /**
     * 
     * SUPER EARLY EXPERIEMNTAL DRAFT OF c++20 format support
        // Problem with allowing 'string_format' is it generates format_string - which I don't think will handle args of unicode chars right...

            @todo rename this to Format once we've fully removed all references to legacy "Format" API
     */
    template <typename... ARGS>
    [[deprecated ("Use Format")]] [[nodiscard]] inline String Fmt (const Configuration::StdCompat::format_string<ARGS...> f, ARGS&&... args)
    {
        return vformat (f.get (), Configuration::StdCompat::make_format_args (args...));
    }
    template <typename... ARGS>
    [[deprecated ("Use Format")]] [[nodiscard]] inline String Fmt (const Configuration::StdCompat::wformat_string<ARGS...> f, ARGS&&... args)
    {
        return vformat (f.get (), Configuration::StdCompat::make_wformat_args (args...));
    }
    template <typename... ARGS>
    [[deprecated ("Use Format")]] [[nodiscard]] inline String Fmt (const std::locale&                                     loc,
                                                                   const Configuration::StdCompat::format_string<ARGS...> f, ARGS&&... args)
    {
        return vformat (loc, f.get (), Configuration::StdCompat::make_format_args (args...));
    }
    template <typename... ARGS>
    [[deprecated ("Use Format")]] [[nodiscard]] inline String Fmt (const std::locale& loc,
                                                                   const Configuration::StdCompat::wformat_string<ARGS...> f, ARGS&&... args)
    {
        return vformat (loc, f.get (), Configuration::StdCompat::make_wformat_args (args...));
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

        /// @todo - same CTOR magic to validate format string??? Maybe not needed?  BUt dont in format_string<> template...

        template <typename... ARGS>
        [[nodiscard]] inline String operator() (ARGS&&... args) const
        {
            using Configuration::StdCompat::make_format_args;
            using Configuration::StdCompat::make_wformat_args;
            using Configuration::StdCompat::vformat;
            using qStroika_Foundation_Characters_FMT_PREFIX_::string_view; // cannot import into StdCompat cuz only 'fmtlib' uses this funky version of string_view
            using qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view;
            if constexpr (same_as<CHAR_T, char>) {
                try {
                    //////////////mmaybe fixed?// @todo fixup the characterset handling here...
                    // @todo redo with SmallStackBuffer<>
                    vector<wchar_t> wideFormatString{sv.begin (), sv.end ()};
                    return vformat (wstring_view{wideFormatString.data (), wideFormatString.size ()}, make_wformat_args (args...));
                }
                catch (...) {
                    //tmphack -
                    // WeakAssertNotReached ();
                    return "BAD"sv;
                }
            }
            else if constexpr (same_as<CHAR_T, wchar_t>) {
                return vformat (wstring_view{sv}, make_wformat_args (args...));
            }
        }
        template <typename... ARGS>
        [[nodiscard]] inline String operator() (const locale& loc, ARGS&&... args) const
        {
            using Configuration::StdCompat::make_format_args;
            using Configuration::StdCompat::make_wformat_args;
            using Configuration::StdCompat::vformat;
            using qStroika_Foundation_Characters_FMT_PREFIX_::string_view; // cannot import into StdCompat cuz only 'fmtlib' uses this funky version of string_view
            using qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view;
            if constexpr (same_as<CHAR_T, char>) {
                try {
                    //////////////mmaybe fixed?// @todo fixup the characterset handling here...
                    // @todo redo with SmallStackBuffer<>
                    vector<wchar_t> wideFormatString{sv.begin (), sv.end ()};
                    return vformat (loc, wstring_view{wideFormatString.begin (), wideFormatString.end ()}, make_wformat_args (args...));
                }
                catch (...) {
                    //tmphack -
                    // WeakAssertNotReached ();
                    return "BAD"sv;
                }
            }
            else if constexpr (same_as<CHAR_T, wchar_t>) {
                return vformat (loc, wstring_view{sv}, make_wformat_args (args...));
            }
        }
    };

    inline namespace Literals {
        inline FormatString<char> operator"" _f (const char* str, size_t len)
        {
            return FormatString<char>{.sv = string_view{str, len}};
        }
        inline FormatString<wchar_t> operator"" _f (const wchar_t * str, size_t len)
        {
            return FormatString<wchar_t>{.sv = wstring_view{str, len}};
        }
    }

    /**
    *  Same as vformat, except always produces valid UNICODE Stroika String...
     */
    [[nodiscard]] inline String VFormat (const FormatString<char> f, const Configuration::StdCompat::wformat_args& args)
    {
        using Configuration::StdCompat::vformat;
        using qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view; // cannot import into StdCompat cuz only 'fmtlib' uses this funky version of string_view
        vector<wchar_t> wideFormatString{f.sv.begin (), f.sv.end ()};
        //        return vformat (string_view{f.sv}, args);
        return Configuration::StdCompat::vformat (qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view{wideFormatString.data (), wideFormatString.size ()}, args);
    }
    [[nodiscard]] inline String VFormat (const FormatString<wchar_t> f, const Configuration::StdCompat::wformat_args& args)
    {
        using Configuration::StdCompat::vformat;
        using qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view; // cannot import into StdCompat cuz only 'fmtlib' uses this funky version of string_view
        return Configuration::StdCompat::vformat (qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view{f.sv}, args);
    }
    [[nodiscard]] inline String VFormat (const locale& loc, const FormatString<char> f, const Configuration::StdCompat::wformat_args& args)
    {
        using Configuration::StdCompat::vformat;
        using qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view; // cannot import into StdCompat cuz only 'fmtlib' uses this funky version of string_view
        vector<wchar_t> wideFormatString{f.sv.begin (), f.sv.end ()};
        return Configuration::StdCompat::vformat (loc, qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view{wideFormatString.data (), wideFormatString.size ()}, args);
    }
    [[nodiscard]] inline String VFormat (const locale& loc, const FormatString<wchar_t> f, const Configuration::StdCompat::wformat_args& args)
    {
        return Configuration::StdCompat::vformat (loc, qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view{f.sv}, args);
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
        return VFormat (f, Configuration::StdCompat::make_wformat_args (args...));
    }
    template <typename... ARGS>
    inline String Format (FormatString<wchar_t> f, ARGS&&... args)
    {
        return VFormat (f, Configuration::StdCompat::make_wformat_args (args...));
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
// NOT sure magic# for lib_ranges right here but ranges::copy doesnt exist on clang++15 for ubuntu 22.04
#if __cpp_lib_ranges >= 202207L
        return std::ranges::copy (std::move (out).str (), ctx.out ()).out;
#else
        return format_to (ctx.out (), L"{}", out.str ());
#endif
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
#if __cpp_lib_ranges >= 202207L
            return std::ranges::copy (dr.AsNarrowSDKString (eIgnoreErrors), ctx.out ()).out;
#else
            return format_to (ctx.out (), "{}", dr.AsNarrowSDKString (eIgnoreErrors));
#endif
        }
        else {
#if __cpp_lib_ranges >= 202207L
            return std::ranges::copy (dr.AsNarrowSDKString (), ctx.out ()).out;
#else
            return format_to (ctx.out (), "{}", dr.AsNarrowSDKString ());
#endif
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
