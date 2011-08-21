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


			const	DurationSecondsType	kInfinite	=	numeric_limits<DurationSecondsType>::max ();

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
