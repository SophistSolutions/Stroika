/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__TextImager_h__
#define	__TextImager_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/TextImager.h,v 2.125 2004/01/26 20:20:34 lewis Exp $
 */

/*
@MODULE:	TextImager
@DESCRIPTION:
		<p>The @'TextImager' class is one of central importance in Led. It is the (abstract) class which
	does all the imaging - drawing of text to an output device.</p>
 */


/*
 *
 * Changes:
 *	$Log: TextImager.h,v $
 *	Revision 2.125  2004/01/26 20:20:34  lewis
 *	SPR#1604: Added Get/SetSelectionGoalColumnm and RecomputeSelectionGoalColumn to help implement goal column functionality with arrowkeys. Added GoalColumnRecomputerControlContext to allow prevention of updating goal column in special cases (like when using up/down arrows). As speed tweek (partly needed cuz we got rid of MRTI overrides and partly just a good idea I noticed) - use stuff like GetStartOfNextRowFromRowContainingPosition () instead of GetRowContainingPosition (in other words - dont use explicit row#s). Similarly - use GetRowRelativeCharAtLoc instead of GetCharAtLocation. Updated docs on GetCharLocation etc to use 'relative' versions as a speedtweek
 *	
 *	Revision 2.124  2003/11/27 02:53:24  lewis
 *	doccomments
 *	
 *	Revision 2.123  2003/11/03 20:05:39  lewis
 *	add new TextImager::GetTextLayoutBlock () method - which in subclasses - can take into account subclasses to use an intiail dir from first row in subsequent rows. Related - added PartitioningTextImager::GetPrimaryPartitionTextDirection ()
 *	
 *	Revision 2.122  2003/04/08 21:38:50  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.121  2003/04/08 21:38:04  lewis
 *	SPR#1404: while working on table cell selection, added GetSelectionWindowRects method (NOTE -
 *	PREV checkin comment was a MISTAKE - this comment belongs with last checking
 *	
 *	Revision 2.120  2003/04/08 21:30:20  lewis
 *	<checking comment mistake - see next comment>
 *	
 *	Revision 2.119  2003/03/14 14:36:03  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.118  2003/03/07 22:31:38  lewis
 *	fix access (public) to TextImager::GetIntraRowTextWindowBoundingRect
 *	
 *	Revision 2.117  2003/03/07 21:44:03  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.116  2003/03/06 20:49:00  lewis
 *	SPR#1324- beginnings of Get/SetSelection support for Table
 *	
 *	Revision 2.115  2003/02/03 17:09:22  lewis
 *	doccomments
 *	
 *	Revision 2.114  2003/02/01 15:49:17  lewis
 *	last SPR# was really 1279(not 1271)
 *	
 *	Revision 2.113  2003/02/01 15:48:18  lewis
 *	SPR#1271- Added TextImager::GetDefaultSelectionFont () and overrode in StandardStyledTextImager,
 *	and used that in WM_GETFONT Led_Win32 callback
 *	
 *	Revision 2.112  2003/01/29 19:15:10  lewis
 *	SPR#1265- use the keyword typename instead of class in template declarations
 *	
 *	Revision 2.111  2003/01/29 17:59:49  lewis
 *	SPR#1264- Get rid of most of the SetDefaultFont overrides and crap - and made InteractiveSetFont
 *	REALLY do the interactive command setfont - and leave SetDefaultFont to just setting the 'default font'
 *	
 *	Revision 2.110  2003/01/13 23:53:50  lewis
 *	for SPR#1237 - wrap TextImager::GetTextBoundingRect in qSupportLed30CompatAPI (OBSOLETE)
 *	
 *	Revision 2.109  2003/01/12 18:42:49  lewis
 *	fix case of BiDiLayoutEngine #include (UNIX case sensative)
 *	
 *	Revision 2.108  2003/01/11 19:28:39  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.107  2003/01/11 18:13:15  lewis
 *	for SPR#1232 - clarify the docs for GetRowRelativeCharAtLoc
 *	
 *	Revision 2.106  2003/01/10 15:28:04  lewis
 *	Lost selectBOL/EOL args to GetIntraRowTextWindowBoundingRect (),
 *	and instead put that logic directly in TextImager::GetRowHilightRects (),
 *	and added assert there that we never create overlapping rects.
 *	
 *	Revision 2.105  2003/01/08 15:36:55  lewis
 *	moved TextImager::CopyOut etc to MarkerOwner::CopyOut (same for GetLength/GetEnd/FindNextCharacter/FindPrevCharacter.
 *	Done so they can be somewhat more easily used. No longer inline - but thats not a big deal since not used in
 *	performance critical places (and if they are - you can manually use/cache the TextStore and call its version).
 *	Also - got rid of obsolete TextImager::PeekAtTextStore_ (some old compile BWA)
 *	
 *	Revision 2.104  2002/12/21 03:00:01  lewis
 *	SPR#1217. Added virtual TextImager::InvalidateAllCaches and made SimpleTextImager/MRTI versions
 *	just overrides. No longer need SimpleTextImager::TabletChangedMetrics overrides etc
 *	since now TextImager::TabletChangedMetrics calls TextImager::InvalidateAllCaches.
 *	MultiRowTextImager::PurgeUnneededMemory () no longer needed since TextImager version
 *	now calls InvalidateAllCaches().
 *	
 *	Revision 2.103  2002/12/20 17:59:11  lewis
 *	SPR#1216- Add new helper TextImager::GetRowHilightRects(). Call that from TextImager::DrawRowHilight
 *	instead of manually computing where to draw hilights. Then use that shared code
 *	from TextImager::GetSelectionWindowRegion (thereby fixing IT for BIDI). Also fixed
 *	TextImager::GetSelectionWindowRegion so it handles interline space (I hope - not tested).
 *	And got rid of overrides of GetSelectionWindowRegion from MRTI and SimpleTextImager.
 *	
 *	Revision 2.102  2002/12/16 19:02:57  lewis
 *	SPR#1208 - added TextImager::GetUseSelectEOLBOLRowHilightStyle support
 *	
 *	Revision 2.101  2002/12/13 18:42:38  lewis
 *	SPR#1204- added TextImager::GetTextDirection API
 *	
 *	Revision 2.100  2002/12/06 15:15:40  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.99  2002/12/06 15:13:00  lewis
 *	SPR#1198 - added 2 new APIs to TextImager, implemented them in PartitioningTextImager,
 *	and used  them in MultiRowTextImager and SimpleTextImager:
 *		GetRowRelativeCharLoc ()
 *		GetRowRelativeCharAtLoc ().
 *	
 *	Revision 2.98  2002/12/04 22:25:29  lewis
 *	doccomments
 *	
 *	Revision 2.97  2002/12/04 15:40:43  lewis
 *	doccoments for CalcSegmentSize - SPR#1193
 *	
 *	Revision 2.96  2002/12/03 18:08:47  lewis
 *	part of SPR#1189- drawrowhilight BIDI code. Lose WRTI::DrawRowHilight() code cuz unneeded and
 *	obsolete. Instead handle the hilight logic better in TextImager::DrawRowHilight and add extra arg to
 *	GetIntraRowTextWindowBoundingRect to handle margin case (better than in Led 3.0 - I think). Also - made
 *	GetIntraRowTextWindowBoundingRect protected instead of public
 *	
 *	Revision 2.95  2002/12/03 14:28:53  lewis
 *	SPR#1190 - changed name for (TextImager/Led_Tablet_)::HilightARectangle to HilightArea
 *	
 *	Revision 2.94  2002/12/02 15:57:54  lewis
 *	SPR#1185 - move DrawRow,DrawRowSegments,DrawRowHilight,DrawInterlineSpace from MRTI/SimpleTextImager
 *	to TextImager (share code. Changed their args slightly. For BIDI work.
 *	
 *	Revision 2.93  2002/11/27 15:58:32  lewis
 *	SPR#1183 - as part of BIDI effort - DrawSegment now replaces 'Led_tChar* text' argument with
 *	a 'TextLayoutBlock& text' argument
 *	
 *	Revision 2.92  2002/11/14 17:12:14  lewis
 *	SPR#1171- wrap qSupportLed30CompatAPI around NotificationOf_SelectionChanged
 *	
 *	Revision 2.91  2002/09/22 15:45:06  lewis
 *	doccomments
 *	
 *	Revision 2.90  2002/09/20 02:05:11  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.89  2002/09/19 15:30:34  lewis
 *	get working on VC++6 - added qAccessCheckAcrossInstancesSometimesWrongWithVirtualBase
 *	
 *	Revision 2.88  2002/09/19 13:23:33  lewis
 *	SPR#1098- also set the default value of 'BackgroundTransparent' based on GetBkMode() for
 *	Windows and fix some doc-comments
 *	
 *	Revision 2.87  2002/09/19 13:15:48  lewis
 *	SPR#1098- Added TrivialImager<TEXTSTORE,IMAGER>::SetBackgroundTransparent
 *	
 *	Revision 2.86  2002/05/06 21:33:38  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.85  2001/11/27 00:29:47  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.84  2001/10/20 13:38:57  lewis
 *	tons of DocComment changes
 *	
 *	Revision 2.83  2001/10/19 20:47:14  lewis
 *	DocComments
 *	
 *	Revision 2.82  2001/10/17 20:59:46  lewis
 *	cleanup DocComments
 *	
 *	Revision 2.81  2001/10/17 20:42:54  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.80  2001/10/13 19:52:37  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.79  2001/10/13 19:50:03  lewis
 *	SPR#1059- moved some Led_DrawText<> / Led_GetTextExtent<> code to shared
 *	code in LedHandySimple - GetTextExtent<>/DrawTextBox<>, and docs cleanups
 *	
 *	Revision 2.78  2001/08/29 23:01:45  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.77  2001/08/28 18:43:31  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.76  2001/07/31 15:17:11  lewis
 *	make FontCacheInfoUpdater protected - not private - since used in subclasses
 *	(later maybe get rid of it and bury it in the tablet code)
 *	
 *	Revision 2.75  2001/05/12 22:50:57  lewis
 *	SPR#0917- new TextImager::G/SetImageUsingOffscreenBitmaps () method to
 *	replace obsolete qUseOffscreenBitmapsToReduceFlicker
 *	
 *	Revision 2.74  2001/05/04 17:09:56  lewis
 *	fixup commnets. SPR#0902 - Added GetEffectiveDefaultTextColor () family of functions
 *	to replace direct use of Led_GetTextBackgroundColor/Led_GetTextColor etc.
 *	
 *	Revision 2.73  2001/04/30 22:35:41  lewis
 *	SPR#0886- moved HilightARectangle_SolidHelper from TextImager class to
 *	Led_Tablet_ (and erasebackground_solidhelper)
 *	
 *	Revision 2.72  2001/04/23 16:38:45  lewis
 *	SPR#0876- Moved some utilities from TextImager to LedGDI
 *	
 *	Revision 2.71  2001/01/03 14:58:40  Lewis
 *	qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS
 *	
 *	Revision 2.70  2000/10/21 14:17:46  lewis
 *	fix bug I had introduced in not calling SnagAttributesFromTablet- and
 *	comment on TrivialImager methods a bit more -
 *	and make the CTORs that dont call SnagAttrs protected
 *	
 *	Revision 2.69  2000/10/03 12:54:43  lewis
 *	clearup docs and code for TrivialImager<TEXTSTORE,IMAGER>. Added overloaded Draw().
 *	Reorder calls to
 *	SnagAttributesFromTablet to avoid pure-virtual-called problems in subclasses
 *	
 *	Revision 2.68  2000/09/25 00:41:15  lewis
 *	Added Led_Tablet_::operator->
 *	
 *	Revision 2.67  2000/09/20 08:46:12  lewis
 *	lose qHeavyDebugging stuff I had left in for a couple releases to assure
 *	my new tabstop code was identical
 *	(semanticly - but faster) to old. Seemed to pass test this time.
 *	
 *	Revision 2.66  2000/09/05 21:24:54  lewis
 *	make DTOR public
 *	
 *	Revision 2.65  2000/06/17 07:44:27  lewis
 *	SPR#0792- Add TextImager::GetRowRelativeBaselineOfRowContainingPosition() as
 *	part of fixing this. And impelmnt
 *	in MRTI. And added asserts to MRTI::DrawRowSegments() that the start/end
 *	are reasonable for that segment.
 *	
 *	Revision 2.64  2000/06/13 15:38:47  lewis
 *	SPR#0767- change name (fPixelsPerTabStop to fTWIPSPerTabStop
 *	
 *	Revision 2.63  2000/06/13 15:31:43  lewis
 *	SPR#0767- convert tabstop support to store TWIPS internally
 *	
 *	Revision 2.62  2000/06/04 00:05:02  lewis
 *	Fix SPR#0768- tabstop bug I had introduced a release or two back -
 *	when I was optimizing that code. I put
 *	back in the assertion-checking code that it computes the old values
 *	for another release or two.
 *	
 *	Revision 2.61  2000/04/15 14:32:36  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.60  2000/04/14 22:40:23  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.59  2000/04/11 14:42:01  lewis
 *	lose some very slow qHeavyDebugging stuff in CalcTabstop code - was there
 *	just to test for one release that
 *	I had preserve teh old semantics, and made running in DEBUG builds
 *	needlessly painfully slow
 *	
 *	Revision 2.58  2000/04/05 21:43:25  lewis
 *	SPR#0734- added TextImager::GetIntraRowTextWindowBoundingRect and
 *	revised TextImager::GetRowTextWindowBoundingRect to
 *	work around bug introduced in fix for 0730
 *	
 *	Revision 2.57  2000/03/14 02:08:44  lewis
 *	TYPO
 *	
 *	Revision 2.56  2000/03/14 00:27:16  lewis
 *	SPR#0713- TextImager::StandardTabStopList::ComputeIthTab () and
 *	TextImager::StandardTabStopList::ComputeTabStopAfterPosition ()
 *	speed tweeked for case of very large arguments
 *	
 *	Revision 2.55  2000/03/11 22:04:48  lewis
 *	SPR#0711 - new GetRealEndOfRow* () routines. And fixed PositionWouldFitInWindowWithThisTopRow ()
 *	to call it. And other small fixes to TextInteractor::ScrollToSelection ()
 *	and Simple/MultiRow::ScrollSoShowing ()
 *	
 *	Revision 2.54  2000/03/09 04:40:29  lewis
 *	replace old MultiRowTextImager::HilightARectanle_ (and SimpleTextImager version)
 *	with TextImager::HilightARectanlge() ETC - SPR#0708
 *	
 *	Revision 2.53  2000/01/22 17:34:57  lewis
 *	Added TextImager::ScrollSoShowingHHelper () to share a little code from
 *	SimpleTextImager::ScrollSoShowing /
 *	MultiRowTextImager::ScrollSoShowing () - code cleanups
 *	
 *	Revision 2.52  1999/12/28 13:28:44  lewis
 *	SPR#0687- reversed order of template args to TrivialImager<>
 *	
 *	Revision 2.51  1999/12/21 03:35:14  lewis
 *	SPR#0670- Major revision to MappedDisplayCharacters() APIS. Updated DOCCOMMENTS on them. And since they
 *	triggered wierd ASSERT error due to funky 'gross hack' in PortableGDI_MeasureText - I commented out
 *	that hack - and things seem to work better
 *	
 *	Revision 2.50  1999/12/19 19:13:45  lewis
 *	Added new ContainsMappedDisplayCharacters/ReplaceMappedDisplayCharacters APIs - SPR#0670
 *	
 *	Revision 2.49  1999/12/15 03:14:19  lewis
 *	SPR#0639- Fix TrivialImager<> template to copy background color and font from Tablet
 *	
 *	Revision 2.48  1999/11/30 20:25:03  lewis
 *	Add TextImager::EraseBackground_SolidHelper can call from TextImager::EraseBackground
 *	
 *	Revision 2.47  1999/11/15 21:26:44  lewis
 *	Use new Led_FontObject instead of CFont - so we don't refer to any MFC classes outside of Led_MFC.h
 *	
 *	Revision 2.46  1999/11/13 16:32:19  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.45  1999/06/28 15:16:35  lewis
 *	spr#0598- use mutable instead of const_cast, as appopropriate
 *	
 *	Revision 2.44  1999/05/03 22:05:02  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.43  1999/04/13 19:11:54  lewis
 *	DOCCOMMENTS
 *	
 *	Revision 2.42  1999/03/24 15:42:01  lewis
 *	spr#0561 fixed- TexImager::NoTabletAvailable now subclasses from TextImager::NotFullyInitialized etc
 *	
 *	Revision 2.41  1999/03/03 01:46:57  lewis
 *	change TrivialImager CTOR to be overloaded to take no argument and call that version from subclass
 *	TrivialWordWrappedTextImager so as to avoid calling pure virtual func from base-class CTOR during init of derived class
 *	
 *	Revision 2.40  1999/02/26 22:38:56  lewis
 *	add bug workaround define qWorkAroundWin98UNICODECharImagingBugs - to cover the Win98 GDI bugs which happen under
 *	Win98, and separate out the ones that are SPECIFICLY Win95 ones. ALSO - add workaround for inability to image
 *	SJIS characters in UNICODE under Win95/98, via a GROSS HACK, specific to that code page (but hopefully sufficient).
 *	Many thanx to Sterl for finding this hack, and evaluating what is needed under each OS
 *	
 *	Revision 2.39  1999/02/21 21:48:32  lewis
 *	fix subtle mac-only bug where we sometimes got called (temporarily during init) with a memset(0) fontspec.
 *	Would then match our default one, and return uninitialised fCachedFontMetrics. Add bool to say if inited for mac,
 *	to parallel the test for NULL of the fCachedFont object for WINDOZE
 *	
 *	Revision 2.38  1999/02/12 01:27:28  lewis
 *	DOCCOMMENTS
 *	
 *	Revision 2.37  1998/07/24 01:02:31  lewis
 *	new useful TrivialImager<IMAGER,TEXTSTORE> template (for using imager to IMAGE - not as part of an interactor).
 *	qWorkAroundWin95UNICODECharImagingBugs.
 *	
 *	Revision 2.36  1998/04/25  01:25:24  lewis
 *	StandardTabStopList.
 *	Cleanups - docs - and moved some nested classes so declared (still nested) outside after main TextImager class.
 *
 *	Revision 2.35  1998/04/08  01:41:24  lewis
 *	GetTextStore() now inherited from MarkerOwner
 *
 *	Revision 2.34  1998/03/04  20:16:21  lewis
 *	*** empty log message ***
 *
 *	Revision 2.33  1997/12/24  04:36:55  lewis
 *	*** empty log message ***
 *
 *	Revision 2.32  1997/09/29  14:48:29  lewis
 *	Lose qSupportLed21CompatAPI and qLedFirstIndex support.
 *
 *	Revision 2.31  1997/07/27  15:56:47  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.30  1997/07/23  23:06:09  lewis
 *	Fixed TextImager::GetStartOfNextRowFromRowContainingPosition.
 *	And docs changes
 *
 *	Revision 2.29  1997/07/15  05:25:15  lewis
 *	AutoDoc content.
 *
 *	Revision 2.28  1997/07/12  20:08:10  lewis
 *	qLedFirstIndex = 0 support
 *	Renamed TextImager_ to TextImager.
 *	AutoDoc support.
 *
 *	Revision 2.27  1997/06/24  03:27:56  lewis
 *	Substantially changed ComputeMaxHScrollPos () (made virutal, and adefault to return zero)
 *	And moved concept of LayoutWidth to WordWrapepdTextImager (spr#0450)
 *
 *	Revision 2.26  1997/06/18  20:04:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.25  1997/06/18  02:44:51  lewis
 *	Lose SetTabStopList. Instead GetTabStopList() is virtual, and takes markerPOS arg.
 *	ResetTabStops and CalcSegmentSize/3 moved to PartitioningTextImager.
 *	Lose qUseAnisotropicModeForWindowsPrinting - assume always true.
 *
 *	Revision 2.24  1997/03/22  13:45:31  lewis
 *	new GetMarkerPositionOfStartOfLastRowOfWindow () to get around ambiguity of a marker position
 *	in a wrapped row which serves as both the start of a row and the end of another. Cannot (as I did before
 *	) just get markerpos of end of window, then say getstartofrowcontainingposition() - cuz could get
 *	start of NEXT row if that line was word-wrapped (was causing bugs with my smart scrollbits code).
 *
 *	Revision 2.23  1997/03/04  20:03:41  lewis
 *	Added EraseBackground ().
 *	FontCacheInfoUpdater::GetMetrics().
 *
 *	Revision 2.22  1997/01/20  05:20:16  lewis
 *	AcquireTable() can now throw exceptions.
 *
 *	Revision 2.21  1997/01/10  02:57:44  lewis
 *	Lots of cleanups. No more qSupportTabStops define - always true.
 *	Added GetStartOfNext/PrevRowFromRowContainingPosition ()
 *	New Get/Set HSCrollPos/ComputeMaxHScrollPos () etc routines for new hscrolling.
 *	Not finished, but close.
 *	Moved ForceAllRowsShowing code here (from multirowtextimager).
 *	CalcSegmentSize () wrapper which doesn't take char* buf as arg.
 *	CalculateLongestRowInWindowPixelWidth () helper for hscrolling.
 *
 *	Revision 2.20  1996/12/13  17:54:11  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.19  1996/12/05  19:08:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.18  1996/10/31  00:08:59  lewis
 *	Add qUseAnisotropicModeForWindowsPrinting for windows printing
 *
 *	Revision 2.17  1996/09/30  14:19:43  lewis
 *	Redid AdjustBounds/InvalidateScrollBarParameters code. Cleaner and simpler
 *	now (and much more efficient - when doing lots of updates before a display - like in reading code).
 *
 *	Revision 2.16  1996/09/01  15:30:29  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.15  1996/05/23  19:27:21  lewis
 *	Get rid of qUsingNewEraserIMCode define - always TRUE.
 *	DrawSegment no longer takes areaLeft - takes optional pxielsDrawn arg.
 *	Led_IncrementalFontSpecification& instead of FontSpecification (big chagne)
 *
 *	Revision 2.14  1996/05/14  20:21:27  lewis
 *	Define qUsingNewEraserIMCode. (tmp hack)
 *	Update DrawSegment_() api to take Tablet.
 *	Added GetRectOfCharRange() utilities.
 *	Cleaned up HookLosingTextStore etc stuff by adding underbar versions.
 *
 *	Revision 2.13  1996/04/18  15:14:55  lewis
 *	Cleanups.
 *	Moved ResetTabStops here from WordWrappedTextImager, cuz now needed here,
 *	and not really specific to word-wrapping (now protected insteaed of private)
 *	PurgeUnneededMemory () virtual.
 *	Completely redid CalcSegmentSize() support. No longer returns height - only
 *	width. And now it isn't propagated through embeddings, and style runs
 *	etc. It is implemented simply using MeasureSegmentWidth(). And then
 *	overrided in a subclas of us (MultiRowTextImager) as a performance tweek
 *	(caching the measured info).
 *
 *	Revision 2.12  1996/03/16  18:31:30  lewis
 *	New - MUCH improved - tabbing support, using new TabStopList class.
 *
 *	Revision 2.11  1996/02/26  18:44:17  lewis
 *	Draw() now takes bool printing parameter.
 *
 *	Revision 2.10  1996/02/05  04:12:50  lewis
 *	Lose unused StandardStyleMarker.
 *	Lose SpecifyTextStore and use HookLosingTextStore/HookGainedNewTextStore instead.
 *
 *	Revision 2.9  1996/01/22  05:10:38  lewis
 *	Temporarily disable qSupportTabStops - til we fix it.
 *	Renamed TabletChanged to TabletChangedMetrics () - and start really
 *	using it now.
 *	Add new pure virtual SetMarkerPositionOfStartOfWindow
 *
 *	Revision 2.8  1995/12/15  02:15:28  lewis
 *	added InvalidRect arg to DrawSegment () to allow for logical clipping.
 *
 *	Revision 2.7  1995/12/09  05:35:02  lewis
 *	Make PeekAtTextStore() out of line since virtual. And add PeekAtTextStore_
 *	replacement as protected inline. Mainly to avoid MSVC 4.0 and earlier
 *	compiler bug with vtables before full object complete.
 *
 *	Revision 2.6  1995/12/06  01:26:40  lewis
 *	PeekAtTextStore () now is an override of MarkerOwner- method.
 *
 *	Revision 2.5  1995/11/04  23:10:05  lewis
 *	Addd new MesaureSegmentBaseLine and measureSegmentHeight no longer
 *	takes text argument and DrawSegemnt now takes baseLine argument
 *
 *	Revision 2.4  1995/11/02  22:20:43  lewis
 *	Added GetStaticDefaultFont() helper function
 *
 *	Revision 2.3  1995/10/19  22:06:35  lewis
 *	Lots of changes to support new styled text runs.
 *	Moved PortableGDI_TabbedStopInterval here.
 *	Moved FontCacheInfoUpdater here.
 *	Moved qTabStop defines here.
 *	Broke DrawSegemnt, and Mesuare etc here into virutal and _helper
 *	routines, so helper could be re-used in styled text imager when it override
 *	default virtuals.
 *
 *	Revision 2.2  1995/10/09  22:21:24  lewis
 *	No more Pre/Post Replace methods
 *
 *	Revision 2.1  1995/09/06  20:52:38  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.12  1995/05/29  23:45:07  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.11  1995/05/25  09:44:43  lewis
 *	Added Tablet_Acquirer utility class.
 *
 *	Revision 1.10  1995/05/20  04:47:56  lewis
 *	Redid support for ImageRect - now gone - use SetLayoutWidth() instead.
 *	BIG change. See SPR 0263.
 *
 *	Revision 1.9  1995/05/18  08:07:37  lewis
 *	Lose old CW5.* bug workarounds (SPR#0249).
 *	Other cleanups.
 *
 *	Revision 1.8  1995/05/16  06:38:56  lewis
 *	Added GetSelectionWindowRegion () and GetFontMetricsAt () pure virtual.
 *
 *	Revision 1.7  1995/05/09  23:21:13  lewis
 *	Lose unused withWhat parameter to Pre/PostReplace() methods.
 *
 *	Revision 1.6  1995/05/08  03:10:16  lewis
 *	AdjustBounds() now inline do nothing rather than pure virtual.
 *	ScrollSoShowing now retyurns nothing. Same for
 *	ScrollByIfRoom. See SPR 0236.
 *
 *	Revision 1.5  1995/05/06  19:34:49  lewis
 *	Use Led_tChar instead of char for UNICODE support - SPR 0232.
 *
 *	Revision 1.4  1995/04/18  00:09:45  lewis
 *	GetStart/EndOfRowContainingPosition - and other cleanups.
 *
 *	Revision 1.3  1995/03/06  21:29:25  lewis
 *	Add eCursorByBuffer to enum    CursorMovementUnit.
 *
 *	Revision 1.2  1995/03/02  05:45:35  lewis
 *	*** empty log message ***
 *
 *	Revision 1.1  1995/03/02  05:30:22  lewis
 *	Initial revision
 *
 *
 *
 */
