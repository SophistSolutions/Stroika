/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Configuration_Private_Defaults_Math_Common_h_
#define	_Stroika_Foundation_Configuration_Private_Defaults_Math_Common_h_	1

	/*
	 * This file is a private Stroika implementation detail, and shouldn't be included directly. 
	 * Its used by StroikaConfig.h - optionally included there -
	 * to help implement the set of Stroika public configuration defines.
	 */

	/*
	 * See DOCS in Stroika/Foundation/Math/Common.h
	 */
	#ifndef	qPlatformSupports_isnan
		#if		defined (__GNUC__)
			#define	qPlatformSupports_isnan	1
		#else
			// default off cuz we have a good safe impl
			#define	qPlatformSupports_isnan	0
		#endif
	#endif
	#ifndef	qPlatformSupports__isnan
		#if		defined (_MSC_VER)
			#define	qPlatformSupports__isnan	1
		#else
			// default off cuz we have a good safe impl
			#define	qPlatformSupports__isnan	0
		#endif
	#endif


#endif	/*_Stroika_Foundation_Configuration_Private_Defaults_Math_Common_h_*/
