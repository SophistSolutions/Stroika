/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Concept_h_
#define _Stroika_Foundation_Configuration_Concept_h_ 1

#include "../StroikaPreComp.h"

#include <concepts>
#include <functional> // needed for std::equal_to
#include <iterator>   // needed for std::begin/std::end calls

#include "../Configuration/Common.h"

#include "ConceptsBase.h"

/*
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Configuration {

    // @todo move these to localized just before needed/used... NOW... TODAY
    namespace Private_ {
        template <typename T>
        using has_minus_t = decltype (std::declval<T> () - std::declval<T> ());
        template <typename T>
        using has_plus_t = decltype (std::declval<T> () + std::declval<T> ());

        // Subtle - but begin () doesn't work with rvalues, so must use declval<T&> -- LGP 2021-11-26
        template <typename T>
        using has_beginend_t = decltype (static_cast<bool> (begin (declval<T&> ()) != end (declval<T&> ())));

        // Would be nice to simplify, but my current version of is_detected_v only takes one template parameter, and the std::experimental version not included in VS2k19
        template <typename ITERABLE_OF_T, typename T>
        struct IsIterableOfT_Impl2_ {
            template <typename X, typename USE_ITERABLE = ITERABLE_OF_T,
                      bool ITER_RESULT_CONVERTIBLE_TO_T = is_convertible_v<decltype (*begin (declval<USE_ITERABLE&> ())), T>>
            static auto check (const X& x)
                -> conditional_t<is_detected_v<has_beginend_t, ITERABLE_OF_T> and ITER_RESULT_CONVERTIBLE_TO_T, substitution_succeeded<T>, substitution_failure>;
            static substitution_failure check (...);
            using type = decltype (check (declval<T> ()));
        };
        template <typename ITERABLE_OF_T, typename T>
        using IsIterableOfT_t =
            integral_constant<bool, not is_same<typename IsIterableOfT_Impl2_<ITERABLE_OF_T, T>::type, substitution_failure>::value>;

        // Would be nice to simplify, but my current version of is_detected_v only takes one template parameter, and the std::experimental version not included in VS2k19
        template <typename ITERABLE_OF_T, template <typename> typename CHECKER_PREDICATE>
        struct IsIterableOfPredicateOfT_Impl2_ {
            template <typename X, typename USE_ITERABLE = ITERABLE_OF_T,
                      bool ITER_RESULT_CONVERTIBLE_TO_T = CHECKER_PREDICATE<decltype (*begin (declval<USE_ITERABLE&> ()))>::value>
            static auto check (const X& x)
                -> conditional_t<is_detected_v<has_beginend_t, ITERABLE_OF_T> and ITER_RESULT_CONVERTIBLE_TO_T, substitution_succeeded<int>, substitution_failure>;
            static substitution_failure check (...);
            using type = decltype (check (declval<int> ()));
        };
        // STILL NOT WORKING -- but trying...
        template <typename ITERABLE_OF_T, template <typename> typename CHECKER_PREDICATE>
        using IsIterableOfPredicateOfT_t =
            integral_constant<bool, not is_same<typename IsIterableOfPredicateOfT_Impl2_<ITERABLE_OF_T, CHECKER_PREDICATE>::type, substitution_failure>::value>;

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

    }

    /**
     *  \brief Extract the number of arguments, return type, and each individual argument type from a lambda or function object.
     *
     *  \par Example Usage
     *      \code
     *          auto lambda = [](int i) { return long(i*10); };
     *
     *          using traits = FunctionTraits<decltype(lambda)>;
     *
     *          static_assert(std::is_same<long, traits::result_type>::value);
     *          static_assert(std::is_same<int, traits::arg<0>::type>::value);
     *      \endcode
     * 
     *  CREDITS:
     *      From https://stackoverflow.com/questions/7943525/is-it-possible-to-figure-out-the-parameter-type-and-return-type-of-a-lambda
     *           https://stackoverflow.com/users/224671/kennytm
     * 
     * For generic types, directly use the result of the signature of its 'operator()'
     * Specialize for pointers to member function
     */
    template <typename T>
    struct FunctionTraits : public FunctionTraits<decltype (&T::operator())> {};
    template <typename CLASS_TYPE, typename RETURN_TYPE, typename... ARGS>
    struct FunctionTraits<RETURN_TYPE (CLASS_TYPE::*) (ARGS...) const> {
        /**
         *  \brief Number of arguments
         */
        static inline constexpr size_t kArity = sizeof...(ARGS);

        /**
         *  Function return type.
         */
        using result_type = RETURN_TYPE;

        /**
         *  type of the ith 'arg'
         */
        template <size_t i>
        struct arg {
            using type = typename std::tuple_element<i, std::tuple<ARGS...>>::type;
            // the i-th argument is equivalent to the i-th tuple element of a tuple
            // composed of those arguments.
        };
    };

    namespace Private_ {
        template <typename T>
        concept HasEq_ = requires (T t) {
                             {
                                 t == t
                                 } -> std::convertible_to<bool>;
                         };
        template <typename T>
        constexpr inline bool HasEq_v_ = HasEq_<T>;
        template <typename T, typename U>
        constexpr inline bool HasEq_v_<std::pair<T, U>> = HasEq_v_<T> and HasEq_v_<U>;
        template <typename... Ts>
        constexpr inline bool HasEq_v_<std::tuple<Ts...>> = (HasEq_v_<Ts> and ...);
    }

    /**
     *  \brief Check if the given type T can be compared with operator==, and result is convertible to bool
     * 
     *  \par Example Usage
     *      \code
     *          struct X {};
     *          static_assert (not IOperatorEq<X>);
     *          static_assert (IOperatorEq<int>);
     *          static_assert (not IOperatorEq<pair<X, X>>);
     *          static_assert (not IOperatorEq<tuple<X, X>>);
     *      \endcode
     * 
     *  \note see https://stackoverflow.com/questions/76510385/how-to-do-simple-c-concept-has-eq-that-works-with-stdpair-is-stdpair-op/76510752#76510752
     *        for explanation about complexities with pair/tuple
     */
    template <typename T>
    concept IOperatorEq = Private_::HasEq_v_<T>;

    namespace Private_ {
        template <typename T>
        concept HasLtBase_ = requires (T t) {
                                 {
                                     t < t
                                     } -> std::convertible_to<bool>;
                             };
        template <typename T>
        constexpr inline bool HasLt_v_ = HasLtBase_<T>;
        template <typename T, typename U>
        constexpr inline bool HasLt_v_<std::pair<T, U>> = HasLt_v_<T> and HasLt_v_<U>;
        template <typename... Ts>
        constexpr inline bool HasLt_v_<std::tuple<Ts...>> = (HasLt_v_<Ts> and ...);
    }

    /**
     *  \brief Concept checks if the given type T can be compared with operator<, and result is convertible to bool
     * 
     *  \par Example Usage
     *      \code
     *          if constexpr (IOperatorLt<T>) {
     *              T a{};
     *              T b{};
     *              return a < b;
     *          }
     *      \endcode
     * 
     *  \note see https://stackoverflow.com/questions/76510385/how-to-do-simple-c-concept-has-eq-that-works-with-stdpair-is-stdpair-op/76510752#76510752
     *        for explanation about complexities with pair/tuple
     */
    template <typename T>
    concept IOperatorLt = Private_::HasLt_v_<T>;

    /**
     *  \brief check if the given type T can be compared with operator<=>
     * 
     *  \par Example Usage
     *      \code
     *          if constexpr (IOperatorSpaceship<T>) {
     *              T a{};
     *              T b{};
     *              return a <=> b;
     *          }
     *      \endcode
     */
    template <typename T>
    concept IOperatorSpaceship = requires (T t) {
                                     {
                                         t <=> t
                                         } -> std::convertible_to<partial_ordering>;
                                 } or requires (T t) {
                                          {
                                              t <=> t
                                              } -> std::convertible_to<strong_ordering>;
                                      } or requires (T t) {
                                               {
                                                   t <=> t
                                                   } -> std::convertible_to<weak_ordering>;
                                           };

    /**
     *  \brief concept checks if the given type T has a const size() method which can be called to return a size_t.
     * 
     *  \par Example Usage
     *      \code
     *          if constexpr (IHasSize<T>) {
     *              T a{};
     *              return a.size ();
     *          }
     *      \endcode
     */
    template <typename T>
    concept IHasSize = requires (const T& t) {
                           {
                               t.size ()
                               } -> std::convertible_to<size_t>;
                       };

    /**
     *  Check if has begin/end methods (not for subclassing Traversal::Iterable<>), and if result of *begin () is convertible to T.
     */
    template <typename ITERABLE_OF_T, template <typename> typename CHECKER_PREDICATE>
    constexpr bool IsIterableOfPredicateOfT_v = Private_::IsIterableOfPredicateOfT_t<ITERABLE_OF_T, CHECKER_PREDICATE>::value;

    /**
     *  Check if equal_to<T> is both well defined, and contains no data. The reason it matters that it contains no data, is because
     *  then one instance is as good as another, and it need not be passed anywhere, opening an optimization opportunity.
     */
    template <typename T>
    constexpr bool HasUsableEqualToOptimization ()
    {
        if constexpr (IOperatorEq<T>) {
            struct EqualToEmptyTester_ : equal_to<T> {
                int a;
            };
            // leverage empty base class optimization to see if equal_to contains any real data
            return sizeof (EqualToEmptyTester_) == sizeof (int);
        }
        return false;
    }

    /**
     *  \brief check if the given type T can be compared with operator==, and result is convertible to bool
     * 
     *  \par Example Usage
     *      \code
     *          if constexpr (IHasValueType<T>) {
     *              typename T::value_type x;
     *          }
     *      \endcode
     * 
     *  Issue is that it cannot be usefully defined (as nearly as I can tell in C++17).
     * 
     *  \note this replaces Stroika v2.1 constexpr inline bool has_value_type_v template variable
     */
    template <typename T>
    concept IHasValueType = requires (T t) { typename T::value_type; };

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

    namespace Private_ {
        template <typename T, typename = void>
        struct ExtractValueType {
            using type = void;
        };
        template <typename T>
        struct ExtractValueType<T, enable_if_t<IHasValueType<T>>> {
            using type = typename T::value_type;
        };
        template <typename T>
        struct ExtractValueType<const T*, void> {
            using type = T;
        };
        template <typename T>
        struct ExtractValueType<T*, void> {
            using type = T;
        };
    }

    /**
     *  \brief Extract the type of elements in a container, or returned by an iterator (value_type) or void it there is no value_type
     * 
     *  \note when known if argument is container or iterator, use std::iter_value_t, or std::ranges::range_value_t
     * 
     * If the given T has a field value_type, return it; returns void if T has no value_type
     * 
     *  NOTE - similar to std::ranges::range_value_t or std::iter_value_t except works with other types.
     */
    template <typename T>
    using ExtractValueType_t = typename Private_::ExtractValueType<remove_cvref_t<T>>::type;

    /////////////////////////////////////////////////////////////
    ////////////////////// DEPREACTED BELOW /////////////////////
    /////////////////////////////////////////////////////////////

    template <typename T>
    [[deprecated ("Since Stroika v3.0d1, use require expression")]] constexpr inline bool has_plus_v = requires (T t) {
                                                                                                           {
                                                                                                               t + t
                                                                                                           };
                                                                                                       };

    namespace Private_ {

        // template <typename T>
        //  using has_size_t = decltype (static_cast<size_t> (declval<T&> ().size ()));

    }
    template <typename T>
    //    [[deprecated ("Since Stroika v3.0d1, use IHasSize")]] constexpr inline bool has_size_v = is_detected_v<Private_::has_size_t, T>;
    [[deprecated ("Since Stroika v3.0d1, use IHasSize")]] constexpr inline bool has_size_v = IHasSize<T>;

    template <typename T>
    [[deprecated ("Since Stroika v3.0d1, use std::ranges::range (probably - roughly same)")]] constexpr inline bool has_beginend_v =
        is_detected_v<Private_::has_beginend_t, T>;

    template <typename T>
    [[deprecated (
        "Since Stroika v3.0d1, use something else, either requires statment, or random_access_iterator for example")]] constexpr inline bool has_minus_v =
        is_detected_v<Private_::has_minus_t, T>;
    template <typename T, typename U>
    [[deprecated (
        "Since Stroika v3.0d1, use something else, either requires statment, or random_access_iterator for example")]] constexpr inline bool
        has_minus_v<std::pair<T, U>> = has_minus_v<T> and has_minus_v<U>;
    template <typename... Ts>
    [[deprecated (
        "Since Stroika v3.0d1, use something else, either requires statment, or random_access_iterator for example")]] constexpr inline bool
        has_minus_v<std::tuple<Ts...>> = (has_minus_v<Ts> and ...);

    namespace Private_ {
        // @todo AFTER WE REMOVE DEPRECATED FUNCTIONS BELOW, CAN REMOVE A BUNCH OF THESE AS WELL

        template <typename T>
        using has_neq_t = decltype (static_cast<bool> (std::declval<T> () != std::declval<T> ()));

    }

    template <typename T>
    [[deprecated ("Since Stroika v3.0d1, use IOperatorEq (cuz in C++20 basically same) concept")]] constexpr inline bool has_neq_v =
        is_detected_v<Private_::has_neq_t, T>;
    template <typename T, typename U>
    [[deprecated ("Since Stroika v3.0d1, use IOperatorEq (cuz in C++20 basically same) concept")]] constexpr inline bool has_neq_v<std::pair<T, U>> =
        has_neq_v<T> and has_neq_v<U>;
    template <typename... Ts>
    [[deprecated ("Since Stroika v3.0d1, use IOperatorEq (cuz in C++20 basically same) concept")]] constexpr inline bool has_neq_v<std::tuple<Ts...>> =
        (has_neq_v<Ts> and ...);

    template <typename T>
    [[deprecated ("Since Stroika v3.0d1, use IOperatorEq concept")]] constexpr inline bool has_eq_v = IOperatorEq<T>;

    template <typename T>
    [[deprecated ("Since Stroika v3.0d1, use IOperatorLt concept")]] constexpr inline bool has_lt_v = IOperatorLt<T>;

    template <typename T>
    [[deprecated ("Since Stroika v3.0d1, use IHasValueType concept")]] constexpr inline bool has_value_type_v = IHasValueType<T>;
    template <typename T>
    [[deprecated ("Since Stroika v3.0d1, use https://en.cppreference.com/w/cpp/concepts/equality_comparable")]] constexpr bool EqualityComparable ()
    {
        return IOperatorEq<T>;
    }

    template <typename T>
    [[deprecated ("Since Stroika v3.0d1, use https://en.cppreference.com/w/cpp/concepts/totally_ordered - NOT SAME THING AT ALL, BUT "
                  "CLOSEST IN STANDARD")]] constexpr bool
    LessThanComparable ()
    {
        return has_lt_v<T>;
    }

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
            struct Derived : T, Fallback {};

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
    template <typename T>
    [[deprecated ("Since Stroika v3.0d1, use https://en.cppreference.com/w/cpp/concepts/invocable")]] constexpr bool is_callable_v =
        Private_::is_callable<T>::value;
    template <typename ITERABLE>
    [[deprecated ("Since Stroika v3.0d1, use ranges::range")]] constexpr bool IsIterable_v =
        has_beginend_v<ITERABLE> and not is_same_v<ExtractValueType_t<ITERABLE>, void>;
    template <typename ITERABLE_OF_T, typename T>
    [[deprecated ("Since Stroika v3.0d1 use Traversal::IIterable concept")]] constexpr bool IsIterableOfT_v =
        Private_::IsIterableOfT_t<ITERABLE_OF_T, T>::value;

    namespace Private_ {
        template <typename T, typename = void>
        struct is_iterator {
            static constexpr bool value = false;
        };
        template <typename T>
        struct is_iterator<T, enable_if_t<!is_same_v<typename iterator_traits<T>::value_type, void>>> {
            static constexpr bool value = true;
        };
    }
    template <typename T>
    [[deprecated ("Since Stroika v3.0d1, use input_iterator, forward_iterator, or some other sort of std iterator concept")]] constexpr bool IsIterator_v =
        Private_::is_iterator<remove_cvref_t<T>>::value;

    template <typename FUNCTOR_ARG, typename FUNCTOR>
    [[deprecated ("Since Stroika v3.0d1, use std::predicate<F,ARG,ARG>")]] constexpr bool IsTPredicate ()
    {
        using T = remove_cvref_t<FUNCTOR_ARG>;
        if constexpr (is_invocable_v<FUNCTOR, T>) {
            return std::is_convertible_v<std::invoke_result_t<FUNCTOR, T>, bool>;
        }
        return false;
    }
    template <typename T>
    [[deprecated ("Since Stroika v3.0d1, use IOperatorSpaceship")]] constexpr inline bool has_spaceship_v = IOperatorSpaceship<T>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Concepts.inl"

#endif /*_Stroika_Foundation_Configuration_Concept_h_ */