#include	"BiDiLayoutEngine.h"
#include	"LedGDI.h"
#include	"LedSupport.h"
#include	"Marker.h"
#include	"TextStore.h"







#if		qLedUsesNamespaces
namespace	Led {
#endif




/*
@CLASS:			TextImager
@BASES:			virtual @'MarkerOwner'
@DESCRIPTION:	<p>The @'TextImager' class is one of central importance in Led.
			TextImagers are responsable for imaging to a @'Led_Tablet' the 
			contents of a particular @'TextStore'. A TextImager (except in the degenerate case)
			is always associated with one TextStore (though a single TextStore can have
			multiple TextImagers displaying its contents). This long-term relationship allows
			the TextImagers to keep track of cached data like word-wrapping information etc.
			TextStores have a flexable notification mechanism so that TextImagers get notifed
			when 'interesting' modications are made to the text (or @'Marker's) in the TextStore
			a TextImager is pointing to.</p>

				<p>A particular TextImager subclass takes the given text,
			and figures out how to compute metrics on it, lay it out, cache layout information, and then
			finally display the text. Different subclasses will apply different sorts of rules (and perhaps
			keep track of additional information) to render the text differently. For example - a
			@'WordWrappedTextImager' will apply word-wrapping rules to display the text differently
			than a @'SimpleTextImager' would.</p>

				<p>A @'TextImager' has no notion of 'windows' - in the sense of a UI. It has no
			notion of user interaction, screen
			updates, etc. It has no notion of style runs, paragraph formatting etc.
			It is a generic API for getting text (in a @'TextStore') imaged onto
			a some output @'Led_Tablet'.</p>
		
				  <p>Interesting subclasses of @'TextImager' include @'PartitioningTextImager', @'SimpleTextImager',
			@'StyledTextImager', @'StandardStyledTextImager', and @'TextInteractor'.</p>


			<h5>Window/Scrolling support</h5>
				<p>We support scrolling through the concept of a window. We chose
			very purposefully to define the window in terms of the top row
			of text visible in the window. This was because we wanted to allow
			for the display of word-wrapped text without having to compute the text
			height or word-wrapping of the text above and below the window.</p>

				<p>But note - we associate the topleft of the window with a particular
			ROW - NOT ROW NUMBER! This borrows from (in fact uses) the concept of
			markers. If people insert rows ABOVE the current window, while this
			affects the ROW NUMBER of the top row shown in the window - it doesn't
			affect what is displayed.</p>

				<p>Also, note that we deal with HORIZONTAL and VERTICAL scrolling through
			very different means. This is because in the case of horizontal scrolling, it is
			basicly just scrolling over a few - perhaps hundreds of pixels). And scrolling
			in terms of pixels is clear, unambiguous, and there is really no other obvious choice
			(characters don't work well cuz text may be proportional, or even multifont).</p>

				<p>And vertical scrolling, pixels - though workable - is a undesirable choice
			for performance reasons (as alluded to above).</p>


			<h5>Future enhancments:</h5>
			<ul>
				<li>	Instead of a single ImageRect, we could have an array of image rects.
				All would have real top/left/bottom/right, except the last which would
				be interpretted as being bottomless (or at least WE define the button).
				This would allow trivial implementation of multicolumn word processors,
				and much more sophisticated page / text layout.
				</li>

				<li>Be sure we deal sensibly with exceptions (on insert/delete mainly - leave
				markers in a good state - maybe preflight).
				</li>
			</ul>
*/
class	TextImager : public virtual MarkerOwner {
	protected:
		TextImager ();
	public:
		virtual ~TextImager ();


