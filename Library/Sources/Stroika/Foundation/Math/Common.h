/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Math_Basic_h_
#define	_Stroika_Foundation_Math_Basic_h_	1

#include	"../StroikaPreComp.h"

#include <math.h>

#ifndef	qUseVSNonStandardLibraryNames
	#if defined(_MSC_VER)
		#define	qUseVSNonStandardLibraryNames	1
	#else
		#define	qUseVSNonStandardLibraryNames	0
	#endif
#endif

#if		qUseVSNonStandardLibraryNames
	#include <float.h>
	#define isnan _isnan
	#define	wtol _wtol
#else
#endif


namespace	Stroika {	
	namespace	Foundation {
		namespace	Math {

			double	nan ();

			const	double	kE	=	2.71828182845904523536;
			const	double	kPi	=	3.14159265358979323846;

		}
	}
}
#endif	/*_Stroika_Foundation_Math_Basic_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Common.inl"


