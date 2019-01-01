/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../Foundation/StroikaPreComp.h"

#include <climits>

#include "../../Foundation/Memory/SmallStackBuffer.h"

#include "GDI.h"
#include "Support.h"

#include "MultiRowTextImager.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;

/*
 ********************************************************************************
 ********************************* MultiRowTextImager ***************************
 ********************************************************************************
 */
MultiRowTextImager::MultiRowTextImager ()
    : inherited ()
    , fPMCacheMgr ()
    , fTopLinePartitionMarkerInWindow (nullptr)
    , fSubRowInTopLineInWindow (0)
    , fTotalRowsInWindow (0) // value must be computed
{
}

MultiRowTextImager::~MultiRowTextImager ()
{
    Assert (fTopLinePartitionMarkerInWindow == nullptr);
}

void MultiRowTextImager::HookLosingTextStore ()
{
    inherited::HookLosingTextStore ();
    HookLosingTextStore_ ();
}

void MultiRowTextImager::HookLosingTextStore_ ()
{
    SetPartition (PartitionPtr ());
}

void MultiRowTextImager::HookGainedNewTextStore ()
{
    inherited::HookGainedNewTextStore ();
    HookGainedNewTextStore_ ();
}

void MultiRowTextImager::HookGainedNewTextStore_ ()
{
    if (GetPartition ().get () == nullptr) {
        SetPartition (MakeDefaultPartition ());
    }
}

void MultiRowTextImager::SetPartition (const PartitionPtr& partitionPtr)
{
    fPMCacheMgr.reset ();
    inherited::SetPartition (partitionPtr);
    if (partitionPtr.get () == nullptr) {
        InvalidateTotalRowsInWindow ();
        fTopLinePartitionMarkerInWindow = nullptr;
    }
    else {
        fPMCacheMgr                     = unique_ptr<PMInfoCacheMgr> (new PMInfoCacheMgr (*this));
        fTopLinePartitionMarkerInWindow = GetFirstPartitionMarker ();
        //      ReValidateSubRowInTopLineInWindow ();
        InvalidateTotalRowsInWindow ();
        AssureWholeWindowUsedIfNeeded ();
        InvalidateScrollBarParameters (); // even if we don't change the top row, we might change enuf about the text to change sbar
    }
}

PartitioningTextImager::PartitionPtr MultiRowTextImager::MakeDefaultPartition () const
{
    return PartitionPtr (new LineBasedPartition (GetTextStore ()));
}

MultiRowTextImager::PartitionElementCacheInfo MultiRowTextImager::GetPartitionElementCacheInfo (Partition::PartitionMarker* pm) const
{
    return fPMCacheMgr->GetPartitionElementCacheInfo (pm);
}

MultiRowTextImager::PartitionElementCacheInfo MultiRowTextImager::GetPartitionElementCacheInfo (MultiRowTextImager::RowReference row) const
{
    return GetPartitionElementCacheInfo (row.GetPartitionMarker ());
}

bool MultiRowTextImager::GetIthRowReferenceFromHere (RowReference* adjustMeInPlace, ptrdiff_t ith) const
{
    for (; ith > 0; ith--) {
        if (not GetNextRowReference (adjustMeInPlace)) {
            return false;
        }
    }
    for (; ith < 0; ith++) {
        if (not GetPreviousRowReference (adjustMeInPlace)) {
            return false;
        }
    }
    return true;
}

size_t MultiRowTextImager::GetRowNumber (RowReference rowRef) const
{
    // NB: This routine is VERY EXPENSIVE, if the text above the given row has not yet been wrapped, since
    // it forces a wrap. This is meant only to be a convenient code-saver in implementing rownumber based
    // APIs - even though their use is discouraged...
    size_t rowNumber = rowRef.GetSubRow ();
    AssertNotNull (rowRef.GetPartitionMarker ());
    for (PartitionMarker* cur = rowRef.GetPartitionMarker ()->GetPrevious (); cur != nullptr; cur = cur->GetPrevious ()) {
        rowNumber += GetPartitionElementCacheInfo (cur).GetRowCount ();
    }
    return (rowNumber);
}

/*
    @METHOD:        MultiRowTextImager::CountRowDifference
    @DESCRIPTION:   <p>Count the # of rows from one rowreference to the other (order doesn't matter)
        <p>See also @'MultiRowTextImager::CountRowDifferenceLimited'</p>
    */
size_t MultiRowTextImager::CountRowDifference (RowReference lhs, RowReference rhs) const
{
    /*
            *  See which row reference comes before the other, and then can from one TO the
            *  other. Sadly - this forces the wrapping of all that text in between.
            *
            *  Note - this CAN be expensive if the two row references are far apart, as it requires
            *  wrapping all the text in-between.
            */
    PartitionMarker* lhsPM          = lhs.GetPartitionMarker ();
    PartitionMarker* rhsPM          = rhs.GetPartitionMarker ();
    size_t           lhsMarkerStart = lhsPM->GetStart ();
    size_t           rhsMarkerStart = rhsPM->GetStart ();
    bool             leftSmaller    = ((lhsMarkerStart < rhsMarkerStart) or
                        ((lhsMarkerStart == rhsMarkerStart) and lhs.GetSubRow () <= rhs.GetSubRow ()));
    RowReference     firstRowRef    = leftSmaller ? lhs : rhs;
    RowReference     lastRowRef     = leftSmaller ? rhs : lhs;

    size_t rowsGoneBy = 0;
    for (RowReference cur = firstRowRef; cur != lastRowRef; rowsGoneBy++) {
        [[maybe_unused]] bool result = GetIthRowReferenceFromHere (&cur, 1);
        Assert (result);
    }
    return rowsGoneBy;
}

/*
    @METHOD:        MultiRowTextImager::CountRowDifferenceLimited
    @DESCRIPTION:   <p>Count the # of rows from one rowreference to the other (order doesn't matter), but
                never count a number of rows exceeding 'limit'. Just return 'limit' if there are 'limit' rows or more.</p>
                    <p>The reason you would use this intead of @'MultiRowTextImager::CountRowDifference' is if you are
                only checking to see the rowcount is 'at least' something, and you dont wnat to count all the rows. That CAN
                be a pig performance dog - since it tends to force a word-wrap.</p>
                    <p>See also @'MultiRowTextImager::CountRowDifference'</p>
    */
size_t MultiRowTextImager::CountRowDifferenceLimited (RowReference lhs, RowReference rhs, size_t limit) const
{
    /*
            *  See which row reference comes before the other, and then can from one TO the
            *  other. Sadly - this forces the wrapping of all that text in between.
            *
            *  Note - this CAN be expensive if the two row references are far apart, as it requires
            *  wrapping all the text in-between.
            */
    PartitionMarker* lhsPM          = lhs.GetPartitionMarker ();
    PartitionMarker* rhsPM          = rhs.GetPartitionMarker ();
    size_t           lhsMarkerStart = lhsPM->GetStart ();
    size_t           rhsMarkerStart = rhsPM->GetStart ();
    bool             leftSmaller    = ((lhsMarkerStart < rhsMarkerStart) or
                        ((lhsMarkerStart == rhsMarkerStart) and lhs.GetSubRow () <= rhs.GetSubRow ()));
    RowReference     firstRowRef    = leftSmaller ? lhs : rhs;
    RowReference     lastRowRef     = leftSmaller ? rhs : lhs;

    size_t rowsGoneBy = 0;
    for (RowReference cur = firstRowRef; cur != lastRowRef; rowsGoneBy++) {
        [[maybe_unused]] bool result = GetIthRowReferenceFromHere (&cur, 1);
        Assert (result);
        if (rowsGoneBy >= limit) {
            break;
        }
    }
    return rowsGoneBy;
}

size_t MultiRowTextImager::GetTopRowInWindow () const
{
    // NB: Use of this function is discouraged as it is inefficent in the presence of word-wrapping
    return (GetRowNumber (GetTopRowReferenceInWindow ()));
}

size_t MultiRowTextImager::GetTotalRowsInWindow () const
{
    return GetTotalRowsInWindow_ ();
}

size_t MultiRowTextImager::GetLastRowInWindow () const
{
    // NB: Use of this function is discouraged as it is inefficent in the presence of word-wrapping
    return (GetRowNumber (GetLastRowReferenceInWindow ()));
}

