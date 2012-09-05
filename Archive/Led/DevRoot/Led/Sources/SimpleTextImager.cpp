/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/SimpleTextImager.cpp,v 2.80 2004/01/26 16:42:00 lewis Exp $
 *
 * Changes:
 *	$Log: SimpleTextImager.cpp,v $
 *	Revision 2.80  2004/01/26 16:42:00  lewis
 *	SPR#1604: Lose overrides of ComputeRelativePosition () since the speedtweek can be done directly in the TextImager::ComputeRelativePosition () version using GetStartOfNextRowFromRowContainingPosition () etc.
 *	
 *	Revision 2.79  2003/11/27 20:22:20  lewis
 *	SPR#1566: use GetTextLayoutBlock () in SimpleTextImager::DrawPartitionElement (already was doing that in MRTI::DrawPartitionElement) instead of direct TextLayoutBlock_Basic CTOR call. Not really faster now - but will be if we cache these records.
 *	
 *	Revision 2.78  2003/02/28 19:07:44  lewis
 *	SPR#1316- CRTDBG_MAP_ALLOC/CRTDBG_MAP_ALLOC_NEW Win32/MSVC debug mem leak support
 *	
 *	Revision 2.77  2003/01/29 18:12:38  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.76  2003/01/29 17:59:53  lewis
 *	SPR#1264- Get rid of most of the SetDefaultFont overrides and crap - and made InteractiveSetFont
 *	REALLY do the interactive command setfont - and leave SetDefaultFont to just setting the 'default font'
 *	
 *	Revision 2.75  2003/01/17 00:41:01  lewis
 *	fix small bug in SimpleTextImager::DrawPartitionElement () so it respets new semantics for DrawRow:
 *	assure rowEnd is the LOGICAL - not REAL row end
 *	
 *	Revision 2.74  2002/12/21 03:02:23  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.73  2002/12/21 03:00:11  lewis
 *	SPR#1217. Added virtual TextImager::InvalidateAllCaches and made SimpleTextImager/MRTI versions
 *	just overrides. No longer need SimpleTextImager::TabletChangedMetrics overrides etc
 *	since now TextImager::TabletChangedMetrics calls TextImager::InvalidateAllCaches.
 *	MultiRowTextImager::PurgeUnneededMemory () no longer needed since TextImager version
 *	now calls InvalidateAllCaches().
 *	
 *	Revision 2.72  2002/12/20 17:56:32  lewis
 *	SPR#1216- get rid of override of GetSelectionWindowRegion. New version in TextImager
 *	now properly handles BIDI, and interline space (interline space not tested,but
 *	not really used much anymore??).
 *	And - fix one very old zero-based index vs. one-based index bug.
 *	
 *	Revision 2.71  2002/12/12 16:53:23  lewis
 *	minor fix to SimpleTextImager::GetStableTypingRegionContaingMarkerRange- if not in window -
 *	use 0 as start instead of 1 (bug from old one-based Led days?)
 *	
 *	Revision 2.70  2002/12/06 15:15:42  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.69  2002/12/06 15:13:41  lewis
 *	SPR#1198 - added 2 new APIs to TextImager, implemented them in PartitioningTextImager,
 *	and used  them in MultiRowTextImager and SimpleTextImager:
 *		GetRowRelativeCharLoc ()
 *		GetRowRelativeCharAtLoc ().
 *	
 *	Revision 2.68  2002/12/04 15:37:52  lewis
 *	SPR#1193- replace calls to CalcSegmentSizeWithImmediateText with just CalcSegmentSize
 *	
 *	Revision 2.67  2002/12/02 16:01:30  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.66  2002/12/02 15:58:07  lewis
 *	SPR#1185 - move DrawRow,DrawRowSegments,DrawRowHilight,DrawInterlineSpace from MRTI/SimpleTextImager
 *	to TextImager (share code. Changed their args slightly. For BIDI work.
 *	
 *	Revision 2.65  2002/12/02 00:52:44  lewis
 *	use TextLayoutBlock for arg to DrawRow etc functions - as I did with MultiRowTextImager - part of SPR#1183 - BIDI
 *	
 *	Revision 2.64  2002/10/30 15:21:23  lewis
 *	use Rect::GetHeight instead of obsolete GetRectHeight() etc (see SPR#1155)
 *	
 *	Revision 2.63  2002/10/30 13:29:24  lewis
 *	SPR#1155 - Added new Rect_Base<> template and made Led_Rect a simple typedef of that. Then TRIED (failed) to make the
 *	top/left/bottom/right fields private by adding accessors. Added the accessors and used them in tons of places - but still
 *	TONS more required. Leave that battle for another day. I'm not even SURE its a good idea. Anyhow - also added a new
 *	typedef for Led_TWIPS_Rect (this was what I was after to begin with with all this nonsese)
 *	
 *	Revision 2.62  2002/10/25 17:41:28  lewis
 *	SPR#1152 - override SimpleTextImager::MakeDefaultPartition
 *	
 *	Revision 2.61  2002/09/20 16:00:01  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.60  2002/05/06 21:33:49  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.59  2001/11/27 00:29:55  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.58  2001/10/17 21:46:42  lewis
 *	massive DocComment cleanups (mostly <p>/</p> fixups)
 *	
 *	Revision 2.57  2001/10/16 15:34:43  lewis
 *	SPR#1062- as a side-effect of this SPR - added default implementation of ComputeMaxHScrollPos ()
 *	
 *	Revision 2.56  2001/09/26 15:41:20  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.55  2001/09/24 14:16:42  lewis
 *	SPR#1042- cleaned up region support. SPR#0989- fixed flicker problem: improve logical
 *	clipping on final EraseBackground call - and stick that in the offscreen bitmap (to kill flicker)
 *	
 *	Revision 2.54  2001/09/13 02:55:36  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.53  2001/09/12 14:53:32  lewis
 *	SPR#0987- Added conditional UseSecondaryHilight support (macos&win32 only so far).
 *	Redo Led_Region support for MacOS so now more like with PC (no operator conversion
 *	to macregion and auto constructs OSRegion except when called with region arg - in
 *	whcih case we dont OWN region and dont delete it on DTOR
 *	
 *	Revision 2.52  2001/08/28 18:43:37  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.51  2001/08/27 20:36:05  lewis
 *	SPR#0978- code I added for (GetImageUsingOffscreenBitmaps() didn't check
 *	'printing' flag in some cases it needed to
 *	
 *	Revision 2.50  2001/05/12 22:50:57  lewis
 *	SPR#0917- new TextImager::G/SetImageUsingOffscreenBitmaps () method to
 *	replace obsolete qUseOffscreenBitmapsToReduceFlicker
 *	
 *	Revision 2.49  2001/05/12 22:29:16  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.48  2001/05/11 23:39:34  lewis
 *	SPR#0915- Added class OffscreenTablet to abstract away common code from
 *	MultiRowTextImager/SimpleTextImager to handle offscreen bitmap drawing.
 *	Much cleanedup (HOPEFULLY not broken).
 *	
 *	Revision 2.47  2001/05/01 14:37:12  lewis
 *	fix small GCC warnings
 *	
 *	Revision 2.46  2001/04/18 20:23:59  lewis
 *	SPR#0873- fix assert error triggered cuz of order of calls in
 *	SimpleTextImager::InvalidateAllCaches ()
 *	
 *	Revision 2.45  2000/11/03 16:51:44  lewis
 *	SPR#0842- fix horizontal autoscroll (scrollsoshowing) when happens WITH
 *	vertical autoscroll, as in a find command
 *	
 *	Revision 2.44  2000/09/28 15:12:00  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.43  2000/06/17 12:56:48  lewis
 *	implement override of TextImager::GetRowRelativeBaselineOfRowContainingPosition
 *	
 *	Revision 2.42  2000/06/12 20:06:48  lewis
 *	SPR#0780- new Led_Tablet_ stuff - supported on Mac
 *	
 *	Revision 2.41  2000/06/12 17:07:57  lewis
 *	SPR#0780- use new Led_Tablet_ code
 *	
 *	Revision 2.40  2000/04/15 14:32:37  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.39  2000/04/14 22:40:26  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.38  2000/04/05 21:45:50  lewis
 *	forgot on last checkin - also  SPR#0734 - call new GetIntraRowTextWindowBoundingRect
 *	
 *	Revision 2.37  2000/04/05 21:44:13  lewis
 *	SPR#0470 fixed - curious assert failure now understood and clarified
 *	
 *	Revision 2.36  2000/03/11 22:04:49  lewis
 *	SPR#0711 - new GetRealEndOfRow* () routines. And fixed PositionWouldFitInWindowWithThisTopRow ()
 *	to call it. And other small fixes to TextInteractor::ScrollToSelection ()
 *	and Simple/MultiRow::ScrollSoShowing ()
 *	
 *	Revision 2.35  2000/03/09 04:40:30  lewis
 *	replace old MultiRowTextImager::HilightARectanle_ (and SimpleTextImager version)
 *	with TextImager::HilightARectanlge() ETC - SPR#0708
 *	
 *	Revision 2.34  2000/01/22 20:51:34  lewis
 *	SPR#0499- fix XXX::ScrollSoShowing () to vertically CENTER the interesting text when its
 *	forced to scroll more than some threshold amount (1 row for now)
 *	
 *	Revision 2.33  2000/01/22 17:34:58  lewis
 *	Added TextImager::ScrollSoShowingHHelper () to share a little code from SimpleTextImager::ScrollSoShowing /
 *	MultiRowTextImager::ScrollSoShowing () - code cleanups
 *	
 *	Revision 2.32  1999/12/23 17:23:06  lewis
 *	spr#0677 - small fix to TextImager::ComputeRelativePosition () for by-row cursoring
 *	
 *	Revision 2.31  1999/12/21 21:27:52  lewis
 *	override ContainsMappedDisplayCharacters/RemoveMappedDisplayCharacters () so ::GetEndOfRow()
 *	code works right - knows we remove the \ns
 *	
 *	Revision 2.30  1999/12/21 21:03:28  lewis
 *	Use same MRTIM::GetEndOfRow() hack here - check (RemoveMappedDisplayCharacters() instead
 *	of specificly checking for \n
 *	
 *	Revision 2.29  1999/12/21 20:55:11  lewis
 *	Use new Led_ThrowIfNull instead of manual check. And lose (I HOPE) obsolete code which
 *	was checkingf for \n (as I had in MultiRowTextImager code)
 *	
 *	Revision 2.28  1999/12/15 03:07:25  lewis
 *	was wrong to catch NoTabletAvailable&- cuz subclasses from other catcher
 *	
 *	Revision 2.27  1999/12/14 21:23:40  lewis
 *	ingore NoTabletAvailable& throws as well
 *	
 *	Revision 2.26  1999/12/14 18:09:33  lewis
 *	silence GCC warning
 *	
 *	Revision 2.25  1999/11/29 21:10:59  lewis
 *	PatPtr SB const Pattern* for MacOS Wrapper functions.
 *	
 *	Revision 2.24  1999/11/29 20:48:23  lewis
 *	Use Led_GetCurrentGDIPort () instead of peeking at 'qd' global (and kBlack constnat). SPR#0634 (MACONLY).
 *	
 *	Revision 2.23  1999/11/15 21:30:12  lewis
 *	Don't refer to CDC or CBitmap directly (MFC classes). Instead of Led_Tablet etc - my own
 *	private Win32SDK wrappers - so no more dependency on MFC. AddRectangleToRegion () now moved to shared code
 *	
 *	Revision 2.22  1999/11/13 16:32:22  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.21  1999/05/03 22:05:13  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.20  1999/03/24 15:47:25  lewis
 *	spr#0561 fixed- TexImager::NoTabletAvailable now subclasses from TextImager::NotFullyInitialized etc
 *	
 *	Revision 2.19  1999/03/04 14:35:58  lewis
 *	Allow SetWindowRect() call before having specified TextStore - by checking if non-null.
 *	Helpful in initialization
 *	
 *	Revision 2.18  1999/03/03 00:31:34  lewis
 *	fRowHeight() starts out as -1, so computed the first time
 *	
 *	Revision 2.17  1999/03/02 16:19:12  lewis
 *	spr#0553 - speed tweek for REMO
 *	
 *	Revision 2.16  1998/07/24 01:12:33  lewis
 *	GetInterlineSpace() virtual/takes PM arg - stuff form REMO.
 *	
 *	Revision 2.15  1998/06/03  01:33:59  lewis
 *	Fix spr#0542, interlinespace wasn't taken into account in offscreen bitmap.
 *
 *	Revision 2.14  1998/04/08  01:49:53  lewis
 *	Support new Partition design (change base class etc).
 *
 *	Revision 2.13  1997/12/24  04:40:13  lewis
 *	*** empty log message ***
 *
 *	Revision 2.12  1997/12/24  03:35:12  lewis
 *	Support new GetRowHeight() accessor function (to satisfy simplication/request of Remo).
 *
 *	Revision 2.11  1997/09/29  15:12:17  lewis
 *	Whole new implementation, baesd on code from MultiRowTextImager, but with most of the
 *	multirow stuff ripped out (for speed, less memory usage).-- spr#0062.
 *
 *	Revision 2.10  1997/07/27  15:58:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.9  1997/07/12  20:13:05  lewis
 *	qLedFirstIndex support.
 *	TextImager_ renamed TextImager.
 *
 *	Revision 2.8  1997/06/18  20:08:49  lewis
 *	*** empty log message ***
 *
 *	Revision 2.7  1997/06/18  03:00:10  lewis
 *	Cleanus, and ResetTabStops takes PM arg.
 *
 *	Revision 2.6  1997/01/10  03:16:03  lewis
 *	Whole new (though very prelimary, but still functional) non-word-wrapping imager.
 *
 *	Revision 2.5  1996/12/13  17:58:46  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.4  1996/12/05  19:11:38  lewis
 *	*** empty log message ***
 *
 *	Revision 2.3  1996/09/01  15:31:50  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.2  1996/03/16  18:53:44  lewis
 *	*** empty log message ***
 *
 *	Revision 2.1  1995/09/06  20:57:00  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.3  1995/05/30  00:07:03  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.2  1995/03/02  05:46:24  lewis
 *	*** empty log message ***
 *
 *	Revision 1.1  1995/03/02  05:30:46  lewis
 *	Initial revision
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<limits.h>

#include	"LedSupport.h"
#include	"LineBasedPartition.h"

#include	"SimpleTextImager.h"



#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif




#if		qLedUsesNamespaces
namespace	Led {
#endif






typedef	Partition::PartitionMarker				PartitionMarker;
typedef	PartitioningTextImager::PartitionPtr	PartitionPtr;






/*
 ********************************************************************************
 ********************** SimpleTextImager::MyPartitionWatcher ********************
 ********************************************************************************
 */
void	SimpleTextImager::MyPartitionWatcher::Init (PartitionPtr partition, SimpleTextImager* imager)
{
	fImager = imager;
	if (not partition.IsNull ()) {
		partition->AddPartitionWatcher (this);
	}
}

void	SimpleTextImager::MyPartitionWatcher::UnInit (PartitionPtr partition)
{
	fImager = NULL;
	if (not partition.IsNull ()) {
		partition->RemovePartitionWatcher (this);
	}
}

void	SimpleTextImager::MyPartitionWatcher::AboutToSplit (PartitionMarker* /*pm*/, size_t /*at*/, void** /*infoRecord*/) const throw ()
{
	Led_AssertNotNil (fImager);
}
				
void	SimpleTextImager::MyPartitionWatcher::DidSplit (void* /*infoRecord*/) const throw ()
{
	Led_AssertNotNil (fImager);
	fImager->InvalidateTotalRowsInWindow ();
}
				
void	SimpleTextImager::MyPartitionWatcher::AboutToCoalece (PartitionMarker* pm, void** infoRecord) const throw ()
{
	Led_AssertNotNil (fImager);

	/*
	 *	If we are deleting the current marker on the top of the screen -
	 *	we must update that marker to refer to its following marker.
	 */
	PartitionMarker*	newTopLine	=	NULL;
	if (pm == fImager->fTopLinePartitionMarkerInWindow) {
		if (pm->GetNext () == NULL) {
			newTopLine = fImager->fTopLinePartitionMarkerInWindow->GetPrevious ();
		}
		else {
			newTopLine = fImager->fTopLinePartitionMarkerInWindow->GetNext ();
		}
		Led_AssertNotNil (newTopLine);
	}

	*infoRecord = newTopLine;
}

void	SimpleTextImager::MyPartitionWatcher::DidCoalece (void* infoRecord) const throw ()
{
	Led_AssertNotNil (fImager);

	PartitionMarker*	newTopLine	=	reinterpret_cast<PartitionMarker*> (infoRecord);

	// If we must change the top-row - then do so NOW - cuz NOW its safe to call GetRowCount ()
	if (newTopLine != NULL) {
		fImager->SetTopRowInWindow_ (RowReference (newTopLine));
	}
	Led_AssertNotNil (fImager->fTopLinePartitionMarkerInWindow);	// can't delete last one...
	Led_AssertMember (fImager->fTopLinePartitionMarkerInWindow, PartitionMarker);
	fImager->InvalidateTotalRowsInWindow ();
}
				






/*
 ********************************************************************************
 ********************************* SimpleTextImager *****************************
 ********************************************************************************
 */
SimpleTextImager::SimpleTextImager ():
	TextImager (),
	PartitioningTextImager (),
	fICreatedPartition (false),
	fMyPartitionWatcher (),
	fRowHeight (Led_Distance (-1)),
	fInterlineSpace (0),
	fTopLinePartitionMarkerInWindow (NULL),
	fTotalRowsInWindow (0)				// value must be computed
{
}

SimpleTextImager::~SimpleTextImager ()
{
	Led_Assert (fTopLinePartitionMarkerInWindow == NULL);
}

void	SimpleTextImager::HookLosingTextStore ()
{
	inherited::HookLosingTextStore ();
	HookLosingTextStore_ ();
}

void	SimpleTextImager::HookLosingTextStore_ ()
{
	InvalidateTotalRowsInWindow ();
	fTopLinePartitionMarkerInWindow = NULL;
	// Only if we created the partition should we delete it. If it was set by external SetPartition () call, don't unset it here.
	if (fICreatedPartition) {
		fMyPartitionWatcher.UnInit (GetPartition ());
		fICreatedPartition = false;
		inherited::SetPartition (NULL);	// Call inherited so we avoid creating a new partition - PeekAtTextStore () still non-NULL, but soon to be NULL
	}
}

void	SimpleTextImager::HookGainedNewTextStore ()
{
	inherited::HookGainedNewTextStore ();
	HookGainedNewTextStore_ ();
}

void	SimpleTextImager::HookGainedNewTextStore_ ()
{
	Led_Require (fTopLinePartitionMarkerInWindow == NULL);
	if (GetPartition ().IsNull ()) {
		SetPartition (NULL);		// sets default one in since we have a textstore...
	}
	Led_Assert (fTopLinePartitionMarkerInWindow == NULL);
	fTopLinePartitionMarkerInWindow = GetFirstPartitionMarker ();
	AssureWholeWindowUsedIfNeeded ();
	InvalidateScrollBarParameters ();	// even if we don't change the top row, we might change enuf about the text to change sbar
}

/*
@METHOD:		SimpleTextImager::SetPartition
@DESCRIPTION:	<p>See @'PartitioningTextImager::SetPartition'. Same, but if called with NULL (and we
			have a TextStore) then make a default partition.</p>
*/
void	SimpleTextImager::SetPartition (const PartitionPtr& partitionPtr)
{
	fMyPartitionWatcher.UnInit (GetPartition ());
	inherited::SetPartition (partitionPtr);
	fICreatedPartition = false;
	if (GetPartition ().IsNull () and PeekAtTextStore () != NULL) {
		inherited::SetPartition (new LineBasedPartition (GetTextStore ()));
		fICreatedPartition = true;
	}
	fMyPartitionWatcher.Init (GetPartition (), this);
}


PartitioningTextImager::PartitionPtr	SimpleTextImager::MakeDefaultPartition () const
{
	return new LineBasedPartition (GetTextStore ());
}

/*
@METHOD:		SimpleTextImager::ReCalcRowHeight
@DESCRIPTION:	<p>Called by @'SimpleTextImager::GetRowHeight' on an as-needed basis (when invalidated by
			@'SimpleTextImager::InvalidateRowHeight'). All rows in a SimpleTextImager are the same height.
			By default this simple uses MeasureSegmentHeight_ (GetDefaultFont (), 0, 0) to establish the per-row
			height. You can override this to provide whatever height you wish. But anything smaller will look cut-off.</p>
*/
Led_Distance	SimpleTextImager::ReCalcRowHeight () const
{
	return MeasureSegmentHeight_ (GetDefaultFont (), 0, 0);
}

Led_Distance	SimpleTextImager::MeasureSegmentHeight (size_t /*from*/, size_t /*to*/) const
{
	//return (MeasureSegmentHeight_ (GetDefaultFont (), from, to));
	return GetRowHeight ();
}

Led_Distance	SimpleTextImager::MeasureSegmentBaseLine (size_t /*from*/, size_t /*to*/) const
{
	// If the user specifies an unusually large row-height, by default make this whitespace become above the
	// baseline, and not below.
	Led_Distance	rh		=	GetRowHeight ();
	Led_Distance	sbrh	=	MeasureSegmentHeight_ (GetDefaultFont (), 0, 0);
	Led_Distance	bl		=	MeasureSegmentBaseLine_ (GetDefaultFont (), 0, 0);
	if (rh > sbrh) {
		bl += (rh-sbrh);
	}
	return bl;
}

bool	SimpleTextImager::GetIthRowReferenceFromHere (RowReference* adjustMeInPlace, long ith) const
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

size_t	SimpleTextImager::GetRowNumber (RowReference rowRef) const
{
	// NB: This routine is VERY EXPENSIVE, if the text above the given row has not yet been wrapped, since
	// it forces a wrap. This is meant only to be a convenient code-saver in implementing rownumber based
	// APIs - even though their use is discouraged...
	size_t	rowNumber	=	0;
	Led_AssertNotNil (rowRef.GetPartitionMarker ());
	for (PartitionMarker* cur = rowRef.GetPartitionMarker ()->GetPrevious ();
			cur != NULL; cur = cur->GetPrevious ()) {
		rowNumber += 1;
	}
	return (rowNumber);
}

size_t	SimpleTextImager::CountRowDifference (RowReference lhs, RowReference rhs) const
{
	/*
	 *	See which row reference comes before the other, and then can from one TO the
	 *	other. Sadly - this forces the wrapping of all that text in between.
	 *
	 *	Note - this CAN be expensive if the two row references are far apart, as it requires
	 *	wrapping all the text in-between.
	 */
	PartitionMarker*	lhsPM			=	lhs.GetPartitionMarker ();
	PartitionMarker*	rhsPM			=	rhs.GetPartitionMarker ();
	size_t					lhsMarkerStart	=	lhsPM->GetStart ();
	size_t					rhsMarkerStart	=	rhsPM->GetStart ();
	bool			leftSmaller	=	(
									 (lhsMarkerStart < rhsMarkerStart) or
									 ((lhsMarkerStart == rhsMarkerStart))
									);
	RowReference	firstRowRef	=	leftSmaller? lhs: rhs;
	RowReference	lastRowRef	=	leftSmaller? rhs: lhs;
	
	size_t	rowsGoneBy	=	0;
	for (RowReference cur = firstRowRef; cur != lastRowRef; rowsGoneBy++) {
		bool	result	=	GetIthRowReferenceFromHere (&cur, 1);
		Led_Assert (result);
	}
	return (rowsGoneBy);
}

size_t	SimpleTextImager::GetTopRowInWindow () const
{
	// NB: Use of this function is discouraged as it is inefficent in the presence of word-wrapping
	return (GetRowNumber (GetTopRowReferenceInWindow ()));
}

size_t	SimpleTextImager::GetTotalRowsInWindow () const
{
	return GetTotalRowsInWindow_ ();
}

size_t	SimpleTextImager::GetLastRowInWindow () const
{
	// NB: Use of this function is discouraged as it is inefficent in the presence of word-wrapping
	return (GetRowNumber (GetLastRowReferenceInWindow ()));
}

void	SimpleTextImager::SetTopRowInWindow (size_t newTopRow)
{
	// NB: Use of this function is discouraged as it is inefficent in the presence of word-wrapping
	Led_Assert (newTopRow >= 0);


	SetTopRowInWindow (GetIthRowReference (newTopRow));

	Led_Assert (GetTopRowInWindow () == newTopRow);	// Since a SetTopRowInWindow() was called - all the
												// intervening lines have been wrapped anyhow - may
												// as well check we have our definitions straight...
}

void	SimpleTextImager::AssureWholeWindowUsedIfNeeded ()
{
	SetTopRowInWindow (GetTopRowReferenceInWindow ());
}

size_t	SimpleTextImager::GetMarkerPositionOfStartOfWindow () const
{
	return (GetStartOfRow (GetTopRowReferenceInWindow ()));
}

size_t	SimpleTextImager::GetMarkerPositionOfEndOfWindow () const
{
	return GetEndOfRow (GetLastRowReferenceInWindow ());
}

size_t		SimpleTextImager::GetMarkerPositionOfStartOfLastRowOfWindow () const
{
	return GetStartOfRow (GetLastRowReferenceInWindow ());
}

long	SimpleTextImager::CalculateRowDeltaFromCharDeltaFromTopOfWindow (long deltaChars) const
{
	Led_Assert (long (GetMarkerPositionOfStartOfWindow ()) >= 0 - deltaChars);
	size_t			pos			=	long (GetMarkerPositionOfStartOfWindow ()) + deltaChars;
	RowReference	targetRow	=	GetRowReferenceContainingPosition (pos);
	size_t			rowDiff		=	CountRowDifference (targetRow, GetTopRowReferenceInWindow ());
	return (deltaChars>=0)? rowDiff: -long(rowDiff);
}

long	SimpleTextImager::CalculateCharDeltaFromRowDeltaFromTopOfWindow (long deltaRows) const
{
	RowReference	row	= GetIthRowReferenceFromHere (GetTopRowReferenceInWindow (), deltaRows);
	return (long (GetStartOfRow (row)) - long (GetMarkerPositionOfStartOfWindow ()));
}

void	SimpleTextImager::ScrollByIfRoom (long downByRows)
{
	RowReference	newTopRow	=	GetTopRowReferenceInWindow ();
	(void)GetIthRowReferenceFromHere (&newTopRow, downByRows);				// ignore result cuz we did say - IF-ROOM!
	SetTopRowInWindow (newTopRow);
}

/*
@METHOD:		SimpleTextImager::ScrollSoShowing
@DESCRIPTION:	<p>Implement @'TextImager::ScrollSoShowing' API.</p>
*/
void	SimpleTextImager::ScrollSoShowing (size_t markerPos, size_t andTryToShowMarkerPos)
{
	Led_Assert (markerPos >= 0);
	Led_Assert (markerPos <= GetLength ());				// Allow any marker position (not just character?)
	Led_Assert (fTotalRowsInWindow == 0 or fTotalRowsInWindow == ComputeRowsThatWouldFitInWindowWithTopRow (GetTopRowReferenceInWindow ()));

	if (andTryToShowMarkerPos == 0) {			// special flag indicating we don't care...
		andTryToShowMarkerPos = markerPos;
	}
	Led_Assert (andTryToShowMarkerPos >= 0);
	Led_Assert (andTryToShowMarkerPos <= GetLength ());	// Allow any marker position (not just character?)


	/*
	 *		First check and see if the given position is within the current window
	 *	If so, do nothing. If it isn't, then try making the given selection
	 *	the first row (this later strategy is subject to chanage - but its
	 *	a plausible, and easy to implement start).
	 */
	size_t	startOfWindow	=	GetMarkerPositionOfStartOfWindow ();
	size_t	endOfWindow		=	GetMarkerPositionOfEndOfWindow ();
	if (markerPos >= startOfWindow and markerPos < endOfWindow and
		andTryToShowMarkerPos >= startOfWindow and andTryToShowMarkerPos < endOfWindow
		) {
		ScrollSoShowingHHelper (markerPos, andTryToShowMarkerPos);
		return;	// nothing (vertical) changed...
	}

	RowReference	originalTop		=	GetTopRowReferenceInWindow ();

	/*
	 *	Now things are a little complicated. We want to show both ends of the
	 *	selection - if we can. But - if we must show only one end or the other, we
	 *	make sure we show the 'markerPos' end.
	 *
	 *	We also would probably (maybe?) like to minimize the amount we scroll-by.
	 *	that is - if we only need to show one more line, don't scroll by
	 *	more than that.
	 */


	/*
	 *	First get us to a RowReference which is close to where we will eventually end up. That way, and calls
	 *	we do which will require word-wrapping (stuff to count rows) will only get applied to rows with a good
	 *	liklihood of needing to be wrapped anyhow.
	 */
	RowReference	newTop		=	originalTop;


	{
		// Speed hack recomended by Remo (roughly) - SPR#0553
		// if our target is closer to 0, then to the current window top, start at 0.
		// Similarly if we are closer to the end ( 0 .. M .. E } ==> M + (E-M)/2 ==> (E+M)/2
		if (markerPos < newTop.GetPartitionMarker ()->GetStart ()/2) {
			newTop = GetRowReferenceContainingPosition (0);
		}
		else if (markerPos > (GetEnd () + newTop.GetPartitionMarker ()->GetEnd ())/2) {
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
		Led_Assert (Contains (markerPos, markerPos, *newTop.GetPartitionMarker ()));
	}



	/*
	 *	At this point, we have a newTop which is CLOSE to where it will end up. We now adjust the
	 *	newTop to ASSURE that markerPos is shown.
	 */
	while (markerPos < GetStartOfRow (newTop) and GetPreviousRowReference (&newTop))
		;
	while (not PositionWouldFitInWindowWithThisTopRow (markerPos, newTop) and GetNextRowReference (&newTop))
		   ;

	// At this point our main desired position should be visible
	Led_Assert (markerPos >= GetStartOfRow (newTop));
	Led_Assert (PositionWouldFitInWindowWithThisTopRow (markerPos, newTop));


	/*
	 *	Now - try to adjust the newTop so that the 'andTryToShowMarkerPos' is also
	 *	shown. But - BE CAREFUL WE PRESERVE VISIBILITY OF 'markerPos'!!!
	 */
	while (not PositionWouldFitInWindowWithThisTopRow (andTryToShowMarkerPos, newTop)) {
		RowReference	trailNewTop	=	newTop;
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
	 *	Now - see if we've moved our 'newTop' by more than a certain threshold. If YES - then we may as well scroll
	 *	so that the new region of interest is CENTERED in the window.
	 */
	const	unsigned	kRowMoveThreshold	=	1;
	if (CountRowDifference (originalTop, newTop) > kRowMoveThreshold) {
		bool	mustPreserveSecondPos	=	PositionWouldFitInWindowWithThisTopRow (andTryToShowMarkerPos, newTop);

		// Now try to center the region of interest. Center by number of rows - not height of pixels. Height of pixels
		// might be better - but I think this is slightly easier - and probably just as good most of the time.
		size_t			topMarkerPos	=	min (markerPos, andTryToShowMarkerPos);
		size_t			botMarkerPos	=	max (markerPos, andTryToShowMarkerPos);
		size_t			numRowsAbove	=	CountRowDifference (newTop, GetRowReferenceContainingPosition (topMarkerPos));
		size_t			rowsInWindow	=	ComputeRowsThatWouldFitInWindowWithTopRow (newTop);
		RowReference	lastRowInWindow	=	GetIthRowReferenceFromHere (newTop, rowsInWindow-1);
		size_t			numRowsBelow	=	CountRowDifference (lastRowInWindow, GetRowReferenceContainingPosition (botMarkerPos));

		size_t	numRowsToSpare	=	numRowsAbove + numRowsBelow;

		// to to make numRowsAbove = 1/2 of numRowsToSpare
		RowReference	trailNewTop	=	newTop;
		GetIthRowReferenceFromHere (&trailNewTop, int (numRowsAbove) - int(numRowsToSpare/2));
		if (PositionWouldFitInWindowWithThisTopRow (markerPos, trailNewTop) and
			(not mustPreserveSecondPos or PositionWouldFitInWindowWithThisTopRow (andTryToShowMarkerPos, trailNewTop))
			) {
			newTop = trailNewTop;
		}
	}

	SetTopRowInWindow (newTop);		// This handles any notification of scrolling/update of sbars etc...

	Led_Assert (GetMarkerPositionOfStartOfWindow () <= markerPos and markerPos <= GetMarkerPositionOfEndOfWindow ());

	/*
	 * Must call this AFTER we've done some VERTICAL scrolling - cuz the vertical scrolling could have affected the MaxHPOS.
	 */
	ScrollSoShowingHHelper (markerPos, andTryToShowMarkerPos);
}

void	SimpleTextImager::SetTopRowInWindow (RowReference row)
{	
	if (GetForceAllRowsShowing ()) {
		row = AdjustPotentialTopRowReferenceSoWholeWindowUsed (row);
	}
	if (row != GetTopRowReferenceInWindow ()) {
		SetTopRowInWindow_ (row);
		InvalidateScrollBarParameters ();
	}
}

void	SimpleTextImager::Draw (const Led_Rect& subsetToDraw, bool printing)
{
	Invariant ();

	Led_Rect	rowsLeftToDrawRect	=	GetWindowRect ();


	Tablet_Acquirer	tablet_ (this);
	Led_Tablet		tablet	=	tablet_;
	Led_AssertNotNil (tablet);


	/*
	 *	Save old font/pen/brush info here, and restore - even in the presence of exceptions -
	 *	on the way out. That way - the drawsegment code need not worry about restoring
	 *	these things.
	 */
	#if		qMacOS
		tablet->SetPort ();
		RGBColor	oldForeColor	=	GDI_GetForeColor ();
		RGBColor	oldBackColor	=	GDI_GetBackColor ();
	#elif	qWindows
		Led_Win_Obj_Selector	pen (tablet, ::GetStockObject (NULL_PEN));
		Led_Win_Obj_Selector	brush (tablet, ::GetStockObject (NULL_BRUSH));
	#endif


	/*
	 *	Do this AFTER the save of colors above cuz no need in preserving that crap for
	 *	offscreen bitmap we cons up here on the fly.
	 */
	OffscreenTablet	thisIsOurNewOST;
	if (GetImageUsingOffscreenBitmaps () and not printing) {
		thisIsOurNewOST.Setup (tablet_);
	}

	try {
		size_t			rowNumberInWindow	=	0;
		size_t			totalRowsInWindow	=	GetTotalRowsInWindow_ ();
		RowReference	topRowInWindow		=	GetTopRowReferenceInWindow ();
		size_t			rowsLeftInWindow	=	totalRowsInWindow;
		for (PartitionMarker* pm = topRowInWindow.GetPartitionMarker (); rowsLeftInWindow != 0; pm = pm->GetNext ()) {
			Led_Assert (pm != NULL);
			size_t		startSubRow		=	0;
			size_t		maxSubRow		=	0;
			maxSubRow = rowsLeftInWindow - 1 + startSubRow;
			size_t	rowsDrawn	=	0;
			DrawPartitionElement (pm, startSubRow, maxSubRow, tablet,
				(GetImageUsingOffscreenBitmaps () and not printing)? &thisIsOurNewOST: NULL,
				printing, subsetToDraw,
				&rowsLeftToDrawRect, &rowsDrawn
			);
			Led_Assert (rowsLeftInWindow >= rowsDrawn);
			rowsLeftInWindow -= rowsDrawn;
		}

		/*
		 *	Now erase to the end of the page.
		 */
		Led_Assert (tablet == tablet_);	// Draw to screen directly past here...
		{
			Led_Rect	eraser	=	GetWindowRect ();
			eraser.top = rowsLeftToDrawRect.top;			// only from here down...
			eraser.bottom = subsetToDraw.bottom;			// cuz image rect may not cover what it used to, and never any need to

			if (eraser.top > eraser.bottom) {
				eraser.bottom = eraser.top;
			}

// SEE IF WE CAN TIGHTEN THIS TEST A BIT MORE, SO WHEN NO PIXELS WILL BE DRAWN, WE DONT BOTHER
// IN OTHER WORDS, CHANGE A COUPLE <= to < - LGP 970315

			// QUICKIE INTERSECT TEST
			if (
				(
					(eraser.top >= subsetToDraw.top and eraser.top <= subsetToDraw.bottom) or
					(eraser.bottom >= subsetToDraw.top and eraser.bottom <= subsetToDraw.bottom)
				) and
				(eraser.GetHeight () > 0 and eraser.GetWidth () > 0)
				) {

				if (GetImageUsingOffscreenBitmaps () and not printing) {
					tablet = thisIsOurNewOST.PrepareRect (eraser);
				}
				EraseBackground (tablet, eraser, printing);
				#if 0
					// Do we want to hilight the section after the end of the last row displayed if the selection continues onto
					// the next window? Somehow, I think it ends up looking schlocky. Leave off for now...
					size_t	hilightStart	=	GetSelectionStart ();
					size_t	hilightEnd		=	GetSelectionEnd ();
					size_t	end				=	GetMarkerPositionOfEndOfWindow ();
					bool	segmentHilightedAtEnd	=	GetSelectionShown () and (hilightStart < end) and (end <= hilightEnd);
					if (not printing and segmentHilightedAtEnd) {
						HilightARectangle (tablet, eraser);
					}
				#endif
				if (GetImageUsingOffscreenBitmaps () and not printing) {
					/*
					 *	Blast offscreen bitmap onto the screen.
					 */
					thisIsOurNewOST.BlastBitmapToOrigTablet ();
					tablet = tablet_;	// don't use offscreen tablet past here... Draw to screen directly!!!
				}
			}
		}
	}
	catch (...) {
		#if		qMacOS
			Led_Assert (*tablet == Led_GetCurrentGDIPort ());
			GDI_RGBForeColor (oldForeColor);
			GDI_RGBBackColor (oldBackColor);
		#endif
		throw;
	}
	#if		qMacOS
		Led_Assert (*tablet == Led_GetCurrentGDIPort ());
		GDI_RGBForeColor (oldForeColor);
		GDI_RGBBackColor (oldBackColor);
	#endif
}

/*
@METHOD:		SimpleTextImager::DrawPartitionElement
@DESCRIPTION:	<p></p>
*/
void	SimpleTextImager::DrawPartitionElement (PartitionMarker* pm, size_t startSubRow, size_t /*maxSubRow*/, Led_Tablet tablet, OffscreenTablet* offscreenTablet, bool printing, const Led_Rect& subsetToDraw, Led_Rect* remainingDrawArea, size_t* rowsDrawn)
{
	Led_RequireNotNil (pm);
	Led_RequireNotNil (remainingDrawArea);
	Led_RequireNotNil (rowsDrawn);

	size_t	start	=	pm->GetStart ();
	size_t	end		=	pm->GetEnd ();

	Led_Assert (end <= GetLength () + 1);
	if (end == GetLength () + 1) {
		end--;		// don't include bogus char at end of buffer
	}

	Led_Tablet					savedTablet		=	tablet;
	size_t						endSubRow		=	0;
	*rowsDrawn = 0;

	for (size_t subRow = startSubRow; subRow <= endSubRow; ++subRow) {
		Led_Rect					currentRowRect	=	*remainingDrawArea;
		currentRowRect.bottom = currentRowRect.top + GetRowHeight ();
		Led_Distance				interlineSpace	=	GetInterLineSpace ();
		if (
			(currentRowRect.bottom + Led_Coordinate (interlineSpace) > subsetToDraw.top) and
			(currentRowRect.top < subsetToDraw.bottom)
			) {

			/*
			 *	patch start/end/len to take into account rows...
			 */
			size_t	rowStart	=	start;
			size_t	rowEnd		=	end;

			// rowEnd is just BEFORE the terminating NL - if any...
			{
				Led_Assert (rowEnd < GetEnd () + 1);
				size_t	prevToEnd	=	FindPreviousCharacter (rowEnd);
				if (prevToEnd >= rowStart and prevToEnd < GetEnd ()) {
					Led_tChar	lastChar;
					CopyOut (prevToEnd, 1, &lastChar);
					if (RemoveMappedDisplayCharacters (&lastChar, 1) == 0) {
						rowEnd = prevToEnd;
					}
				}
			}

			TextLayoutBlock_Copy	rowText = GetTextLayoutBlock (rowStart, rowEnd);

			if (offscreenTablet != NULL) {
				tablet = offscreenTablet->PrepareRect (currentRowRect, interlineSpace);
			}

			{
				Led_Rect	invalidRowRect	=	currentRowRect;
				invalidRowRect.left = subsetToDraw.left;
				invalidRowRect.right = subsetToDraw.right;
// PERHAPS should also intersect the 'subsetToDraw' top/bottom with this invalidRowRect to get better logical clipping?
// Maybe important for tables!!!
// LGP 2002-09-22
//				DrawRow (RowReference (pm), tablet, currentRowRect, invalidRowRect, rowText, rowStart, rowEnd, printing);
				DrawRow (tablet, currentRowRect, invalidRowRect, rowText, rowStart, rowEnd, printing);
			}

			/*
			 *	Now erase/draw any interline space.
			 */
			if (interlineSpace != 0) {
				size_t	hilightStart	=	GetSelectionStart ();
				size_t	hilightEnd		=	GetSelectionEnd ();
				bool	segmentHilightedAtEnd	=	GetSelectionShown () and (hilightStart < rowEnd) and (rowEnd <= hilightEnd);
				if (pm->GetNext () == NULL) {
					segmentHilightedAtEnd = false;			// last row always contains no NL - so no invert off to the right...
				}
				DrawInterLineSpace (interlineSpace, tablet, currentRowRect.bottom, segmentHilightedAtEnd, printing);
			}

			if (offscreenTablet != NULL) {
				/*
				 *	Blast offscreen bitmap onto the screen.
				 */
				offscreenTablet->BlastBitmapToOrigTablet ();
				tablet = savedTablet;	// don't use offscreen tablet past here... Draw to screen directly!!!
			}
		}

		remainingDrawArea->top = currentRowRect.bottom + interlineSpace;
		(*rowsDrawn) ++;
	}
}

Led_Distance	SimpleTextImager::ComputeMaxHScrollPos () const
{
	Led_Distance	maxHWidth	=	0;
	{
		/*
		 *	Figure the largest amount we might need to scroll given the current windows contents.
		 *	But take into account where we've scrolled so far, and never invalidate that
		 *	scroll amount. Always leave at least as much layout-width as needed to
		 *	preserve the current scroll-to position.
		 */
		Led_Distance	width	=	CalculateLongestRowInWindowPixelWidth ();
		if (GetHScrollPos () != 0) {
			width = Led_Max (width, GetHScrollPos () + GetWindowRect ().GetWidth ());
		}
		maxHWidth = Led_Max (width, 1);
	}
	Led_Distance	wWidth	=	GetWindowRect ().GetWidth ();
	if (maxHWidth > wWidth) {
		return (maxHWidth - wWidth);
	}
	else {
		return 0;
	}
}

Led_Rect	SimpleTextImager::GetCharLocation (size_t afterPosition)	const
{
	return (GetCharLocationRowRelative (afterPosition, RowReference (GetFirstPartitionMarker ())));
}

Led_Rect	SimpleTextImager::GetCharWindowLocation (size_t afterPosition)	const
{
	Led_Point	windowOrigin	=	GetWindowRect ().GetOrigin () - Led_Point (0, GetHScrollPos ());
	return (windowOrigin +
		GetCharLocationRowRelative (afterPosition, GetTopRowReferenceInWindow (), GetTotalRowsInWindow_ ())
	);
}

size_t	SimpleTextImager::GetCharAtLocation (const Led_Point& where) const
{
	return (GetCharAtLocationRowRelative (where, RowReference (GetFirstPartitionMarker ())));
}

size_t	SimpleTextImager::GetCharAtWindowLocation (const Led_Point& where) const
{
	Led_Point	windowOrigin	=	GetWindowRect ().GetOrigin () - Led_Point (0, GetHScrollPos ());
	return (GetCharAtLocationRowRelative (where - windowOrigin,
											GetTopRowReferenceInWindow (),
											GetTotalRowsInWindow_ ()
										)
		);
}

size_t	SimpleTextImager::GetStartOfRow (size_t rowNumber) const
{
	// NB: Use of routines using rowNumbers force word-wrap, and so can be quite slow.
	// Routines using RowReferences often perform MUCH better
	return (GetStartOfRow (GetIthRowReference (rowNumber)));
}

size_t	SimpleTextImager::GetStartOfRowContainingPosition (size_t charPosition) const
{
	return (GetStartOfRow (GetRowReferenceContainingPosition (charPosition)));
}

size_t	SimpleTextImager::GetEndOfRow (size_t rowNumber) const
{
	// NB: Use of routines using rowNumbers force word-wrap, and so can be quite slow.
	// Routines using RowReferences often perform MUCH better
	return (GetEndOfRow (GetIthRowReference (rowNumber)));
}

size_t	SimpleTextImager::GetEndOfRowContainingPosition (size_t charPosition) const
{
	return (GetEndOfRow (GetRowReferenceContainingPosition (charPosition)));
}

size_t	SimpleTextImager::GetRealEndOfRow (size_t rowNumber) const
{
	// NB: Use of routines using rowNumbers force word-wrap, and so can be quite slow.
	// Routines using RowReferences often perform MUCH better
	return (GetRealEndOfRow (GetIthRowReference (rowNumber)));
}

size_t	SimpleTextImager::GetRealEndOfRowContainingPosition (size_t charPosition) const
{
	return (GetRealEndOfRow (GetRowReferenceContainingPosition (charPosition)));
}

size_t	SimpleTextImager::GetStartOfRow (RowReference row) const
{
	PartitionMarker*	cur		=	row.GetPartitionMarker ();
	Led_AssertNotNil (cur);
	return (cur->GetStart ());
}

size_t	SimpleTextImager::GetEndOfRow (RowReference row) const
{
	size_t	markerEnd	=	GetRealEndOfRow (row);
	// Be careful about NL at end. If we end with an NL, then don't count that.
	// And for the last PM - it contains a bogus empty character. Dont count
	// that either.
	Led_Assert (markerEnd <= GetLength () + 1);
	if (markerEnd == GetLength () + 1) {
		return (GetLength ());
	}
	size_t	prevToEnd	=	FindPreviousCharacter (markerEnd);
	if (prevToEnd >= GetStartOfRow (row)) {
		Led_tChar	lastChar;
		CopyOut (prevToEnd, 1, &lastChar);
		if (RemoveMappedDisplayCharacters (&lastChar, 1) == 0) {
			return (prevToEnd);
		}
	}
	return (markerEnd);
}

size_t	SimpleTextImager::GetRealEndOfRow (RowReference row) const
{
	PartitionMarker*	cur		=	row.GetPartitionMarker ();
	Led_AssertNotNil (cur);
	Led_Assert (cur->GetEnd () > 0);
	size_t	markerEnd	=	cur->GetEnd ();
	Led_Assert (markerEnd <= GetLength () + 1);
	return (markerEnd);
}

SimpleTextImager::RowReference	SimpleTextImager::GetRowReferenceContainingPosition (size_t charPosition) const
{
	Led_Require (charPosition >= 0);
	Led_Require (charPosition <= GetEnd ());
	PartitionMarker*	pm	=	GetPartitionMarkerContainingPosition (charPosition);
	Led_AssertNotNil (pm);
	return (RowReference (pm));
}

size_t	SimpleTextImager::GetRowContainingPosition (size_t charPosition) const
{
	return (GetRowNumber (GetRowReferenceContainingPosition (charPosition)));
}

size_t	SimpleTextImager::GetRowCount () const
{
	// NB: This is an expensive routine because it forces a word-wrap on all the text!
	size_t	rowCount	=	0;
	for (PartitionMarker* cur = GetFirstPartitionMarker (); cur != NULL; cur = cur->GetNext ()) {
		PartitionMarker*	pm	=	cur;
		Led_AssertNotNil (pm);
		Led_AssertMember (pm, PartitionMarker);
		rowCount += 1;
	}
	return (rowCount);
}

Led_Rect	SimpleTextImager::GetCharLocationRowRelativeByPosition (size_t afterPosition, size_t positionOfTopRow, size_t maxRowsToCheck)	const
{
	return GetCharLocationRowRelative (afterPosition, GetRowReferenceContainingPosition (positionOfTopRow), maxRowsToCheck);
}

Led_Distance	SimpleTextImager::GetRowHeight (size_t /*rowNumber*/)	const
{
	return (GetRowHeight ());
}

/*
@METHOD:		SimpleTextImager::GetRowRelativeBaselineOfRowContainingPosition
@DESCRIPTION:	<p>Override/implement @'TextImager::GetRowRelativeBaselineOfRowContainingPosition'.</p>
*/
Led_Distance	SimpleTextImager::GetRowRelativeBaselineOfRowContainingPosition (size_t charPosition)	const
{
	RowReference	thisRow		=	GetRowReferenceContainingPosition (charPosition);
	size_t			startOfRow	=	GetStartOfRow (thisRow);
	size_t			endOfRow	=	GetEndOfRow (thisRow);
	return MeasureSegmentBaseLine (startOfRow, endOfRow);
}

void	SimpleTextImager::GetStableTypingRegionContaingMarkerRange (size_t fromMarkerPos, size_t toMarkerPos,
								size_t* expandedFromMarkerPos, size_t * expandedToMarkerPos) const
{
	Led_AssertNotNil (expandedFromMarkerPos);
	Led_AssertNotNil (expandedToMarkerPos);
	Led_Assert (fromMarkerPos >= 0);
	Led_Assert (fromMarkerPos <= toMarkerPos);
	Led_Assert (toMarkerPos <= GetEnd ());
	#if		qMultiByteCharacters && qDebug
		Assert_CharPosDoesNotSplitCharacter (fromMarkerPos);
		Assert_CharPosDoesNotSplitCharacter (toMarkerPos);
	#endif

	size_t		curTopRowRelativeRowNumber	=	0;

	RowReference	curRow	=	GetTopRowReferenceInWindow ();
	do {
		PartitionMarker*	cur		=	curRow.GetPartitionMarker ();
		Led_AssertNotNil (cur);
		size_t	start	=	cur->GetStart ();
		size_t	end		=	cur->GetEnd ();

		// For the last partition marker - we are including a BOGUS character past the end of the buffer.
		// We don't want to return that. But otherwise - it is OK to return the NL at the end of the
		// other lines (though perhaps that is unnecceary).... LGP 950210
		if (cur->GetNext () == NULL) {
			end--;
		}

		size_t	len		=	end-start;

		// If we are strictly before the first row, we won't appear later...
		if (curTopRowRelativeRowNumber == 0 and (fromMarkerPos < start)) {
			break;
		}

		curTopRowRelativeRowNumber++;

		if (Contains (*cur, fromMarkerPos) and Contains (*cur, toMarkerPos)) {
			(*expandedFromMarkerPos) = start;
			(*expandedToMarkerPos) = end;
			Led_Assert ((*expandedFromMarkerPos) >= 0);
			Led_Assert ((*expandedFromMarkerPos) <= (*expandedToMarkerPos));
			Led_Assert ((*expandedToMarkerPos) <= GetEnd ());
			return;
		}

		if (curTopRowRelativeRowNumber >= GetTotalRowsInWindow_ ()) {
			break;		// though this might allow is to go too far - no matter. We'd return
						// the same result anyhow. And the extra overhead in counter rows
						// as opposed to lines doesn't offset the overhead in counting a few
						// extra lines - besides - this is simpler...
		}
	}
	while (GetNextRowReference (&curRow));

	(*expandedFromMarkerPos) = 0;
	(*expandedToMarkerPos) = GetEnd ();
}

Led_Distance	SimpleTextImager::GetHeightOfRows (size_t startingRow, size_t rowCount)	const
{
	return (GetHeightOfRows (GetIthRowReference (startingRow), rowCount));
}

Led_Distance	SimpleTextImager::GetHeightOfRows (RowReference startingRow, size_t rowCount)	const
{
	Led_Distance	height	=	0;
	for (RowReference curRow = startingRow; rowCount > 0; rowCount--) {
		PartitionMarker*	curPM	=	curRow.GetPartitionMarker ();
		height += GetRowHeight ();
		height += GetInterLineSpace (curPM);
		(void)GetNextRowReference (&curRow);
	}
	return (height);
}

/*
@METHOD:		SimpleTextImager::SetInterLineSpace
@DESCRIPTION:	<p>Set the interline space associated for the entire text buffer. Call the
			no-arg version of @'SimpleTextImager::GetInterLineSpace' to get the currently set value.</p>
*/
void	SimpleTextImager::SetInterLineSpace (Led_Distance interlineSpace)
{
	fInterlineSpace = interlineSpace;
}

/*
@METHOD:		SimpleTextImager::GetInterLineSpace
@DESCRIPTION:	<p>Get the interline space associated with a particular partition element. This method
			is virtual so that subclassers can provide a different interline space for each partition element.
			But the default behavior is to simply use the global - buffer value - specified by
			@'SimpleTextImager::SetInterLineSpace' and returned by the no-arg version of
			@'SimpleTextImager::GetInterLineSpace'.</p>
*/
Led_Distance	SimpleTextImager::GetInterLineSpace (PartitionMarker* /*pm*/) const
{
	return GetInterLineSpace ();
}

/*
@METHOD:		SimpleTextImager::ChangedInterLineSpace
@DESCRIPTION:	<p>If you override the virtual, one-arg version of @'SimpleTextImager::GetInterLineSpace', you must
			call this method whenever the interline space value changes, for a given PM. This method will clear any caches
			associated with that PM. And refresh the screen, as needed (in some subclass).</p>
				<p>See the one-arg @'SimpleTextImager::GetInterLineSpace' for more details.</p>
*/
void	SimpleTextImager::ChangedInterLineSpace (PartitionMarker* /*pm*/)
{
	// for now - we do nothing. Not sure there are any caches - at this point.
}

void	SimpleTextImager::SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont)
{
//	TextImager::SetDefaultFont_ (defaultFont);
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

void	SimpleTextImager::DidUpdateText (const UpdateInfo& updateInfo) throw ()
{
	InvalidateTotalRowsInWindow ();
	inherited::DidUpdateText (updateInfo);
	Led_AssertNotNil (fTopLinePartitionMarkerInWindow);
	AssureWholeWindowUsedIfNeeded ();
	InvalidateScrollBarParameters ();			// even if we don't change the top row, we might change enuf about the text to change sbar
}

void	SimpleTextImager::SetWindowRect (const Led_Rect& windowRect)
{
	bool	heightChanged	=	GetWindowRect ().GetHeight () != windowRect.GetHeight ();
	inherited::SetWindowRect (windowRect);
	if (heightChanged and PeekAtTextStore () != NULL) {
		InvalidateTotalRowsInWindow ();
		AssureWholeWindowUsedIfNeeded ();
		InvalidateScrollBarParameters ();
	}
}

void	SimpleTextImager::InvalidateAllCaches ()
{
	inherited::InvalidateAllCaches ();
	InvalidateRowHeight ();
	if (not GetPartition ().IsNull ()) {
		// Invalidate totalRows at END of this procedure - after invalidating scrollbar params and assurewholewindowusedifneeded () - cuz
		// InvalidateScrollBarParameters/AssureWholeWindowUsedIfNeeded can revalidate some cache values...
		InvalidateScrollBarParameters ();
		AssureWholeWindowUsedIfNeeded ();
		InvalidateTotalRowsInWindow ();
	}
}

SimpleTextImager::RowReference	SimpleTextImager::AdjustPotentialTopRowReferenceSoWholeWindowUsed (const RowReference& potentialTopRow)
{
	/*
	 *	This check is always safe, but probably not a worthwhile optimization, except that it avoids
	 *	some problems about initializing the top-row-reference before we've got a valid
	 *	tablet setup to use.
	 */
	if (potentialTopRow.GetPartitionMarker ()->GetPrevious () == NULL) {
		return potentialTopRow;
	}

	Led_Coordinate	windowHeight	=	GetWindowRect ().GetHeight ();
	Led_Coordinate	heightUsed		=	0;

	for (RowReference curRow = potentialTopRow; ; ) {
		PartitionMarker*	curPM	=	curRow.GetPartitionMarker ();
		heightUsed += GetRowHeight ();
		heightUsed += GetInterLineSpace (curPM);
		if (heightUsed >= windowHeight) {
			return (potentialTopRow);		// Then we used all the space we could have - and that is a good row!
		}
		if (not GetNextRowReference (&curRow)) {
			break;
		}
	}

	#if		qNoSupportForNewForLoopScopingRules
		{
	#endif
	// If we got here - we ran out of rows before we ran out of height.
	// That means we should scroll back a smidge...
	for (RowReference curRow = potentialTopRow; ; ) {
		if (not GetPreviousRowReference (&curRow)) {
			return (curRow);		// if we've gone back as far as we can - were done!
									// Even if we didn't use all the height
		}

		PartitionMarker*	curPM	=	curRow.GetPartitionMarker ();
		heightUsed += GetRowHeight ();
		heightUsed += GetInterLineSpace (curPM);
		if (heightUsed > windowHeight) {
			// We went back one too far - forward one and return that.
			bool	result	=	GetNextRowReference (&curRow);
			Led_Assert (result);
			return (curRow);
		}
		else if (heightUsed == windowHeight) {
			return (curRow);		// Then we used all the space we could have - and that is a good row!
		}
	}
	#if		qNoSupportForNewForLoopScopingRules
		}
	#endif
	Led_Assert (false);	return (potentialTopRow);	// NotReached / silence compiler warnings
}

bool	SimpleTextImager::PositionWouldFitInWindowWithThisTopRow (size_t markerPos, const RowReference& newTopRow)
{
	if (markerPos < GetStartOfRow (newTopRow)) {
		return false;
	}

	size_t			rowCount	=	ComputeRowsThatWouldFitInWindowWithTopRow (newTopRow);
	RowReference	lastRow		=	GetIthRowReferenceFromHere (newTopRow, rowCount-1);

	return (markerPos < GetRealEndOfRow (lastRow));
}

size_t	SimpleTextImager::ComputeRowsThatWouldFitInWindowWithTopRow (const RowReference& newTopRow) const
{
	/*
	 *	For now, we don't show partial rows at the bottom. We
	 *	might want to reconsider this.
	 */
	Led_Coordinate	windowHeight	=	GetWindowRect ().GetHeight ();

	/*
	 *	Wind out way to the bottom of the window from our current position,
	 *	and count rows.
	 */
	size_t			rowCount	=	0;
	Led_Coordinate	heightUsed	=	0;
	for (RowReference curRow = newTopRow; ; ) {
		rowCount++;
		PartitionMarker*	curPM	=	curRow.GetPartitionMarker ();
		heightUsed += GetRowHeight ();
		heightUsed += GetInterLineSpace (curPM);
		if (heightUsed > windowHeight) {
			// we went one too far
			rowCount--;
			break;
		}
		else if (heightUsed == windowHeight) {
			break;	// thats all that will fit
		}

		if (not GetNextRowReference (&curRow)) {
			break;
		}
	}
	if (rowCount == 0) {	// always for the existence of at least one row...
		rowCount = 1;
	}

	return (rowCount);
}

Led_Rect	SimpleTextImager::GetCharLocationRowRelative (size_t afterPosition, RowReference topRow, size_t maxRowsToCheck)	const
{
	// MUST FIGURE OUT WHAT TODO HERE BETTER - 10000 not good enough answer always...
	const	Led_Rect	kMagicBeforeRect	=	Led_Rect (-10000, 0, 0, 0);
	const	Led_Rect	kMagicAfterRect		=	Led_Rect (10000, 0, 0, 0);

	Led_Assert (afterPosition >= 0);
	Led_Assert (afterPosition <= GetEnd ());		// does that ever make sense???
	#if		qMultiByteCharacters && qDebug
		Assert_CharPosDoesNotSplitCharacter (afterPosition);
	#endif

	if (afterPosition < GetStartOfRow (topRow)) {
		return (kMagicBeforeRect);
	}

	RowReference	curRow						=	topRow;
	size_t			curTopRowRelativeRowNumber	=	0;
	Led_Coordinate	topVPos						=	0;
	do {
		PartitionMarker*	cur		=	curRow.GetPartitionMarker ();
		Led_AssertNotNil (cur);
		size_t	start	=	cur->GetStart ();
		size_t	end		=	cur->GetEnd ();			// end points JUST PAST LAST VISIBLE/OPERATED ON CHAR

		Led_Assert (end <= GetEnd () + 1);


		curTopRowRelativeRowNumber++;

		if (afterPosition >= start and afterPosition < end) {
			Led_Assert (start <= afterPosition);
			Led_Distance	hStart	=	0;
			Led_Distance	hEnd	=	0;
			GetRowRelativeCharLoc (afterPosition, &hStart, &hEnd);
			Led_Assert (hStart <= hEnd);
			return (Led_Rect (topVPos, hStart, GetRowHeight (), hEnd-hStart));
		}

		topVPos += GetRowHeight ();
		topVPos += GetInterLineSpace (cur);

		if (curTopRowRelativeRowNumber >= maxRowsToCheck) {
			break;	// return bogus place at the end...
		}
	}
	while (GetNextRowReference (&curRow));

	return (kMagicAfterRect);
}

size_t		SimpleTextImager::GetCharAtLocationRowRelative (const Led_Point& where, RowReference topRow, size_t maxRowsToCheck) const
{
	/*
	 *	Not 100% sure how to deal with points outside our range. For now - we just
	 *	return topMost/bottomMost marker positions. That seems to work decently - at least
	 *	for now... But I worry if it is the right thing when we do
	 *	autoscrolling...
	 */
	if (where.v < 0) {
		#if		qMultiByteCharacters
			Assert_CharPosDoesNotSplitCharacter (0);
		#endif
		return (0);
	}

	RowReference	curRow						=	topRow;
	size_t			curTopRowRelativeRowNumber	=	0;
	Led_Coordinate	topVPos						=	0;
	do {
		PartitionMarker*	cur		=	curRow.GetPartitionMarker ();
	
		Led_AssertNotNil (cur);
		size_t	start	=	cur->GetStart ();
		#if		qMultiByteCharacters
			Assert_CharPosDoesNotSplitCharacter (start);
		#endif

		/*
		 *	Count the interline space as part of the last row of the line for the purpose of hit-testing.
		 */
		Led_Distance	interLineSpace	=	GetInterLineSpace (cur);

		curTopRowRelativeRowNumber++;
		if (where.v >= topVPos and where.v < topVPos + Led_Coordinate (GetRowHeight () + interLineSpace)) {
			return GetRowRelativeCharAtLoc (where.h, start);
		}

		if (curTopRowRelativeRowNumber >= maxRowsToCheck) {
			break;	// we've checked enuf...
		}

		topVPos += GetRowHeight () + interLineSpace;
	}
	while (GetNextRowReference (&curRow));

	#if		qMultiByteCharacters
		Assert_CharPosDoesNotSplitCharacter (GetLength ()+1);
	#endif
	return (GetEnd ());
}

/*
@METHOD:		SimpleTextImager::ContainsMappedDisplayCharacters
@DESCRIPTION:	<p>Override @'TextImager::ContainsMappedDisplayCharacters' to hide '\n' characters.
			See @'qDefaultLedSoftLineBreakChar'.</p>
*/
bool	SimpleTextImager::ContainsMappedDisplayCharacters (const Led_tChar* text, size_t nTChars) const
{
	return 
			ContainsMappedDisplayCharacters_HelperForChar (text, nTChars, '\n') or
			inherited::ContainsMappedDisplayCharacters (text, nTChars)
		;
}

/*
@METHOD:		SimpleTextImager::RemoveMappedDisplayCharacters
@DESCRIPTION:	<p>Override @'TextImager::RemoveMappedDisplayCharacters' to hide '\n' characters.</p>
*/
size_t	SimpleTextImager::RemoveMappedDisplayCharacters (Led_tChar* copyText, size_t nTChars) const
{
	size_t	newLen	=	inherited::RemoveMappedDisplayCharacters (copyText, nTChars);
	Led_Assert (newLen <= nTChars);
	size_t	newerLen	=	RemoveMappedDisplayCharacters_HelperForChar (copyText, newLen, '\n');
	Led_Assert (newerLen <= newLen);
	Led_Assert (newerLen <= nTChars);
	return newerLen;
}



#if		qLedUsesNamespaces
}
#endif


// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
