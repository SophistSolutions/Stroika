/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	__Stroika_Foundation_Execution_WaitAbandonedException_h__
#define	__Stroika_Foundation_Execution_WaitAbandonedException_h__	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Basics.h"

#include	"Exceptions.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			class	WaitAbandonedException : public StringException {
				public:
					WaitAbandonedException ();
			};

		}
	}
}
#endif	/*__Stroika_Foundation_Execution_WaitAbandonedException_h__*/
