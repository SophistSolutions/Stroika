/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Concept_h_
#define _Stroika_Foundation_Configuration_Concept_h_ 1

#include "../StroikaPreComp.h"

#include <functional> // needed for std::equal_to
#include <iterator>   // needed for std::begin/std::end calls

#include "../Configuration/Common.h"

#include "ConceptsBase.h"

/*
 *
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 *
 * TODO:
 *      @todo   Maybe put concepts in sub-namespace Concept {} - and then list them out here? Instead of the _ stuff?
 *
 *      @todo   Think out if/when qCheckConceptRequirements SB defined? See if it takes up any runtime space/time? And decide
 *              according to that!
 */

namespace Stroika::Foundation::Configuration {

    /**
     *  \brief check if the given type T can be compared with operator==
     * 
     *  \par Example Usage
     *      \code
     *          if constexpr (has_eq_v<T>) {
     *              T a{};
     *              T b{};
     *              return a == b;
     *          }
     *      \endcode
     * 
     *  \note see https://stroika.atlassian.net/browse/STK-749 - for why pair/tuple specializations - not sure why STL doesn't do this directly in pair<> template
     * 
     *  \note - why no has_equal_to_v<> defined?
     *        @see https://stackoverflow.com/questions/70119120/how-to-fix-sfinae-check-for-operator-existing-so-that-it-works-with-stdpair/70122069#70122069
     * 
     *  Issue is that it cannot be usefully defined (as nearly as I can tell in C++17).
     */
    template <typename T>
    using has_eq_t = decltype (static_cast<bool> (std::declval<T> () == std::declval<T> ()));
    template <typename T>
    constexpr inline bool has_eq_v = is_detected_v<has_eq_t, T>;
    template <typename T, typename U>
    constexpr inline bool has_eq_v<std::pair<T, U>> = has_eq_v<T>and has_eq_v<U>;
    template <typename... Ts>
    constexpr inline bool has_eq_v<std::tuple<Ts...>> = (has_eq_v<Ts> and ...);

    /**
     *  \brief check if the given type T can be compared with operator!=
     * 
     *  \par Example Usage
     *      \code
     *          if constexpr (has_neq_v<T>) {
     *              T a{};
     *              T b{};
     *              return a != b;
     *          }
     *      \endcode
     * 
     *  \note see https://stroika.atlassian.net/browse/STK-749 - for why pair/tuple specializations - not sure why STL doesn't do this directly in pair<> template
     */
    template <typename T>
    using has_neq_t = decltype (static_cast<bool> (std::declval<T> () != std::declval<T> ()));
    template <typename T>
    constexpr inline bool has_neq_v = is_detected_v<has_neq_t, T>;
    template <typename T, typename U>
    constexpr inline bool has_neq_v<std::pair<T, U>> = has_neq_v<T>and has_neq_v<U>;
    template <typename... Ts>
    constexpr inline bool has_neq_v<std::tuple<Ts...>> = (has_neq_v<Ts> and ...);

    /**
     *  \brief check if the given type T can be compared with operator!=
     * 
     *  \par Example Usage
     *      \code
     *          if constexpr (has_lt_v<T>) {
     *              T a{};
     *              T b{};
     *              return a < b;
     *          }
     *      \endcode
     * 
     *  \note see https://stroika.atlassian.net/browse/STK-749 - for why pair/tuple specializations - not sure why STL doesn't do this directly in pair<> template
     */
    template <typename T>
    using has_lt_t = decltype (static_cast<bool> (std::declval<T> () < std::declval<T> ()));
    template <typename T>
    constexpr inline bool has_lt_v = is_detected_v<has_lt_t, T>;
    template <typename T, typename U>
    constexpr inline bool has_lt_v<std::pair<T, U>> = has_lt_v<T>and has_lt_v<U>;
    template <typename... Ts>
    constexpr inline bool has_lt_v<std::tuple<Ts...>> = (has_lt_v<Ts> and ...);

    /**
     *  \brief check if the given type T can be compared with operator!=
     * 
     *  \par Example Usage
     *      \code
     *          if constexpr (has_minus_v<T>) {
     *              T a{};
     *              T b{};
     *              return a - b;
     *          }
     *      \endcode
     * 
     *  \note see https://stroika.atlassian.net/browse/STK-749 - for why pair/tuple specializations - not sure why STL doesn't do this directly in pair<> template
     */
    template <typename T>
    using has_minus_t = decltype (std::declval<T> () - std::declval<T> ());
    template <typename T>
    constexpr inline bool has_minus_v = is_detected_v<has_minus_t, T>;
    template <typename T, typename U>
    constexpr inline bool has_minus_v<std::pair<T, U>> = has_minus_v<T>and has_minus_v<U>;
    template <typename... Ts>
    constexpr inline bool has_minus_v<std::tuple<Ts...>> = (has_minus_v<Ts> and ...);

