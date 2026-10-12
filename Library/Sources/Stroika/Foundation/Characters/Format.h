/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Format_h_
#define _Stroika_Foundation_Characters_Format_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <array>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Concepts.h"
#include "Stroika/Foundation/Common/StdCompat.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::Characters {

    /**
     *  \brief what Stroika's formatting - FormatString (_f), Format, DbgTrace - formats: anything std::format can, and anything
     *         else Characters::ToString can print, as its ToString
     *
     *  So a std::optional, an exception_ptr or an enum formats with "{}"_f, though std::format has no formatter for it (and
     *  Stroika registers none: @see Private_::IUseToStringFormatterForFormatter_); a format-spec then applies to its ToString, as to
     *  a String.
     */
    template <typename T>
    concept IFormattable = Common::StdCompat::formattable<remove_cvref_t<T>, wchar_t> or IToString<remove_cvref_t<T>>;

    namespace Private_ {
        /**
         *  f (the wformat_args of args): each argument itself where std::format can format it, else its Characters::ToString - kept
         *  alive through the call. Declared again in Debug/Trace.h, which cannot include this header.
         */
        template <typename RESULT, typename F, typename... ARGS>
        RESULT WithFormatArgs_ (F&& f, const ARGS&... args);

#if qStroika_Foundation_Debug_AssertionsChecked
        /**
         *  Does s last as long as the program - true too where Stroika cannot tell (@see Execution::IsAddressReadOnlyAppLifetime).
         *  Defined in Format.cpp, so this header need not include Execution/Module.h.
         */
        bool LastsAppLifetime_ (wstring_view s);
#endif

        /**
         *  A string literal as a template argument - so "..."_f can widen it, and check it against its arguments, at compile time.
         */
        template <typename CHAR_T, size_t N>
        struct FixedString_ {
            using CharType                 = CHAR_T;
            static constexpr size_t kChars = N - 1; // without the terminating NUL

            CHAR_T fChars[N]{}; // as the literal, with its terminating NUL

            consteval FixedString_ (const CHAR_T (&s)[N]);
        };

        // LITERAL, widened (for FormatLiteral)
        template <FixedString_ LITERAL>
        consteval array<wchar_t, decltype (LITERAL)::kChars + 1> Widen_ ();

        // is LITERAL ASCII (as a narrow "..."_f must be)
        template <FixedString_ LITERAL>
        consteval bool IsASCII_ ();
    }

    /**
     *  \brief Roughly equivalent to std::wformat_string, except that it can be constructed from 'char' string, and if 'char' require ASCII characters for format string
     * 
     *  \note - the string argument to FormatString must have application lifetime (@see String::FromStringConstant), as it is
     *        saved by reference. This is meant to be used with "foo={}"_f syntax - with constant c strings. (FormatString<char>
     *        keeps its own, widened, copy - so only FormatString<wchar_t>'s argument must.) Debug builds check it.
     * 
     *  \note   Types that can be formatted with FormatString are those IFormattable: anything std::formattable<T,wchar_t>
     *          (see https://en.cppreference.com/w/cpp/utility/format/formatter), and anything else Characters::ToString prints.
     *          In particular:
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
    template </*Common::IAnyOf< char, wchar_t>*/ typename CHAR_T>
    //requires (Common::IAnyOf<CHAR_T,char,wchar_t>)
    struct FormatString {
        static_assert (Common::IAnyOf<CHAR_T, char, wchar_t>); // not sure why this works but requires/concept applied in template not working...

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
        template <IFormattable... ARGS>
        [[nodiscard]] inline String operator() (ARGS&&... args) const;
        template <IFormattable... ARGS>
        [[nodiscard]] inline String operator() (const locale& loc, ARGS&&... args) const;

    protected:
        // not checked: for FormatString<char>, whose view is of its own copy, which lasts as long as it does, and FormatLiteral,
        // whose view is of a static array
        friend struct FormatString<char>;
        struct Unchecked_ {};
        constexpr FormatString (const basic_string_view<CHAR_T>& s, Unchecked_);

    private:
        wstring_view fSV_; // maybe SB wformat_string here??
    };
    template <typename CHAR_T>
    FormatString (const CHAR_T*) -> FormatString<CHAR_T>;
    template <typename CHAR_T>
    FormatString (const basic_string_view<CHAR_T>&) -> FormatString<CHAR_T>;
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

#if !qCompilerAndStdLib_vector_constexpr_Buggy
        constexpr