	/*
	 *	Note we can associate any textstore we want with this imager. One one
	 *	can be associated at a time. And it is an error to call most TextImager
	 *	methods (any that call GetTextStore()) while the textstore is set to
	 *	NULL. Always reset the textstore to NULL before destroying the textstore
	 *	pointed to. And note that switching textstores loses any information
	 *	that might have been saved there, like current selection etc.
	 */
	public:
		override	TextStore*	PeekAtTextStore () const;	// from MarkerOwner
	public:
		nonvirtual	void	SpecifyTextStore (TextStore* useTextStore);
	protected:
		virtual		void	HookLosingTextStore ();
		nonvirtual	void	HookLosingTextStore_ ();
		virtual		void	HookGainedNewTextStore ();
		nonvirtual	void	HookGainedNewTextStore_ ();

	private:
		TextStore*	fTextStore;



	public:
		virtual	void	PurgeUnneededMemory ();

	protected:
		virtual	void	InvalidateAllCaches ();

	/*
	 *	Led's model of font application is slightly different than most
	 *	other editors. Led defines a DefaultFont to be applied to all
	 *	text. Then, this font can be overriden by the presence of
	 *	special markers in the text. This overriding is done
	 *	in subclasses of TextImager. So some textImagers will
	 *	only support a single monolithic font for the entire text buffer
	 *	and others will support refinements of the font specification
	 *	on a character by character basis (using markers).
	 *
	 *	DefaultFont defaults to GetStaticDefaultFont () (really - no puns intended :-))
	 */
	public:
		nonvirtual	Led_FontSpecification	GetDefaultFont () const;
		virtual		void					SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont);
	protected:
		nonvirtual	void					SetDefaultFont_ (const Led_IncrementalFontSpecification& defaultFont);
	private:
		Led_FontSpecification				fDefaultFont;

	// Query the OS for the default font that should be used for new text windows
	public:
		static	Led_FontSpecification	GetStaticDefaultFont ();
		#if		qWindows
		static	Led_FontSpecification	GetStaticDefaultFont (BYTE charSet);
		#endif

	public:
		virtual	Led_FontSpecification	GetDefaultSelectionFont () const;


	public:
		/*
		@CLASS:			TextImager::NotFullyInitialized
		@DESCRIPTION:	<p>Led requires a complex of objects to all be setup, and hooked together in order to
			perform several operations correctly. E.G. to invalidate part of the screen, it must indirectly
			call on code to check the @'TextStore', or the @'Partition'.</p>
				<p>As part of code re-use, during the setup process, and part of the mechanism of providing default
			objects for many of these peices, we call SetThisOrThat() methods which invoke various InvalidateThisOrThat()
			methods, which in turn may occasionally try to compute something (like the max# of rows in a line). These
			sorts of errors are innocuous, but must be handled somehow. We could either write the code to be friendly of
			such uninitiualized variables/values, or to throw like this. My inclination is that throwing will be a more
			robust solution, since it will prevent an further use of a guessed value (and maybe prevent its being cached).</p>
		*/
		class	NotFullyInitialized {};

	protected:
		/*
		@CLASS:			TextImager::NoTabletAvailable
		@BASES:			@'TextImager::NotFullyInitialized'
		@DESCRIPTION:	<p>Thrown by @'TextImager::AcquireTablet' when no tablet available. See that method docs
			for more details.</p>
		*/
		class	NoTabletAvailable : public NotFullyInitialized {};

	protected:
		#if		qAccessCheckAcrossInstancesSometimesWrongWithVirtualBase
			public:
		#endif
		/*
		@METHOD:		TextImager::AcquireTablet
		@DESCRIPTION:
			<p>By "Tablet" I mean on the mac a grafport, or on windows a CDC (@'Led_Tablet'). Something
		we can draw into, and calculate text metrics with.</p>
			<p>Sometimes Led is lucky enough to find itself in a position where it is
		handed a tabet (drawing). But sometimes it isn't so luck. Conisider if someone
		asks for a row number, and this causes a complex chain of events resulting
		in an attempt to word-wrap, which, in turn, requires a tablet. We need some API
		in the Imager to request a tablet. This API MUST return the appropriate tablet
		for the view, appropriately prepared for drawing/text metric calculations.</p>
			<p>Calls to @'TextImager::AcquireTablet' must ALWAYS be balanced with calls to @'TextImager::ReleaseTablet ()'
		(including in the prescence of exceptions).</p>
			<p>For windows, @'TextImager::AcquireTablet ()' must return the current PaintDC used in the PaintDC
		call (if we are in the context of a paint).</p>
			<p>For Mac, we must not only return the grafPort, but be sure it is prepared.</p>
			<p>@'TextImager::AcquireTablet ()' only fail by throwing - typically NoTabletAvailable (Led 2.1 and earlier
		didn't allow failure).</p>
			<p>Calls to @'TextImager::AcquireTablet ()'/@'TextImager::ReleaseTablet ()' can be nested.</p>
			<p>Also, sometimes something happens which causes all text metrics to become</p>
		invalid (say we are about to image to a print port). In this case, the imager
		needs to be notified that all its text metrics are invalid. Call @'TextImager::TabletChangedMetrics ()'
		in this case.</p>
		*/
		virtual	Led_Tablet	AcquireTablet () const 												=	0;
		/*
		@METHOD:		TextImager::ReleaseTablet
		@DESCRIPTION:	<p>Tablet API</p>
				<p>See @'TextImager::AcquireTablet'. Generally don't call this directly - but instead use the
			@'TextImager::Tablet_Acquirer' class.</p>
		*/
		virtual	void		ReleaseTablet (Led_Tablet tablet) const								=	0;
	protected:
		virtual	void		TabletChangedMetrics ();


	public:
		class	Tablet_Acquirer;
		friend	class	Tablet_Acquirer;


		public:
			//class	TabStopList;
			/*
			@CLASS:			TextImager::TabStopList
			@DESCRIPTION:	<p>See @'Led_TabStopList'</p>
			*/
			typedef	Led_TabStopList	TabStopList;
			class	SimpleTabStopList;
			class	StandardTabStopList;
		public:
			virtual		const TabStopList&	GetTabStopList (size_t containingPos) const;



	/*
	 *	Window/Scrolling support.
	 */
	public:
		nonvirtual	Led_Rect	GetWindowRect () const;
		virtual		void		SetWindowRect (const Led_Rect& windowRect);
	protected:
		nonvirtual	void		SetWindowRect_ (const Led_Rect& windowRect);
	private:
		Led_Rect	fWindowRect;