void MultiRowTextImager::SetTopRowInWindow (size_t newTopRow)
{
// NB: Use of this function is discouraged as it is inefficent in the presence of word-wrapping
#if 0
        Assert (newTopRow <= GetRowCount ());       // We require this, but don't call since would cause word-wrapping of entire text...
#endif

    SetTopRowInWindow (GetIthRowReference (newTopRow));

    Assert (GetTopRowInWindow () == newTopRow); // Since a SetTopRowInWindow() was called - all the
    // intervening lines have been wrapped anyhow - may
    // as well check we have our definitions straight...
}

void MultiRowTextImager::AssureWholeWindowUsedIfNeeded ()
{
    SetTopRowInWindow (GetTopRowReferenceInWindow ());
}

/*
    @METHOD:        MultiRowTextImager::GetMarkerPositionOfStartOfWindow
    @DESCRIPTION:   <p>Efficient implementation of @'TextImager::GetMarkerPositionOfStartOfWindow'</p>
    */
size_t MultiRowTextImager::GetMarkerPositionOfStartOfWindow () const
{
    return (GetStartOfRow (GetTopRowReferenceInWindow ()));
}

/*
    @METHOD:        MultiRowTextImager::GetMarkerPositionOfEndOfWindow
    @DESCRIPTION:   <p>Efficient implementation of @'TextImager::GetMarkerPositionOfEndOfWindow'</p>
    */
size_t MultiRowTextImager::GetMarkerPositionOfEndOfWindow () const
{
    return GetEndOfRow (GetLastRowReferenceInWindow ());
}

size_t MultiRowTextImager::GetMarkerPositionOfStartOfLastRowOfWindow () const
{
    return GetStartOfRow (GetLastRowReferenceInWindow ());
}

ptrdiff_t MultiRowTextImager::CalculateRowDeltaFromCharDeltaFromTopOfWindow (long deltaChars) const
{
    Assert (long(GetMarkerPositionOfStartOfWindow ()) >= 0 - deltaChars);
    size_t       pos       = long(GetMarkerPositionOfStartOfWindow ()) + deltaChars;
    RowReference targetRow = GetRowReferenceContainingPosition (pos);
    size_t       rowDiff   = CountRowDifference (targetRow, GetTopRowReferenceInWindow ());
    return (deltaChars >= 0) ? rowDiff : -long(rowDiff);
}

ptrdiff_t MultiRowTextImager::CalculateCharDeltaFromRowDeltaFromTopOfWindow (ptrdiff_t deltaRows) const
{
    RowReference row = GetIthRowReferenceFromHere (GetTopRowReferenceInWindow (), deltaRows);
    return (long(GetStartOfRow (row)) - long(GetMarkerPositionOfStartOfWindow ()));
}

void MultiRowTextImager::ScrollByIfRoom (ptrdiff_t downByRows)
{
    RowReference newTopRow = GetTopRowReferenceInWindow ();
    (void)GetIthRowReferenceFromHere (&newTopRow, downByRows); // ignore result cuz we did say - IF-ROOM!
    SetTopRowInWindow (newTopRow);
}

/*
    @METHOD:        MultiRowTextImager::ScrollSoShowing
    @DESCRIPTION:   <p>Implement @'TextImager::ScrollSoShowing' API.</p>
    */
void MultiRowTextImager::ScrollSoShowing (size_t markerPos, size_t andTryToShowMarkerPos)
{
    Assert (markerPos <= GetLength ()); // Allow any marker position (not just character?)
    Assert (fTotalRowsInWindow == 0 or fTotalRowsInWindow == ComputeRowsThatWouldFitInWindowWithTopRow (GetTopRowReferenceInWindow ()));

    if (andTryToShowMarkerPos == 0) { // special flag indicating we don't care...
        andTryToShowMarkerPos = markerPos;
    }
    Assert (andTryToShowMarkerPos <= GetLength ()); // Allow any marker position (not just character?)

    /*
            *      First check and see if the given position is within the current window
            *  If so, do nothing. If it isn't, then try making the given selection
            *  the first row (this later strategy is subject to chanage - but its
            *  a plausible, and easy to implement start).
            */
    size_t startOfWindow = GetMarkerPositionOfStartOfWindow ();
    size_t endOfWindow   = GetMarkerPositionOfEndOfWindow ();
    if (markerPos >= startOfWindow and markerPos < endOfWindow and
        andTryToShowMarkerPos >= startOfWindow and andTryToShowMarkerPos < endOfWindow) {
        ScrollSoShowingHHelper (markerPos, andTryToShowMarkerPos);
        return; // nothing (vertical) changed...
    }

    RowReference originalTop = GetTopRowReferenceInWindow ();

    /*
            *  Now things are a little complicated. We want to show both ends of the
            *  selection - if we can. But - if we must show only one end or the other, we
            *  make sure we show the 'markerPos' end.
            *
            *  We also would probably (maybe?) like to minimize the amount we scroll-by.
            *  that is - if we only need to show one more line, don't scroll by
            *  more than that.
            */

    /*
            *  First get us to a RowReference which is close to where we will eventually end up. That way, and calls
            *  we do which will require word-wrapping (stuff to count rows) will only get applied to rows with a good
            *  liklihood of needing to be wrapped anyhow.
            */
    RowReference newTop = originalTop;
    while (markerPos < newTop.GetPartitionMarker ()->GetStart ()) {
        newTop = RowReference (newTop.GetPartitionMarker ()->GetPrevious (), 0);
    }
    // only try scrolling down at all if we don't already fit in the window - cuz otherwise - we could artificially
    // scroll when not needed.
    if (not PositionWouldFitInWindowWithThisTopRow (markerPos, newTop)) {
        while (markerPos > newTop.GetPartitionMarker ()->GetEnd ()) {
            if (newTop.GetPartitionMarker ()->GetNext () == nullptr) {
                // could be going to row IN last line
                break;
            }
            newTop = RowReference (newTop.GetPartitionMarker ()->GetNext (), 0); // use row 0 to avoid computing RowCount()
        }
        Assert (Contains (markerPos, markerPos, *newTop.GetPartitionMarker ()));
    }

    /*
            *  At this point, we have a newTop which is CLOSE to where it will end up. We now adjust the
            *  newTop to ASSURE that markerPos is shown.
            */
    while (markerPos < GetStartOfRow (newTop) and GetPreviousRowReference (&newTop))
        ;
    while (not PositionWouldFitInWindowWithThisTopRow (markerPos, newTop) and GetNextRowReference (&newTop))
        ;

    // At this point our main desired position should be visible
    Assert (markerPos >= GetStartOfRow (newTop));
    Assert (PositionWouldFitInWindowWithThisTopRow (markerPos, newTop));

    /*
            *  Now - try to adjust the newTop so that the 'andTryToShowMarkerPos' is also
            *  shown. But - BE CAREFUL WE PRESERVE VISIBILITY OF 'markerPos'!!!
            */
    while (not PositionWouldFitInWindowWithThisTopRow (andTryToShowMarkerPos, newTop)) {
        RowReference trailNewTop = newTop;
        if (andTryToShowMarkerPos < GetStartOfRow (trailNewTop)) {
            if (not GetPreviousRowReference (&trailNewTop)) {
                break;
            }
        }
        else {
            if (not GetNextRowReference (&trailNewTop)) {
                break;
            }
        }
        if (PositionWouldFitInWindowWithThisTopRow (markerPos, trailNewTop)) {
            newTop = trailNewTop;
        }
        else {
            break;
        }
    }

    /*
            *  Now - see if we've moved our 'newTop' by more than a certain threshold. If YES - then we may as well scroll
            *  so that the new region of interest is CENTERED in the window.
            */
    const unsigned kRowMoveThreshold = 1;
    if (CountRowDifferenceLimited (originalTop, newTop, kRowMoveThreshold + 1) > kRowMoveThreshold) {
        bool mustPreserveSecondPos = PositionWouldFitInWindowWithThisTopRow (andTryToShowMarkerPos, newTop);

        // Now try to center the region of interest. Center by number of rows - not height of pixels. Height of pixels
        // might be better - but I think this is slightly easier - and probably just as good most of the time.
        size_t       topMarkerPos    = min (markerPos, andTryToShowMarkerPos);
        size_t       botMarkerPos    = max (markerPos, andTryToShowMarkerPos);
        size_t       numRowsAbove    = CountRowDifference (newTop, GetRowReferenceContainingPosition (topMarkerPos));
        size_t       rowsInWindow    = ComputeRowsThatWouldFitInWindowWithTopRow (newTop);
        RowReference lastRowInWindow = GetIthRowReferenceFromHere (newTop, rowsInWindow - 1);
        size_t       numRowsBelow    = CountRowDifference (lastRowInWindow, GetRowReferenceContainingPosition (botMarkerPos));

        size_t numRowsToSpare = numRowsAbove + numRowsBelow;

        // to to make numRowsAbove = 1/2 of numRowsToSpare
        RowReference trailNewTop = newTop;
        GetIthRowReferenceFromHere (&trailNewTop, int(numRowsAbove) - int(numRowsToSpare / 2));
        if (PositionWouldFitInWindowWithThisTopRow (markerPos, trailNewTop) and
            (not mustPreserveSecondPos or PositionWouldFitInWindowWithThisTopRow (andTryToShowMarkerPos, trailNewTop))) {
            newTop = trailNewTop;
        }
    }

    SetTopRowInWindow (newTop); // This handles any notification of scrolling/update of sbars etc...

    Assert (GetMarkerPositionOfStartOfWindow () <= markerPos and markerPos <= GetMarkerPositionOfEndOfWindow ());

    /*
            * Must call this AFTER we've done some VERTICAL scrolling - cuz the vertical scrolling could have affected the MaxHPOS.
            */
    ScrollSoShowingHHelper (markerPos, andTryToShowMarkerPos);
}

