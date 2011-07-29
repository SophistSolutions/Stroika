/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Containers_Basics_h_
#define	_Stroika_Foundation_Containers_Basics_h_	1

#include	"../StroikaPreComp.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Containers {


			// For a contiguous container (such as a vector or basic_string) - find the pointers to the start/end of the container
			template	<typename CONTAINER>
				typename	CONTAINER::value_type*	Start (CONTAINER& c);
			template	<typename CONTAINER>
				typename	typename const CONTAINER::value_type*	Start (const CONTAINER& c);
			template	<typename CONTAINER>
				typename	CONTAINER::value_type*	End (CONTAINER& c);
			template	<typename CONTAINER>
				typename	typename const CONTAINER::value_type*	End (const CONTAINER& c);

			template	<typename	CONTAINER>
				void	ReserveSpeedTweekAdd1 (typename CONTAINER& c, size_t kMinChunk = 10);
			template	<typename	CONTAINER>
				void	ReserveSpeedTweekAddN (typename CONTAINER& c, size_t n, size_t kMinChunk = 10);

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


