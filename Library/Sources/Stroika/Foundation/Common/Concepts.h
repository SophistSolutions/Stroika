/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_Concept_h_
#define _Stroika_Foundation_Common_Concept_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <chrono>
#include <concepts>
#include <functional> // needed for std::equal_to
#include <iterator>   // needed for std::begin/std::end calls
#include <memory>
#include <optional>
#include <type_traits>
#include <variant>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/ConceptsBase.h"
#include "Stroika/Foundation/Common/StdCompat.h"

/*
 *  \file
 *      Miscellaneous type traits and concepts for metaprogramming
 * 
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 * 
 * 
 USEFUL EXAMPLE:
         template <IStdPathLike2UNICODEString TOSTRINGABLE>
        explicit String (TOSTRINGABLE&& s)
            requires (
                         not IBasicUNICODEStdString<remove_cvref_t<TOSTRINGABLE>> and
                         not requires (TOSTRINGABLE t) {
                             {
                                 []<IUNICODECanUnambiguouslyConvertFrom T1> (const T1*) {}(t)
                             };
                         } and
                         not requires (TOSTRINGABLE t) {
                             {
                                 []<IUNICODECanUnambiguouslyConvertFrom T1> (const span<const T1>&) {}(t)
                             };
                         } and
                         not requires (TOSTRINGABLE t) {
                             {
                                 []<IStdBasicStringCompatibleCharacter T1> (const basic_string_view<T1>&) {}(t)
                             };
                         })
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : String{mkSTR_ (forward<TOSTRINGABLE> (s))} {}
#else
        ;
#endif

 */

namespace Stroika::Foundation::Common {

    namespace Private_ {
        struct void_type {
            using type = void;
        };
    }

    /**
     * \brief ClassNotFinal
     */
    template <typename T>
    concept ClassNotFinal = not is_final_v<T>;

    /**
     *  \brief concept true if integral or floating-point type 'T'. Not sure why not provided by std c++
     *
     *  Also note - NOT marked true for arithmetic-like types, like big-num package (perhaps provide another concept for this).
     */
    template <typename T>
    concept IBuiltinArithmetic = is_arithmetic_v<T>;

    /**
     *  \brief Extract the number of arguments, return type, and each individual argument type from a lambda or simple function object.
     *
     *  \par Example Usage
     *      \code
     *          auto lambda = [](int i) { return long(i*10); };
     *
     *          using traits = FunctionTraits<decltype(lambda)>;
     *
     *          static_assert (traits::kArity == 1);
     *          static_assert (same_as<long, traits::result_type>);
     *          static_assert (same_as<int, traits::arg_t<0>>);
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
    private:
        template <size_t i>
        struct arg_ {
            using type = typename tuple_element<i, tuple<ARGS...>>::type;
        };

    public:
        /**
         *  \brief Number of arguments
         */
        static inline constexpr size_t kArity = sizeof...(ARGS);

    public:
        /**
         *  Function return type.
         */
        using result_type = RETURN_TYPE;

    public:
        /**
         * @brief Since you cannot use a parameter-pack as type directly, wrap it in a tuple, and then
         *        it can be used (e.g. as KEY in a map).
         */
        using args_tuple = tuple<ARGS...>;

    public:
        /**
         * @brief Return the ith argument type
         * 
         *  \note UNCLEAR if/how this might work if the function is overloaded...
         * 
         *  \see arg_t, ArgOrVoid_t
         * 
         * @tparam I 
         */
        template <size_t I>
        using arg_t = typename arg_<I>::type;

