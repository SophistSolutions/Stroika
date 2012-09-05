/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/MultiRowTextImager.cpp,v 2.130 2004/01/26 16:42:02 lewis Exp $
 *
 * Changes:
 *	$Log: MultiRowTextImager.cpp,v $
 *	Revision 2.130  2004/01/26 16:42:02  lewis
 *	SPR#1604: Lose overrides of ComputeRelativePosition () since the speedtweek can be done directly in the TextImager::ComputeRelativePosition () version using GetStartOfNextRowFromRowContainingPosition () etc.
 *	
 *	Revision 2.129  2003/11/03 20:05:42  lewis
 *	add new TextImager::GetTextLayoutBlock () method - which in subclasses - can take into account subclasses to use an intiail dir from first row in subsequent rows. Related - added PartitioningTextImager::GetPrimaryPartitionTextDirection ()
 *	
 *	Revision 2.128  2003/04/01 18:24:38  lewis
 *	SPR#1322: fix MultiRowTextImager::DrawPartitionElement () to properly compute invalidRowRect intersecting with 'invalidRect'
 *	
 *	Revision 2.127  2003/03/21 14:49:43  lewis
 *	added CRTDBG_MAP_ALLOC_NEW support
 *	
 *	Revision 2.126  2003/03/06 14:39:46  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.125  2003/03/05 17:45:32  lewis
 *	SPR#1319 - fix a number of things (SpecifyTextStore related mostly) to speed tweek
 *	tables and other display.
 *	
 *	Revision 2.124  2003/02/03 20:56:52  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.123  2003/01/29 17:59:52  lewis
 *	SPR#1264- Get rid of most of the SetDefaultFont overrides and crap - and made
 *	InteractiveSetFont REALLY do the interactive command setfont - and leave SetDefaultFont
 *	to just setting the 'default font'
 *	
 *	Revision 2.122  2003/01/12 16:44:19  lewis
 *	cleanup slight speed tweek
 *	
 *	Revision 2.121  2003/01/11 19:28:43  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.120  2003/01/10 15:24:05  lewis
 *	as part of SPR#1226- added code to MultiRowTextImager::DrawPartitionElement so the 'rowEnd' arg it passes
 *	to other functions is a regular (apparent) row end, and not the REAL-ROW-END (has terminating NL removed)
 *	
 *	Revision 2.119  2002/12/21 03:00:10  lewis
 *	SPR#1217. Added virtual TextImager::InvalidateAllCaches and made SimpleTextImager/MRTI versions
 *	just overrides. No longer need SimpleTextImager::TabletChangedMetrics overrides etc
 *	since now TextImager::TabletChangedMetrics calls TextImager::InvalidateAllCaches.
 *	MultiRowTextImager::PurgeUnneededMemory () no longer needed since TextImager version
 *	now calls InvalidateAllCaches().
 *	
 *	Revision 2.118  2002/12/20 17:56:32  lewis
 *	SPR#1216- get rid of override of GetSelectionWindowRegion. New version in TextImager
 *	now properly handles BIDI, and interline space (interline space not tested,but
 *	not really used much anymore??).
 *	And - fix one very old zero-based index vs. one-based index bug.
 *	
 *	Revision 2.117  2002/12/06 15:15:41  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.116  2002/12/06 15:13:40  lewis
 *	SPR#1198 - added 2 new APIs to TextImager, implemented them in PartitioningTextImager,
 *	and used  them in MultiRowTextImager and SimpleTextImager:
 *		GetRowRelativeCharLoc ()
 *		GetRowRelativeCharAtLoc ().
 *	
 *	Revision 2.115  2002/12/04 15:44:00  lewis
 *	SPR#1193 - lose qCacheTextMeasurementsForOneRow hack (now in PartitioningTextImager) and 
 *	use CalcSegmetnSize instead of CalcSegmentSizeWithImmediateText
 *	
 *	Revision 2.114  2002/12/03 02:41:54  lewis
 *	SPR#1187- Fix MultiRowTextImager::GetCharLocationRowRelative to call RemoveMappedDisplayCharacters
 *	
 *	Revision 2.113  2002/12/02 15:58:04  lewis
 *	SPR#1185 - move DrawRow,DrawRowSegments,DrawRowHilight,DrawInterlineSpace from MRTI/SimpleTextImager
 *	to TextImager (share code. Changed their args slightly. For BIDI work.
 *	
 *	Revision 2.112  2002/11/28 21:02:05  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.111  2002/11/28 20:57:13  lewis
 *	Work on BIDI (SPR#1183).  Create TextLayoutBlock_Basic for each row instead of one for whole
 *	para and then breakup into _VirtualSubset for each row. Though docs indicate otherwise -
 *	empirically - that seems to be what is required (comparing with layout of MSWord and MSIE)
 *	
 *	Revision 2.110  2002/11/28 02:31:10  lewis
 *	A couple simple name changes (GetVirtualText->PeekAtVirtualText and TextLayoutBlock_Subset 
 *	to TextLayoutBlock_VirtualSubset
 *	
 *	Revision 2.109  2002/11/27 18:22:27  lewis
 *	fix bug with maxSubRow arg passed to DrawPartitionElement
 *	
 *	Revision 2.108  2002/11/27 15:58:46  lewis
 *	SPR#1183 - as part of BIDI effort - DrawSegment now replaces 'Led_tChar* text' argument with
 *	a 'TextLayoutBlock& text' argument
 *	
 *	Revision 2.107  2002/11/27 15:13:59  lewis
 *	As part of SPR#1183 - BIDI project- used TextLayoutBlock class instead of Led_tChar* as arg to DrawRow 
 *	and DrawRowSegments() routines. Much more of this sort to come! NB: NOT BACKWARD COMPAT!
 *	
 *	Revision 2.106  2002/11/27 13:06:55  lewis
 *	add DrawPartitionElement() method and break out some of the Draw() code into that as prelim for SPR#1183
 *	
 *	Revision 2.105  2002/10/30 13:29:19  lewis
 *	SPR#1155 - Added new Rect_Base<> template and made Led_Rect a simple typedef of that. Then TRIED (failed) to
 *	make the top/left/bottom/right fields private by adding accessors. Added the accessors and used them in tons
 *	of places - but still TONS more required. Leave that battle for another day. I'm not even SURE its a good idea.
 *	Anyhow - also added a new typedef for Led_TWIPS_Rect (this was what I was after to begin with with all this nonsese)
 *	
 *	Revision 2.104  2002/10/23 20:47:48  lewis
 *	minor tweak to PMInfoCacheMgr to make it respect private access properly (detected on MSVC60)
 *	
 *	Revision 2.103  2002/10/23 20:01:31  lewis
 *	SPR#1144 - qAutoPtrBrokenBug workaround
 *	
 *	Revision 2.102  2002/10/23 18:00:24  lewis
 *	small tweeks to get working on MSVC60
 *	
 *	Revision 2.101  2002/10/21 20:27:41  lewis
 *	SPR#1134- Moved date in PartitionElementCacheInfo into a shared Rep object so copy CTOR/op= would be MUCH faster
 *	(at the cost of extra deref in use)
 *	
 *	Revision 2.100  2002/10/21 16:21:41  lewis
 *	SPR#1133 - more cleanups of new WPPartition code and PMCacheMgr etc. Still more todo
 *	
 *	Revision 2.99  2002/10/21 14:41:44  lewis
 *	SPR#1134 - partial cleanup of new PartitionElementCacheInfo and PMInfoCacheMgr code
 *	
 *	Revision 2.98  2002/10/20 19:38:18  lewis
 *	SPR#1128 and SPR#1129 - BIG change. Get rid of MultiRowPartitionMarker and MultiRowPartition. Still coded very
 *	slopily (with idefs) and needs major cleanups/tweeks. But - its now functional and testable
 *	
 *	Revision 2.97  2002/10/14 23:22:48  lewis
 *	added requires
 *	
 *	Revision 2.96  2002/10/08 16:38:06  lewis
 *	SPR#1119- Added CheckForSplits/NeedToCoalesce to LineBasedPartition. Add WordProcessor::WPPartition.
 *	Add / use MakeDefaultPartition () so we create the new partition for WPs.
 *	
 *	Revision 2.95  2002/10/08 01:34:21  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.94  2002/09/22 15:45:48  lewis
 *	doccomments
 *	
 *	Revision 2.93  2002/05/06 21:33:48  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.92  2001/11/27 00:29:54  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.91  2001/10/20 13:38:59  lewis
 *	tons of DocComment changes
 *	
 *	Revision 2.90  2001/10/17 20:42:58  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.89  2001/09/26 15:41:20  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.88  2001/09/24 14:16:42  lewis
 *	SPR#1042- cleaned up region support. SPR#0989- fixed flicker problem: improve
 *	logical clipping on final EraseBackground call - and stick that in the
 *	offscreen bitmap (to kill flicker)
 *	
 *	Revision 2.87  2001/09/19 14:43:35  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.86  2001/09/12 17:23:26  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.85  2001/09/12 16:38:18  lewis
 *	SPR#0987- fix secondary hilight region to extned to LHS of window for rows other than the first
 *	
 *	Revision 2.84  2001/09/12 14:53:31  lewis
 *	SPR#0987- Added conditional UseSecondaryHilight support (macos&win32 only so far). Redo
 *	Led_Region support for MacOS so now more like with PC (no operator conversion to macregion
 *	and auto constructs OSRegion except when called with region arg - in whcih case we dont
 *	OWN region and dont delete it on DTOR
 *	
 *	Revision 2.83  2001/08/29 23:39:13  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.82  2001/08/29 23:36:33  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.81  2001/08/28 18:43:37  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.80  2001/08/27 20:41:19  lewis
 *	SPR#0978- code I added for (GetImageUsingOffscreenBitmaps() didn't check
 *	'printing' flag in some cases it needed to
 *	
 *	Revision 2.79  2001/06/27 13:29:31  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.78  2001/05/12 22:50:57  lewis
 *	SPR#0917- new TextImager::G/SetImageUsingOffscreenBitmaps () method to
 *	replace obsolete qUseOffscreenBitmapsToReduceFlicker
 *	
 *	Revision 2.77  2001/05/12 22:29:15  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.76  2001/05/11 23:39:34  lewis
 *	SPR#0915- Added class OffscreenTablet to abstract away common code from
 *	MultiRowTextImager/SimpleTextImager to handle offscreen bitmap drawing.
 *	Much cleanedup (HOPEFULLY not broken).
 *	
 *	Revision 2.75  2001/05/01 14:37:12  lewis
 *	fix small GCC warnings
 *	
 *	Revision 2.74  2001/04/26 16:39:30  lewis
 *	delete ifdefed out code fragments (cleanups)
 *	
 *	Revision 2.73  2001/01/03 14:42:05  Lewis
 *	qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__. And lose asserts that
 *	some unsigned ints were >= 0
 *	
 *	Revision 2.72  2000/11/03 16:51:43  lewis
 *	SPR#0842- fix horizontal autoscroll (scrollsoshowing) when happens WITH
 *	vertical autoscroll, as in a find command
 *	
 *	Revision 2.71  2000/09/25 00:44:31  lewis
 *	react to change in args for Led_Tablet_ mac CTOR
 *	
 *	Revision 2.70  2000/08/13 03:37:31  lewis
 *	SPR#0821- MultiRowTextImager::ReValidateSubRowInTopLineInWindow () should NOT
 *	validate a given row - cuz can be called during DidUpdates - see SPR for details
 *	
 *	Revision 2.69  2000/07/26 05:25:47  lewis
 *	Documented MultiRowTextImager::CountRowDifference and added new
 *	MultiRowTextImager::CountRowDifferenceLimited. Using that made a huge
 *	difference in speed in the scrollsoshowing () code
 *	
 *	Revision 2.68  2000/06/17 07:44:28  lewis
 *	SPR#0792- Add TextImager::GetRowRelativeBaselineOfRowContainingPosition() as
 *	part of fixing this. And impelmnt in MRTI. And added asserts to MRTI::DrawRowSegments()
 *	that the start/end are reasonable for that segment.
 *	
 *	Revision 2.67  2000/06/12 20:09:26  lewis
 *	because of SPR#0760- I noted we get better display results (more like MSWord2000)
 *	if we pin the baseline to the currentRowRect.bottom
 *	
 *	Revision 2.66  2000/06/12 16:20:04  lewis
 *	support SPR#0780- Led_Tablet_ support
 *	
 *	Revision 2.65  2000/04/15 14:32:37  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.64  2000/04/14 22:40:25  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.63  2000/04/05 21:45:17  lewis
 *	SPR#0470 fixed - curious assert failure now understood and clarified.
 *	AND - SPR#0734 - call new GetIntraRowTextWindowBoundingRect ()
 *	
 *	Revision 2.62  2000/04/03 22:36:02  lewis
 *	SPR#0730: MultiRowTextImager::DrawRowHilight () should use RemoveMappedDisplayCharacters ()
 *	to replace ifdefed out code that was handling NL special case - so we only grab rect for a single row
 *	
 *	Revision 2.61  2000/03/13 22:13:46  lewis
 *	FIX COMMENT
 *	
 *	Revision 2.60  2000/03/11 22:04:48  lewis
 *	SPR#0711 - new GetRealEndOfRow* () routines. And fixed PositionWouldFitInWindowWithThisTopRow ()
 *	to call it. And other small fixes to TextInteractor::ScrollToSelection ()
 *	and Simple/MultiRow::ScrollSoShowing ()
 *	
 *	Revision 2.59  2000/03/09 04:40:29  lewis
 *	replace old MultiRowTextImager::HilightARectanle_ (and SimpleTextImager version)
 *	with TextImager::HilightARectanlge() ETC - SPR#0708
 *	
 *	Revision 2.58  2000/01/22 20:51:33  lewis
 *	SPR#0499- fix XXX::ScrollSoShowing () to vertically CENTER the interesting text when
 *	its forced to scroll more than some threshold amount (1 row for now)
 *	
 *	Revision 2.57  2000/01/22 17:34:58  lewis
 *	Added TextImager::ScrollSoShowingHHelper () to share a little code from SimpleTextImager::ScrollSoShowing
 *	/ MultiRowTextImager::ScrollSoShowing () - code cleanups
 *	
 *	Revision 2.56  1999/12/23 17:23:06  lewis
 *	spr#0677 - small fix to TextImager::ComputeRelativePosition () for by-row cursoring
 *	
 *	Revision 2.55  1999/12/21 20:55:27  lewis
 *	Use new Led_ThrowIfNull instead of manual check
 *	
 *	Revision 2.54  1999/12/21 03:22:58  lewis
 *	PREV CHECKIN WAS PART OF SPR#0671.
 *	
 *	Revision 2.53  1999/12/21 03:22:14  lewis
 *	Lose all references to \n - except in new ContainsMappedDisplayCharacters/
 *	RemoveMappedDisplayCharacters functions. Use inherited instead of TextImager
 *	as base class a few places. Fixup help strings a few places.
 *	
 *	Revision 2.52  1999/12/14 18:08:39  lewis
 *	Respect qHeavyDebugging flag and don't call GetLastRow() in assert unless thats ON
 *	
 *	Revision 2.51  1999/11/29 21:10:59  lewis
 *	PatPtr SB const Pattern* for MacOS Wrapper functions.
 *	
 *	Revision 2.50  1999/11/29 20:48:02  lewis
 *	Use Led_GetCurrentGDIPort () instead of peeking at 'qd' global (and kBlack constnat).
 *	SPR#0634 (MACONLY).
 *	
 *	Revision 2.49  1999/11/15 21:29:12  lewis
 *	Don't refer to CDC or CBitmap directly (MFC classes). Instead of Led_Tablet etc -
 *	my own private Win32SDK wrappers - so no more dependency on MFC. AddRectangleToRegion ()
 *	now moved to shared code
 *	
 *	Revision 2.48  1999/11/13 16:32:22  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.47  1999/06/28 15:05:26  lewis
 *	spr#0598- use mutable instead of const_cast, as appopropriate
 *	
 *	Revision 2.46  1999/05/03 22:05:12  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.45  1999/03/24 15:47:23  lewis
 *	spr#0561 fixed- TexImager::NoTabletAvailable now subclasses from
 *	TextImager::NotFullyInitialized etc
 *	
 *	Revision 2.44  1999/03/12 01:38:38  lewis
 *	Improve some doc/comments
 *	
 *	Revision 2.43  1999/03/04 14:31:32  lewis
 *	Allow SetWindowRect() call before having specified TextStore - by checking if
 *	non-null. Helpful in initialization
 *	
 *	Revision 2.42  1999/02/23 18:41:05  lewis
 *	fix spr# 0552 - call rgbfore/backcolor before CopyBits
 *	
 *	Revision 2.41  1999/02/05 01:43:39  lewis
 *	in MultiRowTextImager::ReValidateSubRowInTopLineInWindow (), add back in comment
 *	out assert, but do it in such a way that it catches and ignores appropriate
 *	exceptions (like if no tablet yet)
 *	
 *	Revision 2.40  1998/10/30 14:39:34  lewis
 *	added row arg to DrawRowSegments()
 *	
 *	Revision 2.39  1998/07/24  01:10:37  lewis
 *	ResetTabStops() API chagned (again). Hopefully for the last time.
 *	MultiRowPartition::MultiRowPartition () version with a eSpecialHackToDisableInit arg,
 *	so IT can be subclassed (LEC/SideBySide).
 *
 *	Revision 2.38  1998/06/03  01:32:55  lewis
 *	override MultiRowTextImager::SetPartition () to fixup fTopLinePartitionMarkerInWindow
 *	etc fields, rather than
 *	doing in HookGained/LostTextStore. This is so subclasses can call SetPartition
 *	with another partition, and we
 *	still invalidate/revalidate those fields.
 *
 *	Revision 2.37  1998/04/25  13:05:16  lewis
 *	*** empty log message ***
 *
 *	Revision 2.36  1998/04/25  01:31:28  lewis
 *	cleanup
 *
 *	Revision 2.35  1998/04/08  01:47:42  lewis
 *	Major reworking to accomodate new Partition design - now MultiRowTextIamger
 *	subclasses from PartitioningTextImager directtly
 *	and instead of subclassing from LineBasedTextImager, it owns a Parition which
 *	subclasses from LineBasedParition (temporarily). Soon we should
 *	lift the requirement in this code for it to depend on its own type of partition
 *	markers and its own partition.
 *
 *	Revision 2.34  1997/12/24  04:40:13  lewis
 *	*** empty log message ***
 *
 *	Revision 2.33  1997/09/29  15:06:54  lewis
 *	Lose qLedFirstIndex support.
 *	Break alot of the drawing code into parts to facilitate narrow, specific
 *	overrides in the WordProcessor drawing code
 *	for justification support.
 *	No nlonger need to wrap operateddlete stuff i qALlowBlockALlocation stuff -
 *	done in Led_Block... itself.
 *	Renamed CalcSegmentSize to CalcSegmentSizeWithImmediateText () (for xtra
 *	arg case) to avoid ambiguities.
 *
 *	Revision 2.32  1997/07/27  15:58:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.31  1997/07/23  23:09:45  lewis
 *	workaround spr#0470. Asserts.
 *
 *	Revision 2.30  1997/07/14  00:46:20  lewis
 *	Support qLedFirstIndex = 0.
 *	Renamed TextImager_ to TextImager, and TextStore_ to TextStore, etc...
 *
 *	Revision 2.29  1997/06/24  03:31:13  lewis
 *	Lose old LayoutWidth () concept. Moved to WordWrappedTextImager (see spr#0450).
 *	Added InvalidateAllCaches () helper (called by subclasses when 'LayoutWidth' changes.
 *
 *	Revision 2.28  1997/06/18  20:08:49  lewis
 *	*** empty log message ***
 *
 *	Revision 2.27  1997/06/18  02:59:14  lewis
 *	qIncludePrefixFile
 *	ResetTabStops now takes PM arg.
 *
 *	Revision 2.26  1997/03/23  00:39:37  lewis
 *	Lose obsolete OpenDoc stuff.
 *	Lose obsolete compiler bug workarounds.
 *	SetLayoutWidth () doesn't inval if width not changed.
 *	GetTotalRowsInWindow_ ()
 *	Changed HilightARectangle_() to take no color args.
 *	Cleanup/fix EraseBacground () stuff.
 *	MRTI::Draw() now uses RowReferences().
 *	More...
 *
 *	Revision 2.25  1997/03/04  20:12:10  lewis
 *	Support for new EraseBackground() call. Had to change args to DrawInterlineSpace().
 *
 *	Revision 2.24  1997/01/20  05:26:04  lewis
 *	Since AcquireTablet() can now throw exception, wrap SetDefaultFont_() call so TabletNotAvailable
 *	doesn't get propagated - safe to ignore.
 *	Support for colored text.
 *	Cleanup/fixup a bit horizontal scrolling code.
 *
 *	Revision 2.23  1997/01/10  03:17:00  lewis
 *	throw specifiers.
 *	qSupportTabStops always true now (define gone).
 *	New GetCharLocationRowRelativeByPosition ().
 *	ForceAllShowing stuff moved to TextImager.
 *	more scrolling changes.
 *
 *	Revision 2.22  1996/12/13  17:58:46  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.21  1996/12/05  19:11:38  lewis
 *	*** empty log message ***
 *
 *	Revision 2.20  1996/10/31  00:27:12  lewis
 *	*** empty log message ***
 *
 *	Revision 2.19  1996/09/30  14:27:39  lewis
 *	Cleanup sbar/AdjustBounds/InvalidateScrollBarParameters stuff. Now use InvalidateScrollBarParameters
 *	instead of obsolete AdjustBounds.
 *
 *	Revision 2.18  1996/09/01  15:31:50  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.17  1996/08/05  06:05:36  lewis
 *	Moved a few Mac GDI_wrapper routines to LedSupport.hh.
 *	In HilightRectangle routine, do GDI_SetPort() - not assert that port was set
 *	cuz sometimes NOT set - at least for Led_TCL.
 *
 *	Revision 2.16  1996/07/03  01:13:08  lewis
 *	Fixed bug with calculating selection window region at end of buffer.
 *
 *	Revision 2.15  1996/06/01  02:17:53  lewis
 *	lose no-RTTI compat code.
 *	Check for running out of memory to avoid crashes in GWorld (mac) code when
 *	low on memory.
 *	more cleanups?
 *
 *	Revision 2.14  1996/05/23  19:54:55  lewis
 *	Lots of minor cleanups and commnets. Main changes were with
 *	DrawSegment no longer taking areaLeft, and instead optional pixelsDrawn.
 *	Also lost old qUsingNewEraserIMCode==0 code.
 *	FontSpecification --> new Led_FontSpecification stuff.
 *	fixed windows only bug where I forget to check printing flag before doing offscreen bitmap stuff.
 *	Lots more little thigns here and there having todo with hilighting/DrawRow etc.
 *
 *	Revision 2.13  1996/05/14  20:35:43  lewis
 *	Implemented qUsingNewEraserIMCode code here, so we do eraserect once for
 *	whole row, and then draw row. THis was needed/highly desirable to fix bug
 *	with cross-segment text hangovers (like italics f, ligatures etc).
 *	Implemented qUseOffscreenBitmapsToReduceFlicker.
 *	Made DrawRow() logical RECT clipping a little more aggressive (one pixel
 *	in each direction) which for typing a single character meant drawing one instead
 *	 of three rows.
 *	Hope that change was correct :-):-):-)
 *
 *	Revision 2.12  1996/04/18  15:28:59  lewis
 *	qCacheTextMeasurementsForOneRow optimization
 *	CalcSegmentSize () now returns width - not Led_Size.
 *	Check if (where.h <= 0) on map of click location to point in text.
 *	new MultiRowTextImager::PurgeUnneededMemory ().
 *
 *	Revision 2.11  1996/03/16  18:54:00  lewis
 *	No more need for H()/V() accessor fuctions.
 *	Fixed args to CalcSegmentSize() for new tabstop support.
 *
 *	Revision 2.10  1996/02/26  22:26:27  lewis
 *	Use Led_Min etc instead of Min().
 *	Draw/DrawInterLineSpace etc now take bool printing parameter, and we don't
 *	erase background if its true.
 *
 *	Revision 2.9  1996/02/05  04:21:26  lewis
 *	Use new inherited typedef
 *	Get rid of SpecifyTextStore () override, and use HookGainedNewTextStore_
 *	etc instead.
 *
 *	Revision 2.8  1996/01/22  05:21:59  lewis
 *	Use Led_Verify instead of VERIFY.
 *	from SetDefaultFont_ call new TabletChangedMetrics() instead
 *	of inline invalidating.
 *	Added new TabletChangedMetrics () override tooinvalidate.
 *	Lose qTmpHackNoTryCatch hacks.
 *	Added MultiRowTextImager::SetMarkerPositionOfStartOfWindowIfRoom()
 *	support.
 *
 *	Revision 2.7  1995/12/15  02:17:39  lewis
 *	Cleanup DrawRow code a bit, and pass along invalidRect arg, and pass
 *	in DrawSegment as well, for logical clipping.
 *
 *	Revision 2.6  1995/12/13  05:58:01  lewis
 *	Use ODNewRgn instead of NewRgn () under opendoc.
 *
 *	Revision 2.5  1995/12/09  05:40:13  lewis
 *	Use new PeekAtTextStore_, partly for efficiency, but mostly to work around
 *	MSVC 4.0 (and earlier) bugs with vtables before object complete.
 *
 *	Revision 2.4  1995/11/04  23:14:40  lewis
 *	Fixed a number of assumptions about rows within a line all being the same height.
 *	Cleaned up draw loop a bit, and now call MeasureBaseLine in DrawRow
 *	and pass it to the DrawSegment routines.
 *
 *	Revision 2.3  1995/10/19  22:24:40  lewis
 *	Simplify code using AsQDRect() helper method of LedRect
 *
 *	Revision 2.2  1995/10/09  22:37:51  lewis
 *	Fix specify textstore override to invalidate things as needed for
 *	changing textstores.
 *	add optimization to top-row-reference adjust code to avoid problem with
 *	no facet yet added. Actually this optimization may not be needed anymore
 *	but I doubt it hurts anything....
 *	Use new DidUpdate/AboutToUpdate instead of old PrePost replace.
 *
 *	Revision 2.1  1995/09/06  20:57:00  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.33  1995/06/19  13:45:21  lewis
 *	Use GetEndOfRow (curRow) < to instead of <= to so secondary hilight same
 *	as primary (gee - I hope I got it right this time :-).
 *
 *	Revision 1.32  1995/06/12  03:06:00  lewis
 *	Fix type - Led_Coordinate instead of Led_Distance to silence compiler warning.
 *
 *	Revision 1.31  1995/06/08  05:21:06  lewis
 *	Also in last checkin:
 *	SPR#0315 - fixed Up/Down row arrows should use pixel goal rather than column# goal.
 *	Routine was: MultiRowTextImager::ComputeRelativePosition ().
 *	Fixed MultiRowTextImager::GetSelectionWindowRegion () for case of end of last
 *	line (SPR#0322).
 *
 *	Revision 1.30  1995/06/08  05:20:04  lewis
 *	Code cleanups.
 *	MultiRowTextImager::GetCharAtLocationRowRelative () to return prevChar when
 *	we get offset > H(where) - not >=. This fix made fix for 315 work better, otherwise
 *	See SPR#0328.
 *
 *	Revision 1.29  1995/06/04  04:29:15  lewis
 *	A number of code cleanups using temporrat curPM instead of
 *	curRow.GetMultiRowPartitionElt () in a bunch of places.
 *
 *	Fixed a bug with MultiRowTextImager::GetSelectionWindowRegion ()
 *	with adding interline space rect for all rows with interline space
 *	in the LINE (sb only last row). SPR#0312. Otehr cleanups / fixups
 *	in this routine.
 *
 *	Revision 1.28  1995/06/02  07:10:32  lewis
 *	Minor code cleanups
 *
 *	Revision 1.27  1995/06/02  06:24:12  lewis
 *	Fixed MultiRowTextImager::ScrollSoShowing() for case where we stay
 *	in line, but differnt row (SPR#0305).
 *	Fixed GetSelectionWindowRegion () routine - SPR 0308.
 *	Fixed GetEndOfRow () (DANGEROUS SO LATE IN GAME) - SPR0309.
 *
 *	Revision 1.26  1995/05/30  00:07:03  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.25  1995/05/29  23:51:36  lewis
 *	A number of cleanups to MultiRowTextImager::GetCharLocationRowRelative()
 *	mainly to cleanup and speed performance (SPR 0296).
 *	Code cleanups, added assertions.
 *
 *	Revision 1.24  1995/05/25  09:50:05  lewis
 *	Lots of cleanups using new stackbased acquire alloate/dealocation
 *	SPR 0282.
 *
 *	Revision 1.23  1995/05/24  07:25:19  lewis
 *	Fixed bugs with GetSelectionWindowRegion () - having todo with interline
 *	space and when to show it. Not 100% sure I got external leading right?
 *	SPR 0279.
 *
 *	Revision 1.22  1995/05/21  17:51:26  lewis
 *	fInterLineSpace () must be initialized to -1 - invalid value.
 *
 *	Revision 1.21  1995/05/20  05:00:47  lewis
 *	Lots of changes for SPR 0263 - use GetWidowRect properly and
 *	GetImageWidth() - loose old GetImageRect.
 *	Large changes for better encapsulation of data of MRPartitionMarekrs
 *	data, and now pack it much better - See spr 0257, 0265.
 *	SPR 0127 - GetRowReferenceContaingPos now calls base class version
 *	which does caching of PM.
 *
 *	Revision 1.20  1995/05/18  08:16:24  lewis
 *	Lose old CW5.* bug workarounds (SPR#0249).
 *	Fixed CalculateRowDeltaFromCharDeltaFromTopOfWindow routine to be much simpler
 *	using rowreferences machinery. And implemented complimentary
 *	CalculateCharDeltaFromRowDeltaFromTopOfWindow (long deltaRows) - LedSPR#0253.
 *	Keep extra field in MultiRowTextImager::MultiRowPartitionMarker for fInterlineSpace. - LedSPR#0250.
 *
 *	Revision 1.19  1995/05/16  06:45:31  lewis
 *	Added new MultiRowTextImager::GetSelectionWindowRegion ()
 *
 *	Revision 1.18  1995/05/09  23:37:54  lewis
 *	Lose unused withWhat parameter to Pre/PostReplace - SPR 0240
 *	Comment out assertion that fSubRowInTopLineInWindow <= fTopLinePartitionMarkerInWindow->GetRowCount ()
 *	cuz too slow - even for debug mode - reading in large files. SPR 0243.
 *
 *	Revision 1.17  1995/05/08  03:20:49  lewis
 *	Massive changes to AdjustBounds, how we compute how many rows in window,
 *	etc. Should now all work for interline space, or funny font sytles
 *	so lines/rows different heights.
 *	Now keep separate caches for different purposes, invalidate and
 *	fill on demand. Rather than claling AdjustBounds for mutliple purposes.
 *	Now AdjustBounds ONLY FOR UPDATING SBAR. Call InvalTotalRowsInWindow
 *	to invalidate that feild, and GetTotalRowsInWindow computes the new value
 *	and caches it. SPR 0236.
 *	Also - fixed cursor into dbcs char bug - SPR 0237.
 *
 *	Revision 1.16  1995/05/06  19:53:50  lewis
 *	Dropped compat with qCallingBaseClassMemberFunctionWithNestedClassesSymCPlusPlusBug for old SYMC++ compiler.
 *	Renamed GetByteLength->GetLength () - SPR #0232.
 *	Use Led_tChar instead of char for UNICODE support - SPR 0232.
 *	Code cleanups for adjustbounds and related code.
 *
 *	Revision 1.15  1995/05/05  19:53:39  lewis
 *	Use Led_SmallStackBuffer template - class gone.
 *	Support for (GetForceAllRowsShowing in AdjustBounds() - SPR 0227.
 *	Support for new MesasureSegmentWidth/Height - SPR 0226.
 *	More...
 *
 *	Revision 1.14  1995/05/03  19:12:00  lewis
 *	Substantial changes to MutliRowImager::AdjsutBounds() to fix several bugs
 *	with how it adjusted how far down we were when we attempt to scroll too far.
 *	Especailly bugs with respect to interlinespace/LVEj SBSMode.
 *
 *	Revision 1.13  1995/04/25  03:01:10  lewis
 *	SPR 0218 - MultiRowTextImager::Coalese()
 *	SPR 0220 - MultiRowTextImager::GetCharLocationRowRelative ()
 *
 *	Revision 1.12  1995/04/20  06:48:33  lewis
 *	Fix compile errors that only came up with MWRERKS compiler (they may be
 *	right - CTOR initializer for array?).
 *	Also fix ComputeRowsInWindow function to take into account
 *	interline space.
 *
 *	Revision 1.11  1995/04/18  00:43:43  lewis
 *	Cleanups.
 *
 *	Revision 1.10  1995/04/18  00:18:22  lewis
 *	Added MultiRowTextImager::GetStartOfRowContainingPosition and END too.
 *	Added then removed workaround for DidUpdate call bug (finailization).
 *	Significant changes to MultiRowTextImager::GetCharLocationRowRelative().
 *
 *	Revision 1.9  1995/04/11  02:56:56  lewis
 *	Added assertions on GetStableTypingRgn for not splitting characters.
 *	Fixup computations for GetInterLineSpace() calls - hilight bug
 *	+++?
 *
 *	Revision 1.8  1995/03/29  22:48:44  lewis
 *	Added new method - MultiRowTextImager::CountRowDifference ().
 *
 *	Revision 1.7  1995/03/29  21:22:59  lewis
 *	Fixed SPR 6828. Actually - this was something of a big deal. It seems
 *	the various editors I've looked at don't totally agree on how they
 *	deal with selecting the character which is used to wrap the line.
 *	See comments in SPR. I think this is fixed, and HOPE I haven't introduced
 *	any new troubles.
 *
 *	Revision 1.6  1995/03/23  03:55:11  lewis
 *	Fixed MultiRowTextImager::GetCharAtLocationRowRelative () - see SPR# 179,
 *	172.
 *
 *	Revision 1.5  1995/03/15  01:07:55  lewis
 *	Quite a few changes.
 *	MutltiRowTextImager::Draw() broken into a few subroutines - DrawRow ()
 *	and DrawInterLineSpace() - to facilitate refineing draw behavior
 *	in subclasses.
 *	Also - take inter line space into account better in code to map pixels
 *	to char offsets - count interline space as part of last row in line for
 *	this purpose.
 *	Fix MutilRowImager::GetEndOfRow() bug - LedSPR#167.
 *
 *	Revision 1.4  1995/03/13  03:23:34  lewis
 *	Lots of changes including some naming ones. Renamed Array/BlockAllocated to
 *	be Led_prefixed.
 *	Also - a number of sublte changes to lift the restriction that lines must
 *	end in NL (useful for implementing LVEJ side-by-side mode). Allowed
 *	old end-- code to get cleaned up a bit anyhow and made more rational.
 *	Turns out a simple way of saying it was don't draw bogus chara
 *	at end of buffer, and dont draw that NL.
 *	I also added support for interline space by just incrementing fPixelHieghtCache
 *	for the whole line. Diff between this and end of rowarray is interline space.
 *	I think there is still one more lingering bug with this code - needs to
 *	be fixed.
 *
 *	Revision 1.3  1995/03/03  15:56:56  lewis
 *	Alot of code cleanups - deleting unused (ifdefd out) code.
 *	Fix bad assertion (SRP #154) in AdjustPotentialTopRowReferenceSoWholeWindowUsed.
 *	Added missing break at the end of MultiRowTextImager::ScrollSoShowing ()
 *	to cure infinite loop on large selection bug (SPR#153).
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

#include	<climits>

#include	"LedSupport.h"
#include	"LedGDI.h"

#include	"MultiRowTextImager.h"






#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif





#if		qLedUsesNamespaces
	namespace	Led {
#endif







/*
 ********************************************************************************
 ********************************* MultiRowTextImager ***************************
 ********************************************************************************
 */
MultiRowTextImager::MultiRowTextImager ():
	inherited (),
	fPMCacheMgr (),
	fTopLinePartitionMarkerInWindow (NULL),
	fSubRowInTopLineInWindow (0),
	fTotalRowsInWindow (0)				// value must be computed
{
}

MultiRowTextImager::~MultiRowTextImager ()
{
	Led_Assert (fTopLinePartitionMarkerInWindow == NULL);
}

void	MultiRowTextImager::HookLosingTextStore ()
{
	inherited::HookLosingTextStore ();
	HookLosingTextStore_ ();
}

void	MultiRowTextImager::HookLosingTextStore_ ()
{
	SetPartition (NULL);
}

void	MultiRowTextImager::HookGainedNewTextStore ()
{
	inherited::HookGainedNewTextStore ();
	HookGainedNewTextStore_ ();
}

void	MultiRowTextImager::HookGainedNewTextStore_ ()
{
	if (GetPartition ().IsNull ()) {
		SetPartition (MakeDefaultPartition ());
	}
}

void	MultiRowTextImager::SetPartition (const PartitionPtr& partitionPtr)
{
	#if		qAutoPtr_NoResetMethodBug
		fPMCacheMgr = auto_ptr<PMInfoCacheMgr> (NULL);
	#else
		fPMCacheMgr.reset ();
	#endif
	inherited::SetPartition (partitionPtr);
	if (partitionPtr.IsNull ()) {
		InvalidateTotalRowsInWindow ();
		fTopLinePartitionMarkerInWindow = NULL;
	}
	else {
		#if		qAutoPtrBrokenBug
			fPMCacheMgr = Led_RefCntPtr<PMInfoCacheMgr> (new PMInfoCacheMgr (*this));
		#else
			fPMCacheMgr = auto_ptr<PMInfoCacheMgr> (new PMInfoCacheMgr (*this));
		#endif
		fTopLinePartitionMarkerInWindow = GetFirstPartitionMarker ();
//		ReValidateSubRowInTopLineInWindow ();
InvalidateTotalRowsInWindow ();
		AssureWholeWindowUsedIfNeeded ();
		InvalidateScrollBarParameters ();	// even if we don't change the top row, we might change enuf about the text to change sbar
	}
}

PartitioningTextImager::PartitionPtr	MultiRowTextImager::MakeDefaultPartition () const
{
	return new LineBasedPartition (GetTextStore ());
}

MultiRowTextImager::PartitionElementCacheInfo	MultiRowTextImager::GetPartitionElementCacheInfo (Partition::PartitionMarker* pm) const
{
	return fPMCacheMgr->GetPartitionElementCacheInfo (pm);
}

MultiRowTextImager::PartitionElementCacheInfo	MultiRowTextImager::GetPartitionElementCacheInfo (MultiRowTextImager::RowReference row) const
{
	return GetPartitionElementCacheInfo (row.GetPartitionMarker ());
}

bool	MultiRowTextImager::GetIthRowReferenceFromHere (RowReference* adjustMeInPlace, long ith) const
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

size_t	MultiRowTextImager::GetRowNumber (RowReference rowRef) const
{
	// NB: This routine is VERY EXPENSIVE, if the text above the given row has not yet been wrapped, since
	// it forces a wrap. This is meant only to be a convenient code-saver in implementing rownumber based
	// APIs - even though their use is discouraged...
	size_t	rowNumber	=	rowRef.GetSubRow ();
	Led_AssertNotNil (rowRef.GetPartitionMarker ());
	for (PartitionMarker* cur = rowRef.GetPartitionMarker ()->GetPrevious (); cur != NULL; cur = cur->GetPrevious ()) {
		rowNumber += GetPartitionElementCacheInfo (cur).GetRowCount ();
	}
	return (rowNumber);
}

/*
@METHOD:		MultiRowTextImager::CountRowDifference
@DESCRIPTION:	<p>Count the # of rows from one rowreference to the other (order doesn't matter)
	<p>See also @'MultiRowTextImager::CountRowDifferenceLimited'</p>
*/
size_t	MultiRowTextImager::CountRowDifference (RowReference lhs, RowReference rhs) const
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
	size_t				lhsMarkerStart	=	lhsPM->GetStart ();
	size_t				rhsMarkerStart	=	rhsPM->GetStart ();
	bool				leftSmaller	=	(
										(lhsMarkerStart < rhsMarkerStart) or
										((lhsMarkerStart == rhsMarkerStart) and lhs.GetSubRow () <= rhs.GetSubRow ())
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

/*
@METHOD:		MultiRowTextImager::CountRowDifferenceLimited
@DESCRIPTION:	<p>Count the # of rows from one rowreference to the other (order doesn't matter), but
			never count a number of rows exceeding 'limit'. Just return 'limit' if there are 'limit' rows or more.</p>
				<p>The reason you would use this intead of @'MultiRowTextImager::CountRowDifference' is if you are
			only checking to see the rowcount is 'at least' something, and you dont wnat to count all the rows. That CAN
			be a pig performance dog - since it tends to force a word-wrap.</p>
				<p>See also @'MultiRowTextImager::CountRowDifference'</p>
*/
size_t	MultiRowTextImager::CountRowDifferenceLimited (RowReference lhs, RowReference rhs, size_t limit) const
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
	size_t				lhsMarkerStart	=	lhsPM->GetStart ();
	size_t				rhsMarkerStart	=	rhsPM->GetStart ();
	bool			leftSmaller	=	(
									 (lhsMarkerStart < rhsMarkerStart) or
									 ((lhsMarkerStart == rhsMarkerStart) and lhs.GetSubRow () <= rhs.GetSubRow ())
									);
	RowReference	firstRowRef	=	leftSmaller? lhs: rhs;
	RowReference	lastRowRef	=	leftSmaller? rhs: lhs;
	
	size_t	rowsGoneBy	=	0;
	for (RowReference cur = firstRowRef; cur != lastRowRef; rowsGoneBy++) {
		bool	result	=	GetIthRowReferenceFromHere (&cur, 1);
		Led_Assert (result);
		if (rowsGoneBy >= limit) {
			break;
		}
	}
	return (rowsGoneBy);
}

size_t	MultiRowTextImager::GetTopRowInWindow () const
{
	// NB: Use of this function is discouraged as it is inefficent in the presence of word-wrapping
	return (GetRowNumber (GetTopRowReferenceInWindow ()));
}

size_t	MultiRowTextImager::GetTotalRowsInWindow () const
{
	return GetTotalRowsInWindow_ ();
}

size_t	MultiRowTextImager::GetLastRowInWindow () const
{
	// NB: Use of this function is discouraged as it is inefficent in the presence of word-wrapping
	return (GetRowNumber (GetLastRowReferenceInWindow ()));
}

void	MultiRowTextImager::SetTopRowInWindow (size_t newTopRow)
{
	// NB: Use of this function is discouraged as it is inefficent in the presence of word-wrapping
#if		0
	Led_Assert (newTopRow <= GetRowCount ());		// We require this, but don't call since would cause word-wrapping of entire text...
#endif


	SetTopRowInWindow (GetIthRowReference (newTopRow));

	Led_Assert (GetTopRowInWindow () == newTopRow);	// Since a SetTopRowInWindow() was called - all the
													// intervening lines have been wrapped anyhow - may
													// as well check we have our definitions straight...
}

void	MultiRowTextImager::AssureWholeWindowUsedIfNeeded ()
{
	SetTopRowInWindow (GetTopRowReferenceInWindow ());
}

/*
@METHOD:		MultiRowTextImager::GetMarkerPositionOfStartOfWindow
@DESCRIPTION:	<p>Efficient implemenation of @'TextImager::GetMarkerPositionOfStartOfWindow'</p>
*/
size_t	MultiRowTextImager::GetMarkerPositionOfStartOfWindow () const
{
	return (GetStartOfRow (GetTopRowReferenceInWindow ()));
}

/*
@METHOD:		MultiRowTextImager::GetMarkerPositionOfEndOfWindow
@DESCRIPTION:	<p>Efficient implemenation of @'TextImager::GetMarkerPositionOfEndOfWindow'</p>
*/
size_t	MultiRowTextImager::GetMarkerPositionOfEndOfWindow () const
{
	return GetEndOfRow (GetLastRowReferenceInWindow ());
}

size_t		MultiRowTextImager::GetMarkerPositionOfStartOfLastRowOfWindow () const
{
	return GetStartOfRow (GetLastRowReferenceInWindow ());
}

long	MultiRowTextImager::CalculateRowDeltaFromCharDeltaFromTopOfWindow (long deltaChars) const
{
	Led_Assert (long (GetMarkerPositionOfStartOfWindow ()) >= 0 - deltaChars);
	size_t			pos			=	long (GetMarkerPositionOfStartOfWindow ()) + deltaChars;
	RowReference	targetRow	=	GetRowReferenceContainingPosition (pos);
	size_t			rowDiff		=	CountRowDifference (targetRow, GetTopRowReferenceInWindow ());
	return (deltaChars>=0)? rowDiff: -long(rowDiff);
}

long	MultiRowTextImager::CalculateCharDeltaFromRowDeltaFromTopOfWindow (long deltaRows) const
{
	RowReference	row	= GetIthRowReferenceFromHere (GetTopRowReferenceInWindow (), deltaRows);
	return (long (GetStartOfRow (row)) - long (GetMarkerPositionOfStartOfWindow ()));
}

void	MultiRowTextImager::ScrollByIfRoom (long downByRows)
{
	RowReference	newTopRow	=	GetTopRowReferenceInWindow ();
	(void)GetIthRowReferenceFromHere (&newTopRow, downByRows);				// ignore result cuz we did say - IF-ROOM!
	SetTopRowInWindow (newTopRow);
}

/*
@METHOD:		MultiRowTextImager::ScrollSoShowing
@DESCRIPTION:	<p>Implement @'TextImager::ScrollSoShowing' API.</p>
*/
void	MultiRowTextImager::ScrollSoShowing (size_t markerPos, size_t andTryToShowMarkerPos)
{
	Led_Assert (markerPos <= GetLength ());				// Allow any marker position (not just character?)
	Led_Assert (fTotalRowsInWindow == 0 or fTotalRowsInWindow == ComputeRowsThatWouldFitInWindowWithTopRow (GetTopRowReferenceInWindow ()));

	if (andTryToShowMarkerPos == 0) {			// special flag indicating we don't care...
		andTryToShowMarkerPos = markerPos;
	}
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
	while (markerPos < newTop.GetPartitionMarker ()->GetStart ()) {
		newTop = RowReference (newTop.GetPartitionMarker ()->GetPrevious (), 0);
	}
	// only try scrolling down at all if we don't already fit in the window - cuz otherwise - we could artificially
	// scroll when not needed.
	if (not PositionWouldFitInWindowWithThisTopRow (markerPos, newTop)) {
		while (markerPos > newTop.GetPartitionMarker ()->GetEnd ()) {
			if (newTop.GetPartitionMarker ()->GetNext () == NULL) {
				// could be going to row IN last line
				break;
			}
			newTop = RowReference (newTop.GetPartitionMarker ()->GetNext (), 0);	// use row 0 to avoid computing RowCount()
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
	if (CountRowDifferenceLimited (originalTop, newTop, kRowMoveThreshold + 1) > kRowMoveThreshold) {
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

void	MultiRowTextImager::SetTopRowInWindow (RowReference row)
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
@METHOD:		MultiRowTextImager::Draw
@DESCRIPTION:	<p>Implement the basic drawing of the @'TextImager::Draw' API by breaking the content
			up into 'rows' of text and drawing each row with @'MultiRowTextImager::DrawRow'. This implementation
			calls @'TextImager::EraseBackground' to erase the background before drawing the text (drawing the actual
			text is typically done in TRANSPARENT mode). Draw the space between lines (interline space) with
			@'MultiRowTextImager::DrawInterLineSpace'.</p>
				<p>This routine also respecs the @'TextImager::GetImageUsingOffscreenBitmaps' flag, and handles the
			ofscreen imaging (to reduce flicker). Note that if the 'printing' argument is set- this overrides the offscreen bitmaps
			flag, and prevents offscreen drawing.</p>
*/
void	MultiRowTextImager::Draw (const Led_Rect& subsetToDraw, bool printing)
{
	Invariant ();

	Led_Rect	rowsLeftToDrawRect	=	GetWindowRect ();

	Tablet_Acquirer	tablet_ (this);
	Led_Tablet	tablet	=	tablet_;
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
		//size_t			rowNumberInWindow	=	0;
		size_t			totalRowsInWindow	=	GetTotalRowsInWindow_ ();
		RowReference	topRowInWindow		=	GetTopRowReferenceInWindow ();
		size_t			rowsLeftInWindow	=	totalRowsInWindow;
		for (PartitionMarker* pm = topRowInWindow.GetPartitionMarker (); rowsLeftInWindow != 0; pm = pm->GetNext ()) {
			Led_Assert (pm != NULL);
			size_t		startSubRow		=	0;
			size_t		maxSubRow		=	static_cast<size_t> (-1);
			if (pm == topRowInWindow.GetPartitionMarker ()) {
				startSubRow = topRowInWindow.GetSubRow ();
			}
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
			// Probably this code (and below case as well) is buggy. Setting back color in offscreen port (which is current now).
			// But the code has been in place for quite some time (dont think broken by my offscreen bitmap move to LedGDI) with no
			// noticable bugs/problems... Reconsider later...
			// LGP 2001-05-11
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
@METHOD:		MultiRowTextImager::DrawPartitionElement
@DESCRIPTION:	<p></p>
*/
void	MultiRowTextImager::DrawPartitionElement (PartitionMarker* pm, size_t startSubRow, size_t maxSubRow, Led_Tablet tablet, OffscreenTablet* offscreenTablet, bool printing, const Led_Rect& subsetToDraw, Led_Rect* remainingDrawArea, size_t* rowsDrawn)
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
	PartitionElementCacheInfo	pmCacheInfo		=	GetPartitionElementCacheInfo (pm);
	size_t						endSubRow		=	min (pmCacheInfo.GetRowCount ()-1, maxSubRow);
	*rowsDrawn = 0;

	size_t	partLen		=	end-start;
	Led_SmallStackBuffer<Led_tChar>	partitionBuf (partLen);
	CopyOut (start, partLen, partitionBuf);

	for (size_t subRow = startSubRow; subRow <= endSubRow; ++subRow) {
		Led_Rect					currentRowRect	=	*remainingDrawArea;
		currentRowRect.bottom = currentRowRect.top + pmCacheInfo.GetRowHeight (subRow);
		Led_Distance				interlineSpace	=	(subRow == pmCacheInfo.GetLastRow ())? pmCacheInfo.GetInterLineSpace (): 0;
		if (
			(currentRowRect.bottom + Led_Coordinate (interlineSpace) > subsetToDraw.top) and
			(currentRowRect.top < subsetToDraw.bottom)
			) {

			/*
			 *	patch start/end/len to take into account rows...
			 */
			size_t	rowStart	=	start + pmCacheInfo.PeekAtRowStart (subRow);
			size_t	rowEnd		=	end;
			if (subRow < pmCacheInfo.GetLastRow ()) {
				rowEnd =  pm->GetStart () + pmCacheInfo.PeekAtRowStart (subRow+1);	// 'end' points just past last character in row
			}
			{
				if (subRow == pmCacheInfo.GetLastRow ()) {
					Led_Assert (pm->GetEnd () > 0);
					size_t	markerEnd	=	pm->GetEnd ();
					Led_Assert (markerEnd <= GetLength () + 1);
					if (markerEnd == GetLength () + 1) {
						rowEnd = GetLength ();
					}
					else {
						size_t	prevToEnd	=	FindPreviousCharacter (markerEnd);
						if (prevToEnd >= rowStart) {
							Led_tChar	lastChar;
							CopyOut (prevToEnd, 1, &lastChar);
							if (RemoveMappedDisplayCharacters (&lastChar, 1) == 0) {
								rowEnd = (prevToEnd);
							}
						}
					}
				}
				Led_Assert (rowEnd == GetEndOfRowContainingPosition (rowStart));
			}

#if 1
			TextLayoutBlock_Copy	rowText = GetTextLayoutBlock (rowStart, rowEnd);
#else
			TextLayoutBlock_Basic	rowText (partitionBuf + (rowStart-start), partitionBuf + (rowStart-start) + (rowEnd-rowStart));
#endif

			if (offscreenTablet != NULL) {
				tablet = offscreenTablet->PrepareRect (currentRowRect, interlineSpace);
			}

			{
				/*
				 *	Not sure why I didn't always do this? But changed from just setting RHS/LHS to subsetToDraw
				 *	to this full intersection as part of SPR#1322 - LGP 2003-04-01.
				 */
				Led_Rect	invalidRowRect	=	Intersection (currentRowRect, subsetToDraw);
				DrawRow (tablet, currentRowRect, invalidRowRect, rowText, rowStart, rowEnd, printing);
			}

			/*
			 *	Now erase/draw any interline space.
			 */
			if (interlineSpace != 0) {
				size_t	hilightStart	=	GetSelectionStart ();
				size_t	hilightEnd		=	GetSelectionEnd ();
				bool	segmentHilightedAtEnd	=	GetSelectionShown () and (hilightStart < rowEnd) and (rowEnd <= hilightEnd);
				if (pm->GetNext () == NULL and subRow == pmCacheInfo.GetLastRow ()) {
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

Led_Rect	MultiRowTextImager::GetCharLocation (size_t afterPosition)	const
{
	return (GetCharLocationRowRelative (afterPosition, RowReference (GetFirstPartitionMarker (), 0)));
}

Led_Rect	MultiRowTextImager::GetCharWindowLocation (size_t afterPosition)	const
{
	Led_Point	windowOrigin	=	GetWindowRect ().GetOrigin () - Led_Point (0, GetHScrollPos ());
	return (windowOrigin +
		GetCharLocationRowRelative (afterPosition, GetTopRowReferenceInWindow (), GetTotalRowsInWindow_ ())
	);
}

size_t	MultiRowTextImager::GetCharAtLocation (const Led_Point& where) const
{
	return (GetCharAtLocationRowRelative (where, RowReference (GetFirstPartitionMarker (), 0)));
}

size_t	MultiRowTextImager::GetCharAtWindowLocation (const Led_Point& where) const
{
	Led_Point	windowOrigin	=	GetWindowRect ().GetOrigin () - Led_Point (0, GetHScrollPos ());
	return (GetCharAtLocationRowRelative (where - windowOrigin,
											GetTopRowReferenceInWindow (),
											GetTotalRowsInWindow_ ()
										)
		);
}

size_t	MultiRowTextImager::GetStartOfRow (size_t rowNumber) const
{
	// NB: Use of routines using rowNumbers force word-wrap, and so can be quite slow.
	// Routines using RowReferences often perform MUCH better
	return (GetStartOfRow (GetIthRowReference (rowNumber)));
}

size_t	MultiRowTextImager::GetStartOfRowContainingPosition (size_t charPosition) const
{
	return (GetStartOfRow (GetRowReferenceContainingPosition (charPosition)));
}

size_t	MultiRowTextImager::GetEndOfRow (size_t rowNumber) const
{
	// NB: Use of routines using rowNumbers force word-wrap, and so can be quite slow.
	// Routines using RowReferences often perform MUCH better
	return (GetEndOfRow (GetIthRowReference (rowNumber)));
}

size_t	MultiRowTextImager::GetEndOfRowContainingPosition (size_t charPosition) const
{
	return (GetEndOfRow (GetRowReferenceContainingPosition (charPosition)));
}

size_t	MultiRowTextImager::GetRealEndOfRow (size_t rowNumber) const
{
	// NB: Use of routines using rowNumbers force word-wrap, and so can be quite slow.
	// Routines using RowReferences often perform MUCH better
	return (GetRealEndOfRow (GetIthRowReference (rowNumber)));
}

size_t	MultiRowTextImager::GetRealEndOfRowContainingPosition (size_t charPosition) const
{
	return (GetRealEndOfRow (GetRowReferenceContainingPosition (charPosition)));
}

size_t	MultiRowTextImager::GetStartOfRow (RowReference row) const
{
	PartitionMarker*	cur		=	row.GetPartitionMarker ();
	size_t				subRow	=	row.GetSubRow ();
	Led_AssertNotNil (cur);
	return (cur->GetStart () + (subRow==0? 0: GetPartitionElementCacheInfo (cur).GetLineRelativeRowStartPosition (subRow)));
}

size_t	MultiRowTextImager::GetEndOfRow (RowReference row) const
{
	PartitionMarker*	cur		=	row.GetPartitionMarker ();
	size_t				subRow	=	row.GetSubRow ();
	Led_AssertNotNil (cur);
	PartitionElementCacheInfo	pmCacheInfo	=	GetPartitionElementCacheInfo (cur);
	if (subRow == pmCacheInfo.GetLastRow ()) {
		// Be careful about NL at end. If we end with an NL, then don't count that.
		// And for the last PM - it contains a bogus empty character. Dont count
		// that either.
		Led_Assert (cur->GetEnd () > 0);

		size_t	markerEnd	=	cur->GetEnd ();
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
	else {
		return (cur->GetStart () + pmCacheInfo.GetLineRelativeRowStartPosition (subRow+1));
	}
}

size_t	MultiRowTextImager::GetRealEndOfRow (RowReference row) const
{
	PartitionMarker*	cur		=	row.GetPartitionMarker ();
	size_t				subRow	=	row.GetSubRow ();
	Led_AssertNotNil (cur);
	PartitionElementCacheInfo	pmCacheInfo	=	GetPartitionElementCacheInfo (cur);
	if (subRow == pmCacheInfo.GetLastRow ()) {
		Led_Assert (cur->GetEnd () > 0);
		size_t	markerEnd	=	cur->GetEnd ();
		return (markerEnd);
	}
	else {
		return (cur->GetStart () + pmCacheInfo.GetLineRelativeRowStartPosition (subRow+1));
	}
}

MultiRowTextImager::RowReference	MultiRowTextImager::GetRowReferenceContainingPosition (size_t charPosition) const
{
	Led_Require (charPosition <= GetEnd ());
	PartitionMarker*	pm	=	GetPartitionMarkerContainingPosition (charPosition);
	Led_AssertNotNil (pm);

	size_t	pmStart	=	pm->GetStart ();
	if (charPosition == pmStart) {		// slight speed tweek
		return (RowReference (pm, 0));
	}

	// figure out what subrow the position occurs in, and return that...
	PartitionElementCacheInfo	pmCacheInfo	=	GetPartitionElementCacheInfo (pm);
	return (RowReference (pm, pmCacheInfo.LineRelativePositionInWhichRow (charPosition - pmStart)));
}

size_t	MultiRowTextImager::GetRowContainingPosition (size_t charPosition) const
{
	// Warning: GetRowReferenceContainingPosition () in preference, since
	// it doesn't require call to pm->GetRowCount () - forcing a word-wrap...
	return (GetRowNumber (GetRowReferenceContainingPosition (charPosition)));
}

size_t	MultiRowTextImager::GetRowCount () const
{
	// NB: This is an expensive routine because it forces a word-wrap on all the text!
	size_t	rowCount	=	0;
	for (PartitionMarker* cur = GetFirstPartitionMarker (); cur != NULL; cur = cur->GetNext ()) {
		Led_AssertNotNil (cur);
		Led_Assert (GetPartitionElementCacheInfo (cur).GetRowCount () >= 1);
		rowCount += GetPartitionElementCacheInfo (cur).GetRowCount ();
	}
	return (rowCount);
}

Led_Rect	MultiRowTextImager::GetCharLocationRowRelativeByPosition (size_t afterPosition, size_t positionOfTopRow, size_t maxRowsToCheck)	const
{
	return GetCharLocationRowRelative (afterPosition, GetRowReferenceContainingPosition (positionOfTopRow), maxRowsToCheck);
}

Led_Distance	MultiRowTextImager::GetRowHeight (size_t rowNumber)	const
{
	// NB: Use of routines using rowNumbers force word-wrap, and so can be quite slow.
	// Routines using RowReferences often perform MUCH better
	return (GetRowHeight (GetIthRowReference (rowNumber)));
}

/*
@METHOD:		MultiRowTextImager::GetRowRelativeBaselineOfRowContainingPosition
@DESCRIPTION:	<p>Override/implement @'TextImager::GetRowRelativeBaselineOfRowContainingPosition'.</p>
*/
Led_Distance	MultiRowTextImager::GetRowRelativeBaselineOfRowContainingPosition (size_t charPosition)	const
{
	RowReference	thisRow		=	GetRowReferenceContainingPosition (charPosition);
	size_t			startOfRow	=	GetStartOfRow (thisRow);
	size_t			endOfRow	=	GetEndOfRow (thisRow);
	return MeasureSegmentBaseLine (startOfRow, endOfRow);
}

void	MultiRowTextImager::GetStableTypingRegionContaingMarkerRange (size_t fromMarkerPos, size_t toMarkerPos,
								size_t* expandedFromMarkerPos, size_t * expandedToMarkerPos) const
{
	Led_AssertNotNil (expandedFromMarkerPos);
	Led_AssertNotNil (expandedToMarkerPos);
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

		// If we are strictly before the first row, we won't appear later...
		if (curTopRowRelativeRowNumber == 0 and (fromMarkerPos < start)) {
			break;
		}

		curTopRowRelativeRowNumber++;

		if (Contains (*cur, fromMarkerPos) and Contains (*cur, toMarkerPos)) {
			(*expandedFromMarkerPos) = start;
			(*expandedToMarkerPos) = end;
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

Led_Distance	MultiRowTextImager::GetHeightOfRows (size_t startingRow, size_t rowCount)	const
{
	return (GetHeightOfRows (GetIthRowReference (startingRow), rowCount));
}

Led_Distance	MultiRowTextImager::GetHeightOfRows (RowReference startingRow, size_t rowCount)	const
{
	Led_Distance	height	=	0;
	for (RowReference curRow = startingRow; rowCount > 0; rowCount--) {
		PartitionMarker*			curPM	=	curRow.GetPartitionMarker ();
		PartitionElementCacheInfo	pmCacheInfo	=	GetPartitionElementCacheInfo (curPM);
		height += pmCacheInfo.GetRowHeight (curRow.GetSubRow ());
		if (curRow.GetSubRow () == pmCacheInfo.GetLastRow ()) {
			height += pmCacheInfo.GetInterLineSpace ();
		}
		(void)GetNextRowReference (&curRow);
	}
	return (height);
}

void	MultiRowTextImager::DidUpdateText (const UpdateInfo& updateInfo) throw ()
{
//maynot need this addtion either - since done in PMInfoCache guy...
#if 1
	/*
	 *	This invalidation is way more aggressive than it needs to be. Really all we need
	 *	todo is check if this was one of the rows in the window, and only invalidate then.
	 *	But that check would be at least somewhat costly. So this may be best for now.
	 *	Maybe later try walking the markers from the start of window by n (number of rows currently
	 *	set in total rows in window cache) and see if we are hit. Maybe that wouldn't be
	 *	too bad cuz we could do nothing in the common case where the row cache was already
	 *	invalid.
	 *		--LGP 960912
	 */
	InvalidateTotalRowsInWindow ();
#endif




	InvalidateTotalRowsInWindow ();
	inherited::DidUpdateText (updateInfo);
	Led_AssertNotNil (fTopLinePartitionMarkerInWindow);
//	ReValidateSubRowInTopLineInWindow ();
	AssureWholeWindowUsedIfNeeded ();
	InvalidateScrollBarParameters ();			// even if we don't change the top row, we might change enuf about the text to change sbar
}

void	MultiRowTextImager::SetWindowRect (const Led_Rect& windowRect)
{
	/*
	 *	NB: We only check that the 'heightChanged' because thats all that can affect the number of rows
	 *	shown on the screen. Someone pointed out that the WIDTH of the window rect changing can ALSO
	 *	change the number of rows, and invalidate the caches. They were thinking of the subclass
	 *	WordWrappedTextImager. And then - in the special case where you have implemented the policy
	 *	'wrap-to-window' (which you would NOT - for example - if you are using a ruler to specify margins).
	 *		The point is - it is THERE - where you implement that wrapping policy - e.g. WordWrappedTextImager::GetLayoutMargins() -
	 *	that you would have to hook SetWindowRect () and invalidate the cache.
	 */
	bool	heightChanged	=	GetWindowRect ().GetHeight () != windowRect.GetHeight ();
	inherited::SetWindowRect (windowRect);
	if (heightChanged and PeekAtTextStore () != NULL) {
		InvalidateTotalRowsInWindow ();
		AssureWholeWindowUsedIfNeeded ();
		InvalidateScrollBarParameters ();
	}
}

/*
@METHOD:		MultiRowTextImager::InvalidateAllCaches
@DESCRIPTION:	<p>Hook @'TextImager::InvalidateAllCaches' method to invalidate additional information.
			Invalidate cached row-height/etc information for the entire imager. Invalidate rows
			in a window cached values, etc.</p>
*/
void	MultiRowTextImager::InvalidateAllCaches ()
{
	inherited::InvalidateAllCaches ();
	if (not GetPartition ().IsNull ()) {		// careful that we aren't changing text metrics while we have no textstore attached!!!
		if (fPMCacheMgr.get () != NULL) {
			fPMCacheMgr->ClearCache ();
		}
		InvalidateTotalRowsInWindow ();
//		ReValidateSubRowInTopLineInWindow ();
		AssureWholeWindowUsedIfNeeded ();
		InvalidateScrollBarParameters ();
	}
}

MultiRowTextImager::RowReference	MultiRowTextImager::AdjustPotentialTopRowReferenceSoWholeWindowUsed (const RowReference& potentialTopRow)
{
	/*
	 *	This check is always safe, but probably not a worthwhile optimization, except that it avoids
	 *	some problems about initializing the top-row-reference before we've got a valid
	 *	tablet setup to use.
	 */
	if (potentialTopRow.GetSubRow () == 0 and potentialTopRow.GetPartitionMarker ()->GetPrevious () == NULL) {
		return potentialTopRow;
	}

	Led_Coordinate	windowHeight	=	GetWindowRect ().GetHeight ();
	Led_Coordinate	heightUsed		=	0;

	for (RowReference curRow = potentialTopRow; ; ) {
		PartitionMarker*			curPM		=	curRow.GetPartitionMarker ();
		PartitionElementCacheInfo	pmCacheInfo	=	GetPartitionElementCacheInfo (curPM);
		heightUsed += pmCacheInfo.GetRowHeight (curRow.GetSubRow ());
		if (curRow.GetSubRow () == pmCacheInfo.GetLastRow ()) {
			heightUsed += pmCacheInfo.GetInterLineSpace ();
		}
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

		PartitionMarker*			curPM	=	curRow.GetPartitionMarker ();
		PartitionElementCacheInfo	pmCacheInfo	=	GetPartitionElementCacheInfo (curPM);
		heightUsed += pmCacheInfo.GetRowHeight (curRow.GetSubRow ());
		if (curRow.GetSubRow () == pmCacheInfo.GetLastRow ()) {
			heightUsed += pmCacheInfo.GetInterLineSpace ();
		}
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
		#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
			#pragma push
			#pragma warn -8008
			#pragma warn -8066
		#endif
	Led_Assert (false);	return (potentialTopRow);	// NotReached / silence compiler warnings
		#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
			#pragma pop
		#endif
}

bool	MultiRowTextImager::PositionWouldFitInWindowWithThisTopRow (size_t markerPos, const RowReference& newTopRow)
{
	if (markerPos < GetStartOfRow (newTopRow)) {
		return false;
	}

	size_t			rowCount	=	ComputeRowsThatWouldFitInWindowWithTopRow (newTopRow);
	RowReference	lastRow		=	GetIthRowReferenceFromHere (newTopRow, rowCount-1);

	return (markerPos < GetRealEndOfRow (lastRow));
}

void	MultiRowTextImager::ReValidateSubRowInTopLineInWindow ()
{
	Led_AssertNotNil (fTopLinePartitionMarkerInWindow);

	// don't bother calling GetRowCount () if fSubRowInTopLineInWindow is already ZERO - avoid possible word-wrap
	if (fSubRowInTopLineInWindow != 0) {
#if 1
		size_t	lastRow			=	GetPartitionElementCacheInfo (fTopLinePartitionMarkerInWindow).GetLastRow ();
		if (fSubRowInTopLineInWindow > lastRow) {
			fSubRowInTopLineInWindow = lastRow;
		}
#else
		bool	pmNotWrapped	=	(fTopLinePartitionMarkerInWindow->fPixelHeightCache == Led_Distance (-1));
		size_t	lastRow			=	GetPartitionElementCacheInfo (fTopLinePartitionMarkerInWindow).GetLastRow ();
		if (fSubRowInTopLineInWindow > lastRow) {
			fSubRowInTopLineInWindow = lastRow;
		}
		if (pmNotWrapped) {
			/*
			 *	We invalidated it for this method. But we may have done so prematurely - during
			 *	the context of a marker DidUpdate () method. See SPR#0821
			 *
			 *	Reset it back to invalid so it will be properly layed out.
			 */
			fTopLinePartitionMarkerInWindow->InvalidateCache ();
		}
#endif
	}
}

size_t	MultiRowTextImager::ComputeRowsThatWouldFitInWindowWithTopRow (const RowReference& newTopRow) const
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
		PartitionMarker*			curPM	=	curRow.GetPartitionMarker ();
		PartitionElementCacheInfo	pmCacheInfo	=	GetPartitionElementCacheInfo (curPM);
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

/*
@METHOD:		MultiRowTextImager::GetCharLocationRowRelative
@DESCRIPTION:	<p>Return the row-relative (to get window relative - add top-left of window-rect) bounding rectange of the
			given character cell. Compute the vertical position relative to the given argument 'topRow' and check and most
			'maxRowsToCheck' before just returning a large 'off-the-end' value result</p>
*/
Led_Rect	MultiRowTextImager::GetCharLocationRowRelative (size_t afterPosition, RowReference topRow, size_t maxRowsToCheck)	const
{
	// MUST FIGURE OUT WHAT TODO HERE BETTER - 10000 not good enough answer always...
	const	Led_Rect	kMagicBeforeRect	=	Led_Rect (-10000, 0, 0, 0);
	const	Led_Rect	kMagicAfterRect		=	Led_Rect (10000, 0, 0, 0);

	Led_Require (afterPosition <= GetEnd ());
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
		size_t				subRow	=	curRow.GetSubRow ();
		Led_AssertNotNil (cur);
		size_t	start	=	cur->GetStart ();
		size_t	end		=	cur->GetEnd ();			// end points JUST PAST LAST VISIBLE/OPERATED ON CHAR

		Led_Assert (end <= GetEnd () + 1);

		PartitionElementCacheInfo	pmCacheInfo		=	GetPartitionElementCacheInfo (cur);

		/*
		 *	patch start/end/len to take into account rows...
		 */
		start += pmCacheInfo.PeekAtRowStart (subRow);
		if (subRow < pmCacheInfo.GetLastRow ()) {
			end = cur->GetStart () + pmCacheInfo.PeekAtRowStart (subRow+1);
			Led_Assert (start <= end);
		}

		curTopRowRelativeRowNumber++;

		/*
		 *	When we've found the right row, then add in the right horizontal offset.
		 */
		if (afterPosition >= start and afterPosition < end) {
			Led_Assert (start <= afterPosition);
			Led_Distance	hStart	=	0;
			Led_Distance	hEnd	=	0;
			GetRowRelativeCharLoc (afterPosition, &hStart, &hEnd);
			Led_Assert (hStart <= hEnd);
			return (Led_Rect (topVPos, hStart, pmCacheInfo.GetRowHeight (subRow), hEnd-hStart));
		}

		topVPos += pmCacheInfo.GetRowHeight (subRow);

		if (pmCacheInfo.GetLastRow () == subRow) {
			topVPos += pmCacheInfo.GetInterLineSpace ();
		}

		if (curTopRowRelativeRowNumber >= maxRowsToCheck) {
			break;	// return bogus place at the end...
		}
	}
	while (GetNextRowReference (&curRow));

	return (kMagicAfterRect);
}

size_t		MultiRowTextImager::GetCharAtLocationRowRelative (const Led_Point& where, RowReference topRow, size_t maxRowsToCheck) const
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
		size_t				subRow	=	curRow.GetSubRow ();
		Led_AssertNotNil (cur);
		size_t	start	=	cur->GetStart ();
		#if		qMultiByteCharacters
			Assert_CharPosDoesNotSplitCharacter (start);
		#endif

		PartitionElementCacheInfo	pmCacheInfo	=	GetPartitionElementCacheInfo (cur);

		/*
		 *	patch start/end/len to take into account rows...
		 */
		start += pmCacheInfo.PeekAtRowStart (subRow);
		#if		qMultiByteCharacters
			Assert_CharPosDoesNotSplitCharacter (start);
		#endif

		/*
		 *	Count the interline space as part of the last row of the line for the purpose of hit-testing.
		 */
		Led_Distance	interLineSpaceIfAny	=	(pmCacheInfo.GetLastRow () == subRow)? pmCacheInfo.GetInterLineSpace (): 0;

		curTopRowRelativeRowNumber++;
		if (where.v >= topVPos and where.v < topVPos + Led_Coordinate (pmCacheInfo.GetRowHeight (subRow) + interLineSpaceIfAny)) {
			return GetRowRelativeCharAtLoc (where.h, start);
		}

		if (curTopRowRelativeRowNumber >= maxRowsToCheck) {
			break;	// we've checked enuf...
		}

		topVPos += pmCacheInfo.GetRowHeight (subRow) + interLineSpaceIfAny;
	}
	while (GetNextRowReference (&curRow));

	#if		qMultiByteCharacters
		Assert_CharPosDoesNotSplitCharacter (GetEnd ());
	#endif
	return (GetEnd ());
}

Led_Distance	MultiRowTextImager::CalculateInterLineSpace (const PartitionMarker* /*pm*/) const
{
	return (0);	// no interline space by default
}

/*
@METHOD:		MultiRowTextImager::ContainsMappedDisplayCharacters
@DESCRIPTION:	<p>Override @'TextImager::ContainsMappedDisplayCharacters' to hide '\n' characters.
	See @'qDefaultLedSoftLineBreakChar'.</p>
*/
bool	MultiRowTextImager::ContainsMappedDisplayCharacters (const Led_tChar* text, size_t nTChars) const
{
	return 
			ContainsMappedDisplayCharacters_HelperForChar (text, nTChars, '\n') or
			inherited::ContainsMappedDisplayCharacters (text, nTChars)
		;
}

/*
@METHOD:		MultiRowTextImager::RemoveMappedDisplayCharacters
@DESCRIPTION:	<p>Override @'TextImager::RemoveMappedDisplayCharacters' to hide '\n' characters.</p>
*/
size_t	MultiRowTextImager::RemoveMappedDisplayCharacters (Led_tChar* copyText, size_t nTChars) const
{
	size_t	newLen	=	inherited::RemoveMappedDisplayCharacters (copyText, nTChars);
	Led_Assert (newLen <= nTChars);
	size_t	newerLen	=	RemoveMappedDisplayCharacters_HelperForChar (copyText, newLen, '\n');
	Led_Assert (newerLen <= newLen);
	Led_Assert (newerLen <= nTChars);
	return newerLen;
}







/*
 ********************************************************************************
 ******************* MultiRowTextImager::PartitionElementCacheInfo **************
 ********************************************************************************
 */
void	MultiRowTextImager::PartitionElementCacheInfo::Clear ()
{
	fRep = new Rep ();
}

void	MultiRowTextImager::PartitionElementCacheInfo::IncrementRowCountAndFixCacheBuffers (size_t newStart, Led_Distance newRowsHeight)
{
	fRep->fRowCountCache++;

	// If rowStart array not big enough then allocate it from the heap...
	if (fRep->fRowCountCache > kPackRowStartCount + 1) {
		RowStart_*	newRowStartArray	=	new RowStart_ [fRep->fRowCountCache-1];
		Led_AssertNotNil (newRowStartArray);
		if (fRep->fRowCountCache == kPackRowStartCount + 1 + 1) {
			::memcpy (newRowStartArray, &fRep->fRowStartArray, sizeof (fRep->fRowStartArray));
		}
		else {
			Led_Assert (fRep->fRowCountCache > 2);
			if (fRep->fRowCountCache > kPackRowStartCount+1+1) {
				Led_AssertNotNil (fRep->fRowStartArray);
				(void)memcpy (newRowStartArray, fRep->fRowStartArray, sizeof (newRowStartArray[1])*(fRep->fRowCountCache-2));
				delete[] fRep->fRowStartArray;
			}
		}
		fRep->fRowStartArray = newRowStartArray;
	}

	// If rowHeight array not big enough then allocate it from the heap...
	if (fRep->fRowCountCache > kPackRowHeightCount) {
		RowHeight_*	newRowHeightArray	=	new RowHeight_ [fRep->fRowCountCache];
		Led_AssertNotNil (newRowHeightArray);
		if (fRep->fRowCountCache == kPackRowHeightCount + 1) {
			::memcpy (newRowHeightArray, &fRep->fRowHeightArray, sizeof (fRep->fRowHeightArray));
		}
		else {
			Led_Assert (fRep->fRowCountCache > 1);
			if (fRep->fRowCountCache > kPackRowHeightCount+1) {
				Led_AssertNotNil (fRep->fRowHeightArray);
				memcpy (newRowHeightArray, fRep->fRowHeightArray, sizeof (newRowHeightArray[1])*(fRep->fRowCountCache-1));
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
MultiRowTextImager::PMInfoCacheMgr::PMInfoCacheMgr (MultiRowTextImager& imager):
	fPMCache (),
	fCurFillCachePM (NULL),
	fCurFillCacheInfo (),
	fImager (imager),
	fMyMarker ()
{
// REDO this class to make IT a MarkerOwner - and use THAT markerowner for MyMarker. Then - store an additional MyMarker for EACH marker
// added to cache (just around the PM its used to wrap). Then remove ONLY that PM from the cache in its DIDUpdate.
	PartitionPtr	part	=	imager.GetPartition ();
	Led_Assert (not part.IsNull ());
	part->AddPartitionWatcher (this);
	#if		qAutoPtrBrokenBug
		fMyMarker = Led_RefCntPtr<MyMarker> (new MyMarker (*this));
	#else
		fMyMarker = auto_ptr<MyMarker> (new MyMarker (*this));
	#endif
	TextStore&	ts	=	part->GetTextStore ();
	ts.AddMarker (fMyMarker.get (), 0, ts.GetLength () + 1, part);
}

MultiRowTextImager::PMInfoCacheMgr::~PMInfoCacheMgr ()
{
	PartitionPtr	part	=	fImager.GetPartition ();
	part->RemovePartitionWatcher (this);
	TextStore&	ts	=	part->GetTextStore ();
	ts.RemoveMarker (fMyMarker.get ());
}

MultiRowTextImager::PartitionElementCacheInfo	MultiRowTextImager::PMInfoCacheMgr::GetPartitionElementCacheInfo (Partition::PartitionMarker* pm) const
{
	if (pm == fCurFillCachePM) {
		return fCurFillCacheInfo;	// allow recursive call to get PMCacheInfo (so far) DURING context of call to FillCache()
	}
	typedef	map<Partition::PartitionMarker*,PartitionElementCacheInfo>	MAP_CACHE;
	MAP_CACHE::iterator i	=	fPMCache.find (pm);
	if (i == fPMCache.end ()) {
		try {
			Led_Assert (fCurFillCachePM == NULL);	// can only do one fillcache at a time...
			fCurFillCachePM = pm;
			fImager.FillCache (pm, fCurFillCacheInfo);
			#if		qDebug
				{
					for (size_t t = 0; t < fCurFillCacheInfo.GetRowCount (); ++t) {
						Led_Assert (fCurFillCacheInfo.GetLineRelativeRowStartPosition (t) <= pm->GetLength ());
						Led_Assert (fCurFillCacheInfo.PeekAtRowStart (t) <= pm->GetLength ());
					}
				}
			#endif
			i = fPMCache.insert (MAP_CACHE::value_type (pm, fCurFillCacheInfo)).first;
			#if		qDebug
				{
					Led_Assert (fCurFillCacheInfo.GetRowCount () == i->second.GetRowCount ());
					for (size_t t = 0; t < fCurFillCacheInfo.GetRowCount (); ++t) {
						Led_Assert (fCurFillCacheInfo.PeekAtRowHeight (t) == i->second.PeekAtRowHeight (t));
						Led_Assert (fCurFillCacheInfo.PeekAtRowStart (t) == i->second.PeekAtRowStart (t));
					}
				}
			#endif
			Led_Assert (fCurFillCachePM == pm);
			fCurFillCachePM = NULL;
		}
		catch (...) {
			Led_Assert (fCurFillCachePM == pm);
			fCurFillCachePM = NULL;
			throw;
		}
	}
	return i->second;
}

void	MultiRowTextImager::PMInfoCacheMgr::ClearCache ()
{
	fPMCache.clear ();
}

void	MultiRowTextImager::PMInfoCacheMgr::AboutToSplit (PartitionMarker* pm, size_t /*at*/, void** infoRecord) const throw ()
{
	*infoRecord = pm;
}

void	MultiRowTextImager::PMInfoCacheMgr::DidSplit (void* infoRecord) const throw ()
{
	PartitionMarker*	pm	=	reinterpret_cast<PartitionMarker*> (infoRecord);
	typedef	map<Partition::PartitionMarker*,PartitionElementCacheInfo>	MAP_CACHE;
	MAP_CACHE::iterator i	=	fPMCache.find (pm);
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

void	MultiRowTextImager::PMInfoCacheMgr::AboutToCoalece (PartitionMarker* pm, void** infoRecord) const throw ()
{
	*infoRecord = pm;

	PartitionMarker*	newTopLine	=	NULL;
	bool				useFirstRow	=	false;	// otherwise use last row...
	if (pm == fImager.fTopLinePartitionMarkerInWindow) {
		if (pm->GetNext () == NULL) {
			newTopLine = fImager.fTopLinePartitionMarkerInWindow->GetPrevious ();
			useFirstRow = false;
		}
		else {
			newTopLine = fImager.fTopLinePartitionMarkerInWindow->GetNext ();
			useFirstRow = true;
		}
		Led_AssertNotNil (newTopLine);
	}

	if (newTopLine != NULL) {
//TMPHACK - REALLY should do old comment out code - see old code in MultiRowTextImager::MultiRowPartition::Coalese - trouble is then
// we need to pass MORE info to DID_COALESE trhought eh INFORECORD so we can pass BOTH pm AND the newTopLine (really should do this call there)
// This is a bit of a hack - but AT LEAST should avoid any crashes/flakies we now see - LGP 2002-10-17
//		fImager.SetTopRowInWindow_ (RowReference (newTopLine, useFirstRow? 0: fImager.GetPartitionElementCacheInfo (newTopLine).GetLastRow ()));
		fImager.SetTopRowInWindow_ (RowReference (newTopLine, 0));
	}

#if 0
	MultiRowPartitionMarker*	newTopLine	=	NULL;
	bool						useFirstRow	=	false;	// otherwise use last row...
	if (pm == fTextImager.fTopLinePartitionMarkerInWindow) {
		if (pm->GetNext () == NULL) {
			newTopLine = fTextImager.fTopLinePartitionMarkerInWindow->GetPreviousMRPM ();
			useFirstRow = false;
		}
		else {
			newTopLine = fTextImager.fTopLinePartitionMarkerInWindow->GetNextMRPM ();
			useFirstRow = true;
		}
		Led_AssertNotNil (newTopLine);
	}

	MultiRowPartitionMarker*	successor	=	(MultiRowPartitionMarker*)pm->GetNext ();	// all our pms are subtypes of this type
	inherited::Coalece (pm);				//	deletes pm....
	if (successor != NULL) {
		successor->InvalidateCache ();				// This is where our xtra text goes - so be sure we invalidate its extent...
	}

	// If we must change the top-row - then do so NOW - cuz NOW its safe to call GetRowCount ()
	if (newTopLine != NULL) {
		fTextImager.SetTopRowInWindow_ (RowReference (newTopLine, useFirstRow? 0: fTextImager.GetPartitionElementCacheInfo (newTopLine).GetLastRow ()));
	}
	Led_AssertNotNil (fTextImager.fTopLinePartitionMarkerInWindow);	// can't delete last one...
	Led_AssertMember (fTextImager.fTopLinePartitionMarkerInWindow, MultiRowPartitionMarker);
	fTextImager.InvalidateTotalRowsInWindow ();
#endif
}

void	MultiRowTextImager::PMInfoCacheMgr::DidCoalece (void* infoRecord) const throw ()
{
	PartitionMarker*	pm	=	reinterpret_cast<PartitionMarker*> (infoRecord);
	typedef	map<Partition::PartitionMarker*,PartitionElementCacheInfo>	MAP_CACHE;
	MAP_CACHE::iterator i	=	fPMCache.find (pm);
	if (i != fPMCache.end ()) {
		fPMCache.erase (i);
	}
	fImager.InvalidateTotalRowsInWindow ();
}

void	MultiRowTextImager::PMInfoCacheMgr::MyMarkerDidUpdateCallback ()
{
	fPMCache.clear ();

	/*
	 *	This invalidation is way more aggressive than it needs to be. Really all we need
	 *	todo is check if this was one of the rows in the window, and only invalidate then.
	 *	But that check would be at least somewhat costly. So this may be best for now.
	 *	Maybe later try walking the markers from the start of window by n (number of rows currently
	 *	set in total rows in window cache) and see if we are hit. Maybe that wouldn't be
	 *	too bad cuz we could do nothing in the common case where the row cache was already
	 *	invalid.
	 *		--LGP 960912
	 */
	fImager.InvalidateTotalRowsInWindow ();
}








/*
 ********************************************************************************
 *************** MultiRowTextImager::PMInfoCacheMgr::MyMarker *******************
 ********************************************************************************
 */
MultiRowTextImager::PMInfoCacheMgr::MyMarker::MyMarker (PMInfoCacheMgr& pmInfoCacheMgr):
	fPMInfoCacheMgr (pmInfoCacheMgr)
{
}

void	MultiRowTextImager::PMInfoCacheMgr::MyMarker::DidUpdateText (const UpdateInfo& updateInfo) throw ()
{
	inherited::DidUpdateText (updateInfo);
	fPMInfoCacheMgr.MyMarkerDidUpdateCallback ();
}





#if		qLedUsesNamespaces
}
#endif




// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***


