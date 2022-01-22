/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_TemplateUtilities_h_
#define _Stroika_Foundation_Common_TemplateUtilities_h_ 1

#include "../StroikaPreComp.h"

#include <type_traits>

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
    template <class T>
    T& Immortalize ();

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
     *  Occasionally helpful to use with LazyType, to push a type T into a ::type field (as if it came from conditional)
     * 
     *  \par Example Usage
     *      \code
     *          static_assert (is_same_v<Identity<double>::type, double>);
     *      \endcode
     */
    template <typename T>
    using Identity = conditional<true, T, T>;

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
     */
    template <typename T>
    using UnsignedOfIf = typename conditional_t<is_integral_v<T>, Common::LazyType<make_unsigned_t, T>, Identity<T>>::type;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TemplateUtilities.inl"

#endif /*_Stroika_Foundation_Common_TemplateUtilities_h_*/
