/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Partition_h_
#define _Stroika_Foundation_Traversal_Partition_h_ 1

#include "../StroikaPreComp.h"

#include "../Math/Common.h"
#include "Iterable.h"

/**
 *  \file
 *
 *  STATUS:     PRELIMINARY DRAFT.
 *
 *  TODO:
 *      @todo   Implementation NOT correct. Cases reported as partition true are true,
 *              but case of DiscreteRange<int> {1, 2}, DiscreteRange<int> {3, 4}) not handled right - where
 *              elts arent equal, but both sides closed and next elt is next point.
 *              (retest/revisit as of Stroika 2.1b8 --- maybe fixed)
 *
 *      @todo   Should be optional param to produce the Range that the elements form a partion of.
 *
 *      @todo   Consider if/how this integrates with std::partition
 *
 *      @todo   Consider if/how this intergrates with DisjointRange
 *
 *      @todo   Support ANY STL container (like initializer_list) as argument. So just anytthing you can say begin/end on to get
 *              iterator.
 *
 */

namespace Stroika::Foundation::Traversal {

    /**
     *  Checks if the given collection of ranges 'cover' an interval, with no overlap (sharing no points).
     *
     *  \note Half open intervals are generally best candidates for creating partitions
     * 
     *  \pre the Range elements RANGE_TYPE - must be natively sortable (note we do not require that they be given in order)
     *
     *  \par Example Usage
     *      \code
     *          using RT = Range<double>;
     *          VerifyTestResult (not IsPartition (Sequence<RT>{RT{1, 2}, RT{3, 4}}));
     *          VerifyTestResult (IsPartition (Sequence<RT>{RT{1, 2}, RT{2, 4}}));
     *      \endcode
     *
     *  \par Example Usage (using integers)
     *      \code
     *          using RT = Range<int, Explicit<int, ExplicitOpenness<Openness::eClosed, Openness::eOpen>>>; // half open intervals best for partitions
     *          VerifyTestResult (not IsPartition (Sequence<RT>{RT{1, 2}, RT{3, 4}}));
     *          VerifyTestResult (IsPartition (Sequence<RT>{RT{1, 2}, RT{2, 4}}));
     *      \endcode
     */
    template <typename RANGETYPE>
    bool IsPartition (const Iterable<RANGETYPE>& iterable);
    template <typename RANGETYPE, typename RANGE_ELT_COMPARER>
    bool IsPartition (const Iterable<RANGETYPE>& iterable, RANGE_ELT_COMPARER comparer);

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Partition.inl"

#endif /*_Stroika_Foundation_Traversal_Partition_h_ */
