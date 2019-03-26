/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/String_Constant.h"
#include "../Characters/ToString.h"

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
    : kOctetStream_CT (L"application/octet-stream"sv)

    , kImage_CT (L"image"sv)
    , kImage_PNG_CT (L"image/png"sv)
    , kImage_GIF_CT (L"image/gif"sv)
    , kImage_JPEG_CT (L"image/jpeg"sv)

    , kText_CT (L"text"sv)
    , kText_HTML_CT (L"text/html"sv)
    , kText_XHTML_CT (L"text/xhtml"sv)
    , kText_XML_CT (L"text/xml"sv)
    , kText_PLAIN_CT (L"text/plain"sv)
    , kText_CSV_CT (L"text/csv"sv)

    , kJSON_CT (L"application/json"sv)

    , kPDF_CT (L"application/pdf"sv)

    // very unclear what to use, no clear standard!
    , kURL_CT (L"application/x-url"sv)

    , kXML_CT (L"text/xml"sv)

    , kXSLT_CT (L"application/x-xslt"sv)
    , kJavaArchive_CT (L"application/java-archive"sv)
    , kApplication_RTF_CT (L"application/rtf"sv)
    , kApplication_Zip_CT (L"application/zip"sv)
{
}

DataExchange::Private_::InternetMediaType_ModuleData_::~InternetMediaType_ModuleData_ ()
{
}

/*
 ********************************************************************************
 ************************** InternetMediaType ***********************************
 ********************************************************************************
 */
bool InternetMediaType::IsTextFormat () const
{
    /*
     * TODO:
     *      o   NEED EXTENSION MECHANSIM TO ADD OTHER TYPES - EG HelathFrameWorks PHR FORMATS
     *              -- LGP 2011-10-04
     */
    return IsSubTypeOfOrEqualTo (*this, PredefinedInternetMediaType::kText) or
           IsSubTypeOfOrEqualTo (*this, PredefinedInternetMediaType::kJSON);
}

bool InternetMediaType::IsImageFormat () const
{
    /*
     * TODO:
     *      o   NEED EXTENSION MECHANSIM TO ADD OTHER TYPES
     */
    return IsSubTypeOfOrEqualTo (*this, PredefinedInternetMediaType::kImage);
}

String InternetMediaType::ToString () const
{
    return Characters::ToString (fType_); // format this string as any other normal string
}

/*
 ********************************************************************************
 ******************************** IsSubTypeOf ***********************************
 ********************************************************************************
 */
bool DataExchange::IsSubTypeOf (const InternetMediaType& moreSpecificType, const InternetMediaType& moreGeneralType)
{
    /*
     * TODO:
     *      o   This could be simpler and clearer using Stroika strings...
     */
    if (moreSpecificType.As<wstring> ().length () <= moreGeneralType.As<wstring> ().length ()) {
        return false;
    }
    return moreGeneralType.As<wstring> () == moreSpecificType.As<wstring> ().substr (0, moreGeneralType.As<wstring> ().length ());
}

/*
 ********************************************************************************
 *************************** IsSubTypeOfOrEqualTo *******************************
 ********************************************************************************
 */
bool DataExchange::IsSubTypeOfOrEqualTo (const InternetMediaType& moreSpecificType, const InternetMediaType& moreGeneralType)
{
    return moreSpecificType == moreGeneralType or IsSubTypeOf (moreSpecificType, moreGeneralType);
}