	// Consider re-doing many of the below ROW APIs using this struct??? - LGP 2002-12-02
	// Then we'd ahve void  GetRowInfo (TransRowSpec tsr, size_t* rowStart, size_t* rowEnd) const = 0 declared here... and use that to get row info?
	public:
		struct	TransientRowSpecification {
			size_t	fRowStartMarkerPos;
		};


	/*
	 *	Vertical Scrolling:
	 */
	public:
		/*
		@METHOD:		TextImager::GetTopRowInWindow
		@DESCRIPTION:	<p>Returns the row number of the top row visible in the scrolled display window.
					This is a generally ineffient routine to call. It is generally much better
					to call @'TextImager::GetMarkerPositionOfStartOfWindow'</p>
		*/
		virtual		size_t		GetTopRowInWindow () const												=	0;
		/*
		@METHOD:		TextImager::GetTotalRowsInWindow
		@DESCRIPTION:	<p></p>
		*/
		virtual		size_t		GetTotalRowsInWindow () const											=	0;
		/*
		@METHOD:		TextImager::GetLastRowInWindow
		@DESCRIPTION:	<p>Returns the row number of the bottom row visible in the scrolled display window.
					This is a generally ineffient routine to call. It is generally much better
					to call @'TextImager::GetMarkerPositionOfEndOfWindow'</p>
		*/
		virtual		size_t		GetLastRowInWindow () const												=	0;
		/*
		@METHOD:		TextImager::SetTopRowInWindow
		@DESCRIPTION:	<p></p>
		*/
		virtual		void		SetTopRowInWindow (size_t newTopRow)									=	0;
		/*
		@METHOD:		TextImager::GetMarkerPositionOfStartOfWindow
		@DESCRIPTION:	<p>This function returns the marker position of the start of the display window. The
					display window here refers to the area which is currently visible though scrolling.</p>
					<p>This doc should be clarified!</p>
					<p>See also @'TextImager::GetMarkerPositionOfStartOfWindow'</p>
		*/
		virtual		size_t		GetMarkerPositionOfStartOfWindow () const								=	0;
		/*
		@METHOD:		TextImager::GetMarkerPositionOfEndOfWindow
		@DESCRIPTION:	<p>This function returns the marker position of the end of the display window. The
					display window here refers to the area which is currently visible though scrolling.</p>
					<p>This doc should be clarified!</p>
					<p>See also @'TextImager::GetMarkerPositionOfStartOfWindow'</p>
		*/
		virtual		size_t		GetMarkerPositionOfEndOfWindow () const									=	0;
		/*
		@METHOD:		TextImager::GetMarkerPositionOfStartOfLastRowOfWindow
		@DESCRIPTION:	<p></p>
		*/
		virtual		size_t		GetMarkerPositionOfStartOfLastRowOfWindow () const						=	0;

	// Some helpful routines for the case where we don't have RowReferences available, but we don't want to be too slow...
	public:
		/*
		@METHOD:		TextImager::CalculateRowDeltaFromCharDeltaFromTopOfWindow
		@DESCRIPTION:	<p></p>
		*/
		virtual		long		CalculateRowDeltaFromCharDeltaFromTopOfWindow (long deltaChars) const	=	0;
		/*
		@METHOD:		TextImager::CalculateCharDeltaFromRowDeltaFromTopOfWindow
		@DESCRIPTION:	<p></p>
		*/
		virtual		long		CalculateCharDeltaFromRowDeltaFromTopOfWindow (long deltaRows) const	=	0;


	// NB:	Though you CAN ask for, or set the top row by number - this is VASTLY less efficient than
	// doing it by relative position from the current window via ScrollByIfRoom ().
	// This is because the former method requires computing the text metrics of all the text (and wrapping)
	// above the row scrolled to (at), while the later - relative method - does not!!!
	public:
		/*
		@METHOD:		TextImager::ScrollByIfRoom
		@DESCRIPTION:	<p>If downBy negative then up
			OK to ask to scroll further than allowed - return true
			if any scrolling (not necesarily same amont requested) done</p>
		*/
		virtual		void		ScrollByIfRoom (long downByRows)										=	0;

		/*
		@METHOD:		TextImager::ScrollSoShowing
		@DESCRIPTION:	<p>If andTryToShowMarkerPos == 0, then just try to show the characters
			after markerPos. If it is NOT zero, then try to show the characters
			BETWEEN the two marker positions (note they might not be in ascending
			order). The (argument) order of the two marker positions determines PREFERENCE.
			That is - if there is no room to show both, be sure the first is showing.</p>
		*/
		virtual		void		ScrollSoShowing (size_t markerPos, size_t andTryToShowMarkerPos = 0)	=	0;


	protected:
		nonvirtual	void		ScrollSoShowingHHelper (size_t markerPos, size_t andTryToShowMarkerPos);


	/*
	 *	This attribute controls if we force the scroll position to be adjusted
	 *	in InvalidateScrollBar() so that the entire window is being used. This is TRUE
	 *	by default. But might sometimes be handy to disable this behavior.
	 */
	public:
		nonvirtual	bool	GetForceAllRowsShowing () const;
		nonvirtual	void	SetForceAllRowsShowing (bool forceAllRowsShowing);
	private:
		bool	fForceAllRowsShowing;
	protected:
		/*
		@METHOD:		TextImager::AssureWholeWindowUsedIfNeeded
		@ACCESS:		protected
		@DESCRIPTION:	<p></p>
		*/
		virtual	void		AssureWholeWindowUsedIfNeeded ()											=	0;


	public:
		nonvirtual	bool	GetImageUsingOffscreenBitmaps () const;
		nonvirtual	void	SetImageUsingOffscreenBitmaps (bool imageUsingOffscreenBitmaps);
	private:
		bool	fImageUsingOffscreenBitmaps;


	/*
	 *	Horizontal Scrolling:
	 *
	 *		The way horizontal scrolling works is that you specify a (horizontal) offset as to where
	 *	drawing into the WindowRect() will be relative to. Typically, this will be ZERO, indicating
	 *	the text is unscrolled. Typically (by default) the ComputeMaxHScrollPos () method will return
	 *	zero. At any time, a UI can call ComputeMaxHScrollPos () in order to inform adjustments to
	 *	a scrollbar. It is the TextImagers responsablity to assure that ComputeMaxHScrollPos () returns
	 *	a correct, but efficiently computed (ie cached) value, where possible.
	 *
	 *
	 *	Design Meanderings: - PRELIMINARY DESIGN THOUGHTS ON HOW TODO THIS - LGP 961231
	 *		(what is connection between ComputeMaxHScrollPos () and GetLayoutWidth()??? Logicly there must
	 *	be one. Really we must keep resetting Layout Width as people type text making a line longer. Then
	 *	ComputeMaxHScrollPos () really just returns GetLayoutWidth() - WindowRect.GetWidth () (or zero, whichever
	 *	is larger).
	 *
	 *	Then calls to SetHScrollPos & SetLayoutWidth() must clip (auto-reset if necessary) HScrollPos.
	 *
	 */
	public:
		nonvirtual	Led_Coordinate	GetHScrollPos () const;
		virtual		void			SetHScrollPos (Led_Coordinate hScrollPos);
	protected:
		nonvirtual	void			SetHScrollPos_ (Led_Coordinate hScrollPos);
	public:
		virtual	Led_Distance		ComputeMaxHScrollPos () const;
	private:
		Led_Coordinate	fHScrollPos;


	/*
	 *	Some utility methods, very handy for implementing horizontal scrolling. Can (and should be) overriden
	 *	in certain subclasses for efficiency. But the default implementation will work.
	 */
	public:
		virtual	Led_Distance	CalculateLongestRowInWindowPixelWidth () const;

#if		qSupportLed30CompatAPI
	/*
	 *	Always/only called at times when it is safe to call other editor API methods.
	 *	It is expected these will be PRIMARY hooks for subclassers (simple users).
	 */
	protected:
		virtual		void		NotificationOf_SelectionChanged ();
#endif

	public:
		enum	CursorMovementDirection	{ eCursorBack, eCursorForward, eCursorToStart, eCursorToEnd };
		enum	CursorMovementUnit		{ eCursorByChar, eCursorByWord, eCursorByRow, eCursorByLine, eCursorByWindow, eCursorByBuffer };
		virtual		size_t	ComputeRelativePosition (size_t fromPos, CursorMovementDirection direction, CursorMovementUnit movementUnit);


	public:
		class	GoalColumnRecomputerControlContext;
		nonvirtual	void		RecomputeSelectionGoalColumn ();
	private:
		bool	fSupressGoalColumnRecompute;

	public:
		nonvirtual	Led_TWIPS	GetSelectionGoalColumn () const;
		nonvirtual	void		SetSelectionGoalColumn (Led_TWIPS selectionGoalColumn);
	private:
		Led_TWIPS	fSelectionGoalColumn;

	public:
		/*
		@METHOD:		TextImager::GetStableTypingRegionContaingMarkerRange
		@DESCRIPTION:	<p>OK, this is a little obscure. But - believe me - it does make sense.
					When doing updates to the text (say typing) the impact if the typing
	 				(in terms of visual display) typically only extends over a narrow region
	 				of the screen. If we are using a non-word-wrapped text editor, then this
	 				is typically a line. If it is a word-wrapped text editor, then it is
	 				typically a paragraph (or perhaps something less than that). This routine
	 				allows communication from the imager to higher-level interactor
	 				software about what areas of the text will need to be redrawn when
	 				changes to localized regions of the text occur.</p>
		*/
		virtual	void	GetStableTypingRegionContaingMarkerRange (size_t fromMarkerPos, size_t toMarkerPos,
							size_t* expandedFromMarkerPos, size_t * expandedToMarkerPos) const				=	0;


	public:
	#if		qSupportLed30CompatAPI
		nonvirtual	Led_Rect	GetTextBoundingRect (size_t fromMarkerPos, size_t toMarkerPos) const;
	#endif
		nonvirtual	Led_Rect	GetTextWindowBoundingRect (size_t fromMarkerPos, size_t toMarkerPos) const;

	public:
		nonvirtual	Led_Rect	GetIntraRowTextWindowBoundingRect (size_t fromMarkerPos, size_t toMarkerPos) const;

	public:
		virtual	vector<Led_Rect>	GetRowHilightRects (const TextLayoutBlock& text, size_t rowStart, size_t rowEnd, size_t hilightStart, size_t hilightEnd) const;


	/*
	 *		Utility routine to compute based on styleruns etc, all the fontinfo we can get
	 *	at a particular point in the text. This can be helpful drawing additional adornments
	 *	on the text in subclases.
	 */
	public:
		virtual		Led_FontMetrics	GetFontMetricsAt (size_t charAfterPos) const;


	// TextImagers don't really have a notion of a scrollbar. And yet for many different
	// subclasses of TextImager, certain things might be detected that would force
	// a subclass which DID have a scrollbar to update it.
	//	These things include:
	//		¥	Any edit operation (including font change)
	//			which increases/decreases number of rows
	//		¥	Any scroll operation
	//		¥	Resizing the LayoutWidth.
	//
	// Call InvalidateScrollBarParameters () whenever any such change happens.
	protected:
		virtual		void	InvalidateScrollBarParameters ();


	public:
		nonvirtual	bool	GetUseSelectEOLBOLRowHilightStyle () const;
		nonvirtual	void	SetUseSelectEOLBOLRowHilightStyle (bool useEOLBOLRowHilightStyle);
	private:
		bool	fUseEOLBOLRowHilightStyle;

	/*
	 *	Control if the caret is currently being displayed for this TE field. Typically it will
	 *	be turned on when this widget gets the focus, and off when it loses it. But that is
	 *	the responsability of higher level software (subclasses).
	 */
	public:
		nonvirtual	bool	GetSelectionShown () const;
		virtual		void	SetSelectionShown (bool shown);
	private:
		bool	fSelectionShown;


	public:
		nonvirtual	size_t	GetSelectionStart () const;
		nonvirtual	size_t	GetSelectionEnd () const;
		nonvirtual	void	GetSelection (size_t* start, size_t* end) const;
		virtual		void	SetSelection (size_t start, size_t end);
	protected:
		nonvirtual	void	SetSelection_ (size_t start, size_t end);
	protected:
		class	HilightMarker : public Marker {
			public:
				HilightMarker ();
		};
		HilightMarker*	fHiliteMarker;
		bool			fWeAllocedHiliteMarker;
	protected:
		nonvirtual	void	SetHilightMarker (HilightMarker* newHilightMarker);	// NB: Caller must free, and call SetHilightMarker (NULL)


