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


			/*
			@CONFIGVAR:		qTargetPlatformSDKUseswchar_t
			@DESCRIPTION:	<p>defines if we use wchar_t or char for most platform interfaces (mostly applicable/useful for windows)</p>
			 */
			#if		!defined (qTargetPlatformSDKUseswchar_t)
				#error "qTargetPlatformSDKUseswchar_t should normally be defined indirectly by StroikaConfig.h"
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

			#ifndef _T
				#define _T(x)    x
			#endif

			#if !qPlatform_Windows
				/*
					This will need changing, but needs discussion. We could declare our own routines, or map all the MS
					_t versions, which just trivially choose between char/wchar_t versions. Complicating things is the MS 
					range checking versions
                */
				#if qTargetPlatformSDKUseswchar_t
					#define	_tcsncpy_s	wcsncpy
				#else
					#define	_tcsncpy_s	strncpy
				#endif
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
