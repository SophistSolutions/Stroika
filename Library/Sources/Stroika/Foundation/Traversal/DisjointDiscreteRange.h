/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DisjointDiscreteRange_h_
#define _Stroika_Foundation_Traversal_DisjointDiscreteRange_h_  1

#include    "../StroikaPreComp.h"

#include    "../Characters/String.h"
#include    "../Configuration/Common.h"
#include    "../Containers/Sequence.h"
#include    "../Containers/SortedSet.h"
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
                template    <typename T2>
                using   Optional = Memory::Optional<T2>;

            public:
                /**
                 *  You can pass in empty Ranges, and ranges out of order, but the constructor always filters out
                 *  empty ranges, and re-orders so subranges well-ordered.
                 */
                DisjointDiscreteRange () = default;
                DisjointDiscreteRange (const DisjointDiscreteRange&) = default;
                DisjointDiscreteRange (const RangeType& from);
                DisjointDiscreteRange (const initializer_list<RangeType>& from);
                template <typename CONTAINER_OF_DISCRETERANGE_OF_T>
                explicit DisjointDiscreteRange (const CONTAINER_OF_DISCRETERANGE_OF_T& from);
                template <typename COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T>
                explicit DisjointDiscreteRange (COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T start, COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T end, typename enable_if < is_convertible <typename COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T::value_type, RangeType>::value, int >::type* = nullptr);
                template <typename COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T>
                explicit DisjointDiscreteRange (COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T start, COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T end, typename enable_if < is_convertible <typename COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T::value_type, ElementType>::value, int >::type* = nullptr);

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
                nonvirtual  Optional<ElementType> GetNext (ElementType elt) const;

            public:
                /**
                 */
                nonvirtual  Optional<ElementType> GetPrevious (ElementType elt) const;

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
                nonvirtual  Iterable<ElementType>   Elements () const;

            public:
                /**
                 *  Not needed, but this provides the ability to performance tweek teh search done by FindFirstThat/FindLastThat
                 */
                struct  FindHints {
                    ElementType fSeedPosition;
                    bool        fForwardFirst;
                    FindHints (ElementType seedPosition, bool forwardFirst);
                };

            public:
                /**
                 *  Find the first element of the DisjointDiscreteRange that passes the argument function test.
                 &&&& docs - assumes a bit that one subrange meeting criteria - fill in details
                 */
                nonvirtual  Optional<ElementType>    FindFirstThat (const function<bool(ElementType)>& testF) const;
                nonvirtual  Optional<ElementType>    FindFirstThat (const function<bool(ElementType)>& testF, const FindHints& hints) const;

            public:
                /**
                 *  Find the last element of the DisjointDiscreteRange that passes the argument function test.
                 &&&& docs - assumes a bit that one subrange meeting criteria - fill in details
                 */
                nonvirtual  Optional<ElementType>    FindLastThat (const function<bool(ElementType)>& testF) const;
                nonvirtual  Optional<ElementType>    FindLastThat (const function<bool(ElementType)>& testF, const FindHints& hints) const;

            private:
                nonvirtual  Optional<ElementType>    ScanTil_ (const function<bool(ElementType)>& testF, const function<Optional<ElementType>(ElementType)>& iterNext, ElementType seedPosition) const;
                nonvirtual  Optional<ElementType>    ScanFindAny_ (const function<bool(ElementType)>& testF, ElementType seedPosition, bool forwardFirst) const;
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
