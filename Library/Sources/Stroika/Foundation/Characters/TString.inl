/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Characters_TString_inl_
#define	_Stroika_Foundation_Characters_TString_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"StringUtils.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Characters {

			inline	string	TString2ANSI (const TString& s)
				{
					#if		qTargetPlatformSDKUseswchar_t
						return WideStringToACP (s);
					#else
						return s;
					#endif
				}
			inline	wstring	TString2Wide (const TString& s)
				{
					#if		qTargetPlatformSDKUseswchar_t
						return s;
					#else
						return ACPStringToWide (s);
					#endif
				}
			inline	TString	ANSI2TString (const string& s)
				{
					#if		qTargetPlatformSDKUseswchar_t
						return ACPStringToWide (s);
					#else
						return s;
					#endif
				}
			inline	TString	Wide2TString (const wstring& s)
				{
					#if		qTargetPlatformSDKUseswchar_t
						return s;
					#else
						return WideStringToACP (s);
					#endif
				}
			inline	TString	ToTString (const string& s)
				{
					return ANSI2TString (s);
				}
			inline	TString	ToTString (const wstring& s)
				{
					return Wide2TString (s);
				}
		}
	}
}

#endif	/*_Stroika_Foundation_Characters_TString_inl_*/
