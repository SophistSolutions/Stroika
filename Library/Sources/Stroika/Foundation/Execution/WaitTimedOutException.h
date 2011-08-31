/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_WaitTimedOutException_h_
#define	_Stroika_Foundation_Execution_WaitTimedOutException_h_	1

#include	"../StroikaPreComp.h"

#include	"Exceptions.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			class	WaitTimedOutException : public StringException {
				public:
					WaitTimedOutException ();
			};

		}
	}
}
#endif	/*_Stroika_Foundation_Execution_WaitTimedOutException_h_*/