    public:
        /**
         *  \brief like 'arg' - except that if index > max legal, instead of failing to compile, will return void. Helpful
         *         sometimes in contexts where c++ templates run more code than you might want.
         */
        template <size_t I>
        using ArgOrVoid_t = conditional_t<(I < sizeof...(ARGS)), arg_t<I>, void>;
    };

    /**
     *  \brief like std::invocable concept, except also requires the invocation doesn't raise exceptions
     */
    template <typename F, typename... Args>
    concept INoThrowInvocable = invocable<F, Args...> and requires (F f, Args... args) {
        { noexcept (f (args...)) };
    };

    /**
     *  \par Example Usage
     *      \code
     *          static_assert (invocable_r<decltype ([] (int) { return ""; }), const char*, int>);
     *          static_assert (invocable_r<decltype ([] (char*, char*) {}), void, char*, char*>);
     *      \endcode
     * 
     *  \note used STL-style name since so closely related to invocable - which is part of the standard library.
     */
    template <typename F, typename R, typename... Args>
    concept invocable_r = invocable<F, Args...> && convertible_to<invoke_result_t<F, Args...>, R>;
    static_assert (invocable_r<decltype ([] (int) { return ""; }), const char*, int>);
    static_assert (invocable_r<decltype ([] (char*, char*) {}), void, char*, char*>);

    // From https://stackoverflow.com/questions/74383254/concept-that-models-only-the-stdchrono-duration-types
    template <typename T>
    concept IDuration =
        requires { []<typename Rep, typename Period> (type_identity<chrono::duration<Rep, Period>>) {}(type_identity<T> ()); };
    static_assert (not IDuration<float>);

    // From https://stackoverflow.com/questions/74383254/concept-that-models-only-the-stdchrono-duration-types
    template <typename T>
    concept ITimePoint =
        requires { []<typename CLOCK, typename DURATION> (type_identity<chrono::time_point<CLOCK, DURATION>>) {}(type_identity<T> ()); };
    static_assert (not ITimePoint<float>);

    /**
     *  \brief concept - trivial shorthand for variadic same_as A or same_as B, or ...
     * 
     *  \par Example Usage
     *      \code
     *          template <typename T>
     *          concept IBasicUNICODECodePoint = same_as<remove_cv_t<T>, char8_t> or same_as<remove_cv_t<T>, char16_t> or same_as<remove_cv_t<T>, char32_t>;
     * 
     *          template <typename T>
     *          concept IBasicUNICODECodePoint = Common::IAnyOf<remove_cv_t<T>, char8_t, char16_t, char32_t>;
     *      \endcode
     */
    template <typename T, typename... U>
    concept IAnyOf = (same_as<T, U> or ...);

    /**
     *  \brief concept version of std::is_trivially_copyable_v
     */
    template <typename T>
    concept trivially_copyable = is_trivially_copyable_v<T>;

    /**
     *  A template which ignores its template arguments, and always returns true_type;
     *  NOT crazy - helpful is template metaprogramming.
     */
    template <typename...>
    using True = true_type;

    using StdCompat::Boolean_testable;
    using StdCompat::explicitly_convertible_to;

    /**
     * \brief equality_comparable_with, but less strict - just checks if it can be equality compared!
     * 
     *      INSPIRATION: https://godbolt.org/z/qevGWKan4
     *      static_assert (equality_comparable_with<nullopt_t, optional<int>>); // note this fails
     */
    template <class _Ty1, class _Ty2>
    concept Weak_Equality_Comparable_With = requires (const remove_reference_t<_Ty1>& __x, const remove_reference_t<_Ty2>& __y) {
        { __x == __y } -> Boolean_testable;
        { __x != __y } -> Boolean_testable;
    };
    static_assert (not equality_comparable_with<nullopt_t, optional<int>>);
    static_assert (Weak_Equality_Comparable_With<nullopt_t, optional<int>>);

    /**
     */
    template <typename OT>
    concept IOptional = same_as<remove_cvref_t<OT>, optional<typename OT::value_type>>;
    static_assert (IOptional<optional<int>>);
    static_assert (not IOptional<int>);

    namespace Private_ {
#if qCompilerAndStdLib_template_concept_matcher_requires_Buggy
        template <typename T1, typename T2 = void>
        struct is_shared_ptr_ : false_type {};
        template <typename T1>
        struct is_shared_ptr_<shared_ptr<T1>> : true_type {};
        template <typename T1, typename T2 = void>
        struct is_pair_ : false_type {};
        template <typename T1, typename T2>
        struct is_pair_<pair<T1, T2>> : true_type {};
        template <typename... ARGS>
        struct is_variant_ : false_type {};
        template <typename... ARGS>
        struct is_variant_<variant<ARGS...>> : true_type {};
#endif
    }

    /**
     *  \brief return true iff argument type T, is std::pair<a,b> for some a/b types
     */
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

    /**
     *  \brief return true iff argument type T, is std::shared_ptr<A> for some A types
     */
    template <typename T>
    concept ISharedPtr =
#if qCompilerAndStdLib_template_concept_matcher_requires_Buggy
        Private_::is_shared_ptr_<T>::value
#else

        requires (T t) {
            {
                []<typename T1> (shared_ptr<T1>) {}(t)
            };
        }
