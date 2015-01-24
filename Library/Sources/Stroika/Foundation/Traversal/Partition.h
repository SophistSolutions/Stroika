/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Partition_h_
#define _Stroika_Foundation_Traversal_Partition_h_  1

#include    "../StroikaPreComp.h"

#include    "Iterable.h"



/**
 *  \file
 *
 *  STATUS:     PRELIMINARY DRAFT.
 *
 *  TODO:
 *      @todo   Implementation NOT correct. Cases reported as partition true are true,
 *              but case of DiscreteRange<int> {1, 2}, DiscreteRange<int> {3, 4}) not handled right - where
 *              elts arent equal, but both sides closed and next elt is next point.
 *
 *      @todo   Consider if/how this integrates with std::partition
 *
 *      @todo   Consider if/how this intergrates with DisjointRange
 *
 *      @todo   Support ANY STL container (like initializer_list) as argument. So just anytthing you can say begin/end on to get
 *              iterator.
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /**
             *  Checks if the given collection of ranges 'cover' an interval, with no overlap (sharing no points).
             */
            template    <typename RANGETYPE>
            bool    IsPartition (const Iterable<RANGETYPE>& iterable);



        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "Partition.inl"

#endif  /*_Stroika_Foundation_Traversal_Partition_h_ */
