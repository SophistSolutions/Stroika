/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_RegularExpression_inl_
#define _Stroika_Foundation_Characters_RegularExpression_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Characters {

    /*
     ********************************************************************************
     ********************** Characters::RegularExpression ***************************
     ********************************************************************************
     */
    inline RegularExpression::RegularExpression ()
        : fCompiledRegExp_ ()
    {
    }
    inline RegularExpression::RegularExpression (const wregex& regEx)
        : fCompiledRegExp_ (regEx)
    {
    }
    inline RegularExpression::RegularExpression (wregex&& regEx)
        : fCompiledRegExp_ (move (regEx))
    {
    }
    inline const wregex& RegularExpression::GetCompiled () const
    {
        return fCompiledRegExp_;
    }

    /*
     ********************************************************************************
     **************************** operator "" _RegEx ********************************
     ********************************************************************************
     */
    inline RegularExpression operator"" _RegEx (const char* str, size_t len) noexcept
    {
        return RegularExpression{String::FromASCII (str, str + len)};
    }
    inline RegularExpression operator"" _RegEx (const wchar_t* str, size_t len) noexcept
    {
        return RegularExpression{String{str, str + len}};
    }
#if __cpp_char8_t >= 201811L
    inline RegularExpression operator"" _RegEx (const char8_t* str, size_t len) noexcept
    {
        return RegularExpression{String{str, str + len}};
    }
#endif
    inline RegularExpression operator"" _RegEx (const char16_t* str, size_t len) noexcept
    {
        return RegularExpression{String{str, str + len}};
    }
    inline RegularExpression operator"" _RegEx (const char32_t* str, size_t len) noexcept
    {
        return RegularExpression{String{str, str + len}};
    }

}

#endif // _Stroika_Foundation_Characters_RegularExpression_inl_
