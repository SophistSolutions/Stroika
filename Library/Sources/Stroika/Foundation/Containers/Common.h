/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Common_h_
#define _Stroika_Foundation_Containers_Common_h_ 1

#include "../StroikaPreComp.h"

#include <cstddef>

#include "../Configuration/Enumeration.h"

namespace Stroika::Foundation::Containers {

// https://stroika.atlassian.net/browse/STK-535
#ifndef qIterationOnCopiedContainer_ThreadSafety_Buggy
#define qIterationOnCopiedContainer_ThreadSafety_Buggy 1
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

    /**
     */
    template <typename CONTAINER>
    void ReserveSpeedTweekAdd1 (CONTAINER& c, size_t minChunk = 16);

    /**
     */
    template <typename CONTAINER>
    void ReserveSpeedTweekAddN (CONTAINER& c, size_t n, size_t minChunk = 16);

    /**
     */
    size_t ReserveSpeedTweekAdjustCapacity (size_t targetCapacity, size_t minChunk = 16);

    /**
     */
    template <typename CONTAINER>
    size_t ReserveSpeedTweekAddNCapacity (const CONTAINER& c, size_t n, size_t minChunk = 16);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Common.inl"

#endif /*_Stroika_Foundation_Containers_Common_h_*/
