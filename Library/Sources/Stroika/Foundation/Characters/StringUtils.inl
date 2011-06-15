/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Characters_StringUtils_inl_
#define	_Stroika_Foundation_Characters_StringUtils_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../Containers/Basics.h"
#include	"../Memory/SmallStackBuffer.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Characters {

			//	TOLOWER
			inline	char	TOLOWER (char c)
				{
					#if		qUseASCIIRangeSpeedHack
						if ('A' <= c and c <= 'Z') {
							Ensure (std::tolower (c) == ((c - 'A') + 'a'));
							return (c - 'A') + 'a';
						}
						else if (c <= 127) {
							Ensure (std::tolower (c) == c);
							return c;
						}
						else {
							return std::tolower (c);
						}
					#else
						return std::tolower (c);
					#endif
				}
			inline	wchar_t	TOLOWER (wchar_t c)
				{
					#if		qUseASCIIRangeSpeedHack
						if ('A' <= c and c <= 'Z') {
							Ensure (towlower (c) == ((c - 'A') + 'a'));
							return (c - 'A') + 'a';
						}
						else if (c <= 127) {
							Ensure (towlower (c) == c);
							return c;
						}
						else {
							return towlower (c);
						}
					#else
						return towlower (c);
					#endif
				}

		//	TOUPPER
			inline	char	TOUPPER (char c)
				{
					#if		qUseASCIIRangeSpeedHack
						if ('a' <= c and c <= 'z') {
							Ensure (std::toupper (c) == ((c - 'a') + 'A'));
							return (c - 'a') + 'A';
						}
						else if (c <= 127) {
							Ensure (std::toupper (c) == c);
							return c;
						}
						else {
							return std::toupper (c);
						}
					#else
						return std::toupper (c);
					#endif
				}
			inline	wchar_t	TOUPPER (wchar_t c)
				{
					#if		qUseASCIIRangeSpeedHack
						if ('a' <= c and c <= 'z') {
							Ensure (towupper (c) == ((c - 'a') + 'A'));
							return (c - 'a') + 'A';
						}
						else if (c <= 127) {
							Ensure (towupper (c) == c);
							return c;
						}
						else {
							return towupper (c);
						}
					#else
						return towupper (c);
					#endif
				}

		
		//	ISSPACE
			inline	bool	ISSPACE (char c)
				{
					#if		qUseASCIIRangeSpeedHack
						if (0 < c and c <= 127) {
								//	space, tab, carriage return, newline, vertical tab or form feed
							bool	r	=	(c == ' ' or c == '\t' or c == '\r' or c == '\n' or c == '\v' or c == '\f');
							Ensure (!!isspace (c) == r);
							return r;
						}
						else {
							#pragma	warning (push)
							#pragma	warning (4 : 4800)
							return isspace (c);
							#pragma	warning (pop)
						}
					#else
						return isspace (c);
					#endif
				}
			inline	bool	ISSPACE (wchar_t c)
				{
					#if		qUseASCIIRangeSpeedHack
						if (0 < c and c <= 127) {
								//	space, tab, carriage return, newline, vertical tab or form feed
							bool	r	=	(c == ' ' or c == '\t' or c == '\r' or c == '\n' or c == '\v' or c == '\f');
							Ensure (!!iswspace (c) == r);
							return r;
						}
						else {
							#pragma	warning (push)
							#pragma	warning (4 : 4800)
							return iswspace (c);
							#pragma	warning (pop)
						}
					#else
						return iswspace (c);
					#endif
				}

		
		//	tolower
			inline	wstring	tolower (const wstring& s)
				{
					wstring	r	=	s;
					tolower (&r);
					return r;
				}
			inline	string	tolower (const string& s)
				{
					string	r	=	s;
					tolower (&r);
					return r;
				}

		//	toupper
			inline	wstring	toupper (const wstring& s)
				{
					wstring	r	=	s;
					toupper (&r);
					return r;
				}
			inline	string	toupper (const string& s)
				{
					string	r	=	s;
					toupper (&r);
					return r;
				}




			inline	void	WideStringToNarrow (const wchar_t* wsStart, const wchar_t* wsEnd, CodePage codePage, string* intoResult)
				{
					RequireNotNil (intoResult);
					Require (wsStart <= wsEnd);
					size_t	wsLen	=	(wsEnd - wsStart);
					int stringLength = ::WideCharToMultiByte (codePage, 0, wsStart, wsLen, NULL, NULL, NULL, NULL);
					intoResult->resize (stringLength);
					if (stringLength != 0) {
						Verify (::WideCharToMultiByte (codePage, 0, wsStart, wsLen, Containers::Start (*intoResult), stringLength, NULL, NULL) == stringLength);
					}
				}
			inline	void	WideStringToNarrow (const wstring& ws, CodePage codePage, string* intoResult)
				{
					RequireNotNil (intoResult);
					int stringLength = ::WideCharToMultiByte (codePage, 0, ws.c_str (), ws.size (), NULL, NULL, NULL, NULL);
					intoResult->resize (stringLength);
					if (stringLength != 0) {
						Verify (::WideCharToMultiByte (codePage, 0, ws.c_str (), ws.size (), Containers::Start (*intoResult), stringLength, NULL, NULL) == stringLength);
					}
				}
			inline	string	WideStringToNarrow (const wstring& ws, CodePage codePage)
				{
					string	result;
					WideStringToNarrow (ws, codePage, &result);
					return result;
				}
			inline	void	NarrowStringToWide (const char* sStart, const char* sEnd, int codePage, wstring* intoResult)
				{
					RequireNotNil (intoResult);
					Require (sStart <= sEnd);
					size_t	sLen	=	(sEnd - sStart);
					int newStrLen = ::MultiByteToWideChar (codePage, 0, sStart, sLen, NULL, NULL);
					intoResult->resize (newStrLen);
					if (newStrLen != 0) {
						Verify (::MultiByteToWideChar (codePage, 0, sStart, sLen, Containers::Start (*intoResult), newStrLen) == newStrLen);
					}
				}
			inline	void	NarrowStringToWide (const string& s, int codePage, wstring* intoResult)
				{
					RequireNotNil (intoResult);
					int newStrLen = ::MultiByteToWideChar (codePage, 0, s.c_str (), s.size (), NULL, NULL);
					intoResult->resize (newStrLen);
					if (newStrLen != 0) {
						Verify (::MultiByteToWideChar (codePage, 0, s.c_str (), s.size (), Containers::Start (*intoResult), newStrLen) == newStrLen);
					}
				}
			inline	wstring	NarrowStringToWide (const string& s, int codePage)
				{
					wstring	result;
					NarrowStringToWide (s, codePage, &result);
					return result;
				}






			inline	wstring	ASCIIStringToWide (const string& s)
				{
					#if		defined (_DEBUG)
						for (string::const_iterator i = s.begin (); i != s.end (); ++i) {
							Assert (isascii (*i));
						}
					#endif
					return wstring (s.begin (), s.end ());
				}
			inline	string	WideStringToASCII (const wstring& s)
				{
					#if		defined (_DEBUG)
						for (wstring::const_iterator i = s.begin (); i != s.end (); ++i) {
							Assert (isascii (*i));
						}
					#endif
					return string (s.begin (), s.end ());
				}
			inline	string	tstring2ANSI (const tstring& s)
				{
					#if		defined (UNICODE)
						return WideStringToACP (s);
					#else
						return s;
					#endif
				}
			inline	wstring	tstring2Wide (const tstring& s)
				{
					#if		defined (UNICODE)
						return s;
					#else
						return ACPStringToWide (s);
					#endif
				}
			inline	tstring	ANSI2tstring (const string& s)
				{
					#if		defined (UNICODE)
						return ACPStringToWide (s);
					#else
						return s;
					#endif
				}
			inline	tstring	Wide2tstring (const wstring& s)
				{
					#if		defined (UNICODE)
						return s;
					#else
						return WideStringToACP (s);
					#endif
				}
			inline	tstring	totstring (const string& s)
				{
					return ANSI2tstring (s);
				}
			inline	tstring	totstring (const wstring& s)
				{
					return Wide2tstring (s);
				}
			inline	string	WideStringToACP (const wstring& ws)
				{
					return WideStringToNarrow (ws, ::GetACP ());
				}
			inline	string	BSTRStringToUTF8 (const BSTR bstr)
				{
					if (bstr == NULL) {
						return string ();
					}
					else {
						int	srcStrLen	=	::SysStringLen (bstr);
						int stringLength = ::WideCharToMultiByte (CP_UTF8, 0, bstr, srcStrLen, NULL, NULL, NULL, NULL);
						string	result;
						result.resize (stringLength);
						Verify (::WideCharToMultiByte (CP_UTF8, 0, bstr, srcStrLen, Containers::Start (result), stringLength, NULL, NULL) == stringLength);
						return result;
					}
				}
			inline	string	WideStringToUTF8 (const wstring& ws)
				{
					return WideStringToNarrow (ws, CP_UTF8);
				}
			inline	void	UTF8StringToWide (const char* s, wstring* intoStr)
				{
					RequireNotNil (s);
					NarrowStringToWide (s, s + ::strlen (s), CP_UTF8, intoStr);
				}
			inline	void	UTF8StringToWide (const string& s, wstring* intoStr)
				{
					NarrowStringToWide (s, CP_UTF8, intoStr);
				}
			inline	wstring	UTF8StringToWide (const char* s)
				{
					RequireNotNil (s);
					wstring	result;
					NarrowStringToWide (s, s + ::strlen (s), CP_UTF8, &result);
					return result;
				}
			inline	wstring	UTF8StringToWide (const string& s)
				{
					return NarrowStringToWide (s, CP_UTF8);
				}
			inline	wstring	BSTR2wstring (BSTR b)
				{
					if (b == NULL) {
						return wstring ();
					}
					else {
						return wstring (b);
					}
				}
			inline	wstring	BSTR2wstring (VARIANT b)
				{
					if (b.vt == VT_BSTR) {
						return BSTR2wstring (b.bstrVal);
					}
					else {
						return wstring ();
					}
				}





			inline	bool	StartsWith (const string& l, const string& prefix)
				{
					if (l.length () < prefix.length ()) {
						return false;
					}
					return l.substr (0, prefix.length ()) == prefix;
				}
			inline	bool	StartsWith (const wchar_t* l, const wchar_t* prefix, StringCompareOptions co)
				{
					// This is a good candiate for inlining because the StringCompareOptions 'co' is nearly ALWAYS a constant
					RequireNotNil (l);
					RequireNotNil (prefix);
					size_t	prefixLen	=	::wcslen (prefix);
					switch (co) {
						case	eWithCase_CO:			return  wcsncmp (l, prefix, prefixLen) == 0;
						case	eCaseInsensitive_CO:	return _wcsnicmp (l, prefix, prefixLen) == 0;
					}
					Assert (false);	return false; // not reached
				}
			inline	bool	StartsWith (const wstring& l, const wstring& prefix, StringCompareOptions co)
				{
					// This is a good candiate for inlining because the StringCompareOptions 'co' is nearly ALWAYS a constant
					size_t	prefixLen	=	prefix.length ();
					if (l.length () < prefixLen) {
						return false;
					}
					switch (co) {
						case	eWithCase_CO:			return wcsncmp (l.c_str (), prefix.c_str (), prefixLen) == 0;
						case	eCaseInsensitive_CO:	return _wcsnicmp (l.c_str (), prefix.c_str (), prefixLen) == 0;
					}
					Assert (false);	return false; // not reached
				}



			inline	bool	EndsWith (const wstring& l, const wstring& suffix, StringCompareOptions co)
				{
					// This is a good candiate for inlining because the StringCompareOptions 'co' is nearly ALWAYS a constant
					size_t	lLen		=	l.length ();
					size_t	suffixLen	=	suffix.length ();
					if (lLen < suffixLen) {
						return false;
					}
					size_t	i	=	lLen - suffixLen;
					switch (co) {
						case	eWithCase_CO:			return wcsncmp (l.c_str () + i, suffix.c_str (), suffixLen) == 0;
						case	eCaseInsensitive_CO:	return _wcsnicmp (l.c_str () + i, suffix.c_str (), suffixLen) == 0;
					}
					Assert (false);	return false; // not reached
				}




			// class	CaseInsensativeLess
			inline	bool CaseInsensativeLess::operator()(const wstring& _Left, const wstring& _Right) const
				{
					return ::_wcsicmp (_Left.c_str (), _Right.c_str ()) < 0;
				}



			template	<typename TCHAR>
				size_t	CRLFToNL (const typename TCHAR* srcText, size_t srcTextBytes, typename TCHAR* outBuf, size_t outBufSize)
					{
						RequireNotNil (srcText);
						RequireNotNil (outBuf);
						typename TCHAR*	outPtr	=	outBuf;
						for (size_t i = 1; i <= srcTextBytes; i++) {
							typename TCHAR	c	 = srcText[i-1];
							if (c == '\r') {
								// peek at next character - and if we have a CRLF sequence - then advance pointer
								// (so we skip next NL) and pretend this was an NL..
								// NB: we DONT map plain CR to NL - to get that to happen - use Led_NormalizeTextToNL()
								if (i < srcTextBytes and srcText[i] == '\n') {
									c = '\n';
									i++;
								}
							}
							*outPtr++ = c;
						}
						size_t	nBytes	=	outPtr - outBuf;
						Assert (nBytes <= outBufSize);
						Arg_Unused (outBufSize);
						return (nBytes);
					}
			template	<typename TCHAR>
				inline	void	CRLFToNL (basic_string<typename TCHAR>* text)
					{
						size_t	origLen	=	text->length ();
						size_t	newLen	=	CRLFToNL (Containers::Start (*text), origLen, Containers::Start (*text), origLen);
						Assert (newLen <= origLen);
						text->resize (newLen);
					}
			template	<typename TCHAR>
				inline	basic_string<typename TCHAR>	CRLFToNL (const basic_string<typename TCHAR>& text)
					{
						basic_string<typename TCHAR>	r	=	text;
						CRLFToNL (&r);
						return r;
					}


			template	<typename TCHAR>
				size_t	NLToCRLF (const TCHAR* srcText, size_t srcTextBytes, TCHAR* outBuf, size_t outBufSize)
					{
						Require (srcText != outBuf);	// though we support this for the others - its too hard
														// in this case for the PC...
						TCHAR*	outPtr	=	outBuf;
						for (size_t i = 1; i <= srcTextBytes; i++) {
							Assert (outPtr < outBuf+outBufSize);
							TCHAR	c	 = srcText[i-1];
							if (c == '\n') {
								*outPtr++ = '\r';
							}
							*outPtr++ = c;
						}
						size_t	nBytes	=	outPtr - outBuf;
						Assert (nBytes <= outBufSize);
						Arg_Unused (outBufSize);
						return (nBytes);
					}
			template	<typename TCHAR>
				inline	basic_string<typename TCHAR>	NLToCRLF (const basic_string<typename TCHAR>& text)
					{
						size_t	outBufSize	=	(text.length () + 1) * 2;
						Memory::SmallStackBuffer<TCHAR>	outBuf (outBufSize);
						size_t newSize = NLToCRLF<TCHAR> (Containers::Start (text), text.length (), outBuf.begin (), outBufSize);
						Assert (newSize < outBufSize);
						outBuf[newSize] = '\0';
						return basic_string<TCHAR> (outBuf);
					}

		
			template	<typename TCHAR>
				size_t	NLToNative (const TCHAR* srcText, size_t srcTextBytes, TCHAR* outBuf, size_t outBufSize)
					{
						Require (srcText != outBuf);	// though we support this for the others - its too hard
														// in this case for the PC...
						TCHAR*	outPtr	=	outBuf;
						for (size_t i = 1; i <= srcTextBytes; i++) {
							Assert (outPtr < outBuf+outBufSize);
							#if		defined (macintosh)
								TCHAR	c	 = (srcText[i-1] == '\n')? '\r': srcText[i-1];
							#elif	(defined (_WIN32) || defined (WIN32))
								TCHAR	c	 = srcText[i-1];
								if (c == '\n') {
									*outPtr++ = '\r';
								}
							#else
								// Unix
								TCHAR	c	 = srcText[i-1];
							#endif
							*outPtr++ = c;
						}
						size_t	nBytes	=	outPtr - outBuf;
						Assert (nBytes <= outBufSize);
						Arg_Unused (outBufSize);
						return (nBytes);
					}
			template	<typename TCHAR>
				inline	basic_string<typename TCHAR>	NLToNative (const basic_string<typename TCHAR>& text)
					{
						size_t	outBufSize	=	(text.length () + 1) * 2;
						Memory::SmallStackBuffer<TCHAR>	outBuf (outBufSize);
						size_t newSize = NLToNative<TCHAR> (Containers::Start (text), text.length (), outBuf.begin (), outBufSize);
						Assert (newSize < outBufSize);
						outBuf[newSize] = '\0';
						return basic_string<TCHAR> (outBuf);
					}


			template	<typename TCHAR>
				size_t	NormalizeTextToNL (const typename TCHAR* srcText, size_t srcTextBytes, typename TCHAR* outBuf, size_t outBufSize)
					{
						Require (srcTextBytes == 0 or srcText != NULL);
						Require (outBufSize == 0 or outBuf != NULL);
						// Require outBufSize big enough to hold the converted srcTextBytes (best to just make sizes the same)

						// NB: We DO Support the case where srcText == outBuf!!!!
						typename TCHAR*	outPtr	=	outBuf;
						for (size_t i = 0; i < srcTextBytes; i++) {
							typename TCHAR	c	=	srcText[i];
							if (c == '\r') {
								// peek at next character - and if we have a CRLF sequence - then advance pointer
								// (so we skip next NL) and pretend this was an NL..
								if (i+1 < srcTextBytes and srcText[i+1] == '\n') {
									i++;
								}
								c = '\n';
							}
							Assert (outPtr < outBuf+outBufSize);
							*outPtr++ = c;
						}
						size_t	nBytes	=	outPtr - outBuf;
						Assert (nBytes <= outBufSize);
						Arg_Unused (outBufSize);
						return (nBytes);
					}
			template	<typename TCHAR>
				inline	void	NormalizeTextToNL (basic_string<typename TCHAR>* text)
					{
						RequireNotNil (text);
						size_t	origLen	=	text->length ();
						size_t	newLen	=	NormalizeTextToNL (static_cast<const typename TCHAR*> (Containers::Start (*text)), origLen, static_cast<typename TCHAR*> (Containers::Start (*text)), origLen);
						Assert (newLen <= origLen);
						text->resize (newLen);
					}
			template	<typename TCHAR>
				inline	basic_string<typename TCHAR>	NormalizeTextToNL (const basic_string<typename TCHAR>& text)
					{
						basic_string<typename TCHAR>	r	=	text;
						NormalizeTextToNL (&r);
						return r;
					}



			inline	bool	StringsCILess (const wstring& l, const wstring& r)
				{
					return ::_wcsicmp (l.c_str (), r.c_str ()) < 0;
				}
			inline	bool	StringsCIEqual (const wstring& l, const wstring& r)
				{
					return ::_wcsicmp (l.c_str (), r.c_str ()) == 0;
				}
			inline	int		StringsCICmp (const wstring& l, const wstring& r)
				{
					return ::_wcsicmp (l.c_str (), r.c_str ());
				}




			template	<typename TCHAR>
				basic_string<typename TCHAR>	LTrim (const basic_string<typename TCHAR>& text)
					{
						std::locale loc1;	// default locale
						const ctype<typename TCHAR>& ct = use_facet<ctype<typename TCHAR> >(loc1);
						basic_string<typename TCHAR>::const_iterator i = text.begin ();
						for (; i != text.end () and ct.is (ctype<TCHAR>::space, *i); ++i)
							;
						return basic_string<TCHAR> (i, text.end ());
					}
			template	<typename TCHAR>
				basic_string<typename TCHAR>	RTrim (const basic_string<typename TCHAR>& text)
					{
						std::locale loc1;	// default locale
						const ctype<typename TCHAR>& ct = use_facet<ctype<typename TCHAR> >(loc1);
						basic_string<typename TCHAR>::const_iterator i = text.end ();
						for (; i != text.begin () and ct.is (ctype<TCHAR>::space, *(i-1)); --i)
							;
						return basic_string<TCHAR> (text.begin (), i);
					}
			template	<typename TCHAR>
				inline	basic_string<typename TCHAR>	Trim (const basic_string<typename TCHAR>& text)
					{
						return LTrim (RTrim (text));
					}


			template	<typename STRING>
				vector<STRING> Tokenize (const STRING& str, const STRING& delimiters)
					{
						vector<STRING>	result;
						STRING::size_type	lastPos = str.find_first_not_of (delimiters, 0);		// Skip delimiters at beginning
						STRING::size_type	pos     = str.find_first_of (delimiters, lastPos);		// Find first "non-delimiter"
						while (STRING::npos != pos || STRING::npos != lastPos) {
							Containers::ReserveSpeedTweekAdd1 (result);
							// Found a token, add it to the vector.
							result.push_back(str.substr (lastPos, pos - lastPos));
							// Skip delimiters.  Note the "not_of"
							lastPos = str.find_first_not_of (delimiters, pos);
							// Find next "non-delimiter"
							pos = str.find_first_of (delimiters, lastPos);
						}
						return result;
					}



			template	<>
				inline	size_t	Length (const char* p)
					{
						RequireNotNil (p);
						return ::strlen (p);
					}
			template	<>
				inline	size_t	Length (const wchar_t* p)
					{
						RequireNotNil (p);
						return ::wcslen (p);
					}
		}
	}
}
#endif	/*_Stroika_Foundation_Characters_StringUtils_inl_*/
