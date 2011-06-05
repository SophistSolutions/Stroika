/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<iomanip>
#include	<sstream>

#include	"../Debug/Assertions.h"
#include	"CodePage.h"
#include	"../Debug/Trace.h"
#include	"../Math/Basics.h"

#include	"StringUtils.h"


using	namespace	Stroika;
using	namespace	Stroika::Foundation;

using	namespace	StringUtils;




/*
 ********************************************************************************
 ***************************** ACPStringToWide **********************************
 ********************************************************************************
 */
wstring	StringUtils::ACPStringToWide (const string& ws)
{
	int newStrLen = ::MultiByteToWideChar (::GetACP (), 0, ws.c_str (), ws.size (), NULL, NULL);
	wstring	result;
	result.resize (newStrLen);
	Verify (::MultiByteToWideChar (::GetACP (), 0, ws.c_str (), ws.size (), Containers::Start (result), newStrLen) == newStrLen);
	return result;
}






/*
 ********************************************************************************
 **************************** UTF8StringToBSTR **********************************
 ********************************************************************************
 */
BSTR	StringUtils::UTF8StringToBSTR (const char* ws)
{
	RequireNotNil (ws);
	size_t	wsLen	=	::strlen (ws);
	int stringLength = ::MultiByteToWideChar (CP_UTF8, 0, ws, wsLen, NULL, NULL);
	BSTR result	= ::SysAllocStringLen (NULL, stringLength);
	if (result == NULL) {
		DbgTrace ("UTF8StringToBSTR () out of memory - throwing bad_alloc");
		throw bad_alloc ();
	}
	Verify (::MultiByteToWideChar (CP_UTF8, 0, ws, wsLen, result, stringLength) == stringLength);
	return result;
}




/*
 ********************************************************************************
 *********************************** tolower ************************************
 ********************************************************************************
 */
void	StringUtils::tolower (wstring* s)
{
	RequireNotNil (s);
	wstring::iterator	end	=	s->end ();
	for (wstring::iterator i = s->begin (); i != end; ++i) {
		*i = TOLOWER (*i);
	}
}

void	StringUtils::tolower (string* s)
{
	RequireNotNil (s);
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
void	StringUtils::toupper (wstring* s)
{
	RequireNotNil (s);
	wstring::iterator	end	=	s->end ();
	for (wstring::iterator i = s->begin (); i != end; ++i) {
		*i = TOUPPER (*i);
	}
}

void	StringUtils::toupper (string* s)
{
	RequireNotNil (s);
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
bool	StringUtils::Contains (const wstring& string2Search, const wstring& substr, StringCompareOptions co)
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
			if (::_wcsnicmp (string2Search.c_str () + i, substr.c_str (), substr.length ()) == 0) {
				return true;
			}
		}
		return false;
	}
}




/*
 ********************************************************************************
 **************************** MungeStringSoSingular *****************************
 ********************************************************************************
 */
namespace	{
	template	<typename STR>
		inline	STR	MungeStringSoSingular_ (const STR& s)
			{
				STR	r	=	s;
				// take an ENGLISH string (assume English) - and munge it so its singular (if it happened to have been plural)

				// hande special case of 'wives' -> 'wife' as in 'midwives'
				if (r.length () >= 5) {
					size_t	l	=	r.length ();
					if (s[l-5] == 'w' and s[l-4] == 'i' and s[l-3] == 'v' and s[l-2] == 'e' and s[l-1] == 's') {
						r = r.substr (0, l-3);
						r.push_back ('f');
						r.push_back ('e');
						return r;
					}
				}

				// trim trailing s from the name (if prev letter is a non-s consonant)
				// or change 'ies' to 'y' at end
				if (s.length () > 3) {
					if (s[s.length ()-3] == 'i' and s[s.length ()-2] == 'e' and s[s.length ()-1]== 's') {
						//r = s.substr (0, s.length () - 3) + "y";
						r = s.substr (0, s.length () - 3);
						r.push_back ('y');
					}
					else if (s.length () > 4 and s[s.length ()-4] == 's' and s[s.length ()-3] == 's' and s[s.length ()-2] == 'e' and s[s.length ()-1]== 's') {
						r = s.substr (0, s.length () - 2);
					}
					else if (s[s.length ()-3] == 's' and s[s.length ()-2] == 'e' and s[s.length ()-1]== 's') {
						r = s.substr (0, s.length () - 1);
					}
					// because of diabets mellitus - (and others???? - don't map trailing 'us' to 'u'
					else if (s[s.length ()-1] == 's' and isascii (s[s.length ()-2]) and isalpha (s[s.length ()-2]) and (s[s.length ()-2]!= 's' and s[s.length ()-2]!= 'u')) {
						r = s.substr (0, s.length () - 1);
					}
				}
				return r;
			}
}
wstring	StringUtils::MungeStringSoSingular (const wstring& s)
{
	return MungeStringSoSingular_ (s);
}

