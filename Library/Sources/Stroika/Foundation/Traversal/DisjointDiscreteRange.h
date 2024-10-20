/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DisjointDiscreteRange_h_
#define _Stroika_Foundation_Traversal_DisjointDiscreteRange_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Containers/SortedSet.h"
#include "Stroika/Foundation/Traversal/DiscreteRange.h"
#include "Stroika/Foundation/Traversal/DisjointRange.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   More efficient DisjointDiscreteRange<T, RANGE_TYPE>::Elements () implementation
 */

namespace Stroika::Foundation::Traversal {

    /**
     *  @todo Add CONCEPT to require RANGE_TYPE subtype of DiscreteRange
     */
    template <typename T, typename RANGE_TYPE = DiscreteRange<T>>
    class DisjointDiscreteRange : public DisjointRange<T, RANGE_TYPE> {
    private:
        using inherited   = DisjointRange<T, RANGE_TYPE>;
        using THIS_CLASS_ = DisjointDiscreteRange<T, RANGE_TYPE>;

    public:
        using value_type = typename inherited::value_type;
        static_assert (same_as<T, value_type>);
        using RangeType = typename inherited::RangeType;
        static_assert (same_as<RANGE_TYPE, RangeType>);

    public:
        /**
         *  You can pass in empty Ranges, and ranges out of order, but the constructor always filters out
         *  empty ranges, and re-orders so subranges well-ordered.
         */
        DisjointDiscreteRange ()                             = default;
        DisjointDiscreteRange (const DisjointDiscreteRange&) = default;
        DisjointDiscreteRange (const RangeType& from);
        DisjointDiscreteRange (const initializer_list<RangeType>& from);
        template <IIterableOf<RANGE_TYPE> RANGES_OF_T>
        explicit DisjointDiscreteRange (RANGES_OF_T&& from);
        template <IIterableOf<T> TS>
        explicit DisjointDiscreteRange (TS&& from);
        template <IInputIterator<RANGE_TYPE> ITERATOR_OF_RANGE_OF_T, sentinel_for<remove_cvref_t<ITERATOR_OF_RANGE_OF_T>> ITERATOR_OF_RANGE_OF_T2>
        explicit DisjointDiscreteRange (ITERATOR_OF_RANGE_OF_T&& start, ITERATOR_OF_RANGE_OF_T2&& end);
        template <IInputIterator<T> ITERATOR_OF_T, sentinel_for<remove_cvref_t<ITERATOR_OF_T>> ITERATOR_OF_T2>
        explicit DisjointDiscreteRange (ITERATOR_OF_T&& start, ITERATOR_OF_T2&& end);

    public:
        nonvirtual DisjointDiscreteRange& operator= (const DisjointDiscreteRange& rhs) = default;

    public:
        /**
         */
        nonvirtual void Add (const value_type& elt);

    public:
        /**
         */
        nonvirtual DisjointDiscreteRange Intersection (const RangeType& rhs) const;
        nonvirtual DisjointDiscreteRange Intersection (const DisjointDiscreteRange& rhs) const;

    public:
        /**
         */
        nonvirtual optional<value_type> GetNext (value_type elt) const;

    public:
        /**
         */
        nonvirtual optional<value_type> GetPrevious (value_type elt) const;

    public:
        /**
         *  \par Example Usage
         *      \code
         *          DisjointDiscreteRange<DiscreteRange<int>> t;
         *          for (T i : t.Elements ()) {
         *          }
         *      \endcode
         *
         *  Elements () makes no guarantees about whether or not modifications to the underlying DisjointDiscreteRange<> will
         *  appear in the Elements() Iterable<T>.
         */
        nonvirtual Iterable<value_type> Elements () const;

    public:
        /**
         *  Not needed, but this provides the ability to performance tweak the search done by Find/FindLastThat
         */
        struct FindHints {
            value_type fSeedPosition;
            bool       fForwardFirst;
            FindHints (value_type seedPosition, bool forwardFirst);
        };

    public:
        /**
         *  Find the first element of the DisjointDiscreteRange that passes the argument function test.
         &&&& docs - assumes a bit that one subrange meeting criteria - fill in details
         */
        nonvirtual optional<value_type> Find (const function<bool (value_type)>& that) const;
        nonvirtual optional<value_type> Find (const function<bool (value_type)>& that, const FindHints& hints) const;

    public:
        /**
         *  Find the last element of the DisjointDiscreteRange that passes the argument function test.
         &&&& docs - assumes a bit that one subrange meeting criteria - fill in details
         */
        nonvirtual optional<value_type> FindLastThat (const function<bool (value_type)>& testF) const;
        nonvirtual optional<value_type> FindLastThat (const function<bool (value_type)>& testF, const FindHints& hints) const;

    private:
        nonvirtual optional<value_type> ScanTil_ (const function<bool (value_type)>& testF,
                                                  const function<optional<value_type> (value_type)>& iterNext, value_type seedPosition) const;
        nonvirtual optional<value_type> ScanFindAny_ (const function<bool (value_type)>& testF, value_type seedPosition, bool forwardFirst) const;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "DisjointDiscreteRange.inl"

#endif /*_Stroika_Foundation_Traversal_DisjointDiscreteRange_h_ */
