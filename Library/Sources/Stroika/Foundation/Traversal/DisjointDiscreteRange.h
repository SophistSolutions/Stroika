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
 *      @todo   More efficient DisjointDiscreteRange<T, RANGE_TYPE>::Elements () implementation
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /**
             *  @todo Add CONCEPT to require RANGE_TYPE subtype of DiscreteRange
             */
            template    <typename T, typename RANGE_TYPE = DiscreteRange<T>>
            class   DisjointDiscreteRange : public DisjointRange<T, RANGE_TYPE> {
            private:
                using inherited = DisjointRange<T, RANGE_TYPE>;
                using THIS_CLASS_ = DisjointDiscreteRange<T, RANGE_TYPE>;

            public:
                using ElementType = typename DisjointRange<T, RANGE_TYPE>::ElementType;

            public:
                using RangeType = typename DisjointRange<T, RANGE_TYPE>::RangeType;

            public:
                /**
                 *  You can pass in empty Ranges, and ranges out of order, but the constructor always filters out
                 *  empty ranges, and re-orders so subranges well-ordered.
                 ***    @todo make it so!
                 */
                DisjointDiscreteRange () = default;
                DisjointDiscreteRange (const DisjointDiscreteRange&) = default;
                DisjointDiscreteRange (const RangeType& from);
                DisjointDiscreteRange (const initializer_list<RangeType>& from);
                template <typename CONTAINER_OF_DISCRETERANGE_OF_T>
                explicit DisjointDiscreteRange (const CONTAINER_OF_DISCRETERANGE_OF_T& from);
                template <typename COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T>
                explicit DisjointDiscreteRange (COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T start, COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T end);

            public:
                nonvirtual  DisjointDiscreteRange& operator= (const DisjointDiscreteRange& rhs) = default;

            public:
                /**
                 */
                nonvirtual  void    Add (ElementType elt);

            public:
                /**
                 */
                nonvirtual  DisjointDiscreteRange       Intersection (const RangeType& rhs) const;
                nonvirtual  DisjointDiscreteRange       Intersection (const DisjointDiscreteRange& rhs) const;

            public:
                /**
                 */
                nonvirtual  Memory::Optional<ElementType> GetNext (ElementType elt) const;

            public:
                /**
                 */
                nonvirtual  Memory::Optional<ElementType> GetPrevious (ElementType elt) const;

            public:
                /**
                 *  Use Example:
                 *      DisjointDiscreteRange<DiscreteRange<int>> t;
                 *      for (T i : t.Elements ()) {
                 *      }
                 *
                 *  Elements () makes no guarantess about whether or not modifications to the underlying DisjointDiscreteRange<> will
                 *  appear in the Elements() Iterable<T>.

                 ///NYI
                 */
                nonvirtual  Iterable<ElementType>   Elements () const;
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
