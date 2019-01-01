/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_TypeHints_h_
#define _Stroika_Foundation_Configuration_TypeHints_h_ 1

#include <type_traits>

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 */

namespace Stroika::Foundation::Configuration {

    /**
     *  \brief  This is an alias for 'T' - but how we want to pass it on stack as formal parameter.
     *
     *  This is NOT intended to be used very widely, but can be used to decide systematically (by type)
     *  if you should pass a given argument by value or by const reference.
     *
     *  \note   Use sizeof(T) <= 2*sizeof(void*) - because passing by reference involves copying one pointer and then if you
     *          access once, thats a second copy. So may as well copy 2 directly (very loosy goosy, as depends on
     *          releative cost of main memory access versus stack).
     */
    template <typename T, typename CHECK_T = T>
    using ArgByValueType = conditional_t<(sizeof (CHECK_T) <= 2 * sizeof (void*)) and is_trivially_copyable_v<CHECK_T>, CHECK_T, const CHECK_T&>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Configuration_TypeHints_h_*/
