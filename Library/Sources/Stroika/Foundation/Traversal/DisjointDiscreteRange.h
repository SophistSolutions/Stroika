/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DisjointDiscreteRange_h_
#define _Stroika_Foundation_Traversal_DisjointDiscreteRange_h_  1

#include    "../StroikaPreComp.h"

#include    "../Characters/String.h"
#include    "../Configuration/Common.h"
#include    "../Containers/Sequence.h"
#include    "../Memory/Optional.h"

#include    "DiscreteRange.h"
#include    "DisjointRange.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 *  TODO:
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /**
             */
            template    <typename RANGE_TYPE>
            class   DisjointDiscreteRange : public DisjointRange<RANGE_TYPE> {
            public:
                using ElementType = typename DisjointRange<RANGE_TYPE>::ElementType;

            public:
                using RangeType = typename DisjointRange<RANGE_TYPE>::RangeType;

            public:
                /**
                 */
                nonvirtual  void    Add (ElementType elt);

            public:
                /**
                 */
                nonvirtual  Memory::Optional<ElementType> GetNext (ElementType elt) const;

            public:
                /**
                 */
                nonvirtual  Memory::Optional<ElementType> GetPrevious (ElementType elt) const;
            };


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "DisjointDiscreteRange.inl"

#endif  /*_Stroika_Foundation_Traversal_DisjointDiscreteRange_h_ */