void MultiRowTextImager::SetTopRowInWindow (RowReference row)
{
    if (GetForceAllRowsShowing ()) {
        row = AdjustPotentialTopRowReferenceSoWholeWindowUsed (row);
    }
    if (row != GetTopRowReferenceInWindow ()) {
        SetTopRowInWindow_ (row);
        InvalidateScrollBarParameters ();
    }
}

/*
    @METHOD:        MultiRowTextImager::Draw
    @DESCRIPTION:   <p>Implement the basic drawing of the @'TextImager::Draw' API by breaking the content
                up into 'rows' of text and drawing each row with @'MultiRowTextImager::DrawRow'. This implementation
                calls @'TextImager::EraseBackground' to erase the background before drawing the text (drawing the actual
                text is typically done in TRANSPARENT mode). Draw the space between lines (interline space) with
                @'MultiRowTextImager::DrawInterLineSpace'.</p>
                    <p>This routine also respecs the @'TextImager::GetImageUsingOffscreenBitmaps' flag, and handles the
                ofscreen imaging (to reduce flicker). Note that if the 'printing' argument is set- this overrides the offscreen bitmaps
                flag, and prevents offscreen drawing.</p>
    */
void MultiRowTextImager::Draw (const Led_Rect& subsetToDraw, bool printing)
{
    Invariant ();

    Led_Rect rowsLeftToDrawRect = GetWindowRect ();

    Tablet_Acquirer tablet_ (this);
    Led_Tablet      tablet = tablet_;
    AssertNotNull (tablet);

/*
            *  Save old font/pen/brush info here, and restore - even in the presence of exceptions -
            *  on the way out. That way - the drawsegment code need not worry about restoring
            *  these things.
            */
#if qPlatform_MacOS
    tablet->SetPort ();
    RGBColor oldForeColor = GDI_GetForeColor ();
    RGBColor oldBackColor = GDI_GetBackColor ();
#elif qPlatform_Windows
    Led_Win_Obj_Selector pen (tablet, ::GetStockObject (NULL_PEN));
    Led_Win_Obj_Selector brush (tablet, ::GetStockObject (NULL_BRUSH));
#endif

    /*
            *  Do this AFTER the save of colors above cuz no need in preserving that crap for
            *  offscreen bitmap we cons up here on the fly.
            */
    OffscreenTablet thisIsOurNewOST;
    if (GetImageUsingOffscreenBitmaps () and not printing) {
        thisIsOurNewOST.Setup (tablet_);
    }

    try {
        //size_t            rowNumberInWindow   =   0;
        size_t       totalRowsInWindow = GetTotalRowsInWindow_ ();
        RowReference topRowInWindow    = GetTopRowReferenceInWindow ();
        size_t       rowsLeftInWindow  = totalRowsInWindow;
        for (PartitionMarker* pm = topRowInWindow.GetPartitionMarker (); rowsLeftInWindow != 0; pm = pm->GetNext ()) {
            Assert (pm != nullptr);
            size_t startSubRow = 0;
            size_t maxSubRow   = static_cast<size_t> (-1);
            if (pm == topRowInWindow.GetPartitionMarker ()) {
                startSubRow = topRowInWindow.GetSubRow ();
            }
            maxSubRow        = rowsLeftInWindow - 1 + startSubRow;
            size_t rowsDrawn = 0;
            DrawPartitionElement (pm, startSubRow, maxSubRow, tablet,
                                  (GetImageUsingOffscreenBitmaps () and not printing) ? &thisIsOurNewOST : nullptr,
                                  printing, subsetToDraw,
                                  &rowsLeftToDrawRect, &rowsDrawn);
            Assert (rowsLeftInWindow >= rowsDrawn);
            rowsLeftInWindow -= rowsDrawn;
        }

        /*
                *  Now erase to the end of the page.
                */
        Assert (tablet == tablet_); // Draw to screen directly past here...
        {
            Led_Rect eraser = GetWindowRect ();
            eraser.top      = rowsLeftToDrawRect.top; // only from here down...
            eraser.bottom   = subsetToDraw.bottom;    // cuz image rect may not cover what it used to, and never any need to

            if (eraser.top > eraser.bottom) {
                eraser.bottom = eraser.top;
            }

            // SEE IF WE CAN TIGHTEN THIS TEST A BIT MORE, SO WHEN NO PIXELS WILL BE DRAWN, WE DON'T BOTHER
            // IN OTHER WORDS, CHANGE A COUPLE <= to < - LGP 970315

            // QUICKIE INTERSECT TEST
            if (
                (
                    (eraser.top >= subsetToDraw.top and eraser.top <= subsetToDraw.bottom) or
                    (eraser.bottom >= subsetToDraw.top and eraser.bottom <= subsetToDraw.bottom)) and
                (eraser.GetHeight () > 0 and eraser.GetWidth () > 0)) {
                if (GetImageUsingOffscreenBitmaps () and not printing) {
                    tablet = thisIsOurNewOST.PrepareRect (eraser);
                }
                EraseBackground (tablet, eraser, printing);
#if 0
                    // Do we want to hilight the section after the end of the last row displayed if the selection continues onto
                    // the next window? Somehow, I think it ends up looking schlocky. Leave off for now...
                    size_t  hilightStart    =   GetSelectionStart ();
                    size_t  hilightEnd      =   GetSelectionEnd ();
                    size_t  end             =   GetMarkerPositionOfEndOfWindow ();
                    bool    segmentHilightedAtEnd   =   GetSelectionShown () and (hilightStart < end) and (end <= hilightEnd);
                    if (not printing and segmentHilightedAtEnd) {
                        HilightARectangle (tablet, eraser);
                    }
#endif
                if (GetImageUsingOffscreenBitmaps () and not printing) {
                    /*
                            *  Blast offscreen bitmap onto the screen.
                            */
                    thisIsOurNewOST.BlastBitmapToOrigTablet ();
                    tablet = tablet_; // don't use offscreen tablet past here... Draw to screen directly!!!
                }
            }
        }
    }
    catch (...) {
#if qPlatform_MacOS
        // Probably this code (and below case as well) is buggy. Setting back color in offscreen port (which is current now).
        // But the code has been in place for quite some time (don't think broken by my offscreen bitmap move to LedGDI) with no
        // noticable bugs/problems... Reconsider later...
        // LGP 2001-05-11
        Assert (*tablet == Led_GetCurrentGDIPort ());
        GDI_RGBForeColor (oldForeColor);
        GDI_RGBBackColor (oldBackColor);
#endif
        throw;
    }
#if qPlatform_MacOS
    Assert (*tablet == Led_GetCurrentGDIPort ());
    GDI_RGBForeColor (oldForeColor);
    GDI_RGBBackColor (oldBackColor);
#endif
}

/*
    @METHOD:        MultiRowTextImager::DrawPartitionElement
    @DESCRIPTION:   <p></p>
    */
