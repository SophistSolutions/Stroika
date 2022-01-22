/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/RegularExpression.h"
#include "../Characters/ToString.h"
#include "../Containers/SortedMapping.h"
#include "../DataExchange/BadFormatException.h"

#include "InternetMediaType.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;

/*
 ********************************************************************************
 ************************** InternetMediaType ***********************************
 ********************************************************************************
 */
InternetMediaType::InternetMediaType (const String& ct)
{
    // @todo RECALL / DOCUMENT where I got this regexp from - roughly corresponds to https://tools.ietf.org/html/rfc2045#section-5.1

    // ROUGHLY based on
    //      grammar from RFC 2045 Section 5.1 and RFC 7231 Section 3.1.1.1
    // and
    //      https://tools.ietf.org/html/rfc2045#section-5.1
    //      https://tools.ietf.org/html/rfc7230#section-3.2.6
    //
    // but I'm not sure its really correct (but probably OK)
    // based on https://www.regextester.com/110183 and fiddling with that regexp to add suffix/params
    static const RegularExpression kTopLevelMatcher_ = L"(^[-\\w.]+)/([-\\w.]+)(\\+[a-z]*)?(.*)"_RegEx;
    Containers::Sequence<String>   matches;
    if (ct.Match (kTopLevelMatcher_, &matches) and matches.length () >= 2) {
        fType_    = matches[0];
        fSubType_ = matches[1];
        if (matches.length () >= 3 and matches[2].length () > 1) {
            fSuffix_ = matches[2].SubString (1);
        }
        if (matches.length () == 4) {
            String moreParameters = matches[3];
            while (not moreParameters.empty ()) {
                static const RegularExpression kParameterMatcher_ = L"\\s*;\\s*([_\\-[:alnum:]]+)\\s*=\\s*(\\S+)(.*)"_RegEx;
                matches.clear ();
                if (moreParameters.Match (kParameterMatcher_, &matches)) {
                    String pName  = matches[0];
                    String pValue = matches[1];
                    if (pValue.StartsWith (L"\"") and pValue.EndsWith (L"\"")) {
                        pValue = pValue.SubString (1, -1);
                    }
                    fParameters_.Add (pName, pValue);
                    if (matches.length () == 3) {
                        moreParameters = matches[2];
                    }
                    else {
                        break;
                    }
                }
                else {
                    break; // rest allowed - treated as comments
                }
            }
        }
    }
    else {
        static const auto kException_ = DataExchange::BadFormatException{L"Badly formatted InternetMediaType"sv};
        Execution::Throw (kException_);
    }
}

String InternetMediaType::ToString () const
{
    return Characters::ToString (As<String> ()); // format this string as any other normal string
}

template <>
String InternetMediaType::As () const
{
    if (empty ()) {
        return String{};
    }
    StringBuilder sb;
    sb += fType_.GetPrintName ();
    sb += L"/";
    sb += fSubType_.GetPrintName ();
    if (fSuffix_) {
        sb += L"+" + fSuffix_->GetPrintName ();
    }
    for (const auto& p : fParameters_) {
        sb += L"; " + p.fKey + L": " + p.fValue;
    }
    return sb.str ();
}

Common::strong_ordering InternetMediaType::THREEWAYCOMPARE_ (const InternetMediaType& rhs) const
{
    Common::strong_ordering cmp = Common::ThreeWayCompare (fType_, rhs.fType_);
    if (cmp != Common::kEqual) {
        return cmp;
    }
    cmp = Common::ThreeWayCompare (fSubType_, rhs.fSubType_);
    if (cmp != Common::kEqual) {
        return cmp;
    }
    cmp = Common::ThreeWayCompare (fSuffix_, rhs.fSuffix_);
    if (cmp != Common::kEqual) {
        return cmp;
    }
    if (fParameters_.empty () and rhs.fParameters_.empty ()) {
        return Common::kEqual; // very common case, shortcut
    }
    else {
        // expensive for rare case, but if we must compare parameters, need some standardized way to iterate over them (key)
        using namespace Containers;
        using namespace Characters;
        auto sortedMapping = [] (auto m) { return SortedMapping<String, String>{String::LessComparer{CompareOptions::eCaseInsensitive}, m}; };
#if qCompilerAndStdLib_template_DefaultArgIgnoredWhenFailedDeduction_Buggy
        return Mapping<String, String>::SequentialThreeWayComparer{Common::ThreeWayComparer{}}(sortedMapping (fParameters_), sortedMapping (rhs.fParameters_));
#else
        return Mapping<String, String>::SequentialThreeWayComparer{}(sortedMapping (fParameters_), sortedMapping (rhs.fParameters_));
#endif
    }
}

/*
 ********************************************************************************
 ******** hash<Stroika::Foundation::DataExchange::InternetMediaType> ************
 ********************************************************************************
 */
size_t std::hash<Stroika::Foundation::DataExchange::InternetMediaType>::operator() (const Stroika::Foundation::DataExchange::InternetMediaType& arg) const
{
    return hash<wstring> () (arg.As<wstring> ());
}

/*
 ********************************************************************************
 ****** DataExchange::DefaultSerializer<DataExchange::InternetMediaType> ********
 ********************************************************************************
 */
Memory::BLOB DataExchange::DefaultSerializer<InternetMediaType>::operator() (const InternetMediaType& arg) const
{
    return DefaultSerializer<Characters::String>{}(arg.As<Characters::String> ());
}
