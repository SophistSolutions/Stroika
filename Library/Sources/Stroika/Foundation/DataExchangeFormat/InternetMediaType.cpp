/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	"InternetMediaType.h"




using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::DataExchangeFormat;
using	namespace	Stroika::Foundation::Execution;





/*
 ********************************************************************************
 *************************** MimeTypes::Private::INIT ***************************
 ********************************************************************************
 */
DataExchangeFormat::Private_::InternetMediaType_INIT::InternetMediaType_INIT ()
	: kImage_CT								(L"image")
	, kImage_PNG_CT							(L"image/png")
	, kImage_GIF_CT							(L"image/gif")

	, kText_CT								(L"text")
	, kText_HTML_CT							(L"text/html")
	, kText_XHTML_CT						(L"text/xhtml")
	, kText_PLAIN_CT						(L"text/plain")

	, kJSON_CT								(L"application/json")

	, kPDF_CT								(L"application/pdf")

	// very unclear what to use, no clear standard!
	, kURL_CT								(L"application/x-url")

	, kXML_CT								(L"text/xml")

	, kXSLT_CT								(L"application/x-xslt")
	, kJavaArchive_CT						(L"application/java-archive")
	, kApplication_RTF_CT					(L"application/rtf")
{
}

DataExchangeFormat::Private_::InternetMediaType_INIT::~InternetMediaType_INIT ()
{
}







/*
 ********************************************************************************
 ************************** InternetMediaType ***********************************
 ********************************************************************************
 */
bool	InternetMediaType::IsTextFormat () const
{
	/*
	 * TODO:
	 *		o	NEED EXTENSION MECHANSIM TO ADD OTHER TYPES - EG HelathFrameWorks PHR FORMATS
	 *				-- LGP 2011-10-04
	 */
	return 
		IsSubTypeOfOrEqualTo (*this, PredefinedInternetMediaType::Text_CT ())
		;
}

bool	InternetMediaType::IsImageFormat () const
{
	/*
	 * TODO:
	 *		o	NEED EXTENSION MECHANSIM TO ADD OTHER TYPES
	 */
	return IsSubTypeOfOrEqualTo (*this, PredefinedInternetMediaType::Image_CT ());
}







/*
 ********************************************************************************
 ******************************** IsSubTypeOf ***********************************
 ********************************************************************************
 */
bool	DataExchangeFormat::IsSubTypeOf (const InternetMediaType& moreSpecificType, const InternetMediaType& moreGeneralType)
{
	/*
	 * TODO:
	 *		o	This could be simpler and clearer using Stroika strings...
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
bool	DataExchangeFormat::IsSubTypeOfOrEqualTo (const InternetMediaType& moreSpecificType, const InternetMediaType& moreGeneralType)
{
	return moreSpecificType == moreGeneralType or IsSubTypeOf (moreSpecificType, moreGeneralType);
}