void MultiRowTextImager::DrawPartitionElement (PartitionMarker* pm, size_t startSubRow, size_t maxSubRow, Led_Tablet tablet, OffscreenTablet* offscreenTablet, bool printing, const Led_Rect& subsetToDraw, Led_Rect* remainingDrawArea, size_t* rowsDrawn)
{
    RequireNotNull (pm);
    RequireNotNull (remainingDrawArea);
    RequireNotNull (rowsDrawn);

    size_t start = pm->GetStart ();
    size_t end   = pm->GetEnd ();

    Assert (end <= GetLength () + 1);
    if (end == GetLength () + 1) {
        end--; // don't include bogus char at end of buffer
    }

    Led_Tablet                savedTablet = tablet;
    PartitionElementCacheInfo pmCacheInfo = GetPartitionElementCacheInfo (pm);
    size_t                    endSubRow   = min (pmCacheInfo.GetRowCount () - 1, maxSubRow);
    *rowsDrawn                            = 0;

    size_t                              partLen = end - start;
    Memory::SmallStackBuffer<Led_tChar> partitionBuf (partLen);
    CopyOut (start, partLen, partitionBuf);

    for (size_t subRow = startSubRow; subRow <= endSubRow; ++subRow) {
        Led_Rect currentRowRect     = *remainingDrawArea;
        currentRowRect.bottom       = currentRowRect.top + pmCacheInfo.GetRowHeight (subRow);
        Led_Distance interlineSpace = (subRow == pmCacheInfo.GetLastRow ()) ? pmCacheInfo.GetInterLineSpace () : 0;
        if (
            (currentRowRect.bottom + Led_Coordinate (interlineSpace) > subsetToDraw.top) and
            (currentRowRect.top < subsetToDraw.bottom)) {

            /*
                    *  patch start/end/len to take into account rows...
                    */
            size_t rowStart = start + pmCacheInfo.PeekAtRowStart (subRow);
            size_t rowEnd   = end;
            if (subRow < pmCacheInfo.GetLastRow ()) {
                rowEnd = pm->GetStart () + pmCacheInfo.PeekAtRowStart (subRow + 1); // 'end' points just past last character in row
            }
            {
                if (subRow == pmCacheInfo.GetLastRow ()) {
                    Assert (pm->GetEnd () > 0);
                    size_t markerEnd = pm->GetEnd ();
                    Assert (markerEnd <= GetLength () + 1);
                    if (markerEnd == GetLength () + 1) {
                        rowEnd = GetLength ();
                    }
                    else {
                        size_t prevToEnd = FindPreviousCharacter (markerEnd);
                        if (prevToEnd >= rowStart) {
                            Led_tChar lastChar;
                            CopyOut (prevToEnd, 1, &lastChar);
                            if (RemoveMappedDisplayCharacters (&lastChar, 1) == 0) {
                                rowEnd = (prevToEnd);
                            }
                        }
                    }
                }
                Assert (rowEnd == GetEndOfRowContainingPosition (rowStart));
            }

#if 1
            TextLayoutBlock_Copy rowText = GetTextLayoutBlock (rowStart, rowEnd);
#else
            TextLayoutBlock_Basic rowText (partitionBuf + (rowStart - start), partitionBuf + (rowStart - start) + (rowEnd - rowStart));
#endif

            if (offscreenTablet != nullptr) {
                tablet = offscreenTablet->PrepareRect (currentRowRect, interlineSpace);
            }

            {
                /*
                        *  Not sure why I didn't always do this? But changed from just setting RHS/LHS to subsetToDraw
                        *  to this full intersection as part of SPR#1322 - LGP 2003-04-01.
                        */
                Led_Rect invalidRowRect = Intersection (currentRowRect, subsetToDraw);
                DrawRow (tablet, currentRowRect, invalidRowRect, rowText, rowStart, rowEnd, printing);
            }

            /*
                    *  Now erase/draw any interline space.
                    */
            if (interlineSpace != 0) {
                size_t hilightStart          = GetSelectionStart ();
                size_t hilightEnd            = GetSelectionEnd ();
                bool   segmentHilightedAtEnd = GetSelectionShown () and (hilightStart < rowEnd) and (rowEnd <= hilightEnd);
                if (pm->GetNext () == nullptr and subRow == pmCacheInfo.GetLastRow ()) {
                    segmentHilightedAtEnd = false; // last row always contains no NL - so no invert off to the right...
                }
                DrawInterLineSpace (interlineSpace, tablet, currentRowRect.bottom, segmentHilightedAtEnd, printing);
            }

            if (offscreenTablet != nullptr) {
                /*
                        *  Blast offscreen bitmap onto the screen.
                        */
                offscreenTablet->BlastBitmapToOrigTablet ();
                tablet = savedTablet; // don't use offscreen tablet past here... Draw to screen directly!!!
            }
        }

        remainingDrawArea->top = currentRowRect.bottom + interlineSpace;
        (*rowsDrawn)++;
    }
}

Led_Rect MultiRowTextImager::GetCharLocation (size_t afterPosition) const
{
    return (GetCharLocationRowRelative (afterPosition, RowReference (GetFirstPartitionMarker (), 0)));
}

Led_Rect MultiRowTextImager::GetCharWindowLocation (size_t afterPosition) const
{
    Led_Point windowOrigin = GetWindowRect ().GetOrigin () - Led_Point (0, GetHScrollPos ());
    return (windowOrigin +
            GetCharLocationRowRelative (afterPosition, GetTopRowReferenceInWindow (), GetTotalRowsInWindow_ ()));
}

size_t MultiRowTextImager::GetCharAtLocation (const Led_Point& where) const
{
    return (GetCharAtLocationRowRelative (where, RowReference (GetFirstPartitionMarker (), 0)));
}

size_t MultiRowTextImager::GetCharAtWindowLocation (const Led_Point& where) const
{
    Led_Point windowOrigin = GetWindowRect ().GetOrigin () - Led_Point (0, GetHScrollPos ());
    return (GetCharAtLocationRowRelative (where - windowOrigin,
                                          GetTopRowReferenceInWindow (),
                                          GetTotalRowsInWindow_ ()));
}

size_t MultiRowTextImager::GetStartOfRow (size_t rowNumber) const
{
    // NB: Use of routines using rowNumbers force word-wrap, and so can be quite slow.
    // Routines using RowReferences often perform MUCH better
    return (GetStartOfRow (GetIthRowReference (rowNumber)));
}

size_t MultiRowTextImager::GetStartOfRowContainingPosition (size_t charPosition) const
{
    return (GetStartOfRow (GetRowReferenceContainingPosition (charPosition)));
}

size_t MultiRowTextImager::GetEndOfRow (size_t rowNumber) const
{
    // NB: Use of routines using rowNumbers force word-wrap, and so can be quite slow.
    // Routines using RowReferences often perform MUCH better
    return (GetEndOfRow (GetIthRowReference (rowNumber)));
}

size_t MultiRowTextImager::GetEndOfRowContainingPosition (size_t charPosition) const
{
    return (GetEndOfRow (GetRowReferenceContainingPosition (charPosition)));
}

size_t MultiRowTextImager::GetRealEndOfRow (size_t rowNumber) const
{
    // NB: Use of routines using rowNumbers force word-wrap, and so can be quite slow.
    // Routines using RowReferences often perform MUCH better
    return (GetRealEndOfRow (GetIthRowReference (rowNumber)));
}

size_t MultiRowTextImager::GetRealEndOfRowContainingPosition (size_t charPosition) const
{
    return (GetRealEndOfRow (GetRowReferenceContainingPosition (charPosition)));
}

size_t MultiRowTextImager::GetStartOfRow (RowReference row) const
{
    PartitionMarker* cur    = row.GetPartitionMarker ();
    size_t           subRow = row.GetSubRow ();
    AssertNotNull (cur);
    return (cur->GetStart () + (subRow == 0 ? 0 : GetPartitionElementCacheInfo (cur).GetLineRelativeRowStartPosition (subRow)));
}

size_t MultiRowTextImager::GetEndOfRow (RowReference row) const
{
    PartitionMarker* cur    = row.GetPartitionMarker ();
    size_t           subRow = row.GetSubRow ();
    AssertNotNull (cur);
    PartitionElementCacheInfo pmCacheInfo = GetPartitionElementCacheInfo (cur);
    if (subRow == pmCacheInfo.GetLastRow ()) {
        // Be careful about NL at end. If we end with an NL, then don't count that.
        // And for the last PM - it contains a bogus empty character. Don't count
        // that either.
        Assert (cur->GetEnd () > 0);

        size_t markerEnd = cur->GetEnd ();
        Assert (markerEnd <= GetLength () + 1);
        if (markerEnd == GetLength () + 1) {
            return (GetLength ());
        }

        size_t prevToEnd = FindPreviousCharacter (markerEnd);
        if (prevToEnd >= GetStartOfRow (row)) {
            Led_tChar lastChar;
            CopyOut (prevToEnd, 1, &lastChar);
            if (RemoveMappedDisplayCharacters (&lastChar, 1) == 0) {
                return (prevToEnd);
            }
        }
        return (markerEnd);
    }
    else {
        return (cur->GetStart () + pmCacheInfo.GetLineRelativeRowStartPosition (subRow + 1));
    }
}

