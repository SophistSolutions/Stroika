/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DisjointRange_inl_
#define _Stroika_Foundation_Traversal_DisjointRange_inl_


namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /*
            ********************************************************************************
            ************************* DisjointRange<RANGE_TYPE> ****************************
            ********************************************************************************
            */
            template    <typename RANGE_TYPE>
            DisjointRange<RANGE_TYPE>::DisjointRange (const RangeType& from)
                : fSubRanges_ {} {
                MergeIn_ (from);
            }
            template    <typename RANGE_TYPE>
            inline  DisjointRange<RANGE_TYPE>::DisjointRange (const initializer_list<RangeType>& from)
                : DisjointRange { begin (from), end (from) } {
            }
            template    <typename RANGE_TYPE>
            template    <typename CONTAINER_OF_RANGE_OF_T>
            inline  DisjointRange<RANGE_TYPE>::DisjointRange (const CONTAINER_OF_RANGE_OF_T& from)
                : DisjointRange { begin (from), end (from) } {
            }
            template    <typename RANGE_TYPE>
            template    <typename COPY_FROM_ITERATOR_OF_RANGE_OF_T>
            DisjointRange<RANGE_TYPE>::DisjointRange (COPY_FROM_ITERATOR_OF_RANGE_OF_T start, COPY_FROM_ITERATOR_OF_RANGE_OF_T end)
                : fSubRanges_ {} {
                for (auto i = start; i != end; ++i)
                {
                    MergeIn_ (*i);
                }
            }
            template    <typename RANGE_TYPE>
            inline  auto    DisjointRange<RANGE_TYPE>::GetSubRanges () const -> Containers::Sequence<RangeType> {
                return fSubRanges_;
            }
            template    <typename RANGE_TYPE>
            DisjointRange<RANGE_TYPE> DisjointRange<RANGE_TYPE>::FullRange ()
            {
                return DisjointRange<RangeType> { RangeType::FullRange () };
            }
            template    <typename RANGE_TYPE>
            inline  bool    DisjointRange<RANGE_TYPE>::empty () const
            {
                return fSubRanges_.empty ();
            }
            template    <typename RANGE_TYPE>
            bool    DisjointRange<RANGE_TYPE>::Contains (ElementType elt) const
            {
                return fSubRanges_.FindFirstThat ([elt] (RangeType r) { return r.Contains (elt); });
            }
            template    <typename RANGE_TYPE>
            RANGE_TYPE   DisjointRange<RANGE_TYPE>::GetBounds () const
            {
                size_t  n   =   fSubRanges_.size ();
                switch (n) {
                    case 0:
                        return RangeType {};
                    case 1:
                        return fSubRanges_[0];
                    default:
                        return RangeType (fSubRanges_[0].GetLowerBound (), fSubRanges_.GetLast ().GetUpperBound ());
                }
            }
            template    <typename RANGE_TYPE>
            void    DisjointRange<RANGE_TYPE>::MergeIn_ (const RangeType& r)
            {
                AssertInternalRepValid_ ();
                if (not r.empty ()) {
                    ElementType         rStart  { r.GetLowerBound () };
                    ElementType         rEnd    { r.GetUpperBound () };
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
                            for (Iterator<RangeType> i = fSubRanges_.begin (); i != fSubRanges_.end (); ++i)
                            {
                                if (i == pOfI) {
                                    return result;
                                }
                                result = i;
                            }
                            return result;
                        };

                        Iterator<RangeType> startI   =   fSubRanges_.FindFirstThat ([rStart] (const RangeType & r) -> bool {return r.GetLowerBound () >= rStart or r.Contains (rStart); });
                        if (startI == fSubRanges_.end ()) {
                            DbgTrace ("Appending subrange cuz this is past the rest: %f/%f",
                                      static_cast<double> (r.GetLowerBound ()), static_cast<double> (r.GetUpperBound ())
                                     );
                            // cuz this means no ranges to the right containing rStart
                            //
                            // when appending, we can sometimes extend the last item
                            ElementType prevVal = RangeType::TraitsType::GetPrevious (rStart);
                            Iterator<RangeType> i = fSubRanges_.FindFirstThat ([prevVal] (const RangeType & r) -> bool {return r.GetUpperBound () == prevVal; });
                            if (i) {
                                Assert (i->GetUpperBound () == prevVal);
                                RangeType newValue { i->GetLowerBound (), rStart };
                                fSubRanges_.Update (i, newValue);
                            }
                            else {
                                fSubRanges_.Append (r);
                            }
                        }
                        else if (r.Intersects (*startI)) {
                            RangeType newValue { min (rStart, startI->GetLowerBound ()), startI->GetUpperBound () };
                            DbgTrace ("Updating subrange element %d from %f/%f to %f/%f",
                                      fSubRanges_.IndexOf (startI),
                                      static_cast<double> (startI->GetLowerBound ()), static_cast<double> (startI->GetUpperBound ()),
                                      static_cast<double> (newValue.GetLowerBound ()), static_cast<double> (newValue.GetUpperBound ())
                                     );
                            if (*startI != newValue) {
                                fSubRanges_.Update (startI, newValue);
                            }
                        }
                        else {
                            DbgTrace ("Inserting subrange element %d from %f/%f to %f/%f",
                                      fSubRanges_.IndexOf (startI),
                                      static_cast<double> (startI->GetLowerBound ()), static_cast<double> (startI->GetUpperBound ()),
                                      static_cast<double> (r.GetLowerBound ()), static_cast<double> (r.GetUpperBound ())
                                     );
                            fSubRanges_.Insert (startI, r);
                        }

                        /*
                         *  Next adjust RHS of rhs-most element.
                         */
                        Iterator<RangeType> endI = prevOfIterator (fSubRanges_.FindFirstThat (startI, [rEnd] (const RangeType & r) -> bool {return rEnd < r.GetLowerBound ();}));
                        if (endI == fSubRanges_.end ()) {
                            endI = prevOfIterator (fSubRanges_.end ());
                        }
                        Assert (endI != fSubRanges_.end ());
                        if (endI->GetLowerBound () <= rEnd) {
                            RangeType newValue { endI->GetLowerBound (), max (rEnd, endI->GetUpperBound ()) };
                            DbgTrace ("Updating RHS of subrange element %d from %f/%f to %f/%f",
                                      fSubRanges_.IndexOf (endI),
                                      static_cast<double> (endI->GetLowerBound ()), static_cast<double> (endI->GetUpperBound ()),
                                      static_cast<double> (newValue.GetLowerBound ()), static_cast<double> (newValue.GetUpperBound ())
                                     );
                            fSubRanges_.Update (endI, newValue);
                        }
                        else {
                            DbgTrace ("Appending RHS subrange element %f/%f",
                                      static_cast<double> (r.GetLowerBound ()), static_cast<double> (r.GetUpperBound ())
                                     );
                            fSubRanges_.Append (r);
                        }

                        // then merge out uneeded items in between
                        // @todo/CLEANUP/REVIEW - not sure we always have startI <= endI...
                        if (startI != fSubRanges_.end ()) {
                            for (auto i = startI; i != endI and i != fSubRanges_.end (); ++i) {
                                if (i != startI and i != endI) {
                                    DbgTrace ("Removing redundant subrange element %d from %f/%f to %f/%f",
                                              fSubRanges_.IndexOf (i),
                                              static_cast<double> (i->GetLowerBound ()), static_cast<double> (i->GetUpperBound ())
                                             );
                                    fSubRanges_.Remove (i);
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
            }
            template    <typename RANGE_TYPE>
            inline  void    DisjointRange<RANGE_TYPE>::AssertInternalRepValid_ ()
            {
#if     qDebug
                Memory::Optional<RangeType> lastRangeSeenSoFar;
                for (RangeType r : fSubRanges_) {
                    Assert (not r.empty ());
                    if (lastRangeSeenSoFar) {
                        Assert (lastRangeSeenSoFar->GetUpperBound () <= r.GetLowerBound ());    // equal maybe bad but check that case with itersects which pays attention to openness
                        Assert (not lastRangeSeenSoFar->Intersects (r));
                        // and make sure we merge together adjacent points
                        ElementType nextVal = RangeType::TraitsType::GetNext (lastRangeSeenSoFar->GetUpperBound ());
                        Assert (nextVal < r.GetLowerBound ());
                    }
                    lastRangeSeenSoFar = r;
                }
#endif
            }


        }
    }
}
#endif /* _Stroika_Foundation_Traversal_DisjointRange_inl_ */
