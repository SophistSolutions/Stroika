/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/TextImager.cpp,v 2.171 2004/01/26 20:20:39 lewis Exp $
 *
 * Changes:
 *	$Log: TextImager.cpp,v $
 *	Revision 2.171  2004/01/26 20:20:39  lewis
 *	SPR#1604: Added Get/SetSelectionGoalColumnm and RecomputeSelectionGoalColumn to help implement goal column functionality with arrowkeys. Added GoalColumnRecomputerControlContext to allow prevention of updating goal column in special cases (like when using up/down arrows). As speed tweek (partly needed cuz we got rid of MRTI overrides and partly just a good idea I noticed) - use stuff like GetStartOfNextRowFromRowContainingPosition () instead of GetRowContainingPosition (in other words - dont use explicit row#s). Similarly - use GetRowRelativeCharAtLoc instead of GetCharAtLocation. Updated docs on GetCharLocation etc to use 'relative' versions as a speedtweek
 *	
 *	Revision 2.170  2004/01/26 02:37:49  lewis
 *	SPR#1623: TextImager::ComputeRelativePosition () is fixed for eCursorByLine eCursorToStart/eCursorToEnd cases. Now - in LedIt - we get the same (or better than) MSWord cursor-by-line functionality (CONTROL UP/DOWN arrow)
 *	
 *	Revision 2.169  2003/12/01 03:04:38  lewis
 *	SPR#1573: TextImager::GetTextWindowBoundingRect () pins result
 *	
 *	Revision 2.168  2003/11/27 20:13:52  lewis
 *	SPR#1566: small tweek to TextImager::DrawSegment_ () sped things up a bit
 *	
 *	Revision 2.167  2003/11/06 02:23:17  lewis
 *	doccomments
 *	
 *	Revision 2.166  2003/11/03 20:05:47  lewis
 *	add new TextImager::GetTextLayoutBlock () method - which in subclasses - can take into account
 *	subclasses to use an intiail dir from first row in subsequent rows. Related - added
 *	PartitioningTextImager::GetPrimaryPartitionTextDirection ()
 *	
 *	Revision 2.165  2003/06/04 14:52:59  lewis
 *	SPR#1523: instead of adjusting fontsize by 1/2 - use 2/3 - for subscript/superscripts
 *	
 *	Revision 2.164  2003/04/09 21:13:34  lewis
 *	SPR#1417: Fixed small bug in TextImager::GetSelectionWindowRects () handling of interline space.
 *	It assumed GetRowHilights () returned rects all with the same top (or at least where none were ATOP
 *	of another). But - with individual table cells being returned - you can get many rows of rectangles
 *	returned on top of one anohter. A stupid waste - since I don't think we even use Interline space
 *	anymore (thoguh who knows - we may someday!)
 *	
 *	Revision 2.163  2003/04/08 21:41:36  lewis
 *	SPR#1404: to help implement table selection, made a version of TextImager::GetSelectionWindowRects
 *	which returns vector<RECT> to replace GetSelectionWindowRegion
 *	
 *	Revision 2.162  2003/03/27 16:23:09  lewis
 *	SPR#1384: TextImager::SetDefaultFont_ now uses SimpleUpdater (realContentUpdate=false) since that
 *	generally shouldn't be treated as changing the docs contents
 *	
 *	Revision 2.161  2003/03/25 15:47:40  lewis
 *	DOCCOMMENT
 *	
 *	Revision 2.160  2003/03/21 02:54:38  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.159  2003/03/19 13:56:45  lewis
 *	extra asserts
 *	
 *	Revision 2.158  2003/03/11 19:34:27  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.157  2003/03/07 22:31:40  lewis
 *	fix access (public) to TextImager::GetIntraRowTextWindowBoundingRect
 *	
 *	Revision 2.156  2003/03/07 21:44:06  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.155  2003/03/06 14:01:08  lewis
 *	allow height returned in TextImager::GetRowHilightRects to be zero - just filter them out
 *	
 *	Revision 2.154  2003/02/28 19:09:22  lewis
 *	SPR#1316- CRTDBG_MAP_ALLOC/CRTDBG_MAP_ALLOC_NEW Win32/MSVC debug mem leak support
 *	
 *	Revision 2.153  2003/02/03 17:09:22  lewis
 *	doccomments
 *	
 *	Revision 2.152  2003/02/01 15:49:18  lewis
 *	last SPR# was really 1279(not 1271)
 *	
 *	Revision 2.151  2003/02/01 15:48:21  lewis
 *	SPR#1271- Added TextImager::GetDefaultSelectionFont () and overrode in StandardStyledTextImager,
 *	and used that in WM_GETFONT Led_Win32 callback
 *	
 *	Revision 2.150  2003/01/29 17:59:55  lewis
 *	SPR#1264- Get rid of most of the SetDefaultFont overrides and crap - and made InteractiveSetFont
 *	REALLY do the interactive command setfont - and leave SetDefaultFont to just setting the 'default font'
 *	
 *	Revision 2.149  2003/01/17 00:44:32  lewis
 *	add asserts to TextImager::GetRowHilightRects/TextImager::GetSelectionWindowRegion to assure dealing
 *	with valid RECTS. And- fix GetRowHilightRects for case where the row extends PAST the end of the
 *	windowRect - take the MAX so we get a valid rowRect. Also - dont add EMPTY rowRects to the
 *	hilgihtRect list. SPR#1243
 *	
 *	Revision 2.148  2003/01/13 23:59:37  lewis
 *	small tweek to TextImager::GetTextWindowBoundingRect
 *	
 *	Revision 2.147  2003/01/13 23:57:12  lewis
 *	for SPR#1237 - wrap TextImager::GetTextBoundingRect in qSupportLed30CompatAPI (OBSOLETE). Minor tweek to
 *	TextImager::GetIntraRowTextWindowBoundingRect (). Rewrite for BIDI TextImager::GetTextWindowBoundingRect ()
 *	
 *	Revision 2.146  2003/01/12 16:27:15  lewis
 *	SPR#1235- small tweek to TextImager::GetSelectionWindowRegion - call GetRowHilightRects
 *	with full hilight range - not just subrow range
 *	
 *	Revision 2.145  2003/01/11 19:58:35  lewis
 *	SPR#1234 - another small hopefully final tweek to TextImager::GetRowHilightRects for EOL hilighting
 *	
 *	Revision 2.144  2003/01/11 19:28:47  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.143  2003/01/11 19:17:06  lewis
 *	SPR#1234 - I HOPE the final fix to TextImager::GetRowHilightRects. This time to better set the
 *	segmentHilightAtEndOfRow flag - and not hilight end of row unless we select the wrap char
 *	
 *	Revision 2.142  2003/01/10 22:06:58  lewis
 *	SPR#1230- more work on TextImager::GetRowHilightRects
 *	
 *	Revision 2.141  2003/01/10 15:40:02  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.140  2003/01/10 15:28:11  lewis
 *	Lost selectBOL/EOL args to GetIntraRowTextWindowBoundingRect (),
 *	and instead put that logic directly in TextImager::GetRowHilightRects (),
 *	and added assert there that we never create overlapping rects.
 *	
 *	Revision 2.139  2003/01/08 04:15:27  lewis
 *	OOPS - was really SPR#1218
 *	
 *	Revision 2.138  2003/01/08 04:08:16  lewis
 *	SPR#1223 - fixed small bug with TextImager::GetSelectionWindowRegion
 *	
 *	Revision 2.137  2002/12/21 03:00:12  lewis
 *	SPR#1217. Added virtual TextImager::InvalidateAllCaches and made SimpleTextImager/MRTI versions
 *	just overrides. No longer need SimpleTextImager::TabletChangedMetrics overrides etc
 *	since now TextImager::TabletChangedMetrics calls TextImager::InvalidateAllCaches.
 *	MultiRowTextImager::PurgeUnneededMemory () no longer needed since TextImager version
 *	now calls InvalidateAllCaches().
 *	
 *	Revision 2.136  2002/12/20 18:05:57  lewis
 *	lose some commented out code
 *	
 *	Revision 2.135  2002/12/20 17:59:19  lewis
 *	SPR#1216- Add new helper TextImager::GetRowHilightRects(). Call that from TextImager::DrawRowHilight
 *	instead of manually computing where to draw hilights. Then use that shared code
 *	from TextImager::GetSelectionWindowRegion (thereby fixing IT for BIDI). Also fixed
 *	TextImager::GetSelectionWindowRegion so it handles interline space (I hope - not tested).
 *	And got rid of overrides of GetSelectionWindowRegion from MRTI and SimpleTextImager.
 *	
 *	Revision 2.134  2002/12/16 19:02:58  lewis
 *	SPR#1208 - added TextImager::GetUseSelectEOLBOLRowHilightStyle support
 *	
 *	Revision 2.133  2002/12/16 18:44:21  lewis
 *	SPR#1207 - Small cleanup to TextImager::DrawRowHilight () and
 *	TextImager::GetIntraRowTextWindowBoundingRect so they draw end of line (etc) row hilights
 *	correctly.
 *	
 *	Revision 2.132  2002/12/13 22:03:53  lewis
 *	Major work on TextImager::GetIntraRowTextWindowBoundingRect () for SPR#1189 so we get
 *	BIDI drawing of row hilights correct.
 *	Added "NEEDS TO BE FIXED FOR BIDI" comment in a few places which need fixing ASAP.
 *	Several other small BIDI changes. Things in state of flux. Lots of coding,cleanup fixing needed.
 *	
 *	Revision 2.131  2002/12/03 20:10:44  lewis
 *	doccomemnts
 *	
 *	Revision 2.130  2002/12/03 18:09:01  lewis
 *	part of SPR#1189- drawrowhilight BIDI code. Lose WRTI::DrawRowHilight() code cuz unneeded and obsolete.
 *	Instead handle the hilight logic better in TextImager::DrawRowHilight and add extra arg to
 *	GetIntraRowTextWindowBoundingRect to handle margin case (better than in Led 3.0 - I think).
 *	Also - made GetIntraRowTextWindowBoundingRect protected instead of public
 *	
 *	Revision 2.129  2002/12/03 15:49:12  lewis
 *	SPR#1191- use new TextDirection enum defined in LedGDI.h. Get rid of ScriptRunElement::fLength.
 *	Add direction arg to Led_Tablet_::TabbedTextOut () and use that to call (WIN32)Led_Tablet_::SetTextAlign().
 *	Hopefully this fixes display on Win9x of Arabic text
 *	
 *	Revision 2.128  2002/12/03 14:41:00  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.127  2002/12/03 14:29:04  lewis
 *	SPR#1190 - changed name for (TextImager/Led_Tablet_)::HilightARectangle to HilightArea
 *	
 *	Revision 2.126  2002/12/03 02:52:42  lewis
 *	SPR#1188- fix small bug (hilightEnd >= GetEnd instead of rowEnd). Also made lots of other changes
 *	for SPR#1189 (BIDI selection hilight work)
 *	
 *	Revision 2.125  2002/12/02 16:45:14  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.124  2002/12/02 15:58:11  lewis
 *	SPR#1185 - move DrawRow,DrawRowSegments,DrawRowHilight,DrawInterlineSpace from MRTI/SimpleTextImager to
 *	TextImager (share code. Changed their args slightly. For BIDI work.
 *	
 *	Revision 2.123  2002/12/02 00:57:12  lewis
 *	SPR#1183 - BIDI - fixed TextImager::DrawSegement_() to handle multiple direction runs (really not needed -
 *	even for LedLineIt cuz it uses a styleTextImager, but you can shut that off - and then its needed)
 *	
 *	Revision 2.122  2002/12/01 17:52:47  lewis
 *	SPR#1183- summarizestylemark fixes for BIDI
 *	
 *	Revision 2.121  2002/11/28 02:31:19  lewis
 *	A couple simple name changes (GetVirtualText->PeekAtVirtualText and TextLayoutBlock_Subset to
 *	TextLayoutBlock_VirtualSubset
 *	
 *	Revision 2.120  2002/11/27 15:58:57  lewis
 *	SPR#1183 - as part of BIDI effort - DrawSegment now replaces 'Led_tChar* text' argument with a
 *	'TextLayoutBlock& text' argument
 *	
 *	Revision 2.119  2002/11/14 17:12:16  lewis
 *	SPR#1171- wrap qSupportLed30CompatAPI around NotificationOf_SelectionChanged
 *	
 *	Revision 2.118  2002/10/30 13:29:32  lewis
 *	SPR#1155 - Added new Rect_Base<> template and made Led_Rect a simple typedef of that. Then TRIED (failed) to
 *	make the top/left/bottom/right fields private by adding accessors. Added the accessors and used them in tons of places -
 *	but still TONS more required. Leave that battle for another day. I'm not even SURE its a good idea. Anyhow -
 *	also added a new typedef for Led_TWIPS_Rect (this was what I was after to begin with with all this nonsese)
 *
 *	Revision 2.117  2002/09/22 15:43:33  lewis
 *	doccomments
 *	
 *	Revision 2.116  2002/05/06 21:33:57  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.115  2001/11/27 00:30:02  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.114  2001/10/17 20:43:03  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.113  2001/09/24 14:14:36  lewis
 *	SPR#1042- new region support
 *	
 *	Revision 2.112  2001/09/12 16:20:35  lewis
 *	*** empty log message ***
 *
 *	Revision 2.111  2001/09/12 14:53:32  lewis
 *	SPR#0987- Added conditional UseSecondaryHilight support (macos&win32 only so far). Redo
 *	Led_Region support for MacOS so now more like with PC (no operator conversion to macregion
 *	and auto constructs OSRegion except when called with region arg - in whcih case we
 *	dont OWN region and dont delete it on DTOR
 *
 *	Revision 2.110  2001/09/05 13:38:37  lewis
 *	minor tweeks to TextImager::ScrollSoShowingHHelper ()
 *	
 *	Revision 2.109  2001/09/05 13:22:31  lewis
 *	SPR#1010- Spedup TextImager::ScrollSoShowingHHelper () (affected typing times).
 *	
 *	Revision 2.108  2001/08/29 23:36:36  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.107  2001/08/28 18:43:43  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.106  2001/07/19 02:21:48  lewis
 *	SPR#0960/0961- CW6Pro support, and preliminary Carbon SDK support
 *	
 *	Revision 2.105  2001/07/11 21:51:03  lewis
 *	SPR#0906- Partly implemented bullet/list support. Just did basic support (bullet lists,
 *	no other style lists). Did support indent levels (including new Increase/Decrease indent
 *	level commands). Still no RTF/HTML support, and a small display bug on following lines
 *	when I add bullet attribute.
 *	
 *	Revision 2.104  2001/05/26 18:20:35  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.103  2001/05/12 22:50:58  lewis
 *	SPR#0917- new TextImager::G/SetImageUsingOffscreenBitmaps () method to replace
 *	obsolete qUseOffscreenBitmapsToReduceFlicker
 *	
 *	Revision 2.102  2001/05/07 16:45:06  lewis
 *	SPR#0888. CLeaned up Led_FontMetrics stuff (esp for X-windows - but also for Mac/Win).
 *	New portable Led_Tablet_::GetFontMetrics method. Now Led_FontMEtrics for x-windows
 *	produces good values.
 *	
 *	Revision 2.101  2001/05/04 20:44:26  lewis
 *	SPR#0888- a lot of work on getting X fonts code working right.
 *	
 *	Revision 2.100  2001/05/04 17:38:00  lewis
 *	react to API change for Led_Tablet_::HilightARectangle_SolidHelper
 *	
 *	Revision 2.99  2001/05/04 17:09:57  lewis
 *	fixup commnets. SPR#0902 - Added GetEffectiveDefaultTextColor () family of functions to
 *	replace direct use of Led_GetTextBackgroundColor/Led_GetTextColor etc.
 *	
 *	Revision 2.98  2001/05/01 15:40:15  lewis
 *	minor tweeks to avoid GCC warnings
 *	
 *	Revision 2.97  2001/04/30 22:34:51  lewis
 *	SPR#0886- moved HilightARectangle_SolidHelper from TextImager class to Led_Tablet_.
 *	Similarly for EraseBackground_SoldHelper (was just copied and now really MOVED so old
 *	code wont compile (doesnt affect Led 2.3 but intermediate versions)
 *	
 *	Revision 2.96  2001/04/25 22:44:47  lewis
 *	SPR#0879- moved EraseBackground_SolidHelper () to Led_Tablet_ from TextImager(wrapper now
 *	in TextImager). And added SetFore/BackColor methods to Led_Tablet_ - again - cleaning
 *	up code from TextImager.
 *	
 *	Revision 2.95  2001/04/23 16:38:46  lewis
 *	SPR#0876- Moved some utilities from TextImager to LedGDI
 *	
 *	Revision 2.94  2001/04/23 15:57:17  lewis
 *	Lose qDoTrapCaching- flag - since 68K mac so old/unimportant
 *	
 *	Revision 2.93  2001/01/09 00:36:19  lewis
 *	patch for fix to SPR#0842. Was broken - and would assert out if you typed in
 *	continuously and got it to scroll right and then back to the left (set window
 *	smaller than margin and type). Code still not clear and elegant - probably should
 *	be rewritten. But I think it - TextImager::ScrollSoShowingHHelper () - should be
 *	good enough for now
 *	
 *	Revision 2.92  2001/01/03 16:00:33  lewis
 *	fix exact type of args to max/min functions to solve overload problem (borlandC++)
 *	
 *	Revision 2.91  2001/01/03 15:45:19  lewis
 *	support NON-STRICT windows.h setting - for BorlandC++
 *	
 *	Revision 2.90  2000/11/06 15:46:23  lewis
 *	SPR#0842- further fix this - so ScrollSoShowingHHelper () takes into account
 *	BOTH markerPos and 'tryToShowMarkerPos' Sources/TextImager.cpp
 *	
 *	Revision 2.89  2000/09/05 23:25:30  lewis
 *	cache the XFontStruct in the Led_Tablet. Tried to see if we could get rid of
 *	the fFOntID crap - but some wierd X-bug pretents it. DEbug later.(XWINDOWS)
 *	
 *	Revision 2.88  2000/09/05 21:27:04  lewis
 *	preliminary fix for PortableGDI_MeasureText for XWindows (hackish implemenation -
 *	very slow)
 *	
 *	Revision 2.87  2000/08/31 20:46:05  lewis
 *	use Led_Pen::kBlackPattern
 *	
 *	Revision 2.86  2000/07/11 15:39:39  lewis
 *	Add a few assertions
 *	
 *	Revision 2.85  2000/06/13 20:27:09  lewis
 *	hacks to get compiling on GCC/XWindows again
 *	
 *	Revision 2.84  2000/06/13 20:10:27  lewis
 *	small hacks to get compiling with GCC/XWindows
 *	
 *	Revision 2.83  2000/06/13 15:31:44  lewis
 *	SPR#0767- convert tabstop support to store TWIPS internally
 *	
 *	Revision 2.82  2000/06/12 20:05:54  lewis
 *	friendlier about questionable baseline values
 *	
 *	Revision 2.81  2000/06/12 16:20:27  lewis
 *	support SPR#0780- Led_Tablet_ support
 *	
 *	Revision 2.80  2000/05/30 13:34:42  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.79  2000/05/10 22:37:41  lewis
 *	fix Win32_GetTextExtentExPoint_Win95n98WorkAround () for display of Chinese under Win98 -
 *	the (CodePageBetterOffUsingWideCharVersion () hack didn't work cuz GetTextCharset() for 'MingLiu'
 *	font returned zero. So instead - decide to use WideChar version iff there are any WIDE-CHARS!
 *	
 *	Revision 2.78  2000/04/15 14:32:38  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.77  2000/04/14 22:40:28  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.76  2000/04/14 12:39:23  lewis
 *	SPR#0738- work around Win95/8 UNICODE TextImaging bugs for a few more charactersets -
 *	still not a good solution - but the best I can see
 *	
 *	Revision 2.75  2000/04/05 21:43:25  lewis
 *	SPR#0734- added TextImager::GetIntraRowTextWindowBoundingRect and revised TextImager::GetRowTextWindowBoundingRect
 *	to work around bug introduced in fix for 0730
 *	
 *	Revision 2.74  2000/04/04 21:05:24  lewis
 *	fix typo for building UNICODE apps
 *	
 *	Revision 2.73  2000/04/03 22:39:21  lewis
 *	SPR#0730: TextImager::GetTextWindowBoundingRect () was simplified, and fixed to handle
 *	single-row case properly (and other cases more simply)
 *	
 *	Revision 2.72  2000/03/29 05:17:43  lewis
 *	LAST CHECKIN COMMENT BOGUS. Really chaned the GetStaticDefaultFont () algorithm to work better -
 *	esp on foreign systems, picking forein fonts under Win2k
 *	
 *	Revision 2.71  2000/03/29 05:15:16  lewis
 *	experimental new qUseNewIMECode IME fixes. Must test at LEC. Basicly - use newer (documented)
 *	versions of the APIs since the old ones seem to sometimes mysteriously fail on Win2k
 *	
 *	Revision 2.70  2000/03/09 04:40:30  lewis
 *	replace old MultiRowTextImager::HilightARectanle_ (and SimpleTextImager version) with
 *	TextImager::HilightARectanlge() ETC - SPR#0708
 *	
 *	Revision 2.69  2000/03/08 13:20:15  lewis
 *	Preliminary qXWindows hacks - so we can get HelloWorld displayed
 *	
 *	Revision 2.68  2000/01/22 17:34:58  lewis
 *	Added TextImager::ScrollSoShowingHHelper () to share a little code from
 *	SimpleTextImager::ScrollSoShowing / MultiRowTextImager::ScrollSoShowing () - code cleanups
 *	
 *	Revision 2.67  1999/12/24 02:19:17  lewis
 *	DOCCOMMENT
 *	
 *	Revision 2.66  1999/12/23 17:23:07  lewis
 *	spr#0677 - small fix to TextImager::ComputeRelativePosition () for by-row cursoring
 *	
 *	Revision 2.65  1999/12/21 20:55:04  lewis
 *	Use new Led_ThrowIfNull instead of manual check. And lose (I HOPE) obsolete code
 *	which was checkingf for \n (as I had in MultiRowTextImager code)
 *	
 *	Revision 2.64  1999/12/21 03:35:14  lewis
 *	SPR#0670- Major revision to MappedDisplayCharacters() APIS. Updated DOCCOMMENTS on them.
 *	And since they triggered wierd ASSERT error due to funky 'gross hack' in PortableGDI_MeasureText -
 *	I commented out that hack - and things seem to work better
 *	
 *	Revision 2.63  1999/12/19 19:14:17  lewis
 *	Added new ContainsMappedDisplayCharacters/ReplaceMappedDisplayCharacters APIs - SPR#0670
 *	
 *	Revision 2.62  1999/12/18 15:50:48  lewis
 *	fix spr#0664 - Windows font sub/superscripts code was broken
 *	
 *	Revision 2.61  1999/12/14 18:11:40  lewis
 *	Preliminary qXWindows support
 *	
 *	Revision 2.60  1999/12/11 21:43:11  lewis
 *	if _UNICODE defines - then kRunning32BitGDI always assumed true (CONST)
 *	
 *	Revision 2.59  1999/12/09 03:27:40  lewis
 *	SPR#0645 - preliminary qSDK_UNICODE (_UNICODE) support (mostly using new
 *	Led_SDK_String/Led_SDK_Char)
 *	
 *	Revision 2.58  1999/11/30 20:25:03  lewis
 *	Add TextImager::EraseBackground_SolidHelper can call from TextImager::EraseBackground
 *	
 *	Revision 2.57  1999/11/29 21:10:59  lewis
 *	PatPtr SB const Pattern* for MacOS Wrapper functions.
 *	
 *	Revision 2.56  1999/11/29 20:48:47  lewis
 *	Use Led_GetCurrentGDIPort () instead of peeking at 'qd' global (and kBlack constnat).
 *	SPR#0634 (MACONLY).
 *	
 *	Revision 2.55  1999/11/15 21:33:11  lewis
 *	Moved  AddRectangleToRegion to LedGDI module. React to several API changes so this
 *	code compiles on PC without ANY MFC header includes - so can build without MFC.
 *	
 *	Revision 2.54  1999/11/13 16:32:24  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.53  1999/08/28 02:39:21  lewis
 *	cleanup code- remove obsolete ifdefs
 *	
 *	Revision 2.52  1999/06/28 15:16:53  lewis
 *	spr#0598- use mutable instead of const_cast, as appopropriate
 *	
 *	Revision 2.51  1999/05/03 22:05:19  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.50  1999/05/03 21:41:35  lewis
 *	Misc minor COSMETIC cleanups - mostly deleting ifdefed out code
 *	
 *	Revision 2.49  1999/04/13 19:11:55  lewis
 *	DOCCOMMENTS
 *	
 *	Revision 2.48  1999/02/26 22:38:57  lewis
 *	add bug workaround define qWorkAroundWin98UNICODECharImagingBugs - to cover the Win98
 *	GDI bugs which happen under Win98, and separate out the ones that are SPECIFICLY Win95
 *	ones. ALSO - add workaround for inability to image SJIS characters in UNICODE under Win95/98,
 *	via a GROSS HACK, specific to that code page (but hopefully sufficient). Many thanx to Sterl
 *	for finding this hack, and evaluating what is needed under each OS
 *	
 *	Revision 2.47  1999/02/21 22:02:22  lewis
 *	fix mac-specific compile bug
 *	
 *	Revision 2.46  1999/02/21 21:48:33  lewis
 *	fix subtle mac-only bug where we sometimes got called (temporarily during init) with
 *	a memset(0) fontspec. Would then match our default one, and return uninitialised fCachedFontMetrics.
 *	Add bool to say if inited for mac, to parallel the test for NULL of the fCachedFont object for WINDOZE
 *	
 *	Revision 2.45  1999/02/12 01:27:54  lewis
 *	DOCCOMMENTS
 *	
 *	Revision 2.44  1998/10/30 14:35:43  lewis
 *	Lots of misc cleanups. New MSVC60 warnign support. Deleted lots of ifdefed out code.
 *	Not sure what else...
 *	
 *	Revision 2.43  1998/07/24  01:13:57  lewis
 *	Massive UNICODE / Win95 bug work around fixes.
 *	Win32CharSetToCodePage/qWorkAroundWin95UNICODECharImagingBugs stuff.
 *	TextImager::GetStaticDefaultFont (BYTE charSet) support.
 *
 *	Revision 2.42  1998/06/03  01:53:03  lewis
 *	*** empty log message ***
 *
 *	Revision 2.41  1998/04/25  17:33:31  lewis
 *	Change GetStaticDefaultFont() to ignore fIsRoman, but to add new test that uses
 *	charset of the ANSI-font (not same
 *	as ANSI_CHARSET - I think/hope - we'll see when we test on Jap Windows...
 *
 *	Revision 2.40  1998/04/25  01:35:30  lewis
 *	UNICODE support - Win32_GetTextExtentPint(), etc - that vector to W/A versions
 *	according to OUR qWideCharacters flag, instead
 *	of the Win32SDK UNICODE flag.
 *	Also - generally avoid MFC (still a few MFCisn'ms here) - and call Win32 functions directly (mostly did this so would
 *	work with UNICODE - but later remove rest of MFC stuff so we can more easily work with other class libs).
 *	Changed (not really tested well - maybe for wosrt - investigate/play with it) - the code which calcs the default font.
 *	STILL haven't come up with a good algorithm for this!!! After all these years!
 *	Anyhow - now at least doesnt prefer/insist on ANSI_CHARSET, or "Ariel" - not good choices for Japanese users (LEC).
 *
 *	Revision 2.39  1998/03/04  20:20:20  lewis
 *	New LogFontsEqual() tester function, instead of comparing whole Led_Font... guy - so we avoid comparing color of text,
 *	which isn't in logfont.
 *
 *	Revision 2.38  1997/12/24  04:40:13  lewis
 *	*** empty log message ***
 *
 *	Revision 2.37  1997/12/24  03:33:02  lewis
 *	*** empty log message ***
 *
 *	Revision 2.36  1997/09/29  15:40:01  lewis
 *	Lose qLedFirstIndex support.
 *	Small changes for compat with justification code (remove hidden left justication assumptionms
 *	in the windowlocation code).
 *
 *	Revision 2.35  1997/07/27  15:58:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.34  1997/07/13  02:21:16  lewis
 *	Renamed TextImager_ to TextImager.
 *	Fixed qLedFirstIndex = 0 support.
 *
 *	Revision 2.33  1997/06/24  03:33:13  lewis
 *	Lose concept of LayoutWidth (see spr#0450). Code which checked for LayoutWidth on RHS of now uses WindowRect.right
 *	instead.
 *	ComputeMaxHScrollPos () now defaults to ZERO (since it doesn;t ahve concept of layoutwidth cannot use that).
 *
 *	Revision 2.32  1997/06/23  16:18:11  lewis
 *	Speed tweek TextImager::GetStaticDefaultFont () - cache its result, and only compute once.
 *	Big speed tweek for reading RTF files on windows.
 *
 *	Revision 2.31  1997/06/18  20:08:49  lewis
 *	*** empty log message ***
 *
 *	Revision 2.30  1997/06/18  03:15:09  lewis
 *	Lose !qUseAnisotropicModeForWindowsPrinting.
 *	qIncludePrefixFile
 *	Work around Win95 print bug with MS print drivers (SPR#0435).
 *	ResetTabStops/CalcSegmentSize () moved to PartitioningTextImager.
 *	Lose SetTabStopList () and instead make GetTabStopList () a virtual, and give it a markerpos arg.
 *	Redo default font calculation. Now much more elaborate (for windows only).
 *	Support sub/superscripts.
 *
 *	Revision 2.29  1997/03/23  00:44:56  lewis
 *	Lose old OpenDoc code. Minor changes for MSVC50
 *	more.
 *
 *	Revision 2.28  1997/03/04  20:09:21  lewis
 *	Made TextImager_::FontCacheInfoUpdater usable outside this file.
 *	Added TextImager_::EraseBackground().
 *
 *	Revision 2.27  1997/01/20  05:27:47  lewis
 *	Support colored text, and Led_Color class changes.
 *
 *	Revision 2.26  1997/01/10  03:13:42  lewis
 *	New horizontal scrolling support (prelimianry).
 *	Lose qSupportTabStops flag - assume always true.
 *	New horizontal scrolling support and revised LayoutWidth support.
 *	SetWindowRect () now sets the layoutwidth, and setlayou;idth notifiers of
 *	scrollbar changes.
 *	CalculateLongestRowInWindowPixelWidth () added.
 *	more scrolling cleanups to come...
 *
 *	Revision 2.25  1996/12/13  17:58:46  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.24  1996/12/05  19:11:38  lewis
 *	*** empty log message ***
 *
 *	Revision 2.23  1996/10/31  00:12:42  lewis
 *	Lose font size mapping crap if qUseAnisotropicModeForWindowsPrinting.
 *
 *	Revision 2.22  1996/09/30  14:29:11  lewis
 *	No more NotificationOf_Scrolling ()
 *
 *	Revision 2.21  1996/09/01  15:31:50  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.20  1996/08/05  06:04:03  lewis
 *	Add more asserts that qd.thePort == tablet (was a bug with Led_TCL).
 *	And be more paranoid about sticking in DoSetPort() calls, so the
 *	asserts never get triggered.
 *	Delete unused GetRectOfCharRange() crap. Never ended up needing it.
 *	Moved GDI_RGBForeColor/BackColor to LedSupport.hh
 *
 *	Revision 2.19  1996/07/03  01:15:34  lewis
 *	Fixed bug in Get Window Selection region code - at end of buffer - just don't test for
 *	end of buffer and code otehrwise correct.
 *
 *	Revision 2.18  1996/06/01  02:14:32  lewis
 *	Use tablet->SetBkMode (TRANSPARENT) on PC to fix overrigth of charcell bug.
 *	Fixed bug i calcing selection region (window funct - used for D&D).
 *	Experimented but failed to get better DefaultFont code.
 *	(by that I mean code to pick default font on PC).
 *	Lose no-RTTI compat code.
 *
 *	Revision 2.17  1996/05/23  20:00:32  lewis
 *	Lots of changes, incluiding using new amountDrawn optional parameter instead of
 *	areaLeft.
 *	Lose old qUsingNewEraserIMCode==0 code.
 *	Led_FontSpecification instead of FontSpecification.
 *	1/3 default tabstop.
 *	(note Led_FontSpecification change more than just a name, lots of code changes to using it).
 *
 *	Revision 2.16  1996/05/14  20:30:53  lewis
 *	Mostly added support for qUsingNewEraserIMCode.
 *	Also added windows-only GetTextExtent() hack to end of textout routine
 *	to fixup offsets (maybe get rid of this in the future, but needed for now
 *	so italics lines up in textou;t and measuretext calls).
 *	Now more Tablet_Acquirer tablet (this) in DrawSegment - instead use
 *	passed in tablet (so we cna do offscreen tablets).
 *	New GetWindowRectOfCharRange etc.
 *	Cleanup HookLosingTextStore etc stuff.
 *
 *	Revision 2.15  1996/05/04  21:08:21  lewis
 *	Minor cleanups and annotations - thinking about SPR with italics. Going
 *	to probably have to make Draconian changes to fix the italics problem - sigh.
 *
 *	Revision 2.14  1996/04/18  15:37:37  lewis
 *	cleanups.
 *	Lose old TextImager_::CalcSegmentSize - and instead do new mechanism
 *	where we just call MeasureSegmentWidth(), and don't expect anybody to
 *	override (for correctness sake - MultiRowTextImager does an overrride
 *	merely as a speed tweek).
 *	Moved ResetTabStops () code here from WordWrappedTextImager.
 *	New TextImager_::PurgeUnneededMemory API.
 *
 *	Revision 2.13  1996/03/16  18:47:39  lewis
 *	Substantially revised/rewritten tabstop support.
 *
 *	Revision 2.12  1996/02/26  22:04:10  lewis
 *	Use Led_MinMax instead of plain versions.
 *	React to Led_Point declaration changes.
 *	fooo.SetPointSize (fooo.GetPointSize ()); hack in computing original
 *	windows font.
 *
 *	Revision 2.11  1996/02/05  04:23:08  lewis
 *	Fix windows setting of font(CreateFont call) when printing.
 *	default font on windows is ANSI_VA
 *	Redid SpecifyTextStore () stuff using HookGaining/Losting TextStore -
 *	greatly simplified all the code that hooked these actions!
 *
 *	Revision 2.10  1996/01/22  05:23:41  lewis
 *	Use Led_Verify() instead of VERIFY().
 *	Fix FontCacheUpdater::CTOR code to handling IsPrinting() case. And lose
 *	a coule Led_Verify's there cuz they get triggerd for printer DCs.
 *	Probably not a bug. SDK docs not totally clear.
 *
 *	Revision 2.9  1995/12/15  02:20:37  lewis
 *	Use new functional notation AsQDRect calls instead of methods.
 *
 *	Revision 2.8  1995/12/13  05:59:41  lewis
 *	Use ODNewRgn () instead of NewRgn () for OpenDoc.
 *
 *	Revision 2.7  1995/12/09  05:43:40  lewis
 *	TextImager_::PeekAtTextStore () is virtual, so no longer inline.
 *
 *	Revision 2.6  1995/11/25  00:28:27  lewis
 *	Hacked PC default font generation. Old code looked terrible. New maybe
 *	'wrong' in some sense. But looks better. Revisit later...
 *
 *	Revision 2.5  1995/11/04  23:18:22  lewis
 *	New MeasureBaseLine routine.
 *	Now DrawSegment takes baseLine as parameter.
 *	Do (maconly) setports less often (commneted out one I think not need).
 *	Much change to the DrawSegment_ routine to support baseLine param, and
 *	erase extra space above and below where the text is drawn (for styled text).
 *
 *	Revision 2.4  1995/11/02  22:38:50  lewis
 *	Added TextImager_::GetStaticDefaultFont () and default font set to
 *	this in CTOR. Done to vector code that was in Led_CLASLIB CTORS
 *	and needed also now creating style runs.
 *
 *	Revision 2.3  1995/10/19  22:27:15  lewis
 *	Moved alot of the WordWrappedTextImager class code here that really
 *	had nothign todo with word wrapping, but was generic image a segment
 *	or measure text widths for a segment code. Give access to that now
 *	using _ methods, and those are now called from new StyledTextImager subclass.
 *	Also, we moved the font-cache from WordWrappedTextImager down here as well,
 *	since that was used for these drawing methods.
 *
 *	Revision 2.2  1995/10/09  22:40:45  lewis
 *	On specifytextstore, we add and remove ourselves from the marker owner
 *	list maintained by the textstore.
 *	Dont call Ajustbounds - warn subclasses to in notification routine.
 *
 *	Revision 2.1  1995/09/06  20:57:00  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.17  1995/06/19  13:50:32  lewis
 *	GetEndOfRow (curRow) < to instead of <= for GetSelectionWindowRegion()
 *
 *	Revision 1.16  1995/06/12  03:06:00  lewis
 *	Fix type - Led_Coordinate instead of Led_Distance to silence compiler warning.
 *
 *	Revision 1.15  1995/06/08  05:18:40  lewis
 *	Code cleanups.
 *	Fix bug with end of last line in TextImager_::GetSelectionWindowRegion ()
 *	SPR#0322.
 *	Fix TextImager_::ComputeRelativePosition () to be pixel-goal-column
 *	oriented (SPR#0315).
 *
 *	Revision 1.14  1995/06/04  04:28:20  lewis
 *	Code cleanups for TextImager_::GetSelectionWindowRegion () - and use
 *	size_t isntead of short, and use better values for testing coords to
 *	see if we need to add to region. SPR#0312.
 *
 *	Revision 1.13  1995/06/02  07:10:46  lewis
 *	 A couple of minor fixes to TextImager_::GetSelectionWindowRegion ()
 *	to synconize it with the MultiRowTextImager one.
 *
 *	Revision 1.12  1995/06/02  06:21:14  lewis
 *	Fixed GetTextBoundingRect/GetTextWindowBoundingRect () to be more
 *	aggressive - SPR 0306.
 *
 *	Revision 1.11  1995/05/30  00:07:03  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.10  1995/05/24  07:25:19  lewis
 *	Fixed bugs with GetSelectionWindowRegion () - having todo with interline
 *	space and when to show it. Not 100% sure I got external leading right?
 *	SPR 0279.
 *
 *	Revision 1.9  1995/05/20  04:57:21  lewis
 *	Fix use of GetImageRect/GetWindowRect - See SPR 0263.
 *	And code tweek - for GetTextBoundingRect - if rhs/lhs same - calc once.
 *
 *	Revision 1.8  1995/05/18  08:12:45  lewis
 *	Fixed TextImager_::GetSelectionWindowRegion () - SPR 0251.
 *
 *	Revision 1.7  1995/05/16  06:44:24  lewis
 *	Added new TextImager_::GetSelectionWindowRegion.
 *
 *	Revision 1.6  1995/05/08  03:17:30  lewis
 *	Cleanup SetWindowRect call for SPR 0236.
 *	Fix character splittin bug on row arrow keys - SPR 0237.
 *
 *	Revision 1.5  1995/05/06  19:45:59  lewis
 *	Renamed GetByteLength->GetLength () - SPR #0232.
 *
 *	Revision 1.4  1995/03/23  03:55:59  lewis
 *	Add more assertions for SetSelection calls (and GetSelection ()).
 *
 *	Revision 1.3  1995/03/06  21:33:17  lewis
 *	Add support for eCursorByBuffer.
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

#if		qMacOS
	#include	<Script.h>
#endif

#include	"LedGDI.h"

#include	"TextImager.h"




#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif





#if		qLedUsesNamespaces
	namespace	Led {
#endif


#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (4 : 4800)		//qUsePragmaWarningToSilenceNeedlessBoolConversionWarnings
#endif









#if		qMacOS
	inline	void	GDI_TextFont (short font)
		{
			#if		TARGET_CARBON
				::TextFont (font);
			#else
				if (Led_GetCurrentGDIPort ()->txFont != font) {
					::TextFont (font);
				}
			#endif
		}
	inline	void	GDI_TextFace (short face)
		{
			#if		TARGET_CARBON
				::TextFace (face);
			#else
				if (Led_GetCurrentGDIPort ()->txFace != face) {
					::TextFace (face);
				}
			#endif
		}
	inline	void	GDI_TextMode (short mode)
		{
			#if		TARGET_CARBON
				::TextMode (mode);
			#else
				if (Led_GetCurrentGDIPort ()->txMode != mode) {
					::TextMode (mode);
				}
			#endif
		}
	inline	void	GDI_TextSize (short size)
		{
			#if		TARGET_CARBON
				::TextSize (size);
			#else
				if (Led_GetCurrentGDIPort ()->txSize != size) {
					::TextSize (size);
				}
			#endif
		}
#endif














/*
 ********************************************************************************
 *********************** TextImager::FontCacheInfoUpdater ***********************
 ********************************************************************************
 */
#if		qWindows
	inline	bool	LogFontsEqual (LOGFONT lhs, LOGFONT rhs)
		{
			size_t	bytesToCompare	=	offsetof (LOGFONT, lfFaceName) + (::_tcslen (lhs.lfFaceName) + 1) * sizeof (Led_SDK_Char);
			Led_Require (bytesToCompare <= sizeof (LOGFONT));	// else we were passed bogus LogFont (and we should validate them before here!)
			return ::memcmp (&lhs, &rhs, bytesToCompare) == 0;
		}
	inline	bool	LogFontsEqual (const Led_FontSpecification& lhs, const Led_FontSpecification& rhs)
		{
			if (lhs.GetStyle_SubOrSuperScript () == rhs.GetStyle_SubOrSuperScript ()) {
				LOGFONT	lhslf;
				lhs.GetOSRep (&lhslf);
				LOGFONT	rhslf;
				rhs.GetOSRep (&rhslf);
				return LogFontsEqual (lhslf, rhslf);
			}
			else {
				return false;
			}
		}
#endif
TextImager::FontCacheInfoUpdater::FontCacheInfoUpdater (const TextImager* imager, Led_Tablet tablet, const Led_FontSpecification& fontSpec):
	fImager (imager)
	#if		qWindows
		,fTablet (tablet),
		fRestoreObject (NULL),
		fRestoreAttribObject (NULL)
	#endif
{
	#if		qMacOS
		/*
		 *	For MAC:
		 *
		 *		Just set the font using the font-spec, and assume any 'restore' will be taken care of
		 *	at a higher level (really just ignore as long as I can, and if I need to, then
		 *	probably best to take care of here!!! (modularity vs speed).
		 */
		short	fontID		=	0;
		short	fontSize	=	0;
		Style	fontStyle	=	0;
		fontSpec.GetOSRep (&fontID, &fontSize, &fontStyle);
		tablet->SetPort ();
		GDI_TextFont (fontID);
		if (fontSpec.GetStyle_SubOrSuperScript () != Led_FontSpecification::eNoSubOrSuperscript) {
			// See SPR#1523- was 'max (fontSize/2, 1);'
			// Careful to sync this with TextImager::DrawSegment_ () 'drawTop' adjustment
			fontSize = max (fontSize * 2 / 3, 1);
		}
		GDI_TextSize (fontSize);
		GDI_TextFace (fontStyle);

		if (not imager->fCachedFontValid or fontSpec != imager->fCachedFontSpec) {
			imager->fCachedFontInfo = tablet->GetFontMetrics ();
			imager->fCachedFontSpec = fontSpec;
			imager->fCachedFontValid = true;
		}
	#elif	qWindows
		/*
		 *	For Windows:
		 *
		 *		Just set the font using the font-spec. Cache font, so we don't keep creating. But select it
		 *	in to tablet each time. And on DTOR, restore old font into tablet.
		 */
		bool	changed	=	false;
		if (imager->fCachedFont == NULL or !LogFontsEqual (fontSpec, imager->fCachedFontSpec)) {
			changed = true;
			delete imager->fCachedFont;
			imager->fCachedFont = NULL;
			imager->fCachedFont = new Led_FontObject ();
			LOGFONT	lf;
			fontSpec.GetOSRep (&lf);
			if (fontSpec.GetStyle_SubOrSuperScript () != Led_FontSpecification::eNoSubOrSuperscript) {
				// See SPR#1523- was 'lf.lfHeight /= 2'
				// Careful to sync this with TextImager::DrawSegment_ () 'drawTop' adjustment
				lf.lfHeight = lf.lfHeight * 2 / 3;
				if (lf.lfHeight == 0) {
					lf.lfHeight = 1;
				}
			}
			Led_Verify (imager->fCachedFont->CreateFontIndirect (&lf));
			imager->fCachedFontSpec = fontSpec;
		}
		Led_AssertNotNil (imager->fCachedFont);

		// See CDC::SelectObject for the logic..., but we do better than thiers and restore
		// right object to right DC!!!! - LGP 950525
		if (tablet->m_hDC != tablet->m_hAttribDC) {
			fRestoreObject = ::SelectObject (tablet->m_hDC, imager->fCachedFont->m_hObject);
			// At one point i had Led_Asserts() here that fRestoreObject != NULL - but at least for PrintDC's, apparently
			// fRestoreObject can be NULL, and apparently it isn't a problem...
		}
		if (tablet->m_hAttribDC != NULL) {
			fRestoreAttribObject = ::SelectObject (tablet->m_hAttribDC, imager->fCachedFont->m_hObject);
			// At one point i had Led_Asserts() here that fRestoreAttribObject != NULL - but at least for PrintDC's, apparently
			// fRestoreAttribObject can be NULL, and apparently it isn't a problem...
		}
		if (changed) {
			imager->fCachedFontInfo = tablet->GetFontMetrics ();
		}
	#elif	qXWindows
		tablet->SetFont (fontSpec);
		imager->fCachedFontInfo = tablet->GetFontMetrics ();
	#endif
}






/*
 ********************************************************************************
 ***************************** TextImager::HilightMarker ************************
 ********************************************************************************
 */
TextImager::HilightMarker::HilightMarker ():
	Marker ()
{
}










/*
 ********************************************************************************
 ************************************** TextImager ******************************
 ********************************************************************************
 */
TextImager::TextImager ():
	fTextStore (NULL),
	fDefaultFont (GetStaticDefaultFont ()),
	fForceAllRowsShowing (true),
	fImageUsingOffscreenBitmaps (qUseOffscreenBitmapsToReduceFlicker),
	fHScrollPos (0),
	fSupressGoalColumnRecompute (false),
	fSelectionGoalColumn (Led_TWIPS (0)),
	fUseEOLBOLRowHilightStyle (true),
	fSelectionShown (false),
	fWindowRect (Led_Rect (0, 0, 0, 0)),
	fHiliteMarker (NULL),
	fWeAllocedHiliteMarker (false),
	//	fDefaultColorIndex (),
	fCachedFontSpec (),
	fCachedFontInfo (),
#if		qWindows
	fCachedFont (NULL)
#else
	fCachedFontValid (false)
#endif
{
	for (Led_Color** i = &fDefaultColorIndex[0]; i < &fDefaultColorIndex[eMaxDefaultColorIndex]; ++i) {
		*i = NULL;
	}
}

TextImager::~TextImager ()
{
	Led_Require (fTextStore == NULL);
	Led_Require (fHiliteMarker == NULL);
	for (Led_Color** i = &fDefaultColorIndex[0]; i < &fDefaultColorIndex[eMaxDefaultColorIndex]; ++i) {
		delete *i;
		*i = NULL;
	}
#if		qWindows
	delete fCachedFont;
#endif
}

TextStore*	TextImager::PeekAtTextStore () const
{
	return fTextStore;		// Can return NULL if no markers owned
}

/*
@METHOD:		TextImager::SpecifyTextStore
@DESCRIPTION:	<p>Associate this TextImager with the given @'TextStore'.</p>
		<p>Note that this will - as a side-effect - call
	@'TextStore::AddMarkerOwner'. So be sure that this @'TextImager' has not yet been added as a @'MarkerOwner' for
	any other (or this given) TextStore.</p>
*/
void	TextImager::SpecifyTextStore (TextStore* useTextStore)
{
	if (fTextStore != useTextStore) {
		if (fTextStore != NULL) {
			HookLosingTextStore ();
		}
		fTextStore = useTextStore;
		if (fTextStore != NULL) {
			HookGainedNewTextStore ();
		}
	}
}

void	TextImager::HookLosingTextStore ()
{
	HookLosingTextStore_ ();
}

void	TextImager::HookLosingTextStore_ ()
{
	// BE MORE CAREFUL HERE - NO NEED TO DELETE HILIGHT MARKER - JUST REMOVE AND RE_ADD!!!
	Led_AssertNotNil (fTextStore);
	if (fHiliteMarker != NULL) {
		Led_Assert (fWeAllocedHiliteMarker);	// otherwise setter better have unset!!!
		GetTextStore ().RemoveMarker (fHiliteMarker);
		delete fHiliteMarker;
		fHiliteMarker = NULL;
		fTextStore->RemoveMarkerOwner (this);
	}
}

void	TextImager::HookGainedNewTextStore ()
{
	HookGainedNewTextStore_ ();
}

void	TextImager::HookGainedNewTextStore_ ()
{
	Led_AssertNotNil (fTextStore);
	if (fHiliteMarker == NULL) {
		fTextStore->AddMarkerOwner (this);
		SetHilightMarker (NULL);			// forces creation of default one...
	}
}

/*
@METHOD:		TextImager::PurgeUnneededMemory
@DESCRIPTION:	<p>Call when you want to have the text engine save a little bit of memory. Frees up data
			that isn't required. This doesn't NEED to be called, but can be called by memory-restricted applications.
			The default implementation calls @'TextImager::InvalidateAllCaches'.
			</p>
*/
void	TextImager::PurgeUnneededMemory ()
{
	InvalidateAllCaches ();
}

/*
@METHOD:		TextImager::InvalidateAllCaches
@DESCRIPTION:	<p>This is called - for example - when you change the wrap width for the entire document, or font information
			for the entire document, or maybe something about the @'Led_Tablet' metrics you are imaging to.
			TextImager::InvalidateAllCaches is called automatically from @'MultiRowTextImager::TabletChangedMetrics ()'
			</p>
*/
void	TextImager::InvalidateAllCaches ()
{
	// Classes which cache font-based information must override and invalidate it...
	#if		qWindows
		delete fCachedFont;
		fCachedFont = NULL;
	#else
		fCachedFontValid = false;
	#endif
}

/*
@METHOD:		TextImager::SetDefaultFont
@DESCRIPTION:	<p>Sets the default font associated with the given imager. This is NOT necessarily the one you will
			see displayed, as the font displayed maybe overriden by more specific font information from
			@'StyledTextImager::StyleMarker' or @'StandardStyledTextImager'.
			</p>
				<p>Note that the semantics of SetDefaultFontChanged in 3.1a4. If you want to set the font
			of a specific range of styled text (e.g. in a word-processor class) - then you may want to
			use @'StandardStyledTextInteractor::InteractiveSetFont'.
			</p>
*/
void	TextImager::SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont)
{
	SetDefaultFont_ (defaultFont);
}

void	TextImager::SetDefaultFont_ (const Led_IncrementalFontSpecification& defaultFont)
{
	if (PeekAtTextStore () != NULL) {
		TextStore::SimpleUpdater	u	(GetTextStore (), 0, GetTextStore ().GetEnd (), false);	// update buffer so cached measurement values refreshed
		fDefaultFont.MergeIn (defaultFont);
		if (defaultFont.GetTextColor_Valid ()) {
			SetDefaultTextColor (eDefaultTextColor, defaultFont.GetTextColor ());
		}
	}
	else {
		fDefaultFont.MergeIn (defaultFont);
		if (defaultFont.GetTextColor_Valid ()) {
			SetDefaultTextColor (eDefaultTextColor, defaultFont.GetTextColor ());
		}
	}
}

	#if		qWindows
	struct	FontSelectionInfo {
		FontSelectionInfo (BYTE desiredCharset):
			fDesiredCharset (desiredCharset),
			fUsesBestCharset (false),
			fIsTT (false),
			fIsANSI_VAR_Font (false),
			fIsGoodBackupCharset (false),
			fIsFavoriteForCharset (false),
			fIsVariablePitch (false),
			fStartsWithAt (false)
			{
				memset (&fBestFont,0,sizeof(fBestFont));
			}
		BYTE	fDesiredCharset;
		LOGFONT	fBestFont;
		bool	fUsesBestCharset;
		bool	fIsTT;
		bool	fIsANSI_VAR_Font;	// a good second choice if Arial not present
		bool	fIsGoodBackupCharset;
		bool	fIsFavoriteForCharset;
		bool	fIsVariablePitch;
		bool	fStartsWithAt;
		inline	int	Score () const
			{
				int	score	=	0;
				if (fUsesBestCharset) {
					score += 10;			// underweight - because (at least with Win2K) - the fCharset field almost always set to zero - so cannot be used reliably
				}
				if (fIsTT) {
					score += 20;
				}
				if (fIsANSI_VAR_Font) {
					score += 4;
				}
				if (fIsGoodBackupCharset) {
					score += 2;
				}
				if (fIsFavoriteForCharset) {
					score += 25;
				}
				if (fIsVariablePitch) {
					//unsure if this is desirable or not - good for US - but not sure about Japan?
					score += 2;
				}
				if (fStartsWithAt) {
					score -= 25;			// we don't really support vertical fonts
				}
				return score;
			}
	};
	static	BOOL	FAR	PASCAL	EnumFontCallback (const LOGFONT* lplf, const TEXTMETRIC* /*lpntm*/, DWORD fontType, long arg)
		{
			// Score each font choice, and pick the 'best' one. Pick randomly if several are 'best'.
			Led_RequireNotNil (lplf);
			FontSelectionInfo&	result	=	*(FontSelectionInfo*)arg;

			static	LOGFONT	theANSILogFont;
			if (theANSILogFont.lfFaceName[0] == '\0') {
				HFONT	xxx	=	HFONT (::GetStockObject (ANSI_VAR_FONT));
				Led_Verify (::GetObject (xxx, sizeof theANSILogFont, &theANSILogFont));
			}

			FontSelectionInfo	potentialResult	=	result;
			memcpy (&potentialResult.fBestFont, lplf, sizeof (LOGFONT));

			if (potentialResult.fDesiredCharset == DEFAULT_CHARSET) {
				potentialResult.fUsesBestCharset = bool (lplf->lfCharSet == theANSILogFont.lfCharSet);
			}
			else {
				potentialResult.fUsesBestCharset = bool (lplf->lfCharSet == potentialResult.fDesiredCharset);
			}
			potentialResult.fIsTT = bool (fontType & TRUETYPE_FONTTYPE);
			potentialResult.fIsANSI_VAR_Font = ::_tcscmp (lplf->lfFaceName, theANSILogFont.lfFaceName) == 0;
			potentialResult.fIsGoodBackupCharset = (lplf->lfCharSet == DEFAULT_CHARSET or lplf->lfCharSet == theANSILogFont.lfCharSet);
			{
				switch (potentialResult.fDesiredCharset) {
					case	SHIFTJIS_CHARSET: {
						if (
								 potentialResult.fBestFont.lfFaceName == Led_SDK_String (Led_SDK_TCHAROF ("MS P Gothic")) or
								 potentialResult.fBestFont.lfFaceName == Led_SDK_String (Led_SDK_TCHAROF ("MS Gothic")) or
								 potentialResult.fBestFont.lfFaceName == Led_SDK_String (Led_SDK_TCHAROF ("MS PGothic"))
							) {
							potentialResult.fIsFavoriteForCharset = true;
						}
					}
					break;
					case	CHINESEBIG5_CHARSET: {
						if (
								 potentialResult.fBestFont.lfFaceName == Led_SDK_String (Led_SDK_TCHAROF ("MS HEI")) or
								 potentialResult.fBestFont.lfFaceName == Led_SDK_String (Led_SDK_TCHAROF ("MingLiU"))
								) {
							potentialResult.fIsFavoriteForCharset = true;
						}
					}
					break;
				}
			}
			potentialResult.fIsVariablePitch = lplf->lfPitchAndFamily & VARIABLE_PITCH;
			potentialResult.fStartsWithAt = lplf->lfFaceName[0] == '@';


			if (potentialResult.Score () > result.Score ()) {
				result = potentialResult;
			}
			return 1;
		}
	#endif
Led_FontSpecification	TextImager::GetStaticDefaultFont ()
{
	//	Since this can be called alot, and since its value shouldn't change during the lifetime
	//	of Led running, cache the result (and since on windows - at least - it is expensive to compute)
	static	bool					sDefaultFontValid	=	false;
	static	Led_FontSpecification	sDefaultFont;
	if (not sDefaultFontValid) {
		#if		qMacOS
			sDefaultFont.SetFontNameSpecifier (::GetScriptVariable (smCurrentScript, smScriptAppFond));
			sDefaultFont.SetPointSize (::GetScriptVariable (smCurrentScript, smScriptAppFondSize));
			sDefaultFont.SetStyle_Plain ();
		#elif	qWindows
			sDefaultFont = GetStaticDefaultFont (DEFAULT_CHARSET);
		#elif	qXWindows
			{
				sDefaultFont.SetFontNameSpecifier ("times");
				sDefaultFont.SetPointSize (12);
			}
		#endif
		sDefaultFont.SetTextColor (Led_GetTextColor ());
		sDefaultFontValid = true;
	}
	return (sDefaultFont);
}

#if		qWindows
Led_FontSpecification	TextImager::GetStaticDefaultFont (BYTE charSet)
{
	Led_FontSpecification	defaultFont;
	//nb: import to go through the intermediary font so we don't set into the
	// LOGFONT lots of fields which are part of the chosen font but are
	// extraneous, and then mess up later choices to change to face name.
	//
	// Eg., if our default font is BOLD, that will result in a big weight# being part of
	// the logFont. But then if the user picks a different face name, we don't want it
	// to stay bold. Maybe BOLD is a bad example cuz that DOES show up in our UI (menu of
	// font attriobutes). But pick one attribute (width? or escarpment) which doesn't show
	// up in our choice lists, and yet gets caried along even after you change face names.
	//
	// See spr# 0426 for more details.
	Led_FontSpecification	fooo;
	FontSelectionInfo		selectedFont (charSet);
	Led_WindowDC			screenDC (NULL);
	#if		STRICT
		::EnumFontFamilies (screenDC.m_hDC, NULL, EnumFontCallback, long (&selectedFont));
	#else
		::EnumFontFamilies (screenDC.m_hDC, NULL, reinterpret_cast<FONTENUMPROC> (EnumFontCallback), long (&selectedFont));
	#endif
	fooo.LightSetOSRep (selectedFont.fBestFont);

	// EnumFontFamilies seems to pick very bad sizes. Not sure why. No biggie. This works
	// pretty well. LGP 970613
	{
		static	LOGFONT	theANSILogFont;
		if (theANSILogFont.lfFaceName[0] == '\0') {
			HFONT	xxx	=	HFONT (::GetStockObject (ANSI_VAR_FONT));
			Led_Verify (::GetObject (xxx, sizeof theANSILogFont, &theANSILogFont));
		}
		fooo.SetPointSize (min (max (Led_FontSpecification (theANSILogFont).GetPointSize (), static_cast<unsigned short> (8)), static_cast<unsigned short> (14)));
	}

	defaultFont.MergeIn (fooo);
	defaultFont.SetTextColor (Led_GetTextColor ());
	return (defaultFont);
}
#endif

/*
@METHOD:		TextImager::GetDefaultSelectionFont
@DESCRIPTION:	<p>Some applications - such as updating the font of the IME - need to know the
			current selections font. For some @'TextImager's this is easy - its just the font used
			for the entire widget (or default implementation). But for others - say an imager with 
			multiple runs of fonts - it can be ambiguous. There is no strictly <em>right</em> answer
			for what to return if there are overlapping runs of font information. Just return the best
			fit possible.
				</p>
				<p>The default implementation just returns @'TextImager::GetDefaultFont', but 
			should be subclassed by @'StandardStyledTextImager::GetDefaultSelectionFont' to reflect just
			the selection's current font.
			</p>
*/
Led_FontSpecification	TextImager::GetDefaultSelectionFont () const
{
	return GetDefaultFont ();
}

void	TextImager::SetSelectionShown (bool shown)
{
	fSelectionShown = shown;
}

/*
@METHOD:		TextImager::GetTabStopList
@DESCRIPTION:	<p>Return the tabstop list (@'TextImager::TabStopList') active at a particular position in the text. Typically this will
	apply to an entire region (often a paragraph/partitionmarker). And we only need specify
	one point in that range.</p>
		<p>Override this to provide a different tabstop list. The default is a @'TextImager::SimpleTabStopList' of width
	roughly 1/3 of an inch.</p>
*/
const TextImager::TabStopList&	TextImager::GetTabStopList (size_t /*containingPos*/) const
{
	// 1/3 inch tabstops by default (roughly 4 chars wide on Mac in Courier 10-point)
	static	SimpleTabStopList	sDefaultTabStopList =	SimpleTabStopList (Led_TWIPS (1440/3));
	return sDefaultTabStopList;
}

/*
@METHOD:		TextImager::SetWindowRect
@DESCRIPTION:	<p>See also @'TextImager::GetWindowRect'.
*/
void	TextImager::SetWindowRect (const Led_Rect& windowRect)
{
	SetWindowRect_ (windowRect);
}

/*
@METHOD:		TextImager::ScrollSoShowingHHelper
@DESCRIPTION:	<p>You probably should NOT call this directly. This is a helper to share code in implementing
	@'TextImager::ScrollSoShowing' in subclasses.</p>
*/
void	TextImager::ScrollSoShowingHHelper (size_t markerPos, size_t andTryToShowMarkerPos)
{
	Led_Coordinate	maxHScrollPos	=	ComputeMaxHScrollPos ();
	Led_Coordinate	hsp				=	GetHScrollPos ();

	/*
	 *	Speed tweek - avoid alot of computations which are unneeded if this is true.
	 */
	if (maxHScrollPos == 0) {
		if (hsp != 0) {
			SetHScrollPos (0);
		}
		return;
	}

	Led_Rect		windowRect		=	GetWindowRect ();

	{
		// Try to see if we can accomodate the 'andTryToShowMarkerPos'.
		Led_Rect		andTryRRR				=	GetCharWindowLocation (andTryToShowMarkerPos);
		Led_Coordinate	whereAtInGlobalCoords	=	windowRect.left - andTryRRR.left;
		if (andTryRRR.left < windowRect.left) {
			Led_Assert (hsp >= whereAtInGlobalCoords);
			hsp	-=	whereAtInGlobalCoords;
		}
		else if (andTryRRR.right > windowRect.right) {
			Led_Coordinate	howFarOffRight	=	andTryRRR.right - windowRect.right;
			hsp += howFarOffRight;	// now the char should be just barely showing.
			hsp = Led_Min (hsp, maxHScrollPos);
		}
	}
	Led_Assert (hsp >= 0);
	Led_Assert (hsp <= maxHScrollPos);

	{
		// Make sure the 'markerPos' is shown. Do this second - in case there is a conflict between 'markerPos' and 'andTryToShowMarkerPos'
		Led_Rect		rrr				=	GetCharWindowLocation (markerPos);

		{
			Led_Coordinate	adjustRRRBy	=	GetHScrollPos () - hsp;
			rrr += Led_Point (0, adjustRRRBy);
		}

		Led_Coordinate	whereAtInGlobalCoords	=	windowRect.GetLeft () - rrr.GetLeft ();
		if (rrr.GetLeft () < windowRect.GetLeft ()) {
			Led_Assert (hsp >= whereAtInGlobalCoords);
			hsp	-=	whereAtInGlobalCoords;
		}
		else if (rrr.GetRight () > windowRect.GetRight ()) {
			Led_Coordinate	howFarOffRight	=	rrr.GetRight () - windowRect.GetRight ();
			hsp += howFarOffRight;	// now the char should be just barely showing.
			hsp = Led_Min (hsp, maxHScrollPos);
		}
	}

	Led_Assert (hsp >= 0);
	Led_Assert (hsp <= maxHScrollPos);
	SetHScrollPos (hsp);
}

void	TextImager::SetHScrollPos (Led_Coordinate hScrollPos)
{
	if (hScrollPos != GetHScrollPos ()) {
		SetHScrollPos_ (hScrollPos);
		InvalidateScrollBarParameters ();
	}
}

/*
@METHOD:		TextImager::ComputeMaxHScrollPos
@DESCRIPTION:	<p>This routine is used for horizontal scrolling (though not from within this class).
			It is mainly called by @'TextInteractor' or @'Led_Win32' etc methods to handle display/positioning
			of the scrollbar. Subclasses can override this to implement whatever horizontal scrolling
			they might want to.
			</p>
				<p>A plausible override
			of this routine might return (roughly - taking care of min'ing out to zero)
			<code>CalculateLongestRowInWindowPixelWidth () - GetWindowRect ().GetWidth ()</code>
			or perhaps even better:
			<code>CalculateLongestRowInDocumentPixelWidth () - GetWindowRect ().GetWidth ()</code>
			</p>
				<p>Some subclasses - such as @'WordProcessor' - already contain their own implementation
			(@'WordProcessor::ComputeMaxHScrollPos').
			</p>
*/
Led_Distance	TextImager::ComputeMaxHScrollPos () const
{
	return 0;
}

/*
@METHOD:		TextImager::CalculateLongestRowInWindowPixelWidth
@DESCRIPTION:	<p>This is a utility methods, very handy for implementing horizontal scrolling.
	It can (and should be) overriden in certain subclasses for efficiency. But the default implementation will work.</p>
*/
Led_Distance	TextImager::CalculateLongestRowInWindowPixelWidth () const
{
	size_t	startOfWindow	=	GetMarkerPositionOfStartOfWindow ();
	size_t	endOfWindow		=	GetMarkerPositionOfEndOfWindow ();

	Led_Distance	longestRowWidth	=	0;
	for (size_t curOffset = startOfWindow; curOffset < endOfWindow; ) {
		Led_Distance	thisRowWidth	=	CalcSegmentSize (curOffset, GetEndOfRowContainingPosition (curOffset));
		longestRowWidth = Led_Max (longestRowWidth, thisRowWidth);
		{
			size_t	newOffset = GetStartOfNextRowFromRowContainingPosition (curOffset);
			if (newOffset <= curOffset) {
				break;		// can happen at end of doc...
			}
			curOffset = newOffset;
		}
	}
	return longestRowWidth;
}

/*
@METHOD:		TextImager::TabletChangedMetrics
@DESCRIPTION:	<p>Call this method when something external, Led cannot detect, has happened to the tablet which would
	invalidate any information the @'TextImager' has cached. This is called automaticly, internal to Led, by anything
	Led knows about which would change the metrics.</p>
*/
void	TextImager::TabletChangedMetrics ()
{
	InvalidateAllCaches ();
}

void	TextImager::SetSelection (size_t start, size_t end)
{
	Led_Assert (start >= 0);
	Led_Assert (end <= GetEnd ());		// char 1 between positions 1..2
#if		qMultiByteCharacters
	Assert_CharPosDoesNotSplitCharacter (start);
	Assert_CharPosDoesNotSplitCharacter (end);
#endif

	#if		0
		LedDebugTrace ("TextImager::SetSelection (this= 0x%x, this_class = %s, oldSelStart=%d, oldSelEnd=%d, newSelStart=%d, newSelEnd=%d)\n",
						this, typeid (*this).name (), GetSelectionStart (), GetSelectionEnd (), start, end
					);
	#endif

	if (start != GetSelectionStart () or end != GetSelectionEnd ()) {
		SetSelection_ (start, end);
#if		qSupportLed30CompatAPI
		NotificationOf_SelectionChanged ();
#endif
	}
}

size_t	TextImager::GetSelectionStart () const
{
	Led_RequireNotNil (PeekAtTextStore ());	// Must specify TextStore before calling this, or any routine that calls it.
#if		qMultiByteCharacters
	Assert_CharPosDoesNotSplitCharacter (fHiliteMarker->GetStart ());
#endif
	return (fHiliteMarker->GetStart ());
}

size_t	TextImager::GetSelectionEnd () const
{
	Led_RequireNotNil (PeekAtTextStore ());	// Must specify TextStore before calling this, or any routine that calls it.
#if		qMultiByteCharacters
	Assert_CharPosDoesNotSplitCharacter (fHiliteMarker->GetEnd ());
#endif
	return (fHiliteMarker->GetEnd ());
}

void	TextImager::GetSelection (size_t* start, size_t* end) const
{
	Led_RequireNotNil (PeekAtTextStore ());	// Must specify TextStore before calling this, or any routine that calls it.
	Led_AssertNotNil (start);
	Led_AssertNotNil (end);
	fHiliteMarker->GetRange (start, end);
#if		qMultiByteCharacters
	Assert_CharPosDoesNotSplitCharacter (*start);
	Assert_CharPosDoesNotSplitCharacter (*end);
#endif
}

void	TextImager::SetSelection_ (size_t start, size_t end)
{
	Led_Require (start >= 0);
	Led_Require (end <= GetEnd ());
	Led_Require (start <= end);
	GetTextStore ().SetMarkerRange (fHiliteMarker, start, end);
}

void	TextImager::SetHilightMarker (HilightMarker* newHilightMarker)
{
	size_t	start	=	0;
	size_t	end		=	0;
	if (fHiliteMarker != NULL) {
		fHiliteMarker->GetRange (&start, &end);
		GetTextStore ().RemoveMarker (fHiliteMarker);
		if (fWeAllocedHiliteMarker) {
			delete fHiliteMarker;
		}
	}
	fHiliteMarker = newHilightMarker;
	fWeAllocedHiliteMarker = bool (fHiliteMarker == NULL);
	if (fHiliteMarker == NULL) {
		fHiliteMarker = new HilightMarker ();
	}
	Led_AssertNotNil (fHiliteMarker);
	GetTextStore ().AddMarker (fHiliteMarker, start, end-start, this);
}

#if		qSupportLed30CompatAPI
void	TextImager::NotificationOf_SelectionChanged ()
{
}
#endif

/*
@METHOD:		TextImager::RecomputeSelectionGoalColumn
@DESCRIPTION:	<p></p>
*/
void	TextImager::RecomputeSelectionGoalColumn ()
{
	if (not fSupressGoalColumnRecompute) {
		// We now maintain a goal-column-target using pixel offsets within the row, rather than
		// character offsets, cuz thats what LEC/Alan Pollack prefers, and I think most
		// Texteditors seem todo likewise - LedSPR#0315
		Led_Distance	lhs	=	0;
		Led_Distance	rhs	=	0;
		GetRowRelativeCharLoc (GetSelectionStart (), &lhs, &rhs);
		SetSelectionGoalColumn (Tablet_Acquirer (this)->CvtToTWIPSH (lhs + (rhs-lhs)/2));
	}
}

/*
@METHOD:		TextImager::ComputeRelativePosition
@DESCRIPTION:	<p></p>
*/
size_t	TextImager::ComputeRelativePosition (size_t fromPos, CursorMovementDirection direction, CursorMovementUnit movementUnit)
{
	/*
	 *	Handle all the different cases of movement directions (back, forward etc) and units (by char, word etc).
	 *	Take the given starting point, and produce no side effects - returning the new resulting position.
	 */
	switch (direction) {
		case	eCursorBack: {
			switch (movementUnit) {
				case	eCursorByChar: {
					return (FindPreviousCharacter (fromPos));
				}
				break;

				case	eCursorByWord: {
					return (GetTextStore ().FindFirstWordStartStrictlyBeforePosition (fromPos));
				}
				break;

				case	eCursorByRow: {
					size_t	startOfStartRow	=	GetStartOfRowContainingPosition (fromPos);
					size_t	startOfPrevRow	=	GetStartOfPrevRowFromRowContainingPosition (fromPos);
					if (startOfStartRow == startOfPrevRow) {
						// no change
						return (fromPos);
					}
					else {
						return GetRowRelativeCharAtLoc (Tablet_Acquirer (this)->CvtFromTWIPSH (GetSelectionGoalColumn ()), startOfPrevRow);
					}
				}
				break;

				case	eCursorByLine: {
					size_t	fromLine	=	GetTextStore ().GetLineContainingPosition (fromPos);
					size_t	newLine	=	(fromLine > 0)? fromLine-1: 0;
					if (newLine == fromLine) {
						// no change
						return (fromPos);
					}
					else {
						// try to maintain the same horizontal position across lines
						size_t	positionInLine	=	fromPos - GetTextStore ().GetStartOfLine (fromLine);	// ZERO RELATIVE
						Led_Assert (positionInLine <= GetTextStore ().GetLineLength (fromLine));
						positionInLine = Led_Min (positionInLine, GetTextStore ().GetLineLength (newLine));		// don't go past end of new line...
						#if		qMultiByteCharacters
							// Don't split a mbyte character
							Led_SmallStackBuffer<Led_tChar>	buf (positionInLine);
							CopyOut (GetTextStore ().GetStartOfLine (newLine), positionInLine, buf);
							if (Led_FindPrevOrEqualCharBoundary (buf, buf + positionInLine) != buf+positionInLine) {
								Led_Assert (positionInLine > 0);
								positionInLine--;
							}
						#endif
						return (GetTextStore ().GetStartOfLine (newLine) + positionInLine);
					}
				}
				break;

				case	eCursorByWindow: {
					Led_Assert (false);	// nyi
				}
				break;

				case	eCursorByBuffer: {
					Led_Assert (false);	// makes no sense - use toStart...
				}
				break;

				default:	Led_Assert (false);
			}
		}
		break;

		case	eCursorForward: {
			switch (movementUnit) {
				case	eCursorByChar: {
					return (FindNextCharacter (fromPos));
				}
				break;

				case	eCursorByWord: {
					TextStore&	ts	=	GetTextStore ();
					return (ts.FindFirstWordStartAfterPosition (ts.FindNextCharacter (fromPos)));
				}
				break;

				case	eCursorByRow: {
					size_t	startOfStartRow	=	GetStartOfRowContainingPosition (fromPos);
					size_t	startOfNextRow	=	GetStartOfNextRowFromRowContainingPosition (fromPos);
					if (startOfStartRow == startOfNextRow) {
						// no change
						return (fromPos);
					}
					else {
						return GetRowRelativeCharAtLoc (Tablet_Acquirer (this)->CvtFromTWIPSH (GetSelectionGoalColumn ()), startOfNextRow);
					}
				}
				break;

				case	eCursorByLine: {
					size_t	fromLine	=	GetTextStore ().GetLineContainingPosition (fromPos);
					size_t	newLine		=	(fromLine == GetTextStore ().GetLineCount ()-1)? fromLine: (fromLine+1);
					Led_Assert (newLine <= GetTextStore ().GetLineCount ()-1);
					if (newLine == fromLine) {
						// no change
						return (fromPos);
					}
					else {
						// try to maintain the same horizontal position across rows
						size_t	positionInLine	=	fromPos - GetTextStore ().GetStartOfLine (fromLine);	// ZERO RELATIVE
						Led_Assert (positionInLine <= GetTextStore ().GetLineLength (fromLine));
						positionInLine = Led_Min (positionInLine, GetTextStore ().GetLineLength (newLine));			// don't go past end of new line...
						return (GetTextStore ().GetStartOfLine (newLine) + positionInLine);
					}
				}
				break;

				case	eCursorByWindow: {
					Led_Assert (false);	// nyi
				}
				break;

				case	eCursorByBuffer: {
					Led_Assert (false);	// makes no sense - use eCursorToEnd...
				}
				break;

				default:	Led_Assert (false);
			}
		}
		break;

		case	eCursorToStart: {
			switch (movementUnit) {
				case	eCursorByChar: {
					Led_Assert (false);		// to start of character - does this make sense???
				}
				break;

				case	eCursorByWord: {
					return (GetTextStore ().FindFirstWordStartStrictlyBeforePosition (FindNextCharacter (fromPos)));
				}
				break;

				case	eCursorByRow: {
					size_t	fromRow	=	GetRowContainingPosition (fromPos);
					return (GetStartOfRow (fromRow));
				}
				break;

				case	eCursorByLine: {
					size_t	result		=	GetTextStore ().GetStartOfLineContainingPosition (fromPos);
					if (fromPos == result) {
						result  = GetTextStore ().GetStartOfLineContainingPosition (GetTextStore ().FindPreviousCharacter (result));
					}
					return result;
				}
				break;

				case	eCursorByWindow: {
					return (GetMarkerPositionOfStartOfWindow ());
				}
				break;

				case	eCursorByBuffer: {
					return (0);
				}
				break;

				default:	Led_Assert (false);
			}
		}
		break;

		case	eCursorToEnd: {
			switch (movementUnit) {
				case	eCursorByChar: {
					Led_Assert (false);		// to start of character - does this make sense???
				}
				break;

				case	eCursorByWord: {
					return (GetTextStore ().FindFirstWordEndAfterPosition (fromPos));
				}
				break;

				case	eCursorByRow: {
					size_t	fromRow	=	GetRowContainingPosition (fromPos);
					return (GetEndOfRow (fromRow));
				}
				break;

				case	eCursorByLine: {
					size_t	result		=	GetTextStore ().GetEndOfLineContainingPosition (fromPos);
					if (fromPos == result) {
						result = GetTextStore ().GetEndOfLineContainingPosition (GetTextStore ().FindNextCharacter (result));
					}
					return result;
				}
				break;

				case	eCursorByWindow: {
					return (GetMarkerPositionOfEndOfWindow ());
				}
				break;

				case	eCursorByBuffer: {
					return (GetTextStore ().GetEnd ());
				}
				break;

				default:	Led_Assert (false);
			}
		}
		break;

		default:	Led_Assert (false);
	}
	Led_Assert (false);		// not reached...
	return (fromPos);
}

#if		qSupportLed30CompatAPI
/*
@METHOD:		TextImager::GetTextBoundingRect
@DESCRIPTION:	<p><b>OBSOLETE</b></p>
				<p>Most likely you want to use @'TextImager::GetTextWindowBoundingRect'. This routine isn't
			BIDI friendly.</p>
*/
Led_Rect	TextImager::GetTextBoundingRect (size_t fromMarkerPos, size_t toMarkerPos) const
{
	Led_Rect	caretStart	=	GetCharLocation (fromMarkerPos);
	Led_Rect	caretEnd	=	(fromMarkerPos == toMarkerPos)? caretStart: GetCharLocation (toMarkerPos);

	Led_Rect	boundingRect;

	boundingRect.top = caretStart.top;
	boundingRect.bottom = caretEnd.bottom;
	if (caretStart.top == caretEnd.top) {
		boundingRect.left = caretStart.left;
		// We only go up to the start of the caretEnd - cuz we produce bounding rect BETWEEN markerPoses - not characters!
		boundingRect.right = caretEnd.left;
	}
	else {
		boundingRect.left = 0;
		boundingRect.right = GetWindowRect ().GetWidth ();
	}
	return (boundingRect);
}
#endif

/*
@METHOD:		TextImager::GetTextWindowBoundingRect
@DESCRIPTION:	<p>GetTextWindowBoundingRect () return a @'Led_Rect' which bounds
	the characters defined by the given marker positions. The will fit nicely around the
	characters if they all fit in one row (and one directional run), but may have sluff around the left/right sides
	if the range crosses row boundaries or directional runs (since the shape wouldn't be a rectangle, but a region -
	try GetSelectionWindowRegion () for that).</p>
		<p>This function operates on CharacterCells.</p>
		<p>This function operates in Window coordinates (ie window relative, calling @'TextImager::GetCharWindowLocation').</p>
		<p>Return value is pinned to be within the WindowRect.</p>
		<p>See also @'TextImager::GetIntraRowTextWindowBoundingRect'</p>
*/
Led_Rect	TextImager::GetTextWindowBoundingRect (size_t fromMarkerPos, size_t toMarkerPos) const
{
	Led_Require (fromMarkerPos <= toMarkerPos);

	Led_Rect	windowRect	=	GetWindowRect ();
	Led_Rect	r1			=	GetCharWindowLocation (fromMarkerPos);
	Led_Rect	r2			=	r1;
	if (fromMarkerPos != toMarkerPos) {
		r2	=	GetCharWindowLocation (FindPreviousCharacter (toMarkerPos));
	}

	size_t	realEndOfRowOfFromMarkerPos = GetRealEndOfRowContainingPosition (fromMarkerPos);

	Led_Rect	boundingRect;
	boundingRect.top = r1.GetTop ();
	boundingRect.bottom = r2.GetBottom ();		// too aggressive??? for case of end of row it is...

	if (realEndOfRowOfFromMarkerPos >= toMarkerPos) {
		/*
		 *	One ROW case
		 *
		 *		This is pretty complicated. We must worry about multiple overlapping runs, and about the
		 *	extensions from the start/end of the text to the window borders (depending on the
		 *	segmentHilightedAtStart/segmentHilightedAtEnd flags).
		 *
		 *		I'm not terribly confident this code is all right, but it really doesn't need to be perfect (though
		 *	it SHOULD be). Its just important that it returns a rectangle BIG ENOUGH to wrap ALL the releveant text.
		 *	Being a little TOO big is only inelegant, and not tragic.
		 *
		 *		See SPR#1237 for some details (and a test case).
		 */

		// A bit of a sloppy hack to make sure any drawing to the right or left of the text (up to the margin)
		// gets erased as well.
		size_t	startOfRow					=	GetStartOfRowContainingPosition (fromMarkerPos);
		size_t	endOfRow					=	GetEndOfRowContainingPosition (fromMarkerPos);
		bool	segmentHilightedAtStart		=	(fromMarkerPos == startOfRow);
		bool	segmentHilightedAtEnd		=	endOfRow < toMarkerPos;

		boundingRect.left = min (r1.GetLeft (), r2.GetLeft ());
		boundingRect.right = max (r1.GetRight (), r2.GetRight ());

		/*
		 *
		 *	Trouble is we could have something like:
		 *		1 2 7 8 3 4 5 6 9
		 *	and if I select from 6 to 9, I'll get just the right side of the row, and miss the
		 *	characters from 7-8.
		 *	Really I want all the way from the LHS of 7 to the RHS of 9 in this case.
		 */
		TextLayoutBlock_Copy	text = GetTextLayoutBlock (startOfRow, endOfRow);

		typedef	TextLayoutBlock::ScriptRunElt	ScriptRunElt;
		vector<ScriptRunElt> runs	=	text.GetScriptRuns ();
		if (runs.size () > 1) {
			// Only needed if there are nested runs...
			for (vector<ScriptRunElt>::const_iterator i = runs.begin (); i != runs.end (); ++i) {
				const ScriptRunElt&	se	=	*i;
				if (TextStore::Overlap (startOfRow + se.fRealStart, startOfRow + se.fRealEnd, fromMarkerPos, toMarkerPos)) {
					/*
					*	OK - there is SOME overlap between this run and the [fromMarkerPos, toMarkerPos] range.
					*
					*	Now see if the run endpoints are in the from/to range, and if so, then extend the
					*	bounding rectangle to accomodate them.
					*/
					if (fromMarkerPos <= startOfRow + se.fRealStart and startOfRow + se.fRealStart <= toMarkerPos) {
						Led_Rect	t1			=	GetCharWindowLocation (startOfRow + se.fRealStart);
						boundingRect.left = min (boundingRect.left, t1.GetLeft ());
						boundingRect.right = max (boundingRect.right, t1.GetRight ());
					}

					if (fromMarkerPos <= startOfRow + se.fRealEnd and startOfRow + se.fRealEnd <= toMarkerPos) {
						Led_Rect	t2			=	GetCharWindowLocation (FindPreviousCharacter (startOfRow + se.fRealEnd));
						boundingRect.left = min (boundingRect.left, t2.GetLeft ());
						boundingRect.right = max (boundingRect.right, t2.GetRight ());
					}
				}
			}
		}

		if (segmentHilightedAtStart) {
			boundingRect.left = windowRect.left;
		}
		if (segmentHilightedAtEnd) {
			boundingRect.right = windowRect.right;
		}
	}
	else {
		/*
		 *	Two or more ROWS case
		 */
		boundingRect.left = windowRect.left;
		boundingRect.right = windowRect.right;
	}

	// pin the results to be within the boundingRect (left and right)
	boundingRect.left = max (boundingRect.left, windowRect.left);
	boundingRect.right = min (boundingRect.right, windowRect.right);

	Led_Ensure (boundingRect.right >= boundingRect.left);
	return (boundingRect);
}

/*
@METHOD:		TextImager::GetIntraRowTextWindowBoundingRect
@ACCESS:		public
@DESCRIPTION:	<p>GetIntraRowTextWindowBoundingRect () return a Rect which bounds
			the characters defined by the given marker positions. The marker positions are required to fall
			within a single row.
				<p>This function operates on CharacterCells.</p>
				<p>This function operates in Window coordinates (ie the same coordinates as the WindowRect,
			is specified in - calling @'TextImager::GetCharWindowLocation').</p>
				<p>See also @'TextImager::GetTextBoundingRect',
			@'TextImager::GetTextWindowBoundingRect' and @'TextImager::GetIntraRowTextWindowBoundingRect'</p>
				<p>You CAN call this function with any range of 'fromMarkerPos' to 'toMarkerPos' within a row, but it only REALLY
			makes sense if you call it within a directional segment.</p>
*/
Led_Rect	TextImager::GetIntraRowTextWindowBoundingRect (size_t fromMarkerPos, size_t toMarkerPos) const
{
	Led_Require (fromMarkerPos <= toMarkerPos);	// and they must be within the same row!!! Assert later...

	Led_Rect	windowRect	=	GetWindowRect ();

	/*
	 *	Note that we could use one side of one character and the same side of the other (e.g. for LTR text
	 *	the left side of the first and second charaters). The trouble with this is that the character at 'toMarkerPos'
	 *	can sometimes be on another line - not RIGHT after the one we are interested in. Furthermore - now that
	 *	we support BIDI text - it could be left or right or almost anywhere if it was the start of another run.
	 *
	 *	So - instead - we measure (taking the LTR case) from the LEFT side of the 'fromMarkerPos' character
	 *	to the RIGHT side of the character PRECEDING the 'toMarkerPos' character (with the special case of when the two
	 *	markerPos positions are the same.
	 */
	Led_Rect	r1	=	GetCharWindowLocation (fromMarkerPos);
	Led_Rect	r2	=	r1;
	if (fromMarkerPos != toMarkerPos) {
		r2	=	GetCharWindowLocation (FindPreviousCharacter (toMarkerPos));
	}

	Led_Rect	boundingRect	=	r1;

	if (GetTextDirection (fromMarkerPos) == eLeftToRight) {
		boundingRect.left = r1.GetLeft ();
		boundingRect.right = (fromMarkerPos == toMarkerPos)? boundingRect.left: r2.GetRight ();
	}
	else {
		boundingRect.right = r1.GetRight ();
		boundingRect.left = (fromMarkerPos == toMarkerPos)? boundingRect.right: r2.GetLeft ();
	}
	Led_Ensure (boundingRect.right >= boundingRect.left);		// If this is triggered, its probably cuz your from/to crossed
																// a directional segment boundary?
	return (boundingRect);
}

/*
@METHOD:		TextImager::GetRowHilightRects
@ACCESS:		public
@DESCRIPTION:	<p>Compute the subregion of the row demarcated by 'rowStart' and 'rowEnd' which interesects with the given
			'hilightStart' and 'hilightEnd'. If the hilights extend past the row (either start or end) this is OK- and the
			hilight rectangle list is adjusted accordingly to display that extended hilgiht
			(if @'TextImager::GetUseSelectEOLBOLRowHilightStyle' is set).</p>
				<p>This routine is called by @'TextImager::GetSelectionWindowRegion' and @'TextImager::DrawRowHilight' to figure out what areas
			of the screen to hilight.</p>
*/
vector<Led_Rect>	TextImager::GetRowHilightRects (const TextLayoutBlock& text, size_t rowStart, size_t rowEnd, size_t hilightStart, size_t hilightEnd) const
{
	Led_Require (rowEnd == GetEndOfRowContainingPosition (rowStart));		// passed in for performance reasons - so not computed multiple times

	vector<Led_Rect>	result;

	size_t	realEndOfRow		=	GetRealEndOfRowContainingPosition (rowStart);
	bool	segmentHilighted	=	max (rowStart, hilightStart) < min (realEndOfRow, hilightEnd);
	if (segmentHilighted) {
		bool	segmentHilightedAtStart	=	false;
		bool	segmentHilightedAtEnd	=	false;
		if (GetUseSelectEOLBOLRowHilightStyle ()) {
			segmentHilightedAtStart	=	(hilightStart < rowStart) or (hilightStart == 0);
			segmentHilightedAtEnd	=	rowEnd < hilightEnd;
			if (segmentHilightedAtEnd and rowEnd >= GetEnd ()) {
				segmentHilightedAtEnd = false;			// last row always contains no NL - so no invert off to the right...
			}
		}

		hilightStart = max (hilightStart, rowStart);

		typedef	TextLayoutBlock::ScriptRunElt	ScriptRunElt;
		vector<ScriptRunElt> runs	=	text.GetScriptRuns ();
		//NB: it doesn't matter what order we iterate over the blocks and draw their hilight
		for (vector<ScriptRunElt>::const_iterator i = runs.begin (); i != runs.end (); ++i) {
			const ScriptRunElt&	se	=	*i;
			size_t				hRunStart	=	max (se.fRealStart + rowStart, hilightStart);
			size_t				hRunEnd		=	min (se.fRealEnd + rowStart, hilightEnd);
			if (hRunStart < hRunEnd) {
				Led_Rect	hilightRect	=	GetIntraRowTextWindowBoundingRect (hRunStart, hRunEnd);
				Led_Assert (hilightRect.GetWidth () >= 0);
				Led_Assert (hilightRect.GetHeight () >= 0);
				if (not hilightRect.IsEmpty ()) {			// don't add empty rectangles
					result.push_back (hilightRect);
				}
			}
		}

		// Add extra rects before or after all the text to the start/end of the row, as needed
		if (segmentHilightedAtStart) {
			size_t	realOffsetOfVirtualRowStart	=	rowStart;
			if (text.GetTextLength () != 0) {
				realOffsetOfVirtualRowStart += text.MapVirtualOffsetToReal (0);
			}
			// Make sure the 'segmentHilightAtStart is drawn even if the selection only comes up to just before first char in row
			Led_Rect	hilightRect	=	GetCharWindowLocation (realOffsetOfVirtualRowStart);
			hilightRect.right = hilightRect.left;
			hilightRect.left = min (GetWindowRect ().GetLeft (), hilightRect.left);
			Led_Assert (hilightRect.GetWidth () >= 0);
			Led_Assert (hilightRect.GetHeight () >= 0);
			if (not hilightRect.IsEmpty ()) {			// don't add empty rectangles
				result.push_back (hilightRect);
			}
		}
		if (segmentHilightedAtEnd) {
			size_t	realOffsetOfVirtualRowEnd	=	rowStart;
			if (text.GetTextLength () != 0) {
				realOffsetOfVirtualRowEnd += text.MapVirtualOffsetToReal (text.GetTextLength () - 1);
			}
			// Make sure the 'segmentHilightAtStart is drawn even if the selection only comes up to just before first char in row
			Led_Rect	hilightRect	=	GetCharWindowLocation (realOffsetOfVirtualRowEnd);
			hilightRect.left = hilightRect.GetRight ();
			hilightRect.right = max (hilightRect.right, GetWindowRect ().GetRight ());
			Led_Assert (hilightRect.GetWidth () >= 0);
			Led_Assert (hilightRect.GetHeight () >= 0);
			if (not hilightRect.IsEmpty ()) {			// don't add empty rectangles
				result.push_back (hilightRect);
			}
		}
	}

	#if		qDebug
		{
			// Make sure rectangles don't overlap with one another (can share an edge) -- SPR#1226
			for (vector<Led_Rect>::const_iterator orit = result.begin (); orit != result.end (); ++orit) {
				Led_Ensure ((*orit).GetWidth () > 0);
				Led_Ensure ((*orit).GetHeight () > 0);
				for (vector<Led_Rect>::const_iterator irit = orit+1; irit != result.end (); ++irit) {
					Led_Rect	hr	=	*irit;
					Led_Ensure (hr.GetWidth () > 0);
					Led_Ensure (hr.GetHeight () > 0);
					Led_Ensure (not Intersect (hr, *orit));
				}
			}
		}
	#endif
	return result;
}

/*
@METHOD:		TextImager::GetTextLayoutBlock
@ACCESS:		public
@DESCRIPTION:	<p>REQUIRE that rowStart and rowEnd are valid rowstart/end values</p>
*/
TextLayoutBlock_Copy	TextImager::GetTextLayoutBlock (size_t rowStart, size_t rowEnd)	const
{
	size_t							rowLen		=	rowEnd-rowStart;
	Led_SmallStackBuffer<Led_tChar>	rowBuf (rowLen);
	CopyOut (rowStart, rowLen, rowBuf);
	TextLayoutBlock_Basic	text (rowBuf, rowBuf + rowLen);
	return TextLayoutBlock_Copy (text);
}

/*
@METHOD:		TextImager::GetSelectionWindowRects
@ACCESS:		public
@DESCRIPTION:	<p>Compute the window-relative region (list of rectangles) bounding the
			given segment of text. This is useful for displaying some sort of text hilight,
			in addition (or apart from)	the standard hilighting of text.</p>
				<p>This function uses @'TextImager::GetRowHilightRects' to figure out what areas of the
			screen to hilight. This routine also tries to take into account interline space by
			extending the hilight from succeeding rows back to the bottom of the preceeding row.</p>
				<p>See also @'TextImager::GetSelectionWindowRegion'</p>
*/
vector<Led_Rect>	TextImager::GetSelectionWindowRects (size_t from, size_t to) const
{
	Led_Require (from <= to);

	vector<Led_Rect>	result;

	from = max (from, GetMarkerPositionOfStartOfWindow ());
	to = min (to, FindNextCharacter (GetMarkerPositionOfEndOfWindow ()));

	if (from >= to) {
		return result;
	}
	Led_Assert (from < to);

	size_t		topRow		=	GetRowContainingPosition (from);	  
	size_t		bottomRow	=	GetRowContainingPosition (to);
	Led_Assert (topRow <= bottomRow);

	// If to is at the start of a row (remember - we wanted the to select char UP-TO that
	// MARKER POS) then we've gone one row too far
	if (GetStartOfRow (bottomRow) == to) {
		// then use end of previous row
		Led_Assert (topRow < bottomRow);
		bottomRow--;
	}

	Led_Coordinate	lastRowBottom	=	0;	// Keep track of last row's bottom for interline-space support
	for (size_t curRow = topRow; ; ) {
		size_t	firstCharInRow	=	from;
		if (topRow != curRow) {
			firstCharInRow = GetStartOfRow (curRow);
		}

		size_t	startOfRow	=	GetStartOfRowContainingPosition (firstCharInRow);
		size_t	endOfRow	=	GetEndOfRowContainingPosition (startOfRow);
#if 1
		TextLayoutBlock_Copy	text = GetTextLayoutBlock (startOfRow, endOfRow);
#else
		size_t	rowLen		=	endOfRow-startOfRow;
		Led_SmallStackBuffer<Led_tChar>	rowBuf (rowLen);
		CopyOut (startOfRow, rowLen, rowBuf);
		TextLayoutBlock_Basic	text (rowBuf, rowBuf + rowLen);
#endif

		vector<Led_Rect>	hilightRects		=	GetRowHilightRects (text, startOfRow, endOfRow, GetSelectionStart (), GetSelectionEnd ());
		Led_Coordinate		newMinTop			=	lastRowBottom;
		Led_Coordinate		newMaxBottom		=	lastRowBottom;
		for (vector<Led_Rect>::const_iterator i = hilightRects.begin (); i != hilightRects.end (); ++i) {
			Led_Rect	hilightRect	=	*i;
			Led_Require (hilightRect.GetWidth () >= 0);
			Led_Assert (hilightRect.GetHeight () > 0);
			if (not hilightRect.IsEmpty ()) {
				result.push_back (hilightRect);
			}
			newMinTop = min (newMinTop, hilightRect.top);
			newMaxBottom = max (newMaxBottom, hilightRect.bottom);
		}

		/*
		 *	Now that we've kept that lastRowBottom and this emitted rows minTop and maxBottom, we can
		 *	see if there is any gap between the lastRowBottom and the newMinTop. If yes, then fill
		 *	that gap.
		 */
		if (lastRowBottom < newMinTop) {
			// Compute this hScrollAdjustedWR inside loop since it should happen extremely rarely, and doing it
			// outside would mean it gets called more (cuz we can only do the test inside).
			Led_Rect	hScrollAdjustedWR	=	GetWindowRect () - Led_Point (0, GetHScrollPos ());
			result.push_back (Led_Rect (lastRowBottom, hScrollAdjustedWR.GetLeft (), newMinTop - lastRowBottom, hScrollAdjustedWR.GetWidth ()));
		}
		lastRowBottom = newMaxBottom;

		if (curRow == bottomRow) {
			break;
		}
		curRow++;
	}

	#if		qDebug
		{
			// Make sure rectangles don't overlap with one another (can share an edge) -- SPR#1226
			for (vector<Led_Rect>::const_iterator orit = result.begin (); orit != result.end (); ++orit) {
				Led_Ensure ((*orit).GetWidth () > 0);
				Led_Ensure ((*orit).GetHeight () > 0);
				for (vector<Led_Rect>::const_iterator irit = orit+1; irit != result.end (); ++irit) {
					Led_Rect	hr	=	*irit;
					Led_Ensure (hr.GetWidth () > 0);
					Led_Ensure (hr.GetHeight () > 0);
					Led_Ensure (not Intersect (hr, *orit));
				}
			}
		}
	#endif
	return result;
}

/*
@METHOD:		TextImager::GetSelectionWindowRegion
@ACCESS:		public
@DESCRIPTION:	<p>Figure the region bounding the given segment of text. Useful for displaying
			some sort of text hilight, in addition (or apart from) the standard hilighting
			of text. Note we use a VAR parameter for the region rather than returing it
			cuz MFC's CRgn class doesn't support being copied.</p>
				<p>This routine is a simple wrapper on @'TextImager::GetSelectionWindowRects'</p>
*/
void	TextImager::GetSelectionWindowRegion (Led_Region* r, size_t from, size_t to) const
{
	Led_RequireNotNil (r);
	vector<Led_Rect>	selRects	=	GetSelectionWindowRects (from, to);
	for (vector<Led_Rect>::const_iterator i = selRects.begin (); i != selRects.end (); ++i) {
		AddRectangleToRegion (*i, r);
	}
}

/*
@METHOD:		TextImager::EraseBackground
@DESCRIPTION:	<p>EraseBackground () is called internally by TextImagers to get arbitrary subsets of the current
	window being drawn to erased. This erasure must only apply to the argument 'subsetToDraw'. Clipping is
	not used to gaurantee this. Failure to follow that rule can produce undesirably results where other bits of
	surrounding text get erased.</p>
		<p>The erasebackground call is made virtual so that you can use this as a hook to provide some sort
	of multi-media, picture or whatever as your background. Note that the is called during the draw
	process as a side-effect. So to have a changing background, you would need to both force periodic
	updates, and override this routine.</p>
		<p>NB: the argument 'subsetToDraw' can occasionally be outside of the WindowRect. If so - then DO draw where this
	function says to draw. This is because you maybe asked to erase window margins/borders outside the Led 'WindowRect' using
	the same color/algorithm as that used inside the Led WindowRect.</p>
		<p>By default - this simply calls @'Led_Tablet_::EraseBackground_SolidHelper' with @'Led_GetTextBackgroundColor' ().</p>
		<p>Note - typically when you override this - you will want to override @'TextImager::HilightARectangle' to change
	its implementation to specify a new background color (so inverting works properly). Perhaps see SPR#0708 for details.</p>
*/
void	TextImager::EraseBackground (Led_Tablet tablet, const Led_Rect& subsetToDraw, bool printing)
{
	Led_RequireNotNil (tablet);
	// Don't erase when printing - at least by default. Tends to screw up most print drivers.
	if (not printing) {
		tablet->EraseBackground_SolidHelper (subsetToDraw, GetEffectiveDefaultTextColor (TextImager::eDefaultBackgroundColor));
	}
}

#if		qSupportLed30CompatAPI
/*
@METHOD:		TextImager::HilightARectangle
@DESCRIPTION:	<p>Hilight the given rectangle of the screen, after its been drawn. This is typically done via
	some sort of pixel or color invesion. The default implemtation uses @'Led_Tablet_::HilightARectangle_SolidHelper'.</p>
		<p>Override this mostly if you want different hilighting behavior, or if you want your hilighting behavior
	to remain in sync with other changes to the EraseBackground behavior.</p>
		<p>OBSOLETE - use @'TextImager::HilightArea' instead.</p>
*/
void	TextImager::HilightARectangle (Led_Tablet tablet, Led_Rect hiliteRect)
{
	Led_RequireNotNil (tablet);
	HilightArea (tablet, hiliteRect);
}
#endif

/*
@METHOD:		TextImager::HilightArea
@DESCRIPTION:	<p>Hilight the given rectangle of the screen, after its been drawn. This is typically done via
	some sort of pixel or color invesion. The default implemtation uses @'Led_Tablet_::HilightArea_SolidHelper'.</p>
		<p>Override this mostly if you want different hilighting behavior, or if you want your hilighting behavior
	to remain in sync with other changes to the EraseBackground behavior.</p>
*/
void	TextImager::HilightArea (Led_Tablet tablet, Led_Rect hiliteArea)
{
	Led_RequireNotNil (tablet);
	tablet->HilightArea_SolidHelper (hiliteArea, GetEffectiveDefaultTextColor (eDefaultSelectedTextBackgroundColor), GetEffectiveDefaultTextColor (eDefaultSelectedTextColor), GetEffectiveDefaultTextColor (eDefaultBackgroundColor), GetEffectiveDefaultTextColor (eDefaultTextColor));
}

/*
@METHOD:		TextImager::HilightArea
@DESCRIPTION:	<p>Hilight the given region of the screen, after its been drawn. This is typically done via
	some sort of pixel or color invesion. The default implemtation uses @'Led_Tablet_::HilightArea_SolidHelper'.</p>
		<p>Override this mostly if you want different hilighting behavior, or if you want your hilighting behavior
	to remain in sync with other changes to the EraseBackground behavior.</p>
*/
void	TextImager::HilightArea (Led_Tablet tablet, const Led_Region& hiliteArea)
{
	Led_RequireNotNil (tablet);
	tablet->HilightArea_SolidHelper (hiliteArea, GetEffectiveDefaultTextColor (eDefaultSelectedTextBackgroundColor), GetEffectiveDefaultTextColor (eDefaultSelectedTextColor), GetEffectiveDefaultTextColor (eDefaultBackgroundColor), GetEffectiveDefaultTextColor (eDefaultTextColor));
}

/*
@METHOD:		TextImager::DrawRow
@DESCRIPTION:	<p>Draw the given row of text. Erase the background (by calling @'TextImager::EraseBackground'), and
			then draw the segments of the row by calling @'TextImager::DrawRowSegments'. Then draw any hilighting.
			This routine CAN cause flicker. Flicker is eliminated at a higher level (caller) by using offscreen bitmaps (see @'MultiRowTextImager::Draw'
			or @'TextImager::GetImageUsingOffscreenBitmaps').</p>
				<p>NB: This bit about flicker changed in Led 2.2. Earlier Led versions tried to elminate flicker at all levels
			and avoid offscreen bitmaps. But problems with kerning made this difficult.</p>
				<p>Note, only the invalidRect subset of currentRowRect need be drawn, though the rest CAN be.</p>
				<p>Renamed to @'TextImager::DrawRowSegments' from MutliRowTextImager::DrawRowSegments for Led 3.1a3 release.</p>
*/
void	TextImager::DrawRow (Led_Tablet tablet, const Led_Rect& currentRowRect, const Led_Rect& invalidRowRect,
									const TextLayoutBlock& text, size_t rowStart, size_t rowEnd, bool printing
								)
{
	Led_RequireNotNil (tablet);
	Led_Require (rowEnd == GetEndOfRowContainingPosition (rowStart));		// passed in for performance reasons - so not computed multiple times

	/*
	 *	Could CONSIDER doing something like:
	 *
	 *		Led_Tablet_::ClipNarrowAndRestore	clipFurtherTo (tablet, currentRowRect);
	 *
	 *	here, but it might have too much of a performance cost. Perhaps I should test this. See SPR#?????
	 */

	EraseBackground (tablet, currentRowRect, printing);

	DrawRowSegments (tablet, currentRowRect, invalidRowRect, text, rowStart, rowEnd);

	/*
	 *	Only draw hilighting if we aren't printing, because this doesn't show up well on printers.
	 */
	if (not printing) {
		DrawRowHilight (tablet, currentRowRect, invalidRowRect, text, rowStart, rowEnd);
	}
}

/*
@METHOD:		TextImager::DrawRowSegments
@DESCRIPTION:	<p>Called by @'TextImager::DrawRow' to draw (in or mode - don't worry about erasing) all the segments
			which make up the row of text. Here, we take care of any sort of justification, or indending (in subclasses which override
			this method). Its rowRect its called with represents the entire row. Subclasses can call DrawSegment () with adjusted row-rects
			taking into account indents, etc.</p>
				<p>Note, only the invalidRect subset of currentRowRect need be drawn, though the rest CAN be.</p>
				<p>Renamed to @'TextImager::DrawRowSegments' from MutliRowTextImager::DrawRowSegments for Led 3.1a3 release.</p>
*/
void	TextImager::DrawRowSegments (Led_Tablet tablet, const Led_Rect& currentRowRect, const Led_Rect& invalidRowRect,
									const TextLayoutBlock& text, size_t rowStart, size_t rowEnd
								)
{
	Led_RequireNotNil (tablet);

	#if		qDebug && 0
// try to get this code enabled again - even here!!! LGP 2002-12-02
		{
			size_t			startOfRow		=	GetStartOfRow (row);
			size_t			endOfRow		=	GetEndOfRow (row);
			size_t			realEndOfRow	=	GetRealEndOfRow (row);
			Led_Assert (startOfRow == start);
			Led_Assert (endOfRow <= end);
			Led_Assert (end <= realEndOfRow);
		}
	#endif


	/*
	 *	We always want to draw all the characters in the row - including the character
	 *	that terminates the row. Typically - this is a space in a word-wrap so it
	 *	cannot be seen. Or a NEWLINE char. In the case of a NEWLINE char - these
	 *	don't really display properly - so we skip drawing those.
	 *
	 *	An earlier attempt at this said - DONT BOTHER drawing the wrap character. The problem
	 *	with this is that in SOME languages (e.g. Japanese) the character used as a wrap-char
	 *	may be a real useful (Japanese) character!
	 */
	size_t			segEnd		=	rowEnd;
	Led_Coordinate	baseLine	=	currentRowRect.top + MeasureSegmentBaseLine (rowStart, segEnd);

	/*
	 *	Its OK for the baseline to be outside of the currentRowRect. But the text display of this
	 *	seems to mimic MSWord better if you pin the baseLine inside the rowRect.
	 */
	baseLine = min (baseLine, currentRowRect.bottom);

	DrawSegment (tablet, rowStart, segEnd, text, currentRowRect, invalidRowRect, baseLine, NULL);
}

/*
@METHOD:		TextImager::DrawRowHilight
@DESCRIPTION:	<p>Called by @'TextImager::DrawRow' to draw any necessary hilighting for the current selection
			for the given row.</p>
				<p>Note, only the invalidRect subset of currentRowRect need be drawn, though the rest CAN be.</p>
				<p>Renamed/Moved to @'TextImager::DrawRowHilight' from MutliRowTextImager::DrawRowHilight for Led 3.1a3 release.</p>
*/
void	TextImager::DrawRowHilight (Led_Tablet tablet, const Led_Rect& currentRowRect, const Led_Rect& /*invalidRowRect*/,
									const TextLayoutBlock& text, size_t rowStart, size_t rowEnd
								)
{
	Led_Require (rowEnd == GetEndOfRowContainingPosition (rowStart));		// passed in for performance reasons - so not computed multiple times
	Led_Arg_Unused (currentRowRect);

	if (GetSelectionShown ()) {
		vector<Led_Rect>	hilightRects	=	GetRowHilightRects (text, rowStart, rowEnd, GetSelectionStart (), GetSelectionEnd ());
		for (vector<Led_Rect>::const_iterator i = hilightRects.begin (); i != hilightRects.end (); ++i) {
			Led_Rect	hilightRect	=	*i;
			#if		qDebug
				{
					// Funky test - see SPR# 0470 for details...
					if (Intersect (hilightRect, currentRowRect) or hilightRect.IsEmpty ()) {
						Led_Rect	x	=	hilightRect;
						Led_Rect	y	=	currentRowRect;
						x.left = y.left;
						x.right = y.right;
						Led_Assert (Intersect (x, y) or x.IsEmpty ());
					}
				}
			#endif
			HilightArea (tablet, hilightRect);
		}
	}
}

/*
@METHOD:		TextImager::DrawInterLineSpace
@DESCRIPTION:	<p>Typically called by @'MultiRowTextImager::DrawPartitionElement' or @'SimpleTextImager::DrawPartitionElement' to draw space between paragraphs (lines).
			Typically this is nothing. But this hook can be used to draw various sorts of annotations on
			paragraphs (as in LECs LVEJ side-by-side mode).</p>
				<p>Renamed to @'TextImager::DrawInterLineSpace' from MutliRowTextImager::DrawInterLineSpace for Led 3.1a3 release.</p>
*/
void	TextImager::DrawInterLineSpace (Led_Distance interlineSpace, Led_Tablet tablet, Led_Coordinate vPosOfTopOfInterlineSpace, bool segmentHilighted, bool printing)
{
	// This code not been checked/tested since I rewrote the erasing code etc.. Maybe wrong - probably wrong? No matter, anybody
	// ever using interline space would probably override this anyhow..
	// LGP 960516
	Led_AssertNotNil (tablet);
	if (interlineSpace != 0) {
		Led_Rect	fillRect	=	GetWindowRect ();
		fillRect.top = vPosOfTopOfInterlineSpace;
		fillRect.bottom = vPosOfTopOfInterlineSpace + interlineSpace;
		EraseBackground (tablet, fillRect, printing);
		if (segmentHilighted) {
			HilightArea (tablet, fillRect);
		}
	}
}

/*
@METHOD:		TextImager::ContainsMappedDisplayCharacters
@DESCRIPTION:	<p>Override this to specify if any of the given characters should be hidden, removed,
	or mapped to some other display character.
	You can change the behavior of this
	function at runtime (depending on user settings). But when its changed - you should invalidate all cached information,
	since cached font metrics will we invalid (often with @'MultiRowTextImager::InvalidateAllCaches ()').</p>
		<p>Note - this function doesn't REPLACE the given character from the text. Instead -
	it merely causes the @'TextImager::ReplaceMappedDisplayCharacters', @'TextImager::RemoveMappedDisplayCharacters',
	and @'TextImager::RemoveMappedDisplayCharacters' () methods to get called.</p>
		<p>Note also that this function takes an array of characters as a performance optimization (so it doesn't need to
	be called too many times, and to avoid copying buffers when nothing need be done). It could have been more logically
	(though less efficeintly) implemented as bool IsMappedDisplayCharacter (Led_tChar).</p>
		<p>If you override this - you may find it handy to call @'TextImager::ContainsMappedDisplayCharacters_HelperForChar'
	to do most of the work.</p>
*/
bool	TextImager::ContainsMappedDisplayCharacters (const Led_tChar* /*text*/, size_t /*nTChars*/) const
{
	return false;
}

/*
@METHOD:		TextImager::ReplaceMappedDisplayCharacters
@DESCRIPTION:	<p>Override this to specify any characters which should be mapped to different values at the last minute for
	display purposes only. This can be used to give a (simple to implement) funny display for particular special characters.
	For example, this technique can be used to make a NEWLINE character display as a special end-of-paragraph marker.</p>
		<p>If you override this - you may find it handy to call @'TextImager::ReplaceMappedDisplayCharacters_HelperForChar'
	to do most of the work.</p>
		<p>See also @'TextImager::ContainsMappedDisplayCharacters'.</p>
*/
void	TextImager::ReplaceMappedDisplayCharacters (const Led_tChar* srcText, Led_tChar* copyText, size_t nTChars) const
{
	// Default to none replaced- just plain copy...
	(void)::memcpy (copyText, srcText, nTChars * sizeof (Led_tChar));
}

/*
@METHOD:		TextImager::RemoveMappedDisplayCharacters
@DESCRIPTION:	<p>Override this to specify any characters which should be removed at the last minute for
	display purposes only. This can be used to give a (simple to implement) way to hide some special characters.
	For example, this technique can be used to implement SHIFT-RETURN soft line breaks (as is done in
	@'WordWrappedTextImager::RemoveMappedDisplayCharacters').</p>
		<p>If you override this - you may find it handy to call @'TextImager::RemoveMappedDisplayCharacters_HelperForChar'
	to do most of the work.</p>
		<p>See also @'TextImager::ContainsMappedDisplayCharacters'.</p>
*/
size_t	TextImager::RemoveMappedDisplayCharacters (Led_tChar* /*copyText*/, size_t nTChars) const
{
	// Default to none removed
	return nTChars;
}

/*
@METHOD:		TextImager::PatchWidthRemoveMappedDisplayCharacters
@DESCRIPTION:	<p>Override this to specify any characters which should be removed at the last minute for
	display purposes only. This particular function patches the distanceResults to zero out the removed characters.
	This can be used to give a (simple to implement) way to hide some special characters.</p>
		<p>For example, this technique can be used to implement SHIFT-RETURN soft line breaks (as is done in
	@'WordWrappedTextImager::RemoveMappedDisplayCharacters').</p>
		<p>If you override this - you may find it handy to call @'TextImager::PatchWidthRemoveMappedDisplayCharacters_HelperForChar'
	to do most of the work.</p>
		<p>See also @'TextImager::ContainsMappedDisplayCharacters'.</p>
*/
void	TextImager::PatchWidthRemoveMappedDisplayCharacters (const Led_tChar* /*srcText*/, Led_Distance* /*distanceResults*/, size_t /*nTChars*/) const
{
}

/*
@METHOD:		TextImager::ContainsMappedDisplayCharacters_HelperForChar
@DESCRIPTION:	<p>Simple implementation of See also @'TextImager::ContainsMappedDisplayCharacters' which is frequently applicable.
	Just specify the special character you are looking for.</p>
*/
bool	TextImager::ContainsMappedDisplayCharacters_HelperForChar (const Led_tChar* text, size_t nTChars, Led_tChar charToMap)
{
	// 'charToMap' characters can appear anywhere in a segment of text (cuz this gets called to compute widths for an entire paragraph at a time).
	const Led_tChar*	end		=	&text[nTChars];
	for (const Led_tChar* cur = text; cur < end; cur = Led_NextChar (cur)) {
		if (*cur == charToMap) {
			return true;
		}
	}
	return false;
}

/*
@METHOD:		TextImager::ReplaceMappedDisplayCharacters_HelperForChar
@DESCRIPTION:	<p>Simple implementation of See also @'TextImager::ReplaceMappedDisplayCharacters_HelperForChar' which is frequently applicable.
	Just specify the special character you are looking for, and the one you are mapping to.</p>
		<p>See also @'TextImager::ContainsMappedDisplayCharacters'.</p>
*/
void	TextImager::ReplaceMappedDisplayCharacters_HelperForChar (Led_tChar* copyText, size_t nTChars, Led_tChar charToMap, Led_tChar charToMapTo)
{
	// 'charToMap' characters can appear anywhere in a segment of text (cuz this gets called to compute widths for an entire paragraph at a time).
	Led_tChar*	end		=	&copyText[nTChars];
	for (Led_tChar* cur = copyText; cur < end; cur = Led_NextChar (cur)) {
		if (*cur == charToMap) {
			*cur = charToMapTo;
		}
	}
}

/*
@METHOD:		TextImager::RemoveMappedDisplayCharacters_HelperForChar
@DESCRIPTION:	<p>Simple implementation of See also @'TextImager::RemoveMappedDisplayCharacters' which is frequently applicable.
	Just specify the special character you are looking to remove.</p>
		<p>See also @'TextImager::ContainsMappedDisplayCharacters'.</p>
*/
size_t	TextImager::RemoveMappedDisplayCharacters_HelperForChar (Led_tChar* copyText, size_t nTChars, Led_tChar charToRemove)
{
	// Trim out any kSoftLineBreakChar characters
	Led_tChar*	outPtr	=	copyText;
	Led_tChar*	end		=	copyText + nTChars;
	for (const Led_tChar* cur = copyText; cur < end; cur = Led_NextChar (cur)) {
		if (*cur != charToRemove) {
			*outPtr = *cur;
			outPtr = Led_NextChar (outPtr);
		}
	}
	size_t	newLen	=	outPtr - copyText;
	Led_Assert (newLen <= nTChars);
	return newLen;
}

/*
@METHOD:		TextImager::PatchWidthRemoveMappedDisplayCharacters_HelperForChar
@DESCRIPTION:	<p>Simple implementation of See also @'TextImager::PatchWidthRemoveMappedDisplayCharacters' which is frequently applicable.
	Just specify the special character you are looking to remove.</p>
		<p>See also @'TextImager::ContainsMappedDisplayCharacters'.</p>
*/
void	TextImager::PatchWidthRemoveMappedDisplayCharacters_HelperForChar (const Led_tChar* srcText, Led_Distance* distanceResults, size_t nTChars, Led_tChar charToRemove)
{
	// Each of these kSoftLineBreakChar will be mapped to ZERO-WIDTH. So walk text (and distanceResults) and when
	// I see a softlinebreak - zero its size, and subtrace from start point total amount of zero-ed softlinebreaks.
	Led_Distance		cumSubtract	=	0;
	const Led_tChar*	end			=	srcText + nTChars;
	for (const Led_tChar* cur = srcText; cur < end; cur = Led_NextChar (cur)) {
		size_t			i					=	cur-srcText;
		Led_Assert (i < nTChars);
		if (*cur == charToRemove) {
			Led_Distance	thisSoftBreakWidth	=	i==0? distanceResults[0]: (distanceResults[i]-distanceResults[i-1]);
			cumSubtract = thisSoftBreakWidth;
		}
		distanceResults[i] -= cumSubtract;
	}
}

/*
@METHOD:		TextImager::DrawSegment
@DESCRIPTION:	<p>DrawSegment () is responsible for rendering the text within a segment (subset of a row).
			Note that because of bidirectional display, the 'from' and 'to' are LOGICAL offsets (what are
			used to lookup in @'TextStore'), but they may NOT be the same as the display-order offsets. That is to
			say - a character between offset 10-11 could be either to the right or left of one at offset 13-14.
			</p>
				<p>Note we REQUIRE that useBaseLine be contained within drawInto invalidRect specified the subset of the drawInto
			rect which really must be filled in. This can be ignored, or used for logical clipping.</p>
				<p>See also @'TextImager::DrawSegment_'. The variable 'pixelsDrawn' is OPTIONAL parameter
			(ie filled in if non-NULL)</p>
				<p>See also @'TextImager::DrawSegment_'.</p>
*/
void	TextImager::DrawSegment (Led_Tablet tablet,
									size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/,
									Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn
								)
{
	DrawSegment_ (tablet, GetDefaultFont (), from, to, text, drawInto, useBaseLine, pixelsDrawn);
}

/*
@METHOD:		TextImager::DrawSegment_
@DESCRIPTION:	<p>Helper function to access tablet text drawing. This function takes care of setting the background/foreground colors,
			and setting up a font object to be used in the Led_Tablet_::TabbedTextOut calls (maybe can go away if we do better integrating
			font code into Led_Tablet/LedGDI and out of TextImager).</p>
				<p>The 'from' marker position must be legit, since it is used to grab the tabstop list.
			The 'end' marker position is OK to fake (passing in other text), as it is just used to determine the string length. Note
			text in 'text' argument need not match the REAL text in the TextStore buffer.
		<p>See also @'TextImager::MeasureSegmentWidth_'.</p>
*/
void	TextImager::DrawSegment_ (Led_Tablet tablet, const Led_FontSpecification& fontSpec,
									size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto,
									Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn
								) const
{
	Led_RequireNotNil (tablet);
	Led_Assert (from <= to);

	/*
	 *	In the presence of multiple markers, you might VERY plaisbly override this method and
	 *	change how the layout of text is done within a segment.
	 *
	 *	I've thought long and hard about how to automatically combine the drawing effects
	 *	of different markers which overlap, and have come up with noting reasonable. So its
	 *	up to YOU by overriding this method todo what you want.
	 */
	size_t	length	=	to-from;


	/*
	 *	Setup the colors to be drawn.
	 */
	Led_Color	foreColor	=	fontSpec.GetTextColor ();
	Led_Color	backColor	=	GetEffectiveDefaultTextColor (eDefaultBackgroundColor);
	tablet->SetBackColor (backColor);
	tablet->SetForeColor (foreColor);

	FontCacheInfoUpdater	fontCacheUpdater (this, tablet, fontSpec);

	Led_Distance	ascent				=	fCachedFontInfo.GetAscent ();
	Led_Assert (useBaseLine >= drawInto.top);


	//Led_Assert (useBaseLine <= drawInto.bottom);		Now allowed... LGP 2000-06-12 - see SPR#0760 - and using EXACT-height of a small height, and use a large font
	Led_Coordinate	drawCharTop			=	useBaseLine - ascent;			// our PortableGDI_TabbedTextOut() assumes draw in topLeft
	//Led_Require (drawCharTop >= drawInto.top);		// Same deal as for useBaseLine - LGP 2000-06-12

	if (fontSpec.GetStyle_SubOrSuperScript () == Led_FontSpecification::eSuperscript) {
		// See SPR#1523- was 'drawCharTop -= fCachedFontInfo.GetAscent ()'
		// Careful to sync this with FontCacheInfoUpdater::CTOR () height adjustment
		// get back to (roughly - round down) original ascent. If we did TIMES 2/3 now
		// mutliply by reciprocal to get back (again - rounding down so we don't go
		// up too high).
		drawCharTop = useBaseLine - ascent * 3 / 2;
	}
	else if (fontSpec.GetStyle_SubOrSuperScript () == Led_FontSpecification::eSubscript) {
		drawCharTop += fCachedFontInfo.GetDescent ();
	}

	typedef	TextLayoutBlock::ScriptRunElt	ScriptRunElt;
	vector<ScriptRunElt> runs	=	text.GetScriptRuns ();
	Led_Assert (not runs.empty () or (length == 0));
	if (runs.size () > 1) {
		// sort by virtual start
		sort (runs.begin (), runs.end (), TextLayoutBlock::LessThanVirtualStart ());
	}

	const Led_tChar*	fullVirtualText	=	text.PeekAtVirtualText ();
	Led_Point			outputAt		=	Led_Point (drawCharTop, drawInto.left);
	if (pixelsDrawn != NULL) {
		*pixelsDrawn = 0;
	}
	for (vector<ScriptRunElt>::const_iterator i = runs.begin (); i != runs.end (); ++i) {
		const ScriptRunElt&	se	=	*i;
		size_t	runLength	=	se.fVirtualEnd - se.fVirtualStart;

		/*
		 *	Fill in the useVirtualText buffer with the text to draw.
		 */
		Led_SmallStackBuffer<Led_tChar>	useVirtualText (runLength);
		copy (&fullVirtualText[se.fVirtualStart], &fullVirtualText[se.fVirtualStart] + runLength, static_cast<Led_tChar*> (useVirtualText));

		/*
		 *	Process 'mapped display characters'
		 */
		Led_tChar*	drawText	=	useVirtualText;
		size_t		drawTextLen	=	runLength;
		Led_SmallStackBuffer<Led_tChar>	mappedDisplayBuf (1);
		if (ContainsMappedDisplayCharacters (drawText, drawTextLen)) {
			mappedDisplayBuf.GrowToSize (drawTextLen);
			ReplaceMappedDisplayCharacters (drawText, mappedDisplayBuf, drawTextLen);
			size_t	newLength	=	RemoveMappedDisplayCharacters (mappedDisplayBuf, drawTextLen);
			Led_Assert (newLength <= drawTextLen);
			drawText = mappedDisplayBuf;
			drawTextLen = newLength;
		}

		/*
		 *	Actually draw the text.
		 */
		Led_Distance	amountDrawn	=	0;
		tablet->TabbedTextOut (fCachedFontInfo, drawText, drawTextLen, se.fDirection, outputAt, GetWindowRect ().left, GetTabStopList (from), &amountDrawn, GetHScrollPos ());
		outputAt.h += amountDrawn;
		if (pixelsDrawn != NULL) {
			*pixelsDrawn += amountDrawn;
		}
	}
}

void	TextImager::MeasureSegmentWidth (size_t from, size_t to, const Led_tChar* text,
														Led_Distance* distanceResults
													) const
{
	MeasureSegmentWidth_ (GetDefaultFont (), from, to, text, distanceResults);
}

/*
@METHOD:		TextImager::MeasureSegmentWidth_
@DESCRIPTION:	<p>Helper function to access tablet text measurement.</p>
				<p>The 'from' and 'to' marker positions are ignored, except to compute the width. There is no
			requirement that the 'text' argument refer to the same text as that stored in the TextStore object.
		<p>See also @'TextImager::DrawSegment_'.</p>
*/
void		TextImager::MeasureSegmentWidth_ (const Led_FontSpecification& fontSpec, size_t from, size_t to,
														const Led_tChar* text,
														Led_Distance* distanceResults
													) const
{
	Led_Require (to > from);

	Tablet_Acquirer	tablet (this);

	size_t			length	=	to-from;
	Led_Assert (length > 0);

	#if		qMultiByteCharacters
		Led_Assert (Led_IsValidMultiByteString (text, length));
		Assert_CharPosDoesNotSplitCharacter (from);
		Assert_CharPosDoesNotSplitCharacter (to);
	#endif

	FontCacheInfoUpdater	fontCacheUpdater (this, tablet, fontSpec);

	if (ContainsMappedDisplayCharacters (text, length)) {
		Led_SmallStackBuffer<Led_tChar>	buf2 (length);
		ReplaceMappedDisplayCharacters (text, buf2, length);
		tablet->MeasureText (fCachedFontInfo, buf2, length, distanceResults);
		PatchWidthRemoveMappedDisplayCharacters (buf2, distanceResults, length);
	}
	else {
		tablet->MeasureText (fCachedFontInfo, text, length, distanceResults);
	}
}

Led_Distance	TextImager::MeasureSegmentHeight (size_t from, size_t to) const
{
	return (MeasureSegmentHeight_ (GetDefaultFont (), from, to));
}

Led_Distance	TextImager::MeasureSegmentHeight_ (const Led_FontSpecification& fontSpec, size_t /*from*/, size_t /*to*/) const
{
	Tablet_Acquirer	tablet (this);
	Led_AssertNotNil (static_cast<Led_Tablet> (tablet));
	FontCacheInfoUpdater	fontCacheUpdater (this, tablet, fontSpec);
	return (fCachedFontInfo.GetLineHeight ());
}

Led_Distance	TextImager::MeasureSegmentBaseLine (size_t from, size_t to) const
{
	return (MeasureSegmentBaseLine_ (GetDefaultFont (), from, to));
}

Led_Distance	TextImager::MeasureSegmentBaseLine_ (const Led_FontSpecification& fontSpec, size_t /*from*/, size_t /*to*/) const
{
	Tablet_Acquirer	tablet (this);
	Led_AssertNotNil (static_cast<Led_Tablet> (tablet));
	FontCacheInfoUpdater	fontCacheUpdater (this, tablet, fontSpec);
	return (fCachedFontInfo.GetAscent ());
}

Led_FontMetrics	TextImager::GetFontMetricsAt (
													#if		qMultiByteCharacters
														size_t charAfterPos
													#else
														size_t /*charAfterPos*/
													#endif
											) const
{
	Tablet_Acquirer	tablet (this);
	Led_AssertNotNil (static_cast<Led_Tablet> (tablet));

	#if		qMultiByteCharacters
		Assert_CharPosDoesNotSplitCharacter (charAfterPos);
	#endif

	Led_FontSpecification	fontSpec	=	GetDefaultFont ();

	FontCacheInfoUpdater	fontCacheUpdater (this, tablet, fontSpec);
	return (fCachedFontInfo);
}


#if		qLedUsesNamespaces
}
#endif




// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

