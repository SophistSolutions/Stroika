/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_STL_Compare_h_
#define _Stroika_Foundation_Containers_STL_Compare_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Common/Compare.h"
#include "../../Configuration/TypeHints.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace STL {

                /**
                 *  Apply Stroika equals comparer to STL (iterators or containers) and return true iff equal.
                 */
                template <typename STROIKA_EQUALS_COMPARER, typename ITERATOR_OF_T>
                bool Equals (ITERATOR_OF_T lhsStart, ITERATOR_OF_T lhsEnd, ITERATOR_OF_T rhsStart, ITERATOR_OF_T rhsEnd);
                template <typename STROIKA_EQUALS_COMPARER, typename CONTAINER_OF_T>
                bool Equals (const CONTAINER_OF_T& lhs, const CONTAINER_OF_T& rhs);
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Compare.inl"

#endif /*_Stroika_Foundation_Containers_STL_Compare_h_*/
