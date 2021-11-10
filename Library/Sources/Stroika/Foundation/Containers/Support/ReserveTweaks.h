/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Support_ReserveTweaks_h_
#define _Stroika_Foundation_Containers_Support_ReserveTweaks_h_ 1

#include "../../StroikaPreComp.h"

#include <cstddef>
#include <optional>

namespace Stroika::Foundation::Containers::Support::ReserveTweaks {

    using namespace std;

    constexpr size_t kDefaultMinChunkSize = 16;

    /**
     *  \brief Comupute the best 'capacity' to use for the given targetSize of the container.
     *
     *  Adjust the given target (required) capacity to grow in geometric rate (so ln N reallocs), and use the argument chunkSize to
     *  make sure the number of entries is a multiple of that chunk size.
     */
    constexpr size_t GetScaledUpCapacity (size_t targetSize, size_t eltSize, size_t minChunk = kDefaultMinChunkSize);

    /**
     *  \brief Comupute the best 'capacity' to use for the given container adding N elements to its given size .
     *
     * @see GetScaledUpCapacity
     */
    template <typename CONTAINER>
    constexpr optional<size_t> GetScaledUpCapacity4AddN (const CONTAINER& c, size_t addN, size_t minChunk = kDefaultMinChunkSize);

    /**
     *  \brief use @see GetScaledUpCapacity () to automatically optimally adjust the capacity (reserve) on the given container before adding N elements to it.
     *
     * For the given container (which supports capacity/reserve/size APIs) - figure how much to adjust the capacity
     * for the given container (using @see ScaleUpCapacity) and call reseve() to perform that adjustment.
     */
    template <typename CONTAINER>
    void Reserve4AddN (CONTAINER& c, size_t n, size_t minChunk = kDefaultMinChunkSize);

    /**
     *  \brief use @see GetScaledUpCapacity () to automatically optimally adjust the capacity (reserve) on the given container before adding one element to it.
     *
     * For the given container (which supports capacity/reserve/size APIs) - figure how much to adjust the capacity
     * for the given container (using @see ScaleUpCapacity) and call reseve() to perform that adjustment.
     */
    template <typename CONTAINER>
    void Reserve4Add1 (CONTAINER& c, size_t minChunk = kDefaultMinChunkSize);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ReserveTweaks.inl"

#endif /*_Stroika_Foundation_Containers_Support_ReserveTweaks_h_*/
