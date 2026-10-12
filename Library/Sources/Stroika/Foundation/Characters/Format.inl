/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::Characters {

    /*
     ********************************************************************************
     ********************* Characters::Private_::WithFormatArgs_ ********************
     ********************************************************************************
     */
    namespace Private_ {
        // t itself where std::format can format it, else its Characters::ToString. A char itself too: with fmtlib, StdCompat::formattable
        // says no to one, though fmtlib formats it as the character - where ToString would print its number
        template <typename T>
        inline decltype (auto) AsFormatArg_ (const T& t)
        {
            if constexpr (Common::StdCompat::formattable<T, wchar_t> or same_as<T, char>) {
                return (t);
            }
            else {
                return Characters::ToString (t);
            }
        }
        /*
         *  make_wformat_args copies no argument - it keeps references, which f's vformat reads later. So the Strings AsFormatArg_
         *  makes must outlive f: they are this lambda's parameters, alive until it returns, and named (newer libraries'
         *  make_wformat_args takes no temporary). Not make_wformat_args (AsFormatArg_ (args)...) in the caller: those temporaries
         *  die at the end of that statement, before vformat reads them. RESULT is explicit, not deduced, so that Debug/Trace.h -
         *  which cannot include this header - can call this through its declaration.
         */
        template <typename RESULT, typename F, typename... ARGS>
        inline RESULT WithFormatArgs_ (F&& f, const ARGS&... args)
        {
            return [&f] (const auto&... a) -> RESULT { return forward<F> (f) (Common::StdCompat::make_wformat_args (a...)); }(AsFormatArg_ (args)...);
        }

        // the type of what std::format is given for an argument of type T (@see AsFormatArg_)
        template <typename T>
        using FormatArg_t = remove_cvref_t<decltype (AsFormatArg_ (declval<const remove_cvref_t<T>&> ()))>;
    }

    /*
     ********************************************************************************
     ********************** Characters::Private_::FixedString_ **********************
     ********************************************************************************
     */
    template <typename CHAR_T, size_t N>
    consteval Private_::FixedString_<CHAR_T, N>::FixedString_ (const CHAR_T (&s)[N])
    {
        for (size_t i = 0; i < N; ++i) {
            fChars[i] = s[i];
        }
    }
    template <Private_::FixedString_ LITERAL>
    consteval array<wchar_t, decltype (LITERAL)::kChars + 1> Private_::Widen_ ()
    {
        array<wchar_t, decltype (LITERAL)::kChars + 1> result{};
        for (size_t i = 0; i < result.size (); ++i) {
            result[i] = static_cast<wchar_t> (LITERAL.fChars[i]); // a narrow one is ASCII - @see IsASCII_
        }
        return result;
    }
    template <Private_::FixedString_ LITERAL>
    consteval bool Private_::IsASCII_ ()
    {
        if constexpr (same_as<typename decltype (LITERAL)::CharType, char>) {
            for (char c : LITERAL.fChars) {
                if (static_cast<unsigned char> (c) >= 0x80) {
                    return false;
                }
            }
        }
        return true;
    }

    /*
     ********************************************************************************
     ***************************** FormatString<CHAR_T> *****************************
     ********************************************************************************
     */
    template </*Common::IAnyOf< char, wchar_t>*/ typename CHAR_T>
    constexpr FormatString<CHAR_T>::FormatString (const basic_string_view<CHAR_T>& s)
        : fSV_{s}
    {
#if qStroika_Foundation_Debug_AssertionsChecked
        if (not is_constant_evaluated ()) {
            Require (Private_::LastsAppLifetime_ (fSV_)); // @see the lifetime note on FormatString
        }
#endif
    }
    template </*Common::IAnyOf< char, wchar_t>*/ typename CHAR_T>
    constexpr FormatString<CHAR_T>::FormatString (const basic_string_view<CHAR_T>& s, Unchecked_)
        : fSV_{s}
    {
    }
    template </*Common::IAnyOf< char, wchar_t>*/ typename CHAR_T>
    constexpr wstring_view FormatString<CHAR_T>::get () const
    {
        return fSV_;
    }
    template </*Common::IAnyOf< char, wchar_t>*/ typename CHAR_T>
    constexpr qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view FormatString<CHAR_T>::getx_ () const
    {
        return qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view{fSV_.data (), fSV_.size ()};
    }
    template </*Common::IAnyOf< char, wchar_t>*/ typename CHAR_T>
    template <IFormattable... ARGS>
    [[nodiscard]] inline String FormatString<CHAR_T>::operator() (ARGS&&... args) const
    {
        return Private_::WithFormatArgs_<String> (
            [this] (Common::StdCompat::wformat_args a) { return Common::StdCompat::vformat (getx_ (), a); }, args...);
    }
    template </*Common::IAnyOf< char, wchar_t>*/ typename CHAR_T>
    template <IFormattable... ARGS>
    [[nodiscard]] inline String FormatString<CHAR_T>::operator() (const locale& loc, ARGS&&... args) const
    {
        return Private_::WithFormatArgs_<String> (
            [&] (Common::StdCompat::wformat_args a) { return Common::StdCompat::vformat (loc, getx_ (), a); }, args...);
    }
#if qCompilerAndStdLib_vector_constexpr_warning_Buggy
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Winvalid-constexpr\"");
#endif
#if qCompilerAndStdLib_vector_constexpr_Buggy
    inline
