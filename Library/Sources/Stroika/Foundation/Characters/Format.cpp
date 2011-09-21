/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<cstdarg>
#include	<cstdlib>
#include	<iomanip>
#include	<sstream>

#include	"../Containers/Common.h"
#include	"../Debug/Assertions.h"
#include	"../Debug/Trace.h"
#include	"../Math/Common.h"
#include	"../Memory/SmallStackBuffer.h"
#include	"CodePage.h"

#include	"Format.h"




using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;
using	namespace	Stroika::Foundation::Memory;






/*
 ********************************************************************************
 ************************************* Format ***********************************
 ********************************************************************************
 */
string	Characters::Format (const char* format, ...)
{
	char		msgBuf [10*1024];	// no idea how big to make it...
	va_list		argsList;
	va_start (argsList, format); 
	#if		__STDC_WANT_SECURE_LIB__
		(void)::vsnprintf_s (msgBuf, NEltsOf (msgBuf), _TRUNCATE, format, argsList);
	#else
		(void)::vsnprintf (msgBuf, NEltsOf (msgBuf), format, argsList);
	#endif
	va_end (argsList);
	Assert (::strlen (msgBuf) < NEltsOf (msgBuf));
	return msgBuf;
}

wstring	Characters::Format (const wchar_t* format, ...)
{
	wchar_t		msgBuf [10*1024];	// no idea how big to make it...
	va_list		argsList;
	va_start (argsList, format);
#if		qPlatform_Windows
	#if		__STDC_WANT_SECURE_LIB__
	//	(void)::vswprintf_s (msgBuf, format, argsList);
		(void)::_vsnwprintf_s (msgBuf, NEltsOf (msgBuf), _TRUNCATE, format, argsList);
	#else
		(void)::vsnwprintf (msgBuf, NEltsOf (msgBuf), format, argsList); 
	#endif
#else
		(void)::vswprintf (msgBuf, NEltsOf (msgBuf), format, argsList);
#endif
	va_end (argsList);
	Assert (::wcslen (msgBuf) < NEltsOf (msgBuf));
	return msgBuf;
}








/*
 ********************************************************************************
 ****************************** HexString2Int ***********************************
 ********************************************************************************
 */
int	Characters::HexString2Int (const string& s)
{
	return ::strtol (s.c_str (), nullptr, 16);
}

int	Characters::HexString2Int (const wstring& s)
{
	// http://msdn.microsoft.com/en-us/library/w4z2wdyc(v=vs.80).aspx
	// http://pubs.opengroup.org/onlinepubs/009695399/functions/wcstol.html
	return ::wcstol (s.c_str (), nullptr, 16);
}





/*
 ********************************************************************************
 ********************************* String2Int ***********************************
 ********************************************************************************
 */
int	Characters::String2Int (const string& s)
{
	return ::atol (s.c_str ());
}

int	Characters::String2Int (const wstring& s)
{
	#if		defined (_MSC_VER)
		return ::_wtol (s.c_str ());
	#else
		// unclear if this is good/safe - cuz of - for example - funky wide japanese numbers etc... probaby must do better
		// Also note use WideStringToNarrowSDKString() instead of WideStringToASCII - cuz the later asserts all valid ascii chars whcih may not be true here...
		//		-- LGP 2011-08-18
		return String2Int (WideStringToNarrowSDKString (s));
	#endif
}





/*
 ********************************************************************************
 ********************************* String2Float *********************************
 ********************************************************************************
 */
float	Characters::String2Float (const wstring& s)
{
	static	const	float	kBADVAL	=	static_cast<float> (Math::nan ());
	return String2Float (s, kBADVAL);
}

float	Characters::String2Float (const wstring& s, float returnValIfInvalidString)
{
	float	num	=	returnValIfInvalidString;
	#pragma	warning (push)
	#pragma	warning (4 : 4996)		// MSVC SILLY WARNING ABOUT USING swscanf_s
	if (::swscanf (s.c_str (), L"%f", &num) == 1) {
		return num;
	}
	#pragma warning (pop)
	return returnValIfInvalidString;
}







/*
 ********************************************************************************
 ********************************* Float2String *********************************
 ********************************************************************************
 */
wstring	Characters::Float2String (float f, unsigned int precision)
{
	if (isnan (f)) {
		return wstring ();
	}
	stringstream s;
	s << setprecision (precision) << f;
	wstring	r = ASCIIStringToWide (s.str ());
	return r;
}







