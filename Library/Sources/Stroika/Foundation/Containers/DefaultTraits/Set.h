/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DefaultTraits_Set_h_
#define _Stroika_Foundation_Containers_DefaultTraits_Set_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Common/Compare.h"
#include "../../Configuration/Common.h"
#include "../../Configuration/Concepts.h"

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
                 *  Default Set<> Traits
                 */
                template <typename T, typename EQUALS_COMPARER = Common::DefaultEqualsComparer<T>>
                struct Set {
                    /**
                     */
                    using EqualsCompareFunctionType = EQUALS_COMPARER;

                    RequireConceptAppliesToTypeMemberOfClass (Concept_EqualsCompareFunctionType, EqualsCompareFunctionType);

                    /**
                     *  Define typedef for this Set traits object (so other traits can generically allow recovery of the
                     *  underlying Set's TRAITS objects.
                     */
                    using SetTraitsType = Set<T, EQUALS_COMPARER>;
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

#endif /*_Stroika_Foundation_Containers_DefaultTraits_Set_h_ */