#else
    constexpr
#endif
        FormatString<char>::FormatString (const FormatString& src)
        : fStringData_{src.fStringData_}
        , fFmtStr_{basic_string_view<wchar_t>{fStringData_.data (), fStringData_.size ()}, FormatString<wchar_t>::Unchecked_{}}
    {
    }
#if qCompilerAndStdLib_vector_constexpr_Buggy
    inline
#else
    constexpr
#endif
        FormatString<char>::FormatString (const basic_string_view<char>& s)
        : fStringData_{s.begin (), s.end ()}
        , fFmtStr_{basic_string_view<wchar_t>{fStringData_.data (), fStringData_.size ()}, FormatString<wchar_t>::Unchecked_{}}
    {
        Character::CheckASCII (span{s});
    }
#if qCompilerAndStdLib_vector_constexpr_Buggy
    inline
#else
    constexpr
#endif
        FormatString<char>& FormatString<char>::operator= (const FormatString& rhs)
    {
        // not defaulted: that would copy rhs's view - of rhs's copy, not this one's
        fStringData_ = rhs.fStringData_;
        fFmtStr_ = FormatString<wchar_t>{basic_string_view<wchar_t>{fStringData_.data (), fStringData_.size ()}, FormatString<wchar_t>::Unchecked_{}};
        return *this;
    }
    constexpr wstring_view FormatString<char>::get () const
    {
        return fFmtStr_.get ();
    }
    constexpr qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view FormatString<char>::getx_ () const
    {
        return fFmtStr_.getx_ ();
    }
    template <Stroika_Foundation_Common_formattable_FilterOnStringLitOp_BWA (IFormattable)... ARGS>
    [[nodiscard]] inline String FormatString<char>::operator() (ARGS&&... args) const
    {
        return fFmtStr_ (forward<ARGS> (args)...);
    }
    template <Stroika_Foundation_Common_formattable_FilterOnStringLitOp_BWA (IFormattable)... ARGS>
    [[nodiscard]] inline String FormatString<char>::operator() (const locale& loc, ARGS&&... args) const
    {
        return fFmtStr_ (loc, forward<ARGS> (args)...);
    }

    /*
     ********************************************************************************
     ************************ FormatLiteral<LITERAL> ********************************
     ********************************************************************************
     */
    template <Private_::FixedString_ LITERAL>
    constexpr FormatLiteral<LITERAL>::FormatLiteral ()
        : FormatString<wchar_t>{wstring_view{kWide_.data (), decltype (LITERAL)::kChars}, Unchecked_{}}
    {
    }
    template <Private_::FixedString_ LITERAL>
    template <Stroika_Foundation_Common_formattable_FilterOnStringLitOp_BWA (IFormattable)... ARGS>
    [[nodiscard]] inline String FormatLiteral<LITERAL>::operator() (ARGS&&... args) const
    {
        CheckArgs<ARGS...> ();
        return FormatString<wchar_t>::operator() (forward<ARGS> (args)...);
    }
    template <Private_::FixedString_ LITERAL>
    template <Stroika_Foundation_Common_formattable_FilterOnStringLitOp_BWA (IFormattable)... ARGS>
    [[nodiscard]] inline String FormatLiteral<LITERAL>::operator() (const locale& loc, ARGS&&... args) const
    {
        CheckArgs<ARGS...> ();
        return FormatString<wchar_t>::operator() (loc, forward<ARGS> (args)...);
    }
    template <Private_::FixedString_ LITERAL>
    template <typename... ARGS>
    consteval void FormatLiteral<LITERAL>::CheckArgs ()
    {
        // as WithFormatArgs_ passes them on
        using Checked = Common::StdCompat::wformat_string<Private_::FormatArg_t<ARGS>...>;
#if qCompilerAndStdLib_format_dynamic_spec_Buggy
        // a replacement field within a replacement field - a dynamic width or precision - is checked only at run time
        bool inField = false;
        for (size_t i = 0; i < decltype (LITERAL)::kChars; ++i) {
            if (kWide_[i] == '{') {
                if (inField) {
                    return;
                }
                if (kWide_[i + 1] == '{') {
                    ++i; // an escaped { (kWide_ ends with a NUL, so [i + 1] is there)
                }
                else {
                    inField = true;
                }
            }
            else if (kWide_[i] == '}') {
                inField = false;
            }
        }
#endif
#if qCompilerAndStdLib_format_check_StaticArray_Buggy
        const auto               wide = Private_::Widen_<LITERAL> ();
        [[maybe_unused]] Checked checked{wstring_view{wide.data (), decltype (LITERAL)::kChars}};
#else
        [[maybe_unused]] Checked checked{wstring_view{kWide_.data (), decltype (LITERAL)::kChars}};
#endif
    }
    template <Private_::FixedString_ LITERAL>
    inline FormatLiteral<LITERAL>::operator FormatString<char> () const
        requires (same_as<typename decltype (LITERAL)::CharType, char>)
    {
        return FormatString<char>{string_view{LITERAL.fChars, decltype (LITERAL)::kChars}};
    }

    /*
     ********************************************************************************
     ************************* Characters::Literals *********************************
     ********************************************************************************
     */
    inline namespace Literals {
        template <Private_::FixedString_ LITERAL>
        consteval FormatLiteral<LITERAL> operator""_f ()
        {
            return FormatLiteral<LITERAL>{};
        }
    }
