/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<cstdarg>
#include	<cstdlib>
#include	<iomanip>
#include	<sstream>

#include	"../Characters/StringUtils.h"
#include	"../Containers/Common.h"
#include	"../Debug/Assertions.h"
#include	"../Debug/Trace.h"

#include	"Words.h"




using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;
using	namespace	Stroika::Foundation::Linguistics;




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
string	Linguistics::PluralizeNoun (const string& s, int count)
{
	return PluralizeNoun_HLPR (s, count);
}

wstring	Linguistics::PluralizeNoun (const wstring& s, int count)
{
	return PluralizeNoun_HLPR (s, count);
}

string	Linguistics::PluralizeNoun (const string& s, const string& sPlural, int count)
{
	return count == 1? s: sPlural;
}

wstring	Linguistics::PluralizeNoun (const wstring& s, const wstring& sPlural, int count)
{
	return count == 1? s: sPlural;
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
wstring	Linguistics::MungeStringSoSingular (const wstring& s)
{
	return MungeStringSoSingular_ (s);
}

string	Linguistics::MungeStringSoSingular (const string& s)
{
	return MungeStringSoSingular_ (s);
}








/*
 ********************************************************************************
 ************************************ CapitalizeEachWord ************************
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
				for (typename STR::iterator i = r.begin (); i != r.end (); ++i) {
					if (prevCharSpace) {
						*i = TOUPPER (*i);
					}
					prevCharSpace = ISSPACE (*i);
				}
				return r;
			}
}
wstring	Linguistics::CapitalizeEachWord (const wstring& s)
{
	return CapitalizeEachWord_ (s);
}

string	Linguistics::CapitalizeEachWord (const string& s)
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
				for (typename STR::iterator i = r.begin (); i != r.end (); ++i) {
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
wstring	Linguistics::CapitalizeEachSentence (const wstring& s)
{
	return CapitalizeEachSentence_ (s);
}

string	Linguistics::CapitalizeEachSentence (const string& s)
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
wstring	Linguistics::UnCapitalizeFirstWord (const wstring& s)
{
	return UnCapitalizeFirstWord_ (s);
}

string	Linguistics::UnCapitalizeFirstWord (const string& s)
{
	return UnCapitalizeFirstWord_ (s);
}







/*
 ********************************************************************************
 ********************************** IsAllCaps ***********************************
 ********************************************************************************
 */
bool	Linguistics::IsAllCaps (const string& s)
{
	return not s.empty () and s == toupper (s);
}

bool	Linguistics::IsAllCaps (const wstring& s)
{
	return not s.empty () and s == toupper (s);
}




