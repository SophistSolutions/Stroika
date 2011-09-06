/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Characters_Platform_Windows_CodePage_inl_
#define	_Stroika_Foundation_Characters_Platform_Windows_CodePage_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../../../Containers/Common.h"
#include	"../../../Debug/Assertions.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Characters {
			namespace	Platform {
				namespace	Windows {

					inline	void	WideStringToNarrow (const wchar_t* wsStart, const wchar_t* wsEnd, CodePage codePage, string* intoResult)
						{
							RequireNotNull (intoResult);
							Require (wsStart <= wsEnd);
							size_t	wsLen	=	(wsEnd - wsStart);
							int stringLength = ::WideCharToMultiByte (codePage, 0, wsStart, static_cast<int> (wsLen), nullptr, 0, nullptr, nullptr);
							intoResult->resize (stringLength);
							if (stringLength != 0) {
								Verify (::WideCharToMultiByte (codePage, 0, wsStart, static_cast<int> (wsLen), Containers::Start (*intoResult), stringLength, nullptr, nullptr) == stringLength);
							}
						}
					inline	void	WideStringToNarrow (const wstring& ws, CodePage codePage, string* intoResult)
						{
							RequireNotNull (intoResult);
							int stringLength = ::WideCharToMultiByte (codePage, 0, ws.c_str (), static_cast<int> (ws.size ()), nullptr, 0, nullptr, nullptr);
							intoResult->resize (stringLength);
							if (stringLength != 0) {
								Verify (::WideCharToMultiByte (codePage, 0, ws.c_str (), static_cast<int> (ws.size ()), Containers::Start (*intoResult), stringLength, nullptr, nullptr) == stringLength);
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
							RequireNotNull (intoResult);
							Require (sStart <= sEnd);
							size_t	sLen	=	(sEnd - sStart);
							int newStrLen = ::MultiByteToWideChar (codePage, 0, sStart, static_cast<int> (sLen), nullptr, 0);
							intoResult->resize (newStrLen);
							if (newStrLen != 0) {
								Verify (::MultiByteToWideChar (codePage, 0, sStart, static_cast<int> (sLen), Containers::Start (*intoResult), newStrLen) == newStrLen);
							}
						}
					inline	void	NarrowStringToWide (const string& s, int codePage, wstring* intoResult)
						{
							RequireNotNull (intoResult);
							int newStrLen = ::MultiByteToWideChar (codePage, 0, s.c_str (), static_cast<int> (s.size ()), nullptr, 0);
							intoResult->resize (newStrLen);
							if (newStrLen != 0) {
								Verify (::MultiByteToWideChar (codePage, 0, s.c_str (), static_cast<int> (s.size ()), Containers::Start (*intoResult), newStrLen) == newStrLen);
							}
						}
					inline	wstring	NarrowStringToWide (const string& s, int codePage)
						{
							wstring	result;
							NarrowStringToWide (s, codePage, &result);
							return result;
						}


					inline	wstring	BSTR2wstring (BSTR b)
						{
							if (b == nullptr) {
								return wstring ();
							}
							else {
								return wstring (b);
							}
						}

				}
			}
		}
	}
}
#endif	/*_Stroika_Foundation_Characters_Platform_Windows_CodePage_inl_*/
