/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
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

			inline	string	tstring2ANSI (const TString& s)
				{
					#if		qTargetPlatformSDKUseswchar_t
						return WideStringToACP (s);
					#else
						return s;
					#endif
				}
			inline	wstring	tstring2Wide (const TString& s)
				{
					#if		qTargetPlatformSDKUseswchar_t
						return s;
					#else
						return ACPStringToWide (s);
					#endif
				}
			inline	TString	ANSI2tstring (const string& s)
				{
					#if		qTargetPlatformSDKUseswchar_t
						return ACPStringToWide (s);
					#else
						return s;
					#endif
				}
			inline	TString	Wide2tstring (const wstring& s)
				{
					#if		qTargetPlatformSDKUseswchar_t
						return s;
					#else
						return WideStringToACP (s);
					#endif
				}
			inline	TString	totstring (const string& s)
				{
					return ANSI2tstring (s);
				}
			inline	TString	totstring (const wstring& s)
				{
					return Wide2tstring (s);
				}
		}
	}
}

#endif	/*_Stroika_Foundation_Characters_TString_inl_*/
