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
	Memory::SmallStackBuffer<char, 10*1024>	msgBuf (10*1024);
	va_list		argsList;
	va_start (argsList, format); 
	#if		__STDC_WANT_SECURE_LIB__
		while (::vsnprintf_s (msgBuf, msgBuf.GetSize (), msgBuf.GetSize ()-1, format, argsList) < 0) {
			msgBuf.GrowToSize (msgBuf.GetSize () * 2);
		}
	#else
		while (::vsnprintf (msgBuf, msgBuf.GetSize (), format, argsList) < 0) {
			msgBuf.GrowToSize (msgBuf.GetSize () * 2);
		}
	#endif
	va_end (argsList);
	Assert (::strlen (msgBuf) < NEltsOf (msgBuf));
	return msgBuf;
}

wstring	Characters::Format (const wchar_t* format, ...)
{
	Memory::SmallStackBuffer<wchar_t, 10*1024>	msgBuf (10*1024);
	va_list		argsList;
	va_start (argsList, format);

	const	wchar_t*	useFormat	=	format;
	#if		!qStdLibSprintfAssumesPctSIsWideInFormatIfWideFormat
		wchar_t		newFormat[5 * 1024];
		{
			size_t	origFormatLen	=	wcslen (format);
			Require (origFormatLen < NEltsOf (newFormat) / 2);	// just to be sure safe - this is already crazy-big for format string...
																// Could use Memory::SmallStackBuffer<> but I doubt this will ever get triggered
			bool lookingAtFmtCvt = false;
			size_t	newFormatIdx	=	0;
			for (size_t i = 0; i < origFormatLen; ++i) {
				if (lookingAtFmtCvt) {
					switch (format[i]) {
						case	'%': {
							lookingAtFmtCvt = false;
						}
						break;
						case	's': {
							newFormat[newFormatIdx] = 'l';
							newFormatIdx++;
						}
						break;
						case	'.': {
							// could still be part for format string
						}
						break;
						default: {
							if (isdigit (format[i])) {
								// could still be part for format string
							}
							else {
								lookingAtFmtCvt = false;	// DONE
							}
						}
						break;
					}
				}
				else {
					if (format[i] == '%') {
						lookingAtFmtCvt = true;
					}
				}
				newFormat[newFormatIdx] = format[i];
				newFormatIdx++;
			}
			Assert (newFormatIdx >= origFormatLen);
			if (newFormatIdx > origFormatLen) {
				newFormat[newFormatIdx] = '\0';
				useFormat	=	newFormat;
			}
		}
	#endif

	// Assume only reason for failure is not enuf bytes, so allocate more.
	// If I'm wrong, we'll just runout of memory and throw out...
	while (::vswprintf (msgBuf, msgBuf.GetSize (), useFormat, argsList) < 0) {
		msgBuf.GrowToSize (msgBuf.GetSize () * 2);
	}

	va_end (argsList);
	Assert (::wcslen (msgBuf) < msgBuf.GetSize ());
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