	/*
	 *	API to get postions of text.
	 */
	public:
		/*
		@METHOD:		TextImager::GetCharLocation
		@DESCRIPTION:	<p>Returns the (document relative) pixel location (rectangle) of the character after the given
					marker position.</p>
						<p>GetCharLocation () returns the rectangle bounding the character after marker-position
					'afterPosition'. The position MUST be a valid one (0..GetEnd ()), but could
					refer to the end of the buffer - and so there would be no character width in this case.</p>
						<p>Note that this is also somewhat ineffecient for larger documents. You can often use
					@'TextImager::GetCharLocationRowRelative' or @'TextImager::GetCharWindowLocation' instead.</p>
		*/
		virtual		Led_Rect		GetCharLocation (size_t afterPosition)	const				=	0;
		/*
		@METHOD:		TextImager::GetCharAtLocation
		@DESCRIPTION:	<p>Always returns a valid location, though it might not make a lot of
					since if it is outside the ImageRect.</p>
						<p>Note that this is also somewhat ineffecient for larger documents. You can often use
					@'TextImager::GetRowRelativeCharAtLoc' or @'TextImager::GetCharAtWindowLocation' instead.</p>
		*/
		virtual		size_t			GetCharAtLocation (const Led_Point& where) const			=	0;
		/*
		@METHOD:		TextImager::GetCharWindowLocation
		@DESCRIPTION:	<p>Like @'TextImager::GetCharLocation' but returns things relative to the start of the window.</p>
						<p><em>NB:</em> if the location is outside of the current window, then the size/origin maybe pinned to some
					arbitrary value, which is pixelwise above (or beleow according to before or after window start/end) the window.</p>
		*/
		virtual		Led_Rect		GetCharWindowLocation (size_t afterPosition)	const		=	0;
		/*
		@METHOD:		TextImager::GetCharAtWindowLocation
		@DESCRIPTION:	<p>Like @'TextImager::GetCharAtLocation' but assumes 'where' is presented window-relative.</p>
						<p>See also @'TextInteractor::GetCharAtClickLocation'</p>
		*/
		virtual		size_t			GetCharAtWindowLocation (const Led_Point& where) const		=	0;

		/*
		@METHOD:		TextImager::GetStartOfRow
		@DESCRIPTION:	<p>GetStartOfRow () returns the position BEFORE the first displayed character in the row.</p>
		*/
		virtual		size_t			GetStartOfRow (size_t rowNumber) const						=	0;
		/*
		@METHOD:		TextImager::GetStartOfRowContainingPosition
		@DESCRIPTION:	<p></p>
		*/
		virtual		size_t			GetStartOfRowContainingPosition (size_t charPosition) const	=	0;
		/*
		@METHOD:		TextImager::GetEndOfRow
		@DESCRIPTION:	<p>GetEndOfRow () returns the position AFTER the last displayed character in the row.
			Note that this can differ from the end of row returned by @'TextImager::GetRealEndOfRow' because of characters removed by
			@TextImager::RemoveMappedDisplayCharacters'.</p>
				<p>See also @'TextImager::GetRealEndOfRow'.</p>
		*/
		virtual		size_t			GetEndOfRow (size_t rowNumber) const						=	0;
		/*
		@METHOD:		TextImager::GetEndOfRowContainingPosition
		@DESCRIPTION:	<p></p>
		*/
		virtual		size_t			GetEndOfRowContainingPosition (size_t charPosition) const	=	0;
		/*
		@METHOD:		TextImager::GetRealEndOfRow
		@DESCRIPTION:	<p>GetRealEndOfRow () returns the position AFTER the last character in the row. See
			@'TextImager::GetEndOfRow'. Note - for the last row of the document, this could be include the
			'bogus character'. So - this will always return a value &lt;= @'TextStore::GetLength' () + 1</p>
		*/
		virtual		size_t			GetRealEndOfRow (size_t rowNumber) const						=	0;
		/*
		@METHOD:		TextImager::GetRealEndOfRowContainingPosition
		@DESCRIPTION:	<p></p>
		*/
		virtual		size_t			GetRealEndOfRowContainingPosition (size_t charPosition) const	=	0;
		/*
		@METHOD:		TextImager::GetRowContainingPosition
		@DESCRIPTION:	<p></p>
		*/
		virtual		size_t			GetRowContainingPosition (size_t charPosition) const		=	0;
		/*
		@METHOD:		TextImager::GetRowCount
		@DESCRIPTION:	<p></p>
		*/
		virtual		size_t			GetRowCount () const										=	0;

		nonvirtual	size_t			GetRowLength (size_t rowNumber) const;


	public:
		/*
		@METHOD:		TextImager::GetCharLocationRowRelativeByPosition
		@DESCRIPTION:	<p></p>
		*/
		virtual		Led_Rect		GetCharLocationRowRelativeByPosition (size_t afterPosition, size_t positionOfTopRow, size_t maxRowsToCheck)	const	=	0;

	// Simple wrappers on GetStartOfRowContainingPosition () etc to allow row navigation without the COSTLY
	// computation of row#s.
	public:
		nonvirtual	size_t			GetStartOfNextRowFromRowContainingPosition (size_t charPosition) const;
		nonvirtual	size_t			GetStartOfPrevRowFromRowContainingPosition (size_t charPosition) const;


	public:
		/*
		@METHOD:		TextImager::GetRowHeight
		@DESCRIPTION:	<p>This API is redundent, but can be much more efficient to get at this information
					than GetCharLocation() - especially in subclasses like MultiRowImager using RowRefernces.</p>
		*/
		virtual		Led_Distance	GetRowHeight (size_t rowNumber)	const						=	0;



	public:
		/*
		@METHOD:		TextImager::GetRowRelativeBaselineOfRowContainingPosition
		@DESCRIPTION:	<p>Returns the number of pixels from the top of the given row, to the baseline.
			The 'charPosition' is a markerPosition just before any character in the row.</p>
		*/
		virtual		Led_Distance	GetRowRelativeBaselineOfRowContainingPosition (size_t charPosition)	const						=	0;


	public:
		/*
		@METHOD:		TextImager::GetTextDirection
		@DESCRIPTION:	<p>Returns the text direction (left to right or right to left) of the given character
					(the one just after the given marker position).</p>
		*/
		virtual		TextDirection	GetTextDirection (size_t charPosition)	const						=	0;



	public:
		virtual		TextLayoutBlock_Copy	GetTextLayoutBlock (size_t rowStart, size_t rowEnd)	const;



	/*
	 *	Figure the region bounding the given segment of text. Useful for displaying
	 *	some sort of text hilight, in addition (or apart from) the standard hilighting
	 *	of text. Note we use a VAR parameter for the region rather than returing it
	 *	cuz MFC's CRgn class doesn't support being copied.
	 */
	public:
		virtual		vector<Led_Rect>	GetSelectionWindowRects (size_t from, size_t to) const;
		virtual		void				GetSelectionWindowRegion (Led_Region* r, size_t from, size_t to) const;



	public:
		/*
		@METHOD:		TextImager::Draw
		@DESCRIPTION:	<p>This is the API called by the underlying window system (or wrappers) to get the imager
					to draw its currently displayed window. The argument 'subsetToDraw' defines the subset (in the same coordinate system
					the windowRect (@'TextImager::GetWindowRect') was specified in (so it will generally be a subset of the windowrect).</p>
		*/
		virtual		void	Draw (const Led_Rect& subsetToDraw, bool printing)					=	0;

	// Misc default foreground/background color attributes
	public:
		/*
		@METHOD:		TextImager::DefaultColorIndex
		@DESCRIPTION:	<p>Enumerator of different color-table default names used by these routines:
							<ul>
								<li>@'TextImager::GetEffectiveDefaultTextColor'</li>
								<li>@'TextImager::GetEffectiveDefaultTextColor'</li>
								<li>@'TextImager::ClearDefaultTextColor'</li>
								<li>@'TextImager::SetDefaultTextColor'</li>
							</ul>
						</p>
						<p>The values are:
							<table>
								<tr>
									<td>Name</td>								<td>Default</td>									<td>Description</td>
								</tr>
								<tr>
									<td>eDefaultTextColor</td>					<td>Led_GetTextColor&nbsp;()</td>					<td>Color used for plain text. This color choice is frequently overriden elsewhere.</td>
								</tr>
								<tr>
									<td>eDefaultBackgroundColor</td>			<td>Led_GetTextBackgroundColor&nbsp;()</td>			<td>The background color which the text is drawn against. This is mainly used in @'TextImager::EraseBackground'. NB: since TextImager::EraseBackground is frequently overriden - setting this value could have no effect.</td>
								</tr>
								<tr>
									<td>eDefaultSelectedTextColor</td>			<td>Led_GetSelectedTextColor&nbsp;()</td>			<td>The color used to display selected text. This would typically by used by @'TextImager::HilightARectangle' - but depending on the style of hilight it uses - this color maybe ignonred.</td>
								</tr>
								<tr>
									<td>eDefaultSelectedTextBackgroundColor</td><td>Led_GetSelectedTextBackgroundColor&nbsp;()</td>	<td>The background color for hilighted (selected) text. Depending on the style of hilight it uses - this color maybe ignonred.</td>
								</tr>
							</table>
						</p>
		*/
		enum DefaultColorIndex { eDefaultTextColor, eDefaultBackgroundColor, eDefaultSelectedTextColor, eDefaultSelectedTextBackgroundColor, eMaxDefaultColorIndex };
		nonvirtual	Led_Color*	GetDefaultTextColor (DefaultColorIndex dci) const;
		nonvirtual	Led_Color	GetEffectiveDefaultTextColor (DefaultColorIndex dci) const;
		nonvirtual	void		ClearDefaultTextColor (DefaultColorIndex dci);
		nonvirtual	void		SetDefaultTextColor (DefaultColorIndex dci, const Led_Color& textColor);
	private:
		Led_Color*	fDefaultColorIndex[eMaxDefaultColorIndex];

	public:
		virtual		void	EraseBackground (Led_Tablet tablet, const Led_Rect& subsetToDraw, bool printing);

	public:
#if		qSupportLed30CompatAPI
		virtual		void	HilightARectangle (Led_Tablet tablet, Led_Rect hiliteRect);
#endif
		virtual		void	HilightArea (Led_Tablet tablet, Led_Rect hiliteArea);
		virtual		void	HilightArea (Led_Tablet tablet, const Led_Region& hiliteArea);


	protected:
		virtual		void		DrawRow (Led_Tablet tablet, const Led_Rect& currentRowRect, const Led_Rect& invalidRowRect,
									const TextLayoutBlock& text, size_t rowStart, size_t rowEnd, bool printing
								);
		virtual		void		DrawRowSegments (Led_Tablet tablet, const Led_Rect& currentRowRect, const Led_Rect& invalidRowRect,
									const TextLayoutBlock& text, size_t rowStart, size_t rowEnd
								);
		virtual		void		DrawRowHilight (Led_Tablet tablet, const Led_Rect& currentRowRect, const Led_Rect& invalidRowRect,
									const TextLayoutBlock& text, size_t rowStart, size_t rowEnd
								);
		virtual		void		DrawInterLineSpace (Led_Distance interlineSpace, Led_Tablet tablet, Led_Coordinate vPosOfTopOfInterlineSpace, bool segmentHilighted, bool printing);

	protected:
		virtual	bool	ContainsMappedDisplayCharacters (const Led_tChar* text, size_t nTChars) const;
		virtual	void	ReplaceMappedDisplayCharacters (const Led_tChar* srcText, Led_tChar* copyText, size_t nTChars) const;
		virtual	size_t	RemoveMappedDisplayCharacters (Led_tChar* copyText, size_t nTChars) const;
		virtual	void	PatchWidthRemoveMappedDisplayCharacters (const Led_tChar* srcText, Led_Distance* distanceResults, size_t nTChars) const;

