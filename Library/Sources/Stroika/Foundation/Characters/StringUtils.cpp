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

#include	"StringUtils.h"




using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;
using	namespace	Stroika::Foundation::Memory;






/*
 ********************************************************************************
 *********************************** tolower ************************************
 ********************************************************************************
 */
void	Characters::tolower (wstring* s)
{
	RequireNotNull (s);
	wstring::iterator	end	=	s->end ();
	for (wstring::iterator i = s->begin (); i != end; ++i) {
		*i = TOLOWER (*i);
	}
}

void	Characters::tolower (string* s)
{
	RequireNotNull (s);
	string::iterator	end	=	s->end ();
	for (string::iterator i = s->begin (); i != end; ++i) {
		*i = TOLOWER (*i);
	}
}





/*
 ********************************************************************************
 *********************************** toupper ************************************
 ********************************************************************************
 */
void	Characters::toupper (wstring* s)
{
	RequireNotNull (s);
	wstring::iterator	end	=	s->end ();
	for (wstring::iterator i = s->begin (); i != end; ++i) {
		*i = TOUPPER (*i);
	}
}

void	Characters::toupper (string* s)
{
	RequireNotNull (s);
	string::iterator	end	=	s->end ();
	for (string::iterator i = s->begin (); i != end; ++i) {
		*i = TOUPPER (*i);
	}
}











/*
 ********************************************************************************
 ********************************* Contains *************************************
 ********************************************************************************
 */
bool	Characters::Contains (const wstring& string2Search, const wstring& substr, StringCompareOptions co)
{
	if (co == eWithCase_CO) {
		return string2Search.find (substr) != wstring::npos;
	}
	else {
		if (string2Search.length () < substr.length ()) {
			return false;
		}
		// sloppy but workable implementation
		size_t	maxL	=	string2Search.length () - substr.length ();
		for (size_t i = 0; i <= maxL; ++i) {
			if (::wcsncasecmp (string2Search.c_str () + i, substr.c_str (), substr.length ()) == 0) {
				return true;
			}
		}
		return false;
	}
}





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
 *************************** StripTrailingCharIfAny *****************************
 ********************************************************************************
 */
namespace	{
	template	<typename STRING>
	inline	STRING	StripTrailingCharIfAny_HLPR (const STRING& str, typename STRING::value_type c)
			{
				if (str.size () > 0 and str[str.size ()-1] == c) {
					STRING	tmp	=	str;
					tmp.erase (tmp.size ()-1);
					return tmp;
				}
				return str;
			}
}

string	Characters::StripTrailingCharIfAny (const string& s, char c)
{
	return StripTrailingCharIfAny_HLPR (s, c);
}

wstring	Characters::StripTrailingCharIfAny (const wstring& s, wchar_t c)
{
	return StripTrailingCharIfAny_HLPR (s, c);
}












/*
 ********************************************************************************
 *********************** MapUNICODETextWithMaybeBOMTowstring ********************
 ********************************************************************************
 */
wstring	Characters::MapUNICODETextWithMaybeBOMTowstring (const char* start, const char* end)
{
	Require (start <= end);
	if (start == end) {
		return wstring ();
	}
	else {
		size_t	outBufSize		=	end-start;
		SmallStackBuffer<wchar_t>	wideBuf (outBufSize);
		size_t	outCharCount	=	outBufSize;
		MapSBUnicodeTextWithMaybeBOMToUNICODE (start, end-start, wideBuf, &outCharCount);
		Assert (outCharCount <= outBufSize);
		if (outCharCount == 0) {
			return wstring ();
		}

		// The wideBuf may be NUL-terminated or not (depending on whether the input was NUL-terminated or not).
		// Be sure to construct the resuting string with the right end-of-string pointer (the length doesn't include
		// the NUL-char)
		return wstring (wideBuf, wideBuf[outCharCount-1] == '\0'? (outCharCount-1): outCharCount);
	}
}





/*
 ********************************************************************************
 *********************** MapUNICODETextWithMaybeBOMTowstring ********************
 ********************************************************************************
 */
