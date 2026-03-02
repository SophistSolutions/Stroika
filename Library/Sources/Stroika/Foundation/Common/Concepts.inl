/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::Common {

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
        { t + t };
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
        has_beginend_v<ITERABLE> and not same_as<ExtractValueType_t<ITERABLE>, void>;

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
        using IsIterableOfT_t = integral_constant<bool, not same_as<typename IsIterableOfT_Impl2_<ITERABLE_OF_T, T>::type, substitution_failure>>;
    }
    template <typename ITERABLE_OF_T, typename T>
    [[deprecated ("Since Stroika v3.0d1 use Traversal::IIterableOfTo concept")]] constexpr bool IsIterableOfT_v =
        Private_::IsIterableOfT_t<ITERABLE_OF_T, T>::value;

    namespace Private_ {
        template <typename T, typename = void>
        struct is_iterator {
            static constexpr bool value = false;
        };
        template <typename T>
        struct is_iterator<T, enable_if_t<!same_as<typename iterator_traits<T>::value_type, void>>> {
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
