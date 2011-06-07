/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	__Stroika_Foundation_Execution_ThreadAbortException_h__
#define	__Stroika_Foundation_Execution_ThreadAbortException_h__	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Basics.h"

#include	"Exceptions.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			// Generally should not be reported. It's just to help force a thread to shut itself down
			class	ThreadAbortException {
				public:
					ThreadAbortException ();
			};

		}
	}
}
#endif	/*__Stroika_Foundation_Execution_ThreadAbortException_h__*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"ThreadAbortException.inl"
