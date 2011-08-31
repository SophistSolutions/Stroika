/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_ThreadAbortException_h_
#define	_Stroika_Foundation_Execution_ThreadAbortException_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"

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
#endif	/*_Stroika_Foundation_Execution_ThreadAbortException_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"ThreadAbortException.inl"
