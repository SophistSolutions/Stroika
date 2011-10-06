/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Time_Timezone_h_
#define	_Stroika_Foundation_Time_Timezone_h_	1

#include	"../StroikaPreComp.h"

#include	<ctime>



/*
 * TODO:
 *		o	
 */


namespace	Stroika {
	namespace	Foundation {
		namespace	Time {

			/*
			 * Return the number of seconds which must be added to a LocalTime value to get GMT.
			 */
			time_t	GetLocaltimeToGMTOffset ();

		}
	}
}
#endif	/*_Stroika_Foundation_Time_Timezone_h_*/
