/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	__StroiaFoundationContainersBasics_h__
#define	__StroiaFoundationContainersBasics_h__	1



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


#endif	/*__StroiaFoundationContainersBasics_h__*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Basics.inl"


