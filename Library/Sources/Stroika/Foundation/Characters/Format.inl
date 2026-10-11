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
        , fFmtStr_{basic_string_view<wchar_t>{fStringData_.data (), fStringData_.size ()}}
    {
    }
#if qCompilerAndStdLib_vector_constexpr_Buggy
    inline
#else
    constexpr
#endif
        FormatString<char>::FormatString (const basic_string_view<char>& s)
        : fStringData_{s.begin (), s.end ()}
        , fFmtStr_{basic_string_view<wchar_t>{fStringData_.data (), fStringData_.size ()}}
    {
        Character::CheckASCII (span{s});
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
     ************************* Characters::Literals *********************************
     ********************************************************************************
     */
    inline namespace Literals {
#if qCompilerAndStdLib_vector_constexpr_Buggy
        inline
#else
        constexpr
#endif
            FormatString<char> operator""_f (const char* str, size_t len)
        {
            return FormatString<char>{string_view{str, len}};
        }
        constexpr FormatString<wchar_t> operator""_f (const wchar_t* str, size_t len)
        {
            return FormatString<wchar_t>{wstring_view{str, len}};
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

}
