/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_Overlap_h_
#define _Stroika_Foundation_Math_Overlap_h_   1

#include    "../StroikaPreComp.h"



/**
 *  \file
 *      @todo   Add Math regression test file - and put regression test for this in.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Math {


            /**
             *  \brief  Check if two intervals overlap, or if one contains the other
             *
             *  This overlap algorithm computes if two intervals on a common range share any points,
             *  or if one contains the other. This is almost like computing the intersection between two
             *  intervals, and returning true iff its non-empty.
             *
             *  The one case one might plausibly consider overlap which this routine does NOT is the
             *  case where two regions touch only at the edges (lhs of one == rhs of the other).
             *
             *  In my experience - this is NOT an interesting case (at least in the case of Led,
             *  a word-processor, that wasn't an interesting case, and it appears to be equally applicable here.
             *
             *      @todo   VERIFY THIS FUNCTION SYMETRIC??? MUST CHANGE EDGE RULE TO MAKE IT SYMETRIC.
             *
             *      @see    TextStore::Overlap - from where this function is derived
             */
            template    <typename T>
            bool    OverlapOrStrictlyContains (T p1Start, T p1End, T p2Start, T p2End);


        }
    }
}
#endif  /*_Stroika_Foundation_Math_Overlap_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Overlap.inl"
