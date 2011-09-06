/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Characters_Platform_Windows_CodePage_h_
#define	_Stroika_Foundation_Characters_Platform_Windows_CodePage_h_	1

#include	"../../../StroikaPreComp.h"

#if		qPlatform_Windows
	#include	<Windows.h>
#else
	#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include	"../../../Configuration/Common.h"

#include	"../../CodePage.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Characters {
			namespace	Platform {
				namespace	Windows {

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

					string	BSTRStringToUTF8 (const BSTR bstr);
					BSTR	UTF8StringToBSTR (const char* ws);
					wstring	BSTR2wstring (BSTR b);
					wstring	BSTR2wstring (VARIANT b);

				}
			}
		}
	}
}
#endif	/*_Stroika_Foundation_Characters_Platform_Windows_CodePage_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"CodePage.inl"