#endif
        ;
    static_assert (ISharedPtr<shared_ptr<int>>);
    static_assert (not ISharedPtr<int>);

    namespace Private_ {
        template <typename T, size_t N>
        concept has_tuple_element = requires (T t) {
            typename tuple_element_t<N, remove_const_t<T>>;
            { get<N> (t) } -> convertible_to<const tuple_element_t<N, T>&>;
        };
    }

    /**
     *  \brief Concept ITuple<T> check if T is a tuple.
     * 
     *  based on https://stackoverflow.com/questions/68443804/c20-concept-to-check-tuple-like-types
     */
    template <typename T>
    concept ITuple = !is_reference_v<T> && requires (T t) {
        typename tuple_size<T>::type;
        requires derived_from<tuple_size<T>, integral_constant<size_t, tuple_size_v<T>>>;
    } && []<size_t... N> (index_sequence<N...>) { return (Private_::has_tuple_element<T, N> && ...); }(make_index_sequence<tuple_size_v<T>> ());

    /**
     *  \brief - detect if T is a std::variant<> type.
     */
    template <typename T>
    concept IVariant =
#if qCompilerAndStdLib_template_concept_matcher_requires_Buggy
        Private_::is_variant_<T>::value
#else
        requires (T t) {
            {
                []<typename... TYPES> (variant<TYPES...>) {}(t)
            };
        }
#endif
        ;
    static_assert (not IVariant<int>);
    static_assert (IVariant<variant<int>>);

    /**
     * Concepts let you construct a 'template' of one arg from one with two args, but class, and variable templates don't allow
     * this; but this magic trick of double indirection does allow it. And cannot use concepts as template arguments to another template
     * sadly, so need this trick...
     * 
     *  The 'test' here just invokes convertible_to<TEST_ARGUMENT, T>
     */
    template <typename T>
    struct ConvertibleTo {
        template <typename TEST_ARGUMENT>
        using Test = conditional_t<convertible_to<TEST_ARGUMENT, T>, true_type, false_type>;
    };

    /**
     * Concepts let you construct a 'template' of one arg from one with two args, but class, and variable templates don't allow
     * this; but this magic trick of double indirection does allow it. And cannot use concepts as template arguments to another template
     * sadly, so need this trick...
     * 
     *  The 'test' here just invokes constructible_from<TEST_ARGUMENT, T>
     */
    template <typename T>
    struct ConvertibleFrom {
        template <typename TEST_ARGUMENT>
        using Test = conditional_t<constructible_from<TEST_ARGUMENT, T>, true_type, false_type>;
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
        { t.size () } -> convertible_to<size_t>;
    };

    namespace Private_ {
        template <typename T>
        concept HasEq_ = requires (T t) {
            { t == t } -> convertible_to<bool>;
        };
        template <typename T>
        constexpr inline bool HasEq_v_ = HasEq_<T>;
        template <typename T, typename U>
        constexpr inline bool HasEq_v_<pair<T, U>> = HasEq_v_<T> and HasEq_v_<U>;
        template <typename... Ts>
        constexpr inline bool HasEq_v_<tuple<Ts...>> = (HasEq_v_<Ts> and ...);
        template <typename T>
        constexpr bool HasUsableEqualToOptimization ()
        {
            // static_assert (Common::IOperatorEq<remove_cvref_t<T>> and ! equality_comparable<T>);
            // static_assert (not Common::IOperatorEq<T> and equality_comparable<T>);
            //   static_assert (Common::IOperatorEq<remove_cvref_t<T>> == equality_comparable<T>);
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
    concept IEqualToOptimizable = equality_comparable<T> and Private_::HasUsableEqualToOptimization<T> ();

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

    /**
     * @brief check T has had remove_cvref_t called on it (e.g. ICVRefTd<const string&> is string)
     */
    template <typename T>
    concept ICVRefTd = same_as<T, remove_cvref_t<T>>;

    /**
     *  from https://stackoverflow.com/questions/32785105/implementing-a-switch-type-trait-with-stdconditional-t-chain-calls
     *  \par Example Usage
     *      \code
     *           using Type = Select_t<Case<false, void>,
     *                                 Case<false, int>,
     *                                 Case<true, std::string>>;
     *      \endcode
     */
    template <bool B, typename T>
    struct Case {
        static constexpr bool value = B;
        using type                  = T;
    };
    template <typename HEAD, typename... TAIL>
    struct Select : conditional_t<HEAD::value, HEAD, Select<TAIL...>> {};
    template <typename T>
    struct Select<T> {
        using type = T;
    };
    template <bool B, typename T>
    struct Select<Case<B, T>> {
        // last one had better be true!
        static_assert (B, "!");
        using type = T;
    };
    template <typename HEAD, typename... TAIL>
    using Select_t = typename Select<HEAD, TAIL...>::type;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Concepts.inl"

#endif /*_Stroika_Foundation_Common_Concept_h_ */