string	StringUtils::MungeStringSoSingular (const string& s)
{
	return MungeStringSoSingular_ (s);
}






/*
 ********************************************************************************
 ********************************* MungeStringSoSingular ************************
 ********************************************************************************
 */
namespace	{
	template	<typename STR>
		inline	STR	CapitalizeEachWord_ (const STR& s)
			{
				STR	r	=	s;
				// take an ENGLISH string (assume English)

				// toupper each lower-case character preceeded by a space
				bool	prevCharSpace	=	true;	// so we upper first char
				for (STR::iterator i = r.begin (); i != r.end (); ++i) {
					if (prevCharSpace) {
						*i = TOUPPER (*i);
					}
					prevCharSpace = ISSPACE (*i);
				}
				return r;
			}
}
wstring	StringUtils::CapitalizeEachWord (const wstring& s)
{
	return CapitalizeEachWord_ (s);
}

string	StringUtils::CapitalizeEachWord (const string& s)
{
	return CapitalizeEachWord_ (s);
}




/*
 ********************************************************************************
 ******************************* CapitalizeEachSentence *************************
 ********************************************************************************
 */
namespace	{
	template	<typename STR>
		inline	STR	CapitalizeEachSentence_ (const STR& s)
			{
				// WAY too kludgly - but hopefully adequate for primitive message cleanups...
				//		-- LGP 2008-09-20
				STR	r	=	s;
				// take an ENGLISH string (assume English)

				// toupper each lower-case character preceeded by a ENDOFSENTECE PUNCT
				bool	nextCharStartsSentence	=	true;	// so we upper first char
				for (STR::iterator i = r.begin (); i != r.end (); ++i) {
					if (nextCharStartsSentence and !ISSPACE (*i)) {
						*i = TOUPPER (*i);
						nextCharStartsSentence = false;
					}
					else {
						nextCharStartsSentence |= (*i == '.' or *i == '!' or *i == '?');
					}
				}
				return r;
			}
}
wstring	StringUtils::CapitalizeEachSentence (const wstring& s)
{
	return CapitalizeEachSentence_ (s);
}

string	StringUtils::CapitalizeEachSentence (const string& s)
{
	return CapitalizeEachSentence_ (s);
}





/*
 ********************************************************************************
 ******************************** UnCapitalizeFirstWord *************************
 ********************************************************************************
 */
namespace	{
	template	<typename STR>
		inline	STR	UnCapitalizeFirstWord_ (const STR& s)
			{
				// WAY too kludgly - but hopefully adequate for primitive message cleanups...
				//		-- LGP 2008-09-20
				STR	r	=	s;
				if (r.length () > 2) {
					if (TOUPPER (r[0])==r[0] and TOUPPER (r[1]) != r[1]) {
						r[0] = TOLOWER (r[0]);
					}
				}
				return r;
			}
}
wstring	StringUtils::UnCapitalizeFirstWord (const wstring& s)
{
	return UnCapitalizeFirstWord_ (s);
}

string	StringUtils::UnCapitalizeFirstWord (const string& s)
{
	return UnCapitalizeFirstWord_ (s);
}






/*
 ********************************************************************************
 ********************************** IsAllCaps ***********************************
 ********************************************************************************
 */
bool	StringUtils::IsAllCaps (const string& s)
{
	return not s.empty () and s == toupper (s);
}

bool	StringUtils::IsAllCaps (const wstring& s)
{
	return not s.empty () and s == toupper (s);
}







/*
 ********************************************************************************
 ************************* QuoteForXMLAttribute *********************************
 ********************************************************************************
 */
string	StringUtils::QuoteForXMLAttribute (const string& s)
{
	string	r;
	r.reserve (s.size () * 6 / 5);	// wild guess about good estimate
	for (string::const_iterator i = s.begin (); i != s.end (); ++i) {
		Containers::ReserveSpeedTweekAdd1 (r);
		switch (*i) {
			case	'&':	r += "&amp;"; break;
			case	'<':	r += "&lt;"; break;
			case	'>':	r += "&gt;"; break;
			case	'\"':	r += "&quot;"; break;
			case	'\'':	r += "&apos;"; break;
			default:		r.push_back (*i); break;
		}
	}
	return r;
}

