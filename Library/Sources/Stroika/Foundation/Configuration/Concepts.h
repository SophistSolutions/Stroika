/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Concept_h_
#define _Stroika_Foundation_Configuration_Concept_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <chrono>
#include <concepts>
#include <functional> // needed for std::equal_to
#include <iterator>   // needed for std::begin/std::end calls
#include <optional>

#include "Stroika/Foundation/Configuration/Common.h"

#include "ConceptsBase.h"

/*
 *  \file Miscellaneous type traits and concepts for metaprogramming
 * 
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::Configuration {

    /**
     *  \brief Extract the number of arguments, return type, and each individual argument type from a lambda or simple function object.
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
     * 
     *  \note this doesn't work for function objects that have templated operator() - such as String::EqualsComparer, since there is no type to extract.
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

    /**
     *  \brief like std::invocable concept, except also requires the invocation doesn't raise exceptions
     */
    template <typename F, typename... Args>
    concept INoThrowInvocable = invocable<F, Args...> and requires (F f, Args... args) {
        {
            noexcept (f (args...))
        };
    };

    // From https://stackoverflow.com/questions/74383254/concept-that-models-only-the-stdchrono-duration-types
    template <typename T>
    concept IDuration = requires { []<class Rep, class Period> (type_identity<chrono::duration<Rep, Period>>) {}(type_identity<T> ()); };

    // From https://stackoverflow.com/questions/74383254/concept-that-models-only-the-stdchrono-duration-types
    template <typename T>
    concept ITimePoint =
        requires { []<class CLOCK, class DURATION> (type_identity<chrono::time_point<CLOCK, DURATION>>) {}(type_identity<T> ()); };

    /**
     *  \brief concept - trivial shorthand for variadic same_as A or same_as B, or ...
     * 
     *  \par Example Usage
     *      \code
     *          template <typename T>
     *          concept IBasicUNICODECodePoint = same_as<remove_cv_t<T>, char8_t> or same_as<remove_cv_t<T>, char16_t> or same_as<remove_cv_t<T>, char32_t>;
     * 
     *          template <typename T>
     *          concept IBasicUNICODECodePoint = Configuration::IAnyOf<remove_cv_t<T>, char8_t, char16_t, char32_t>;
     *      \endcode
     */
    template <typename T, typename... U>
    concept IAnyOf = (same_as<T, U> or ...);

    /**
     *  A template which ignores its template arguments, and always returns true_type;
     *  NOT crazy - helpful is template metaprogramming.
     */
    template <typename...>
    using True = true_type;

    /**
     */
    template <typename OT>
    concept IOptional = same_as<remove_cvref_t<OT>, std::optional<typename OT::value_type>>;
    static_assert (IOptional<std::optional<int>>);
    static_assert (not IOptional<int>);

    namespace Private_ {
#if qCompilerAndStdLib_template_concept_matcher_requires_Buggy
        template <typename T1, typename T2 = void>
        struct is_pair_ : std::false_type {};
        template <typename T1, typename T2>
        struct is_pair_<pair<T1, T2>> : std::true_type {};
#endif
    }

    template <typename T>
    concept IPair =
#if qCompilerAndStdLib_template_concept_matcher_requires_Buggy
        Private_::is_pair_<T>::value
#else

        requires (T t) {
            {
                []<typename T1, typename T2> (pair<T1, T2>) {}(t)
            };
        }
