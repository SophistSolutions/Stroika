/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DisjointDiscreteRange_inl_
#define _Stroika_Foundation_Traversal_DisjointDiscreteRange_inl_


namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /*
            ********************************************************************************
            ********************* DisjointDiscreteRange<T, RANGE_TYPE> *********************
            ********************************************************************************
            */
            template    <typename T, typename RANGE_TYPE>
            inline  DisjointDiscreteRange<T, RANGE_TYPE>::DisjointDiscreteRange (const RangeType& from)
                : inherited (from)
            {
            }
            template    <typename T, typename RANGE_TYPE>
            inline  DisjointDiscreteRange<T, RANGE_TYPE>::DisjointDiscreteRange (const initializer_list<RangeType>& from)
                : inherited (from)
            {
            }
            template    <typename T, typename RANGE_TYPE>
            template <typename CONTAINER_OF_DISCRETERANGE_OF_T>
            DisjointDiscreteRange<T, RANGE_TYPE>::DisjointDiscreteRange (const CONTAINER_OF_DISCRETERANGE_OF_T& from)
                : inherited (from)
            {
            }
            template    <typename T, typename RANGE_TYPE>
            template <typename COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T>
            DisjointDiscreteRange<T, RANGE_TYPE>::DisjointDiscreteRange (COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T start, COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T end)
                : inherited (start, end)
            {
            }
            template    <typename T, typename RANGE_TYPE>
            void    DisjointDiscreteRange<T, RANGE_TYPE>::Add (ElementType elt)
            {
                Containers::Sequence<RangeType> srs { this->SubRanges () };
                // Walk list, and if new item < than a given, either extend or insert. If contained, we have nothing todo
                for (Iterator<RangeType> i = srs.begin (); i != srs.end (); ++i) {
                    if (i->Contains (elt)) {
                        return;
                    }
                    else if (elt == i->GetLowerBound () - 1) {
                        srs.Update (i, DiscreteRange<ElementType> (elt, i->GetUpperBound ()));
                        // No need to check for merge adjacent cuz done by constructor
                        *this = move (THIS_CLASS_ { srs });
                        return;
                    }
                    else if (elt == i->GetUpperBound () + 1) {
                        srs.Update (i, DiscreteRange<ElementType> (i->GetLowerBound (), elt));
                        // No need to check for merge adjacent cuz done by constructor
                        *this = move (THIS_CLASS_ { srs });
                        return;
                    }
                    else if (elt < i->GetLowerBound ()) {
                        // wont be found later, so break now, and add the point
                        break;
                    }
                }
                // if not less than any there, we must append new item
                srs.push_back (DiscreteRange<ElementType> (elt, elt));
                *this = move (THIS_CLASS_ { srs });
            }
            template    <typename T, typename RANGE_TYPE>
            auto    DisjointDiscreteRange<T, RANGE_TYPE>::Intersection (const RangeType& rhs) const -> DisjointDiscreteRange
            {
                // @todo could do more efficiently
                return DisjointDiscreteRange { inherited::Intersection (rhs).SubRanges () };
            }
            template    <typename T, typename RANGE_TYPE>
            auto    DisjointDiscreteRange<T, RANGE_TYPE>::Intersection (const DisjointDiscreteRange& rhs) const -> DisjointDiscreteRange
            {
                // @todo could do more efficiently
                return DisjointDiscreteRange { inherited::Intersection (rhs).SubRanges () };
            }
            template    <typename T, typename RANGE_TYPE>
            auto   DisjointDiscreteRange<T, RANGE_TYPE>::GetNext (ElementType elt) const -> Memory::Optional<ElementType> {
                Containers::Sequence<RangeType> subRanges { this->SubRanges () };
                // Find the first subrange which might contain elt, or successors
                ElementType next = RANGE_TYPE::TraitsType::GetNext (elt);
                Iterator<RangeType> i { subRanges.FindFirstThat ([next] (const RangeType & r) -> bool {return r.GetUpperBound () >= next;}) };
                if (i)
                {
                    return max (next, i->GetLowerBound ());
                }
                return Memory::Optional<ElementType> ();
            }
            template    <typename T, typename RANGE_TYPE>
            auto DisjointDiscreteRange<T, RANGE_TYPE>::GetPrevious (ElementType elt) const -> Memory::Optional<ElementType> {
                Containers::Sequence<RangeType> subRanges { this->SubRanges () };
                // Find the first subrange which might contain elt, or predecessors
                ElementType prev = RANGE_TYPE::TraitsType::GetPrevious (elt);
                Iterator<RangeType> i { subRanges.FindFirstThat ([prev] (const RangeType & r) -> bool {return r.GetUpperBound () >= prev;}) };
                if (i)
                {
                    if (i->Contains (prev)) {
                        return prev;
                    }
                }

                // tmphack - need random-access iterators !!! for sequence at least!
                auto prevOfIterator = [&subRanges] (const Iterator<RangeType>& pOfI) -> Iterator<RangeType> {
                    Iterator<RangeType> result = Iterator<RangeType>::GetEmptyIterator ();
                    for (Iterator<RangeType> i = subRanges.begin (); i != subRanges.end (); ++i)
                    {
                        if (i == pOfI) {
                            return result;
                        }
                        result = i;
                    }
                    return result;
                };

                // if none contain next, find the last before we pass prev
                i  = prevOfIterator (subRanges.FindFirstThat ([prev] (const RangeType & r) -> bool {return r.GetUpperBound () > prev;}));
                if (i)
                {
                    Ensure (i->GetUpperBound () < prev);
                    return i->GetUpperBound ();
                }
                return Memory::Optional<ElementType> ();
            }
            template    <typename T, typename RANGE_TYPE>
            auto   DisjointDiscreteRange<T, RANGE_TYPE>::Elements () const -> Iterable<ElementType> {
                // HORIBLE but for now hopefully adequate implementation
                // @todo see MultiSet::ElementsIteratorHelper_ for better impl strategy...
                Containers::Sequence<RangeType>     subRanges { this->SubRanges () };
                Containers::Sequence<ElementType>   result {};
                for (RangeType ri : subRanges)
                {
                    for (ElementType rri : ri) {
                        result.Append (rri);
                    }
                }
                return result;
            }


        }
    }
}
#endif /* _Stroika_Foundation_Traversal_DisjointDiscreteRange_inl_ */
