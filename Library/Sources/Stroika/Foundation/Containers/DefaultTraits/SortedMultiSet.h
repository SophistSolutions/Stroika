/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DefaultTraits_SortedMultiSet_h_
#define _Stroika_Foundation_Containers_DefaultTraits_SortedMultiSet_h_ 1

#include "../../StroikaPreComp.h"

#include "MultiSet.h"

/*
 *  \file
 *  TODO:
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace DefaultTraits {

                /**
                 *  Default MultiSet<> Traits
                 */
                template <typename T, typename WELL_ORDER_COMPARER = Common::ComparerWithWellOrder<T>>
                struct SortedMultiSet : MultiSet<T, WELL_ORDER_COMPARER> {
                    /**
                     */
                    using WellOrderCompareFunctionType = WELL_ORDER_COMPARER;

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

#endif /*_Stroika_Foundation_Containers_DefaultTraits_SortedMultiSet_h_ */
