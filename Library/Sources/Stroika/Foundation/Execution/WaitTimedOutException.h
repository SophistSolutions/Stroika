/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	__Stroika_Foundation_Execution_WaitTimedOutException_h__
#define	__Stroika_Foundation_Execution_WaitTimedOutException_h__	1

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
#endif	/*__Stroika_Foundation_Execution_WaitTimedOutException_h__*/