size_t MultiRowTextImager::GetRealEndOfRow (RowReference row) const
{
    PartitionMarker* cur    = row.GetPartitionMarker ();
    size_t           subRow = row.GetSubRow ();
    AssertNotNull (cur);
    PartitionElementCacheInfo pmCacheInfo = GetPartitionElementCacheInfo (cur);
    if (subRow == pmCacheInfo.GetLastRow ()) {
        Assert (cur->GetEnd () > 0);
        size_t markerEnd = cur->GetEnd ();
        return (markerEnd);
    }
    else {
        return (cur->GetStart () + pmCacheInfo.GetLineRelativeRowStartPosition (subRow + 1));
    }
}

MultiRowTextImager::RowReference MultiRowTextImager::GetRowReferenceContainingPosition (size_t charPosition) const
{
    Require (charPosition <= GetEnd ());
    PartitionMarker* pm = GetPartitionMarkerContainingPosition (charPosition);
    AssertNotNull (pm);

    size_t pmStart = pm->GetStart ();
    if (charPosition == pmStart) { // slight speed tweek
        return (RowReference (pm, 0));
    }

    // figure out what subrow the position occurs in, and return that...
    PartitionElementCacheInfo pmCacheInfo = GetPartitionElementCacheInfo (pm);
    return (RowReference (pm, pmCacheInfo.LineRelativePositionInWhichRow (charPosition - pmStart)));
}

size_t MultiRowTextImager::GetRowContainingPosition (size_t charPosition) const
{
    // Warning: GetRowReferenceContainingPosition () in preference, since
    // it doesn't require call to pm->GetRowCount () - forcing a word-wrap...
    return (GetRowNumber (GetRowReferenceContainingPosition (charPosition)));
}

size_t MultiRowTextImager::GetRowCount () const
{
    // NB: This is an expensive routine because it forces a word-wrap on all the text!
    size_t rowCount = 0;
    for (PartitionMarker* cur = GetFirstPartitionMarker (); cur != nullptr; cur = cur->GetNext ()) {
        AssertNotNull (cur);
        Assert (GetPartitionElementCacheInfo (cur).GetRowCount () >= 1);
        rowCount += GetPartitionElementCacheInfo (cur).GetRowCount ();
    }
    return (rowCount);
}

Led_Rect MultiRowTextImager::GetCharLocationRowRelativeByPosition (size_t afterPosition, size_t positionOfTopRow, size_t maxRowsToCheck) const
{
    return GetCharLocationRowRelative (afterPosition, GetRowReferenceContainingPosition (positionOfTopRow), maxRowsToCheck);
}

Led_Distance MultiRowTextImager::GetRowHeight (size_t rowNumber) const
{
    // NB: Use of routines using rowNumbers force word-wrap, and so can be quite slow.
    // Routines using RowReferences often perform MUCH better
    return (GetRowHeight (GetIthRowReference (rowNumber)));
}

/*
    @METHOD:        MultiRowTextImager::GetRowRelativeBaselineOfRowContainingPosition
    @DESCRIPTION:   <p>Override/implement @'TextImager::GetRowRelativeBaselineOfRowContainingPosition'.</p>
    */
Led_Distance MultiRowTextImager::GetRowRelativeBaselineOfRowContainingPosition (size_t charPosition) const
{
    RowReference thisRow    = GetRowReferenceContainingPosition (charPosition);
    size_t       startOfRow = GetStartOfRow (thisRow);
    size_t       endOfRow   = GetEndOfRow (thisRow);
    return MeasureSegmentBaseLine (startOfRow, endOfRow);
}

void MultiRowTextImager::GetStableTypingRegionContaingMarkerRange (size_t fromMarkerPos, size_t toMarkerPos,
                                                                   size_t* expandedFromMarkerPos, size_t* expandedToMarkerPos) const
{
    AssertNotNull (expandedFromMarkerPos);
    AssertNotNull (expandedToMarkerPos);
    Assert (fromMarkerPos <= toMarkerPos);
    Assert (toMarkerPos <= GetEnd ());
#if qMultiByteCharacters && qDebug
    Assert_CharPosDoesNotSplitCharacter (fromMarkerPos);
    Assert_CharPosDoesNotSplitCharacter (toMarkerPos);
#endif

    size_t curTopRowRelativeRowNumber = 0;

    RowReference curRow = GetTopRowReferenceInWindow ();
    do {
        PartitionMarker* cur = curRow.GetPartitionMarker ();
        AssertNotNull (cur);
        size_t start = cur->GetStart ();
        size_t end   = cur->GetEnd ();

        // For the last partition marker - we are including a BOGUS character past the end of the buffer.
        // We don't want to return that. But otherwise - it is OK to return the NL at the end of the
        // other lines (though perhaps that is unnecceary).... LGP 950210
        if (cur->GetNext () == nullptr) {
            end--;
        }

        // If we are strictly before the first row, we won't appear later...
        if (curTopRowRelativeRowNumber == 0 and (fromMarkerPos < start)) {
            break;
        }

        curTopRowRelativeRowNumber++;

        if (Contains (*cur, fromMarkerPos) and Contains (*cur, toMarkerPos)) {
            (*expandedFromMarkerPos) = start;
            (*expandedToMarkerPos)   = end;
            Assert ((*expandedFromMarkerPos) <= (*expandedToMarkerPos));
            Assert ((*expandedToMarkerPos) <= GetEnd ());
            return;
        }

        if (curTopRowRelativeRowNumber >= GetTotalRowsInWindow_ ()) {
            break; // though this might allow is to go too far - no matter. We'd return
            // the same result anyhow. And the extra overhead in counter rows
            // as opposed to lines doesn't offset the overhead in counting a few
            // extra lines - besides - this is simpler...
        }
    } while (GetNextRowReference (&curRow));

    (*expandedFromMarkerPos) = 0;
    (*expandedToMarkerPos)   = GetEnd ();
}

Led_Distance MultiRowTextImager::GetHeightOfRows (size_t startingRow, size_t rowCount) const
{
    return (GetHeightOfRows (GetIthRowReference (startingRow), rowCount));
}

Led_Distance MultiRowTextImager::GetHeightOfRows (RowReference startingRow, size_t rowCount) const
{
    Led_Distance height = 0;
    for (RowReference curRow = startingRow; rowCount > 0; rowCount--) {
        PartitionMarker*          curPM       = curRow.GetPartitionMarker ();
        PartitionElementCacheInfo pmCacheInfo = GetPartitionElementCacheInfo (curPM);
        height += pmCacheInfo.GetRowHeight (curRow.GetSubRow ());
        if (curRow.GetSubRow () == pmCacheInfo.GetLastRow ()) {
            height += pmCacheInfo.GetInterLineSpace ();
        }
        (void)GetNextRowReference (&curRow);
    }
    return (height);
}

void MultiRowTextImager::DidUpdateText (const UpdateInfo& updateInfo) noexcept
{
//maynot need this addtion either - since done in PMInfoCache guy...
#if 1
    /*
            *  This invalidation is way more aggressive than it needs to be. Really all we need
            *  todo is check if this was one of the rows in the window, and only invalidate then.
            *  But that check would be at least somewhat costly. So this may be best for now.
            *  Maybe later try walking the markers from the start of window by n (number of rows currently
            *  set in total rows in window cache) and see if we are hit. Maybe that wouldn't be
            *  too bad cuz we could do nothing in the common case where the row cache was already
            *  invalid.
            *      --LGP 960912
            */
    InvalidateTotalRowsInWindow ();
#endif

    InvalidateTotalRowsInWindow ();
    inherited::DidUpdateText (updateInfo);
    AssertNotNull (fTopLinePartitionMarkerInWindow);
    //  ReValidateSubRowInTopLineInWindow ();
    AssureWholeWindowUsedIfNeeded ();
    InvalidateScrollBarParameters (); // even if we don't change the top row, we might change enuf about the text to change sbar
}

void MultiRowTextImager::SetWindowRect (const Led_Rect& windowRect)
{
    /*
            *  NB: We only check that the 'heightChanged' because thats all that can affect the number of rows
            *  shown on the screen. Someone pointed out that the WIDTH of the window rect changing can ALSO
            *  change the number of rows, and invalidate the caches. They were thinking of the subclass
            *  WordWrappedTextImager. And then - in the special case where you have implemented the policy
            *  'wrap-to-window' (which you would NOT - for example - if you are using a ruler to specify margins).
            *      The point is - it is THERE - where you implement that wrapping policy - e.g. WordWrappedTextImager::GetLayoutMargins() -
            *  that you would have to hook SetWindowRect () and invalidate the cache.
            */
    bool heightChanged = GetWindowRect ().GetHeight () != windowRect.GetHeight ();
    inherited::SetWindowRect (windowRect);
    if (heightChanged and PeekAtTextStore () != nullptr) {
        InvalidateTotalRowsInWindow ();
        AssureWholeWindowUsedIfNeeded ();
        InvalidateScrollBarParameters ();
    }
}

