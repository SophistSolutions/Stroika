/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../Foundation/StroikaPreComp.h"

#include <limits.h>

#include "LineBasedPartition.h"
#include "Support.h"

#include "SimpleTextImager.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

using PartitionMarker = Partition::PartitionMarker;
using PartitionPtr    = PartitioningTextImager::PartitionPtr;

/*
 ********************************************************************************
 ********************** SimpleTextImager::MyPartitionWatcher ********************
 ********************************************************************************
 */
void SimpleTextImager::MyPartitionWatcher::Init (PartitionPtr partition, SimpleTextImager* imager)
{
    fImager = imager;
    if (partition.get () != nullptr) {
        partition->AddPartitionWatcher (this);
    }
}

void SimpleTextImager::MyPartitionWatcher::UnInit (PartitionPtr partition)
{
    fImager = NULL;
    if (partition.get () != nullptr) {
        partition->RemovePartitionWatcher (this);
    }
}

void SimpleTextImager::MyPartitionWatcher::AboutToSplit (PartitionMarker* /*pm*/, size_t /*at*/, void** /*infoRecord*/) const noexcept
{
    AssertNotNull (fImager);
}

void SimpleTextImager::MyPartitionWatcher::DidSplit (void* /*infoRecord*/) const noexcept
{
    AssertNotNull (fImager);
    fImager->InvalidateTotalRowsInWindow ();
}

void SimpleTextImager::MyPartitionWatcher::AboutToCoalece (PartitionMarker* pm, void** infoRecord) const noexcept
{
    AssertNotNull (fImager);

    /*
     *  If we are deleting the current marker on the top of the screen -
     *  we must update that marker to refer to its following marker.
     */
    PartitionMarker* newTopLine = NULL;
    if (pm == fImager->fTopLinePartitionMarkerInWindow) {
        if (pm->GetNext () == NULL) {
            newTopLine = fImager->fTopLinePartitionMarkerInWindow->GetPrevious ();
        }
        else {
            newTopLine = fImager->fTopLinePartitionMarkerInWindow->GetNext ();
        }
        AssertNotNull (newTopLine);
    }

    *infoRecord = newTopLine;
}

void SimpleTextImager::MyPartitionWatcher::DidCoalece (void* infoRecord) const noexcept
{
    AssertNotNull (fImager);

    PartitionMarker* newTopLine = reinterpret_cast<PartitionMarker*> (infoRecord);

    // If we must change the top-row - then do so NOW - cuz NOW its safe to call GetRowCount ()
    if (newTopLine != NULL) {
        fImager->SetTopRowInWindow_ (RowReference (newTopLine));
    }
    AssertNotNull (fImager->fTopLinePartitionMarkerInWindow); // can't delete last one...
    AssertMember (fImager->fTopLinePartitionMarkerInWindow, PartitionMarker);
    fImager->InvalidateTotalRowsInWindow ();
}

/*
 ********************************************************************************
 ********************************* SimpleTextImager *****************************
 ********************************************************************************
 */
SimpleTextImager::SimpleTextImager ()
    : fICreatedPartition (false)
    , fMyPartitionWatcher ()
    , fRowHeight (Led_Distance (-1))
    , fInterlineSpace (0)
    , fTopLinePartitionMarkerInWindow (NULL)
    , fTotalRowsInWindow (0) // value must be computed
{
}

SimpleTextImager::~SimpleTextImager ()
{
    Assert (fTopLinePartitionMarkerInWindow == NULL);
}

void SimpleTextImager::HookLosingTextStore ()
{
    inherited::HookLosingTextStore ();
    HookLosingTextStore_ ();
}

void SimpleTextImager::HookLosingTextStore_ ()
{
    InvalidateTotalRowsInWindow ();
    fTopLinePartitionMarkerInWindow = NULL;
    // Only if we created the partition should we delete it. If it was set by external SetPartition () call, don't unset it here.
    if (fICreatedPartition) {
        fMyPartitionWatcher.UnInit (GetPartition ());
        fICreatedPartition = false;
        inherited::SetPartition (NULL); // Call inherited so we avoid creating a new partition - PeekAtTextStore () still non-NULL, but soon to be NULL
    }
}

void SimpleTextImager::HookGainedNewTextStore ()
{
    inherited::HookGainedNewTextStore ();
    HookGainedNewTextStore_ ();
}

void SimpleTextImager::HookGainedNewTextStore_ ()
{
    Require (fTopLinePartitionMarkerInWindow == NULL);
    if (GetPartition ().get () == nullptr) {
        SetPartition (NULL); // sets default one in since we have a textstore...
    }
    Assert (fTopLinePartitionMarkerInWindow == NULL);
    fTopLinePartitionMarkerInWindow = GetFirstPartitionMarker ();
    AssureWholeWindowUsedIfNeeded ();
    InvalidateScrollBarParameters (); // even if we don't change the top row, we might change enuf about the text to change sbar
}

