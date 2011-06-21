/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_WaitAbandonedException_h_
#define	_Stroika_Foundation_Execution_WaitAbandonedException_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"

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
#endif	/*_Stroika_Foundation_Execution_WaitAbandonedException_h_*/
