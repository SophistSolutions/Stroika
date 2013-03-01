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


            /*
             ********************************************************************************
             ********************************** Math::Overlaps ******************************
             ********************************************************************************
             */
            template    <typename T>
            bool    Overlaps (const pair<T, T>& p1, const pair<T, T>& p2)
            {
                Require (p1.first <= p1.second);
                Require (p2.first <= p2.second);

                bool    doesntOverlap   =   (p2.second <= p1.first) or (p2.first >= p1.second);
                return not doesntOverlap;
#if 0
                if ((p1.first <= p2.second) and (p2.first <= p1.second)) {
                    // Maybe overlap - handle nuanced cases of zero-sized overlaps
                    size_t  overlapSize;
                    if (p1.second < p2.second) {
                        Assert (p1.second >= p2.first);  // else we wouldn't be in the overlapping case
                        overlapSize = min (p1.second - p1.first, p2.second - p2.first);
                    }
                    else {
                        Assert (p2.second >= p1.first);
                        overlapSize = min (p2.second - p1.first, p2.second - p2.first);
                    }
                    Assert (overlapSize <= static_cast<size_t> (p1.second - p1.first));
                    Assert (overlapSize <= static_cast<size_t> (p2.second - p2.first));

                    if (overlapSize == 0) {
                        /*
                         *  The ONLY case where we want to allow for a zero-overlap to imply a legit overlap is when the marker itself
                         *  is zero-sized (cuz otherwise - it would never get found).
                         */
                        return
                            ((p2.second == p2.first) and (p1.second != p1.first)) or
                            ((p1.second == p1.first) and (p2.second != p2.first))
                            ;
                    }
                    else {
                        return true;        // easy/clear case - big overlap!
                    }
                }
                else {
                    return false;
                }
#endif
            }
            template    <typename T>
            inline  bool    Overlaps (T p1Start, T p1End, T p2Start, T p2End)
            {
                return Overlaps (pair<T, T> (p1Start, p1Start), pair<T, T> (p2Start, p2End));
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Math_Overlap_inl_*/



