/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <regex>

#include    "RegularExpression.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;



namespace   {
    regex_constants::syntax_option_type mkOption_ (RegularExpression::SyntaxType st, CompareOptions co)
    {
        regex_constants::syntax_option_type f   =   (st == RegularExpression::SyntaxType::eECMAScript ? regex_constants::ECMAScript : regex_constants::basic);
        if (co == CompareOptions::eCaseInsensitive) {
            f |= regex_constants::syntax_option_type::icase;
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
    :  fCompiledRegExp_ (re.As<wstring> (), mkOption_ (SyntaxType::DEFAULT, co))
{
}