/*
@METHOD:        SimpleTextImager::SetPartition
@DESCRIPTION:   <p>See @'PartitioningTextImager::SetPartition'. Same, but if called with NULL (and we
            have a TextStore) then make a default partition.</p>
*/
void SimpleTextImager::SetPartition (const PartitionPtr& partitionPtr)
{
    fMyPartitionWatcher.UnInit (GetPartition ());
    inherited::SetPartition (partitionPtr);
    fICreatedPartition = false;
    if (GetPartition ().get () == nullptr and PeekAtTextStore () != NULL) {
        inherited::SetPartition (PartitionPtr (new LineBasedPartition (GetTextStore ())));
        fICreatedPartition = true;
    }
    fMyPartitionWatcher.Init (GetPartition (), this);
}

PartitioningTextImager::PartitionPtr SimpleTextImager::MakeDefaultPartition () const
{
    return PartitionPtr (new LineBasedPartition (GetTextStore ()));
}

/*
@METHOD:        SimpleTextImager::ReCalcRowHeight
@DESCRIPTION:   <p>Called by @'SimpleTextImager::GetRowHeight' on an as-needed basis (when invalidated by
            @'SimpleTextImager::InvalidateRowHeight'). All rows in a SimpleTextImager are the same height.
            By default this simple uses MeasureSegmentHeight_ (GetDefaultFont (), 0, 0) to establish the per-row
            height. You can override this to provide whatever height you wish. But anything smaller will look cut-off.</p>
*/
Led_Distance SimpleTextImager::ReCalcRowHeight () const
{
    return MeasureSegmentHeight_ (GetDefaultFont (), 0, 0);
}

Led_Distance SimpleTextImager::MeasureSegmentHeight (size_t /*from*/, size_t /*to*/) const
{
    //return (MeasureSegmentHeight_ (GetDefaultFont (), from, to));
    return GetRowHeight ();
}

Led_Distance SimpleTextImager::MeasureSegmentBaseLine (size_t /*from*/, size_t /*to*/) const
{
    // If the user specifies an unusually large row-height, by default make this whitespace become above the
    // baseline, and not below.
    Led_Distance rh   = GetRowHeight ();
    Led_Distance sbrh = MeasureSegmentHeight_ (GetDefaultFont (), 0, 0);
    Led_Distance bl   = MeasureSegmentBaseLine_ (GetDefaultFont (), 0, 0);
    if (rh > sbrh) {
        bl += (rh - sbrh);
    }
    return bl;
}

bool SimpleTextImager::GetIthRowReferenceFromHere (RowReference* adjustMeInPlace, ptrdiff_t ith) const
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

size_t SimpleTextImager::GetRowNumber (RowReference rowRef) const
{
    // NB: This routine is VERY EXPENSIVE, if the text above the given row has not yet been wrapped, since
    // it forces a wrap. This is meant only to be a convenient code-saver in implementing rownumber based
    // APIs - even though their use is discouraged...
    size_t rowNumber = 0;
    AssertNotNull (rowRef.GetPartitionMarker ());
    for (PartitionMarker* cur = rowRef.GetPartitionMarker ()->GetPrevious ();
         cur != NULL; cur     = cur->GetPrevious ()) {
        rowNumber += 1;
    }
    return (rowNumber);
}

size_t SimpleTextImager::CountRowDifference (RowReference lhs, RowReference rhs) const
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
                        ((lhsMarkerStart == rhsMarkerStart)));
    RowReference     firstRowRef    = leftSmaller ? lhs : rhs;
    RowReference     lastRowRef     = leftSmaller ? rhs : lhs;

    size_t rowsGoneBy = 0;
    for (RowReference cur = firstRowRef; cur != lastRowRef; rowsGoneBy++) {
        [[maybe_unused]] bool result = GetIthRowReferenceFromHere (&cur, 1);
        Assert (result);
    }
    return rowsGoneBy;
}

size_t SimpleTextImager::GetTopRowInWindow () const
{
    // NB: Use of this function is discouraged as it is inefficent in the presence of word-wrapping
    return (GetRowNumber (GetTopRowReferenceInWindow ()));
}

size_t SimpleTextImager::GetTotalRowsInWindow () const
{
    return GetTotalRowsInWindow_ ();
}

size_t SimpleTextImager::GetLastRowInWindow () const
{
    // NB: Use of this function is discouraged as it is inefficent in the presence of word-wrapping
    return (GetRowNumber (GetLastRowReferenceInWindow ()));
}