vector<Byte>	Characters::MapUNICODETextToSerializedFormat (const wchar_t* start, const wchar_t* end, CodePage useCP)
{
	CodePageConverter	cpc (useCP, CodePageConverter::eHandleBOM);
	size_t	outCharCount	=	cpc.MapFromUNICODE_QuickComputeOutBufSize (start, end-start);
	SmallStackBuffer<char>	buf (outCharCount);
	cpc.MapFromUNICODE (start, end-start, buf, &outCharCount);
	const Byte* bs = reinterpret_cast<const Byte*> (static_cast<const char*> (buf));
	return vector<Byte> (bs, bs + outCharCount);
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








/*
 ********************************************************************************
 ********************************* LimitLength **********************************
 ********************************************************************************
 */
namespace	{
	template	<typename STRING>
		inline	STRING	LimitLength_HLPR (const STRING& str, size_t maxLen, bool keepLeft, const STRING& kELIPSIS)
			{
				if (str.length () <= maxLen) {
					return str;
				}
				size_t	useLen	=	maxLen;
				if (useLen > kELIPSIS.length ()) {
					useLen -= kELIPSIS.length ();
				}
				else {
					useLen = 0;
				}
				if (keepLeft) {
					return str.substr (0, useLen) + kELIPSIS;
				}
				else {
					return kELIPSIS + str.substr (str.length ()-useLen);
				}
			}
}
string	Characters::LimitLength (const string& str, size_t maxLen, bool keepLeft)
{
	return LimitLength_HLPR<string> (str, maxLen, keepLeft, "...");
}

wstring	Characters::LimitLength (const wstring& str, size_t maxLen, bool keepLeft)
{
	return LimitLength_HLPR<wstring> (str, maxLen, keepLeft, L"...");
}






/*
 ********************************************************************************
 **************************** StringVectorEncoding ******************************
 ********************************************************************************
 */
namespace	{
	inline	bool	IS_ENCODED_CHAR (wchar_t c)
		{
			return c == '\0' or c == '(' or c == ')' or c == '&';
		}
	inline	wstring	ENCODE (wchar_t c)
		{
			Require (IS_ENCODED_CHAR (c));
			switch (c) {
				case	'\0':	return L"&#0;";
				case	'&':	return L"&#38;";
				case	'(':	return L"&#40;";
				case	')':	return L"&#41;";
			}
			Assert (false); return wstring ();
		}
	wstring	ENCODE (const wstring& s)
		{
			wstring	r;
			r.reserve (s.size ());
			for (wstring::const_iterator i = s.begin (); i != s.end (); ++i) {
				Containers::ReserveSpeedTweekAdd1 (r);
				if (IS_ENCODED_CHAR (*i)) {
					r += ENCODE (*i);
				}
				else {
					r.push_back (*i);
				}
			}
			return r;
		}

	// return true iff resultChar is a valid character (has nothing to do with if more chars available)
	bool	DECODE (wstring::const_iterator* i, wstring::const_iterator end, wchar_t* resultChar)
		{
			RequireNotNull (resultChar);
			if (*i == end) {
				return false;
			}
			if (**i == '&') {
				// complex case - look for entity ref
				(*i)++;
				if (*i != end and **i == '#') {
					(*i)++;
				}
				else {
					*i = end;
					return false;		// bad format!
				}

				bool	 isGood	=	false;
				if (*i != end and isdigit (**i)) {
					if (**i == '0' and *(*i + 1) == ';') {
						(*i) += 2;
						*resultChar = '\0';
						return true;
					}
					if (**i == '3' and *(*i + 1) == '8' and *(*i + 2) == ';') {
						(*i) += 3;
						*resultChar = '&';
						return true;
					}
					if (**i == '4' and *(*i + 1) == '0' and *(*i + 2) == ';') {
						(*i) += 3;
						*resultChar = '(';
						return true;
					}
					if (**i == '4' and *(*i + 1) == '1' and *(*i + 2) == ';') {
						(*i) += 3;
						*resultChar = ')';
						return true;
					}
				}
				
				// if we get here - BAD FORMAT
				{
					*i = end;
					return false;		// bad format!
				}

			}
			else {
				*resultChar = **i;
				(*i)++;
				return true;
			}
		}
}
wstring			Characters::StringVectorEncoding::Encode (const vector<wstring>& v)
{
	/*
	 *	use LISP-LIKE SYNTAX for encoding.
	 */
	wstring	result;
	result.reserve (v.size () *  10 + 10);

	result += L"(";
	for (vector<wstring>::const_iterator i = v.begin (); i != v.end (); ++i) {
		Containers::ReserveSpeedTweekAddN (result, i->length () + 25);
		result += L"(";
		{
			result += ENCODE (*i);
		}
		result += L")";
	}
	result += L")";

	Assert (Decode (result) == v);
	return result;
}

vector<wstring>	Characters::StringVectorEncoding::Decode (const wstring& encodedString)
{
	vector<wstring>	result;

	if (encodedString.size () > 4 and encodedString[0] == '(' and encodedString[encodedString.length ()-1] == ')') {
		/*
		 *	Look for (XXX) - open paren, chars (other than close paren, and then close paren to end string. Then - if more in string - look
		 *	for next string.
		 */
		bool	inString	=	false;
		wstring	curAccumulatedString;
		for (wstring::const_iterator i = encodedString.begin () + 1; i+1 != encodedString.end ();) {
			// Better be looking at open paren - start of 
			if (inString) {
				if (*i == ')') {
					result.push_back (curAccumulatedString);
					inString = false;
					 ++i;
				}
				else {
					wchar_t	c	=	'\0';
					if (DECODE (&i, encodedString.end (), &c)) {
						Containers::ReserveSpeedTweekAdd1 (curAccumulatedString);
						curAccumulatedString.push_back (c);
					}
					else {
						// bad string - maybe should report exception - but dont bother for now - LGP 2006-04-15
						Assert (false);
						return vector<wstring> ();
					}
				}
			}
			else {
				if (*i == '(') {
					inString = true;
					curAccumulatedString.clear ();
					++i;
				}
				else {
					// bad string - maybe should report exception - but dont bother for now - LGP 2006-04-15
					Assert (false);
					return vector<wstring> ();
				}
			}
		}
	}
	return result;
}

