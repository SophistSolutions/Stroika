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
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Characters {

    /**
     *  \brief what types Stroika's formatting - FormatString (_f), Format, DbgTrace - formats: anything std::format can, and anything
     *         else Characters::ToString can print, as its ToString
     *
     *  So it takes more types than std::format. It also formats a std::optional, an exception_ptr,
     *  an enum - as its Characters::ToString, a format-spec applying to that as to a String. (Stroika's own types, and any of
     *  yours with a ToString () method, std::format formats too: Stroika registers a std::formatter for them.)
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
     *  \brief Stroika's format string: it always formats WIDE - each argument as std::formatter<T, wchar_t> formats it - into a
     *         String; written narrow ("..."_f) only as a terser spelling, so then it must be ASCII
     *
     *  A narrow format string (FormatString<char>) must be ASCII - so what it means cannot depend on the compiler's narrow
     *  character set. A wide one may be any UNICODE, and so may what is formatted into either - a String, say.
     *
     *  But EITHER way it produces a fully UNICODE String, and all the conversions - {} - are UNICODE.
     * 
     *  For narrow output instead - a std::string, in the program's narrow character set - use std::format with a narrow format
     *  string. Stroika's own types format there too (a String as AsNarrowSDKString (eIgnoreErrors) gives it), but not what only
     *  _f adds: the types std::format cannot format, formatted through their Characters::ToString (@see IFormattable).
     *
     *  \note A FormatString<wchar_t> views its text, so that text must last as long as the program (@see String::FromStringConstant)
     *        - Debug builds check it. A FormatString<char> keeps its own, widened, copy; "..."_f's is static (@see FormatLiteral).
     *
     *  \note What it formats: @see IFormattable.
     *
     *  \par Example Usage
     *      \code
     *          String a       = FormatString<char>{"Internet Media Type {} not supported"sv}(mediaType); // checked at run time
     *          String sameAsA = "Internet Media Type {} not supported"_f (mediaType);                    // checked at compile time
     *          DbgTrace ("prettyVersionString={}"_f, ppv);
     *          DbgTrace ("currentException={}"_f, current_exception ());
     *          DbgTrace ("seq={}"_f, Sequence<IO::Networking::URI>{...});
     *      \endcode
     * 
     */
    template <typename CHAR_T>
    struct FormatString {
        // a static_assert rather than a constraint, so Debug/Trace.h can forward-declare FormatString without including
        // Common/Concepts.h - a redeclaration must repeat a constraint
        static_assert (Common::IAnyOf<CHAR_T, char, wchar_t>);

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
         *  \brief the format string as the formatting library's own wstring_view (std's, or fmtlib's) - only for calling
         *         Common::StdCompat::vformat directly
         */
        constexpr qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view getx_ () const;

    public:
        /**
         *  \brief format args - checked against the format string at run time: a mismatch throws format_error (a FormatLiteral,
         *         "..."_f, checks at compile time)
         */
        template <IFormattable... ARGS>
        [[nodiscard]] inline String operator() (ARGS&&... args) const;
        template <IFormattable... ARGS>
        [[nodiscard]] inline String operator() (const locale& loc, ARGS&&... args) const; ///< \brief As above, with a locale

    protected:
        // not checked: for FormatString<char>, whose view is of its own copy, which lasts as long as it does, and FormatLiteral,
        // whose view is of a static array
        friend struct FormatString<char>;
        struct Unchecked_ {};
        constexpr FormatString (const basic_string_view<CHAR_T>& s, Unchecked_);

    private:
        wstring_view fSV_;
    };
    template <typename CHAR_T>
    FormatString (const CHAR_T*) -> FormatString<CHAR_T>;
    template <typename CHAR_T>
    FormatString (const basic_string_view<CHAR_T>&) -> FormatString<CHAR_T>;

    /**
     *  \brief a narrow FormatString: its text must be ASCII - checked at run time, it throws otherwise - and it keeps its own
     *         widened copy, so that text need not last
     */
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
     *  The literal must be ASCII if narrow (a compile-time error otherwise) - narrow or wide, it formats wide, into a String
     *  (@see FormatString).
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
     *  \brief same as std::vformat, except that it always uses wformat_args, and produces a Stroika String
     *
     *  \note FormatString typically created with _f, as in "foo={}"_f
     */
    template <typename CHAR_T>
    [[nodiscard]] String VFormat (const FormatString<CHAR_T>& f, const Common::StdCompat::wformat_args& args);
    template <typename CHAR_T>
    [[nodiscard]] String VFormat (const locale& loc, const FormatString<CHAR_T>& f, const Common::StdCompat::wformat_args& args); ///< \brief As above, with a locale

    /**
     *  \brief printf-style formatting of a va_list, into a String - with one difference from vswprintf: %s takes a const wchar_t*
     *         (as Microsoft's does - the better reading, with wide characters)
     *
     *  New code formats with "..."_f; a C format string, with CString::Format.
     */
    String FormatV (const wchar_t* format, va_list argsList);
    [[deprecated ("Since Stroika v3.0d6 - use _f format strings - not old style c format strings - use CString::Format for old style C "
                  "format strings")]] String
    Format (const wchar_t* format, ...);

    /**
     *  \brief Like std::format, but formats wide, into a String: f (args...) - its args checked at run time
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