void SimpleTextImager::SetTopRowInWindow (size_t newTopRow)
{
    // NB: Use of this function is discouraged as it is inefficent in the presence of word-wrapping
    Assert (newTopRow >= 0);

    SetTopRowInWindow (GetIthRowReference (newTopRow));

    Assert (GetTopRowInWindow () == newTopRow); // Since a SetTopRowInWindow() was called - all the
    // intervening lines have been wrapped anyhow - may
    // as well check we have our definitions straight...
}

void SimpleTextImager::AssureWholeWindowUsedIfNeeded ()
{
    SetTopRowInWindow (GetTopRowReferenceInWindow ());
}

size_t SimpleTextImager::GetMarkerPositionOfStartOfWindow () const
{
    return (GetStartOfRow (GetTopRowReferenceInWindow ()));
}

size_t SimpleTextImager::GetMarkerPositionOfEndOfWindow () const
{
    return GetEndOfRow (GetLastRowReferenceInWindow ());
}

size_t SimpleTextImager::GetMarkerPositionOfStartOfLastRowOfWindow () const
{
    return GetStartOfRow (GetLastRowReferenceInWindow ());
}

ptrdiff_t SimpleTextImager::CalculateRowDeltaFromCharDeltaFromTopOfWindow (long deltaChars) const
{
    Assert (long(GetMarkerPositionOfStartOfWindow ()) >= 0 - deltaChars);
    size_t       pos       = long(GetMarkerPositionOfStartOfWindow ()) + deltaChars;
    RowReference targetRow = GetRowReferenceContainingPosition (pos);
    size_t       rowDiff   = CountRowDifference (targetRow, GetTopRowReferenceInWindow ());
    return (deltaChars >= 0) ? rowDiff : -long(rowDiff);
}

ptrdiff_t SimpleTextImager::CalculateCharDeltaFromRowDeltaFromTopOfWindow (ptrdiff_t deltaRows) const
{
    RowReference row = GetIthRowReferenceFromHere (GetTopRowReferenceInWindow (), deltaRows);
    return (long(GetStartOfRow (row)) - long(GetMarkerPositionOfStartOfWindow ()));
}

void SimpleTextImager::ScrollByIfRoom (ptrdiff_t downByRows)
{
    RowReference newTopRow = GetTopRowReferenceInWindow ();
    (void)GetIthRowReferenceFromHere (&newTopRow, downByRows); // ignore result cuz we did say - IF-ROOM!
    SetTopRowInWindow (newTopRow);
}