string	StringUtils::QuoteForXMLAttribute (const wstring& s)
{
	string	r;
	r.reserve (s.size () * 7 / 5);	// wild guess about good estimate
	for (wstring::const_iterator i = s.begin (); i != s.end (); ++i) {
		Containers::ReserveSpeedTweekAdd1 (r);
		switch (*i) {
			case	'&':	r += "&amp;"; break;
			case	'<':	r += "&lt;"; break;
			case	'>':	r += "&gt;"; break;
			case	'\"':	r += "&quot;"; break;
			case	'\'':	r += "&apos;"; break;
			default:		{
				wchar_t	ccode	=	*i;
				if (ccode != '\t' and ccode != '\n' and ccode != '\r' and (ccode < 32 || ccode > 127)) {
					r +=  Format ("&#%d;", ccode);
				}
				else {
					r.push_back (static_cast<char> (ccode));
				}
			}
			break;
		}
	}
	return r;
}

wstring	StringUtils::QuoteForXMLAttributeW (const wstring& s)
{
	string	tmp	=	QuoteForXMLAttribute (s);
	return ACPStringToWide (tmp);
}






/*
 ********************************************************************************
 ******************************** QuoteForXML ***********************************
 ********************************************************************************
 */
string	StringUtils::QuoteForXML (const string& s)
{
	string	r;
	r.reserve (s.size () * 6 / 5);	// wild guess about good estimate
	for (string::const_iterator i = s.begin (); i != s.end (); ++i) {
		Containers::ReserveSpeedTweekAdd1 (r);
		switch (*i) {
			case	'&':	r += "&amp;"; break;
			case	'<':	r += "&lt;"; break;
			case	'>':	r += "&gt;"; break;
			case	'-':	{
				// A 'dash' or 'minus-sign' can be dangerous in XML - if you get two in a row (start/end comment designator).
				// So avoid any leading/trailing dash, and any double-dashes.
				//
				// NB: This code WOULD be simpler if we just always mapped, but then much ordinary and safe text with dashes becomes
				// less readable (and produces huge diffs in my regression tests - but thats a one-time annoyance).
				//
				if ((i == s.begin ()) or (i + 1 == s.end ()) or (*(i-1) == '-')) {
					r += "&#45;";
				}
				else {
					r.push_back ('-');
				}
			}
			break;
			default:		r.push_back (*i); break;
		}
	}
	return r;
}

string	StringUtils::QuoteForXML (const wstring& s)
{
	string	r;
	r.reserve (s.size () * 7 / 5);	// wild guess about good estimate
	for (wstring::const_iterator i = s.begin (); i != s.end (); ++i) {
		Containers::ReserveSpeedTweekAdd1 (r);
		switch (*i) {
			case	'&':	r += "&amp;"; break;
			case	'<':	r += "&lt;"; break;
			case	'>':	r += "&gt;"; break;
			case	'-':	{
				// A 'dash' or 'minus-sign' can be dangerous in XML - if you get two in a row (start/end comment designator).
				// So avoid any leading/trailing dash, and any double-dashes.
				//
				// NB: This code WOULD be simpler if we just always mapped, but then much ordinary and safe text with dashes becomes
				// less readable (and produces huge diffs in my regression tests - but thats a one-time annoyance).
				//
				if ((i == s.begin ()) or (i + 1 == s.end ()) or (*(i-1) == '-')) {
					r += "&#45;";
				}
				else {
					r.push_back ('-');
				}
			}
			break;
			default:		{
				wchar_t	ccode	=	*i;
				if (ccode != '\t' and ccode != '\n' and ccode != '\r' and (ccode < 32 || ccode > 127)) {
					r +=  Format ("&#%d;", ccode);
				}
				else {
					r.push_back (static_cast<char> (ccode));
				}
			}
			break;
		}
	}
	return r;
}

wstring	StringUtils::QuoteForXMLW (const wstring& s)
{
	string	tmp	=	QuoteForXML (s);
	return ACPStringToWide (tmp);
}






/*
 ********************************************************************************
 ************************************* Format ***********************************
 ********************************************************************************
 */
string	StringUtils::Format (const char* format, ...)
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

