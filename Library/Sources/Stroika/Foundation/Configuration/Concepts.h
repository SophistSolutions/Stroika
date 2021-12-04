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

    namespace Private_ {
        template <typename T>
        using has_eq_t = decltype (static_cast<bool> (std::declval<T> () == std::declval<T> ()));
        template <typename T>
        using has_neq_t = decltype (static_cast<bool> (std::declval<T> () != std::declval<T> ()));
        template <typename T>
        using has_lt_t = decltype (static_cast<bool> (std::declval<T> () < std::declval<T> ()));
        template <typename T>
        using has_minus_t = decltype (std::declval<T> () - std::declval<T> ());
        template <typename T>
        using has_plus_t = decltype (std::declval<T> () + std::declval<T> ());
#if __cpp_impl_three_way_comparison >= 201907
        template <typename T>
        using has_spaceship_t = decltype (std::declval<T> () <=> std::declval<T> ());
#endif
        // Subtle - but begin () doesn't work with rvalues, so must use declval<T&> -- LGP 2021-11-26
        template <typename T>
        using has_beginend_t = decltype (static_cast<bool> (begin (declval<T&> ()) != end (declval<T&> ())));

        // Would be nice to simplify, but my current version of is_detected_v only takes one template parameter, and the std::experimental version not included in VS2k19
        template <typename ITERABLE_OF_T, typename T>
        struct IsIterableOfT_Impl2_ {
            template <typename X, typename USE_ITERABLE = ITERABLE_OF_T, bool ITER_RESULT_CONVERTIBLE_TO_T = is_convertible_v<decltype (*begin (declval<USE_ITERABLE&> ())), T>>
            static auto check (const X& x) -> conditional_t<
                is_detected_v<has_beginend_t, ITERABLE_OF_T> and
                    ITER_RESULT_CONVERTIBLE_TO_T,
                substitution_succeeded<T>,
                substitution_failure>;
            static substitution_failure check (...);
            using type = decltype (check (declval<T> ()));
        };
        template <typename ITERABLE_OF_T, typename T>
        using IsIterableOfT_t = integral_constant<bool, not is_same<typename IsIterableOfT_Impl2_<ITERABLE_OF_T, T>::type, substitution_failure>::value>;

        // Would be nice to simplify, but my current version of is_detected_v only takes one template parameter, and the std::experimental version not included in VS2k19
        template <typename ITERABLE_OF_T, template <typename> typename CHECKER_PREDICATE>
        struct IsIterableOfPredicateOfT_Impl2_ {
            template <typename X, typename USE_ITERABLE = ITERABLE_OF_T, bool ITER_RESULT_CONVERTIBLE_TO_T = CHECKER_PREDICATE<decltype (*begin (declval<USE_ITERABLE&> ()))>::value>
            static auto check (const X& x) -> conditional_t<
                is_detected_v<has_beginend_t, ITERABLE_OF_T> and
                    ITER_RESULT_CONVERTIBLE_TO_T,
                substitution_succeeded<int>,
                substitution_failure>;
            static substitution_failure check (...);
            using type = decltype (check (declval<int> ()));
        };
        // STILL NOT WORKING -- but trying...
        template <typename ITERABLE_OF_T, template <typename> typename CHECKER_PREDICATE>
        using IsIterableOfPredicateOfT_t = integral_constant<bool, not is_same<typename IsIterableOfPredicateOfT_Impl2_<ITERABLE_OF_T, CHECKER_PREDICATE>::type, substitution_failure>::value>;

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
        template <typename T>
        using is_callable = conditional_t<is_class_v<T>, is_callable_impl_<T>, false_type>;

    }

    // handy to have remove_cvref/remove_cvref_t definition around, even if using pre-c++20
    // Put in our namespace, and when we switch to C++20, we can deprecate our local namespace copy
    // NOTE: can also use std::decay_t which does this and more in c++17
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
     *  \brief Extract the number of arguments, return type, and each individual argument type from a lambda or function object.
     *
     *  CREDITS:
     *      From https://stackoverflow.com/questions/7943525/is-it-possible-to-figure-out-the-parameter-type-and-return-type-of-a-lambda
     *           https://stackoverflow.com/users/224671/kennytm
     * 
     *  \par Example Usage
     *      \code
     *          auto lambda = [](int i) { return long(i*10); };
     *
     *          typedef function_traits<decltype(lambda)> traits;
     *
     *          static_assert(std::is_same<long, traits::result_type>::value, "err");
     *          static_assert(std::is_same<int, traits::arg<0>::type>::value, "err");
     *      \endcode
     */
    template <typename T>
    struct function_traits : public function_traits<decltype (&T::operator())> {
    };
    // For generic types, directly use the result of the signature of its 'operator()'
    template <typename ClassType, typename ReturnType, typename... Args>
    struct function_traits<ReturnType (ClassType::*) (Args...) const>
    // we specialize for pointers to member function
    {
        /**
         *  \brief Number of arguments
         */
        static inline constexpr size_t kArity = sizeof...(Args);

        /**
         */
        typedef ReturnType result_type;

        /**
         *  type of the ith 'arg'
         */
        template <size_t i>
        struct arg {
            typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
            // the i-th argument is equivalent to the i-th tuple element of a tuple
            // composed of those arguments.
        };
    };

    /**
     *  \brief check if the given type T can be compared with operator==, and result is convertible to bool
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
    constexpr inline bool has_eq_v = is_detected_v<Private_::has_eq_t, T>;
    template <typename T, typename U>
    constexpr inline bool has_eq_v<std::pair<T, U>> = has_eq_v<T>and has_eq_v<U>;
    template <typename... Ts>
    constexpr inline bool has_eq_v<std::tuple<Ts...>> = (has_eq_v<Ts> and ...);

    /**
     *  \brief check if the given type T can be compared with operator!=, and result is convertible to bool
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
    constexpr inline bool has_neq_v = is_detected_v<Private_::has_neq_t, T>;
    template <typename T, typename U>
    constexpr inline bool has_neq_v<std::pair<T, U>> = has_neq_v<T>and has_neq_v<U>;
    template <typename... Ts>
    constexpr inline bool has_neq_v<std::tuple<Ts...>> = (has_neq_v<Ts> and ...);

    /**
     *  \brief check if the given type T can be compared with operator<, and result is convertible to bool
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
    constexpr inline bool has_lt_v = is_detected_v<Private_::has_lt_t, T>;
    template <typename T, typename U>
    constexpr inline bool has_lt_v<std::pair<T, U>> = has_lt_v<T>and has_lt_v<U>;
    template <typename... Ts>
    constexpr inline bool has_lt_v<std::tuple<Ts...>> = (has_lt_v<Ts> and ...);

    /**
     *  \brief check if the given type T can be combined with a second T using operator-
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
    constexpr inline bool has_minus_v = is_detected_v<Private_::has_minus_t, T>;
    template <typename T, typename U>
    constexpr inline bool has_minus_v<std::pair<T, U>> = has_minus_v<T>and has_minus_v<U>;
    template <typename... Ts>
    constexpr inline bool has_minus_v<std::tuple<Ts...>> = (has_minus_v<Ts> and ...);

    /**
     *  \brief check if the given type T can be combined with a second T using operator+
     * 
     *  \par Example Usage
     *      \code
     *          if constexpr (has_plus_v<T>) {
     *              T a{};
     *              T b{};
     *              return a + b;
     *          }
     *      \endcode
     * 
     *  \note see https://stroika.atlassian.net/browse/STK-749 - for why pair/tuple specializations - not sure why STL doesn't do this directly in pair<> template
     */
    template <typename T>
    constexpr inline bool has_plus_v = is_detected_v<Private_::has_plus_t, T>;
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
    constexpr inline bool has_spaceship_v = is_detected_v<Private_::has_spaceship_t, T>;
    template <typename T, typename U>
    constexpr inline bool has_spaceship_v<std::pair<T, U>> = has_spaceship_v<T>and has_spaceship_v<U>;
    template <typename... Ts>
    constexpr inline bool has_spaceship_v<std::tuple<Ts...>> = (has_spaceship_v<Ts> and ...);
