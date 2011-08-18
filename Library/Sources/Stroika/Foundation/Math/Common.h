/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Math_Basic_h_
#define	_Stroika_Foundation_Math_Basic_h_	1

#include	"../StroikaPreComp.h"

#include	<cmath>			// though perhaps not strictly needed, probably wanted if including Stroika/Foundation/Math/Common.h


/*
@CONFIGVAR:		qPlatformSupports_isnan
@DESCRIPTION:	<p>Defines if the compiler stdC++/c99 library supports the std::isnan() function</p>
	*/
#ifndef	qPlatformSupports_isnan
	#error "qPlatformSupports_isnan should normally be defined indirectly by StroikaConfig.h"
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

#if		!qPlatformSupports_isnan
// SB in std namespace
namespace	std {
	bool	isnan (float f);
	bool	isnan (double d);
}
#endif


#endif	/*_Stroika_Foundation_Math_Basic_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Common.inl"


