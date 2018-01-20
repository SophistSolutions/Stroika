/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DefaultTraits_SortedCollection_h_
#define _Stroika_Foundation_Containers_DefaultTraits_SortedCollection_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Common/Compare.h"
#include "../../Configuration/Concepts.h"

/*
 *  \file
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace DefaultTraits {

                /**
                 *  Default SortedCollection<> Traits
                *
                *  Traits to define the well-ordering of elements of the SortedCollection.
                *
                *  Note - that a well-ordering also imputes a notion of equality (not (a<b or b < a)), so we define
                *  that as well.
                *
                */
                template <typename T, typename WELL_ORDER_COMPARER = Common::ComparerWithWellOrder<T>>
                struct SortedCollection {

                    /**
                    */
                    using EqualsCompareFunctionType = WELL_ORDER_COMPARER;

                    RequireConceptAppliesToTypeMemberOfClass (Concept_EqualsCompareFunctionType, EqualsCompareFunctionType);

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

#endif /*_Stroika_Foundation_Containers_DefaultTraits_SortedCollection_h_ */
