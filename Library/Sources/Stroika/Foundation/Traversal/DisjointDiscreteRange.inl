/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DisjointDiscreteRange_inl_
#define _Stroika_Foundation_Traversal_DisjointDiscreteRange_inl_

#include "Generator.h"

namespace Stroika::Foundation::Traversal {

    /*
     ********************************************************************************
     ************* DisjointDiscreteRange<T, RANGE_TYPE>::FindHints ******************
     ********************************************************************************
     */
    template <typename T, typename RANGE_TYPE>
    inline DisjointDiscreteRange<T, RANGE_TYPE>::FindHints::FindHints (value_type seedPosition, bool forwardFirst)
        : fSeedPosition (seedPosition)
        , fForwardFirst (forwardFirst)
    {
    }

    /*
     ********************************************************************************
     ********************* DisjointDiscreteRange<T, RANGE_TYPE> *********************
     ********************************************************************************
     */
    template <typename T, typename RANGE_TYPE>
    inline DisjointDiscreteRange<T, RANGE_TYPE>::DisjointDiscreteRange (const RangeType& from)
        : inherited (from)
    {
    }
    template <typename T, typename RANGE_TYPE>
    inline DisjointDiscreteRange<T, RANGE_TYPE>::DisjointDiscreteRange (const initializer_list<RangeType>& from)
        : inherited (from)
    {
    }
    template <typename T, typename RANGE_TYPE>
    template <typename CONTAINER_OF_DISCRETERANGE_OF_T>
    DisjointDiscreteRange<T, RANGE_TYPE>::DisjointDiscreteRange (const CONTAINER_OF_DISCRETERANGE_OF_T& from)
        : DisjointDiscreteRange (from.begin (), from.end ())
    {
    }
    template <typename T, typename RANGE_TYPE>
    template <typename COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T>
    DisjointDiscreteRange<T, RANGE_TYPE>::DisjointDiscreteRange (COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T start, COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T end, enable_if_t<is_convertible_v<typename COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T::value_type, RangeType>, int>*)
        : inherited (start, end)
    {
    }
    template <typename T, typename RANGE_TYPE>
    template <typename COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T>
    DisjointDiscreteRange<T, RANGE_TYPE>::DisjointDiscreteRange (COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T start, COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T end, enable_if_t<is_convertible_v<typename COPY_FROM_ITERATOR_OF_DISCRETERANGE_OF_T::value_type, value_type>, int>*)
        : inherited ()
    {
        Containers::Sequence<RangeType> srs{};
        // @todo DEBUG why this initializer syntax produces wrong overload call ???
        // --2015-01-02
        //Containers::SortedSet<value_type> ss { start, end };
        Containers::SortedSet<value_type> ss = Containers::SortedSet<value_type> (start, end);
        value_type                        startAt{};
        optional<value_type>              endAt;
        for (value_type i : ss) {
            if (not endAt.has_value ()) {
                startAt = i;
                endAt   = i;
            }
            else if (RangeType::TraitsType::GetNext (*endAt) == i) {
                endAt = i;
            }
            else {
                Assert (startAt <= *endAt);
                srs.Append (RangeType (startAt, *endAt));
                startAt = i;
                endAt   = i;
            }
        }
        if (endAt) {
            Assert (startAt <= *endAt);
            srs.Append (RangeType (startAt, *endAt));
        }
        *this = move (THIS_CLASS_{srs});
    }
    template <typename T, typename RANGE_TYPE>
    void DisjointDiscreteRange<T, RANGE_TYPE>::Add (value_type elt)
    {
        Containers::Sequence<RangeType> srs{this->SubRanges ()};
        // Walk list, and if new item < than a given, either extend or insert. If contained, we have nothing todo
        for (Iterator<RangeType> i = srs.begin (); i != srs.end (); ++i) {
            if (i->Contains (elt)) {
                return;
            }
            else if (elt == i->GetLowerBound () - 1) {
                srs.Update (i, DiscreteRange<value_type> (elt, i->GetUpperBound ()));
                // No need to check for merge adjacent cuz done by constructor
                *this = move (THIS_CLASS_{srs});
                return;
            }
            else if (elt == i->GetUpperBound () + 1) {
                srs.Update (i, DiscreteRange<value_type> (i->GetLowerBound (), elt));
                // No need to check for merge adjacent cuz done by constructor
                *this = move (THIS_CLASS_{srs});
                return;
            }
            else if (elt < i->GetLowerBound ()) {
                // wont be found later, so break now, and add the point
                break;
            }
        }
        // if not less than any there, we must append new item
        srs.push_back (DiscreteRange<value_type> (elt, elt));
        *this = move (THIS_CLASS_{srs});
    }
    template <typename T, typename RANGE_TYPE>
    auto DisjointDiscreteRange<T, RANGE_TYPE>::Intersection (const RangeType& rhs) const -> DisjointDiscreteRange
    {
        // @todo could do more efficiently
        return DisjointDiscreteRange{inherited::Intersection (rhs).SubRanges ()};
    }
    template <typename T, typename RANGE_TYPE>
    auto DisjointDiscreteRange<T, RANGE_TYPE>::Intersection (const DisjointDiscreteRange& rhs) const -> DisjointDiscreteRange
    {
        // @todo could do more efficiently
        return DisjointDiscreteRange{inherited::Intersection (rhs).SubRanges ()};
    }
    template <typename T, typename RANGE_TYPE>
    auto DisjointDiscreteRange<T, RANGE_TYPE>::GetNext (value_type elt) const -> optional<value_type>
    {
        Containers::Sequence<RangeType> subRanges{this->SubRanges ()};
        // Find the first subrange which might contain elt, or successors
        value_type          next = RANGE_TYPE::TraitsType::GetNext (elt);
        Iterator<RangeType> i{subRanges.FindFirstThat ([next] (const RangeType& r) -> bool { return r.GetUpperBound () >= next; })};
        if (i) {
            return max (next, i->GetLowerBound ());
        }
        return nullopt;
    }
    template <typename T, typename RANGE_TYPE>
    auto DisjointDiscreteRange<T, RANGE_TYPE>::GetPrevious (value_type elt) const -> optional<value_type>
    {
        Containers::Sequence<RangeType> subRanges{this->SubRanges ()};
        // Find the first subrange which might contain elt, or predecessors
        value_type          prev = RANGE_TYPE::TraitsType::GetPrevious (elt);
        Iterator<RangeType> i{subRanges.FindFirstThat ([prev] (const RangeType& r) -> bool { return r.GetUpperBound () >= prev; })};
        if (i) {
            if (i->Contains (prev)) {
                return prev;
            }
        }

        // tmphack - need random-access iterators !!! for sequence at least!
        auto prevOfIterator = [&subRanges] (const Iterator<RangeType>& pOfI) -> Iterator<RangeType> {
            Iterator<RangeType> result = Iterator<RangeType>::GetEmptyIterator ();
            for (Iterator<RangeType> i = subRanges.begin (); i != subRanges.end (); ++i) {
                if (i == pOfI) {
                    return result;
                }
                result = i;
            }
            return result;
        };

        // if none contain next, find the last before we pass prev
        i = prevOfIterator (subRanges.FindFirstThat ([prev] (const RangeType& r) -> bool { return r.GetUpperBound () > prev; }));
        if (i) {
            Ensure (i->GetUpperBound () < prev);
            return i->GetUpperBound ();
        }
        return nullopt;
    }
    template <typename T, typename RANGE_TYPE>
    auto DisjointDiscreteRange<T, RANGE_TYPE>::Elements () const -> Iterable<value_type>
    {
        using UnsignedDifferenceType = typename RANGE_TYPE::UnsignedDifferenceType;
        Containers::Sequence<RangeType> subRanges{this->SubRanges ()};
        struct context_ {
            Containers::Sequence<RangeType> fSubRanges;
            size_t                          fSubRangeIdx{};
            UnsignedDifferenceType          fCurrentSubRangeIteratorAt{};
            context_ () = delete;
            context_ (const Containers::Sequence<RangeType>& sr)
                : fSubRanges (sr)
            {
            }
            context_ (const context_& from) = default;
        };
        auto myContext = make_shared<context_> (context_{subRanges});
        auto getNext   = [myContext] () -> optional<value_type> {
            if (myContext->fSubRangeIdx < myContext->fSubRanges.size ()) {
                RangeType              curRange{myContext->fSubRanges[myContext->fSubRangeIdx]};
                UnsignedDifferenceType nEltsPerRange{curRange.GetDistanceSpanned ()};
                Assert (myContext->fCurrentSubRangeIteratorAt <= nEltsPerRange);
                value_type result = curRange.GetLowerBound () + myContext->fCurrentSubRangeIteratorAt;
                if (myContext->fCurrentSubRangeIteratorAt == nEltsPerRange) {
                    myContext->fSubRangeIdx++;
                    myContext->fCurrentSubRangeIteratorAt = 0;
                }
                else {
                    myContext->fCurrentSubRangeIteratorAt++;
                }
                return result;
            }
            return nullopt;
        };
        return Traversal::CreateGenerator<value_type> (getNext);
    }
    template <typename T, typename RANGE_TYPE>
    auto DisjointDiscreteRange<T, RANGE_TYPE>::FindFirstThat (const function<bool (value_type)>& testF) const -> optional<value_type>
    {
        return this->empty () ? optional<value_type> () : FindFirstThat (testF, FindHints (this->GetBounds ().GetLowerBound (), true));
    }
    template <typename T, typename RANGE_TYPE>
    auto DisjointDiscreteRange<T, RANGE_TYPE>::FindFirstThat (const function<bool (value_type)>& testF, const FindHints& hints) const -> optional<value_type>
    {
        Require (this->Contains (hints.fSeedPosition));
        optional<value_type> o = ScanFindAny_ (testF, hints.fSeedPosition, hints.fForwardFirst);
        if (o) {
            // If we found any, then there is a first, so find scan back to find it...
            value_type firstTrueFor{*o};
            value_type i{firstTrueFor};
            while (testF (i)) {
                firstTrueFor           = i;
                optional<value_type> o = GetPrevious (i);
                if (o) {
                    i = *o;
                }
                else {
                    break;
                }
            }
            return firstTrueFor;
        }
        else {
            return nullopt;
        }
    }
    template <typename T, typename RANGE_TYPE>
    auto DisjointDiscreteRange<T, RANGE_TYPE>::FindLastThat (const function<bool (value_type)>& testF) const -> optional<value_type>
    {
        return this->empty () ? optional<value_type> () : FindLastThat (testF, FindHints (this->GetBounds ().GetUpperBound (), false));
    }
    template <typename T, typename RANGE_TYPE>
    auto DisjointDiscreteRange<T, RANGE_TYPE>::FindLastThat (const function<bool (value_type)>& testF, const FindHints& hints) const -> optional<value_type>
    {
        Require (this->Contains (hints.fSeedPosition));
        optional<value_type> o = ScanFindAny_ (testF, hints.fSeedPosition, hints.fForwardFirst);
        if (o) {
            // If we found any, then there is a last, so find scan forwards to find it...
            value_type lastTrueFor{*o};
            value_type i{lastTrueFor};
            while (testF (i)) {
                lastTrueFor            = i;
                optional<value_type> o = GetNext (i);
                if (o) {
                    i = *o;
                }
                else {
                    break;
                }
            }
            return lastTrueFor;
        }
        else {
            return nullopt;
        }
    }
    template <typename T, typename RANGE_TYPE>
    auto DisjointDiscreteRange<T, RANGE_TYPE>::ScanTil_ (const function<bool (value_type)>& testF, const function<optional<value_type> (value_type)>& iterNext, value_type seedPosition) const -> optional<value_type>
    {
        value_type i{seedPosition};
        while (not testF (i)) {
            optional<value_type> o = iterNext (i);
            if (o) {
                i = *o;
            }
            else {
                return nullopt;
            }
        }
        Ensure (testF (i));
        return i;
    }
    template <typename T, typename RANGE_TYPE>
    auto DisjointDiscreteRange<T, RANGE_TYPE>::ScanFindAny_ (const function<bool (value_type)>& testF, value_type seedPosition, bool forwardFirst) const -> optional<value_type>
    {
        /*
            *  First we must find a value/position where testF is true. It could be forward or backward from our start hint.
            *  Try one direction, and then the other.
            *
            *  Only return 'IsIMissing()' if there are no values from which testF is true.
            */
        function<optional<value_type> (value_type)> backwardNext = [this] (value_type i) { return GetPrevious (i); };
        function<optional<value_type> (value_type)> forwardNext  = [this] (value_type i) { return GetNext (i); };
        value_type                                  i{seedPosition};
        optional<value_type>                        o{ScanTil_ (testF, forwardFirst ? forwardNext : backwardNext, i)};
        if (o) {
            // then we found a value scanning back for which testF is true
            i = *o;
        }
        else {
            // scan the other way and see if its found
            o = ScanTil_ (testF, forwardFirst ? backwardNext : forwardNext, i);
            if (o) {
                i = *o;
            }
            else {
                // if not found scanning forward, or backward, its not there!
                return nullopt;
            }
        }
        Assert (testF (i));
        return i;
    }

}

#endif /* _Stroika_Foundation_Traversal_DisjointDiscreteRange_inl_ */
