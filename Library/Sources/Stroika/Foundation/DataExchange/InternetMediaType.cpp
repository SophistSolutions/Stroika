/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/String_Constant.h"

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
    : kOctetStream_CT (L"application/octet-stream"_k)

    , kImage_CT (L"image"_k)
    , kImage_PNG_CT (L"image/png"_k)
    , kImage_GIF_CT (L"image/gif"_k)
    , kImage_JPEG_CT (L"image/jpeg"_k)

    , kText_CT (L"text"_k)
    , kText_HTML_CT (L"text/html"_k)
    , kText_XHTML_CT (L"text/xhtml"_k)
    , kText_XML_CT (L"text/xml"_k)
    , kText_PLAIN_CT (L"text/plain"_k)
    , kText_CSV_CT (L"text/csv"_k)

    , kJSON_CT (L"application/json"_k)

    , kPDF_CT (L"application/pdf"_k)

    // very unclear what to use, no clear standard!
    , kURL_CT (L"application/x-url"_k)

    , kXML_CT (L"text/xml"_k)

    , kXSLT_CT (L"application/x-xslt"_k)
    , kJavaArchive_CT (L"application/java-archive"_k)
    , kApplication_RTF_CT (L"application/rtf"_k)
    , kApplication_Zip_CT (L"application/zip"_k)
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
    return fType_;
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