	protected:
		static	bool	ContainsMappedDisplayCharacters_HelperForChar (const Led_tChar* text, size_t nTChars, Led_tChar charToMap);
		static	void	ReplaceMappedDisplayCharacters_HelperForChar (Led_tChar* copyText, size_t nTChars, Led_tChar charToMap, Led_tChar charToMapTo);
		static	size_t	RemoveMappedDisplayCharacters_HelperForChar (Led_tChar* copyText, size_t nTChars, Led_tChar charToRemove);
		static	void	PatchWidthRemoveMappedDisplayCharacters_HelperForChar (const Led_tChar* srcText, Led_Distance* distanceResults, size_t nTChars, Led_tChar charToRemove);


	protected:
		virtual		void		DrawSegment (Led_Tablet tablet,
										size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& invalidRect,
										Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn
									);
	public:
		// Note we REQUIRE that useBaseLine be contained within drawInto
		nonvirtual	void		DrawSegment_ (Led_Tablet tablet, const Led_FontSpecification& fontSpec,
										size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn
									) const;

	protected:
		// distanceResults must be an array of (to-from) elements - which is filled in with the widths
		// of subsegments from 'from' up to 'i', where 'i' is the ith-from element of 'distanceResults'.
		// For mbyte chars - splitting char results and looking at those values are UNDEFINED).
		//
		// In general - textwidth of text from from to b (where b MUST be contained in from/to)
		// is distanceResults[b-from-1] - and of course ZERO if b == from
		//
		virtual		void			MeasureSegmentWidth (size_t from, size_t to, const Led_tChar* text,
														Led_Distance* distanceResults
													) const;
	public:
		nonvirtual		void		MeasureSegmentWidth_ (const Led_FontSpecification& fontSpec, size_t from, size_t to,
														const Led_tChar* text,
														Led_Distance* distanceResults
													) const;

	protected:
		virtual		Led_Distance	MeasureSegmentHeight (size_t from, size_t to) const;
	public:
		nonvirtual	Led_Distance	MeasureSegmentHeight_ (const Led_FontSpecification& fontSpec, size_t from, size_t to) const;


	protected:
		virtual		Led_Distance	MeasureSegmentBaseLine (size_t from, size_t to) const;
	public:
		nonvirtual	Led_Distance	MeasureSegmentBaseLine_ (const Led_FontSpecification& fontSpec, size_t from, size_t to) const;


	public:
		/*
		@METHOD:		TextImager::CalcSegmentSize
		@DESCRIPTION:	<p>Measures the distance in pixels from 'from' to 'to'.
					This function requires that 'from' and 'to' are on the same row.</p>
						<p>Note that for bidirectional text, the notion of 'from' and 'to' CAN be ambiguous. A given
					marker-position between two characters in the buffer could refer to characters that are VISUALLY
					not next to one another. Luckily - there is a convenient disambiguator in this API. Since we are always
					measuring 'from' one position 'to' another - we assume the region in question is the one <em>after</em>
					the 'from' and <em>before</em> the 'to'.</p>
						<p>Note - because of this definition - and because of bidirectional characters - trying to find
					the left and right side of a character with:
							<CODE><PRE>
							LHS = CalcSegmentSize (ROWSTART, i)
							RHS = CalcSegmentSize (ROWSTART, i+1);
							</PRE></CODE>
						will NOT work. Use @'TextImager::GetRowRelativeCharLoc' to properly take into account the script
					directionality runs.
						</P>
		*/
		virtual	Led_Distance	CalcSegmentSize (size_t from, size_t to) const			=	0;



	public:
		/*
		@METHOD:		TextImager::GetRowRelativeCharLoc
		@DESCRIPTION:	<p>Returns the row-relative bounds of the given character (the one from charLoc to charLoc + 1).
					The left hand side is window-relative, so if you want window coordinates, add the window left. 
					This routine is more complex than just calling @'TextImager::CalcSegmentSize' () on the right and
					left sides of the character, because of bidirectional editing.</p>
						<p>Note that even though the character maybe RTL or LTR, we still return the *lhs <= *rhs for the
					character cell. This routine calls @'TextImager::RemoveMappedDisplayCharacters' and if the given character is
					removed, its proper LHS will still be returned, but the RHS will be EQUAL to its LHS.</p>
						<p>Note that 'charLoc' must be a legal char location or just at the end of the buffer - 0 <= charLoc <= GetEnd ().
					if charLoc==GetEnd() - you will get the same results as in the @'TextImager::RemoveMappedDisplayCharacters' case.</p>
						<p>See also @'TextImager::GetRowRelativeCharLoc'</p>
		*/
		virtual	void	GetRowRelativeCharLoc (size_t charLoc, Led_Distance* lhs, Led_Distance* rhs) const			=	0;




	public:
		/*
		@METHOD:		TextImager::GetRowRelativeCharAtLoc
		@DESCRIPTION:	<p>Look in the row which begins at 'rowStart'. Look 'hOffset' pixels into the row, and return the character which
					would be found at that point (assuming the vertical positioning is already correct). If its too far to the right, return ENDOFROW.</p>
						<p>See also @'TextImager::GetRowRelativeCharLoc'</p>
						<p>Returns the last character in the row (not a marker PAST that
					character) if the point passed in is past the end of the last character in the row.
						</p>
		 */
		virtual	size_t	GetRowRelativeCharAtLoc (Led_Coordinate hOffset, size_t rowStart) const			=	0;


	// Font info caches...
	private:
		mutable	Led_FontSpecification	fCachedFontSpec;
		mutable	Led_FontMetrics			fCachedFontInfo;
		#if		qWindows
			mutable	Led_FontObject*		fCachedFont;
		#else
			mutable	bool				fCachedFontValid;
		#endif

	protected:
		class	FontCacheInfoUpdater {
			public:
				FontCacheInfoUpdater (const TextImager* imager, Led_Tablet tablet, const Led_FontSpecification& fontSpec);
				~FontCacheInfoUpdater ();

			public:
				nonvirtual	Led_FontMetrics		GetMetrics () const;

			private:
				const TextImager*	fImager;

			#if		qWindows
			private:
				Led_Tablet	fTablet;
				HGDIOBJ 	fRestoreObject;
				HGDIOBJ 	fRestoreAttribObject;
			#endif
		};
		friend	class	FontCacheInfoUpdater;

	private:
		friend	class	GoalColumnRecomputerControlContext;
};




/*
@CLASS:			TextImager::Tablet_Acquirer
@DESCRIPTION:	<p>Stack-based resource allocation/deallocation. See @'TextImager::AcquireTablet'.</p>
*/
class	TextImager::Tablet_Acquirer {
	public:
		Tablet_Acquirer (const TextImager* textImager):
			fTextImager (textImager)
			{
				Led_AssertNotNil (fTextImager);
				fTablet = fTextImager->AcquireTablet ();
			}
		operator	Led_Tablet  ()
			{
				Led_AssertNotNil (fTablet);
				return (fTablet);
			}
		Led_Tablet	operator-> ()
			{
				return fTablet;
			}
		~Tablet_Acquirer ()
			{
				Led_AssertNotNil (fTextImager);
				Led_AssertNotNil (fTablet);
				fTextImager->ReleaseTablet (fTablet);
			}
	private:
		const TextImager*	fTextImager;
		Led_Tablet			fTablet;
};







/*
@CLASS:			TextImager::SimpleTabStopList
@BASES:			@'TextImager::TabStopList'
@DESCRIPTION:	<p>A simple tabstop implementation in which all tabstops are equidistant from each other, and start
			at position zero. This is commonly used as the default tabstop object.</p>
*/
class	TextImager::SimpleTabStopList : public TextImager::TabStopList {
	public:
		SimpleTabStopList (Led_TWIPS twipsPerTabStop);
	public:
		override	Led_TWIPS	ComputeIthTab (size_t i) const;
		override	Led_TWIPS	ComputeTabStopAfterPosition (Led_TWIPS afterPos) const;
	public:
		Led_TWIPS	fTWIPSPerTabStop;
};





/*
@CLASS:			TextImager::StandardTabStopList
@BASES:			@'TextImager::TabStopList'
@DESCRIPTION:	<p>A simple tabstop implementation in the caller specifies the exact position of each tabstop. This
			is most directly ananogous to the Win32SDK GetTabbedTextExtent () API apporach.</p>
*/
class	TextImager::StandardTabStopList : public TextImager::TabStopList {
	public:
		StandardTabStopList ();		// default to 1/2 inch
		StandardTabStopList (Led_TWIPS eachWidth);
		StandardTabStopList (const vector<Led_TWIPS>& tabstops);
		StandardTabStopList (const vector<Led_TWIPS>& tabstops, Led_TWIPS afterTabsWidth);
	public:
		override	Led_TWIPS	ComputeIthTab (size_t i) const;
		override	Led_TWIPS	ComputeTabStopAfterPosition (Led_TWIPS afterPos) const;
	public:
		Led_TWIPS			fDefaultTabWidth;	// 	for tabs PAST the ones specified in the fTabStops list
		vector<Led_TWIPS>	fTabStops;
	public:
		nonvirtual	bool	operator== (const StandardTabStopList& rhs) const;
		nonvirtual	bool	operator!= (const StandardTabStopList& rhs) const;
};




/*
@CLASS:			TextImager::GoalColumnRecomputerControlContext
@DESCRIPTION:	<p></p>
*/
class	TextImager::GoalColumnRecomputerControlContext {
	public:
		GoalColumnRecomputerControlContext (TextImager& imager, bool supressRecompute);
		~GoalColumnRecomputerControlContext ();

	private:
		TextImager&	fTextImager;
		bool		fSavedSupressRecompute;
};





/*
@CLASS:			TrivialImager<TEXTSTORE,IMAGER>
@DESCRIPTION:	<p>Handy little template, if you want to use the power of Led, but just to wrap a particular imager,
			in a localized, one-time fasion, say todo printing, or some such. Not for interactors.</p>
				<p>Depending on the IMAGER you wish to use, you may want to try @'TrivialImager_Interactor<TEXTSTORE,IMAGER>'
			or @'TrivialWordWrappedImager<TEXTSTORE, IMAGER>' - both of which come with examples of their use.</p>
 */
template	<typename TEXTSTORE, typename	IMAGER>
	class	TrivialImager : public IMAGER {
		private:
			typedef	IMAGER	inherited;
		protected:
			TrivialImager (Led_Tablet t);
		public:
			TrivialImager (Led_Tablet t, Led_Rect bounds, const Led_tString& initialText = LED_TCHAR_OF(""));
			~TrivialImager ();


		public:
			nonvirtual	void	Draw (bool printing = false);
			override	void	Draw (const Led_Rect& subsetToDraw, bool printing);

		protected:
			override	Led_Tablet	AcquireTablet () const;
			override	void		ReleaseTablet (Led_Tablet /*tablet*/) const;

		protected:
			override	void	EraseBackground (Led_Tablet tablet, const Led_Rect& subsetToDraw, bool printing);

		protected:
			nonvirtual	void	SnagAttributesFromTablet ();

		public:
			nonvirtual	Led_Color	GetBackgroundColor () const;
			nonvirtual	void		SetBackgroundColor (Led_Color c);
			nonvirtual	bool		GetBackgroundTransparent () const;
			nonvirtual	void		SetBackgroundTransparent (bool transparent);

		private:
			TEXTSTORE	fTextStore;
			Led_Tablet	fTablet;
			bool		fBackgroundTransparent;
	};






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

// class TextImager::SimpleTabStopList
		inline	TextImager::SimpleTabStopList::SimpleTabStopList (Led_TWIPS twipsPerTabStop):
			TabStopList (),
			fTWIPSPerTabStop (twipsPerTabStop)
			{
				Led_Require (twipsPerTabStop > 0);
			}
		inline	Led_TWIPS	TextImager::SimpleTabStopList::ComputeIthTab (size_t i) const
			{
				return Led_TWIPS ((i+1)*fTWIPSPerTabStop);
			}
		inline	Led_TWIPS	TextImager::SimpleTabStopList::ComputeTabStopAfterPosition (Led_TWIPS afterPos) const
			{
				Led_Assert (fTWIPSPerTabStop > 0);
				size_t	idx	=	afterPos/fTWIPSPerTabStop;
				Led_TWIPS	result	=	Led_TWIPS ((idx+1)*fTWIPSPerTabStop);
				Led_Ensure (result % fTWIPSPerTabStop == 0);
				Led_Ensure (result > afterPos);
				return result;
			}



