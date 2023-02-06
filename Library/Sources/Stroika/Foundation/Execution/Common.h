/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Common_h_
#define _Stroika_Foundation_Execution_Common_h_ 1

#include "../StroikaPreComp.h"

#include <execution>

#include "../Configuration/Enumeration.h"

namespace Stroika::Foundation::Execution {

    /**
     *  \brief equivilent which of 4 types being used std::execution::sequenced_policy, parallel_policy, etc...
     * 
     *  Much simpler to just have a single enum rather than these 4 classes. STL choice based on (probably more historical)
     *  C++ limitations with overloading (no constexpr) - not much point in that anymore, I don't think.
     */
    enum class SequencePolicy {
        /**
         *  \brief default case - not parallelized
         * 
         *  Equivilent to std::seq, std::execution::sequenced_policy
         * 
         *  ..."a parallel algorithm's execution may not be parallelized.
         *  The invocations of element access functions in parallel algorithms invoked with this policy
         *  (usually specified as std::execution::seq) are indeterminately sequenced in the calling thread."
         */
        eSeq,

        /**
         *  \brief must syncronize shared data, can use mutex (or atomics), cuz each parallel execution in real thread
         * 
         *  Equivilent to std::par, std::execution::parallel_policy 
         * 
         *  ..."parallel algorithm's execution may be parallelized. The invocations of element access 
         *  functions in parallel algorithms invoked with this policy (usually specified as std::execution::par)
         *  are permitted to execute in either the invoking thread or in a thread implicitly created by the
         *  library to support parallel algorithm execution. Any such invocations executing in the same
         *  thread are indeterminately sequenced with respect to each other.
         */
        ePar,

        /**
         *  \brief Unclear how this differs from eUnseq, but no locks allowed
         * 
         *  Equivilent to std::par_unseq , std::execution::parallel_unsequenced_policy  
         * 
         *  ..."a parallel algorithm's execution may be parallelized, vectorized, 
         *  or migrated across threads (such as by a parent-stealing scheduler). The invocations of 
         *  element access functions in parallel algorithms invoked with this policy are permitted 
         *  to execute in an unordered fashion in unspecified threads, and unsequenced with respect 
         *  to one another within each thread..
         */
        eParUnseq,

        /**
         *  \brief SIMD, no locks allowed
         * 
         *  Equivilent to std::unseq  , std::execution::unsequenced_policy   
         * 
         *  ..."a parallel algorithm's execution may be parallelized, vectorized, 
         *  or migrated across threads (such as by a parent-stealing scheduler). The invocations of 
         *  element access functions in parallel algorithms invoked with this policy are permitted 
         *  to execute in an unordered fashion in unspecified threads, and unsequenced with respect 
         *  to one another within each thread..
         */
        eUnseq,

        Stroika_Define_Enum_Bounds (eSeq, eUnseq)

            eDefault = eSeq,
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Execution_Common_h_*/