#endif

    /**
     *  \brief check if the given type T can have std::begin()/std::end () applied, and they can be compared and that compare converted to bool
     * 
     *  \par Example Usage
     *      \code
     *          if constexpr (has_beginend_v<T>) {
     *              T a{};
     *              return begin (a) != end (a);
     *          }
     *      \endcode
     * 
     *  \note see https://stroika.atlassian.net/browse/STK-749 - for why pair/tuple specializations - not sure why STL doesn't do this directly in pair<> template
     */
    template <typename T>
    constexpr inline bool has_beginend_v = is_detected_v<Private_::has_beginend_t, T>;

    /**
     *  Check if ITERABLE has begin/end methods, and the begin/end stuff looks like they return an iterator.
     *  This does NOT check for subclassing Traversal::Iterable<> (so works with an array, or stl vector etc).
     */
    template <typename ITERABLE>
    constexpr bool IsIterable_v = has_beginend_v<ITERABLE>;

    /**
     *  Check if has begin/end methods (not for subclassing Traversal::Iterable<>), and if result of *begin () is convertible to T.
     */
    template <typename ITERABLE_OF_T, typename T>
    constexpr bool IsIterableOfT_v = Private_::IsIterableOfT_t<ITERABLE_OF_T, T>::value;

    /**
     *  Check if has begin/end methods (not for subclassing Traversal::Iterable<>), and if result of *begin () is convertible to T.
     */
    template <typename ITERABLE_OF_T, template <typename> typename CHECKER_PREDICATE>
    constexpr bool IsIterableOfPredicateOfT_v = Private_::IsIterableOfPredicateOfT_t<ITERABLE_OF_T, CHECKER_PREDICATE>::value;

    /**
     *  See http://en.cppreference.com/w/cpp/concept/EqualityComparable
     */
    template <typename T>
    constexpr bool EqualityComparable ()
    {
        return has_eq_v<T>;
    }

    /**
     *  See http://en.cppreference.com/w/cpp/concept/LessThanComparable
     */
    template <typename T>
    constexpr bool LessThanComparable ()
    {
        return has_lt_v<T>;
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

    /**
     *  \brief 
     * 
     *  \par Example Usage
     *      \code
     *      \endcode
     * 
     *  \note see https://stroika.atlassian.net/browse/STK-749 - for why pair/tuple specializations - not sure why STL doesn't do this directly in pair<> template
     */
    template <typename From, typename To>
    constexpr inline bool is_explicitly_convertible_v = Private_::is_explicitly_convertible<From, To>::value;

    /**
     *  Check T is an interator, but checking if it has iterator_traits...
     */
    template <typename T>
    constexpr bool is_iterator_v = Private_::is_iterator<T>::value;

    /**
     */
    template <typename T>
    constexpr bool is_callable_v = Private_::is_callable<T>::value;

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

    /// DEPRECATED CALLS ////////////////////

    // move to bottom when I've removed dependencies
    STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (beginend, (std::begin (x) != std::end (x))); // DEPRECATED -
    STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (begin, std::begin (x));                      // DEPRECATED -
    STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (eq, (x == x));                               // DEPRECATED - use has_eq_v
    STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (minus, (x - x));                             // DEPRECATED - use has_minus_v
    STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (lt, (x < x));                                // DEPRECATED - use has_lt_v

    template <typename T>
    [[deprecated ("Since Stroika 2.1b14 - not sure why had something so specific")]] constexpr bool HasMinusWithIntegerResult ()
    {
        return has_minus_v<T> && is_convertible_v<minus_result<T>, int>;
    }

    STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (neq, (x != x)); // DEPRECATED - use has_neq_v
#if __cpp_impl_three_way_comparison >= 201907
    STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (spaceship, (x <=> x)); // DEPRECATED - use has_spaceship_v
#endif
    template <typename ITERABLE>
    using IsIterable [[deprecated ("Since Stroika 2.1b14 dont use (probaly use IsIterable_v)")]] = integral_constant<bool, has_beginend_v<ITERABLE>>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Concepts.inl"

#endif /*_Stroika_Foundation_Configuration_Concept_h_ */
