/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_SimpleTextImager_h_
#define _Stroika_Frameworks_Led_SimpleTextImager_h_ 1

/*
@MODULE:    SimpleTextImager
@DESCRIPTION:
        <p>The SimpleTextImager module provides a very fast, lightweight text imaging implementation. This implementation
    does not support wordwrapping (or multiple rows per line (Partition element)</p>

 */

#include "../../Foundation/StroikaPreComp.h"

#include <limits.h> // for UINT_MAX
#include <string.h>

#include "PartitioningTextImager.h"

namespace Stroika::Frameworks::Led {

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250) // inherits via dominance warning
#endif

    /*
    @CLASS:         SimpleTextImager
    @BASES:         @'PartitioningTextImager'
    @DESCRIPTION:
            <p>SimpleTextImager is a simple subclass of @'PartitioningTextImager'. NB: this is a change
        from Led 2.2, when as an implementation detail, it subclassed from MultiRowTextImager.</p>
            <p>SimpleTextImager assumes that all rows have the same height (specified by ReCalcRowHeight)
        and that all lines (paragraphs) are displayed in a single row. It is these assumptions that allow
        this class to display text with much less runtime overhead (both memory and time).</p>
            <p>You should be able to assign an arbitrary partition object to a @'SimpleTextImager' via
        the @'SimpleTextImager::SetPartition' function. And these partition objects should be sharable among
        multiple SimpleTextImager's.</p>
            <p>NB:  Much of this class is subject to change (mostly simplication). This code was originally
        based on the code for MultiRowTextImager, and as such, it has alot of generality built in
        which isn't appropriate (like assuming rows all same height allows alot to be simplfied in
        computations etc).</p>
            <p>Unless you must, avoid counting too much on the internals and details of this class.</p>
    */
    class SimpleTextImager : public PartitioningTextImager {
    private:
        using inherited = PartitioningTextImager;

    protected:
        SimpleTextImager ();
        virtual ~SimpleTextImager ();

    private:
        SimpleTextImager (const SimpleTextImager&);          // don't call. not implemented
        nonvirtual void operator= (const SimpleTextImager&); // don't call. not implemented

    protected:
        virtual void    HookLosingTextStore () override;
        nonvirtual void HookLosingTextStore_ ();
        virtual void    HookGainedNewTextStore () override;
        nonvirtual void HookGainedNewTextStore_ ();

    public:
        nonvirtual void SetPartition (const PartitionPtr& partitionPtr);

    private:
        bool fICreatedPartition;

    public:
        virtual PartitionPtr MakeDefaultPartition () const override;

    private:
        class MyPartitionWatcher : public Partition::PartitionWatcher {
        public:
            nonvirtual void Init (PartitionPtr partition, SimpleTextImager* imager);
            nonvirtual void UnInit (PartitionPtr partition);

        public:
            virtual void AboutToSplit (PartitionMarker* pm, size_t at, void** infoRecord) const noexcept override;
            virtual void DidSplit (void* infoRecord) const noexcept override;
            virtual void AboutToCoalece (PartitionMarker* pm, void** infoRecord) const noexcept override;
            virtual void DidCoalece (void* infoRecord) const noexcept override;

        private:
            SimpleTextImager* fImager;
        };

    private:
        friend class MyPartitionWatcher;
        MyPartitionWatcher fMyPartitionWatcher;

    public:
        nonvirtual Led_Distance GetRowHeight () const;

    protected:
        nonvirtual void      InvalidateRowHeight ();
        virtual Led_Distance ReCalcRowHeight () const;

    private:
        Led_Distance fRowHeight;

    protected:
        virtual Led_Distance MeasureSegmentHeight (size_t from, size_t to) const override;
        virtual Led_Distance MeasureSegmentBaseLine (size_t from, size_t to) const override;

    protected:
        using PartitionMarker = Partition::PartitionMarker;

    public:
        /*
        @CLASS:         SimpleTextImager::RowReference
        @DESCRIPTION:
                <p>A utility class to represent a row. It is a struct with a parition marker, and a row number.
                These things are NOT long-lived. And shouldn't be saved anyplace, as no attempt is made to keep
                them automaticlly up to date as the text is modified.
                They are just a convenient, short-hand way to navigate through rows of text.</p>
                <p>NOTE - this RowReference stuff is just a design vestige from an earlier implementation. At some point,
            this module should be rewritten/cleaned up to eliminate this (LGP - 2001-10-20).
        */
        class RowReference {
        public:
            RowReference (const RowReference& from);
            RowReference (PartitionMarker* partitionMarker);

        private:
            RowReference (); // left undefined to assure never called...
        public:
            nonvirtual RowReference& operator= (const RowReference& rhs);

        public:
            nonvirtual PartitionMarker* GetPartitionMarker () const;

        private:
            PartitionMarker* fPartitionMarker;
        };

        // Row Reference support routines...
    public:
        nonvirtual bool GetNextRowReference (RowReference* adjustMeInPlace) const;     // return true if there is a next, and false if at end
        nonvirtual bool GetPreviousRowReference (RowReference* adjustMeInPlace) const; // return true if there is a previous, and false if at the beginning

        // NB: if ith==1, that means do NOTHING - for convenience...
        nonvirtual bool GetIthRowReferenceFromHere (RowReference* adjustMeInPlace, ptrdiff_t ith) const; // return true if there is an ith, and false if we run off end... (ith==0 implies no change, < 0 means go back)
        nonvirtual RowReference GetIthRowReferenceFromHere (RowReference fromHere, ptrdiff_t ith) const; // ERROR if ith doesn't exist... (ith==0 implies no change, < 0 means go back)
        nonvirtual RowReference GetIthRowReference (size_t ith) const;                                   // ERROR if ith doesn't exist...(1 th is first row)

        nonvirtual size_t GetRowNumber (RowReference rowRef) const; // Use of row numbers is discouraged, but this routine
        // can be helpful in implementing those APIs anyhow

        // Count the # of rows from one rowreference to the other (order doesn't matter)
        nonvirtual size_t CountRowDifference (RowReference lhs, RowReference rhs) const;

        /*
            *  Window/Scrolling support.
            */
    public:
        virtual size_t    GetTopRowInWindow () const override;
        virtual size_t    GetTotalRowsInWindow () const override;
        virtual size_t    GetLastRowInWindow () const override;
        virtual void      SetTopRowInWindow (size_t newTopRow) override;
        virtual size_t    GetMarkerPositionOfStartOfWindow () const override;
        virtual size_t    GetMarkerPositionOfEndOfWindow () const override;
        virtual size_t    GetMarkerPositionOfStartOfLastRowOfWindow () const override;
        virtual ptrdiff_t CalculateRowDeltaFromCharDeltaFromTopOfWindow (long deltaChars) const override;
        virtual ptrdiff_t CalculateCharDeltaFromRowDeltaFromTopOfWindow (ptrdiff_t deltaRows) const override;
        virtual void      ScrollByIfRoom (ptrdiff_t downByRows); // if downBy negative then up
        // OK to ask to scroll further
        // than allowed - return true
        // if any scrolling (not necesarily
        // same amont requested) done
    public:
        virtual void ScrollSoShowing (size_t markerPos, size_t andTryToShowMarkerPos = 0) override;

    protected:
        nonvirtual RowReference GetTopRowReferenceInWindow () const;
        nonvirtual RowReference GetLastRowReferenceInWindow () const;
        virtual void            SetTopRowInWindow (RowReference row);

    protected:
        nonvirtual void SetTopRowInWindow_ (RowReference row); // just sets the fields without any hook functions
        // getting called. This is important sometimes when
        // it would be unsafe for subclasses to get a chance
        // to call methods while our data structures are not
        // not completely up-to-date.

    protected:
        virtual void AssureWholeWindowUsedIfNeeded () override;

    public:
        virtual Led_Distance ComputeMaxHScrollPos () const override;

    public:
        virtual Led_Rect GetCharLocation (size_t afterPosition) const override;
        virtual size_t   GetCharAtLocation (const Led_Point& where) const override;
        virtual Led_Rect GetCharWindowLocation (size_t afterPosition) const override;
        virtual size_t   GetCharAtWindowLocation (const Led_Point& where) const override;

    public:
        virtual size_t   GetStartOfRow (size_t rowNumber) const override;
        virtual size_t   GetStartOfRowContainingPosition (size_t charPosition) const override;
        virtual size_t   GetEndOfRow (size_t rowNumber) const override;
        virtual size_t   GetEndOfRowContainingPosition (size_t charPosition) const override;
        virtual size_t   GetRealEndOfRow (size_t rowNumber) const override;
        virtual size_t   GetRealEndOfRowContainingPosition (size_t charPosition) const override;
        virtual size_t   GetRowContainingPosition (size_t charPosition) const override;
        virtual size_t   GetRowCount () const override;
        virtual Led_Rect GetCharLocationRowRelativeByPosition (size_t afterPosition, size_t positionOfTopRow, size_t maxRowsToCheck) const override;

    public:
        nonvirtual size_t GetStartOfRow (RowReference row) const;
        nonvirtual size_t GetEndOfRow (RowReference row) const;
        nonvirtual size_t GetRealEndOfRow (RowReference row) const;
        nonvirtual RowReference GetRowReferenceContainingPosition (size_t charPosition) const;
        nonvirtual size_t GetRowLength (RowReference row) const;

    public:
        virtual Led_Distance GetRowHeight (size_t rowNumber) const override;

    public:
        virtual Led_Distance GetRowRelativeBaselineOfRowContainingPosition (size_t charPosition) const override;

    public:
        nonvirtual Led_Distance GetHeightOfRows (size_t startingRow, size_t rowCount) const;
        nonvirtual Led_Distance GetHeightOfRows (RowReference startingRow, size_t rowCount) const;

    public:
        virtual void GetStableTypingRegionContaingMarkerRange (size_t fromMarkerPos, size_t toMarkerPos,
                                                               size_t* expandedFromMarkerPos, size_t* expandedToMarkerPos) const override;

    public:
        virtual void Draw (const Led_Rect& subsetToDraw, bool printing) override;

    public:
        virtual void DrawPartitionElement (PartitionMarker* pm, size_t startSubRow, size_t maxSubRow, Led_Tablet tablet, OffscreenTablet* offscreenTablet, bool printing, const Led_Rect& subsetToDraw, Led_Rect* remainingDrawArea, size_t* rowsDrawn);

    protected:
        virtual Led_Rect GetCharLocationRowRelative (size_t afterPosition, RowReference topRow, size_t maxRowsToCheck = UINT_MAX) const;
        virtual size_t   GetCharAtLocationRowRelative (const Led_Point& where, RowReference topRow, size_t maxRowsToCheck = UINT_MAX) const;

    public:
        nonvirtual Led_Distance GetInterLineSpace () const;
        nonvirtual void         SetInterLineSpace (Led_Distance interlineSpace);

    private:
        Led_Distance fInterlineSpace;

    public:
        virtual Led_Distance GetInterLineSpace (PartitionMarker* pm) const;
        virtual void         ChangedInterLineSpace (PartitionMarker* pm);

        // Hook to invalidate cached info based on fontmetrics
    public:
        virtual void SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont) override;

        // To assure our top-line scroll info not left corrupt...
    protected:
        virtual void DidUpdateText (const UpdateInfo& updateInfo) noexcept override;

        // override to invalidate caches.
    public:
        virtual void SetWindowRect (const Led_Rect& windowRect) override;

    protected:
        virtual void InvalidateAllCaches () override;

    private:
        nonvirtual RowReference AdjustPotentialTopRowReferenceSoWholeWindowUsed (const RowReference& potentialTopRow);
        nonvirtual bool         PositionWouldFitInWindowWithThisTopRow (size_t markerPos, const RowReference& newTopRow);

    private:
        PartitionMarker* fTopLinePartitionMarkerInWindow;

        // Support for GetTotalRowsInWindow
        //
        // Override ComputeRowsThatWouldFitInWindowWithTopRow () to change the policy of how we
        // pack rows into a window
    private:
        mutable size_t fTotalRowsInWindow; // zero means invalid cached - fill cache on call to GetTotalRowsInWindow
    protected:
        nonvirtual size_t GetTotalRowsInWindow_ () const;
        nonvirtual void   InvalidateTotalRowsInWindow ();
        virtual size_t    ComputeRowsThatWouldFitInWindowWithTopRow (const RowReference& newTopRow) const;

    protected:
        virtual bool   ContainsMappedDisplayCharacters (const Led_tChar* text, size_t nTChars) const override;
        virtual size_t RemoveMappedDisplayCharacters (Led_tChar* copyText, size_t nTChars) const override;

    private:
        friend class RowReference;
        friend bool operator== (RowReference lhs, RowReference rhs);
        friend bool operator!= (RowReference lhs, RowReference rhs);
    };

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif

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

#endif /*_Stroika_Frameworks_Led_SimpleTextImager_h_*/
