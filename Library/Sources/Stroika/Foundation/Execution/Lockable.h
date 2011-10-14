/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_Lockable_h_
#define	_Stroika_Foundation_Execution_Lockable_h_	1

#include	"../StroikaPreComp.h"

#include	"CriticalSection.h"


/*
 * TODO:
 *			(o)	Support for POD-types is incomplete, and requires lots of typing. It would be nice to find a better way.
 *				Baring that, implement the remaining cases...
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
						NO_COPY_CONSTRUCTOR2(Lockable, Lockable<BASE,LOCKTYPE>);
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