/*
@METHOD:        SimpleTextImager::ScrollSoShowing
@DESCRIPTION:   <p>Implement @'TextImager::ScrollSoShowing' API.</p>
*/
void SimpleTextImager::ScrollSoShowing (size_t markerPos, size_t andTryToShowMarkerPos)
{
    Assert (markerPos >= 0);
    Assert (markerPos <= GetLength ()); // Allow any marker position (not just character?)
    Assert (fTotalRowsInWindow == 0 or fTotalRowsInWindow == ComputeRowsThatWouldFitInWindowWithTopRow (GetTopRowReferenceInWindow ()));

    if (andTryToShowMarkerPos == 0) { // special flag indicating we don't care...
        andTryToShowMarkerPos = markerPos;
    }
    Assert (andTryToShowMarkerPos >= 0);
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

    {
        // Speed hack recomended by Remo (roughly) - SPR#0553
        // if our target is closer to 0, then to the current window top, start at 0.
        // Similarly if we are closer to the end ( 0 .. M .. E } ==> M + (E-M)/2 ==> (E+M)/2
        if (markerPos < newTop.GetPartitionMarker ()->GetStart () / 2) {
            newTop = GetRowReferenceContainingPosition (0);
        }
        else if (markerPos > (GetEnd () + newTop.GetPartitionMarker ()->GetEnd ()) / 2) {
            newTop = GetRowReferenceContainingPosition (GetEnd ());
        }
    }

    while (markerPos < newTop.GetPartitionMarker ()->GetStart ()) {
        newTop = RowReference (newTop.GetPartitionMarker ()->GetPrevious ());
    }
    // only try scrolling down at all if we don't already fit in the window - cuz otherwise - we could artificially
    // scroll when not needed.
    if (not PositionWouldFitInWindowWithThisTopRow (markerPos, newTop)) {
        while (markerPos > newTop.GetPartitionMarker ()->GetEnd ()) {
            if (newTop.GetPartitionMarker ()->GetNext () == NULL) {
                // could be going to row IN last line
                break;
            }
            newTop = RowReference (newTop.GetPartitionMarker ()->GetNext ());
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
    if (CountRowDifference (originalTop, newTop) > kRowMoveThreshold) {
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

void SimpleTextImager::SetTopRowInWindow (RowReference row)
{
    if (GetForceAllRowsShowing ()) {
        row = AdjustPotentialTopRowReferenceSoWholeWindowUsed (row);
    }
    if (row != GetTopRowReferenceInWindow ()) {
        SetTopRowInWindow_ (row);
        InvalidateScrollBarParameters ();
    }
}

void SimpleTextImager::Draw (const Led_Rect& subsetToDraw, bool printing)
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
        size_t       totalRowsInWindow = GetTotalRowsInWindow_ ();
        RowReference topRowInWindow    = GetTopRowReferenceInWindow ();
        size_t       rowsLeftInWindow  = totalRowsInWindow;
        for (PartitionMarker* pm = topRowInWindow.GetPartitionMarker (); rowsLeftInWindow != 0; pm = pm->GetNext ()) {
            Assert (pm != NULL);
            size_t startSubRow = 0;
            size_t maxSubRow   = 0;
            maxSubRow          = rowsLeftInWindow - 1 + startSubRow;
            size_t rowsDrawn   = 0;
            DrawPartitionElement (pm, startSubRow, maxSubRow, tablet,
                                  (GetImageUsingOffscreenBitmaps () and not printing) ? &thisIsOurNewOST : NULL,
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

            // SEE IF WE CAN TIGHTEN THIS TEST A BIT MORE, SO WHEN NO PIXELS WILL BE DRAWN, WE DONT BOTHER
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
@METHOD:        SimpleTextImager::DrawPartitionElement
@DESCRIPTION:   <p></p>
*/
void SimpleTextImager::DrawPartitionElement (PartitionMarker* pm, size_t startSubRow, size_t /*maxSubRow*/, Led_Tablet tablet, OffscreenTablet* offscreenTablet, bool printing, const Led_Rect& subsetToDraw, Led_Rect* remainingDrawArea, size_t* rowsDrawn)
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

    Led_Tablet savedTablet = tablet;
    size_t     endSubRow   = 0;
    *rowsDrawn             = 0;

    for (size_t subRow = startSubRow; subRow <= endSubRow; ++subRow) {
        Led_Rect currentRowRect     = *remainingDrawArea;
        currentRowRect.bottom       = currentRowRect.top + GetRowHeight ();
        Led_Distance interlineSpace = GetInterLineSpace ();
        if (
            (currentRowRect.bottom + Led_Coordinate (interlineSpace) > subsetToDraw.top) and
            (currentRowRect.top < subsetToDraw.bottom)) {

            /*
             *  patch start/end/len to take into account rows...
             */
            size_t rowStart = start;
            size_t rowEnd   = end;

            // rowEnd is just BEFORE the terminating NL - if any...
            {
                Assert (rowEnd < GetEnd () + 1);
                size_t prevToEnd = FindPreviousCharacter (rowEnd);
                if (prevToEnd >= rowStart and prevToEnd < GetEnd ()) {
                    Led_tChar lastChar;
                    CopyOut (prevToEnd, 1, &lastChar);
                    if (RemoveMappedDisplayCharacters (&lastChar, 1) == 0) {
                        rowEnd = prevToEnd;
                    }
                }
            }

            TextLayoutBlock_Copy rowText = GetTextLayoutBlock (rowStart, rowEnd);

            if (offscreenTablet != NULL) {
                tablet = offscreenTablet->PrepareRect (currentRowRect, interlineSpace);
            }

            {
                Led_Rect invalidRowRect = currentRowRect;
                invalidRowRect.left     = subsetToDraw.left;
                invalidRowRect.right    = subsetToDraw.right;
                // PERHAPS should also intersect the 'subsetToDraw' top/bottom with this invalidRowRect to get better logical clipping?
                // Maybe important for tables!!!
                // LGP 2002-09-22
                //              DrawRow (RowReference (pm), tablet, currentRowRect, invalidRowRect, rowText, rowStart, rowEnd, printing);
                DrawRow (tablet, currentRowRect, invalidRowRect, rowText, rowStart, rowEnd, printing);
            }

            /*
             *  Now erase/draw any interline space.
             */
            if (interlineSpace != 0) {
                size_t hilightStart          = GetSelectionStart ();
                size_t hilightEnd            = GetSelectionEnd ();
                bool   segmentHilightedAtEnd = GetSelectionShown () and (hilightStart < rowEnd) and (rowEnd <= hilightEnd);
                if (pm->GetNext () == NULL) {
                    segmentHilightedAtEnd = false; // last row always contains no NL - so no invert off to the right...
                }
                DrawInterLineSpace (interlineSpace, tablet, currentRowRect.bottom, segmentHilightedAtEnd, printing);
            }

            if (offscreenTablet != NULL) {
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

Led_Distance SimpleTextImager::ComputeMaxHScrollPos () const
{
    Led_Distance maxHWidth = 0;
    {
        /*
         *  Figure the largest amount we might need to scroll given the current windows contents.
         *  But take into account where we've scrolled so far, and never invalidate that
         *  scroll amount. Always leave at least as much layout-width as needed to
         *  preserve the current scroll-to position.
         */
        Led_Distance width = CalculateLongestRowInWindowPixelWidth ();
        if (GetHScrollPos () != 0) {
            width = max (width, GetHScrollPos () + GetWindowRect ().GetWidth ());
        }
        maxHWidth = max<Led_Distance> (width, 1);
    }
    Led_Distance wWidth = GetWindowRect ().GetWidth ();
    if (maxHWidth > wWidth) {
        return (maxHWidth - wWidth);
    }
    else {
        return 0;
    }
}

Led_Rect SimpleTextImager::GetCharLocation (size_t afterPosition) const
{
    return (GetCharLocationRowRelative (afterPosition, RowReference (GetFirstPartitionMarker ())));
}

Led_Rect SimpleTextImager::GetCharWindowLocation (size_t afterPosition) const
{
    Led_Point windowOrigin = GetWindowRect ().GetOrigin () - Led_Point (0, GetHScrollPos ());
    return (windowOrigin +
            GetCharLocationRowRelative (afterPosition, GetTopRowReferenceInWindow (), GetTotalRowsInWindow_ ()));
}

size_t SimpleTextImager::GetCharAtLocation (const Led_Point& where) const
{
    return (GetCharAtLocationRowRelative (where, RowReference (GetFirstPartitionMarker ())));
}

size_t SimpleTextImager::GetCharAtWindowLocation (const Led_Point& where) const
{
    Led_Point windowOrigin = GetWindowRect ().GetOrigin () - Led_Point (0, GetHScrollPos ());
    return (GetCharAtLocationRowRelative (where - windowOrigin,
                                          GetTopRowReferenceInWindow (),
                                          GetTotalRowsInWindow_ ()));
}

size_t SimpleTextImager::GetStartOfRow (size_t rowNumber) const
{
    // NB: Use of routines using rowNumbers force word-wrap, and so can be quite slow.
    // Routines using RowReferences often perform MUCH better
    return (GetStartOfRow (GetIthRowReference (rowNumber)));
}

size_t SimpleTextImager::GetStartOfRowContainingPosition (size_t charPosition) const
{
    return (GetStartOfRow (GetRowReferenceContainingPosition (charPosition)));
}

size_t SimpleTextImager::GetEndOfRow (size_t rowNumber) const
{
    // NB: Use of routines using rowNumbers force word-wrap, and so can be quite slow.
    // Routines using RowReferences often perform MUCH better
    return (GetEndOfRow (GetIthRowReference (rowNumber)));
}

size_t SimpleTextImager::GetEndOfRowContainingPosition (size_t charPosition) const
{
    return (GetEndOfRow (GetRowReferenceContainingPosition (charPosition)));
}

size_t SimpleTextImager::GetRealEndOfRow (size_t rowNumber) const
{
    // NB: Use of routines using rowNumbers force word-wrap, and so can be quite slow.
    // Routines using RowReferences often perform MUCH better
    return (GetRealEndOfRow (GetIthRowReference (rowNumber)));
}

size_t SimpleTextImager::GetRealEndOfRowContainingPosition (size_t charPosition) const
{
    return (GetRealEndOfRow (GetRowReferenceContainingPosition (charPosition)));
}

size_t SimpleTextImager::GetStartOfRow (RowReference row) const
{
    PartitionMarker* cur = row.GetPartitionMarker ();
    AssertNotNull (cur);
    return (cur->GetStart ());
}

size_t SimpleTextImager::GetEndOfRow (RowReference row) const
{
    size_t markerEnd = GetRealEndOfRow (row);
    // Be careful about NL at end. If we end with an NL, then don't count that.
    // And for the last PM - it contains a bogus empty character. Don't count
    // that either.
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

size_t SimpleTextImager::GetRealEndOfRow (RowReference row) const
{
    PartitionMarker* cur = row.GetPartitionMarker ();
    AssertNotNull (cur);
    Assert (cur->GetEnd () > 0);
    size_t markerEnd = cur->GetEnd ();
    Assert (markerEnd <= GetLength () + 1);
    return (markerEnd);
}

SimpleTextImager::RowReference SimpleTextImager::GetRowReferenceContainingPosition (size_t charPosition) const
{
    Require (charPosition >= 0);
    Require (charPosition <= GetEnd ());
    PartitionMarker* pm = GetPartitionMarkerContainingPosition (charPosition);
    AssertNotNull (pm);
    return (RowReference (pm));
}

size_t SimpleTextImager::GetRowContainingPosition (size_t charPosition) const
{
    return (GetRowNumber (GetRowReferenceContainingPosition (charPosition)));
}

size_t SimpleTextImager::GetRowCount () const
{
    // NB: This is an expensive routine because it forces a word-wrap on all the text!
    size_t rowCount = 0;
    for (PartitionMarker* cur = GetFirstPartitionMarker (); cur != NULL; cur = cur->GetNext ()) {
        [[maybe_unused]] PartitionMarker* pm = cur;
        AssertNotNull (pm);
        AssertMember (pm, PartitionMarker);
        rowCount += 1;
    }
    return (rowCount);
}

Led_Rect SimpleTextImager::GetCharLocationRowRelativeByPosition (size_t afterPosition, size_t positionOfTopRow, size_t maxRowsToCheck) const
{
    return GetCharLocationRowRelative (afterPosition, GetRowReferenceContainingPosition (positionOfTopRow), maxRowsToCheck);
}

Led_Distance SimpleTextImager::GetRowHeight (size_t /*rowNumber*/) const
{
    return (GetRowHeight ());
}

/*
@METHOD:        SimpleTextImager::GetRowRelativeBaselineOfRowContainingPosition
@DESCRIPTION:   <p>Override/implement @'TextImager::GetRowRelativeBaselineOfRowContainingPosition'.</p>
*/
Led_Distance SimpleTextImager::GetRowRelativeBaselineOfRowContainingPosition (size_t charPosition) const
{
    RowReference thisRow    = GetRowReferenceContainingPosition (charPosition);
    size_t       startOfRow = GetStartOfRow (thisRow);
    size_t       endOfRow   = GetEndOfRow (thisRow);
    return MeasureSegmentBaseLine (startOfRow, endOfRow);
}

void SimpleTextImager::GetStableTypingRegionContaingMarkerRange (size_t fromMarkerPos, size_t toMarkerPos,
                                                                 size_t* expandedFromMarkerPos, size_t* expandedToMarkerPos) const
{
    AssertNotNull (expandedFromMarkerPos);
    AssertNotNull (expandedToMarkerPos);
    Assert (fromMarkerPos >= 0);
    Assert (fromMarkerPos <= toMarkerPos);
    Assert (toMarkerPos <= GetEnd ());
    if constexpr (qMultiByteCharacters && qDebug) {
        Assert_CharPosDoesNotSplitCharacter (fromMarkerPos);
        Assert_CharPosDoesNotSplitCharacter (toMarkerPos);
    }

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
        if (cur->GetNext () == NULL) {
            end--;
        }

        //size_t len = end - start;

        // If we are strictly before the first row, we won't appear later...
        if (curTopRowRelativeRowNumber == 0 and (fromMarkerPos < start)) {
            break;
        }

        curTopRowRelativeRowNumber++;

        if (Contains (*cur, fromMarkerPos) and Contains (*cur, toMarkerPos)) {
            (*expandedFromMarkerPos) = start;
            (*expandedToMarkerPos)   = end;
            Assert ((*expandedFromMarkerPos) >= 0);
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

Led_Distance SimpleTextImager::GetHeightOfRows (size_t startingRow, size_t rowCount) const
{
    return (GetHeightOfRows (GetIthRowReference (startingRow), rowCount));
}

Led_Distance SimpleTextImager::GetHeightOfRows (RowReference startingRow, size_t rowCount) const
{
    Led_Distance height = 0;
    for (RowReference curRow = startingRow; rowCount > 0; rowCount--) {
        PartitionMarker* curPM = curRow.GetPartitionMarker ();
        height += GetRowHeight ();
        height += GetInterLineSpace (curPM);
        (void)GetNextRowReference (&curRow);
    }
    return (height);
}

/*
@METHOD:        SimpleTextImager::SetInterLineSpace
@DESCRIPTION:   <p>Set the interline space associated for the entire text buffer. Call the
            no-arg version of @'SimpleTextImager::GetInterLineSpace' to get the currently set value.</p>
*/
void SimpleTextImager::SetInterLineSpace (Led_Distance interlineSpace)
{
    fInterlineSpace = interlineSpace;
}

/*
@METHOD:        SimpleTextImager::GetInterLineSpace
@DESCRIPTION:   <p>Get the interline space associated with a particular partition element. This method
            is virtual so that subclassers can provide a different interline space for each partition element.
            But the default behavior is to simply use the global - buffer value - specified by
            @'SimpleTextImager::SetInterLineSpace' and returned by the no-arg version of
            @'SimpleTextImager::GetInterLineSpace'.</p>
*/
Led_Distance SimpleTextImager::GetInterLineSpace (PartitionMarker* /*pm*/) const
{
    return GetInterLineSpace ();
}

/*
@METHOD:        SimpleTextImager::ChangedInterLineSpace
@DESCRIPTION:   <p>If you override the virtual, one-arg version of @'SimpleTextImager::GetInterLineSpace', you must
            call this method whenever the interline space value changes, for a given PM. This method will clear any caches
            associated with that PM. And refresh the screen, as needed (in some subclass).</p>
                <p>See the one-arg @'SimpleTextImager::GetInterLineSpace' for more details.</p>
*/
void SimpleTextImager::ChangedInterLineSpace (PartitionMarker* /*pm*/)
{
    // for now - we do nothing. Not sure there are any caches - at this point.
}

void SimpleTextImager::SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont)
{
    //  TextImager::SetDefaultFont_ (defaultFont);
    inherited::SetDefaultFont (defaultFont);
    try {
        TabletChangedMetrics ();
    }
    catch (NotFullyInitialized&) {
        // ignore this - no harm...
    }
    catch (...) {
        throw;
    }
}

void SimpleTextImager::DidUpdateText (const UpdateInfo& updateInfo) noexcept
{
    InvalidateTotalRowsInWindow ();
    inherited::DidUpdateText (updateInfo);
    AssertNotNull (fTopLinePartitionMarkerInWindow);
    AssureWholeWindowUsedIfNeeded ();
    InvalidateScrollBarParameters (); // even if we don't change the top row, we might change enuf about the text to change sbar
}

void SimpleTextImager::SetWindowRect (const Led_Rect& windowRect)
{
    bool heightChanged = GetWindowRect ().GetHeight () != windowRect.GetHeight ();
    inherited::SetWindowRect (windowRect);
    if (heightChanged and PeekAtTextStore () != NULL) {
        InvalidateTotalRowsInWindow ();
        AssureWholeWindowUsedIfNeeded ();
        InvalidateScrollBarParameters ();
    }
}

void SimpleTextImager::InvalidateAllCaches ()
{
    inherited::InvalidateAllCaches ();
    InvalidateRowHeight ();
    if (GetPartition ().get () != nullptr) {
        // Invalidate totalRows at END of this procedure - after invalidating scrollbar params and assurewholewindowusedifneeded () - cuz
        // InvalidateScrollBarParameters/AssureWholeWindowUsedIfNeeded can revalidate some cache values...
        InvalidateScrollBarParameters ();
        AssureWholeWindowUsedIfNeeded ();
        InvalidateTotalRowsInWindow ();
    }
}

SimpleTextImager::RowReference SimpleTextImager::AdjustPotentialTopRowReferenceSoWholeWindowUsed (const RowReference& potentialTopRow)
{
    /*
     *  This check is always safe, but probably not a worthwhile optimization, except that it avoids
     *  some problems about initializing the top-row-reference before we've got a valid
     *  tablet setup to use.
     */
    if (potentialTopRow.GetPartitionMarker ()->GetPrevious () == NULL) {
        return potentialTopRow;
    }

    Led_Coordinate windowHeight = GetWindowRect ().GetHeight ();
    Led_Coordinate heightUsed   = 0;

    for (RowReference curRow = potentialTopRow;;) {
        PartitionMarker* curPM = curRow.GetPartitionMarker ();
        heightUsed += GetRowHeight ();
        heightUsed += GetInterLineSpace (curPM);
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

        PartitionMarker* curPM = curRow.GetPartitionMarker ();
        heightUsed += GetRowHeight ();
        heightUsed += GetInterLineSpace (curPM);
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

bool SimpleTextImager::PositionWouldFitInWindowWithThisTopRow (size_t markerPos, const RowReference& newTopRow)
{
    if (markerPos < GetStartOfRow (newTopRow)) {
        return false;
    }

    size_t       rowCount = ComputeRowsThatWouldFitInWindowWithTopRow (newTopRow);
    RowReference lastRow  = GetIthRowReferenceFromHere (newTopRow, rowCount - 1);

    return (markerPos < GetRealEndOfRow (lastRow));
}

size_t SimpleTextImager::ComputeRowsThatWouldFitInWindowWithTopRow (const RowReference& newTopRow) const
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
        PartitionMarker* curPM = curRow.GetPartitionMarker ();
        heightUsed += GetRowHeight ();
        heightUsed += GetInterLineSpace (curPM);
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

Led_Rect SimpleTextImager::GetCharLocationRowRelative (size_t afterPosition, RowReference topRow, size_t maxRowsToCheck) const
{
    // MUST FIGURE OUT WHAT TODO HERE BETTER - 10000 not good enough answer always...
    const Led_Rect kMagicBeforeRect = Led_Rect (-10000, 0, 0, 0);
    const Led_Rect kMagicAfterRect  = Led_Rect (10000, 0, 0, 0);

    Assert (afterPosition >= 0);
    Assert (afterPosition <= GetEnd ()); // does that ever make sense???
    if constexpr (qMultiByteCharacters && qDebug) {
        Assert_CharPosDoesNotSplitCharacter (afterPosition);
    }

    if (afterPosition < GetStartOfRow (topRow)) {
        return (kMagicBeforeRect);
    }

    RowReference   curRow                     = topRow;
    size_t         curTopRowRelativeRowNumber = 0;
    Led_Coordinate topVPos                    = 0;
    do {
        PartitionMarker* cur = curRow.GetPartitionMarker ();
        AssertNotNull (cur);
        size_t start = cur->GetStart ();
        size_t end   = cur->GetEnd (); // end points JUST PAST LAST VISIBLE/OPERATED ON CHAR

        Assert (end <= GetEnd () + 1);

        curTopRowRelativeRowNumber++;

        if (afterPosition >= start and afterPosition < end) {
            Assert (start <= afterPosition);
            Led_Distance hStart = 0;
            Led_Distance hEnd   = 0;
            GetRowRelativeCharLoc (afterPosition, &hStart, &hEnd);
            Assert (hStart <= hEnd);
            return (Led_Rect (topVPos, hStart, GetRowHeight (), hEnd - hStart));
        }

        topVPos += GetRowHeight ();
        topVPos += GetInterLineSpace (cur);

        if (curTopRowRelativeRowNumber >= maxRowsToCheck) {
            break; // return bogus place at the end...
        }
    } while (GetNextRowReference (&curRow));

    return (kMagicAfterRect);
}

size_t SimpleTextImager::GetCharAtLocationRowRelative (const Led_Point& where, RowReference topRow, size_t maxRowsToCheck) const
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
        PartitionMarker* cur = curRow.GetPartitionMarker ();

        AssertNotNull (cur);
        size_t start = cur->GetStart ();
#if qMultiByteCharacters
        Assert_CharPosDoesNotSplitCharacter (start);
#endif

        /*
         *  Count the interline space as part of the last row of the line for the purpose of hit-testing.
         */
        Led_Distance interLineSpace = GetInterLineSpace (cur);

        curTopRowRelativeRowNumber++;
        if (where.v >= topVPos and where.v < topVPos + Led_Coordinate (GetRowHeight () + interLineSpace)) {
            return GetRowRelativeCharAtLoc (where.h, start);
        }

        if (curTopRowRelativeRowNumber >= maxRowsToCheck) {
            break; // we've checked enuf...
        }

        topVPos += GetRowHeight () + interLineSpace;
    } while (GetNextRowReference (&curRow));

#if qMultiByteCharacters
    Assert_CharPosDoesNotSplitCharacter (GetLength () + 1);
#endif
    return (GetEnd ());
}

/*
@METHOD:        SimpleTextImager::ContainsMappedDisplayCharacters
@DESCRIPTION:   <p>Override @'TextImager::ContainsMappedDisplayCharacters' to hide '\n' characters.
            See @'qDefaultLedSoftLineBreakChar'.</p>
*/
bool SimpleTextImager::ContainsMappedDisplayCharacters (const Led_tChar* text, size_t nTChars) const
{
    return ContainsMappedDisplayCharacters_HelperForChar (text, nTChars, '\n') or
           inherited::ContainsMappedDisplayCharacters (text, nTChars);
}

/*
@METHOD:        SimpleTextImager::RemoveMappedDisplayCharacters
@DESCRIPTION:   <p>Override @'TextImager::RemoveMappedDisplayCharacters' to hide '\n' characters.</p>
*/
size_t SimpleTextImager::RemoveMappedDisplayCharacters (Led_tChar* copyText, size_t nTChars) const
{
    size_t newLen = inherited::RemoveMappedDisplayCharacters (copyText, nTChars);
    Assert (newLen <= nTChars);
    size_t newerLen = RemoveMappedDisplayCharacters_HelperForChar (copyText, newLen, '\n');
    Assert (newerLen <= newLen);
    Assert (newerLen <= nTChars);
    return newerLen;
}
