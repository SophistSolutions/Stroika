/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_Overlap_inl_
#define _Stroika_Foundation_Math_Overlap_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Debug/Assertions.h"

namespace Stroika::Foundation::Math {

    /*
     ********************************************************************************
     ********************************** Math::Overlaps ******************************
     ********************************************************************************
     */
    template <typename T>
    constexpr bool Overlaps (const pair<T, T>& i1, const pair<T, T>& i2)
    {
        Require (i1.first <= i1.second);
        Require (i2.first <= i2.second);

        /*
         *                         |                i1               |
         *  | A |                | B |             | C |           | D |           | E |
         *  |                                        F                                 |
         *
         *  The algorithm is completely symetric. Pick one point as i1, and consider possible ranges for
         *  interval i2, and name them A, B, C, D, E, or F.
         *
         *  The above captures the basic cases. i1 is a given range, and we consdier the possible
         *  placements of A-F relative to i1.
         *
         *  The only ones obviously visually outside of the range p1 are A and E.
         *
         *  We CHOOSE to consider the case of empty insersections to be Overlap() - iff i2 (A-F) is empty.
         */
        // CASE A
        if (i2.second < i1.first) {
            return false;
        }
        // variant of case "A" where they touch at the edges but not the empty "A" case
        if (i2.second <= i1.first and (i1.first != i1.second and i2.first != i2.second)) {
            return false;
        }
        // CASE E
        if (i2.first > i1.second) {
            return false;
        }
        // variant of case "E" where they touch at the edges but not the empty "A" case
        if (i2.first >= i1.second and (i1.first != i1.second and i2.first != i2.second)) {
            return false;
        }
        return true;
    }
    template <typename T>
    constexpr bool Overlaps (T p1Start, T p1End, T p2Start, T p2End)
    {
        return Overlaps (pair<T, T> (p1Start, p1End), pair<T, T> (p2Start, p2End));
    }

}

#endif /*_Stroika_Foundation_Math_Overlap_inl_*/
