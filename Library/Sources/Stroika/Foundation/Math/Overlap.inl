/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_Overlap_inl_
#define _Stroika_Foundation_Math_Overlap_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Math {


            template    <typename T>
            inline  bool    OverlapOrStrictlyContains (T p1Start, T p1End, T p2Start, T p2End)
            {
                Require (p1Start <= p1End);
                Require (p2Start <= p2End);
                if ((p1Start <= p2End) and (p2Start <= p1End)) {
                    // Maybe overlap - handle nuanced cases of zero-sized overlaps
                    size_t  overlapSize;
                    if (p1End < p2End) {
                        Assert (p1End >= p2Start);  // else we wouldn't be in the overlapping case
                        overlapSize = min (p1End - p1Start, p2End - p2Start);
                    }
                    else {
                        Assert (p2End >= p1Start);
                        overlapSize = min (p2End - p1Start, p2End - p2Start);
                    }
                    Assert (overlapSize <= (p1End - p1Start));
                    Assert (overlapSize <= (p2End - p2Start));

                    if (overlapSize == 0) {
                        /*
                         *  The ONLY case where we want to allow for a zero-overlap to imply a legit overlap is when the marker itself
                         *  is zero-sized (cuz otherwise - it would never get found).
                         */
                        return
                            ((p2End == p2Start) and (p1End != p1Start)) or
                            ((p1End == p1Start) and (p2End != p2Start))
                            ;
                    }
                    else {
                        return true;        // easy/clear case - big overlap!
                    }
                }
                else {
                    return false;
                }
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Math_Overlap_inl_*/