#if qCompilerAndStdLib_vector_constexpr_warning_Buggy
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Winvalid-constexpr\"");
#endif

    /*
     ********************************************************************************
     *********************************** VFormat ************************************
     ********************************************************************************
     */
    template <typename CHAR_T>
    [[nodiscard]] inline String VFormat (const FormatString<CHAR_T>& f, const Common::StdCompat::wformat_args& args)
    {
        return Common::StdCompat::vformat (f.getx_ (), args);
    }
    template <typename CHAR_T>
    [[nodiscard]] inline String VFormat (const locale& loc, const FormatString<CHAR_T>& f, const Common::StdCompat::wformat_args& args)
    {
        return Common::StdCompat::vformat (loc, f.getx_ (), args);
    }

    /*
     ********************************************************************************
     ************************************ Format ************************************
     ********************************************************************************
     */
    template <typename CHAR_T, IFormattable... ARGS>
    inline String Format (const FormatString<CHAR_T>& f, ARGS&&... args)
    {
        return f (forward<ARGS> (args)...);
    }
    template <typename CHAR_T, IFormattable... ARGS>
    inline String Format (const locale& loc, const FormatString<CHAR_T>& f, ARGS&&... args)
    {
        return f (loc, forward<ARGS> (args)...);
    }
    template <Private_::FixedString_ LITERAL, Stroika_Foundation_Common_formattable_FilterOnStringLitOp_BWA (IFormattable)... ARGS>
    inline String Format (const FormatLiteral<LITERAL>& f, ARGS&&... args)
    {
        return f (forward<ARGS> (args)...);
    }
    template <Private_::FixedString_ LITERAL, Stroika_Foundation_Common_formattable_FilterOnStringLitOp_BWA (IFormattable)... ARGS>
    inline String Format (const locale& loc, const FormatLiteral<LITERAL>& f, ARGS&&... args)
    {
        return f (loc, forward<ARGS> (args)...);
    }

}
