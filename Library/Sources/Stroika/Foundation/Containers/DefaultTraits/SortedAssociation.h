/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DefaultTraits_SortedAssociation_h_
#define _Stroika_Foundation_Containers_DefaultTraits_SortedAssociation_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Common/Compare.h"
#include "../../Configuration/Concepts.h"

/*
 *  \file
 *  TODO:
 *
 */

namespace Stroika::Foundation::Containers::DefaultTraits {

    /**
     *  Default Association<> Traits
     */
    template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER = Common::ComparerWithEquals<KEY_TYPE>, typename VALUE_EQUALS_COMPARER = Common::DefaultEqualsComparerOptionally<VALUE_TYPE>, typename KEY_WELL_ORDER_COMPARER = Common::ComparerWithWellOrder<KEY_TYPE>>
    struct SortedAssociation : Mapping_DefaultTraits<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER, VALUE_EQUALS_COMPARER> {
        /**
         */
        using KeyWellOrderCompareFunctionType = KEY_WELL_ORDER_COMPARER;

        /**
         */
        RequireConceptAppliesToTypeMemberOfClass (Concept_WellOrderCompareFunctionType, KeyWellOrderCompareFunctionType);
    };
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Containers_DefaultTraits_SortedAssociation_h_ */
