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


            /**
             *  LIFTED -@todo add docs/reference
             *  from Stroustrup C++11 book - page 800
             */
            struct substitution_failure {};


            /**
             *  LIFTED -@todo add docs/reference
             *  from Stroustrup C++11 book - page 800
             */
            template <typename T> struct substitution_succeeded : std::true_type {};
            template <> struct substitution_succeeded<substitution_failure> : std::false_type {};


            /*
             *  BASED ON
             *      http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3701.pdf
             *
             *  But not in standard yet, and these not well documented. So go with definitions in
             *      http://en.cppreference.com/w/cpp/concept/
             *  for now
             *
             *  Starting to experiment...
             *  But since I cannot do in a single simple statement/template, at least do this magic in a macro...
             */
#define STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS(NAME,XTEST)\
    namespace Private_ {\
        using   namespace Stroika::Foundation::Configuration;\
        template    <typename T>\
        struct  NAME##_result_impl {\
            template    <typename X>\
            static auto check(const X& x) -> decltype(XTEST);\
            static substitution_failure check(...);\
            using type = decltype(check(declval<T>()));\
        };\
    }\
    template    <typename T>\
    using   NAME##_result = typename Private_::NAME##_result_impl<T>::type;\
    template    <typename T>\
    struct  has_##NAME : integral_constant <bool, not is_same<NAME##_result<T>, substitution_failure>::value> {};





            /*
             *  BASED ON
             *      http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3701.pdf
             *
             *  But not in standard yet, and these not well documented. So go with definitions in
             *      http://en.cppreference.com/w/cpp/concept/
             *  for now
             *
             *  Starting to experiment...
             */
#if 1
            STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS(eq, (x == x));
#else
            namespace Private_ {
                template    <typename T>
                struct  eq_result_impl {
                    template    <typename X>
                    static auto check(const X& x) -> decltype(x == x);
                    static substitution_failure check(...);
                    using type = decltype(check(declval<T>()));
                };
            }
            template    <typename T>
            using   eq_result = typename Private_::eq_result_impl<T>::type;
            template    <typename T>
            struct  has_eq : integral_constant <bool, not is_same<eq_result<T>, substitution_failure>::value> {};
#endif

#if 1
            STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS(neq, (x != x));
#else
            namespace Private_ {
                template    <typename T>
                struct  neq_result_impl {
                    template    <typename X>
                    static auto check(const X& x) -> decltype(x != x);
                    static substitution_failure check(...);
                    using type = decltype(check(declval<T>()));
                };
            }
            template    <typename T>
            using   neq_result = typename Private_::neq_result_impl<T>::type;
            template    <typename T>
            struct  has_neq : integral_constant <bool, not is_same<neq_result<T>, substitution_failure>::value>   {};
#endif


#if 1
            STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS(lt, (x < x));
#else
            namespace Private_ {
                template    <typename T>
                struct  lt_result_impl {
                    template    <typename X>
                    static auto check(const X& x) -> decltype(x < x);
                    static substitution_failure check(...);
                    using type = decltype(check(declval<T>()));
                };
            }
            template    <typename T>
            using   lt_result = typename Private_::lt_result_impl<T>::type;
            template    <typename T>
            struct  has_lt : integral_constant <bool, not is_same<lt_result<T>, substitution_failure>::value> {};
#endif


            /**
             *  See http://en.cppreference.com/w/cpp/concept/EqualityComparable
             */
            template    <typename T>
            constexpr bool  EqualityComparable ()
            {
                return has_eq<T>::value && is_convertible<eq_result<T>, bool>::value;
            }


            /**
             *  See http://en.cppreference.com/w/cpp/concept/LessThanComparable
             */
            template    <typename T>
            constexpr bool LessThanComparable ()
            {
                return has_lt<T>::value && is_convertible<lt_result<T>, bool>::value;
            }



            /*
             * See GCC /usr/include/c++/4.7/bits/boost_concept_check.h
             *      @todo OBSOLETE - USE static_assert<EqualityComparable<T>>
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


            /*
             * See GCC /usr/include/c++/4.7/bits/boost_concept_check.h
             *      @todo OBSOLETE - USE static_assert<EqualityComparable<T>>
             */
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


            /*
             *      @todo OBSOLETE - USE static_assert<LessThanComparable<T>>
             */
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