//	class	TextImager::GoalColumnRecomputerControlContext
		inline	TextImager::GoalColumnRecomputerControlContext::GoalColumnRecomputerControlContext (TextImager& imager, bool supressRecompute):
			fTextImager (imager),
			fSavedSupressRecompute (imager.fSupressGoalColumnRecompute)
			{
				imager.fSupressGoalColumnRecompute = supressRecompute;
			}
		inline	TextImager::GoalColumnRecomputerControlContext::~GoalColumnRecomputerControlContext ()
			{
				fTextImager.fSupressGoalColumnRecompute = fSavedSupressRecompute;
			}



// class TextImager::StandardTabStopList
		inline	TextImager::StandardTabStopList::StandardTabStopList ():
			TabStopList (),
			fDefaultTabWidth (720),		//	default to 1/2 inch - RTF spec default
			fTabStops ()
			{
				Led_Assert (fDefaultTabWidth > 0);
			}
		inline	TextImager::StandardTabStopList::StandardTabStopList (Led_TWIPS eachWidth):
			TabStopList (),
			fDefaultTabWidth (eachWidth),
			fTabStops ()
			{
				Led_Require (fDefaultTabWidth > 0);
			}
		inline	TextImager::StandardTabStopList::StandardTabStopList (const vector<Led_TWIPS>& tabstops):
			TabStopList (),
			fDefaultTabWidth (720),		//	default to 1/2 inch - RTF spec default
			fTabStops (tabstops)
			{
				Led_Assert (fDefaultTabWidth > 0);
			}
		inline	TextImager::StandardTabStopList::StandardTabStopList (const vector<Led_TWIPS>& tabstops, Led_TWIPS afterTabsWidth):
			TabStopList (),
			fDefaultTabWidth (afterTabsWidth),
			fTabStops (tabstops)
			{
				Led_Require (fDefaultTabWidth > 0);
			}
		inline	Led_TWIPS	TextImager::StandardTabStopList::ComputeIthTab (size_t i) const
			{
				Led_TWIPS	r	=	Led_TWIPS (0);
				size_t		smallI	=	min (i+1, fTabStops.size ());
				for (size_t j = 0; j < smallI; ++j) {
					r += fTabStops [j];
				}
				if (smallI <= i) {
					r = 	Led_TWIPS ((r/fDefaultTabWidth+(i-smallI+1))*fDefaultTabWidth);
				}
				return r;
			}
		inline	Led_TWIPS	TextImager::StandardTabStopList::ComputeTabStopAfterPosition (Led_TWIPS afterPos) const
			{
				// Instead if walking all tabstops til we find the right one - GUESS where the right one is (division) and then
				// walk back and forth if/as needed to narrow it down. This will guess perfectly if there are no user-defined tabstops.
				size_t		guessIdx	=	afterPos / fDefaultTabWidth;
				Led_TWIPS	guessVal	=	ComputeIthTab (guessIdx);

				// Go back first to assure we've gotten the FIRST one after 'afterPos' - not just 'A' tabstop after 'afterPos'.
				while (guessIdx > 0 and guessVal > afterPos) {
					Led_Assert (guessIdx == 0 or ComputeIthTab (guessIdx-1) < ComputeIthTab (guessIdx));	// assure monotonicly increasing so this will complete!
					guessIdx--;
					guessVal = ComputeIthTab (guessIdx);
				}

				// Now we've scanned to a good spot to start looking...
				Led_Assert (guessIdx == 0 or guessVal <= afterPos);
				for ( ; ; ++guessIdx) {
					Led_Assert (guessIdx == 0 or ComputeIthTab (guessIdx-1) < ComputeIthTab (guessIdx));	// assure monotonicly increasing so this will complete!
					Led_TWIPS	d	=	ComputeIthTab (guessIdx);
					if (d > afterPos) {
						return d;
					}
				}
				#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
					#pragma push
                                        #pragma warn -8008
					#pragma warn -8066
				#endif
				Led_Assert (false); return afterPos;
				#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
					#pragma pop
				#endif
			}
		inline	bool	TextImager::StandardTabStopList::operator== (const StandardTabStopList& rhs) const
			{
				return fDefaultTabWidth == rhs.fDefaultTabWidth and fTabStops == rhs.fTabStops;
			}
		inline	bool	TextImager::StandardTabStopList::operator!= (const StandardTabStopList& rhs) const
			{
				return not (*this == rhs);
			}



	inline	void	TextImager::SetWindowRect_ (const Led_Rect& windowRect)
		{
			fWindowRect = windowRect;
		}
	/*
	@METHOD:		TextImager::GetForceAllRowsShowing
	@DESCRIPTION:	<p>Return the ForceAllRowsShowing flag. If this flag is true, then the TextImager
				will never allow scrolling so that the last row doesn't reach the end of the buffer.
				This is useful primarily for printing. Because otherwise, you would probably always want
				to disable scrolling past the point where the last row is showing.</p>
					<p>But thats a UI choice we allow you to make if you like (or if you have some other application
				like printing where it makes sense).</p>
					<p>Call @'TextImager::SetForceAllRowsShowing' () to set the value.</p>
	*/
	inline	bool	TextImager::GetForceAllRowsShowing () const
		{
			return (fForceAllRowsShowing);
		}
	/*
	@METHOD:		TextImager::SetForceAllRowsShowing
	@DESCRIPTION:	<p>See @'TextImager::GetForceAllRowsShowing'.</p>
	*/
	inline	void	TextImager::SetForceAllRowsShowing (bool forceAllRowsShowing)
		{
			if (forceAllRowsShowing != fForceAllRowsShowing) {
				fForceAllRowsShowing = forceAllRowsShowing;
				AssureWholeWindowUsedIfNeeded ();
			}
		}
	/*
	@METHOD:		TextImager::GetImageUsingOffscreenBitmaps
	@DESCRIPTION:	<p>Led already has very little flicker. This is because we are very careful to
				draw as little as possible, and to draw quickly. But some cases still exist.
				Like large pictures being drawn are flicker, cuz we must erase the bounds and then
				draw the picture.</p>
					<p>Using this gets rid of these few cases of flicker, but at a small performance cost
			in overall draw speed.</p>
					<p>This value defaults to true (temporarily defaulting to @'qUseOffscreenBitmapsToReduceFlicker).</p>
					<p>See @'TextImager::SetImageUsingOffscreenBitmaps'.</p>
	*/
	inline	bool	TextImager::GetImageUsingOffscreenBitmaps () const
		{
			return fImageUsingOffscreenBitmaps;
		}
	/*
	@METHOD:		TextImager::SetImageUsingOffscreenBitmaps
	@DESCRIPTION:	<p>See @'TextImager::GetForceAllRowsShowing'.</p>
	*/
	inline	void	TextImager::SetImageUsingOffscreenBitmaps (bool imageUsingOffscreenBitmaps)
		{
			if (imageUsingOffscreenBitmaps != fImageUsingOffscreenBitmaps) {
				fImageUsingOffscreenBitmaps = imageUsingOffscreenBitmaps;
				// add some hook function in the future if there is a need for subclasses to know this has changed...
			}
		}
	/*
	@METHOD:		TextImager::GetHScrollPos
	@DESCRIPTION:	<p>Returns the horizontal offset the image is scrolled to. If this is zero, then
				no horizontal scrolling has taken place. Call @'TextImager::SetHScrollPos ()' to set where you have
				horizontally scrolled to.</p>
	*/
	inline	Led_Coordinate	TextImager::GetHScrollPos () const
		{
			return (fHScrollPos);
		}
	inline	void	TextImager::SetHScrollPos_ (Led_Coordinate hScrollPos)
		{
			fHScrollPos = hScrollPos;
		}
	/*
	@METHOD:		TextImager::GetSelectionGoalColumn
	@DESCRIPTION:	<p></p>
	*/
	inline	Led_TWIPS	TextImager::GetSelectionGoalColumn () const
		{
			return (fSelectionGoalColumn);
		}
	/*
	@METHOD:		TextImager::SetSelectionGoalColumn
	@DESCRIPTION:	<p></p>
	*/
	inline	void	TextImager::SetSelectionGoalColumn (Led_TWIPS selectionGoalColumn)
		{
			fSelectionGoalColumn = selectionGoalColumn;
		}
	inline	void	TextImager::InvalidateScrollBarParameters ()
		{
			// typically you would override this to update your scrollbar position, and perhaps
			// hilight state (if you have a scrollbar). Typically only used in TextInteractor subclasses.
		}
	/*
	@METHOD:		TextImager::GetUseSelectEOLBOLRowHilightStyle
	@DESCRIPTION:	<p>On MacOS - when you select a region of text which includes a newline - the visual display of selecting
				the newline is that you 'invert' or 'hilight' all the way to the end of the following row and in the following
				row all the way to the start of the next character. On Windows - you simply don't show the hilighting for the newline
				(as in Notepad) or you display a very narrow empty character (as in WinWord XP).</p>
					<p>I personally don't like this Windows style hilight display, and greatly prefer the MacOS style. Since
				nobody has ever complained - the MacOS style will remain the default. But - since I was rewriting the 
				draw-hilgiht code anyhow (SPR#1207 etc), and I noted this would be easy - I now support both styles
				in case someone prefers the more standard windows style.</p>
					<p>See also @'TextImager::SetUseSelectEOLBOLRowHilightStyle'.</p>
	*/
	inline	bool	TextImager::GetUseSelectEOLBOLRowHilightStyle () const
		{
			return fUseEOLBOLRowHilightStyle;
		}
	/*
	@METHOD:		TextImager::SetUseSelectEOLBOLRowHilightStyle
	@DESCRIPTION:	<p>See also @'TextImager::GetUseSelectEOLBOLRowHilightStyle'.</p>
	*/
	inline	void	TextImager::SetUseSelectEOLBOLRowHilightStyle (bool useEOLBOLRowHilightStyle)
		{
			fUseEOLBOLRowHilightStyle = useEOLBOLRowHilightStyle;
		}
	inline	bool	TextImager::GetSelectionShown () const
		{
			return (fSelectionShown);
		}
	/*
	@METHOD:		TextImager::GetWindowRect
	@DESCRIPTION:	<p>Returns the WindowRect associated with this TextImager. The WindowRect is essentially two things. Its
				origin specifies the offset mapping applied to all coordinate transformations before GDI output (and typically in TextInteractors
				UI input). And its Size dictate the size of the current window, used for deciding how much to draw, and scrolling operations.</p>
					<p>You can logically think of this - in Win32 SDK terminology - as the 'client rect'. In MacOS SDK terms, its close
				to the 'portRect' - its the same if you presume you are already focused, or that your text
				editor view takes up the entire window.</p>
					<p>See @'TextImager' for more information about Windows/Scrolling. See also @'TextImager::SetWindowRect'.</p>
	*/
	inline	Led_Rect	TextImager::GetWindowRect () const
		{
			return (fWindowRect);
		}
	/*
	@METHOD:		TextImager::GetDefaultFont
	@DESCRIPTION:	<p>Returns the default font used to image new text. The exact meaning and use of this
		default font will depend some on the particular TextImager subclass you are using.</p>
	*/
	inline	Led_FontSpecification	TextImager::GetDefaultFont () const
		{
			return (fDefaultFont);
		}
	/*
	@METHOD:		TextImager::GetRowLength
	@DESCRIPTION:	<p>Returns the number of tChars within the given row (by row index). This method's use is discouraged
				when word-wrapping is present, because it forces word-wrapping up to the given row # in the buffer.</p>
	*/
	inline	size_t	TextImager::GetRowLength (size_t rowNumber) const
		{
			return (GetEndOfRow (rowNumber) - GetStartOfRow (rowNumber));
		}
	/*
	@METHOD:		TextImager::GetDefaultTextColor
	@DESCRIPTION:	<p>Returns the value of the given indexed color attribute. These colors values default to NULL,
				which means @'TextImager::GetEffectiveDefaultTextColor' will use the system default for each color value.</p>
					<p>See also
						<ul>
							<li>@'TextImager::GetEffectiveDefaultTextColor'</li>
							<li>@'TextImager::ClearDefaultTextColor'</li>
							<li>@'TextImager::SetDefaultTextColor'</li>
						</ul>
					</p>
	*/
	inline	Led_Color*	TextImager::GetDefaultTextColor (DefaultColorIndex dci) const
		{
			Led_Require (dci < eMaxDefaultColorIndex);
			return fDefaultColorIndex[dci];
		}
	/*
	@METHOD:		TextImager::GetEffectiveDefaultTextColor
	@DESCRIPTION:	<p>Returns either the effective color corresponding to the given @'TextImager::DefaultColorIndex',
				or its default value.</p>
					<p>See also
						<ul>
							<li>@'TextImager::GetDefaultTextColor'</li>
							<li>@'TextImager::ClearDefaultTextColor'</li>
							<li>@'TextImager::SetDefaultTextColor'</li>
						</ul>
					</p>
	*/
	inline	Led_Color	TextImager::GetEffectiveDefaultTextColor (DefaultColorIndex dci) const
		{
			Led_Require (dci < eMaxDefaultColorIndex);
			if (fDefaultColorIndex[dci] == NULL) {
				switch (dci) {
					case	eDefaultTextColor:						return fDefaultFont.GetTextColor ();;
					case	eDefaultBackgroundColor:				return Led_GetTextBackgroundColor ();
					case	eDefaultSelectedTextColor:				return Led_GetSelectedTextColor ();
					case	eDefaultSelectedTextBackgroundColor:	return Led_GetSelectedTextBackgroundColor ();
					default:										Led_Assert (false);/*NOTREACHED*/ return Led_Color::kBlack;
				}
			}
			else {
				return *fDefaultColorIndex[dci];
			}
		}
	/*
	@METHOD:		TextImager::ClearDefaultTextColor
	@DESCRIPTION:	<p>Nulls out the given default color attribute, which means @'TextImager::GetEffectiveDefaultTextColor' will
				use the system default for each color value.</p>
					<p>See also
						<ul>
							<li>@'TextImager::GetDefaultTextColor'</li>
							<li>@'TextImager::GetEffectiveDefaultTextColor'</li>
							<li>@'TextImager::SetDefaultTextColor'</li>
						</ul>
					</p>
	*/
	inline	void	TextImager::ClearDefaultTextColor (DefaultColorIndex dci)
		{
			Led_Require (dci < eMaxDefaultColorIndex);
			delete fDefaultColorIndex[dci];
			fDefaultColorIndex[dci] = NULL;
			if (dci == eDefaultTextColor) {
				fDefaultFont.SetTextColor (Led_GetTextColor ());
			}
		}
	/*
	@METHOD:		TextImager::SetDefaultTextColor
	@DESCRIPTION:	<p>Sets the given indexed default text color attribute. The effect of this is to override the behavior
				of @'TextImager::GetEffectiveDefaultTextColor'.</p>
					<p>See also
						<ul>
							<li>@'TextImager::GetDefaultTextColor'</li>
							<li>@'TextImager::GetEffectiveDefaultTextColor'</li>
							<li>@'TextImager::ClearDefaultTextColor'</li>
						</ul>
					</p>
	*/
	inline	void	TextImager::SetDefaultTextColor (DefaultColorIndex dci, const Led_Color& textColor)
		{
			Led_Require (dci < eMaxDefaultColorIndex);
			ClearDefaultTextColor (dci);
			if (dci == eDefaultTextColor) {
				fDefaultFont.SetTextColor (textColor);
			}
			fDefaultColorIndex[dci] = new Led_Color (textColor);
		}
	/*
	@METHOD:		TextImager::GetStartOfNextRowFromRowContainingPosition
	@DESCRIPTION:	<p>Returns the marker position of the start of the row following the row which contains the given marker position.
				If the given marker position is already in the last row, then it returns the start of that last row. So the result could
				posibly be less than the initial value.</p>
					<p>The reason for this apparantly obscure API is that marker positions allow you to name rows in a way which doesn't involve
				word-wrapping (row numbers would). And yet this code doesn't assume use of MultiRowTextImager, or any of its data structures (so
				these APIs can be used in things like TextInteractor).</p>
	*/
	inline	size_t	TextImager::GetStartOfNextRowFromRowContainingPosition (size_t charPosition) const
		{
			// Use startOfRow positions as 'tokens' for RowReferences...
			size_t	rowEnd	=	FindNextCharacter (GetEndOfRowContainingPosition (charPosition));
			return GetStartOfRowContainingPosition (rowEnd);
		}
	/*
	@METHOD:		TextImager::GetStartOfPrevRowFromRowContainingPosition
	@DESCRIPTION:	<p>Returns the marker position of the start of the row preceeding the row
				which contains the given marker position, or the beginning of the buffer, if none preceed it.</p>
					<p>See the docs for @'TextImager::GetStartOfNextRowFromRowContainingPosition' for
				insight into how to use this API.</p>
	*/
	inline	size_t	TextImager::GetStartOfPrevRowFromRowContainingPosition (size_t charPosition) const
		{
			// Use startOfRow positions as 'tokens' for RowReferences...
			size_t	rowStart	=	GetStartOfRowContainingPosition (charPosition);
			if (rowStart > 0) {
				rowStart = GetStartOfRowContainingPosition (FindPreviousCharacter (rowStart));
			}
			return rowStart;
		}



