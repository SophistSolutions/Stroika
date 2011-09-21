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

