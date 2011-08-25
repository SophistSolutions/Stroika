/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../StroikaPreComp.h"

#if	qPlatform_Windows
	#include	<atlbase.h>

	#include	<Windows.h>
	#include	<URLMon.h>
#endif

#include	"../../Execution/Exceptions.h"

#include	"URL.h"

using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;

#if	qPlatform_Windows
using	Stroika::Foundation::Execution::ThrowIfErrorHRESULT;
#endif

using	namespace	Stroika::Foundation::IO;
using	namespace	Stroika::Foundation::IO::Network;


namespace	{
	inline	int	ConvertReadSingleHexDigit_ (char digit)
		{
			if (isupper (digit)) {
				digit = tolower (digit);
			}
			if (isdigit (digit)) {
				return digit - '0';
			}
			else if (islower (digit)) {
				return 10 + (digit - 'a');
			}
			else {
				return 0;
			}
		}
}


/*
 ********************************************************************************
 ********************* Network::GetDefaultPortForProtocol ***********************
 ********************************************************************************
 */
int		Network::GetDefaultPortForProtocol (const wstring& proto)
{
	// From http://www.iana.org/assignments/port-numbers
	if (proto == L"")		{		return 80;	}
	if (proto == L"http")	{		return 80;	}
	if (proto == L"https")	{		return 443;	}
	if (proto == L"ldap")	{		return 389;	}
	if (proto == L"ldaps")	{		return 636;	}
	if (proto == L"ftp")	{		return 21;	}
	if (proto == L"ftps")	{		return 990;	}

	Assert (false);	// if this ever happens - we probably have some work todo - the above list is inadequate
	return 80;	// hack...
}





/*
 ********************************************************************************
 ********************************** URLCracker **********************************
 ********************************************************************************
 */
#if	qPlatform_Windows
namespace	{
	void	OLD_Cracker (const wstring& w, wstring* protocol, wstring* host, wstring* port, wstring* relPath, wstring* query)
	{
		RequireNotNil (protocol);
		RequireNotNil (host);
		RequireNotNil (relPath);
		RequireNotNil (query);

		DWORD	ingored	=	0;
		wchar_t	outBuf[10*1024];

		wstring	canonical;
		ThrowIfErrorHRESULT (::CoInternetParseUrl (CComBSTR (w.c_str ()), PARSE_CANONICALIZE, 0, outBuf, NEltsOf (outBuf), &ingored, 0));
		canonical = outBuf;

		{
			size_t	e	=	canonical.find (':');
			if (e != wstring::npos) {
				*protocol = canonical.substr (0, e);
			}
		}

		if (SUCCEEDED (::CoInternetParseUrl (CComBSTR (canonical.c_str ()), PARSE_DOMAIN, 0, outBuf, NEltsOf (outBuf), &ingored, 0))) {
			*host = outBuf;
		}

		// I cannot see how to get other fields using CoInternetParseURL??? - LGP 2004-04-13...
		{
			wstring	matchStr		=	*protocol + L"://" + *host;
			size_t	startOfPath		=	canonical.find (matchStr);
			if (startOfPath == wstring::npos) {
				matchStr		=	*protocol + L":";
				startOfPath		=	canonical.find (matchStr);
			}
			if (startOfPath == wstring::npos) {
				startOfPath = canonical.length ();
			}
			else {
				startOfPath += matchStr.length ();
			}
			*relPath = canonical.substr (startOfPath);

			size_t	startOfQuery	=	relPath->find ('?');
			if (startOfQuery != wstring::npos) {
				*query = relPath->substr (startOfQuery + 1);
				relPath->erase (startOfQuery);
			}
		}
	}
}
#endif
URLCracker::URLCracker (const wstring& w):
	fProtocol (),
	fHost (),
	fPort (kDefaultPort),
	fRelPath (),
	fQuery (),
	fFragment ()
{
	if (w.empty ()) {
		return;
	}

	/*
	 *	We MIGHT need to canonicalize the URL:
	 *			ThrowIfErrorHRESULT (::CoInternetParseUrl (CComBSTR (w.c_str ()), PARSE_CANONICALIZE, 0, outBuf, NEltsOf (outBuf), &ingored, 0));
	 *	But empirically, so far, its slow, and doesn't appear to be doing anything. It MIGHT be used in case where we get a URL
	 *	with %nn encodings, and they need to be rewritten. But even that doesn't appear critical here. Consider doing that if we
	 *	ever get diffs with the OLD_Cracker () reported.
	 */

	{
		size_t	e	=	w.find (':');
		if (e != wstring::npos) {
			fProtocol = tolower (w.substr (0, e));
		}
	}

	size_t i	=	0;
	{
		size_t	len				=	w.length ();
		size_t	hostNameStart	=	0;		// default with hostname at start of URL, unless there is a PROTOCOL: in front
		size_t	e				=	w.find (':');
		if (e != wstring::npos) {
			hostNameStart = e + 1;
		}
		i = hostNameStart;

		// Look for // and assume whats after is a hostname, and otherwise, the rest is a relative url
		// (REALLY UNSURE ABOUT THIS LOGIC - HAVENT FOUND GOOD DOCS ON THE NET FOR THIS FORMAT
		//		-- LGP 2006-01-24

		if (hostNameStart + 2 < len and w[hostNameStart] == '/' and w[hostNameStart+1] == '/') {
			// skip '//' before hostname
			hostNameStart++;
			hostNameStart++;

			// then hostname extends to EOS, or first colon (for port#) or first '/'
			i = hostNameStart;
			for (; i != w.length (); ++i) {
				wchar_t	c	=	w[i];
				if (c == ':' or c == '/' or c == '\\') {
					break;
				}
			}
			size_t	endOfHost		=	i;
			fHost = w.substr (hostNameStart, endOfHost-hostNameStart);
			
			// COULD check right here for port# if c == ':' - but dont bother since never did before - and this is apparantly good enuf for now...
			if (i < w.length ()) {
				if (w[i]==':') {
					wstring num;
					++i;
					while (i < w.length () && isdigit (w[i])) {
						num.push_back (w[i]);
						++i;
					}
					if (!num.empty ()) {
						fPort = String2Int (num);
					}
				}
			}
		}
	}

	{
		fRelPath = w.substr (i);

		// It should be RELATIVE to that hostname and the slash is the separator character
		// NB: This is a change as of 2008-09-04 - so be careful in case anyone elsewhere dependend
		// on the leading slash!
		//		-- LGP 2008-09-04
		if (not fRelPath.empty () and fRelPath[0] == '/') {
			fRelPath = fRelPath.substr (1);
		}

		size_t	startOfFragment	=	fRelPath.find ('#');
		if (startOfFragment != wstring::npos) {
			fFragment = fRelPath.substr (startOfFragment + 1);
			fRelPath.erase (startOfFragment);
		}

		size_t	startOfQuery	=	fRelPath.find ('?');
		if (startOfQuery != wstring::npos) {
			fQuery = fRelPath.substr (startOfQuery + 1);
			fRelPath.erase (startOfQuery);
		}
	}


#if		qDebug && qPlatform_Windows
	{
		wstring	testProtocol;
		wstring	testHost;
		wstring	testPort;
		wstring	testRelPath;
		wstring	testQuery;
		OLD_Cracker (w, &testProtocol, &testHost, &testPort, &testRelPath, &testQuery);
		Assert (testProtocol == fProtocol);
		if (testProtocol == L"http") {
			Assert (testHost == tolower (fHost));
			{
				//Assert (testPort == fPort);
				if (fPort == 80) {
					Assert (testPort == L"" or testPort == L"80");
				}
				else {
					// apparently never really implemented in old cracker...
					//Assert (fPort == ::_wtoi (testPort.c_str ()));
				}
			}
			Assert (testRelPath == fRelPath || testRelPath.find (':') != wstring::npos || ((L"/" + fRelPath) == testRelPath));	//old code didnt handle port#	--LGP 2007-09-20
			Assert (testQuery == fQuery or not fFragment.empty ());	// old code didn't check fragment
		}
	}
#endif
}

