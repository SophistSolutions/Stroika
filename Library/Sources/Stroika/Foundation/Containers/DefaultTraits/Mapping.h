/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DefaultTraits_Mapping_h_
#define _Stroika_Foundation_Containers_DefaultTraits_Mapping_h_  1

#include    "../../StroikaPreComp.h"

#include    "../../Common/Compare.h"
#include    "../../Common/KeyValuePair.h"
#include    "../../Configuration/Common.h"
#include    "../../Configuration/Concepts.h"
#include    "../../Execution/Synchronized.h"
#include    "../../Memory/Optional.h"
#include    "../../Traversal/Iterable.h"



/*
 *  \file
 *  TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   DefaultTraits {


                using   Configuration::ArgByValueType;
                using   Traversal::Iterable;
                using   Traversal::Iterator;

                using   Common::KeyValuePair;


                /**
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER = Common::DefaultEqualsComparer<KEY_TYPE>>
                struct   Mapping {
                    /**
                     */
                    using   KeyEqualsCompareFunctionType    =   KEY_EQUALS_COMPARER;

                    RequireConceptAppliesToTypeMemberOfClass(Concept_EqualsCompareFunctionType, KeyEqualsCompareFunctionType);

                    /**
                     *  Define typedef for this Mapping traits object (so other traits can generically allow recovery of the
                     *  underlying Mapping's TRAITS objects.
                     */
                    using   MappingTraitsType               =    Mapping<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>;
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

#endif  /*_Stroika_Foundation_Containers_DefaultTraits_Mapping_h_ */