/*
    @METHOD:        MultiRowTextImager::InvalidateAllCaches
    @DESCRIPTION:   <p>Hook @'TextImager::InvalidateAllCaches' method to invalidate additional information.
                Invalidate cached row-height/etc information for the entire imager. Invalidate rows
                in a window cached values, etc.</p>
    */
void MultiRowTextImager::InvalidateAllCaches ()
{
    inherited::InvalidateAllCaches ();
    if (GetPartition ().get () != nullptr) { // careful that we aren't changing text metrics while we have no textstore attached!!!
        if (fPMCacheMgr.get () != nullptr) {
            fPMCacheMgr->ClearCache ();
        }
        InvalidateTotalRowsInWindow ();
        //      ReValidateSubRowInTopLineInWindow ();
        AssureWholeWindowUsedIfNeeded ();
        InvalidateScrollBarParameters ();
    }
}

MultiRowTextImager::RowReference MultiRowTextImager::AdjustPotentialTopRowReferenceSoWholeWindowUsed (const RowReference& potentialTopRow)
{
    /*
            *  This check is always safe, but probably not a worthwhile optimization, except that it avoids
            *  some problems about initializing the top-row-reference before we've got a valid
            *  tablet setup to use.
            */
    if (potentialTopRow.GetSubRow () == 0 and potentialTopRow.GetPartitionMarker ()->GetPrevious () == nullptr) {
        return potentialTopRow;
    }

    Led_Coordinate windowHeight = GetWindowRect ().GetHeight ();
    Led_Coordinate heightUsed   = 0;

    for (RowReference curRow = potentialTopRow;;) {
        PartitionMarker*          curPM       = curRow.GetPartitionMarker ();
        PartitionElementCacheInfo pmCacheInfo = GetPartitionElementCacheInfo (curPM);
        heightUsed += pmCacheInfo.GetRowHeight (curRow.GetSubRow ());
        if (curRow.GetSubRow () == pmCacheInfo.GetLastRow ()) {
            heightUsed += pmCacheInfo.GetInterLineSpace ();
        }
        if (heightUsed >= windowHeight) {
            return (potentialTopRow); // Then we used all the space we could have - and that is a good row!
        }
        if (not GetNextRowReference (&curRow)) {
            break;
        }
    }

    // If we got here - we ran out of rows before we ran out of height.
    // That means we should scroll back a smidge...
    for (RowReference curRow = potentialTopRow;;) {
        if (not GetPreviousRowReference (&curRow)) {
            return (curRow); // if we've gone back as far as we can - were done!
            // Even if we didn't use all the height
        }

        PartitionMarker*          curPM       = curRow.GetPartitionMarker ();
        PartitionElementCacheInfo pmCacheInfo = GetPartitionElementCacheInfo (curPM);
        heightUsed += pmCacheInfo.GetRowHeight (curRow.GetSubRow ());
        if (curRow.GetSubRow () == pmCacheInfo.GetLastRow ()) {
            heightUsed += pmCacheInfo.GetInterLineSpace ();
        }
        if (heightUsed > windowHeight) {
            // We went back one too far - forward one and return that.
            [[maybe_unused]] bool result = GetNextRowReference (&curRow);
            Assert (result);
            return curRow;
        }
        else if (heightUsed == windowHeight) {
            return (curRow); // Then we used all the space we could have - and that is a good row!
        }
    }
    Assert (false);
    return (potentialTopRow); // NotReached / silence compiler warnings
}

bool MultiRowTextImager::PositionWouldFitInWindowWithThisTopRow (size_t markerPos, const RowReference& newTopRow)
{
    if (markerPos < GetStartOfRow (newTopRow)) {
        return false;
    }

    size_t       rowCount = ComputeRowsThatWouldFitInWindowWithTopRow (newTopRow);
    RowReference lastRow  = GetIthRowReferenceFromHere (newTopRow, rowCount - 1);

    return (markerPos < GetRealEndOfRow (lastRow));
}

void MultiRowTextImager::ReValidateSubRowInTopLineInWindow ()
{
    AssertNotNull (fTopLinePartitionMarkerInWindow);

    // don't bother calling GetRowCount () if fSubRowInTopLineInWindow is already ZERO - avoid possible word-wrap
    if (fSubRowInTopLineInWindow != 0) {
#if 1
        size_t lastRow = GetPartitionElementCacheInfo (fTopLinePartitionMarkerInWindow).GetLastRow ();
        if (fSubRowInTopLineInWindow > lastRow) {
            fSubRowInTopLineInWindow = lastRow;
        }
#else
        bool   pmNotWrapped = (fTopLinePartitionMarkerInWindow->fPixelHeightCache == Led_Distance (-1));
        size_t lastRow      = GetPartitionElementCacheInfo (fTopLinePartitionMarkerInWindow).GetLastRow ();
        if (fSubRowInTopLineInWindow > lastRow) {
            fSubRowInTopLineInWindow = lastRow;
        }
        if (pmNotWrapped) {
            /*
                    *  We invalidated it for this method. But we may have done so prematurely - during
                    *  the context of a marker DidUpdate () method. See SPR#0821
                    *
                    *  Reset it back to invalid so it will be properly layed out.
                    */
            fTopLinePartitionMarkerInWindow->InvalidateCache ();
        }
#endif
    }
}

size_t MultiRowTextImager::ComputeRowsThatWouldFitInWindowWithTopRow (const RowReference& newTopRow) const
{
    /*
            *  For now, we don't show partial rows at the bottom. We
            *  might want to reconsider this.
            */
    Led_Coordinate windowHeight = GetWindowRect ().GetHeight ();

    /*
            *  Wind out way to the bottom of the window from our current position,
            *  and count rows.
            */
    size_t         rowCount   = 0;
    Led_Coordinate heightUsed = 0;
    for (RowReference curRow = newTopRow;;) {
        rowCount++;
        PartitionMarker*          curPM       = curRow.GetPartitionMarker ();
        PartitionElementCacheInfo pmCacheInfo = GetPartitionElementCacheInfo (curPM);
        heightUsed += pmCacheInfo.GetRowHeight (curRow.GetSubRow ());
        if (curRow.GetSubRow () == pmCacheInfo.GetLastRow ()) {
            heightUsed += pmCacheInfo.GetInterLineSpace ();
        }
        if (heightUsed > windowHeight) {
            // we went one too far
            rowCount--;
            break;
        }
        else if (heightUsed == windowHeight) {
            break; // thats all that will fit
        }

        if (not GetNextRowReference (&curRow)) {
            break;
        }
    }
    if (rowCount == 0) { // always for the existence of at least one row...
        rowCount = 1;
    }

    return (rowCount);
}

/*
    @METHOD:        MultiRowTextImager::GetCharLocationRowRelative
    @DESCRIPTION:   <p>Return the row-relative (to get window relative - add top-left of window-rect) bounding rectange of the
                given character cell. Compute the vertical position relative to the given argument 'topRow' and check and most
                'maxRowsToCheck' before just returning a large 'off-the-end' value result</p>
    */
