/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
    : kOctetStream_CT (String_Constant{L"application/octet-stream"})

    , kImage_CT (String_Constant{L"image"})
    , kImage_PNG_CT (String_Constant{L"image/png"})
    , kImage_GIF_CT (String_Constant{L"image/gif"})
    , kImage_JPEG_CT (String_Constant{L"image/jpeg"})

    , kText_CT (String_Constant{L"text"})
    , kText_HTML_CT (String_Constant{L"text/html"})
    , kText_XHTML_CT (String_Constant{L"text/xhtml"})
    , kText_XML_CT (String_Constant{L"text/xml"})
    , kText_PLAIN_CT (String_Constant{L"text/plain"})
    , kText_CSV_CT (String_Constant{L"text/csv"})

    , kJSON_CT (String_Constant{L"application/json"})

    , kPDF_CT (String_Constant{L"application/pdf"})

    // very unclear what to use, no clear standard!
    , kURL_CT (String_Constant{L"application/x-url"})

    , kXML_CT (String_Constant{L"text/xml"})

    , kXSLT_CT (String_Constant{L"application/x-xslt"})
    , kJavaArchive_CT (String_Constant{L"application/java-archive"})
    , kApplication_RTF_CT (String_Constant{L"application/rtf"})
    , kApplication_Zip_CT (String_Constant{L"application/zip"})
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
    return IsSubTypeOfOrEqualTo (*this, PredefinedInternetMediaType::Text_CT ()) or
           IsSubTypeOfOrEqualTo (*this, PredefinedInternetMediaType::JSON_CT ());
}

bool InternetMediaType::IsImageFormat () const
{
    /*
     * TODO:
     *      o   NEED EXTENSION MECHANSIM TO ADD OTHER TYPES
     */
    return IsSubTypeOfOrEqualTo (*this, PredefinedInternetMediaType::Image_CT ());
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