bool	URLCracker::IsSecure () const
{
	// should be large list of items - and maybe do soemthing to assure case matching handled properly, if needed?
	return fProtocol==L"https";
}

wstring	URLCracker::GetURL () const
{
	wstring	result;
	result.reserve (10 + fHost.length () + fRelPath.length () + fQuery.length () + fFragment.length ());

	if (fProtocol.empty ()) {
		result += L"http:";
	}
	else {
		result += fProtocol + L":";
	}

	if (not fHost.empty ()) {
		result += L"//" + fHost;
		if (fPort != kDefaultPort and fPort != GetDefaultPortForProtocol (fProtocol)) {
			result += Format (L":%d", fPort);
		}
		result +=  L"/";
	}

	result += fRelPath;

	if (not fQuery.empty ()) {
		result += L"?" + fQuery;
	}

	if (not fFragment.empty ()) {
		result += L"#" + fFragment;
	}

	return result;
}

wstring	URLCracker::GetHostRelPathDir () const
{
	wstring	result	=	fRelPath;
	size_t	i	=	result.rfind ('/');
	if (i == wstring::npos) {
		result.clear ();
	}
	else {
		result.erase (i + 1);
	}
	return result;
}

bool	Network::operator== (const URLCracker& lhs, const URLCracker& rhs)
{
	// A simpler way to compare - and probably better - is if they both produce the same URL string, they are the
	// same URL (since GetURL normalizes output)
	//	-- LGP 2009-01-17
	return lhs.GetURL () == rhs.GetURL ();
}

#if 0
		HRESULT	hr	=	::CoInternetParseUrl (url.c_str (), ParseAction, dwParseFlags, pwzResult, cchResult, pcchResult, dwReserved);

  protocol. This section defines the scheme-specific syntax and
   semantics for http URLs.

   http_URL = "http:" "//" host [ ":" port ] [ abs_path [ "?" query ]]

   If the port is empty or not given, port 80 is assumed. The semantics
   are that the identified resource is located at the server listening
   for TCP connections on that port of that host, and the Request-URI
   for the resource is abs_path (section 5.1.2). The use of IP addresses
   in URLs SHOULD be avoided whenever possible (see RFC 1900 [24]). If
   the abs_path is not present in the URL, it MUST be given as "/" when
   used as a Request-URI for a resource (section 5.1.2). If a proxy
   receives a host name which is not a fully qualified domain name, it
   MAY add its domain to the host name it received. If a proxy receives
   a fully qualified domain name, the proxy MUST NOT change the host
   name.








