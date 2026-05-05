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
     *  If any function throws, an arbitrary one of those exceptions will be rethrown by RunAll.
     * 
     *  All functions will complete before RunAll returns (regardless of whether any throw).
     * 
     *  \note no guarantee all run in parallel, but suggestion they are.
     * 
     *  This function returns the value of all completed functions as a tuple, unless they return void, in which case they are
     *  skipped, and for the special case of all returning void, the RunAll return type is void.
     * 
     *  \par Example Usage
     *      \code
     *          auto results = RunAll ([] () { return 1; }, [] () { return 2; }, [] () { return 3; });
     *          EXPECT_EQ (results, make_tuple (1, 2, 3));
     *      \endcode
     * 
     *  \par Example Usage
     *      \code
     *          tuple<int> results = RunAll ([] () -> void {}, [] () { return 3; });
     *          EXPECT_EQ (results, make_tuple (3));
     *      \endcode
     * 
     *  \par Example Usage
     *      \code
     *          int a = 0;
     *          int b = 1;
     *          int c = 2;
     *          RunAll ([&] () { a = 3; }, [&] () { b = 4; }, [&] () { c = 5; });
     *          EXPECT_EQ (a, 3);
     *          EXPECT_EQ (b, 4);
     *          EXPECT_EQ (c, 5);
     *      \endcode
     * 
     *  \par Example Usage
     *      \code
     *          static const auto kExcept_ = Execution::Exception {"Test exception"sv};
     *          auto thrower = [] () { Execution::Throw (kExcept_); };
     *          EXPECT_THROW (RunAll (thrower, [] () { return 3; }), Execution::Exception<>);
     *      \endcode
     * 
     *  \todo future versions of this function MAY cancel running functions if one throws.
     * 
     *  @todo Typically will auto-allocate threadpool of size  #virtual CPUs (hardware_parallelism). 
     */
    template <invocable<>... I>
    auto RunAll (I... functions);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Async.inl"

#endif /*_Stroika_Foundation_Execution_Async_h_*/