    /**
     *  \brief check if the given type T can be compared with operator+
     * 
     *  \par Example Usage
     *      \code
     *          if constexpr (has_plus_v<T>) {
     *              T a{};
     *              T b{};
     *              return a - b;
     *          }
     *      \endcode
     * 
     *  \note see https://stroika.atlassian.net/browse/STK-749 - for why pair/tuple specializations - not sure why STL doesn't do this directly in pair<> template
     */
    template <typename T>
    using has_plus_t = decltype (std::declval<T> () + std::declval<T> ());
    template <typename T>
    constexpr inline bool has_plus_v = is_detected_v<has_plus_t, T>;
    template <typename T, typename U>
    constexpr inline bool has_plus_v<std::pair<T, U>> = has_plus_v<T>and has_plus_v<U>;
    template <typename... Ts>
    constexpr inline bool has_plus_v<std::tuple<Ts...>> = (has_plus_v<Ts> and ...);

    /**
     *  \brief check if the given type T can be compared with operator<=>
     * 
     *  \par Example Usage
     *      \code
     *          if constexpr (has_spaceship_v<T>) {
     *              T a{};
     *              T b{};
     *              return a <=> b;
     *          }
     *      \endcode
     * 
     *  \note see https://stroika.atlassian.net/browse/STK-749 - for why pair/tuple specializations - not sure why STL doesn't do this directly in pair<> template
     */
#if __cpp_impl_three_way_comparison >= 201907
    template <typename T>
    using has_spaceship_t = decltype (std::declval<T> () <=> std::declval<T> ());
    template <typename T>
    constexpr inline bool has_spaceship_v = is_detected_v<has_spaceship_t, T>;
    template <typename T, typename U>
    constexpr inline bool has_spaceship_v<std::pair<T, U>> = has_spaceship_v<T>and has_spaceship_v<U>;
    template <typename... Ts>
    constexpr inline bool has_spaceship_v<std::tuple<Ts...>> = (has_spaceship_v<Ts> and ...);
#endif

    // move to bottom when I've removed dependencies
    STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (eq, (x == x));   // DEPRECATED - use has_eq_v
    STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (minus, (x - x)); // DEPRECATED - use has_minus_v
    STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (lt, (x < x));    // DEPRECATED - use has_lt_v

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
     */
    namespace Private_ {
        template <typename ITERABLE_OF_T, typename T>
        struct IsIterableOfT_Impl2_ {
            template <typename X, typename USE_ITERABLE = ITERABLE_OF_T, bool ITER_RESULT_CONVERTIBLE_TO_T = is_convertible_v<typename iterator_traits<begin_result<USE_ITERABLE>>::value_type, T>>
            static auto check (const X& x) -> conditional_t<
                has_beginend<ITERABLE_OF_T>::value and
                    ITER_RESULT_CONVERTIBLE_TO_T,
                substitution_succeeded<T>,
                substitution_failure>;
            static substitution_failure check (...);
            using type = decltype (check (declval<T> ()));
        };
    }
    /**
     *  Check if has begin/end methods 
     */
    template <typename ITERABLE>
    using IsIterable = integral_constant<bool, has_beginend<ITERABLE>::value>;

    /**
     *  Check if has begin/end methods (not for subclassing Traversal::Iterable<>)
     */
    template <typename ITERABLE>
    constexpr bool IsIterable_v = IsIterable<ITERABLE>::value;

    /**
     *  Check if has begin/end methods (not for subclassing Traversal::Iterable<>)
     */
    template <typename ITERABLE_OF_T, typename T>
    using IsIterableOfT = integral_constant<bool, not is_same<typename Private_::IsIterableOfT_Impl2_<ITERABLE_OF_T, T>::type, substitution_failure>::value>;

    /**
     *  Check if has begin/end methods (not for subclassing Traversal::Iterable<>)
     */
    template <typename ITERABLE_OF_T, typename T>
    constexpr bool IsIterableOfT_v = IsIterableOfT<ITERABLE_OF_T, T>::value;

    /**
     *  See http://en.cppreference.com/w/cpp/concept/EqualityComparable
     */
    template <typename T>
    constexpr bool EqualityComparable ()
    {
        return has_eq<T>::value && is_convertible_v<eq_result<T>, bool>;
    }

