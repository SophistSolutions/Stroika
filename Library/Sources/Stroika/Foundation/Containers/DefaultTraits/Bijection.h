/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DefaultTraits_Bijection_h_
#define _Stroika_Foundation_Containers_DefaultTraits_Bijection_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Common/Compare.h"
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
                 */
                enum Bijection_InjectivityViolationPolicy {
                    eAssertionError,
                    eThrowException,
                };

                /**
                 *  Default Bijection<> Traits
                 */
                template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename DOMAIN_EQUALS_COMPARER = Common::DefaultEqualsComparer<DOMAIN_TYPE>, typename RANGE_EQUALS_COMPARER = Common::DefaultEqualsComparerOptionally<RANGE_TYPE>>
                struct Bijection {
                    /**
                     */
                    using DomainEqualsCompareFunctionType = DOMAIN_EQUALS_COMPARER;

                    RequireConceptAppliesToTypeMemberOfClass (Concept_EqualsCompareFunctionType, DomainEqualsCompareFunctionType);

                    /**
                     * only defined optionally...(see what can call this - gen list here @todo)
                     */
                    using RangeEqualsCompareFunctionType = RANGE_EQUALS_COMPARER;

                    /**
                     *  Define typedef for this Bijection traits object (so other traits can generically allow recovery of the
                     *  underlying Bijection's TRAITS objects.
                     */
                    using BijectionTraitsType = Bijection<DOMAIN_TYPE, RANGE_TYPE, DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>;

                    /**
                     */
                    static constexpr Bijection_InjectivityViolationPolicy InjectivityViolationPolicy = Bijection_InjectivityViolationPolicy::eAssertionError;
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

#endif /*_Stroika_Foundation_Containers_DefaultTraits_Bijection_h_ */
