/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DefaultTraits_SortedMapping_h_
#define _Stroika_Foundation_Containers_DefaultTraits_SortedMapping_h_ 1

#include "../../StroikaPreComp.h"

#include "Mapping.h"

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
                 *  Default SortedMapping<> Traits
                 */
                template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER = typename Mapping<KEY_TYPE, VALUE_TYPE>::KeyEqualsCompareFunctionType, typename KEY_WELL_ORDER_COMPARER = Common::ComparerWithWellOrder<KEY_TYPE>>
                struct SortedMapping : Mapping<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER> {
                    /**
                     */
                    using KeyWellOrderCompareFunctionType = KEY_WELL_ORDER_COMPARER;

                    /**
                     */
                    RequireConceptAppliesToTypeMemberOfClass (Concept_WellOrderCompareFunctionType, KeyWellOrderCompareFunctionType);
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

#endif /*_Stroika_Foundation_Containers_DefaultTraits_SortedMapping_h_ */
