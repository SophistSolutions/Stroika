/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Common_h_
#define _Stroika_Foundation_Containers_Common_h_ 1

#include "../StroikaPreComp.h"

#include <cstddef>

#include "../Configuration/Enumeration.h"

// while we support deprecated APIS
#include "Support/ReserveTweaks.h"

namespace Stroika::Foundation::Containers {

// https://stroika.atlassian.net/browse/STK-535
// APPEARS FIXED AS OF 2021-11-10 - Stroika 2.1b14
#ifndef qIterationOnCopiedContainer_ThreadSafety_Buggy
#define qIterationOnCopiedContainer_ThreadSafety_Buggy 0
//#define qIterationOnCopiedContainer_ThreadSafety_Buggy 1
#endif

    /**
     *  \brief For a contiguous container (such as a vector or basic_string) - find the pointer to the start of the container
     *
     *  For a contiguous container (such as a vector or basic_string) - find the pointer to the start of the container
     *
     *  Note: this is like std::begin(), except that it returns a pointer, not an iterator, and returns nullptr if the
     *  container is empty.
     */
    template <typename CONTAINER>
    typename CONTAINER::value_type* Start (CONTAINER& c);
    template <typename CONTAINER>
    const typename CONTAINER::value_type* Start (const CONTAINER& c);

    /**
     *  \brief For a contiguous container (such as a vector or basic_string) - find the pointer to the end of the container
     *
     *  For a contiguous container (such as a vector or basic_string) - find the pointer to the end of the container
     *
     *  Note: this is like std::end(), except that it returns a pointer, not an iterator, and returns nullptr if the
     *  container is empty.
     */
    template <typename CONTAINER>
    typename CONTAINER::value_type* End (CONTAINER& c);
    template <typename CONTAINER>
    const typename CONTAINER::value_type* End (const CONTAINER& c);

    template <typename CONTAINER>
    [[deprecated ("Since Stroika 2.1b14 use Support::ReserveTweaks::Reserve4Add1")]] inline void ReserveSpeedTweekAdd1 (CONTAINER& c, size_t minChunk = 16)
    {
        Support::ReserveTweaks::Reserve4Add1 (c, minChunk);
    }
    template <typename CONTAINER>
    [[deprecated ("Since Stroika 2.1b14 use Support::ReserveTweaks::Reserve4AddN")]] inline void ReserveSpeedTweekAddN (CONTAINER& c, size_t n, size_t minChunk = 16)
    {
        Support::ReserveTweaks::Reserve4AddN (c, n, minChunk);
    }
    [[deprecated ("Since Stroika 2.1b14 use Support::ReserveTweaks::GetScaledUpCapacity")]] inline size_t ReserveSpeedTweekAdjustCapacity (size_t targetCapacity, size_t minChunk = 16)
    {
        return Support::ReserveTweaks::GetScaledUpCapacity (targetCapacity, 1, minChunk);
    }
    template <typename CONTAINER>
    [[deprecated ("Since Stroika 2.1b14 use Support::ReserveTweaks::GetScaledUpCapacity4AddN")]] inline size_t ReserveSpeedTweekAddNCapacity (const CONTAINER& c, size_t n, size_t minChunk = 16)
    {
        return Support::ReserveTweaks::GetScaledUpCapacity4AddN (c, n, minChunk).value_or (size_t (-1));
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Common.inl"

#endif /*_Stroika_Foundation_Containers_Common_h_*/
