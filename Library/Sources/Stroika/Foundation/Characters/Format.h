/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Format_h_
#define _Stroika_Foundation_Characters_Format_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Characters/ToString.h"
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
     *  \brief Roughly equivalent to std::wformat_string, except that it can be constructed from 'char' string, and if 'char' require ASCII characters for format string
     * 
     *  \note - the lifetime of the string argument to FormatString is application-lifetime - because the string
     *        is typically saved by reference. This is meant to be used with "foo={}"_f syntax - with constant c strings.
     * 
     *        Sadly, I know of no way to check the lifetime of the argument, so this goes un-enforced.
     * 
     *  \note   Types that can be formatted with FormatString (are formattable with it) are:
     *              Anything that is std::formattable<T,wchar_t>        (see https://en.cppreference.com/w/cpp/utility/format/formatter)
     *          in particular:
     *              o   any class with .ToString() method
     *              o   std::type_index
     *              o   std::is_enum<>
     *              o   std::exception
     *              o   std::filesystem::path
     *              o   exception_ptr
     *              o   POD types (int, bool, double, etc)
     *              o   String, or any T IConvertibleToString<T> (such as std::wstring)
     *              o   container (T) where T is a IToString (something you can call Characters::ToStirng() on), such as
     *                  o   is_array<T>
     *                  o   anything with .begin (), .end () - so any container/iterable
     *                  o   std::pair<T1,T2>
     *                  o   std::tuple<TN...>
     *                  o   std::optional<T>
     *                  o   KeyValuePair<T1,T2>
     *                  o   CountedValue<T>
     *                  o   atomic<T>
     *                  o   std::variant<TN....>
     * 
     *  \par Example Usage
     *      \code
     *          String a = FormatString<char>{"Internet Media Type {} not supported"sv}(mediaType);
     *          String sameAsA = "Internet Media Type {} not supported"_f (mediaType);
     *          DbgTrace ("prettyVersionString={}"_f, ppv);
     *          DbgTrace ("currentException={}"_f, current_exception ());
     *          DbgTrace ("seq={}"_f, Sequence<IO::Networking::URI>{...});
     *      \endcode
     * 
     */
    template </*Configuration::IAnyOf< char, wchar_t>*/ typename CHAR_T>
    //requires (Configuration::IAnyOf<CHAR_T,char,wchar_t>)
    struct FormatString {
        static_assert (Configuration::IAnyOf<CHAR_T, char, wchar_t>); // not sure why this works but requires/concept applied in template not working...

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
        /**
         */
        template <Configuration::StdCompat::formattable<wchar_t>... ARGS>
        [[nodiscard]] inline String operator() (ARGS&&... args) const;
        template <Configuration::StdCompat::formattable<wchar_t>... ARGS>
        [[nodiscard]] inline String operator() (const locale& loc, ARGS&&... args) const;

    private:
        wstring_view fSV_; // maybe SB wformat_string here??
        /// @todo - same CTOR magic to validate format string??? Maybe not needed?  BUt dont in format_string<> template...
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
        template <Configuration::StdCompat::formattable<wchar_t>... ARGS>
        [[nodiscard]] inline String operator() (ARGS&&... args) const;
        template <Configuration::StdCompat::formattable<wchar_t>... ARGS>
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
    *   MAYBE OK - since probably not intrinsically confusing?
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

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Format.inl"

#endif /*_Stroika_Foundation_Characters_Format_h_*/
