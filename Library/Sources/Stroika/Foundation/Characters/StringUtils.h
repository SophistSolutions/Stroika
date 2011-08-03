/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Characters_StringUtils_h_
#define	_Stroika_Foundation_Characters_StringUtils_h_	1

#include	"../StroikaPreComp.h"

#include	<cctype>
#include	<locale>
#include	<string>
#include	<vector>


#include	"../Configuration/Common.h"
#include	"CodePage.h"
#include	"TChar.h"



/// THIS MODULE SB OBSOLETE ONCE WE GET STROIKA CONTAINERS WORKING



/*
This should be moved after discussion
Case insensitve string comparisons are poorly supported. For single character there are a least 4 possible names: strcasecmp, strcmpi, stricmp and _stricmp. strcasecmp is POSIX, others are non-standard but MSVS only supports _stricmp (plus stricmp but deprecated). _stricmp adds mandatory memory verification stuff, to avoid various hacker exploits.
Situation even worse for wide characters, as apparently NOTHING is POSIX currentlty. GCC supports wcscasecmp and MS supports _wcsnicmp.

For now: stick with GCC names, as at least one of them is POSIX.

*/

#if		qPlatform_Windows
	#define	strcasecmp	_stricmp
	#define	strncasecmp	_strnicmp
	#define	wcscasecmp	_wcsicmp
	#define	wcsncasecmp	_wcsnicmp
#else
	#if qTargetPlatformSDKUseswchar_t
		#define	_tcsncpy_s	wcsncpy
	#else
		#define	_tcsncpy_s	strncpy
	#endif
#endif

#if		qPlatform_Windows
	// not sure what to do about "safe" versions yet
	#define		sscanf	sscanf_s
	#define		swscanf	swscanf_s
	#define		sprintf	sprintf_s
#endif


namespace	Stroika {	
	namespace	Foundation {
		namespace	Characters {

			using	std::string;
			using	std::wstring;
			using	std::basic_string;

			using	Characters::CodePage;


			void	WideStringToNarrow (const wchar_t* wsStart, const wchar_t* wsEnd, CodePage codePage, string* intoResult);
			void	WideStringToNarrow (const wstring& ws, CodePage codePage, string* intoResult);
			string	WideStringToNarrow (const wstring& ws, CodePage codePage);
			void	NarrowStringToWide (const char* sStart, const char* sEnd, CodePage codePage, wstring* intoResult);
			void	NarrowStringToWide (const string& s, CodePage codePage, wstring* intoResult);
			wstring	NarrowStringToWide (const string& s, CodePage codePage);


			string	WideStringToACP (const wstring& ws);
			wstring	ACPStringToWide (const string& ws);


		#if		qPlatform_Windows
			string	BSTRStringToUTF8 (const BSTR bstr);
		#endif
			string	WideStringToUTF8 (const wstring& ws);
			void	UTF8StringToWide (const char* s, wstring* intoStr);
			void	UTF8StringToWide (const string& s, wstring* intoStr);
			wstring	UTF8StringToWide (const char* ws);
			wstring	UTF8StringToWide (const string& ws);

		#if		qPlatform_Windows
			BSTR	UTF8StringToBSTR (const char* ws);
		#endif
			wstring	ASCIIStringToWide (const string& s);
			string	WideStringToASCII (const wstring& s);

		#if		qPlatform_Windows
			wstring	BSTR2wstring (BSTR b);
			wstring	BSTR2wstring (VARIANT b);
		#endif
			string	Format (const char* format, ...);
			wstring	Format (const wchar_t* format, ...);


			/*
			 *	Implement current-ui-language-specific noun-pluralization logic for the given noun string (assuming the count of that noun
			 *	is given (english rules - if count != 1 - append s, but we dont wnat that logic to proliferate through the app, so
			 *	its easier to localize.
			 */
			string	PluralizeNoun (const string& s, int count = 1000);
			wstring	PluralizeNoun (const wstring& s, int count = 1000);

			string	StripTrailingCharIfAny (const string& s, char c);
			wstring	StripTrailingCharIfAny (const wstring& s, wchar_t c);




			wstring			MapUNICODETextWithMaybeBOMTowstring (const char* start, const char* end);
			vector<Byte>	MapUNICODETextToSerializedFormat (const wchar_t* start, const wchar_t* end, CodePage useCP = kCodePage_UTF8);	// suitable for files



			int		HexString2Int (const string& s);
			int		HexString2Int (const wstring& s);
			int		String2Int (const string& s);
			int		String2Int (const wstring& s);
			float	String2Float (const wstring& s);	// returns R4LLib::nan () if invalid string
			float	String2Float (const wstring& s, float returnValIfInvalidString);
			wstring	Float2String (float f, unsigned int precision = 6);		// map nan to empty string, and use limited precision, and strip trialing .0...

			// Speed-tweek versions of these ANSI-standard routines. NB: These assume a congtiguous range encoding of upper/lower characters,
			// as with ASCII, and UNICODE character encodings
			//
			// NB: we should revisit if this speedtweek is a good idea with each new compiler release, since in principal, a good compiler
			// and runtime lib would do better than what we've coded.
			//
			//			-- LGP 2006-01-17
			//
			#ifndef	qUseASCIIRangeSpeedHack
			#define	qUseASCIIRangeSpeedHack		1
			#endif

			// use qUseASCIIRangeSpeedHack - if turned on...
			char	TOLOWER (char c);
			wchar_t	TOLOWER (wchar_t c);
			char	TOUPPER (char c);
			wchar_t	TOUPPER (wchar_t c);
			bool	ISSPACE (char c);
			bool	ISSPACE (wchar_t c);


