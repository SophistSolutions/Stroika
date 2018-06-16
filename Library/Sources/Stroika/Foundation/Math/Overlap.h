/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_Overlap_h_
#define _Stroika_Foundation_Math_Overlap_h_ 1

#include "../StroikaPreComp.h"

/**
 *  \file
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Math {

            /**
             *  \brief  Check if two intervals overlap (including empty overlap if one interval empty)
             *
             *  This overlap algorithm computes if two intervals on a common range share any points,
             *  or if one contains the other. Here we consider one interval contained if it has no
             *  points outside the others range, and it intersects (even at edges).
             *
             *  This is almost like computing the intersection between two intervals,
             *  and returning true iff its non-empty.
             *
             *  The one case one might plausibly consider overlap which this routine does NOT is the
             *  case where two regions touch only at the edges (lhs of one == rhs of the other).
             *
             *  In my experience - this is NOT an interesting case (at least in the case of Led,
             *  a word-processor, that wasn't an interesting case, and it appears to be equally
             *  applicable here).
             *
             *      @see    TextStore::Overlap - from where this function is derived
             */
            template <typename T>
            bool Overlaps (const pair<T, T>& i1, const pair<T, T>& i2);
            template <typename T>
            bool Overlaps (T i1Start, T i1End, T i2Start, T i2End);
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Overlap.inl"

#endif /*_Stroika_Foundation_Math_Overlap_h_*/