#endif
            FormatString& operator= (const FormatString& rhs);

        constexpr wstring_view                                             get () const;
        constexpr qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view getx_ () const;
        template <Stroika_Foundation_Common_formattable_FilterOnStringLitOp_BWA (IFormattable)... ARGS>
        [[nodiscard]] inline String operator() (ARGS&&... args) const;
        template <Stroika_Foundation_Common_formattable_FilterOnStringLitOp_BWA (IFormattable)... ARGS>
        [[nodiscard]] inline String operator() (const locale& loc, ARGS&&... args) const;
    };

    /**
     *  \brief what "..."_f is: a format string whose literal is part of its type - so it is widened (if narrow) at compile time,
     *         and checked against the arguments of each call at compile time, as std::format checks its format string
     *
     *  It is a FormatString<wchar_t>, so anything taking a FormatString takes one - but only what takes a FormatLiteral checks
     *  the arguments: its operator (), Format, DbgTrace, TraceContextBumper, Logger::Log and WebServer::Response::write.
     *
     *  \note Before Stroika v3.0d25, "..."_f was a FormatString<char> (or <wchar_t>): its format string checked only at run time
     *        (a mistake threw format_error), and a narrow one widened - into a newly allocated buffer - each time it ran.
     *
     *  \par Example Usage
     *      \code
     *          String a = "provider={}"_f (provider);
     *          DbgTrace ("provider={}"_f, provider);
     *          // String b = "{} {}"_f (provider);     // does not compile: two arguments wanted, one given
     *      \endcode
     */
    template <Private_::FixedString_ LITERAL>
    struct FormatLiteral : FormatString<wchar_t> {
        static_assert (Common::IAnyOf<typename decltype (LITERAL)::CharType, char, wchar_t>);
        static_assert (Private_::IsASCII_<LITERAL> (), "a narrow \"...\"_f must be ASCII");

    public:
        /**
         */
        constexpr FormatLiteral ();

    public:
        /**
         *  As FormatString's operator (), but args are checked against the format string at compile time
         */
        template <Stroika_Foundation_Common_formattable_FilterOnStringLitOp_BWA (IFormattable)... ARGS>
        [[nodiscard]] String operator() (ARGS&&... args) const;
        template <Stroika_Foundation_Common_formattable_FilterOnStringLitOp_BWA (IFormattable)... ARGS>
        [[nodiscard]] String operator() (const locale& loc, ARGS&&... args) const; ///< \brief As FormatString's, but checked at compile time

    public:
        /**
         *  \brief Unless the format string suits arguments of types ARGS..., a compile-time error - this is an immediate function
         */
        template <typename... ARGS>
        static consteval void CheckArgs ();

    public:
        /**
         *  For code that names FormatString<char>, as a narrow "..."_f was before Stroika v3.0d25: this one copies the literal
         */
        operator FormatString<char> () const
            requires (same_as<typename decltype (LITERAL)::CharType, char>);

    public:
        // what IFormatLiteral looks for - a mark, so Debug/Trace.h, which cannot include this header, can tell a FormatLiteral too
        using FormatLiteralTag_ = void;

    private:
        // the literal, widened at compile time: static, so it lasts as long as the program
        static constexpr array<wchar_t, decltype (LITERAL)::kChars + 1> kWide_ = Private_::Widen_<LITERAL> ();
    };

    /**
     *  \brief a FormatLiteral - what "..."_f is
     *
     *  So a function taking a format string and its arguments can check them at compile time: give it an overload for an
     *  IFormatLiteral FORMAT_LITERAL, which calls FORMAT_LITERAL::CheckArgs<ARGS...> () (or formats with it, whose operator () checks).
     */
    template <typename T>
    concept IFormatLiteral = requires { typename remove_cvref_t<T>::FormatLiteralTag_; };

    /**
     * \brief Create a format-string (see std::wformat_string or Stroika FormatString, or python 'f' strings) - a FormatLiteral
     *
     *  The literal must be ASCII if narrow (a compile-time error otherwise).
     *
     *  \par Example Usage
     *      \code
     *          String a = "provider={}"_f (provider);
     *          DbgTrace ("provider={}"_f , provider);
     *      \endcode
     */
    inline namespace Literals {
        template <Private_::FixedString_ LITERAL>
        consteval FormatLiteral<LITERAL> operator""_f ();
    }

    /**
     *  Same as vformat, except always produces valid UNICODE Stroika String...
     *   \brief same as std::vformat, except always uses wformat_args, and produces Stroika String (and maybe more - soon - ??? - add extra conversions if I can find how?)
     * 
     *  \note FormatString typically created with _f, as in "foo={}"_f
     */
    template <typename CHAR_T>
    [[nodiscard]] String VFormat (const FormatString<CHAR_T>& f, const Common::StdCompat::wformat_args& args);
    template <typename CHAR_T>
    [[nodiscard]] String VFormat (const locale& loc, const FormatString<CHAR_T>& f, const Common::StdCompat::wformat_args& args);

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
    template <typename CHAR_T, IFormattable... ARGS>
    String Format (const FormatString<CHAR_T>& f, ARGS&&... args);
    template <typename CHAR_T, IFormattable... ARGS>
    String Format (const locale& l, const FormatString<CHAR_T>& f, ARGS&&... args); ///< \brief Like std::format, with a locale
    template <Private_::FixedString_ LITERAL, Stroika_Foundation_Common_formattable_FilterOnStringLitOp_BWA (IFormattable)... ARGS>
    String Format (const FormatLiteral<LITERAL>& f, ARGS&&... args); ///< \brief Like std::format - args checked against f at compile time
    template <Private_::FixedString_ LITERAL, Stroika_Foundation_Common_formattable_FilterOnStringLitOp_BWA (IFormattable)... ARGS>
    String Format (const locale& l, const FormatLiteral<LITERAL>& f, ARGS&&... args); ///< \brief Like std::format, with a locale - checked at compile time

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Format.inl"

#endif /*_Stroika_Foundation_Characters_Format_h_*/