wstring	StringUtils::Format (const wchar_t* format, ...)
{
	wchar_t		msgBuf [10*1024];	// no idea how big to make it...
	va_list		argsList;
	va_start (argsList, format);
	#if		__STDC_WANT_SECURE_LIB__
	//	(void)::vswprintf_s (msgBuf, format, argsList);
		(void)::_vsnwprintf_s (msgBuf, NEltsOf (msgBuf), _TRUNCATE, format, argsList);
	#else
		(void)::vsnwprintf (msgBuf, NEltsOf (msgBuf), format, argsList);
	#endif
	va_end (argsList);
	Assert (::wcslen (msgBuf) < NEltsOf (msgBuf));
	return msgBuf;
}





/*
 ********************************************************************************
 ****************************** PluralizeNoun ***********************************
 ********************************************************************************
 */
namespace	{
	template	<typename STRING>
		inline	STRING	PluralizeNoun_HLPR (const STRING& str, int count)
			{
				// Implement VERY WEAK ENGLISH rules for now... (ignores y ->ies, and other cases too)
				if (count == 1) {
					return str;
				}
				else {
					STRING	tmp	=	str;
					tmp.push_back ('s');
					return tmp;
				}
			}
}
string	StringUtils::PluralizeNoun (const string& s, int count)
{
	return PluralizeNoun_HLPR (s, count);
}

wstring	StringUtils::PluralizeNoun (const wstring& s, int count)
{
	return PluralizeNoun_HLPR (s, count);
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

string	StringUtils::StripTrailingCharIfAny (const string& s, char c)
{
	return StripTrailingCharIfAny_HLPR (s, c);
}

wstring	StringUtils::StripTrailingCharIfAny (const wstring& s, wchar_t c)
{
	return StripTrailingCharIfAny_HLPR (s, c);
}












/*
 ********************************************************************************
 *********************** MapUNICODETextWithMaybeBOMTowstring ********************
 ********************************************************************************
 */
wstring	StringUtils::MapUNICODETextWithMaybeBOMTowstring (const char* start, const char* end)
{
	Require (start <= end);
	if (start == end) {
		return wstring ();
	}
	else {
		size_t	outBufSize		=	end-start;
		SmallStackBuffer<wchar_t>	wideBuf (outBufSize);
		size_t	outCharCount	=	outBufSize;
		CodePageUtils::MapSBUnicodeTextWithMaybeBOMToUNICODE (start, end-start, wideBuf, &outCharCount);
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
vector<Byte>	StringUtils::MapUNICODETextToSerializedFormat (const wchar_t* start, const wchar_t* end, CodePage useCP)
{
	using	namespace	CodePageUtils;
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
int	StringUtils::HexString2Int (const string& s)
{
	int	v	=	0;
	(void)::sscanf_s (s.c_str (), "%x", &v);
	return v;
}

int	StringUtils::HexString2Int (const wstring& s)
{
	int	v	=	0;
	(void)::swscanf_s (s.c_str (), L"%x", &v);
	return v;
}





/*
 ********************************************************************************
 ********************************* String2Int ***********************************
 ********************************************************************************
 */
int	StringUtils::String2Int (const string& s)
{
	return atol (s.c_str ());
}

int	StringUtils::String2Int (const wstring& s)
{
	return _wtol (s.c_str ());
}





/*
 ********************************************************************************
 ********************************* String2Float *********************************
 ********************************************************************************
 */
float	StringUtils::String2Float (const wstring& s)
{
	static	const	float	kBADVAL	=	static_cast<float> (Math::nan ());
	return String2Float (s, kBADVAL);
}

float	StringUtils::String2Float (const wstring& s, float returnValIfInvalidString)
{
	float	num	=	returnValIfInvalidString;
	if (::swscanf_s (s.c_str (), L"%f", &num) == 1) {
		return num;
	}
	return returnValIfInvalidString;
}







/*
 ********************************************************************************
 ********************************* Float2String *********************************
 ********************************************************************************
 */
wstring	StringUtils::Float2String (float f, unsigned int precision)
{
	if (_isnan (f)) {
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
string	StringUtils::LimitLength (const string& str, size_t maxLen, bool keepLeft)
{
	return LimitLength_HLPR<string> (str, maxLen, keepLeft, "...");
}

wstring	StringUtils::LimitLength (const wstring& str, size_t maxLen, bool keepLeft)
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
			RequireNotNil (resultChar);
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
wstring			StringUtils::StringVectorEncoding::Encode (const vector<wstring>& v)
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

vector<wstring>	StringUtils::StringVectorEncoding::Decode (const wstring& encodedString)
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

