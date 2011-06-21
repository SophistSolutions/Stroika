/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_IRunnable_h_
#define	_Stroika_Foundation_Execution_IRunnable_h_	1

#include	"../StroikaPreComp.h"

#include	<windows.h>

#include	"../Configuration/Common.h"
#include	"../Memory/RefCntPtr.h"

#include	"CriticalSection.h"
#include	"Exceptions.h"
#include	"Event.h"




namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {



			/// More TODO - Improve use of RUNNABLE CLASS - SEE http://bugzilla/show_bug.cgi?id=746
			class	IRunnable {
				public:
					virtual ~IRunnable ();
				public:
					virtual	void	Run () = 0;
			};


		}
	}
}
#endif	/*_Stroika_Foundation_Execution_IRunnable_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"IRunnable.inl"
