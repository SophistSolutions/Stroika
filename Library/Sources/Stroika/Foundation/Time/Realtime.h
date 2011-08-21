/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Time_Realtime_h_
#define	_Stroika_Foundation_Time_Realtime_h_	1

#include	"../StroikaPreComp.h"

#include	<limits>


namespace	Stroika {	
	namespace	Foundation {
		namespace	Time {

			typedef	double	DurationSecondsType;

			DurationSecondsType	GetTickCount ();


#if 1
			// NEED NEW COMPILER DEFINE qCompiler_Supports_constexpr
			const	DurationSecondsType	kInfinite	=	DBL_MAX;
#else
			constexpr	DurationSecondsType	kInfinite	=	numeric_limits<DurationSecondsType>::max ();
#endif

		}
	}
}
#endif	/*_Stroika_Foundation_Time_Realtime_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Realtime.inl"
