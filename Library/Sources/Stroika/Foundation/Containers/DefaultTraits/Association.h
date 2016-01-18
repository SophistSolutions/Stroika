/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DefaultTraits_Association_h_
#define _Stroika_Foundation_Containers_DefaultTraits_Association_h_  1

#include    "../../StroikaPreComp.h"

#include    "../../Common/Compare.h"
#include    "../../Configuration/Concepts.h"



/*
 *  \file
 *  TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   DefaultTraits {


                /**
                 *  Default Association<> Traits
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER = Common::DefaultEqualsComparer<KEY_TYPE>, typename VALUE_EQUALS_COMPARER = Common::DefaultEqualsComparerOptionally<VALUE_TYPE>>
                struct   Association {
                    /**
                     */
                    using   KeyEqualsCompareFunctionType    =   KEY_EQUALS_COMPARER;

                    RequireConceptAppliesToTypeMemberOfClass(Concept_EqualsCompareFunctionType, KeyEqualsCompareFunctionType);

                    /**
                     * only defined optionally...(see what can call this - gen list here @todo)
                     */
                    using   ValueEqualsCompareFunctionType  =   VALUE_EQUALS_COMPARER;

                    /**
                     *  Define typedef for this Association traits object (so other traits can generically allow recovery of the
                     *  underlying Association's TRAITS objects.
                     */
                    using   AssociationTraitsType               =    Association<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER, VALUE_EQUALS_COMPARER>;
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

#endif  /*_Stroika_Foundation_Containers_DefaultTraits_Association_h_ */
