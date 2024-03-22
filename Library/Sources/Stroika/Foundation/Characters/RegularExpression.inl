/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
        : fCompiledRegExp_{}
    {
    }
    inline RegularExpression::RegularExpression (const wregex& regEx)
        : fCompiledRegExp_{regEx}
    {
    }
    inline RegularExpression::RegularExpression (wregex&& regEx)
        : fCompiledRegExp_{move (regEx)}
    {
    }
    inline RegularExpression::RegularExpression (const String& re, CompareOptions co)
        : RegularExpression{re, SyntaxType::eDEFAULT, co}
    {
    }
    inline const wregex& RegularExpression::GetCompiled () const
    {
        return fCompiledRegExp_;
    }

    /*
     ********************************************************************************
     *********************** Literals::operator "" _RegEx ***************************
     ********************************************************************************
     */
    inline namespace Literals {
        inline RegularExpression operator"" _RegEx (const char* str, size_t len)
        {
            return RegularExpression{String::FromStringConstant (span{str, len})};
        }
        inline RegularExpression operator"" _RegEx (const wchar_t* str, size_t len)
        {
            return RegularExpression{String{span{str, len}}};
        }
        inline RegularExpression operator"" _RegEx (const char8_t* str, size_t len)
        {
            return RegularExpression{String{span{str, len}}};
        }
        inline RegularExpression operator"" _RegEx (const char16_t* str, size_t len)
        {
            return RegularExpression{String{span{str, len}}};
        }
        inline RegularExpression operator"" _RegEx (const char32_t* str, size_t len)
        {
            return RegularExpression{String{span{str, len}}};
        }
    }

}

#endif // _Stroika_Foundation_Characters_RegularExpression_inl_
