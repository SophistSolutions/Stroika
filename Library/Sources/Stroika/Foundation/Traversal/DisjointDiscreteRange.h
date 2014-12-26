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



            template    <typename RANGE_TYPE>
            class   DisjointDiscreteRange : public DisjointRange<RANGE_TYPE> {
            public:
                nonvirtual  void    Add (ElementType elt);

            public:
                nonvirtual  Memory::Optional<ElementType> GetNext (ElementType s) const;

            public:
                nonvirtual  Memory::Optional<ElementType> GetPrevious (ElementType s) const;
            };
#if 0
public:
            nonvirtual  void    Add (ElementType elt);

#endif
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
