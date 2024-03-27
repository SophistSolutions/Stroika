/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Format_inl_
#define _Stroika_Foundation_Characters_Format_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
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
    constexpr qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view FormatString<CHAR_T>::getx () const
    {
        return qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view{fSV_.data (), fSV_.size ()};
    }
    template </*Configuration::IAnyOf< char, wchar_t>*/ typename CHAR_T>
    template <typename... ARGS>
    [[nodiscard]] inline String FormatString<CHAR_T>::operator() (ARGS&&... args) const
    {
        using Configuration::StdCompat::make_wformat_args;
        using Configuration::StdCompat::vformat;
        return vformat (getx (), make_wformat_args (args...));
    }
    template </*Configuration::IAnyOf< char, wchar_t>*/ typename CHAR_T>
    template <typename... ARGS>
    [[nodiscard]] inline String FormatString<CHAR_T>::operator() (const locale& loc, ARGS&&... args) const
    {
        using Configuration::StdCompat::make_wformat_args;
        using Configuration::StdCompat::vformat;
        return vformat (loc, getx (), make_wformat_args (args...));
    }
     #if !qCompilerAndStdLib_vector_constexpr_Buggy
        constexpr 
        #else
        inline
        #endif
     FormatString<char>::FormatString (const FormatString& src)
        : fStringData_{src.fStringData_}
        , fFmtStr_{basic_string_view<wchar_t>{fStringData_.data (), fStringData_.size ()}}
    {
    }
     #if !qCompilerAndStdLib_vector_constexpr_Buggy
        constexpr 
        #else
        inline
        #endif
         FormatString<char>::FormatString (const basic_string_view<char>& s)
        : fStringData_{s.begin (), s.end ()}
        , fFmtStr_{basic_string_view<wchar_t>{fStringData_.data (), fStringData_.size ()}}
    {
        // @todo REQWUIRE s ISASCII
        // require arg - lifetime forever - string-view - no way to check...
    }
    constexpr wstring_view FormatString<char>::get () const
    {
        return fFmtStr_.get ();
    }
    constexpr qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view FormatString<char>::getx () const
    {
        return fFmtStr_.getx ();
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
        inline FormatString<char> operator"" _f (const char* str, size_t len)
        {
            return FormatString<char>{string_view{str, len}};
        }
        inline FormatString<wchar_t> operator"" _f (const wchar_t * str, size_t len)
        {
            return FormatString<wchar_t>{wstring_view{str, len}};
        }
    }

    /*
     ********************************************************************************
     *********************************** VFormat ************************************
     ********************************************************************************
     */
    template <typename CHAR_T>
    [[nodiscard]] inline String VFormat (const FormatString<CHAR_T>& f, const Configuration::StdCompat::wformat_args& args)
    {
        return Configuration::StdCompat::vformat (f.getx (), args);
    }
    template <typename CHAR_T>
    [[nodiscard]] inline String VFormat (const locale& loc, const FormatString<CHAR_T>& f, const Configuration::StdCompat::wformat_args& args)
    {
        return Configuration::StdCompat::vformat (loc, f.getx (), args);
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

#endif /*_Stroika_Foundation_Characters_Format_inl_*/
