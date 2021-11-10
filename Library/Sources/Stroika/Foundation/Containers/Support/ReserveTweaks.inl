/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Support_ReserveTweaks_inl_
#define _Stroika_Foundation_Containers_Support_ReserveTweaks_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include <algorithm>

#include "../../Math/Common.h"

namespace Stroika::Foundation::Containers::Support::ReserveTweaks {

    /*
     ********************************************************************************
     ********************************* GetScaledUpCapacity **************************
     ********************************************************************************
     */
    constexpr inline size_t GetScaledUpCapacity (size_t targetSize, [[maybe_unused]] size_t eltSize, size_t minChunk)
    {
        size_t capacity{targetSize};
        if (capacity <= 2 * minChunk) {
            capacity *= 4;
        }
        else if (capacity <= 5 * minChunk) {
            capacity *= 2;
        }
        else {
            capacity *= 6;
            capacity /= 5;
        }
        capacity = Math::RoundUpTo (capacity, minChunk);
        return capacity;
    }

    /*
     ********************************************************************************
     ******************************* GetScaledUpCapacity4AddN ***********************
     ********************************************************************************
     */
    template <typename CONTAINER>
    constexpr inline optional<size_t> GetScaledUpCapacity4AddN (const CONTAINER& c, size_t addN, size_t minChunk)
    {
        size_t targetSize{c.size () + addN};
        size_t capacity{c.capacity ()};
        if (targetSize >= capacity) {
            return GetScaledUpCapacity (targetSize, sizeof (typename CONTAINER::value_type), minChunk);
        }
        return nullopt;
    }

    /*
     ********************************************************************************
     ********************************** Reserve4AddN ********************************
     ********************************************************************************
     */
    template <typename CONTAINER>
    inline void Reserve4AddN (CONTAINER& c, size_t n, size_t minChunk)
    {
        if (auto newCapacity = GetScaledUpCapacity4AddN (c, n, minChunk)) {
            c.reserve (*newCapacity);
        }
    }

    /*
     ********************************************************************************
     ******************************* Reserve4Add1 ***********************************
     ********************************************************************************
     */
    template <typename CONTAINER>
    inline void Reserve4Add1 (CONTAINER& c, size_t minChunk)
    {
        Reserve4AddN (c, 1, minChunk);
    }

}

#endif /*_Stroika_Foundation_Containers_Support_ReserveTweaks_inl_*/
