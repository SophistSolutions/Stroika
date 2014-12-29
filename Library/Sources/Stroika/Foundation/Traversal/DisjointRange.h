/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DisjointRange_h_
#define _Stroika_Foundation_Traversal_DisjointRange_h_  1

#include    "../StroikaPreComp.h"

#include    "../Characters/String.h"
#include    "../Configuration/Common.h"
#include    "../Containers/Sequence.h"
#include    "../Memory/Optional.h"

#include    "Range.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 *  TODO:
 *      @todo   Consider if we should subclass from Iterable<Range> or return one?
 *
 *      @todo   better document threadsafety stuff and use AssertExternallyLocked locker crap to assert/assure only used at same time
 *              from multiple threads.
 *
 *      @todo   Proplery handle edge conditions for open/closed stuff - adding open/closed intervals and doing unions properly.
 *              I think all the bugs in MergeIn_();
 *
 *      @todo   Add overload of Contains() taking RANGETYPE, and ensure it does more than test Contains of the endpoints
 *              (contains all points in range).
 *
 *              THEN use that in Ensure() at end of DisjointRange<T, RANGE_TYPE>::MergeIn_();
 *
 *      @todo   Add Contains() overload taking Range (and maybe disjoint range)?
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /**
             *
             *  \note   NOT internally threadsafe. To use from multiple threads, use Synchonized<DisjointRange>, or otherwise protect.
             */
            template    <typename T, typename RANGE_TYPE = Range<T>>
            class   DisjointRange {
            public:
                /**
                 */
                using   RangeType     =   RANGE_TYPE;

            public:
                /**
                 */
                using   ElementType     =   typename RangeType::ElementType;

            public:
                /**
                 *  You can pass in empty Ranges, ranges out of order, and overlaping ranges, and the constructor
                 *  always filters out empty ranges, and re-order so subranges well-ordered and disjoint.
                 */
                DisjointRange () = default;
                DisjointRange (const DisjointRange&) = default;
                DisjointRange (const RangeType& from);
                DisjointRange (const initializer_list<RangeType>& from);
                template    <typename CONTAINER_OF_RANGE_OF_T>
                explicit DisjointRange (const CONTAINER_OF_RANGE_OF_T& from);
                template    <typename COPY_FROM_ITERATOR_OF_RANGE_OF_T>
                explicit DisjointRange (COPY_FROM_ITERATOR_OF_RANGE_OF_T start, COPY_FROM_ITERATOR_OF_RANGE_OF_T end);

            public:
                nonvirtual  DisjointRange& operator= (const DisjointRange& rhs) = default;

            public:
                /**
                 */
                static  DisjointRange   FullRange ();

            public:
                /**
                 *  A disjoint range is made up of a fininte number of disjoint (non-overlapping) subranges, which are arranged
                 *  following the natural ordering intrinsic to the ElementType. This returns those subranges.
                 */
                nonvirtual  Containers::Sequence<RangeType>    SubRanges () const;

            public:
                /**
                 */
                nonvirtual   bool    empty () const;

            public:
                /**
                 *  @see Range::Contains.
                 *
                 *  This returns true if r is contained in some sub-element range. This is only the smae as
                 *  GetBounds().Contains() if SubRanges ().size () <= 1.
                 */
                nonvirtual  bool    Contains (ElementType elt) const;

            public:
                /**
                 * if empty, returns empty range()
                 */
                nonvirtual  RangeType   GetBounds () const;

            public:
                /**
                 *  This returns true if the constituent subranges are all equal. This amounts to checking
                 *  if the 'covered points' are all equal.
                 */
                template    <typename T2, typename RANGE_TYPE2>
                nonvirtual  bool    Equals (const DisjointRange<T2, RANGE_TYPE2>& rhs) const;

            public:
                /**
                 */
                nonvirtual  bool    Intersects (const RangeType& rhs) const;
                nonvirtual  bool    Intersects (const DisjointRange& rhs) const;

            public:
                /**
                 */
                nonvirtual  DisjointRange    Intersection (const RangeType& rhs) const;
                nonvirtual  DisjointRange    Intersection (const DisjointRange& rhs) const;

            public:
                /**
                ***NYI
                 */
                nonvirtual  DisjointRange   Union (const DisjointRange& rhs) const;

            public:
                /**
                ***NYI
                 */
                nonvirtual  RangeType    UnionBounds (const DisjointRange& rhs) const;


            private:
                nonvirtual  void    MergeIn_ (const RangeType& r);

            private:
                nonvirtual  void    AssertInternalRepValid_ ();

            private:
                Containers::Sequence<RangeType> fSubRanges_;
            };


            /**
             */
            template    <typename T, typename RANGE_TYPE>
            DisjointRange<T, RANGE_TYPE>   operator+ (const DisjointRange<T, RANGE_TYPE>& lhs, const DisjointRange<T, RANGE_TYPE>& rhs);


            /**
             */
            template    <typename T, typename RANGE_TYPE>
            DisjointRange<T, RANGE_TYPE>   operator^ (const DisjointRange<T, RANGE_TYPE>& lhs, const DisjointRange<T, RANGE_TYPE>& rhs);


            /**
             */
            template    <typename T, typename RANGE_TYPE>
            bool    operator== (const DisjointRange<T, RANGE_TYPE>& lhs, const DisjointRange<T, RANGE_TYPE>& rhs);


            /**
             */
            template    <typename T, typename RANGE_TYPE>
            bool    operator!= (const DisjointRange<T, RANGE_TYPE>& lhs, const DisjointRange<T, RANGE_TYPE>& rhs);


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "DisjointRange.inl"

#endif  /*_Stroika_Foundation_Traversal_DisjointRange_h_ */
