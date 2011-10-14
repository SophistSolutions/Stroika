/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_Lockable_h_
#define	_Stroika_Foundation_Execution_Lockable_h_	1

#include	"../StroikaPreComp.h"

#include	"CriticalSection.h"


/*
 * TODO:
 *			(o)	We need todo something to support POD types.
 *				This COULD be done using a PRIVATE Lockable_POD, which does what we want and then
 *				template specialization to indirect to the Lockable_POD type.
 */



namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {


			//
			// This template provides essentially no functionality - except that its a handy way to associate a CriticalSection with a 
			// peice of data it locks (making it easier and more practical to have a single critical section for each peice of data)
			//
			// You can use Lockable<T> as an argument to 
			//
			template	<typename BASE, typename LOCKTYPE = CriticalSection>
				class	Lockable : public BASE, public LOCKTYPE {
					private:
						NO_COPY_CONSTRUCTOR(Lockable);
						NO_ASSIGNMENT_OPERATOR(Lockable);
					public:
						Lockable ();
						Lockable (const BASE& from);

						const Lockable& operator= (const BASE& rhs);
				};


		}
	}
}
#endif	/*_Stroika_Foundation_Execution_Lockable_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Lockable.inl"
