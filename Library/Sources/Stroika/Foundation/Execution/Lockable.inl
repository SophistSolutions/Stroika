/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_Lockable_inl_
#define	_Stroika_Foundation_Execution_Lockable_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Exceptions.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {


		//	class	Lockable
			template	<typename BASE, typename LOCKTYPE>
				inline	Lockable<BASE,LOCKTYPE>::Lockable ()
					: BASE ()
					, LOCKTYPE ()
					{
					}
			template	<typename BASE, typename LOCKTYPE>
				inline	Lockable<BASE,LOCKTYPE>::Lockable (const BASE& from)
					: BASE (from)
					, LOCKTYPE ()
					{
					}
			template	<typename BASE, typename LOCKTYPE>
				inline	const Lockable<BASE,LOCKTYPE>& Lockable<BASE,LOCKTYPE>::operator= (const BASE& rhs)
					{
						BASE&	THIS_BASE	=	*this;
						THIS_BASE = rhs;
						return *this;
					}



		}
	}
}
#endif	/*_Stroika_Foundation_Execution_Lockable_inl_*/
