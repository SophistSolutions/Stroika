/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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
 *************************** MimeTypes::Private::INIT ***************************
 ********************************************************************************
 */
DataExchange::Private_::InternetMediaType_ModuleData_::InternetMediaType_ModuleData_ ()
    : kText_Type{L"text"sv}
    , kImage_Type{L"image"sv}
    , kApplication_Type{L"application"sv}

    , kOctetStream_CT (kApplication_Type, L"octet-stream"sv)

    , kImage_PNG_CT (kImage_Type, L"png"sv)
    , kImage_GIF_CT (kImage_Type, L"gif"sv)
    , kImage_JPEG_CT (kImage_Type, L"jpeg"sv)

    , kText_HTML_CT (kText_Type, L"html"sv)
    , kText_XHTML_CT (kText_Type, L"xhtml"sv)
    , kText_XML_CT (kText_Type, L"xml"sv)
    , kText_PLAIN_CT (kText_Type, L"plain"sv)
    , kText_CSV_CT (kText_Type, L"csv"sv)

    , kJSON_CT (kApplication_Type, L"json"sv)

    , kPDF_CT (kApplication_Type, L"pdf"sv)

    // very unclear what to use, no clear standard!
    , kURL_CT (kApplication_Type, L"x-url"sv)

    , kXML_CT (kText_Type, L"xml"sv)

    , kXSLT_CT (kApplication_Type, L"x-xslt"sv)
    , kJavaArchive_CT (kApplication_Type, L"java-archive"sv)
    , kApplication_RTF_CT (kApplication_Type, L"rtf"sv)
    , kApplication_Zip_CT (kApplication_Type, L"zip"sv)
{
}

/*
 ********************************************************************************
 ************************** InternetMediaType ***********************************
 ********************************************************************************
 */
InternetMediaType::InternetMediaType (const String& ct)
{
    static const RegularExpression kTopLevelMatcher_ = L"([_\\-[:alnum:]]+|\\*)/([_\\-[:alnum:]]+|\\*)(.*)"_RegEx;
    Containers::Sequence<String>   matches;
    if (ct.Match (kTopLevelMatcher_, &matches) and matches.length () >= 2) {
        fType_    = matches[0];
        fSubType_ = matches[1];
        if (matches.length () == 3) {
            String moreParameters = matches[2];
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
        Execution::Throw (DataExchange::BadFormatException{L"Badly formatted InternetMediaType"sv});
    }
}

bool InternetMediaType::IsTextFormat () const
{
    if (fType_ == InternetMediaTypes::Types::kText ()) {
        return true;
    }
    /*
     * TODO:
     *      o   NEED EXTENSION MECHANSIM TO ADD OTHER TYPES - EG HelathFrameWorks PHR FORMATS
     *              -- LGP 2011-10-04
     */
    if (Match (InternetMediaTypes::kJSON)) {
        return true;
    }
    if (Match (InternetMediaTypes::kURL)) {
        return true;
    }
    if (Match (InternetMediaTypes::kApplication_XSLT)) {
        return true;
    }
    if (Match (InternetMediaTypes::kApplication_RTF)) {
        return true;
    }
    return false;
}

bool InternetMediaType::IsImageFormat () const
{
    if (fType_ == InternetMediaTypes::Types::kImage ()) {
        return true;
    }
    /*
     * TODO:
     *      o   NEED EXTENSION MECHANSIM TO ADD OTHER TYPES
     */
    return false;
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
    for (auto&& p : fParameters_) {
        sb += L"; " + p.fKey + L": " + p.fValue;
    }
    return sb.str ();
}

/*
 ********************************************************************************
 ********************** InternetMediaType::ThreeWayComparer *********************
 ********************************************************************************
 */
int InternetMediaType::ThreeWayComparer::operator() (const InternetMediaType& lhs, const InternetMediaType& rhs) const
{
    if (int cmp = Common::ThreeWayCompare (lhs.fType_, rhs.fType_)) {
        return cmp;
    }
    if (int cmp = Common::ThreeWayCompare (lhs.fSubType_, rhs.fSubType_)) {
        return cmp;
    }
    if (lhs.fParameters_.empty () and rhs.fParameters_.empty ()) {
        return 0; // very common case, shortcut
    }
    else {
        // expensive for rare case, but if we must compare parameters, need some standardized way to iterate over them (key)
        using namespace Containers;
        using namespace Characters;
        auto sortedMapping = [] (auto m) { return SortedMapping<String, String>{String::LessComparer{CompareOptions::eCaseInsensitive}, m}; };
#if qCompilerAndStdLib_template_DefaultArgIgnoredWhenFailedDeduction_Buggy
        return Mapping<String, String>::SequentialThreeWayComparer{Common::ThreeWayComparer<KeyValuePair<String, String>>{}}(sortedMapping (lhs.fParameters_), sortedMapping (rhs.fParameters_));
#else
        return Mapping<String, String>::SequentialThreeWayComparer{}(sortedMapping (lhs.fParameters_), sortedMapping (rhs.fParameters_));
#endif
    }
}
