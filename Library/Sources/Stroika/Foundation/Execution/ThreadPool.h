/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_ThreadPool_h_
#define	_Stroika_Foundation_Execution_ThreadPool_h_	1

#include	"../StroikaPreComp.h"


#include	"Thread.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {


			using	Memory::SharedPtr;


			// ADD THREADPOOL (Thread Pool) SUPPORT:
			//
			//		Basic idea is to separate out this existing class into two parts - the part that has
			//		NotifyOfAbort/MainLoop () - and the surrounding code that does the ThreadMain etc stuff.
			//
			//		Then do one backend impl that does just what we have here, and one that pools the thread objects
			//		(handles) - and keeps them sleeping when not used.
			//
			//		LOW PRIORITY - but a modest change. Its mainly useful for servicing lost-cost calls/threads, where
			//		the overhead of constructing the thread is significant compared to the cost of performing the
			//		action, and where the priority & stacksize can all be predeterimed and 'shared'.
			//



		}
	}
}
#endif	/*_Stroika_Foundation_Execution_ThreadPool_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"ThreadPool.inl"
