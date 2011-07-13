/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Characters_TChar_h_
#define	_Stroika_Foundation_Characters_TChar_h_	1

/*
@MODULE:	CodePage
@DESCRIPTION:
		<p>This module is designed to provide mappings between wide UNICODE and various other code pages
	and UNICODE encodings.</p>
 */

#include	"../StroikaPreComp.h"

#include	<cstdlib>

#if		qPlatform_Windows
	#include	<tchar.h>
#endif

#include	"../Configuration/Common.h"



namespace	Stroika {	
	namespace	Foundation {
		namespace	Characters {


			#if		qTargetPlatformSDKUseswchar_t
				#if !defined(_UNICODE) || !defined(UNICODE)
#error "INCONSITENT VALS"
				#endif
			#else
				#if defined(_UNICODE) || defined(UNICODE)
#error "INCONSITENT VALS"
				#endif
			#endif


			#if		qTargetPlatformSDKUseswchar_t
				typedef	wchar_t	TChar;
			#else
				typedef	char	TChar;
			#endif


			// MACRO to wrap strings to get const TCHAR*
			#if		qTargetPlatformSDKUseswchar_t
				#define TSTR(x)    _TEXT(x)
			#else
				#define TSTR(x)    x
			#endif
		}
	}
}
#endif	/*_Stroika_Foundation_Characters_TChar_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"TChar.inl"