Led_Rect MultiRowTextImager::GetCharLocationRowRelative (size_t afterPosition, RowReference topRow, size_t maxRowsToCheck) const
{
    // MUST FIGURE OUT WHAT TODO HERE BETTER - 10000 not good enough answer always...
    const Led_Rect kMagicBeforeRect = Led_Rect (-10000, 0, 0, 0);
    const Led_Rect kMagicAfterRect  = Led_Rect (10000, 0, 0, 0);

    Require (afterPosition <= GetEnd ());
#if qMultiByteCharacters && qDebug
    Assert_CharPosDoesNotSplitCharacter (afterPosition);
#endif

    if (afterPosition < GetStartOfRow (topRow)) {
        return (kMagicBeforeRect);
    }

    RowReference   curRow                     = topRow;
    size_t         curTopRowRelativeRowNumber = 0;
    Led_Coordinate topVPos                    = 0;
    do {
        PartitionMarker* cur    = curRow.GetPartitionMarker ();
        size_t           subRow = curRow.GetSubRow ();
        AssertNotNull (cur);
        size_t start = cur->GetStart ();
        size_t end   = cur->GetEnd (); // end points JUST PAST LAST VISIBLE/OPERATED ON CHAR

        Assert (end <= GetEnd () + 1);

        PartitionElementCacheInfo pmCacheInfo = GetPartitionElementCacheInfo (cur);

        /*
                *  patch start/end/len to take into account rows...
                */
        start += pmCacheInfo.PeekAtRowStart (subRow);
        if (subRow < pmCacheInfo.GetLastRow ()) {
            end = cur->GetStart () + pmCacheInfo.PeekAtRowStart (subRow + 1);
            Assert (start <= end);
        }

        curTopRowRelativeRowNumber++;

        /*
                *  When we've found the right row, then add in the right horizontal offset.
                */
        if (afterPosition >= start and afterPosition < end) {
            Assert (start <= afterPosition);
            Led_Distance hStart = 0;
            Led_Distance hEnd   = 0;
            GetRowRelativeCharLoc (afterPosition, &hStart, &hEnd);
            Assert (hStart <= hEnd);
            return (Led_Rect (topVPos, hStart, pmCacheInfo.GetRowHeight (subRow), hEnd - hStart));
        }

        topVPos += pmCacheInfo.GetRowHeight (subRow);

        if (pmCacheInfo.GetLastRow () == subRow) {
            topVPos += pmCacheInfo.GetInterLineSpace ();
        }

        if (curTopRowRelativeRowNumber >= maxRowsToCheck) {
            break; // return bogus place at the end...
        }
    } while (GetNextRowReference (&curRow));

    return (kMagicAfterRect);
}

size_t MultiRowTextImager::GetCharAtLocationRowRelative (const Led_Point& where, RowReference topRow, size_t maxRowsToCheck) const
{
    /*
            *  Not 100% sure how to deal with points outside our range. For now - we just
            *  return topMost/bottomMost marker positions. That seems to work decently - at least
            *  for now... But I worry if it is the right thing when we do
            *  autoscrolling...
            */
    if (where.v < 0) {
#if qMultiByteCharacters
        Assert_CharPosDoesNotSplitCharacter (0);
#endif
        return (0);
    }

    RowReference   curRow                     = topRow;
    size_t         curTopRowRelativeRowNumber = 0;
    Led_Coordinate topVPos                    = 0;
    do {
        PartitionMarker* cur    = curRow.GetPartitionMarker ();
        size_t           subRow = curRow.GetSubRow ();
        AssertNotNull (cur);
        size_t start = cur->GetStart ();
#if qMultiByteCharacters
        Assert_CharPosDoesNotSplitCharacter (start);
#endif

        PartitionElementCacheInfo pmCacheInfo = GetPartitionElementCacheInfo (cur);

        /*
                *  patch start/end/len to take into account rows...
                */
        start += pmCacheInfo.PeekAtRowStart (subRow);
#if qMultiByteCharacters
        Assert_CharPosDoesNotSplitCharacter (start);
#endif

        /*
                *  Count the interline space as part of the last row of the line for the purpose of hit-testing.
                */
        Led_Distance interLineSpaceIfAny = (pmCacheInfo.GetLastRow () == subRow) ? pmCacheInfo.GetInterLineSpace () : 0;

        curTopRowRelativeRowNumber++;
        if (where.v >= topVPos and where.v < topVPos + Led_Coordinate (pmCacheInfo.GetRowHeight (subRow) + interLineSpaceIfAny)) {
            return GetRowRelativeCharAtLoc (where.h, start);
        }

        if (curTopRowRelativeRowNumber >= maxRowsToCheck) {
            break; // we've checked enuf...
        }

        topVPos += pmCacheInfo.GetRowHeight (subRow) + interLineSpaceIfAny;
    } while (GetNextRowReference (&curRow));

#if qMultiByteCharacters
    Assert_CharPosDoesNotSplitCharacter (GetEnd ());
#endif
    return (GetEnd ());
}

Led_Distance MultiRowTextImager::CalculateInterLineSpace (const PartitionMarker* /*pm*/) const
{
    return (0); // no interline space by default
}

/*
    @METHOD:        MultiRowTextImager::ContainsMappedDisplayCharacters
    @DESCRIPTION:   <p>Override @'TextImager::ContainsMappedDisplayCharacters' to hide '\n' characters.
        See @'qDefaultLedSoftLineBreakChar'.</p>
    */
bool MultiRowTextImager::ContainsMappedDisplayCharacters (const Led_tChar* text, size_t nTChars) const
{
    return ContainsMappedDisplayCharacters_HelperForChar (text, nTChars, '\n') or
           inherited::ContainsMappedDisplayCharacters (text, nTChars);
}

/*
    @METHOD:        MultiRowTextImager::RemoveMappedDisplayCharacters
    @DESCRIPTION:   <p>Override @'TextImager::RemoveMappedDisplayCharacters' to hide '\n' characters.</p>
    */
size_t MultiRowTextImager::RemoveMappedDisplayCharacters (Led_tChar* copyText, size_t nTChars) const
{
    size_t newLen = inherited::RemoveMappedDisplayCharacters (copyText, nTChars);
    Assert (newLen <= nTChars);
    size_t newerLen = RemoveMappedDisplayCharacters_HelperForChar (copyText, newLen, '\n');
    Assert (newerLen <= newLen);
    Assert (newerLen <= nTChars);
    return newerLen;
}

/*
        ********************************************************************************
        ******************* MultiRowTextImager::PartitionElementCacheInfo **************
        ********************************************************************************
        */
void MultiRowTextImager::PartitionElementCacheInfo::Clear ()
{
    fRep = make_shared<Rep> ();
}

void MultiRowTextImager::PartitionElementCacheInfo::IncrementRowCountAndFixCacheBuffers (size_t newStart, Led_Distance newRowsHeight)
{
    fRep->fRowCountCache++;

    // If rowStart array not big enough then allocate it from the heap...
    if (fRep->fRowCountCache > kPackRowStartCount + 1) {
        RowStart_* newRowStartArray = new RowStart_[fRep->fRowCountCache - 1];
        AssertNotNull (newRowStartArray);
        if (fRep->fRowCountCache == kPackRowStartCount + 1 + 1) {
            ::memcpy (newRowStartArray, &fRep->fRowStartArray, sizeof (fRep->fRowStartArray));
        }
        else {
            Assert (fRep->fRowCountCache > 2);
            if (fRep->fRowCountCache > kPackRowStartCount + 1 + 1) {
                AssertNotNull (fRep->fRowStartArray);
                (void)memcpy (newRowStartArray, fRep->fRowStartArray, sizeof (newRowStartArray[1]) * (fRep->fRowCountCache - 2));
                delete[] fRep->fRowStartArray;
            }
        }
        fRep->fRowStartArray = newRowStartArray;
    }

    // If rowHeight array not big enough then allocate it from the heap...
    if (fRep->fRowCountCache > kPackRowHeightCount) {
        RowHeight_* newRowHeightArray = new RowHeight_[fRep->fRowCountCache];
        AssertNotNull (newRowHeightArray);
        if (fRep->fRowCountCache == kPackRowHeightCount + 1) {
            ::memcpy (newRowHeightArray, &fRep->fRowHeightArray, sizeof (fRep->fRowHeightArray));
        }
        else {
            Assert (fRep->fRowCountCache > 1);
            if (fRep->fRowCountCache > kPackRowHeightCount + 1) {
                AssertNotNull (fRep->fRowHeightArray);
                memcpy (newRowHeightArray, fRep->fRowHeightArray, sizeof (newRowHeightArray[1]) * (fRep->fRowCountCache - 1));
                delete[] fRep->fRowHeightArray;
            }
        }
        fRep->fRowHeightArray = newRowHeightArray;
    }

    SetRowStart (fRep->fRowCountCache - 1, newStart);
    SetRowHeight (fRep->fRowCountCache - 1, newRowsHeight);
    fRep->fPixelHeightCache += newRowsHeight;
}

/*
        ********************************************************************************
        *********************** MultiRowTextImager::PMInfoCacheMgr *********************
        ********************************************************************************
        */
MultiRowTextImager::PMInfoCacheMgr::PMInfoCacheMgr (MultiRowTextImager& imager)
    : fPMCache ()
    , fCurFillCachePM (nullptr)
    , fCurFillCacheInfo ()
    , fImager (imager)
    , fMyMarker ()
{
    // REDO this class to make IT a MarkerOwner - and use THAT markerowner for MyMarker. Then - store an additional MyMarker for EACH marker
    // added to cache (just around the PM its used to wrap). Then remove ONLY that PM from the cache in its DIDUpdate.
    PartitionPtr part = imager.GetPartition ();
    Assert (part.get () != nullptr);
    part->AddPartitionWatcher (this);
    fMyMarker     = unique_ptr<MyMarker> (new MyMarker (*this));
    TextStore& ts = part->GetTextStore ();
    ts.AddMarker (fMyMarker.get (), 0, ts.GetLength () + 1, part.get ());
}