    /**
     *  See http://en.cppreference.com/w/cpp/concept/LessThanComparable
     */
    template <typename T>
    constexpr bool LessThanComparable ()
    {
        return has_lt<T>::value && is_convertible_v<lt_result<T>, bool>;
    }

    template <typename T>
    constexpr bool HasMinusWithIntegerResult ()
    {
        return has_minus<T>::value && is_convertible_v<minus_result<T>, int>;
    }

    /**
     *  Check if equal_to<T> is both well defined, and contains no data. The reason it matters that it contains no data, is because
     *  then one instance is as good as another, and it need not be passed anywhere, opening an optimization opportunity.
     */
    template <typename T>
    constexpr bool HasUsableEqualToOptimization ()
    {
        if constexpr (has_eq_v<T>) {
            struct equal_to_empty_tester : equal_to<T> {
                int a;
            };
            // leverage empty base class optimization to see if equal_to contains any real data
            return sizeof (equal_to_empty_tester) == sizeof (int);
        }
        return false;
    }

    /*
     * FROM http://stackoverflow.com/questions/16893992/check-if-type-can-be-explicitly-converted
     */
    template <typename From, typename To>
    struct is_explicitly_convertible {
        template <typename T>
        static void f (T);

        template <typename F, typename T>
        static constexpr auto test (int) -> decltype (f (static_cast<T> (declval<F> ())), true)
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

    /**
     *  Check T is an interator, but checking if it has iterator_traits...
     *  from https://stackoverflow.com/questions/12032771/how-to-check-if-an-arbitrary-type-is-an-iterator
     */
    template <typename T, typename = void>
    struct is_iterator {
        static constexpr bool value = false;
    };
    template <typename T>
    struct is_iterator<T, enable_if_t<!is_same_v<typename iterator_traits<T>::value_type, void>>> {
        static constexpr bool value = true;
    };

    /**
     *  Check T is an interator, but checking if it has iterator_traits...
     */
    template <typename T>
    constexpr bool is_iterator_v = is_iterator<T>::value;

    namespace Private_ {
        // From https://stackoverflow.com/questions/15393938/find-out-if-a-c-object-is-callable
        template <typename T>
        struct is_callable_impl_ {
        private:
            typedef char (&yes)[1];
            typedef char (&no)[2];

            struct Fallback {
                void operator() ();
            };
            struct Derived : T, Fallback {
            };

            template <typename U, U>
            struct Check;

            template <typename>
            static yes test (...);

            template <typename C>
            static no test (Check<void (Fallback::*) (), &C::operator()>*);

        public:
            static const bool value = sizeof (test<Derived> (0)) == sizeof (yes);
        };
    }
    /**
     */
    template <typename T>
    using is_callable = conditional_t<is_class_v<T>, Private_::is_callable_impl_<T>, false_type>;

    /**
     */
    template <typename T>
    constexpr bool is_callable_v = is_callable<T>::value;

    // handy to have remove_cvref/remove_cvref_t definition around, even if using pre-c++20
    // Put in our namespace, and when we switch to C++20, we can deprecate our local namespace copy
#if __cplusplus < kStrokia_Foundation_Configuration_cplusplus_20
    template <class T>
    struct remove_cvref {
        typedef std::remove_cv_t<std::remove_reference_t<T>> type;
    };
    template <class T>
    using remove_cvref_t = typename remove_cvref<T>::type;
#else
    template <class T>
    using remove_cvref = std::remove_cvref<T>;
    template <class T>
    using remove_cvref_t = std::remove_cvref_t<T>;
#endif

    /**
     * Return true iff FUNCTOR is of the form  function<bool(T)> or convertible to that.
     */
    template <typename FUNCTOR_ARG, typename FUNCTOR>
    constexpr bool IsTPredicate ()
    {
        using T = remove_cvref_t<FUNCTOR_ARG>;
        if constexpr (is_invocable_v<FUNCTOR, T>) {
            return std::is_convertible_v<std::invoke_result_t<FUNCTOR, T>, bool>;
        }
        return false;
    }

    /// DEPRECATED CALLS
    STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (neq, (x != x)); // DEPRECATED - use has_neq_v
#if __cpp_impl_three_way_comparison >= 201907
    STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (spaceship, (x <=> x)); // DEPRECATED - use has_spaceship_v
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Concepts.inl"

#endif /*_Stroika_Foundation_Configuration_Concept_h_ */
