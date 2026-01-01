/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Async_h_
#define _Stroika_Foundation_Execution_Async_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <concepts>

#include "Stroika/Foundation/Common/Common.h"

/*
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 *
 */

namespace Stroika::Foundation::Execution {

    /**
     *  \brief Run all the argument functions (logically/potentially) in parallel, and wait until they all complete.
     *
     *  Could be implemented with std::async, or ThreadPool.
     * 
     *  @todo describe if any of the functions throw... (probably rethrow the first, but NYI) - and then does this interrupt the rest?
     *  Probably SHOULD but not clear always can.
     * 
     *  @todo could enhance this to also return tuple of return results of each function;
     *  no guarantee all run in parallel, but suggestion they are. Typically will auto-allocate threadpool of size
     *  #virtual CPUs (hardware_parallelism). 
     */
    template <invocable<>... I>
    void RunAll (I... functions);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Async.inl"

#endif /*_Stroika_Foundation_Execution_Async_h_*/
