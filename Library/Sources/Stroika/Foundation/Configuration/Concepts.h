/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Concept_h_
#define _Stroika_Foundation_Configuration_Concept_h_   1

#include    "../StroikaPreComp.h"

#include    <type_traits>

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


            //LIFTED -@todo add docs/reference
            // from Stroustrup C++11 book - page 800
            struct substition_failure {};

            template <typename T> struct substitution_succeeded : std::true_type {};
            template <> struct substitution_succeeded<substition_failure> : std::false_type {};


            /*
             *BASED ON
                    http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3701.pdf
             *
             *  Starting to experiment...
             */

            template<typename T>
            struct eq_result_impl {
                template<typename X>
                static auto check(const X& x) -> decltype(x == x);
                static substition_failure check(...);
                using type = decltype(check(declval<T>()));
            };
            template<typename T>
            using eq_result = typename eq_result_impl<T>::type;
            template<typename T>
            struct has_eq
                    : integral_constant < bool, !is_same<eq_result<T>, substition_failure>::value >
            { };

            template<typename T>
            struct neq_result_impl {
                template<typename X>
                static auto check(const X& x) -> decltype(x != x);
                static substition_failure check(...);
                using type = decltype(check(declval<T>()));
            };
            template<typename T>
            using neq_result = typename neq_result_impl<T>::type;
            template<typename T>
            struct has_neq
                    : integral_constant < bool, !is_same<neq_result<T>, substition_failure>::value >
            { };


            template    <typename T>
            constexpr bool Equality_comparable ()
            {
                return has_eq<T>::value && is_convertible<eq_result<T>, bool>::value
                       && has_neq<T>::value && is_convertible<neq_result<T>, bool>::value;
            }
            template    <typename T>
            constexpr bool Weakly_ordered ()
            {
                return false;   // NYI
            }
            template    <typename T>
            constexpr bool Totally_ordered ()
            {
                return Weakly_ordered<T> () && Equality_comparable<T> ();
            }



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