MultiRowTextImager::PMInfoCacheMgr::~PMInfoCacheMgr ()
{
    PartitionPtr part = fImager.GetPartition ();
    part->RemovePartitionWatcher (this);
    TextStore& ts = part->GetTextStore ();
    ts.RemoveMarker (fMyMarker.get ());
}

MultiRowTextImager::PartitionElementCacheInfo MultiRowTextImager::PMInfoCacheMgr::GetPartitionElementCacheInfo (Partition::PartitionMarker* pm) const
{
    if (pm == fCurFillCachePM) {
        return fCurFillCacheInfo; // allow recursive call to get PMCacheInfo (so far) DURING context of call to FillCache()
    }
    using MAP_CACHE       = map<Partition::PartitionMarker*, PartitionElementCacheInfo>;
    MAP_CACHE::iterator i = fPMCache.find (pm);
    if (i == fPMCache.end ()) {
        try {
            Assert (fCurFillCachePM == nullptr); // can only do one fillcache at a time...
            fCurFillCachePM = pm;
            fImager.FillCache (pm, fCurFillCacheInfo);
#if qDebug
            {
                for (size_t t = 0; t < fCurFillCacheInfo.GetRowCount (); ++t) {
                    Assert (fCurFillCacheInfo.GetLineRelativeRowStartPosition (t) <= pm->GetLength ());
                    Assert (fCurFillCacheInfo.PeekAtRowStart (t) <= pm->GetLength ());
                }
            }
#endif
            i = fPMCache.insert (MAP_CACHE::value_type (pm, fCurFillCacheInfo)).first;
#if qDebug
            {
                Assert (fCurFillCacheInfo.GetRowCount () == i->second.GetRowCount ());
                for (size_t t = 0; t < fCurFillCacheInfo.GetRowCount (); ++t) {
                    Assert (fCurFillCacheInfo.PeekAtRowHeight (t) == i->second.PeekAtRowHeight (t));
                    Assert (fCurFillCacheInfo.PeekAtRowStart (t) == i->second.PeekAtRowStart (t));
                }
            }
#endif
            Assert (fCurFillCachePM == pm);
            fCurFillCachePM = nullptr;
        }
        catch (...) {
            Assert (fCurFillCachePM == pm);
            fCurFillCachePM = nullptr;
            throw;
        }
    }
    return i->second;
}

void MultiRowTextImager::PMInfoCacheMgr::ClearCache ()
{
    fPMCache.clear ();
}

void MultiRowTextImager::PMInfoCacheMgr::AboutToSplit (PartitionMarker* pm, size_t /*at*/, void** infoRecord) const noexcept
{
    *infoRecord = pm;
}

void MultiRowTextImager::PMInfoCacheMgr::DidSplit (void* infoRecord) const noexcept
{
    PartitionMarker* pm   = reinterpret_cast<PartitionMarker*> (infoRecord);
    using MAP_CACHE       = map<Partition::PartitionMarker*, PartitionElementCacheInfo>;
    MAP_CACHE::iterator i = fPMCache.find (pm);
    if (i != fPMCache.end ()) {
        fPMCache.erase (i);
    }
    fImager.InvalidateTotalRowsInWindow ();
#if 0
        if (pm == fImager.fTopLinePartitionMarkerInWindow) {
            // if splitting top row - we must revalidate top subrow
            fImager.ReValidateSubRowInTopLineInWindow ();
        }
#endif
}

void MultiRowTextImager::PMInfoCacheMgr::AboutToCoalece (PartitionMarker* pm, void** infoRecord) const noexcept
{
    *infoRecord = pm;

    PartitionMarker* newTopLine  = nullptr;
    bool             useFirstRow = false; // otherwise use last row...
    if (pm == fImager.fTopLinePartitionMarkerInWindow) {
        if (pm->GetNext () == nullptr) {
            newTopLine  = fImager.fTopLinePartitionMarkerInWindow->GetPrevious ();
            useFirstRow = false;
        }
        else {
            newTopLine  = fImager.fTopLinePartitionMarkerInWindow->GetNext ();
            useFirstRow = true;
        }
        AssertNotNull (newTopLine);
    }

    if (newTopLine != nullptr) {
        //TMPHACK - REALLY should do old comment out code - see old code in MultiRowTextImager::MultiRowPartition::Coalese - trouble is then
        // we need to pass MORE info to DID_COALESE trhought eh INFORECORD so we can pass BOTH pm AND the newTopLine (really should do this call there)
        // This is a bit of a hack - but AT LEAST should avoid any crashes/flakies we now see - LGP 2002-10-17
        //      fImager.SetTopRowInWindow_ (RowReference (newTopLine, useFirstRow? 0: fImager.GetPartitionElementCacheInfo (newTopLine).GetLastRow ()));
        fImager.SetTopRowInWindow_ (RowReference (newTopLine, 0));
    }

#if 0
        MultiRowPartitionMarker*    newTopLine  =   nullptr;
        bool                        useFirstRow =   false;  // otherwise use last row...
        if (pm == fTextImager.fTopLinePartitionMarkerInWindow) {
            if (pm->GetNext () == nullptr) {
                newTopLine = fTextImager.fTopLinePartitionMarkerInWindow->GetPreviousMRPM ();
                useFirstRow = false;
            }
            else {
                newTopLine = fTextImager.fTopLinePartitionMarkerInWindow->GetNextMRPM ();
                useFirstRow = true;
            }
            AssertNotNull (newTopLine);
        }

        MultiRowPartitionMarker*    successor   =   (MultiRowPartitionMarker*)pm->GetNext ();   // all our pms are subtypes of this type
        inherited::Coalece (pm);                //  deletes pm....
        if (successor != nullptr) {
            successor->InvalidateCache ();              // This is where our xtra text goes - so be sure we invalidate its extent...
        }

        // If we must change the top-row - then do so NOW - cuz NOW its safe to call GetRowCount ()
        if (newTopLine != nullptr) {
            fTextImager.SetTopRowInWindow_ (RowReference (newTopLine, useFirstRow ? 0 : fTextImager.GetPartitionElementCacheInfo (newTopLine).GetLastRow ()));
        }
        AssertNotNull (fTextImager.fTopLinePartitionMarkerInWindow);    // can't delete last one...
        AssertMember (fTextImager.fTopLinePartitionMarkerInWindow, MultiRowPartitionMarker);
        fTextImager.InvalidateTotalRowsInWindow ();
#endif
}

void MultiRowTextImager::PMInfoCacheMgr::DidCoalece (void* infoRecord) const noexcept
{
    PartitionMarker* pm   = reinterpret_cast<PartitionMarker*> (infoRecord);
    using MAP_CACHE       = map<Partition::PartitionMarker*, PartitionElementCacheInfo>;
    MAP_CACHE::iterator i = fPMCache.find (pm);
    if (i != fPMCache.end ()) {
        fPMCache.erase (i);
    }
    fImager.InvalidateTotalRowsInWindow ();
}

void MultiRowTextImager::PMInfoCacheMgr::MyMarkerDidUpdateCallback ()
{
    fPMCache.clear ();

    /*
            *  This invalidation is way more aggressive than it needs to be. Really all we need
            *  todo is check if this was one of the rows in the window, and only invalidate then.
            *  But that check would be at least somewhat costly. So this may be best for now.
            *  Maybe later try walking the markers from the start of window by n (number of rows currently
            *  set in total rows in window cache) and see if we are hit. Maybe that wouldn't be
            *  too bad cuz we could do nothing in the common case where the row cache was already
            *  invalid.
            *      --LGP 960912
            */
    fImager.InvalidateTotalRowsInWindow ();
}

/*
        ********************************************************************************
        *************** MultiRowTextImager::PMInfoCacheMgr::MyMarker *******************
        ********************************************************************************
        */
MultiRowTextImager::PMInfoCacheMgr::MyMarker::MyMarker (PMInfoCacheMgr& pmInfoCacheMgr)
    : fPMInfoCacheMgr (pmInfoCacheMgr)
{
}

void MultiRowTextImager::PMInfoCacheMgr::MyMarker::DidUpdateText (const UpdateInfo& updateInfo) noexcept
{
    inherited::DidUpdateText (updateInfo);
    fPMInfoCacheMgr.MyMarkerDidUpdateCallback ();
}
