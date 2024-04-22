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

#include "Stroika/Foundation/Characters/Character.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Configuration/Concepts.h"
#include "Stroika/Foundation/Configuration/StdCompat.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::Characters {

    /**
     *  \brief Roughly equivalent to std::wformat_string, except that it can be constructed from 'char' string
     * 
     *  \note - the lifetime of the string argument to FormatString is application-lifetime - because the string
     *        is typically saved by reference. This is meant to be used with "foo={}"_f syntax - with constant c strings.
     * 
     *        Sadly, I know of no way to check the lifetime of the argument, so this goes un-enforced.
     * 
     * /olddocs
     * Simple wrapper on basic_string_view, but specifically for the purpose of treating a string_view as a format string.
     * Many of the (c++-20) format APIs just take a ?string_view as argument. But for purposes of overloading and differentiating
     * behavior, using a more specific type in some contexts is helpful.
     * 
     *  For example, when you use the operator"" _f (), it allows its result to unambiguously be treated as a (new format)
     * 
     * 
     *  format string.
     * 
     *  Somewhat like wformat_string, except that templated, and can take input ASCII 'char' string as well, and just maps it to wstring.
    */
    template </*Configuration::IAnyOf< char, wchar_t>*/ typename CHAR_T>
    //requires (Configuration::IAnyOf<CHAR_T,char,wchar_t>)
    struct FormatString {
        static_assert (Configuration::IAnyOf<CHAR_T, char, wchar_t>); // not sure why this works but requires/concept applied in template not working...

    private:
        wstring_view fSV_; // maybe SB wformat_string here??
        /// @todo - same CTOR magic to validate format string??? Maybe not needed?  BUt dont in format_string<> template...

    public:
        /**
         */
        FormatString ()                    = delete;
        FormatString (const FormatString&) = default;
        constexpr FormatString (const basic_string_view<CHAR_T>& s);

    public:
        /**
         *  This retrieves the underlying string data - for the format string itself (regardless of how created, as a wstring_view).
         */
        constexpr wstring_view get () const;

    public:
        /**
         *  Hack for interfacing with fmtlib - dont use unless you need to interact directly with fmtlib
         */
        constexpr qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view getx_ () const;

    public:
        template <typename... ARGS>
        [[nodiscard]] inline String operator() (ARGS&&... args) const;
        template <typename... ARGS>
        [[nodiscard]] inline String operator() (const locale& loc, ARGS&&... args) const;
    };
    template <>
    struct FormatString<char> {
    private:
        vector<wchar_t>       fStringData_;
        FormatString<wchar_t> fFmtStr_;

    public:
        FormatString () = delete;
#if !qCompilerAndStdLib_vector_constexpr_Buggy
        constexpr
#endif
            FormatString (const FormatString& src);
#if !qCompilerAndStdLib_vector_constexpr_Buggy
        constexpr
#endif
            FormatString (const basic_string_view<char>& s);

        constexpr wstring_view                                             get () const;
        constexpr qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view getx_ () const;
        template <typename... ARGS>
        [[nodiscard]] inline String operator() (ARGS&&... args) const;
        template <typename... ARGS>
        [[nodiscard]] inline String operator() (const locale& loc, ARGS&&... args) const;
    };

    /**
     * \brief Create a format-string (see std::wformat_string or Stroika FormatString, or python 'f' strings
     * 
     *  \par Example Usage
     *      \code
     *          String a = "provider={}"_f (provider);
     *          DbgTrace ("provider={}"_f , provider);
     *      \endcode
     */
    inline namespace Literals {
#if !qCompilerAndStdLib_vector_constexpr_Buggy
        constexpr
#endif
            FormatString<char>
            operator"" _f (const char* str, size_t len);
        constexpr FormatString<wchar_t> operator"" _f (const wchar_t * str, size_t len);
    }

    /**
     *  Same as vformat, except always produces valid UNICODE Stroika String...
     *   \brief same as std::vformat, except always uses wformat_args, and produces Stroika String (and maybe more - soon - ??? - add extra conversions if I can find how?)
     * 
     *  \note FormatString typically created with _f, as in "foo={}"_f
     */
    template <typename CHAR_T>
    [[nodiscard]] String VFormat (const FormatString<CHAR_T>& f, const Configuration::StdCompat::wformat_args& args);
    template <typename CHAR_T>
    [[nodiscard]] String VFormat (const locale& loc, const FormatString<CHAR_T>& f, const Configuration::StdCompat::wformat_args& args);

    /*
    * 
    *   &&& SOON TO BE OSBOLETE &&&& - NOT SURE IF/HOW MUCH WE WANT TO MAINTAIN THIS FUNCTIONALITY???
    *   MAYBE OK - since probably not intrindically confusing?
    * 
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
    [[deprecated ("Since Stroika v3.0d6 - use _f format strings - not old style c format strings - use CString::Format for old style C "
                  "format strings")]] String
    Format (const wchar_t* format, ...);

    /**
     *  \brief Like std::format, except returning stroika String, and taking _f (FormatString) string as argument (which can be ASCII, but still produce UNICODE output).
     */
    template <typename CHAR_T, typename... ARGS>
    String Format (const FormatString<CHAR_T>& f, ARGS&&... args);
    template <typename CHAR_T, typename... ARGS>
    String Format (const locale& l, const FormatString<CHAR_T>& f, ARGS&&... args);

}

// SUPER PRIMITIVE ROUGH FIRST DRAFT
template <>
struct qStroika_Foundation_Characters_FMT_PREFIX_::formatter<Stroika::Foundation::Characters::String, wchar_t> {
    qStroika_Foundation_Characters_FMT_PREFIX_::formatter<std::wstring, wchar_t> fDelegate2_;

    template <typename ParseContext>
    constexpr typename ParseContext::iterator parse (ParseContext& ctx)
    {
        return fDelegate2_.parse (ctx);
    }

    template <typename FmtContext>
    typename FmtContext::iterator format (Stroika::Foundation::Characters::String s, FmtContext& ctx) const
    {
        return fDelegate2_.format (s.As<std::wstring> (), ctx);
    }
};
template <>
struct qStroika_Foundation_Characters_FMT_PREFIX_::formatter<Stroika::Foundation::Characters::String, char> {
    bool ignoreerrors{true}; // maybe set from thread-local variable, or parse() settings, or both

    template <typename ParseContext>
    constexpr typename ParseContext::iterator parse (ParseContext& ctx)
    {
        auto it = ctx.begin ();
        while (it != ctx.end ()) {
            ++it;
#if 0
                if (it == ctx.end()) {
                    throw Configuration::StdCompat::format_error{"Invalid format args (missing }) for formatter<String,char>."};
                }
#endif
            if (*it == '}') {
                return it;
            }
        }
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
