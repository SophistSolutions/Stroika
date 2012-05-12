/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Containers_Basics_h_
#define	_Stroika_Foundation_Containers_Basics_h_	1

#include	"../StroikaPreComp.h"

#include	<cstddef>


namespace	Stroika {	
	namespace	Foundation {
		namespace	Containers {


			// For a contiguous container (such as a vector or basic_string) - find the pointers to the start/end of the container
			template	<typename CONTAINER>
				typename CONTAINER::value_type*	Start (CONTAINER& c);
			template	<typename CONTAINER>
				 const typename CONTAINER::value_type*	Start (const CONTAINER& c);
			template	<typename CONTAINER>
				typename CONTAINER::value_type*	End (CONTAINER& c);
			template	<typename CONTAINER>
				const typename CONTAINER::value_type*	End (const CONTAINER& c);

			/*
			 * Take the given value and map it to -1, 0, 1 - without any compiler warnings. Handy for 32/64 bit etc codiing when you maybe comparing
			 * different sized values and just returning an int, but don't want the warnings about overflow etc.
			 */
			template	<typename FROM_INT_TYPE, typename TO_INT_TYPE>
				TO_INT_TYPE	CompareResultNormalizeHelper (FROM_INT_TYPE f);

			template	<typename	CONTAINER>
				void	ReserveSpeedTweekAdd1 (CONTAINER& c, size_t kMinChunk = 10);
			template	<typename	CONTAINER>
				void	ReserveSpeedTweekAddN (CONTAINER& c, size_t n, size_t kMinChunk = 10);

		}

	}
}
#endif	/*_Stroika_Foundation_Containers_Basics_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Common.inl"


