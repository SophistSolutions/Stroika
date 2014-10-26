/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <regex>

#include    "RegularExpression.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;


#if		!qCompilerAndStdLib_regex_Buggy


namespace   {
    regex_constants::syntax_option_type mkOption_ (RegularExpression::SyntaxType st, CompareOptions co)
    {
        regex_constants::syntax_option_type f   =   static_cast<regex_constants::syntax_option_type> (st);
        if (co == CompareOptions::eCaseInsensitive) {
            f |= regex_constants::icase;
        }
        return f;
    }
}




/*
 ********************************************************************************
 ********************** Characters::RegularExpression ***************************
 ********************************************************************************
 */
RegularExpression::RegularExpression (const String& re, SyntaxType syntaxType, CompareOptions co)
    :  fCompiledRegExp_ (re.As<wstring> (), mkOption_ (syntaxType, co))
{
}

RegularExpression::RegularExpression (const String& re, CompareOptions co)
    :  fCompiledRegExp_ (re.As<wstring> (), mkOption_ (SyntaxType::eDEFAULT, co))
{
}
#endif
