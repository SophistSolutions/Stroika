/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DisjointDiscreteRange_h_
#define _Stroika_Foundation_Traversal_DisjointDiscreteRange_h_ 1

#include "../StroikaPreComp.h"

#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Containers/Sequence.h"
#include "../Containers/SortedSet.h"

#include "DiscreteRange.h"
#include "DisjointRange.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   More efficient DisjointDiscreteRange<T, RANGE_TYPE>::Elements () implementation
 */

namespace Stroika {
    namespace Foundation {
        namespace Traversal {

            /**
             *  @todo Add CONCEPT to require RANGE_TYPE subtype of DiscreteRange
             */
            template <typename T, typename RANGE_TYPE = DiscreteRange<T>>
            class DisjointDiscreteRange : public DisjointRange<T, RANGE_TYPE> {
            private:
                using inherited   = DisjointRange<T, RANGE_TYPE>;
                using THIS_CLASS_ = DisjointDiscreteRange<T, RANGE_TYPE>;

            public:
                using value_type = typename DisjointRange<T, RANGE_TYPE>::value_type;

            public:
                using RangeType = typename DisjointRange<T, RANGE_TYPE>::RangeType;

            public:
                /**
                 *  You can pass in empty Ranges, and ranges out of order, but the constructor always filters out
                 *  empty ranges, and re-orders so subranges well-ordered.
                 */
                DisjointDiscreteRange ()                             = default;
                DisjointDiscreteRange (const DisjointDiscreteRange&) = default;
                DisjointDiscreteRange (const RangeType& from);
                DisjointDiscreteRange (const initializer_list<RangeType>& from);
                template <typename CONTAINER_OF_DISCRETERANGE_OF_T>
                explicit DisjointDiscreteRange (const CONTAINER_OF_DISCRETERANGE_OF_T& from);
                template <typename COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T>
                explicit DisjointDiscreteRange (COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T start, COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T end, typename enable_if<is_convertible<typename COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T::value_type, RangeType>::value, int>::type* = nullptr);
                template <typename COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T>
                explicit DisjointDiscreteRange (COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T start, COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T end, typename enable_if<is_convertible<typename COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T::value_type, value_type>::value, int>::type* = nullptr);

            public:
                nonvirtual DisjointDiscreteRange& operator= (const DisjointDiscreteRange& rhs) = default;

            public:
                /**
                 */
                nonvirtual void Add (value_type elt);

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
                 *      DisjointDiscreteRange<DiscreteRange<int>> t;
                 *      for (T i : t.Elements ()) {
                 *      }
                 *      \endcode
                 *
                 *  Elements () makes no guarantess about whether or not modifications to the underlying DisjointDiscreteRange<> will
                 *  appear in the Elements() Iterable<T>.
                 */
                nonvirtual Iterable<value_type> Elements () const;

            public:
                /**
                 *  Not needed, but this provides the ability to performance tweek the search done by FindFirstThat/FindLastThat
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
                nonvirtual optional<value_type> FindFirstThat (const function<bool(value_type)>& testF) const;
                nonvirtual optional<value_type> FindFirstThat (const function<bool(value_type)>& testF, const FindHints& hints) const;

            public:
                /**
                 *  Find the last element of the DisjointDiscreteRange that passes the argument function test.
                 &&&& docs - assumes a bit that one subrange meeting criteria - fill in details
                 */
                nonvirtual optional<value_type> FindLastThat (const function<bool(value_type)>& testF) const;
                nonvirtual optional<value_type> FindLastThat (const function<bool(value_type)>& testF, const FindHints& hints) const;

            private:
                nonvirtual optional<value_type> ScanTil_ (const function<bool(value_type)>& testF, const function<optional<value_type> (value_type)>& iterNext, value_type seedPosition) const;
                nonvirtual optional<value_type> ScanFindAny_ (const function<bool(value_type)>& testF, value_type seedPosition, bool forwardFirst) const;
            };
        }
    }
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "DisjointDiscreteRange.inl"

#endif /*_Stroika_Foundation_Traversal_DisjointDiscreteRange_h_ */
