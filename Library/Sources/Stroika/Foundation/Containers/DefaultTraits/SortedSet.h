/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DefaultTraits_SortedSet_h_
#define _Stroika_Foundation_Containers_DefaultTraits_SortedSet_h_ 1

#include "../../StroikaPreComp.h"

#include "Set.h"

/*
 *  \file
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace DefaultTraits {

                /**
                 *  Default Set<> Traits
                 *  \req    DEFAULT IMPLEMENTATION (with no args) - RequireConceptAppliesToTypeMemberOfClass(RequireOperatorLess, T);
                 */
                template <typename T, typename EQUALS_COMPARER = typename Set<T>::EqualsCompareFunctionType, typename WELL_ORDER_COMPARER = Common::ComparerWithWellOrder<T>>
                struct SortedSet : Set<T, EQUALS_COMPARER> {
                    /**
                     */
                    using WellOrderCompareFunctionType = WELL_ORDER_COMPARER;

                    /**
                    */
                    RequireConceptAppliesToTypeMemberOfClass (Concept_WellOrderCompareFunctionType, WellOrderCompareFunctionType);
                };
            }
        }
    }
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Containers_DefaultTraits_SortedSet_h_ */
