/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	"BadFormatException.h"




using	namespace	Stroika::Foundation;


/*
 ********************************************************************************
 *********************** DataExchangeFormat::BadFormatException ****************************
 ********************************************************************************
 */
DataExchangeFormat::BadFormatException::BadFormatException ()
	: StringException (L"Badly formatted input")
	, fDetails_ ()
{
}

DataExchangeFormat::BadFormatException::BadFormatException (const wstring& details)
	: StringException (L"Badly formatted input: " + details)
	, fDetails_ (details)
{
}