//	class	TextImager::FontCacheInfoUpdater
	inline	TextImager::FontCacheInfoUpdater::~FontCacheInfoUpdater ()
		{
			#if		qWindows
				if (fRestoreObject != NULL) {
					Led_Verify (::SelectObject (fTablet->m_hDC, fRestoreObject));
				}
				if (fRestoreAttribObject != NULL) {
					Led_Verify (::SelectObject (fTablet->m_hAttribDC, fRestoreAttribObject));
				}
			#endif
		}
	inline	Led_FontMetrics		TextImager::FontCacheInfoUpdater::GetMetrics () const
		{
			return fImager->fCachedFontInfo;
		}




//	class	TrivialImager<TEXTSTORE,IMAGER>
	template	<typename TEXTSTORE, typename	IMAGER>
		/*
		@METHOD:		TrivialImager<TEXTSTORE,IMAGER>::TrivialImager
		@DESCRIPTION:	<p>Two overloaded versions - one protected, and the other public. The protected one
					does NOT call @'TrivialImager<TEXTSTORE,IMAGER>::SnagAttributesFromTablet' - so you must in your subclass.</p>
		*/
		TrivialImager<TEXTSTORE,IMAGER>::TrivialImager (Led_Tablet t):
			IMAGER (),
			fTablet (t),
			fTextStore (),
			fBackgroundTransparent (false)
			{
				SpecifyTextStore (&fTextStore);
			}
	template	<typename TEXTSTORE, typename	IMAGER>
		TrivialImager<TEXTSTORE,IMAGER>::TrivialImager (Led_Tablet t, Led_Rect bounds, const Led_tString& initialText):
			IMAGER (),
			fTablet (t),
			fTextStore (),
			fBackgroundTransparent (false)
			{
				SpecifyTextStore (&fTextStore);
				SnagAttributesFromTablet ();
				SetWindowRect (bounds);
				fTextStore.Replace (0, 0, initialText.c_str (), initialText.length ());
			}
	template	<typename TEXTSTORE, typename	IMAGER>
		TrivialImager<TEXTSTORE,IMAGER>::~TrivialImager ()
			{
				SpecifyTextStore (NULL);
			}
	template	<typename TEXTSTORE, typename	IMAGER>
		void	TrivialImager<TEXTSTORE,IMAGER>::Draw (bool printing)
			{
				Draw (GetWindowRect (), printing);
			}
	template	<typename TEXTSTORE, typename	IMAGER>
		void	TrivialImager<TEXTSTORE,IMAGER>::Draw (const Led_Rect& subsetToDraw, bool printing)
			{
				IMAGER::Draw (subsetToDraw, printing);
			}
	template	<typename TEXTSTORE, typename	IMAGER>
		Led_Tablet	TrivialImager<TEXTSTORE,IMAGER>::AcquireTablet () const
			{
				return fTablet;
			}
	template	<typename TEXTSTORE, typename	IMAGER>
		void	TrivialImager<TEXTSTORE,IMAGER>::ReleaseTablet (Led_Tablet /*tablet*/) const
			{
			}
	template	<typename TEXTSTORE, typename	IMAGER>
		void	TrivialImager<TEXTSTORE,IMAGER>::EraseBackground (Led_Tablet tablet, const Led_Rect& subsetToDraw, bool printing)
			{
				if (not fBackgroundTransparent) {
					inherited::EraseBackground (tablet, subsetToDraw, printing);
				}
			}
	template	<typename TEXTSTORE, typename	IMAGER>
		/*
		@METHOD:		TrivialImager<TEXTSTORE,IMAGER>::SnagAttributesFromTablet
		@DESCRIPTION:	<p>Snag font, background color etc, from the currently associated tablet.</p>
						<p>Since this calls virtual methods of the imager (this) - it must be called in the final
					CTOR (most specific type). Really it shouldnt need to be THE most specific - just enough specific to
					get the right virtual methods called. But because of MSVC compiler bugs 
					(@'qCannotSafelyCallLotsOfComplexVirtMethodCallsInsideCTORDTOR')- its generally best to be THE final
					CTOR.</p>
		*/
		void	TrivialImager<TEXTSTORE,IMAGER>::SnagAttributesFromTablet ()
			{
				#if		qMacOS
					// Should probably do something similar?
				#elif	qWindows
					HFONT	hFont	=	(HFONT)::GetCurrentObject (fTablet->m_hAttribDC, OBJ_FONT);
					Led_Verify (hFont != NULL);
					LOGFONT	lf;
					Led_Verify (::GetObject (hFont, sizeof(LOGFONT), &lf));
					SetDefaultFont (Led_FontSpecification (lf));
					SetDefaultTextColor (eDefaultBackgroundColor, Led_Color (::GetBkColor (fTablet->m_hAttribDC)));
					if (::GetBkMode (fTablet->m_hAttribDC) == TRANSPARENT) {
						SetBackgroundTransparent (true);
					}
				#endif
			}
	template	<typename TEXTSTORE, typename	IMAGER>
		/*
		@METHOD:		TrivialImager<TEXTSTORE,IMAGER>::GetBackgroundColor
		@DESCRIPTION:	<p>See also @'TrivialImager<TEXTSTORE,IMAGER>::SetBackgroundColor'.</p>
		*/
		inline	Led_Color	TrivialImager<TEXTSTORE,IMAGER>::GetBackgroundColor () const
			{
				return GetEffectiveDefaultTextColor (eDefaultBackgroundColor);
			}
	template	<typename TEXTSTORE, typename	IMAGER>
		/*
		@METHOD:		TrivialImager<TEXTSTORE,IMAGER>::SetBackgroundColor
		@DESCRIPTION:	<p>Specifies the color the background of the imager will be drawn with.</p>
						<p>See also @'TrivialImager<TEXTSTORE,IMAGER>::SetBackgroundTransparent',
					@'TrivialImager<TEXTSTORE,IMAGER>::GetBackgroundColor'.</p>
		*/
		inline	void	TrivialImager<TEXTSTORE,IMAGER>::SetBackgroundColor (Led_Color c)
			{
				SetDefaultTextColor (eDefaultBackgroundColor, c);
			}
	template	<typename TEXTSTORE, typename	IMAGER>
		/*
		@METHOD:		TrivialImager<TEXTSTORE,IMAGER>::GetBackgroundTransparent
		@DESCRIPTION:	<p>See also @'TrivialImager<TEXTSTORE,IMAGER>::SetBackgroundTransparent'.</p>
		*/
		inline	bool	TrivialImager<TEXTSTORE,IMAGER>::GetBackgroundTransparent () const
			{
				return fBackgroundTransparent;
			}
	template	<typename TEXTSTORE, typename	IMAGER>
		/*
		@METHOD:		TrivialImager<TEXTSTORE,IMAGER>::SetBackgroundTransparent
		@DESCRIPTION:	<p>Specifies that the given TrivialImager will (or will not) draw any background color. Use this if
					you are drawing over an existing background which you don't want touched - you just want the TEXT imaged
					and no 'erasing' to be done. When TRUE, it negates the effect of @'TrivialImager<TEXTSTORE,IMAGER>::SetBackgroundColor'</p>
						<p>This defaults to off (except if overriden by @'TrivialImager<TEXTSTORE,IMAGER>::SnagAttributesFromTablet' if your
					Led_Tablet's ::GetBkMode ()==TRANSPARENT)</p>
						<p>See also @'TrivialImager<TEXTSTORE,IMAGER>::GetBackgroundTransparent',
					@'TrivialImager<TEXTSTORE,IMAGER>::SetBackgroundColor'.</p>
		*/
		inline	void	TrivialImager<TEXTSTORE,IMAGER>::SetBackgroundTransparent (bool transparent)
			{
				fBackgroundTransparent = transparent;
			}


#if		qLedUsesNamespaces
}
#endif



#endif	/*__TextImager_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