			string	tolower (const string& s);
			void	tolower (string* s);
			wstring	tolower (const wstring& s);
			void	tolower (wstring* s);

			string	toupper (const string& s);
			void	toupper (string* s);
			wstring	toupper (const wstring& s);
			void	toupper (wstring* s);

			// for now, only do wstring version since for narrow-strings, case compare is ambiguous (need to specify codeset)
			bool	StringsCIEqual (const wstring& l, const wstring& r);
			bool	StringsCILess (const wstring& l, const wstring& r);
			// like strcmp() < 0 impless less, = 0 implies equal, and > 0 implies >
			int		StringsCICmp (const wstring& l, const wstring& r);

			enum	StringCompareOptions {
				eWithCase_CO,
				eCaseInsensitive_CO,
			};
			bool	StartsWith (const string& l, const string& prefix);
			bool	StartsWith (const wchar_t* l, const wchar_t* prefix, StringCompareOptions co = eWithCase_CO);
			bool	StartsWith (const wstring& l, const wstring& prefix, StringCompareOptions co = eWithCase_CO);
			bool	EndsWith (const wstring& l, const wstring& suffix, StringCompareOptions co = eWithCase_CO);
			bool	Contains (const wstring& string2Search, const wstring& substr, StringCompareOptions co = eWithCase_CO);

			struct	CaseInsensativeLess : public binary_function<wstring, wstring, bool> {
				bool operator()(const wstring& _Left, const wstring& _Right) const;
			};


			// Only implemented for char/wchar_t. Reason for this is so code using old-style C++ strings can leverage overloading!
			template	<typename T>
				size_t	Length (const T* p);


			wstring	MungeStringSoSingular (const wstring& s);
			string	MungeStringSoSingular (const string& s);

			string	CapitalizeEachWord (const string& s);	// e.g. 'joe smith' becomes 'Joe Smith'
			wstring	CapitalizeEachWord (const wstring& s);
			string	CapitalizeEachSentence (const string& s);	// e.g. 'joe smith' becomes 'Joe smith'
			wstring	CapitalizeEachSentence (const wstring& s);
			string	UnCapitalizeFirstWord (const string& s);	// e.g. 'Joe Smith' becomes 'joe Smith', but 'IBM eats' stays 'IBM eats'
			wstring	UnCapitalizeFirstWord (const wstring& s);
			bool	IsAllCaps (const string& s);
			bool	IsAllCaps (const wstring& s);


			string	QuoteForXMLAttribute (const string& s);
			string	QuoteForXMLAttribute (const wstring& s);		// encode non-ascii characters as entity-references
			wstring	QuoteForXMLAttributeW (const wstring& s);
			string	QuoteForXML (const string& s);
			string	QuoteForXML (const wstring& s);		// encode non-ascii characters as entity-references
			wstring	QuoteForXMLW (const wstring& s);



			// return #bytes in output buffer (NO NULL TERM) - assert buffer big enough - output buf as big is input buf
			// always big enough. OK for srcText and outBuf to be SAME PTR.
			template	<typename TCHAR>
				size_t	CRLFToNL (const TCHAR* srcText, size_t srcTextBytes, TCHAR* outBuf, size_t outBufSize);
			template	<typename TCHAR>
				void	CRLFToNL (basic_string<TCHAR>* text);	// modified in-place
			template	<typename TCHAR>
				basic_string<TCHAR>	CRLFToNL (const basic_string<TCHAR>& text);

			template	<typename TCHAR>
				size_t	NLToCRLF (const TCHAR* srcText, size_t srcTextBytes, TCHAR* outBuf, size_t outBufSize);
			template	<typename TCHAR>
				basic_string<TCHAR>	NLToCRLF (const basic_string<TCHAR>& text);


			template	<typename TCHAR>
				size_t	NLToNative (const TCHAR* srcText, size_t srcTextBytes, TCHAR* outBuf, size_t outBufSize);
			template	<typename TCHAR>
				basic_string<TCHAR>	NLToNative (const basic_string<TCHAR>& text);

			// return #bytes in output buffer (NO NULL TERM) - assert buffer big enough - output buf as big is input buf
			// always big enough. OK for srcText and outBuf to be SAME PTR.
			template	<typename TCHAR>
				size_t	NormalizeTextToNL (const TCHAR* srcText, size_t srcTextBytes, TCHAR* outBuf, size_t outBufSize);
			template	<typename TCHAR>
				void	NormalizeTextToNL (basic_string<TCHAR>* text);	// modified in-place
			template	<typename TCHAR>
				basic_string<TCHAR>	NormalizeTextToNL (const basic_string<TCHAR>& text);



			string	LimitLength (const string& str, size_t maxLen, bool keepLeft = true);
			wstring	LimitLength (const wstring& str, size_t maxLen, bool keepLeft = true);


			template	<typename TCHAR>
				basic_string<TCHAR>	LTrim (const basic_string<TCHAR>& text);
			template	<typename TCHAR>
				basic_string<TCHAR>	RTrim (const basic_string<TCHAR>& text);
			template	<typename TCHAR>
				basic_string<TCHAR>	Trim (const basic_string<TCHAR>& text);




			namespace	StringVectorEncoding {
				/*
				 *	Don't write this to a file, as the format of the encoding is subject to change. This is a 
				 *	work-in-progress, and just intended to solve - for now - an immediate need (list of ids passed through javascript)
				 *
				 *		-- LGP 2006-04-15
				 */
				wstring			Encode (const vector<wstring>& v);
				vector<wstring>	Decode (const wstring& encodedString);
			}


			template	<typename STRING>
				vector<STRING> Tokenize (const STRING& str, const STRING& delimiters);

		}
	}
}
#endif	/*_Stroika_Foundation_Characters_StringUtils_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"StringUtils.inl"

