/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Concept_h_
#define _Stroika_Foundation_Configuration_Concept_h_   1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"



/*
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * Description:
 *
 *
 *
 * TODO:
 *      @todo   Maybe put concepts in sub-namespace Concept {} - and then list them out here? Instead of the _ stuff?
 *
 *      @todo   Think out if/when qCheckConceptRequirements SB defined? See if it takes up any runtime space/time? And decide
 *              according to that!
 *
 *      @todo   I think the RequireConceptAppliesToTypeInFunction can be replaced with RequireConceptAppliesToTypeMemberOfClass,
 *              and then both renamed to something simpler... test...
 *
 *
 * Notes:
 *
 *
 *
 */



/**
 */
#ifndef qCheckConceptRequirements
#define qCheckConceptRequirements 1
#endif



namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {


            /*
             * See GCC /usr/include/c++/4.7/bits/boost_concept_check.h
             */

            template <typename T>
            struct RequireOperatorEquals {
                static void check ()
                {
                    if (false) {
                        if (*static_cast<const T*> (nullptr) == *static_cast<const T*> (nullptr)) {
                        }
                    }
                }
                RequireOperatorEquals ()
                {
                    check ();
                }
            };


            template <typename T>
            struct RequireOperatorNotEquals {
                static void check ()
                {
                    if (false) {
                        if (*static_cast<const T*> (nullptr) != *static_cast<const T*> (nullptr)) {
                        }
                    }
                }
                RequireOperatorNotEquals ()
                {
                    check ();
                }
            };


            template <typename T>
            struct RequireOperatorLess {
                static void check ()
                {
                    if (false) {
                        if (*static_cast<const T*> (nullptr) < *static_cast<const T*> (nullptr)) {
                        }
                    }
                }
                RequireOperatorLess ()
                {
                    check ();
                }
            };


            template <typename TRAITS>
            struct Concept_EqualsCompareFunctionType {
                static void check ()
                {
                    using   T       =   typename TRAITS::ElementType;
                    if (false) {
                        if (TRAITS::Equals (*static_cast<const T*> (nullptr), *static_cast<const T*> (nullptr))) {
                        }
                    }
                }
                Concept_EqualsCompareFunctionType ()
                {
                    check ();
                }
            };


            template <typename TRAITS>
            struct Concept_WellOrderCompareFunctionType {
                static void check ()
                {
                    using   T       =   typename TRAITS::ElementType;
                    if (false) {
                        int i = TRAITS::Compare (*static_cast<const T*> (nullptr), *static_cast<const T*> (nullptr));
                    }
                }
                Concept_WellOrderCompareFunctionType ()
                {
                    check ();
                }
            };




#if     qCheckConceptRequirements

#define RequireConceptAppliesToTypeMemberOfClass1(TEMPLATE,T,REQUIREMEMENT_NAME)\
    Stroika::Foundation::Configuration::TEMPLATE<T> _IGNORE_##REQUIREMEMENT_NAME;

#define RequireConceptAppliesToTypeMemberOfClass(TEMPLATE,T)\
    RequireConceptAppliesToTypeMemberOfClass1(TEMPLATE,T,_REQ_##TEMPLATE)

#define RequireConceptAppliesToTypeInFunction(TEMPLATE,T)\
    Stroika::Foundation::Configuration::TEMPLATE<T> ();

#else

#define RequireConceptAppliesToTypeMemberOfClass(TEMPLATE,T,REQUIREMEMENT_NAME)
#define RequireConceptAppliesToTypeInFunction(TEMPLATE,T)

#endif





        }
    }
}


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Concepts.inl"

#endif  /*_Stroika_Foundation_Configuration_Concept_h_ */
