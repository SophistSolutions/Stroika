/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_Overlap_h_
#define _Stroika_Foundation_Math_Overlap_h_ 1

#include "../StroikaPreComp.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Math {

/**
    * DEPRECATED - use Range<T>::Intersects() - with appropriately specified Range openness.
    * see https://stroika.atlassian.net/browse/STK-756
     */
#if 0
    template <typename T>
    constexpr bool Overlaps (const pair<T, T>& i1, const pair<T, T>& i2);
    template <typename T>
    constexpr bool Overlaps (T i1Start, T i1End, T i2Start, T i2End);
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Overlap.inl"

#endif /*_Stroika_Foundation_Math_Overlap_h_*/
