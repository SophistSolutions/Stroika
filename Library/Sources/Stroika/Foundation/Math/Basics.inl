/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Contmathsgbasics_inl
#define	_Contmathsgbasics_inl	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	<limits>

namespace	Stroika {	
	namespace	Foundation {
		namespace	Math {

			inline	double	nan ()
				{
					return numeric_limits<double>::quiet_NaN ();
				}

		}
	}
}

#endif	/*_Contmathsgbasics_inl*/



