/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Math_Basic_inl_
#define	_Stroika_Foundation_Math_Basic_inl_	1


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
#endif	/*_Stroika_Foundation_Math_Basic_inl_*/



