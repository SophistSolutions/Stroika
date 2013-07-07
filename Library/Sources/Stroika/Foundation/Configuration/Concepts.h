/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Concept_h_
#define _Stroika_Foundation_Configuration_Concept_h_   1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"



/*
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 * Description:
 *
 *
 *
 * TODO:
 *
 *
 *
 * Notes:
 *
 *
 *
 */


/*
 * See DOCS in Stroika/Foundation/Characters/Format.h
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
                static void check () {
                    if (false) {
                        if (*static_cast<const T*> (nullptr) == *static_cast<const T*> (nullptr)) {
                        }
                    }
                }
                RequireOperatorEquals () {
                    check ();
                }
            };

            template <typename T>
            struct RequireOperatorNotEquals {
                static void check () {
                    if (false) {
                        if (*static_cast<const T*> (nullptr) != *static_cast<const T*> (nullptr)) {
                        }
                    }
                }
                RequireOperatorNotEquals () {
                    check ();
                }
            };

            template <typename T>
            struct RequireOperatorLess {
                static void check () {
                    if (false) {
                        if (*static_cast<const T*> (nullptr) < *static_cast<const T*> (nullptr)) {
                        }
                    }
                }
                RequireOperatorLess () {
                    check ();
                }
            };


            template <typename TRAITS>
            struct Concept_EqualsCompareFunctionType {
                static void check () {
                    typedef typename TRAITS::ElementType        T;
                    if (false) {
                        if (TRAITS::Equals (*static_cast<const T*> (nullptr), *static_cast<const T*> (nullptr))) {
                        }
                    }
                }
                Concept_EqualsCompareFunctionType () {
                    check ();
                }
            };




#if     qCheckConceptRequirements

#define RequireElementTraitsInClass1(TEMPLATE,T,REQUIREMEMENT_NAME)\
    Stroika::Foundation::Configuration::TEMPLATE<T> _IGNORE_##REQUIREMEMENT_NAME;

#define RequireElementTraitsInClass(TEMPLATE,T)\
    RequireElementTraitsInClass1(TEMPLATE,T,_REQ_##TEMPLATE)

#define RequireElementTraitsInMethod(TEMPLATE,T)\
    Stroika::Foundation::Configuration::TEMPLATE<T> ();

#else

#define RequireElementTraitsInClass(TEMPLATE,T,REQUIREMEMENT_NAME)
#define RequireElementTraitsInMethod(TEMPLATE,T)

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
