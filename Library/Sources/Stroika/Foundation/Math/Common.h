/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Math_Basic_h_
#define	_Stroika_Foundation_Math_Basic_h_	1

#include	"../StroikaPreComp.h"

#include	<cmath>			// though perhaps not strictly needed, probably wanted if including Stroika/Foundation/Math/Common.h





namespace	Stroika {
	namespace	Foundation {
		namespace	Math {

			double	nan ();

			const	double	kE	=	2.71828182845904523536;
			const	double	kPi	=	3.14159265358979323846;

             /*
             * RoundUpTo() - round towards posative infinity.
             * RoundDownTo() - round towards negative infinity.
             */
            int			RoundUpTo (unsigned x, unsigned toNearest);
            int			RoundDownTo (unsigned x, unsigned toNearest);
            int			RoundUpTo (int x, unsigned toNearest);
            int			RoundDownTo (int x, unsigned toNearest);

        }
	}
}

#if		!qCompilerAndStdLib_isnan
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


