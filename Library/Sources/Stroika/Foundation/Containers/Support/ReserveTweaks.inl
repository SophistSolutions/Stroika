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
    constexpr inline size_t GetScaledUpCapacity (size_t targetSize, [[maybe_unused]] size_t eltSizeInBytes, size_t minChunk)
    {
        /*
         *      Rounding up works well at small scales - total memory
         *  waste is small (bounded). It is simple, and it helps speed up
         *  loops like while condition { append (); } considerably.
         *
         *      Scaling up (multiplicatively) has the advantage that for large n, we get
         *  log(n) reallocs (rather than n/IncSize in the roundup case).
         *  This is much better long-term large-size performance.
         *  The only trouble with this approach is that in order to keep
         *  memory waste small, we must scale by a small number (here 20%)
         *  and so we need array sizes > 100 before we start seeing any real
         *  benefit at all. Such cases do happen, but we want to be able to
         *  optimize the much more common, small array cases too.
         *
         *      So the compromise is to use a roundup-like strategy for
         *  small n, and a scaling approach as n gets larger.
         */
        size_t capacity{targetSize};
        // if small, grow quicky
        if (capacity <= 2 * minChunk) {
            capacity *= 4;
        }
        else if (capacity <= 5 * minChunk) {
            capacity *= 2;
        }
        else {
            // if already large, then  Grow by 20%
            capacity = (capacity * 6) / 5;
        }
        // then adjust up to minimum chunk size
        capacity = Math::RoundUpTo (capacity, minChunk);
        Ensure (capacity >= targetSize);
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
