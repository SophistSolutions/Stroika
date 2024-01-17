/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DisjointRange_inl_
#define _Stroika_Foundation_Traversal_DisjointRange_inl_

namespace Stroika::Foundation::Traversal {

    /*
     ********************************************************************************
     ********************** DisjointRange<T, RANGE_TYPE> ****************************
     ********************************************************************************
     */
    template <typename T, typename RANGE_TYPE>
    DisjointRange<T, RANGE_TYPE>::DisjointRange (const RangeType& from)
    {
        MergeIn_ (from);
    }
    template <typename T, typename RANGE_TYPE>
    inline DisjointRange<T, RANGE_TYPE>::DisjointRange (const initializer_list<RangeType>& from)
        : DisjointRange{begin (from), end (from)}
    {
    }
    template <typename T, typename RANGE_TYPE>
    template <typename CONTAINER_OF_RANGE_OF_T>
    inline DisjointRange<T, RANGE_TYPE>::DisjointRange (const CONTAINER_OF_RANGE_OF_T& from)
        : DisjointRange{begin (from), end (from)}
    {
    }
    template <typename T, typename RANGE_TYPE>
    template <typename COPY_FROM_ITERATOR_OF_RANGE_OF_T>
    DisjointRange<T, RANGE_TYPE>::DisjointRange (COPY_FROM_ITERATOR_OF_RANGE_OF_T start, COPY_FROM_ITERATOR_OF_RANGE_OF_T end)
    {
        for (auto i = start; i != end; ++i) {
            MergeIn_ (*i);
        }
        if constexpr (qDebug) {
            for (auto i = fSubRanges_.begin (); i != fSubRanges_.end (); ++i) {
                if (i + 1 != fSubRanges_.end ()) {
                    Ensure (i->GetUpperBound () <= (i + 1)->GetLowerBound ());
                }
            }
        }
    }
    template <typename T, typename RANGE_TYPE>
    inline auto DisjointRange<T, RANGE_TYPE>::SubRanges () const -> Containers::Sequence<RangeType>
    {
        return fSubRanges_;
    }
    template <typename T, typename RANGE_TYPE>
    DisjointRange<T, RANGE_TYPE> DisjointRange<T, RANGE_TYPE>::FullRange ()
    {
        return DisjointRange<RangeType>{RangeType::FullRange ()};
    }
    template <typename T, typename RANGE_TYPE>
    inline bool DisjointRange<T, RANGE_TYPE>::empty () const
    {
        return fSubRanges_.empty ();
    }
    template <typename T, typename RANGE_TYPE>
    inline void DisjointRange<T, RANGE_TYPE>::clear ()
    {
        fSubRanges_.clear ();
    }
    template <typename T, typename RANGE_TYPE>
    bool DisjointRange<T, RANGE_TYPE>::Contains (value_type elt) const
    {
        return static_cast<bool> (fSubRanges_.Find ([&elt] (const RangeType& r) { return r.Contains (elt); }));
    }
    template <typename T, typename RANGE_TYPE>
    bool DisjointRange<T, RANGE_TYPE>::Contains (const RangeType& rhs) const
    {
        // @todo could be more efficient
        DisjointRange<T, RANGE_TYPE>     intersection = Intersection (rhs);
        Containers::Sequence<RANGE_TYPE> sr{intersection.SubRanges ()};
        return sr.size () == 1 and sr[0] == rhs;
    }
    template <typename T, typename RANGE_TYPE>
    RANGE_TYPE DisjointRange<T, RANGE_TYPE>::GetBounds () const
    {
        // NOTE: this code counts on the fact that the subranges are ORDERED
        size_t n = fSubRanges_.size ();
        switch (n) {
            case 0:
                return RangeType{};
            case 1:
                return fSubRanges_[0];
            default:
                // @todo need constexpr check if RangeType gas this CTOR (careful of case when used with DicreteRange)
                //return RangeType{fSubRanges_[0].GetLowerBound (), fSubRanges_.Last ()->GetUpperBound (), fSubRanges_[0].GetLowerBoundOpenness (), fSubRanges_.Last ()->GetUpperBoundOpenness ()};
                return RangeType{fSubRanges_[0].GetLowerBound (), fSubRanges_.Last ()->GetUpperBound ()};
        }
    }
    template <typename T, typename RANGE_TYPE>
    RANGE_TYPE DisjointRange<T, RANGE_TYPE>::Closure () const
    {
        // NOTE: this code counts on the fact that the subranges are ORDERED
        size_t n = fSubRanges_.size ();
        switch (n) {
            case 0:
                return RangeType{};
            case 1:
                return fSubRanges_[0];
            default:
                // @todo need constexpr check if RangeType gas this CTOR (careful of case when used with DicreteRange)
                //return RangeType{fSubRanges_[0].GetLowerBound (), fSubRanges_.Last ()->GetUpperBound (), Openness::eClosed, Openness::eClosed};
                return RangeType{fSubRanges_[0].GetLowerBound (), fSubRanges_.Last ()->GetUpperBound ()};
        }
    }
    template <typename T, typename RANGE_TYPE>
    inline bool DisjointRange<T, RANGE_TYPE>::Intersects (const RangeType& rhs) const
    {
        // @todo could do more efficiently
        return not Intersection (rhs).empty ();
    }
    template <typename T, typename RANGE_TYPE>
    inline bool DisjointRange<T, RANGE_TYPE>::Intersects (const DisjointRange& rhs) const
    {
        // @todo could do more efficiently
        return not Intersection (rhs).empty ();
    }
    template <typename T, typename RANGE_TYPE>
    auto DisjointRange<T, RANGE_TYPE>::Intersection (const RangeType& rhs) const -> DisjointRange
    {
        // @todo could do more efficiently
        return Intersection (DisjointRange{rhs});
    }
    template <typename T, typename RANGE_TYPE>
    auto DisjointRange<T, RANGE_TYPE>::Intersection (const DisjointRange& rhs) const -> DisjointRange
    {
        // @todo could do more efficiently
        Containers::Sequence<RangeType> disjointRanges{};
        for (const RangeType& rri : rhs.SubRanges ()) {
            for (const RangeType& mySR : this->SubRanges ()) {
                RangeType intersectedSubPart = rri ^ mySR;
                if (not intersectedSubPart.empty ()) {
                    disjointRanges.Append (intersectedSubPart);
                }
            }
        }
        return DisjointRange{disjointRanges};
    }
    template <typename T, typename RANGE_TYPE>
    auto DisjointRange<T, RANGE_TYPE>::Union (const DisjointRange& rhs) const -> DisjointRange
    {
        // @todo could do more efficiently
        Containers::Sequence<RangeType> disjointRanges{};
        for (const RangeType& rri : rhs.SubRanges ()) {
            for (const RangeType& mySR : this->SubRanges ()) {
                RangeType sp = rri + mySR;
                disjointRanges.Append (sp);
            }
        }
        return DisjointRange{disjointRanges};
    }
    template <typename T, typename RANGE_TYPE>
    auto DisjointRange<T, RANGE_TYPE>::UnionBounds (const DisjointRange& rhs) const -> RangeType
    {
        // @todo could do more efficiently (since 1-dimensional and subranges ordered, just need to min (lhs of each)
        // and max (rhs of each (and grab closedness of edge captured)
        return Union (rhs).GetBounds ();
    }
    template <typename T, typename RANGE_TYPE>
    Characters::String DisjointRange<T, RANGE_TYPE>::ToString (const function<Characters::String (T)>& elt2String) const
    {
        Characters::StringBuilder out;
        out << "["sv;
        for (const RangeType& rri : SubRanges ()) {
            out << rri.ToString (elt2String);
        }
        out << "]"sv;
        return out.str ();
    }
    template <typename T, typename RANGE_TYPE>
    void DisjointRange<T, RANGE_TYPE>::MergeIn_ (const RangeType& r)
    {
#if 0
        if (sNoisyDebugTrace_) {
            DbgTrace (L"MergeIn (r = %s)", r.Format ().c_str ());
        }
#endif
        AssertInternalRepValid_ ();
        if (not r.empty ()) {
            value_type rStart{r.GetLowerBound ()};
            value_type rEnd{r.GetUpperBound ()};
            if (fSubRanges_.size () == 0) {
                fSubRanges_.Append (r);
            }
            else {
                /*
                 *         [XXXXXX]      [XXX]             [XXXXXXXXXXXX]
                 *  EX 1 ^                                    ^
                 *  BECOMES:
                 *       [XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX]
                 *
                 *         [XXXXXX]      [XXX]             [XXXXXXXXXXXX]
                 *  EX 2               ^                          ^
                 *  BECOMES:
                 *         [XXXXXX]    [XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX]
                 *
                 *  Find first range to the right of LEFT-ANCHOR, and adjusting it to the LEFT based on rStart,
                 *  and stretch it to the left (if needed).
                 *
                 *  Then grab the last block on the right (starting before rEnd) of the defined range,
                 *  and stetch its right side to the right. Then delete all those in between.
                 */
                Assert (fSubRanges_.size () >= 1);

                // tmphack - need random-access iterators !!! for sequence at least!
                auto prevOfIterator = [this] (const Iterator<RangeType>& pOfI) -> Iterator<RangeType> {
                    Iterator<RangeType> result = Iterator<RangeType>::GetEmptyIterator ();
                    for (Iterator<RangeType> i = fSubRanges_.begin (); i != fSubRanges_.end (); ++i) {
                        if (i == pOfI) {
                            return result;
                        }
                        result = i;
                    }
                    return result;
                };

                auto findStartI = [this, &rStart] () -> Iterator<RangeType> {
                    return fSubRanges_.Find (
                        [&rStart] (const RangeType& r) -> bool { return r.GetLowerBound () >= rStart or r.Contains (rStart); });
                };

                Iterator<RangeType> startI = findStartI ();
                bool                extendedRange{false};
                if (startI == fSubRanges_.end ()) {
                    if (sNoisyDebugTrace_) {
                        DbgTrace ("Appending subrange cuz this is past the rest: %f/%f", static_cast<double> (r.GetLowerBound ()),
                                  static_cast<double> (r.GetUpperBound ()));
                    }
                    // cuz this means no ranges to the right containing rStart
                    //
                    // when appending, we can sometimes extend the last item
                    value_type          prevVal = RangeType::TraitsType::GetPrevious (rStart);
                    Iterator<RangeType> i =
                        fSubRanges_.Find ([prevVal] (const RangeType& r) -> bool { return r.GetUpperBound () == prevVal; });
                    if (i) {
                        Assert (i->GetUpperBound () == prevVal);
                        RangeType newValue{i->GetLowerBound (), rStart};
                        fSubRanges_.Update (i, newValue);
                        extendedRange = true;
                    }
                    else {
                        fSubRanges_.Append (r);
                    }
                }
                else if (r.Intersects (*startI)) {
                    RangeType newValue{min (rStart, startI->GetLowerBound ()), startI->GetUpperBound ()};
                    if (sNoisyDebugTrace_) {
                        DbgTrace ("Updating subrange element %d from %f/%f to %f/%f", fSubRanges_.IndexOf (startI),
                                  static_cast<double> (startI->GetLowerBound ()), static_cast<double> (startI->GetUpperBound ()),
                                  static_cast<double> (newValue.GetLowerBound ()), static_cast<double> (newValue.GetUpperBound ()));
                    }
                    if (*startI != newValue) {
                        fSubRanges_.Update (startI, newValue);
                        extendedRange = true;
                    }
                }
                else {
                    if (sNoisyDebugTrace_) {
                        DbgTrace ("Inserting subrange element %d before %f/%f of %f/%f", fSubRanges_.IndexOf (startI),
                                  static_cast<double> (startI->GetLowerBound ()), static_cast<double> (startI->GetUpperBound ()),
                                  static_cast<double> (r.GetLowerBound ()), static_cast<double> (r.GetUpperBound ()));
                    }
                    fSubRanges_.Insert (startI, r);
                }

                startI = findStartI (); // refresh iterator cuz container changed

                /*
                 *  Next adjust RHS of rhs-most element.
                 */
                Iterator<RangeType> endI =
                    prevOfIterator (fSubRanges_.Find (startI, [rEnd] (const RangeType& r) -> bool { return rEnd < r.GetLowerBound (); }));
                if (endI == fSubRanges_.end ()) {
                    endI = prevOfIterator (fSubRanges_.end ());
                }
                Assert (endI != fSubRanges_.end ());
                if (endI->GetLowerBound () <= rEnd) {
                    RangeType newValue{endI->GetLowerBound (), max (rEnd, endI->GetUpperBound ())};
                    if (newValue != *endI) {
                        if (sNoisyDebugTrace_) {
                            DbgTrace ("Updating RHS of subrange element %d from %f/%f to %f/%f", fSubRanges_.IndexOf (endI),
                                      static_cast<double> (endI->GetLowerBound ()), static_cast<double> (endI->GetUpperBound ()),
                                      static_cast<double> (newValue.GetLowerBound ()), static_cast<double> (newValue.GetUpperBound ()));
                        }
                        fSubRanges_.Update (endI, newValue);
                        extendedRange = true;
                    }
                }
                else {
                    if (sNoisyDebugTrace_) {
                        DbgTrace ("Appending RHS subrange element %f/%f", static_cast<double> (r.GetLowerBound ()),
                                  static_cast<double> (r.GetUpperBound ()));
                    }
                    fSubRanges_.Append (r);
                }

                startI = findStartI (); // refresh iterator cuz container changed

                // hack cuz endI invalidated - REALLY must rewrite this algorithm given new rules for iterator patching / invalidation!!!
                {
                    endI =
                        prevOfIterator (fSubRanges_.Find (startI, [rEnd] (const RangeType& r) -> bool { return rEnd < r.GetLowerBound (); }));
                    if (endI == fSubRanges_.end ()) {
                        endI = prevOfIterator (fSubRanges_.end ());
                    }
                    Assert (endI != fSubRanges_.end ());
                }

                // then merge out uneeded items in between
                // @todo/CLEANUP/REVIEW - not sure we always have startI <= endI...
                if (extendedRange and startI != fSubRanges_.end ()) {
                    for (auto i = startI; i != endI and i != fSubRanges_.end ();) {
                        if (i != startI and i != endI) {
                            if (sNoisyDebugTrace_) {
                                DbgTrace ("Removing redundant subrange element %d from %f/%f to %f/%f", fSubRanges_.IndexOf (i),
                                          static_cast<double> (i->GetLowerBound ()), static_cast<double> (i->GetUpperBound ()));
                            }
                            i = fSubRanges_.erase (i);
                        }
                        else {
                            ++i;
                        }
                    }
                }
            }
        }
        AssertInternalRepValid_ ();
        //Ensure (Contains (r); NYI so do below tmphack alternative
        Ensure (r.GetLowerBoundOpenness () == Openness::eOpen or Contains (r.GetLowerBound ()));
        Ensure (r.GetUpperBoundOpenness () == Openness::eOpen or Contains (r.GetUpperBound ()));
        Ensure (GetBounds ().Contains (r));
//Ensure (Contains (r));            DISABLE TEMPORARILY CUZ CONTAINS CONSTRUCTS (ANOTHER) NEW RANGE, CAUSING INFINITE RECURSE - ...
#if 0
        if (sNoisyDebugTrace_) {
            DbgTrace (L"MergeIn (r = %s)", r.Format ().c_str ());
        }
#endif
    }
    template <typename T, typename RANGE_TYPE>
    inline void DisjointRange<T, RANGE_TYPE>::AssertInternalRepValid_ () const
    {
        if constexpr (qDebug) {
            optional<RangeType> lastRangeSeenSoFar;
            for (const RangeType& r : fSubRanges_) {
                Assert (not r.empty ());
                if (lastRangeSeenSoFar) {
                    Assert (lastRangeSeenSoFar->GetUpperBound () <= r.GetLowerBound ()); // equal maybe bad but check that case with itersects which pays attention to openness
                    Assert (not lastRangeSeenSoFar->Intersects (r));
                    // and make sure we merge together adjacent points
                    value_type nextVal = RangeType::TraitsType::GetNext (lastRangeSeenSoFar->GetUpperBound ());
                    Assert (nextVal < r.GetLowerBound ()); // if nextval of previous item == lowerBound of successive one, we could have merged them into a contiguous run
                }
                lastRangeSeenSoFar = r;
            }
        }
    }
    template <typename T, typename RANGE_TYPE>
    constexpr bool DisjointRange<T, RANGE_TYPE>::operator== (const DisjointRange& rhs) const
    {
        return SubRanges () == rhs.SubRanges ();
    }

    /*
     ********************************************************************************
     ***************** DisjointRange<T, RANGE_TYPE> Operators ***********************
     ********************************************************************************
     */
    template <typename T, typename RANGE_TYPE>
    inline DisjointRange<T, RANGE_TYPE> operator+ (const DisjointRange<T, RANGE_TYPE>& lhs, const DisjointRange<T, RANGE_TYPE>& rhs)
    {
        return lhs.Union (rhs);
    }
    template <typename T, typename RANGE_TYPE>
    inline DisjointRange<T, RANGE_TYPE> operator^ (const DisjointRange<T, RANGE_TYPE>& lhs, const DisjointRange<T, RANGE_TYPE>& rhs)
    {
        return lhs.Intersection (rhs);
    }

}
#endif /* _Stroika_Foundation_Traversal_DisjointRange_inl_ */
