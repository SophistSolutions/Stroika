/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_TemplateUtilities_h_
#define _Stroika_Foundation_Common_TemplateUtilities_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <type_traits>
#include <variant>

#include "Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
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
     */
    template <template <typename...> class Cont, typename... Ts>
    struct LazyType {
        using type = Cont<Ts...>;
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
     *          static_assert (is_same_v<DifferenceType<int>, int>);
     *          static_assert (is_same_v<DifferenceType<double>, double>);
     *      \endcode
     */
    template <typename T>
    using DifferenceType = typename conditional_t<is_enum_v<T>, LazyType<underlying_type_t, T>, LazyType<Private_::BaseDifferenceType_, T>>::type;

    /**
     *  Given a type, if there is an unsigned variant of it, convert to that. Works for any type T (and returns T)
     * 
     *  \par Example Usage
     *      \code
     *          static_assert (is_same_v<UnsignedOfIf<int>, unsigned int>);
     *          static_assert (is_same_v<UnsignedOfIf<string>, string>);
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
     *                  return Streams::TextReader::New (NewReadStream<byte> ());
     *              case VariantIndex_<String>:
     *                  return Streams::TextReader::New (get<String> (fSourceData_));
     *              default:
     *                  AssertNotReached ();
     *                  return {};
     *          }
     *      \endcode
     */
    //template <typename VARIANT_VALUE, typename T> // CANNOT figure out how to declare here and define in INL file...
    //constexpr size_t VariantIndex;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TemplateUtilities.inl"

#endif /*_Stroika_Foundation_Common_TemplateUtilities_h_*/
