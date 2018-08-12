
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Containers_STL_Compare_inl_
#define _Stroika_Foundation_Containers_STL_Compare_inl_ 1

#include "../../Debug/Assertions.h"

namespace Stroika::Foundation::Containers::STL {

    /*
     ********************************************************************************
     ******************************** STL::Equals ***********************************
     ********************************************************************************
     */
    template <typename STROIKA_EQUALS_COMPARER, typename ITERATOR_OF_T>
    bool Equals (ITERATOR_OF_T lhsStart, ITERATOR_OF_T lhsEnd, ITERATOR_OF_T rhsStart, ITERATOR_OF_T rhsEnd)
    {
#if 0
        //  @todo - COULD check type-traits and punt out early.... if random-access-iterator!
        // Check length so we dont need to check both iterators for end/done
        if ((lhsEnd - lhsStart) != (rhsEnd - rhsStart)) {
            return false;
        }
#endif
        auto li = lhsStart;
        auto ri = rhsStart;
        while ((li != lhsEnd) and STROIKA_EQUALS_COMPARER{}(*li, *ri)) {
            ++li;
            ++ri;
        }
        return li == lhsEnd;
    }
    template <typename STROIKA_EQUALS_COMPARER, typename CONTAINER_OF_T>
    inline bool Equals (const CONTAINER_OF_T& lhs, const CONTAINER_OF_T& rhs)
    {
        //return Equals<STROIKA_EQUALS_COMPARER, typename CONTAINER_OF_T::const_iterator> (lhs.begin (), lhs.end (), rhs.begin (), rhs.end ());
        return Equals<STROIKA_EQUALS_COMPARER> (lhs.begin (), lhs.end (), rhs.begin (), rhs.end ());
    }

}

#endif /*_Stroika_Foundation_Containers_STL_Compare_inl_*/