Fielding, et al.            Standards Track                    [Page 19]

RFC 2616                        HTTP/1.1                       June 1999


3.2.3 URI Comparison

   When comparing two URIs to decide if they match or not, a client
   SHOULD use a case-sensitive octet-by-octet comparison of the entire
   URIs, with these exceptions:

      - A port that is empty or not given is equivalent to the default
        port for that URI-reference;

        - Comparisons of host names MUST be case-insensitive;

        - Comparisons of scheme names MUST be case-insensitive;

        - An empty abs_path is equivalent to an abs_path of "/".

   Characters other than those in the "reserved" and "unsafe" sets (see
   RFC 2396 [42]) are equivalent to their ""%" HEX HEX" encoding.

   For example, the following three URIs are equivalent:

      http://abc.com:80/~smith/home.html
      http://ABC.com/%7Esmith/home.html
      http://ABC.com:/%7esmith/home.html
#endif





/*
 ********************************************************************************
 *********************** EncodeURLQueryStringField ******************************
 ********************************************************************************
 */
string	Network::EncodeURLQueryStringField (const wstring& s)
{
	//
	// According to http://tools.ietf.org/html/rfc3986 - URLs need to be treated as UTF-8 before
	// doing % etc substitution
	//
	// From http://tools.ietf.org/html/rfc3986#section-2.3
	//		unreserved  = ALPHA / DIGIT / "-" / "." / "_" / "~"
	string	utf8Query	=	WideStringToUTF8 (s);
	string	result;
	size_t	sLength = utf8Query.length ();
	result.reserve (sLength);
	for (size_t i = 0; i < sLength; ++i) {
		Containers::ReserveSpeedTweekAdd1 (result);
		switch (utf8Query[i]) {
			case ' ': result += "+"; break;
			default:  {
				wchar_t	ccode	=	utf8Query[i];
				if (isascii (ccode) and (isalnum (ccode) or (ccode == '-') or (ccode == '.') or (ccode == '_') or (ccode == '~'))) {
					result += static_cast<char> (utf8Query[i]);
				}
				else {
					result +=  Format ("%%%.2x", ccode);
				}
			}
		}
	}
	return result;
}






/*
 ********************************************************************************
 ********************************* URLQueryDecoder ******************************
 ********************************************************************************
 */
namespace	{
	// According to http://tools.ietf.org/html/rfc3986 - URLs need to be treated as UTF-8 before
	// doing % etc substitution
	void	InitURLQueryDecoder_ (map<wstring,wstring>* m, const string& utf8Query)
		{
			size_t	utfqLen	=	utf8Query.length ();
			for (size_t i = 0; i < utfqLen; ) {
				size_t	e	=	utf8Query.find ('&', i);
				string	elt	=	utf8Query.substr (i, e-i);
				size_t	brk	=	elt.find('=');
				if (brk != string::npos) {
					string	val	=	elt.substr (brk+1);
					for (string::iterator i = val.begin (); i != val.end (); ++i) {
						switch (*i) {
							case	'+':	*i = ' '; break;
							case	'%': {
								if (i + 2 < val.end ()) {
									unsigned char	newC	=	(ConvertReadSingleHexDigit_ (*(i+1)) << 4) + ConvertReadSingleHexDigit_ (*(i+2));
									i = val.erase (i, i+2);
									*i = static_cast<char> (newC);
								}
								break;
							}
						}
					}
					m->insert (map<wstring,wstring>::value_type (UTF8StringToWide (elt.substr (0, brk)), UTF8StringToWide (val)));
				}
				if (e == wstring::npos) {
					break;
				}
				i = e + 1;
			}
		}
}
URLQueryDecoder::URLQueryDecoder (const wstring& query):
	fMap ()
{
	InitURLQueryDecoder_ (&fMap, WideStringToUTF8 (query));
}

URLQueryDecoder::URLQueryDecoder (const string& query):
	fMap ()
{
	InitURLQueryDecoder_ (&fMap, query);
}

void	URLQueryDecoder::RemoveFieldIfAny (const wstring& idx)
{
	map<wstring,wstring>::iterator	i	=	fMap.find (idx);
	if (i != fMap.end ()) {
		fMap.erase (i);
	}
}

wstring	URLQueryDecoder::ComputeQueryString () const
{
	string	result;
	for (map<wstring,wstring>::const_iterator i = fMap.begin (); i != fMap.end (); ++i) {
		Containers::ReserveSpeedTweekAdd1 (result);
		if (not result.empty ()) {
			result += "&";
		}
		//careful - need to encode first/second
		result += EncodeURLQueryStringField (i->first) + "=" + EncodeURLQueryStringField (i->second);
	}
	return ASCIIStringToWide (result);
}

