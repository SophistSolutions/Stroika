/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_SimpleTextImager_inl_
#define _Stroika_Framework_Led_SimpleTextImager_inl_ 1

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
    inline SimpleTextImager::RowReference::RowReference (PartitionMarker* partitionMarker)
        : fPartitionMarker (partitionMarker)
    {
    }
    inline void SimpleTextImager::InvalidateTotalRowsInWindow ()
    {
        fTotalRowsInWindow = 0; // zero is sentinal meaning invalid
    }
    inline SimpleTextImager::RowReference SimpleTextImager::GetTopRowReferenceInWindow () const
    {
        RequireNotNull (PeekAtTextStore ()); //  Must associate textstore before we can ask for row-references
        EnsureNotNull (fTopLinePartitionMarkerInWindow);
        return (RowReference (fTopLinePartitionMarkerInWindow));
    }
    inline size_t SimpleTextImager::GetTotalRowsInWindow_ () const
    {
        if (fTotalRowsInWindow == 0) { // cached value invalid
            fTotalRowsInWindow = ComputeRowsThatWouldFitInWindowWithTopRow (GetTopRowReferenceInWindow ());
        }
        Assert (fTotalRowsInWindow >= 1); // always have at least one row...
        Assert (fTotalRowsInWindow == ComputeRowsThatWouldFitInWindowWithTopRow (GetTopRowReferenceInWindow ()));
        return (fTotalRowsInWindow);
    }

    //  class   SimpleTextImager::RowReference
    inline SimpleTextImager::RowReference::RowReference (const RowReference& from)
        : fPartitionMarker (from.fPartitionMarker)
    {
    }
    inline SimpleTextImager::RowReference& SimpleTextImager::RowReference::operator= (const SimpleTextImager::RowReference& rhs)
    {
        fPartitionMarker = rhs.fPartitionMarker;
        return (*this);
    }
    inline SimpleTextImager::PartitionMarker* SimpleTextImager::RowReference::GetPartitionMarker () const
    {
        return (fPartitionMarker);
    }

    //  class   SimpleTextImager
    /*
    @METHOD:        SimpleTextImager::GetRowHeight
    @DESCRIPTION:   <p>Gets the row height for all rows in the Imager. Note that for SimpleTextImager, all rows have
                the same height. This function caches the value (computed by @'SimpleTextImager::ReCalcRowHeight' ()). To cause
                the value to be recomputed, call @'SimpleTextImager::InvalidateRowHeight' (). This is automatically done for you
                from </p>
    */
    inline Led_Distance SimpleTextImager::GetRowHeight () const
    {
        if (fRowHeight == Led_Distance (-1)) {
            // use mutable when available
            const_cast<SimpleTextImager*> (this)->fRowHeight = ReCalcRowHeight ();
        }
        Ensure (fRowHeight > 0);
        Ensure (fRowHeight != Led_Distance (-1));
        return fRowHeight;
    }
    /*
    @METHOD:        SimpleTextImager::InvalidateRowHeight
    @DESCRIPTION:   <p>Note that the rowHeight associated with this TextImager is invalid. Next time the value is
                requested (via @'SimpleTextImager::GetRowHeight' ()), recalculate it first.</p>
    */
    inline void SimpleTextImager::InvalidateRowHeight ()
    {
        fRowHeight = Led_Distance (-1);
    }
    /*
    @METHOD:        SimpleTextImager::GetNextRowReference
    @DESCRIPTION:   <p>Advance the given row reference argument to the next row. Return true if there
                is a valid next row. And false if <code>adjustMeInPlace</code> was already on the last row.</p>
                    <p>See also @'SimpleTextImager::GetPreviousRowReference'.</p>
    */
    inline bool SimpleTextImager::GetNextRowReference (RowReference* adjustMeInPlace) const
    {
        RequireNotNull (adjustMeInPlace);
        PartitionMarker* cur    = adjustMeInPlace->GetPartitionMarker ();
        size_t           subRow = 0;

        if (subRow + 1 < 1) {
            subRow++;
            *adjustMeInPlace = RowReference (cur);
            return true;
        }
        else {
            if (cur->GetNext () == NULL) {
                return false;
            }
            else {
                cur              = cur->GetNext ();
                subRow           = 0;
                *adjustMeInPlace = RowReference (cur);
                return true;
            }
        }
    }
    /*
    @METHOD:        SimpleTextImager::GetPreviousRowReference
    @DESCRIPTION:   <p>Retreat the given row reference argument to the previous row. Return true if there
                is a valid previous row. And false if <code>adjustMeInPlace</code> was already on the first row.</p>
                    <p>See also @'SimpleTextImager::GetNextRowReference'.</p>
    */
    inline bool SimpleTextImager::GetPreviousRowReference (RowReference* adjustMeInPlace) const
    {
        AssertNotNull (adjustMeInPlace);
        PartitionMarker* cur    = adjustMeInPlace->GetPartitionMarker ();
        size_t           subRow = 0;

        if (subRow > 0) {
            subRow--;
            *adjustMeInPlace = RowReference (cur);
            return true;
        }
        else {
            if (cur->GetPrevious () == NULL) {
                return false;
            }
            else {
                cur              = cur->GetPrevious ();
                subRow           = 0;
                *adjustMeInPlace = RowReference (cur);
                return true;
            }
        }
    }
    /*
    @METHOD:        SimpleTextImager::GetIthRowReferenceFromHere
    @DESCRIPTION:   <p>Adjust <code>fromHere</code> by <code>ith</code> rows. If <code>ith</code> is zero, then
                this returns <code>fromHere</code>. It asserts there are at least ith rows from the given one to retrieve.
                It calls @'SimpleTextImager::GetIthRowReferenceFromHere' todo its work (which returns a bool rather than asserting).</p>
                    <p>See also @'SimpleTextImager::GetNextRowReference', @'SimpleTextImager::GetPreviousRowReference'.</p>
    */
    inline SimpleTextImager::RowReference SimpleTextImager::GetIthRowReferenceFromHere (RowReference fromHere, ptrdiff_t ith) const
    {
        [[maybe_unused]] bool result = GetIthRowReferenceFromHere (&fromHere, ith);
        Assert (result);
        return (fromHere);
    }
    /*
    @METHOD:        SimpleTextImager::GetIthRowReference
    @DESCRIPTION:   <p>Get the <code>ith</code> row reference in the document. Asserts value <code>ith</code> refers to
                a valid row number.</p>
                    <p>It calls @'SimpleTextImager::GetIthRowReferenceFromHere' todo its work (which returns a bool rather than asserting).</p>
    */
    inline SimpleTextImager::RowReference SimpleTextImager::GetIthRowReference (size_t ith) const
    {
        RowReference          fromHere ((PartitionMarker*)GetFirstPartitionMarker ());
        [[maybe_unused]] bool result = GetIthRowReferenceFromHere (&fromHere, ith);
        Assert (result);
        return fromHere;
    }
    /*
    @METHOD:        SimpleTextImager::GetRowLength
    @DESCRIPTION:   <p>Gets the length of the given row (in @'Led_tChar's).</p>
                    <p>See also  @'SimpleTextImager::GetStartOfRow' and @'SimpleTextImager::GetEndOfRow'.</p>
    */
    inline size_t SimpleTextImager::GetRowLength (RowReference row) const
    {
        return (GetEndOfRow (row) - GetStartOfRow (row));
    }
    /*
    @METHOD:        SimpleTextImager::GetLastRowReferenceInWindow
    @DESCRIPTION:   <p>Returns the last row-reference in the window (end of window).</p>
    */
    inline SimpleTextImager::RowReference SimpleTextImager::GetLastRowReferenceInWindow () const
    {
        RowReference row = GetTopRowReferenceInWindow ();
        Assert (GetTotalRowsInWindow_ () >= 1);
        (void)GetIthRowReferenceFromHere (&row, GetTotalRowsInWindow_ () - 1);
        return (row);
    }
    inline void SimpleTextImager::SetTopRowInWindow_ (RowReference row)
    {
        fTopLinePartitionMarkerInWindow = row.GetPartitionMarker ();
        AssertNotNull (fTopLinePartitionMarkerInWindow);
        InvalidateTotalRowsInWindow ();
    }
    /*
    @METHOD:        SimpleTextImager::GetInterLineSpace
    @DESCRIPTION:   <p>Get the interline space associated with a SimpleTextImager. Set the per-buffer value with
                @'SimpleTextImager::GetInterLineSpace'.</p>
                    <p>NB: There is also a one-arg version of @'SimpleTextImager::GetInterLineSpace' which takes a PM
                as arg. See its docs, or @'SimpleTextImager::ChangedInterLineSpace' for more details.</p>
    */
    inline Led_Distance SimpleTextImager::GetInterLineSpace () const
    {
        return (fInterlineSpace);
    }

    inline bool operator== (SimpleTextImager::RowReference lhs, SimpleTextImager::RowReference rhs)
    {
        return (lhs.GetPartitionMarker () == rhs.GetPartitionMarker ());
    }
    inline bool operator!= (SimpleTextImager::RowReference lhs, SimpleTextImager::RowReference rhs)
    {
        return (lhs.GetPartitionMarker () != rhs.GetPartitionMarker ());
    }

}

#endif /*_Stroika_Framework_Led_SimpleTextImager_inl_*/
