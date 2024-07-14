/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Characters {

    /*
     ********************************************************************************
     ***************************** FormatString<CHAR_T> *****************************
     ********************************************************************************
     */
    template </*Configuration::IAnyOf< char, wchar_t>*/ typename CHAR_T>
    constexpr FormatString<CHAR_T>::FormatString (const basic_string_view<CHAR_T>& s)
        : fSV_{s}
    {
    }
    template </*Configuration::IAnyOf< char, wchar_t>*/ typename CHAR_T>
    constexpr wstring_view FormatString<CHAR_T>::get () const
    {
        return fSV_;
    }
    template </*Configuration::IAnyOf< char, wchar_t>*/ typename CHAR_T>
    constexpr qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view FormatString<CHAR_T>::getx_ () const
    {
        return qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view{fSV_.data (), fSV_.size ()};
    }
    template </*Configuration::IAnyOf< char, wchar_t>*/ typename CHAR_T>
    template <typename... ARGS>
    [[nodiscard]] inline String FormatString<CHAR_T>::operator() (ARGS&&... args) const
    {
        return Configuration::StdCompat::vformat (getx_ (), Configuration::StdCompat::make_wformat_args (args...));
    }
    template </*Configuration::IAnyOf< char, wchar_t>*/ typename CHAR_T>
    template <typename... ARGS>
    [[nodiscard]] inline String FormatString<CHAR_T>::operator() (const locale& loc, ARGS&&... args) const
    {
        return Configuration::StdCompat::vformat (loc, getx_ (), Configuration::StdCompat::make_wformat_args (args...));
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
    template <typename... ARGS>
    [[nodiscard]] inline String FormatString<char>::operator() (ARGS&&... args) const
    {
        return fFmtStr_ (args...);
    }
    template <typename... ARGS>
    [[nodiscard]] inline String FormatString<char>::operator() (const locale& loc, ARGS&&... args) const
    {
        return fFmtStr_ (loc, args...);
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
            FormatString<char>
            operator"" _f (const char* str, size_t len)
        {
            return FormatString<char>{string_view{str, len}};
        }
        constexpr FormatString<wchar_t> operator"" _f (const wchar_t * str, size_t len)
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
    [[nodiscard]] inline String VFormat (const FormatString<CHAR_T>& f, const Configuration::StdCompat::wformat_args& args)
    {
        return Configuration::StdCompat::vformat (f.getx_ (), args);
    }
    template <typename CHAR_T>
    [[nodiscard]] inline String VFormat (const locale& loc, const FormatString<CHAR_T>& f, const Configuration::StdCompat::wformat_args& args)
    {
        return Configuration::StdCompat::vformat (loc, f.getx_ (), args);
    }

    /*
     ********************************************************************************
     ************************************ Format ************************************
     ********************************************************************************
     */
    template <typename CHAR_T, typename... ARGS>
    inline String Format (const FormatString<CHAR_T>& f, ARGS&&... args)
    {
        return VFormat (f, Configuration::StdCompat::make_wformat_args (args...));
    }
    template <typename CHAR_T, typename... ARGS>
    inline String Format (const locale& loc, const FormatString<CHAR_T>& f, ARGS&&... args)
    {
        return VFormat (loc, f, Configuration::StdCompat::make_wformat_args (args...));
    }

}
