/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_MultiRowTextImager_inl_
#define _Stroika_Framework_Led_MultiRowTextImager_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::Led {

    /*
        ********************************************************************************
        ***************************** Implementation Details ***************************
        ********************************************************************************
        */
    // defined out of order cuz used earlier
    inline MultiRowTextImager::RowReference::RowReference (PartitionMarker* partitionMarker, size_t subRow)
        : fPartitionMarker (partitionMarker)
        , fSubRow (subRow)
    {
    }
    inline void MultiRowTextImager::InvalidateTotalRowsInWindow ()
    {
        fTotalRowsInWindow = 0; // zero is sentinal meaning invalid
    }
    inline MultiRowTextImager::RowReference MultiRowTextImager::GetTopRowReferenceInWindow () const
    {
        RequireNotNull (PeekAtTextStore ()); //  Must associate textstore before we can ask for row-references
        EnsureNotNull (fTopLinePartitionMarkerInWindow);
        const_cast<MultiRowTextImager*> (this)->ReValidateSubRowInTopLineInWindow ();
        return (RowReference (fTopLinePartitionMarkerInWindow, fSubRowInTopLineInWindow));
    }
    inline size_t MultiRowTextImager::GetTotalRowsInWindow_ () const
    {
        if (fTotalRowsInWindow == 0) { // cached value invalid
            fTotalRowsInWindow = ComputeRowsThatWouldFitInWindowWithTopRow (GetTopRowReferenceInWindow ());
        }
        Assert (fTotalRowsInWindow >= 1); // always have at least one row...
        Assert (fTotalRowsInWindow == ComputeRowsThatWouldFitInWindowWithTopRow (GetTopRowReferenceInWindow ()));
        return (fTotalRowsInWindow);
    }

    //  class   MultiRowTextImager::PartitionElementCacheInfo::Rep
    inline MultiRowTextImager::PartitionElementCacheInfo::Rep::Rep ()
        : fPixelHeightCache (Led_Distance (-1))
        , fInterlineSpace (0)
        , fRowCountCache (0)
        , fRowStartArray (nullptr)
        , fRowHeightArray (nullptr)
    {
    }
    inline MultiRowTextImager::PartitionElementCacheInfo::Rep::~Rep ()
    {
        if (fRowCountCache > kPackRowStartCount + 1) {
            delete[] fRowStartArray;
        }
        if (fRowCountCache > kPackRowHeightCount) {
            delete[] fRowHeightArray;
        }
    }

    //  class   MultiRowTextImager::PartitionElementCacheInfo
    inline MultiRowTextImager::PartitionElementCacheInfo::PartitionElementCacheInfo ()
        : fRep (make_shared<Rep> ())
    {
    }
    /*
    @METHOD:        MultiRowTextImager::PartitionElementCacheInfo::GetInterLineSpace
    @DESCRIPTION:   <p>Get the interline space associated with a partition. Each partition can
        have a unique interline space. These are computed by overriding the
        MultiRowTextImager::CalculateInterLineSpace () method. They are not set directly.
        </p>
    */
    inline Led_Distance MultiRowTextImager::PartitionElementCacheInfo::GetInterLineSpace () const
    {
        Assert (fRep->fInterlineSpace != Led_Distance (-1));
        return (fRep->fInterlineSpace);
    }
    inline void MultiRowTextImager::PartitionElementCacheInfo::SetInterLineSpace (Led_Distance interlineSpace)
    {
        Assert (interlineSpace != Led_Distance (-1));
        fRep->fInterlineSpace = interlineSpace;
    }

    /*
    @METHOD:        MultiRowTextImager::PartitionElementCacheInfo::GetPixelHeight
    @DESCRIPTION:   <p>Return the cached height of the given partition element. This is the sum of the
        pixel height computed in FillCache () for the actual text, and the interline space (see GetInterLineSpace ()).</p>
    */
    inline Led_Distance MultiRowTextImager::PartitionElementCacheInfo::GetPixelHeight () const
    {
        return (fRep->fPixelHeightCache + GetInterLineSpace ());
    }
    /*
    @METHOD:        MultiRowTextImager::PartitionElementCacheInfo::GetRowCount
    @DESCRIPTION:   <p>Return the cached number of rows in this partition element. This is computed in
        MultiRowTextImager::FillCache (), and specified indirectly via calls (from inside FillCache)
        to MultiRowTextImager::MultiRowPartitionMarker::IncrementRowCountAndFixCacheBuffers ().</p>
    */
    inline size_t MultiRowTextImager::PartitionElementCacheInfo::GetRowCount () const
    {
        Assert (fRep->fRowCountCache >= 1); // even for empty lines we have 1 row (by definition)
        return (fRep->fRowCountCache);
    }
    /*
    @METHOD:        MultiRowTextImager::PartitionElementCacheInfo::PeekRowCount
    @DESCRIPTION:   <p>Return the cached number of rows in this partition element. This is computed in
        MultiRowTextImager::FillCache (), and specified indirectly via calls (from inside FillCache)
        to MultiRowTextImager::MultiRowPartitionMarker::IncrementRowCountAndFixCacheBuffers ().</p>
    */
    inline size_t MultiRowTextImager::PartitionElementCacheInfo::PeekRowCount () const
    {
        return (fRep->fRowCountCache);
    }
    /*
    @METHOD:        MultiRowTextImager::PartitionElementCacheInfo::GetLastRow
    @DESCRIPTION:   <p>Return the last valid row index (may invoke FillCache if cached result not
        already available).</p>
    */
    inline size_t MultiRowTextImager::PartitionElementCacheInfo::GetLastRow () const
    {
        return GetRowCount () - 1;
    }
    /*
    @METHOD:        MultiRowTextImager::PartitionElementCacheInfo::PeekAtRowHeight
    @DESCRIPTION:   <p>Return the height - (in whatever unit the GDI is using, but typically pixels) of the given row.
        The internal representation of these things is somewhat obscure for data size reasons, so there is some
        unpacking to be done.</p>
    */
    inline Led_Distance MultiRowTextImager::PartitionElementCacheInfo::PeekAtRowHeight (size_t i) const
    {
        Assert (i < fRep->fRowCountCache); // MFC Hint - when this assert fails, look closely at your
        // stack-trace - often its cuz some other assert failed in the context
        // of a FillCache, and so the cache info isn't completely filled in
        // yet...
        /*
            *  A bit of trickery. --- XPLAIN
            */
        if (fRep->fRowCountCache <= kPackRowHeightCount) {
            // Then we use the pointer to the array as the actual array
            const RowHeight_* theArray = reinterpret_cast<const RowHeight_*> (&fRep->fRowHeightArray);
            return (theArray[i]);
        }
        AssertNotNull (fRep->fRowHeightArray);
        return (fRep->fRowHeightArray[i]);
    }
    /*
    @METHOD:        MultiRowTextImager::PartitionElementCacheInfo::SetRowHeight
    @DESCRIPTION:   <p>Set the height of a given row. This is typically just done within FillCache().</p>
    */
    inline void MultiRowTextImager::PartitionElementCacheInfo::SetRowHeight (size_t i, Led_Distance rowHeight)
    {
        Assert (i < fRep->fRowCountCache);
        Assert (sizeof (RowHeight_) > 1 or rowHeight <= 0xff);   // be sure value fits..
        Assert (sizeof (RowHeight_) > 2 or rowHeight <= 0xffff); // be sure value fits.
        /*
            *  A bit of trickery. --- XPLAIN
            */
        if (fRep->fRowCountCache <= kPackRowHeightCount) {
            // Then we use the pointer to the array as the actual array
            RowHeight_* theArray = reinterpret_cast<RowHeight_*> (&fRep->fRowHeightArray);
            theArray[i]          = RowHeight_ (rowHeight);
        }
        else {
            AssertNotNull (fRep->fRowHeightArray);
            fRep->fRowHeightArray[i] = RowHeight_ (rowHeight);
        }
    }
    /*
    @METHOD:        MultiRowTextImager::PartitionElementCacheInfo::PeekAtRowStart
    @DESCRIPTION:   <p>Return the partition element relative offset of the start of a given row. So for the
        first row, this is always zero.</p>
    */
    inline size_t MultiRowTextImager::PartitionElementCacheInfo::PeekAtRowStart (size_t i) const
    {
        Assert (i < fRep->fRowCountCache);

        if (i == 0) {
            return (0);
        }
        else {
            /*
                *  A bit of trickery. --- XPLAIN
                */
            Assert (i >= 1);
            if (fRep->fRowCountCache <= kPackRowStartCount + 1) {
                // Then we use the pointer to the array as the actual array
                const RowStart_* theArray = reinterpret_cast<const RowStart_*> (&fRep->fRowStartArray);
                return (theArray[i - 1]);
            }
            AssertNotNull (fRep->fRowStartArray);
            return (fRep->fRowStartArray[i - 1]);
        }
    }
    /*
    @METHOD:        MultiRowTextImager::PartitionElementCacheInfo::SetRowStart
    @DESCRIPTION:   <p>Set the partition element relative offset of the start of a given row. So for the
        first row, this is MUST BE zero. This is typically just called during FillCache ().</p>
    */
    inline void MultiRowTextImager::PartitionElementCacheInfo::SetRowStart (size_t i, size_t rowStart)
    {
        Assert (i < fRep->fRowCountCache);

        if (i == 0) {
            Assert (rowStart == 0);
        }
        else {
            /*
                *  A bit of trickery. --- XPLAIN
                */
            Assert (i >= 1);
            Assert (sizeof (RowStart_) > 1 or rowStart <= 0xff);   // be sure value fits..
            Assert (sizeof (RowStart_) > 2 or rowStart <= 0xffff); // be sure value fits.
            if (fRep->fRowCountCache <= kPackRowStartCount + 1) {
                // Then we use the pointer to the array as the actual array
                RowStart_* theArray = reinterpret_cast<RowStart_*> (&fRep->fRowStartArray);
                theArray[i - 1]     = RowStart_ (rowStart);
            }
            else {
                AssertNotNull (fRep->fRowStartArray);
                fRep->fRowStartArray[i - 1] = RowStart_ (rowStart);
            }
        }
    }
    inline size_t MultiRowTextImager::PartitionElementCacheInfo::GetLineRelativeRowStartPosition (size_t ithRow) const
    {
        return (PeekAtRowStart (ithRow));
    }
    inline Led_Distance MultiRowTextImager::PartitionElementCacheInfo::GetRowHeight (size_t ithRow) const
    {
        return (PeekAtRowHeight (ithRow));
    }
    inline size_t MultiRowTextImager::PartitionElementCacheInfo::LineRelativePositionInWhichRow (size_t charPos) const
    {
        //  ZERO based charPos - ie zero is just before first byte in first row
        //  Require (charPos >= 0); // yes I know this is a degenerate test - just for doc purposes...
        //  Assert (charPos < OURLENGTH);
        for (size_t row = fRep->fRowCountCache; row >= 1; row--) {
            if (charPos >= PeekAtRowStart (row - 1)) {
                return (row - 1);
            }
        }
        Assert (false);
        return (0); // if we get here - must have been before our line...
    }

    //  class   MultiRowTextImager::RowReference
    inline MultiRowTextImager::RowReference::RowReference (const RowReference& from)
        : fPartitionMarker (from.fPartitionMarker)
        , fSubRow (from.fSubRow)
    {
    }
    inline MultiRowTextImager::RowReference& MultiRowTextImager::RowReference::operator= (const MultiRowTextImager::RowReference& rhs)
    {
        fPartitionMarker = rhs.fPartitionMarker;
        fSubRow          = rhs.fSubRow;
        return (*this);
    }
    inline MultiRowTextImager::PartitionMarker* MultiRowTextImager::RowReference::GetPartitionMarker () const
    {
        return (fPartitionMarker);
    }
    inline size_t MultiRowTextImager::RowReference::GetSubRow () const
    {
        return (fSubRow);
    }

    //  class   MultiRowTextImager
    /*
    @METHOD:        MultiRowTextImager::GetNextRowReference
    @DESCRIPTION:   <p>Advance the given row reference argument to the next row. Return true if there
        is a valid next row. And false if <code>adjustMeInPlace</code> was already on the last row.</p>
            <p>See also @'MultiRowTextImager::GetPreviousRowReference'.</p>
    */
    inline bool MultiRowTextImager::GetNextRowReference (RowReference* adjustMeInPlace) const
    {
        RequireNotNull (adjustMeInPlace);
        PartitionMarker*          cur         = adjustMeInPlace->GetPartitionMarker ();
        size_t                    subRow      = adjustMeInPlace->GetSubRow ();
        PartitionElementCacheInfo pmCacheInfo = GetPartitionElementCacheInfo (cur);
        if (subRow + 1 < pmCacheInfo.GetRowCount ()) {
            subRow++;
            *adjustMeInPlace = RowReference (cur, subRow);
            return true;
        }
        else {
            if (cur->GetNext () == nullptr) {
                return false;
            }
            else {
                cur              = cur->GetNext ();
                subRow           = 0;
                *adjustMeInPlace = RowReference (cur, subRow);
                return true;
            }
        }
    }
    /*
    @METHOD:        MultiRowTextImager::GetPreviousRowReference
    @DESCRIPTION:   <p>Retreat the given row reference argument to the previous row. Return true if there
        is a valid previous row. And false if <code>adjustMeInPlace</code> was already on the first row.</p>
            <p>See also @'MultiRowTextImager::GetNextRowReference'.</p>
    */
    inline bool MultiRowTextImager::GetPreviousRowReference (RowReference* adjustMeInPlace) const
    {
        AssertNotNull (adjustMeInPlace);
        PartitionMarker* cur    = adjustMeInPlace->GetPartitionMarker ();
        size_t           subRow = adjustMeInPlace->GetSubRow ();
        if (subRow > 0) {
            subRow--;
            *adjustMeInPlace = RowReference (cur, subRow);
            return true;
        }
        else {
            if (cur->GetPrevious () == nullptr) {
                return false;
            }
            else {
                cur                                   = cur->GetPrevious ();
                PartitionElementCacheInfo pmCacheInfo = GetPartitionElementCacheInfo (cur);
                subRow                                = pmCacheInfo.GetRowCount () - 1;
                *adjustMeInPlace                      = RowReference (cur, subRow);
                return true;
            }
        }
    }
    /*
    @METHOD:        MultiRowTextImager::GetIthRowReferenceFromHere
    @DESCRIPTION:   <p>Adjust <code>fromHere</code> by <code>ith</code> rows. If <code>ith</code> is zero, then
        this returns <code>fromHere</code>. It asserts there are at least ith rows from the given one to retrieve.
        It calls @'MultiRowTextImager::GetIthRowReferenceFromHere' todo its work (which returns a bool rather than asserting).</p>
            <p>See also @'MultiRowTextImager::GetNextRowReference', @'MultiRowTextImager::GetPreviousRowReference'.</p>
    */
    inline MultiRowTextImager::RowReference MultiRowTextImager::GetIthRowReferenceFromHere (RowReference fromHere, ptrdiff_t ith) const
    {
        [[maybe_unused]] bool result = GetIthRowReferenceFromHere (&fromHere, ith);
        Assert (result);
        return fromHere;
    }
    /*
    @METHOD:        MultiRowTextImager::GetIthRowReference
    @DESCRIPTION:   <p>Get the <code>ith</code> row reference in the document. Asserts value <code>ith</code> refers to
        a valid row number.</p>
            <p>It calls @'MultiRowTextImager::GetIthRowReferenceFromHere' todo its work (which returns a bool rather than asserting).</p>
    */
    inline MultiRowTextImager::RowReference MultiRowTextImager::GetIthRowReference (size_t ith) const
    {
        RowReference          fromHere (GetFirstPartitionMarker (), 0);
        [[maybe_unused]] bool result = GetIthRowReferenceFromHere (&fromHere, ith);
        Assert (result);
        return fromHere;
    }
    /*
    @METHOD:        MultiRowTextImager::GetRowLength
    @DESCRIPTION:   <p>Gets the length of the given row (in @'Led_tChar's).</p>
            <p>See also  @'MultiRowTextImager::GetStartOfRow' and @'MultiRowTextImager::GetEndOfRow'.</p>
    */
    inline size_t MultiRowTextImager::GetRowLength (RowReference row) const
    {
        return (GetEndOfRow (row) - GetStartOfRow (row));
    }
    /*
    @METHOD:        MultiRowTextImager::GetLastRowReferenceInWindow
    @DESCRIPTION:   <p>Returns the last row-reference in the window (end of window).</p>
    */
    inline MultiRowTextImager::RowReference MultiRowTextImager::GetLastRowReferenceInWindow () const
    {
        RowReference row = GetTopRowReferenceInWindow ();
        Assert (GetTotalRowsInWindow_ () >= 1);
        (void)GetIthRowReferenceFromHere (&row, GetTotalRowsInWindow_ () - 1);
        return (row);
    }
    inline void MultiRowTextImager::SetTopRowInWindow_ (RowReference row)
    {
        fTopLinePartitionMarkerInWindow = row.GetPartitionMarker ();
        fSubRowInTopLineInWindow        = row.GetSubRow ();
        AssertNotNull (fTopLinePartitionMarkerInWindow);
        InvalidateTotalRowsInWindow ();
    }
    /*
    @METHOD:        MultiRowTextImager::GetRowHeight
    @DESCRIPTION:   <p>Returns the height (in standard GDI units, usually pixels) of the given row reference.</p>
    */
    inline Led_Distance MultiRowTextImager::GetRowHeight (RowReference row) const
    {
        AssertNotNull (row.GetPartitionMarker ());
        return GetPartitionElementCacheInfo (row.GetPartitionMarker ()).GetRowHeight (row.GetSubRow ());
    }

    inline bool operator== (MultiRowTextImager::RowReference lhs, MultiRowTextImager::RowReference rhs)
    {
        return (lhs.GetPartitionMarker () == rhs.GetPartitionMarker () and
                lhs.GetSubRow () == rhs.GetSubRow ());
    }
    inline bool operator!= (MultiRowTextImager::RowReference lhs, MultiRowTextImager::RowReference rhs)
    {
        return (lhs.GetPartitionMarker () != rhs.GetPartitionMarker () or
                lhs.GetSubRow () != rhs.GetSubRow ());
    }

}

#endif /*_Stroika_Framework_Led_MultiRowTextImager_inl_*/