#endif
        ;

    namespace Private_ {
        template <typename T, std::size_t N>
        concept has_tuple_element = requires (T t) {
            typename std::tuple_element_t<N, std::remove_const_t<T>>;
            {
                get<N> (t)
            } -> std::convertible_to<const std::tuple_element_t<N, T>&>;
        };
    }

    /**
     *  \brief Concept ITuple<T> check if T is a tuple.
     * 
     *  based on https://stackoverflow.com/questions/68443804/c20-concept-to-check-tuple-like-types
     */
    template <typename T>
    concept ITuple = !std::is_reference_v<T> && requires (T t) {
        typename std::tuple_size<T>::type;
        requires std::derived_from<std::tuple_size<T>, std::integral_constant<std::size_t, std::tuple_size_v<T>>>;
    } && []<std::size_t... N> (std::index_sequence<N...>) {
        return (Private_::has_tuple_element<T, N> && ...);
    }(std::make_index_sequence<std::tuple_size_v<T>> ());

    /**
     * Concepts let you construct a 'template' of one arg from one with two args, but class, and variable templates don't allow
     * this; but this magic trick of double indirection does allow it. And cannot use concepts as template arguments to another template
     * sadly, so need this trick...
     */
    template <typename T>
    struct ConvertibleTo {
        template <typename POTENTIALLY_ADDABLE_T>
        using Test = is_convertible<POTENTIALLY_ADDABLE_T, T>;
    };

    /**
     *  \brief Concept checks if the given type T has a const size() method which can be called to return a size_t.
     * 
     *  \par Example Usage
     *      \code
     *          if constexpr (IHasSizeMethod<T>) {
     *              T a{};
     *              return a.size ();
     *          }
     *      \endcode
     */
    template <typename T>
    concept IHasSizeMethod = requires (const T& t) {
        {
            t.size ()
        } -> std::convertible_to<size_t>;
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
        template <typename T>
        constexpr bool HasUsableEqualToOptimization ()
        {
            // static_assert (Configuration::IOperatorEq<remove_cvref_t<T>> and ! equality_comparable<T>);
            // static_assert (not Configuration::IOperatorEq<T> and equality_comparable<T>);
            //   static_assert (Configuration::IOperatorEq<remove_cvref_t<T>> == equality_comparable<T>);
            // @todo figure out why Private_::HasEq_v_ needed and cannot use equality_comparable
            if constexpr (Private_::HasEq_v_<T>) {
                struct EqualToEmptyTester_ : equal_to<T> {
                    int a;
                };
                // leverage empty base class optimization to see if equal_to contains any real data
                return sizeof (EqualToEmptyTester_) == sizeof (int);
            }
            return false;
        }
    }

    /**
     *  Check if equal_to<T> is both well defined, and contains no data. The reason it matters that it contains no data, is because
     *  then one instance is as good as another, and it need not be passed anywhere, opening an optimization opportunity.
     */
    template <typename T>
    concept IEqualToOptimizable = Private_::HasUsableEqualToOptimization<T> ();

    /**
     *  \brief Concept checks if the given type T has a value_type (type) member
     * 
     *  \par Example Usage
     *      \code
     *          if constexpr (IHasValueType<T>) {
     *              typename T::value_type x;
     *          }
     *      \endcode
     * 
     *  \note this replaces Stroika v2.1 constexpr inline bool has_value_type_v template variable
     */
    template <typename T>
    concept IHasValueType = requires (T t) { typename T::value_type; };

    namespace Private_ {
        template <typename T, typename = void>
        struct ExtractValueType {
            using type = void;
        };
        template <IHasValueType T>
        struct ExtractValueType<T> {
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

    DISABLE_COMPILER_MSC_WARNING_START (4996);
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    template <typename T>
    [[deprecated ("Since Stroika v3.0d1, use IEqualToOptimizable concept")]] constexpr bool HasUsableEqualToOptimization ()
    {
        return IEqualToOptimizable<T>;
    }

    template <typename T>
    [[deprecated ("Since Stroika v3.0d1, use require expression")]] constexpr inline bool has_plus_v = requires (T t) {
        {
            t + t
        };
    };
    namespace Private_ {
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
    template <typename From, typename To>
    [[deprecated ("Since Stroika v3.0d1 - not sure any point - probalyuse convertible or constructible concepts")]] constexpr inline bool is_explicitly_convertible_v =
        Private_::is_explicitly_convertible<From, To>::value;

    template <typename T>
    [[deprecated ("Since Stroika v3.0d1, use IHasSizeMethod")]] constexpr inline bool has_size_v = IHasSizeMethod<T>;

    namespace Private_ {
        template <typename T>
        using has_beginend_t = decltype (static_cast<bool> (begin (declval<T&> ()) != end (declval<T&> ())));
    }
    template <typename T>
    [[deprecated ("Since Stroika v3.0d1, use std::ranges::range (probably - roughly same)")]] constexpr inline bool has_beginend_v =
        is_detected_v<Private_::has_beginend_t, T>;

    namespace Private_ {
        template <typename T>
        using has_minus_t = decltype (std::declval<T> () - std::declval<T> ());
    }
    template <typename T>
    [[deprecated ("Since Stroika v3.0d1, use something else, either requires statment, or random_access_iterator for "
                  "example")]] constexpr inline bool has_minus_v = is_detected_v<Private_::has_minus_t, T>;
    template <typename T, typename U>
    [[deprecated ("Since Stroika v3.0d1, use something else, either requires statment, or random_access_iterator for "
                  "example")]] constexpr inline bool has_minus_v<std::pair<T, U>> = has_minus_v<T> and has_minus_v<U>;
    template <typename... Ts>
    [[deprecated ("Since Stroika v3.0d1, use something else, either requires statment, or random_access_iterator for "
                  "example")]] constexpr inline bool has_minus_v<std::tuple<Ts...>> = (has_minus_v<Ts> and ...);
    namespace Private_ {
        template <typename T>
        using has_neq_t = decltype (static_cast<bool> (std::declval<T> () != std::declval<T> ()));
    }
    template <typename T>
    [[deprecated ("Since Stroika v3.0d1, use equality_comparable (cuz in C++20 basically same) concept")]] constexpr inline bool has_neq_v =
        is_detected_v<Private_::has_neq_t, T>;
    template <typename T, typename U>
    [[deprecated ("Since Stroika v3.0d1, use equality_comparable (cuz in C++20 basically same) concept")]] constexpr inline bool has_neq_v<std::pair<T, U>> =
        has_neq_v<T> and has_neq_v<U>;
    template <typename... Ts>
    [[deprecated ("Since Stroika v3.0d1, use equality_comparable (cuz in C++20 basically same) concept")]] constexpr inline bool has_neq_v<std::tuple<Ts...>> =
        (has_neq_v<Ts> and ...);

    template <typename T>
    [[deprecated ("Since Stroika v3.0d1, use equality_comparable concept")]] constexpr inline bool has_eq_v = equality_comparable<T>;

    template <typename T>
    [[deprecated ("Since Stroika v3.0d1, use totally_ordered concept")]] constexpr inline bool has_lt_v = totally_ordered<T>;

    template <typename T>
    [[deprecated ("Since Stroika v3.0d1, use IHasValueType concept")]] constexpr inline bool has_value_type_v = IHasValueType<T>;
    template <typename T>
    [[deprecated ("Since Stroika v3.0d1, use https://en.cppreference.com/w/cpp/concepts/equality_comparable")]] constexpr bool EqualityComparable ()
    {
        return equality_comparable<T>;
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

    namespace Private_ {

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
    }
    template <typename ITERABLE_OF_T, typename T>
    [[deprecated ("Since Stroika v3.0d1 use Traversal::IIterableOf concept")]] constexpr bool IsIterableOfT_v =
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
    [[deprecated ("Since Stroika v3.0d1, use three_way_comparable")]] constexpr inline bool has_spaceship_v = three_way_comparable<T>;
    DISABLE_COMPILER_MSC_WARNING_END (4996);
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Concepts.inl"

#endif /*_Stroika_Foundation_Configuration_Concept_h_ */
