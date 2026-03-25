/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_TemplateUtilities_h_
#define _Stroika_Foundation_Common_TemplateUtilities_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <type_traits>
#include <variant>

#include "Stroika/Foundation/Common/Common.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Common {

    /**
     *  Create a singleton of a class initialized once, but whose DTOR is never called.
     *
     *  Based on template <class _Ty> _Ty& _Immortalize() from VS2k19 runtime library....
     */
    template <typename T, typename... ARGS>
    T& Immortalize (ARGS... args);

    /**
     *  Utility to wrap a class that will not evaluate (illegal) for use with conditional_t (or other similar)
     *  where it wont actually get used, just so it will compile.
     * 
     *      Thanks to https://stackoverflow.com/users/65863/remy-lebeau for idea
     *      See https://stackoverflow.com/questions/65257926/problem-with-false-case-in-conditional-t-being-compiled-and-evaluated
     * 
     * \note this supports type arguments, but not size_t arguments, which would sometimes be useful too, but cannot see how to mix and match with variadic templates.
     */
    template <template <typename...> class Cont, typename... Ts>
    struct LazyType {
        using type = Cont<Ts...>;
    };

    /**
     * @see LazyType
     */
    template <template <typename...> class Cont, typename... Ts>
    using LazyType_t = typename LazyType<Cont, Ts...>::type;

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
        template <size_t i>
        struct ArgOrVoid_ {
            using type = typename conditional_t<(i < sizeof...(ARGS)), tuple_element<i, tuple<ARGS...>>, Private_::void_type>::type;
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
         *  \brief like 'arg_t' - except that if index > max legal, instead of failing to compile, will return void. Helpful
         *         sometimes in contexts where c++ templates run more code than you might want.
         */
        template <size_t I>
        using ArgOrVoid_t = typename ArgOrVoid_<I>::type;
    };

    /**
     *  \brief function object whose action is to map its argument, back to the same value it started with (identity function).
     * 
     *  \see also https://stackoverflow.com/questions/41767240/what-is-stdidentity-and-how-it-is-used 
     */
    struct Identity {
        using is_transparent = void;

        template <typename T>
        constexpr T&& operator() (T&& t) const noexcept
        {
            return std::forward<T> (t);
        }
    };

    namespace Private_ {
        template <typename T>
        using BaseDifferenceType_ = decltype (T{} - T{});
    }

    /**
     *  Computes the difference between two types, plus for enums, returns the difference between the underlying types.
     * 
     *  \par Example Usage
     *      \code
     *          static_assert (same_as<DifferenceType<int>, int>);
     *          static_assert (same_as<DifferenceType<double>, double>);
     *      \endcode
     */
    template <typename T>
    using DifferenceType = typename conditional_t<is_enum_v<T>, LazyType<underlying_type_t, T>, LazyType<Private_::BaseDifferenceType_, T>>::type;

    /**
     *  Given a type, if there is an unsigned variant of it, convert to that, else value is T. Works for any type T.
     * 
     *  \par Example Usage
     *      \code
     *          static_assert (same_as<UnsignedOfIf<int>, unsigned int>);
     *          static_assert (same_as<UnsignedOfIf<string>, string>);
     *      \endcode
     * 
     *  \note conditional<true, T, T> is a trick similar to LazyType
     */
    template <typename T>
    using UnsignedOfIf = typename conditional_t<is_integral_v<T>, LazyType<make_unsigned_t, T>, conditional<true, T, T>>::type;

    /**
     *  Utility to map from an std::variant<...> and map a TYPE to its underlying index in the given variant
     * 
     *      \note Considered doing this as a function, so could take optional argument and use to figure out
     *            type of VARIANT_VALUE, but frequently used in constexpr setting where this probably would
     *            be helpful (see below example).
     * 
     *  \par Example Usage
     *      \code
     *          variant<filesystem::path, BLOB, String> fSourceData_;
     *          template <typename T>
     *          static constexpr size_t VariantIndex_ = VariantIndex<decltype(fSourceData_), T>;
     *          ...
     *          switch (fSourceData_.index ()) {
     *              case VariantIndex_<filesystem::path>:
     *              case VariantIndex_<BLOB>:
     *                  return Streams::BinaryToText::Reader::New (NewReadStream<byte> ());
     *              case VariantIndex_<String>:
     *                  return Streams::BinaryToText::Reader::New (get<String> (fSourceData_));
     *              default:
     *                  AssertNotReached ();
     *                  return {};
     *          }
     *      \endcode
     */
    //template <typename VARIANT_VALUE, typename T> // CANNOT figure out how to declare here and define in INL file...
    //constexpr size_t VariantIndex;

    namespace Private_ {
        // from https://stackoverflow.com/questions/66254907/parameterize-of-tuple-with-repeated-type
        template <typename T, typename Seq>
        struct expander;
        template <typename T, size_t... Is>
        struct expander<T, index_sequence<Is...>> {
            template <typename E, size_t>
            using elem = E;
            using type = tuple<elem<T, Is>...>;
        };
        template <size_t N, class Type>
        struct my_tuple {
            using type = typename expander<Type, make_index_sequence<N>>::type;
        };
    }

    /**
     *  \brief same_as<RepeatedTuple_t<3,int>,tuple<int,int,int>>
     */
    template <size_t N, class Type>
    using RepeatedTuple_t = typename Private_::my_tuple<N, Type>::type;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TemplateUtilities.inl"

#endif /*_Stroika_Foundation_Common_TemplateUtilities_h_*/
