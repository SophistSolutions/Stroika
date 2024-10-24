/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DisjointRange_h_
#define _Stroika_Foundation_Traversal_DisjointRange_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Traversal/Range.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   better document thread-safety stuff and use AssertExternallyLocked locker crap to assert/assure only used at same time
 *              from multiple threads.
 *
 *      @todo   Properly handle edge conditions for open/closed stuff - adding open/closed intervals and doing unions properly.
 *              I think all the bugs in MergeIn_();
 *
 *      @todo   Add overload of Contains() taking RANGETYPE, and ensure it does more than test Contains of the endpoints
 *              (contains all points in range).
 *
 *              THEN use that in Ensure() at end of DisjointRange<T, RANGE_TYPE>::MergeIn_();
 *
 *              And have Contains() overload that takes DisjointRange(), as alias for Intersects(); I think?
 *              Maybe Contains ALWAYS is just alias for Intersects?
 *
 *              DOCUMENT that Contains() is ContainsOrEquals () - not proper containment.
 */

namespace Stroika::Foundation::Traversal {

    /**
     *  \brief A DisjointRange is NOT a range, but a collection of non-overlapping (except at the edges) Ranges. It its BOUNDs (GetBounds) form a Range.
     *
     *  DiscreteRange<> is an immutable type (once constructed, will never change), except for allowing operator=.
     * 
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      o   operator==, operator!= supported, but this assumes operator== is defined on 'T'
     */
    template <typename T, typename RANGE_TYPE = Range<T>>
    class DisjointRange {
    public:
        /**
         *  \brief a Disjoint range collection of (disjoint) ranges of this type.
         */
        using RangeType = RANGE_TYPE;

    public:
        /**
         *  \brief Range::value_type is the type of the contained elements of the range (say range of integers, value_type=int)
         */
        using value_type = typename RangeType::value_type;

    public:
        /**
         *  You can pass in empty Ranges, ranges out of order, and overlapping ranges, and the constructor
         *  always filters out empty ranges, and re-order so subranges well-ordered and disjoint.
         */
        DisjointRange ()                     = default;
        DisjointRange (const DisjointRange&) = default;
        DisjointRange (const RangeType& from);
        DisjointRange (const initializer_list<RangeType>& from);
        template <typename CONTAINER_OF_RANGE_OF_T>
        explicit DisjointRange (const CONTAINER_OF_RANGE_OF_T& from);
        template <typename COPY_FROM_ITERATOR_OF_RANGE_OF_T, sentinel_for<remove_cvref_t<COPY_FROM_ITERATOR_OF_RANGE_OF_T>> COPY_FROM_ITERATOR_OF_RANGE_OF_T2>
        explicit DisjointRange (COPY_FROM_ITERATOR_OF_RANGE_OF_T&& start, COPY_FROM_ITERATOR_OF_RANGE_OF_T2&& end);

    public:
        nonvirtual DisjointRange& operator= (const DisjointRange& rhs) = default;

    public:
        /**
         */
        static DisjointRange FullRange ();

    public:
        /**
         *  A disjoint range is made up of a finite number of disjoint (non-overlapping) subranges, which are arranged
         *  following the natural ordering intrinsic to the value_type. This returns those subranges.
         */
        nonvirtual Containers::Sequence<RangeType> SubRanges () const;

    public:
        /**
         *  Containing no points (same as containing no subranges).
         */
        nonvirtual bool empty () const;

    public:
        /**
         *  Same as *this = DisjointRange ()
         */
        nonvirtual void clear ();

    public:
        /**
         *  @see Range::Contains.
         *
         *  This returns true if r is contained in some sub-element range. This is only the smae as
         *  GetBounds().Contains() if SubRanges ().size () <= 1.
         */
        nonvirtual bool Contains (value_type elt) const;
        nonvirtual bool Contains (const RangeType& rhs) const;

    public:
        /**
         * if empty, returns empty range()
         * 
         * \note the resulting BOUNDS should captures the openness flags from the LHS and RHS bounds
         *
         *  // @todo need constexpr check if RangeType gas this CTOR (careful of case when used with DicreteRange)
         */
        nonvirtual RangeType GetBounds () const;

    public:
        /**
         *  \brief like GetBounds (), but always returns a closed range
         *
         *  // @todo need constexpr check if RangeType gas this CTOR (careful of case when used with DicreteRange)
         */
        nonvirtual RangeType Closure () const;

    public:
        /**
         *  This returns true if the constituent subranges are all equal. This amounts to checking
         *  if the 'covered points' are all equal.
         */
        constexpr bool operator== (const DisjointRange& rhs) const;

    public:
        /**
         */
        nonvirtual bool Intersects (const RangeType& rhs) const;
        nonvirtual bool Intersects (const DisjointRange& rhs) const;

    public:
        /**
         */
        nonvirtual DisjointRange Intersection (const RangeType& rhs) const;
        nonvirtual DisjointRange Intersection (const DisjointRange& rhs) const;

    public:
        /**
         */
        nonvirtual DisjointRange Union (const DisjointRange& rhs) const;

    public:
        /**
         */
        nonvirtual RangeType UnionBounds (const DisjointRange& rhs) const;

    public:
        /**
         */
        nonvirtual Characters::String ToString (const function<Characters::String (T)>& elt2String = [] (T x) -> Characters::String {
            return Characters::ToString (x);
        }) const;

    private:
        nonvirtual void MergeIn_ (const RangeType& r); // non-const method, but only called during construction

    private:
        nonvirtual void AssertInternalRepValid_ () const;

    private:
        Containers::Sequence<RangeType> fSubRanges_;

    private:
        static constexpr bool sNoisyDebugTrace_ = false;
    };

    /**
     */
    template <typename T, typename RANGE_TYPE>
    DisjointRange<T, RANGE_TYPE> operator+ (const DisjointRange<T, RANGE_TYPE>& lhs, const DisjointRange<T, RANGE_TYPE>& rhs);

    /**
     */
    template <typename T, typename RANGE_TYPE>
    DisjointRange<T, RANGE_TYPE> operator^ (const DisjointRange<T, RANGE_TYPE>& lhs, const DisjointRange<T, RANGE_TYPE>& rhs);

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "DisjointRange.inl"

#endif /*_Stroika_Foundation_Traversal_DisjointRange_h_ */
