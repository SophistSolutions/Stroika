/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Concept_h_
#define _Stroika_Foundation_Configuration_Concept_h_ 1

#include "../StroikaPreComp.h"

#include <iterator> // needed for std::begin/std::end calls
#include <type_traits>

#include "../Configuration/Common.h"

/*
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
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

namespace Stroika {
    namespace Foundation {
        namespace Configuration {

            /**
             *  LIFTED -@todo add docs/reference
             *  from Stroustrup C++11 book - page 800
             */
            struct substitution_failure {
            };

            /**
             *  LIFTED -@todo add docs/reference
             *  from Stroustrup C++11 book - page 800
             */
            template <typename T>
            struct substitution_succeeded : std::true_type {
            };
            template <>
            struct substitution_succeeded<substitution_failure> : std::false_type {
            };

/**
             *  \brief  Define has_XXX (where you specify XXX) class which has a 'value' field saying
             *          if the XXX property is present.
             *
             *  Since I cannot (so far figure out how to) do in a single simple statement/template,
             *  at least do this magic in a macro...
             */
#define STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS(NAME, XTEST)                                                                                  \
    namespace Private_ {                                                                                                                          \
        template <typename T>                                                                                                                     \
        struct NAME##_result_impl {                                                                                                               \
            template <typename X>                                                                                                                 \
            static auto                                                     check (const X& x) -> decltype (XTEST);                               \
            static Stroika::Foundation::Configuration::substitution_failure check (...);                                                          \
            using type = decltype (check (declval<T> ()));                                                                                        \
        };                                                                                                                                        \
    }                                                                                                                                             \
    template <typename T>                                                                                                                         \
    using NAME##_result = typename Private_::NAME##_result_impl<T>::type;                                                                         \
    template <typename T>                                                                                                                         \
    struct has_##NAME : integral_constant<bool, not is_same<NAME##_result<T>, Stroika::Foundation::Configuration::substitution_failure>::value> { \
    };

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
            STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (eq, (x == x));
            STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (neq, (x != x));
            STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (lt, (x < x));

            /*
             *  has_beginend<T>::value is true iff T has a begin/end method
             *  @todo fix so checks results act more like iterators - subclass from iterator_tag>
             */
            STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (beginend, (std::begin (x) != std::end (x)));

            /*
             *  has_beginend<T>::value is true iff T has a begin/end method
             *  @todo fix so checks results act more like iterators - subclass from iterator_tag>
             */
            STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (begin, std::begin (x));

            /**
             *  Would like to use constexpr function (as with c++14 concepts) - but cannot due to weakness in constexpr support (absence) for vs2k13
             */
            namespace Private_ {
                template <typename ITERABLE_OF_T, typename T>
                struct IsIterableOfT_Impl2_ {
                    template <typename X, typename USE_ITERABLE = ITERABLE_OF_T, bool ITER_RESULT_CONVERTIBLE_TO_T = std::is_convertible<typename std::iterator_traits<begin_result<USE_ITERABLE>>::value_type, T>::value>
                    static auto check (const X& x) ->
                        typename std::conditional<
                            has_beginend<ITERABLE_OF_T>::value and
                                ITER_RESULT_CONVERTIBLE_TO_T,
                            substitution_succeeded<T>,
                            substitution_failure>::type;
                    static substitution_failure check (...);
                    using type = decltype (check (declval<T> ()));
                };
            }
            /**
             *  Check if has begin/end methods (not for subclassing Traversal::Iterable<>)
             */
            template <typename ITERABLE_OF_T, typename T>
            using IsIterableOfT = std::integral_constant<bool, not std::is_same<typename Private_::IsIterableOfT_Impl2_<ITERABLE_OF_T, T>::type, substitution_failure>::value>;

            /**
             *  See http://en.cppreference.com/w/cpp/concept/Container
             */
            template <typename T>
            constexpr bool Container ()
            {
#if 1
                // no where near enough, but a start...
                // IsIterableOfT<T>::value would be closer, but until we fix the SFINAE bug with that - that it doesn't compile instead of returning false - we msut use has_beginend
                return has_beginend<T>::value;
#else
                // about right?
                return IsIterableOfT<T>::value;
#endif
            }

            /**
             *  See http://en.cppreference.com/w/cpp/concept/EqualityComparable
             */
            template <typename T>
            constexpr bool EqualityComparable ()
            {
                return has_eq<T>::value && is_convertible<eq_result<T>, bool>::value;
            }

            /**
             *  See http://en.cppreference.com/w/cpp/concept/LessThanComparable
             */
            template <typename T>
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
                    using T = typename TRAITS::value_type;
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
                    using T = typename TRAITS::value_type;
                    if (false) {
                        int i = TRAITS::Compare (*static_cast<const T*> (nullptr), *static_cast<const T*> (nullptr));
                    }
                }
                Concept_WellOrderCompareFunctionType ()
                {
                    check ();
                }
            };

            /*
             * FROM http://stackoverflow.com/questions/16893992/check-if-type-can-be-explicitly-converted
             */
            template <typename From, typename To>
            struct is_explicitly_convertible {
                template <typename T>
                static void f (T);

                template <typename F, typename T>
                static constexpr auto test (int) -> decltype (f (static_cast<T> (std::declval<F> ())), true)
                {
                    return true;
                }

                template <typename F, typename T>
                static constexpr auto test (...) -> bool
                {
                    return false;
                }

                static bool const value = test<From, To> (0);
            };

#if qCheckConceptRequirements

#define RequireConceptAppliesToTypeMemberOfClass1(TEMPLATE, T, REQUIREMEMENT_NAME) \
    Stroika::Foundation::Configuration::TEMPLATE<T> _IGNORE_##REQUIREMEMENT_NAME;

#define RequireConceptAppliesToTypeMemberOfClass(TEMPLATE, T) \
    RequireConceptAppliesToTypeMemberOfClass1 (TEMPLATE, T, _REQ_##TEMPLATE)

#define RequireConceptAppliesToTypeInFunction(TEMPLATE, T) \
    Stroika::Foundation::Configuration::TEMPLATE<T> ();

#else

#define RequireConceptAppliesToTypeMemberOfClass(TEMPLATE, T, REQUIREMEMENT_NAME)
#define RequireConceptAppliesToTypeInFunction(TEMPLATE, T)

#endif
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Concepts.inl"

#endif /*_Stroika_Foundation_Configuration_Concept_h_ */
