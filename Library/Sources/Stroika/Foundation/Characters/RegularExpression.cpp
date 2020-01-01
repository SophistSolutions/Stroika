/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <regex>

#include "../Containers/Sequence.h"

#include "String_Constant.h"

#include "RegularExpression.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

namespace {
    regex_constants::syntax_option_type mkOption_ (RegularExpression::SyntaxType st, CompareOptions co)
    {
        regex_constants::syntax_option_type f = static_cast<regex_constants::syntax_option_type> (st);
        if (co == CompareOptions::eCaseInsensitive) {
            f |= regex_constants::icase;
        }
        f |= regex_constants::optimize;
        return f;
    }
}

/*
 ********************************************************************************
 ********************** Characters::RegularExpression ***************************
 ********************************************************************************
 */
const RegularExpression RegularExpression::kNONE{L"(?!)"sv};
const RegularExpression RegularExpression::kAny{L".*"sv};

RegularExpression::RegularExpression (const String& re, SyntaxType syntaxType, CompareOptions co)
    : fCompiledRegExp_ (re.As<wstring> (), mkOption_ (syntaxType, co))
{
}

/*
 ********************************************************************************
 ***************** Characters::RegularExpressionMatch ***************************
 ********************************************************************************
 */
RegularExpressionMatch::RegularExpressionMatch (const String& fullMatch)
    : fFullMatch_ (fullMatch)
    , fSubMatches_ (Containers::Sequence<String> ())
{
}

RegularExpressionMatch::RegularExpressionMatch (const String& fullMatch, const Containers::Sequence<String>& subMatches)
    : fFullMatch_ (fullMatch)
    , fSubMatches_ (subMatches)
{
}

String RegularExpressionMatch::GetFullMatch () const
{
    return fFullMatch_;
}

Containers::Sequence<String> RegularExpressionMatch::GetSubMatches () const
{
    return fSubMatches_;
}
