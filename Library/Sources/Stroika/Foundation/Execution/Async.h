/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Async_h_
#define _Stroika_Foundation_Execution_Async_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Common.h"

/*
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 *
 */

namespace Stroika::Foundation::Execution {

	/**
	 *	\brief run all the argument functions asynchronously, and wait until they all complete.
	 *
	 *	Could be implemented with std::async, or ThreadPool.
	 * 
	 *	@todo describe if any of the functions throw...
	 *	@todo could enhance this to also return tuple of return results of each function;
	 */
	template <invocable<>... I>
   void InvokeAsync (I ... f);
   
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Async.inl"

#endif /*_Stroika_Foundation_Execution_Async_h_*/
