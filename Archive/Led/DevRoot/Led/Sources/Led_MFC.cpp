/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/Led_MFC.cpp,v 2.95 2003/05/15 12:50:29 lewis Exp $
 *
 * Changes:
 *	$Log: Led_MFC.cpp,v $
 *	Revision 2.95  2003/05/15 12:50:29  lewis
 *	SPR#1487: Add a 'GetEnabled' method to TextInteractor::CommandUpdater and subclasses (to help with SPR)
 *	
 *	Revision 2.94  2003/04/03 21:52:47  lewis
 *	SPR#1407: more cleanups of new CommandNumberMapping<> stuff and other new command# processing support
 *	
 *	Revision 2.93  2003/04/03 16:41:26  lewis
 *	SPR#1407: First cut at major change in command-processing classes. Instead of using templated command classes, just builtin to TextInteractor/WordProcessor (and instead of template params use new TextInteractor/WordProcessor::DialogSupport etc
 *	
 *	Revision 2.92  2003/02/03 20:56:07  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.91  2003/02/03 15:13:04  lewis
 *	SPR#1291 - Added qLedCheckCompilerFlagsConsistency to check for mismatches on compile flags
 *	
 *	Revision 2.90  2003/01/28 23:21:20  lewis
 *	SPR#1262- DoDeclare_Led_MFC_Helper_MessageMap(CWnd,TextInteractor) etc (for CWnd)
 *	
 *	Revision 2.89  2003/01/15 15:18:04  lewis
 *	SPR#1239 - qProvideLedStubsForOLEACCDLL to handle OLEACC.DLL dependency in MFC 7.0 (VC.NET)
 *	
 *	Revision 2.88  2003/01/11 19:28:41  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.87  2002/10/25 17:16:11  lewis
 *	SPR#1146 - Added 'Insert Symbol' menu item and fix Led_MFC D&D code so it worked with CharacterMap.exe
 *	(for some wierd reason - I need to hack it to pass along the full FORMATC???).
 *	
 *	Revision 2.86  2002/09/19 14:14:31  lewis
 *	Lose qSupportLed23CompatAPI (and related backward compat API tags like qOldStyleMarkerBackCompatHelperCode,
 *	qBackwardCompatUndoHelpers, etc).
 *	
 *	Revision 2.85  2002/05/06 21:33:45  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.84  2001/11/27 00:29:51  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.83  2001/10/17 20:42:58  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.82  2001/10/13 19:52:37  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.81  2001/10/13 19:50:04  lewis
 *	SPR#1059- moved some Led_DrawText<> / Led_GetTextExtent<> code to
 *	shared code in LedHandySimple - GetTextExtent<>/DrawTextBox<>, and docs cleanups
 *	
 *	Revision 2.80  2001/09/26 15:41:19  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.79  2001/09/05 16:24:39  lewis
 *	SPR#1016- cleanup semantics of ReaderFlavorPackage::GetFlavorSize/ReaderFlavorPackage::ReadFlavorData
 *	
 *	Revision 2.78  2001/08/29 23:36:32  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.77  2001/08/28 18:43:35  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.76  2000/04/25 19:01:35  lewis
 *	Made GetFlavorSize/ReadFlavorData() CONST
 *	
 *	Revision 2.75  2000/04/24 17:29:22  lewis
 *	SPR#0743- added new Led_MFC_WordProcessor module - and broke some stuff from Led_MFC out and into it
 *	
 *	Revision 2.74  2000/04/16 14:39:36  lewis
 *	trivial tweeks to get compiling on Mac
 *	
 *	Revision 2.73  2000/04/14 22:40:25  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.72  2000/04/14 19:13:38  lewis
 *	SPR##0739- react breakout into FlavorPackage code of that stuff from TextInteractor
 *	
 *	Revision 2.71  2000/04/04 20:49:57  lewis
 *	SPR# 0732- added  Led_Set_OutOfMemoryException_Handler/Led_Set_BadFormatDataException_Handler/
 *	Led_Set_ThrowOSErrException_Handler. And qGenerateStandardPowerPlantExceptions and qGenerateStandardMFCExceptions
 *	defines. The idea is to make it easier to override these exception throw formats, and get better default behavior
 *	for cases other than MFC and PowerPlant.
 *	
 *	Revision 2.70  2000/03/28 03:41:04  lewis
 *	more work on qOldStyleMarkerBackCompatHelperCode/SPR#0718. Now StyleMarker class takes extra RunElement argument.
 *	Now this new code is called by default StyledTextImager::DrawText() etc overrides. And now HiddenText code
 *	properly merges at least its own drawing and StandardStyledTextImager::StyleMarker. All this respecting
 *	qOldStyleMarkerBackCompatHelperCode - so old code (not new hidden text code) should still compile and
 *	work roughly as before with this turned on.
 *	
 *	Revision 2.69  1999/12/28 14:59:48  lewis
 *	SPR#0688- added Led_DrawText<TRIVIALWORDWRAPPEDTEXTIMAGER,SIMPLETEXTIMAGER,TEXTSTORE> ETC
 *	
 *	Revision 2.68  1999/12/21 20:55:26  lewis
 *	Use new Led_ThrowIfNull instead of manual check
 *	
 *	Revision 2.67  1999/12/09 17:26:40  lewis
 *	fix code for proper use of Led_SDK_String/LED_TCHAR_OF in Led_MFC_ControlItem etc
 *	
 *	Revision 2.66  1999/11/29 18:57:48  lewis
 *	DoDeclare_Led_MFC_Helper_MessageMap now takes 2 args - INCOMPATABLE CHANGE
 *	
 *	Revision 2.65  1999/11/15 21:27:13  lewis
 *	use Led_MFC_CDCFromTablet - since now Led_Tablet not a typdef to CDC*
 *	
 *	Revision 2.64  1999/11/13 16:32:21  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.63  1999/07/16 22:23:19  lewis
 *	remove uneeded ; chars which generated warnings
 *	
 *	Revision 2.62  1999/06/15 04:02:20  lewis
 *	spr#0585- break D&D support out of Led_MFC_CViewHelper, and into separate Led_MFC_DragAndDropWindow<>
 *	
 *	Revision 2.61  1999/05/03 22:05:08  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.60  1999/05/03 21:41:28  lewis
 *	Misc minor COSMETIC cleanups - mostly deleting ifdefed out code
 *	
 *	Revision 2.59  1999/03/12 16:01:13  lewis
 *	add spaces to the message map macros around '>' allowed me to undo hacks/typedefs to Declare_Led_MFC_XXX calls
 *	
 *	Revision 2.58  1999/03/08 22:44:42  lewis
 *	Break out RTFInfo stuff which was really just a repository for type declarations, and store
 *	that into new type RTFIO
 *	
 *	Revision 2.57  1999/03/07 14:38:52  lewis
 *	massive re-organization of Led_MFC class. Now its a trivial typedef. And all the old code has been
 *	broken into a series of separate, but individually selectable/combinable templates. Not only
 *	allowing you to only compile in the features you want, but also to select a different
 *	baseclass (CWnd instead of CView, for example. SPR#0557
 *	
 *	Revision 2.56  1999/03/02 22:39:27  lewis
 *	fix spr#0554- had h/v reversed reading in OLE objects and scaling, and had NO code to write out sizes.
 *	Still not working great (see spr#0555), but much better (added Led_MFC_ControlItem::GetSize() to help as well)
 *	
 *	Revision 2.55  1999/03/01 15:08:54  lewis
 *	Added Led_MFC::Get/SetH/VScrollInfo () methods, to replace old qMakeWindowsScrollbarAppearOnlyIfNeeded
 *	compile-time define, and don't assume WS_VSCROLL etc set by caller - we set automagically based on this
 *	new varaible. And changed API for SetV/HScrollInfo (INCOMPATABLY). All to allow autoshowhide on a
 *	per-instance basis of sbars, and more conveniently for programmer (using Led)
 *	
 *	Revision 2.54  1999/02/21 23:02:14  lewis
 *	revised AcquireTablet method to be more exception friendly (dont increment count if we throw). And
 *	Led_MFC::UpdateScrollBars () CAN be called when fPrintInfo != NULL, so just ignore then, don't assert
 *	
 *	Revision 2.53  1999/01/26 15:21:13  lewis
 *	Fixed  qMakeWindowsScrollbarAppearOnlyIfNeeded - only set SIF_DISABLENOSCROLL if FALSE
 *	
 *	Revision 2.52  1998/10/30 14:30:29  lewis
 *	Use vector<> instead of Led_Array.
 *	Fix bug with printing subranges of pages - not starting at one.
 *	Support MFC60.
 *	New MSVC60 compiler warning support.
 *	
 *	Revision 2.51  1998/07/24  01:08:36  lewis
 *	Docs, fix bad call to LEd_ThrowOUtOfMemory (clipboard thing). SetVScrollInfo() etc now VIRTUAL,
 *	and can be overriden.
 *	Led_MFC::GetCurClickClickCount (). (ETC on above SetVSCrollInfo thing - lots of related changes).
 *	Led_MFC::CalculatePrintingRect () NEW - and other printing fixes to facilitate things like margins/
 *	titles in printouts (not done
 *	in my applets, for for LEC applets).
 *
 *	Revision 2.50  1998/06/03  01:32:35  lewis
 *	qWorkAroundWin95BrokenUNICODESupport
 *
 *	Revision 2.49  1998/05/05  00:30:50  lewis
 *	Support MWERKS CWPro3 - and lose old bug workarounds.
 *
 *	Revision 2.48  1998/04/25  01:30:53  lewis
 *	UNICODE support - LED_TCHAR_OF, and a couple MultiByteToWideChar () calls.
 *
 *	Revision 2.47  1998/03/04  20:21:54  lewis
 *	::WriteClassStg() call added, and add TabletCHangedMetrics call.
 *
 *	Revision 2.46  1997/12/24  04:40:13  lewis
 *	*** empty log message ***
 *
 *	Revision 2.45  1997/12/24  03:31:27  lewis
 *	In OnCreate - do the register of the fDropTarget (was being done in the LedItView subclass -
 *	etc - silly!).
 *	And big addition - Led_MFC_ControlItem now implements the RTFOleEmebdding API - and does alot
 *	of work to allow these
 *	embedings to write into and be created from RTF text.
 *
 *	Revision 2.44  1997/09/29  15:03:48  lewis
 *	Lose qSupportLed21CompatAPI and qLedFirstIndex support.
 *
 *	Revision 2.43  1997/07/27  15:58:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.42  1997/07/23  23:08:51  lewis
 *	Major reworking/fixup of scrolling (just calls to Win32 SDK code for scrollbar -
 *	old SCROLLINFO code wasn't quite right).
 *
 *	Revision 2.41  1997/07/15  05:26:36  lewis
 *	AutoDoc content.
 *
 *	Revision 2.40  1997/07/14  15:55:46  lewis
 *	*** empty log message ***
 *
 *	Revision 2.39  1997/07/14  01:18:09  lewis
 *	Renamed Led_MFC_ to Led_MFC, TextInteractor_ to TextInteractor, etc.
 *	Support qLedFirstIndex == 0.
 *	Command names use string class instead of char* now.
 *
 *	Revision 2.38  1997/06/28  17:15:22  lewis
 *	Fix infintite recursion bug with Led_MFC::GetWindowRect(LPRECT)
 *
 *	Revision 2.37  1997/06/24  03:30:58  lewis
 *	Lose obsolete SetLayoutWidth () calls/Support.
 *
 *	Revision 2.36  1997/06/18  20:08:49  lewis
 *	*** empty log message ***
 *
 *	Revision 2.35  1997/06/18  02:56:54  lewis
 *	Lose HandleSharedMouseMoveUpdateSelCode () in favor of better, and code/sahred
 *	WhileSimpleMouseTracking ().
 *	qUseAnisotropicModeForWindowsPrinting always true.
 *	qIncludePrefixFile support.
 *	qLedAssertsDefaultToMFCAsserts support.
 *	Dont call Get/SetLayoutWidth in printing code.
 *
 *	Revision 2.34  1997/03/23  03:06:46  lewis
 *	on dblclick message, just incremnet click count, don't set absolutely to two - cuz doesn't work well
 *	on quadruple click (still want to extend row selection in that case).
 *
 *	Revision 2.33  1997/03/23  00:49:01  lewis
 *	Some fixes to smart cut and paste.
 *	But mostly redid the mouse-based-selection to use new WhileTrackingConstrainSelection ()
 *	support. MUCH clearer now!
 *	Also new 32bit scrolling code (including page size button support).
 *	Better scrolling speed tuning.
 *	(qDynamiclyChooseAutoScrollIncrement).
 *
 *	Revision 2.32  1997/03/04  20:09:54  lewis
 *	Add support for SmartCutAndPasteMode with DragNDrop.
 *
 *	Revision 2.31  1997/01/20  06:23:09  lewis
 *	*** empty log message ***
 *
 *	Revision 2.30  1997/01/20  05:24:07  lewis
 *	Support for new eDefaultUpdate UpdateMode.
 *	Support for new text color parameter in FontSpecification - for SetTextColor() calls.
 *
 *	Revision 2.29  1997/01/10  03:18:55  lewis
 *	Lots of changes. Mostly first cut at scrolling support. Also redid mixin code. Now templated.
 *
 *	Revision 2.28  1996/12/13  17:58:46  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.27  1996/12/05  19:11:38  lewis
 *	*** empty log message ***
 *
 *	Revision 2.26  1996/10/31  00:25:35  lewis
 *	Add support for WinMessaging hooks into UNDO (EM_UNDO, etc).
 *	Add qUseAnisotropicModeForWindowsPrinting printing support, and fix print preview disabling button bug.
 *
 *	Revision 2.25  1996/09/30  14:23:58  lewis
 *	New AdjustBounds->InvalidateScrollBarparameters() suppoortr. (and UpdateScrollbars() etc.)
 *	And support qSupportEnterIdleCallback.
 *
 *	Revision 2.24  1996/09/03  15:05:13  lewis
 *	cleanup - lose comment out code
 *
 *	Revision 2.23  1996/09/01  15:31:50  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.22  1996/08/05  06:07:40  lewis
 *	OnSetCursor(), should set to arrow when over text we could drag.
 *
 *	Revision 2.21  1996/07/19  16:35:44  lewis
 *	Handle case of D&D from one window to another within the same app
 *	as the same as CROSS-APP. (basicly fixed log buf with global varaible).
 *
 *	Revision 2.20  1996/06/01  02:08:43  lewis
 *	Massive changes.
 *	Moved the OleClientItem stuff here (Led_MFC_ControlItem) from LedIt application.
 *	Lose no RTTI support.
 *	Fixup drag/mouse move timer tracking etc.
 *	Lose OnDropFiles () stuff
 *	Fix must undo-boundary bugs.
 *	Fix (must above sb misc.) various word-selection/space-selection bugs.
 *	Fix couple of printing bugs.
 *
 *	Revision 2.19  1996/05/23  19:34:51  lewis
 *	Put in DragAcceptFiles/OnDropFiles stuff. But probably in error. I know
 *	handle this elsewhere. Probably should get rid of it.
 *	Use new Led_IncrementalFontSpecification support. Couple more minor
 *	cleanups.
 *
 *	Revision 2.18  1996/05/14  20:26:13  lewis
 *	ScrollByIfRoom now takes eImmediateUpdate param in places where we scroll
 *	(so we invoke new scrollbits code).
 *
 *	Revision 2.17  1996/05/04  21:04:21  lewis
 *	added debugging fragment BeginEnumFormats/GetNextFormat) to debug
 *	D&D stuff (ifdefed out - only hack to enable when debugging).
 *
 *	Revision 2.16  1996/04/18  15:20:54  lewis
 *	Massive changes.
 *	Moved OLE / Drag/Drop stuff here from LedIt!
 *	And mostly reimpelmented all that stuff anyhow ontop of new Led classlib
 *	internalize/externalize flavor stuff. Now works right with undo.
 *	And drag and drop and cut/and/paste of embeddings (and undo of said) now work.
 *
 *	Revision 2.15  1996/03/16  18:58:11  lewis
 *	Override OnCreate() to call TabletChangedMetrics() (now sets initial tab value)
 *
 *	Revision 2.14  1996/03/04  07:45:36  lewis
 *	Call ProcessSimpleClick hook, which allows click to invoke open
 *	and select properly for embeddings (not fully taken advantage of here
 *	yet).
 *
 *	Revision 2.13  1996/02/26  22:32:31  lewis
 *	Renamed TextInteracter --> TextInteractor.
 *	StandardStyledWordWrappedLed_MFC::OnBeginPrinting/EndPrinting to save
 *	and restore force all rows shown flag.
 *	Led_MFC_::PrintInfo::PrintInfo ()
 *	Led_Min etc instead of Min().
 *	Cleaned up OnCommandPaste support a bit based on changes in TextInteractor.
 *	Cleaned up some printing stuff - though still somewhat buggy.
 *
 *	Revision 2.12  1996/02/05  04:18:34  lewis
 *	Get rid of SpecifyTextStore and arg TextStore stuff. Now handled in
 *	class Led.
 *
 *	Revision 2.11  1996/01/22  05:17:47  lewis
 *	Lots of changes - use Led_Verify instead of VERIFY.
 *	Lose any VERIFY on FontUpdater::CTOR for grabing old values for CDC -
 *	cuz sometimes failed for printer DC. Not obviously a bug. Probably OK.
 *	Started (but not finished) updating message-based UNDO code to use
 *	new Led-CommandHandler UNDO support.
 *	Fixed bug in tracking during mouse down I had added inadvertantly on my
 *	previous round of word select fixes.
 *	Printing support - not perfect - but  OK to start.
 *	Added a few Break/Commandhandler calls so typing and cut/paste not groupe
 *	together. Must find a better way to handle this!
 *
 *	Revision 2.10  1996/01/11  08:17:31  lewis
 *	Reimplement word selection support, fixing bugs in old code, and additionally
 *	supporting new qDoubleClickSelectsSpace code.
 *
 *	Revision 2.9  1996/01/03  23:58:22  lewis
 *	code cleanups
 *
 *	Revision 2.8  1995/12/13  05:56:15  lewis
 *	Lose qUseLocalBoolFlagToKeepTrackOfShowHideCaretForNT
 *
 *	Revision 2.7  1995/12/09  05:37:42  lewis
 *	Work around MSVC 4.0 (and earlier) bugs with vtables before object
 *	complete by constructing default textstore in ctor body (really this
 *	doesn't help much). Also, we can cleanup on throw better now.
 *
 *	Revision 2.6  1995/12/06  01:32:35  lewis
 *	Fix const_cast usage - const cast can only change const, not also change
 *	type - even for legal promotions.
 *
 *	Revision 2.5  1995/11/25  00:24:45  lewis
 *	Hacked in new OnPasteCommand() support. Not well done. Should be
 *	cleaned up.
 *
 *	Revision 2.4  1995/11/02  22:24:24  lewis
 *	Check if window rect empty and dont call Redraw/Update windowrect.
 *	And lose setting of default font - now taken care of by code in
 *	TextImager::GetDefaultStaticFont()
 *
 *	Revision 2.3  1995/10/19  22:21:14  lewis
 *	Add new StandardStyledWordWrappedLed_MFC.
 *	Lose old qWin32s_EM_Messages_Broken workarounds.
 *
 *	Revision 2.2  1995/10/09  22:29:21  lewis
 *	Lose unneeded Replace overrides
 *
 *	Revision 2.1  1995/09/06  20:57:00  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.33  1995/06/08  05:13:53  lewis
 *	Code cleanups.
 *	Changed timeout on autoscroling timer (SPR#0326).
 *	Cleanup Led_MFC_ default font setting code in CTOR (SPR#0327).
 *	Remove hack/work around for MSVC compiler bug in
 *	SimpleLed_MFC::Invariant_ ()/SimpleLed_ChunkedArray_MFC::Invariant_ ();
 *	SPR#0323.
 *
 *	Revision 1.32  1995/06/02  06:32:53  lewis
 *	Fix problem with fix for SPR 145 - autoscroll ending selection.
 *
 *	Revision 1.31  1995/06/02  06:22:48  lewis
 *	Fix UpdateModes for DoCursorEdit calls (SPR # 0307).
 *	Fix what we select on autoscroll out of window (SPR#145).
 *
 *	Revision 1.30  1995/05/31  09:38:31  lewis
 *	Use new ClaculateCaretRect() routine (SPR 303) instead of manual caret
 *	calculation. Use its height and windth in createSoldiCaret() call.
 *	Most importalyn, now use IT in call to RefreshWindowRect() in UpdateCaretState -
 *	had several compensating bugs before - Refresh_ call instead of RecreshWIndowRect
 *	made up for the fact that we were often (on EOR) passing empty caret Rect!
 *	Now we pass teh right one in all cases.
 *
 *	Revision 1.29  1995/05/31  08:10:48  lewis
 *	Another oops - thats what I get for not testing this stuff on the PC first!
 *
 *	Revision 1.28  1995/05/31  08:06:08  lewis
 *	Opps - Use InvalidateRect instead of Invalidate() and RedrawWindow(RECT,flag)
 *	rather than UpdateWindow() - later has no form taking rect arg!!!
 *
 *	Revision 1.27  1995/05/31  07:34:44  lewis
 *	Call RefreshWindowRect() not Refresh() in UpdateCaretPos!
 *	Fixed Update_ and Refresh_/RefreshWindowRect_ to actually use
 *	the RECTANGLE provided. Old tmphack never fixed til now - SPR 0302.
 *
 *	Revision 1.26  1995/05/30  00:07:03  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.25  1995/05/29  23:54:04  lewis
 *	Added if (CheckIfDraggingBeepAndReturn ()) {  return; } in a number of
 *	keystroke processing places to fix SPR 0299.
 *	Use const_cast macro - SPR 0289
 *
 *	Revision 1.24  1995/05/25  09:50:33  lewis
 *	fAllocatedTablet now a direct member (CDC instead of CWindowDC) for SPR 0286 (speed / fragmentation).
 *
 *	Revision 1.23  1995/05/24  07:24:40  lewis
 *	Work around qUseLocalBoolFlagToKeepTrackOfShowHideCaretForNT
 *	 SPR 0277
 *
 *	Revision 1.22  1995/05/21  17:53:47  lewis
 *	Use qLedFirstIndex - SPR 0272
 *	Moved qLotsOfSubclassesFromMixinPointYeildTemporarilyBadVTable back here
 *	since need concrete TextStore.
 *	Lose InsertAfter/DeleteAfter (SPR 0270).
 *	Fix OnGetDlgCode() to look at WS_WANTSRETURN - SPR 0274.
 *
 *	Revision 1.21  1995/05/20  05:07:50  lewis
 *	Huge changes and cleanups. Mainly for SPR 0261- uses WordWrappedTextIteracter
 *
 *	to sahre code.
 *	Also some cahnges for 0263 - WindowRect/LayourRect cahnge
 *	Many cleanups - 0262.
 *	fided LedMFC::OnDraw() - had no logical clipping -re-enabled.SPR 0259.
 *
 *	Revision 1.20  1995/05/18  08:19:03  lewis
 *	Fix infinite loop (maybe?) in ::ShowCaret () calls (ugly MS API!) - SPR#0254.
 *	Implemented CONTROL-PAGEUP/DOWN for PC - LedSPR#0255
 *	Add define qMakeWindowsScrollbarAppearOnlyIfNeeded and make default to true.
 *	This supports funny behavior in WordForWindows adjusting selection during
 *	page up/down operations. - LedSPR#0248.
 *
 *	Revision 1.19  1995/05/16  06:43:15  lewis
 *	Call new InteractiveReplace() instead of Replace ().
 *
 *	Revision 1.18  1995/05/12  21:13:21  lewis
 *	Cleanup the MFC Serialize/CArchive support.
 *	FIx bug with SetWindowText(NULL) - OK - means make empty.
 *	Added overrides for OnNcCalcSize/OnGetDlgCode/OnNcPaint for
 *	WS_BORDER fixes. Maybe didn't need the NC overrides. OnGetDlgCode ()
 *	was pre-emptive fix to make sure I returned same answer as other
 *	TEs in windows.
 *	Add new define qMakeWindowsScrollbarAppearOnlyIfNeeded for PC (default false)
 *	and now by default (when false) show sbar iff WS_VSCROLL style - which
 *	seems to be the new windows stanard.
 *	Seems like overrides of NC routines may not be needed. Maybe windows does
 *	WS_BORDER autoamagically - except for EDITTEXTS? We'll see.
 *	Also - fixed subclasswindow guy to delete old and re-create. Seems
 *	far simpler way of assuring things get setup right. THis is what old
 *	CV editor code had done. Otherwise I got funny behavior from
 *	base-class which I really didn't want.
 *
 *	Revision 1.17  1995/05/09  23:41:06  lewis
 *	Lose unused withWhat parameter to Pre/PostReplace - SPR 0240.
 *	Call OnTypedNormalCharacter on character messages, and provide
 *	QueryInputKeyStrokesPending () override so we can vector our on-typing
 *	code from Led_TCL/Led_MFC to TextInteractor_.
 *	LedSPR#0242.
 *
 *	Revision 1.16  1995/05/08  03:48:38  lewis
 *	Fixed PC-only typos.
 *
 *	Revision 1.15  1995/05/08  03:24:27  lewis
 *	Lots of fixes to ScrollSoSowing etc for SPR 0236.
 *
 *	Revision 1.14  1995/05/06  19:51:03  lewis
 *	AdjustBounds with m_hWnd == NULL fix - SPR 0229.
 *	Renamed GetByteLength->GetLength () - SPR #0232.
 *	Use Led_tChar instead of char for UNICODE support - SPR 0232.
 *	qPeekForMoreCharsOnUserTyping fix - SPR 0231.
 *
 *	Revision 1.13  1995/05/05  19:49:54  lewis
 *	Led_SmallStackBuffer now templated class.
 *
 *	Revision 1.12  1995/04/18  01:58:27  lewis
 *	Use Led_IME::Get() rather than gIME
 *
 *	Revision 1.11  1995/04/18  00:15:15  lewis
 *	Simplify code for call toetCaretShownAfterPos in mouse click routine.
 *
 *	Revision 1.10  1995/04/16  19:30:35  lewis
 *	Get rid of SimpleTextInteractor - SPR # 205.
 *	Support for setting flag about where to display caret - SPR 204.
 *	More cleanups.
 *
 *	Revision 1.9  1995/03/29  21:09:04  lewis
 *	Lots of changes - include UpdateCaretState/InvalidateCaretState change-SPR0193.
 *	Some clipboard error handling better - SPR 0192.
 *
 *	Revision 1.8  1995/03/24  01:27:37  lewis
 *	Redid Show/Hide Caret code (SPR 0186).
 *
 *	Revision 1.7  1995/03/23  03:49:09  lewis
 *	Fixed args and implementation of SimpleLed_ChunkedArray_MFC::SelectWholeLineAfter ()
 *	and SimpleLed_MFC::SelectWholeLineAfter (). See TextInteractor_ version.
 *	Get rid of asserts on UNDO SendMessage() wrappers, and do callbacks
 *	for the 3 undo routines. Just have CanUndo() always return false, and then
 *	OK to assert Undo() never called.
 *	SPRs 176, 177.
 *
 *	Revision 1.6  1995/03/17  03:06:22  lewis
 *	Fixed scrolling (sprs #171, #147, #148). Fixes all (I hope) pc scrolling
 *	problems.
 *
 *	Revision 1.5  1995/03/13  03:21:22  lewis
 *	Prefixed some utility routine names in LedSupport with Led_ to avoid
 *	name conflicts.
 *
 *	Revision 1.4  1995/03/06  21:32:55  lewis
 *	On HOME/END keys - do appropriate cursoring.
 *
 *	Revision 1.3  1995/03/03  15:55:49  lewis
 *	Ignore ESC character in Led_MFC::OnChar() - this is what Windows Notepad
 *	and MS Word do (SPR #150).
 *
 *	Revision 1.2  1995/03/02  06:01:28  lewis
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

#include	<afxext.h>
#include	<afxole.h>

#include	"Led_MFC.h"



#if		qSupportDrawTextGetTextExtent

#include	"WordWrappedTextImager.h"
#include	"SimpleTextImager.h"
#include	"SimpleTextStore.h"

#endif


#if		qLedUsesNamespaces
	namespace	Led {
#endif




#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (4 : 4786)		//qQuiteAnnoyingDebugSymbolTruncationWarnings
	#pragma	warning (4 : 4800)		//qUsePragmaWarningToSilenceNeedlessBoolConversionWarnings
#endif





#if		qLedAssertsDefaultToMFCAsserts && qDebug
	static	class	OneTimeLedMFCAssertionFunctionSetter {
		public:
			static	void	MFCAssertionHandler (const char* fileName, int lineNum)
				{
					::AfxAssertFailedLine (fileName, lineNum);
				}
			OneTimeLedMFCAssertionFunctionSetter ()
				{
					Led_SetAssertionHandler (MFCAssertionHandler);
				}
	}	sOneTimeLedMFCAssertionFunctionSetter;
#endif




#if		qLedCheckCompilerFlagsConsistency
	namespace LedCheckCompilerFlags_Led_MFC {
		int	LedCheckCompilerFlags_(qMFCRequiresCWndLeftmostBaseClass)	=	qMFCRequiresCWndLeftmostBaseClass;
	}
#endif









/*
 ********************************************************************************
 *********************** LedSupport class lib support ***************************
 ********************************************************************************
 */
#if		qGenerateStandardMFCExceptions
	static	struct	MFC_MODULE_INIT {
		static	void	DoThrowAfxThrowMemoryException () { AfxThrowMemoryException (); }
		static	void	DoThrowAfxThrowArchiveException () { AfxThrowArchiveException (CArchiveException::badIndex);/* best guess/message/exception I could think of*/ }
		MFC_MODULE_INIT ()
		{
			Led_Set_OutOfMemoryException_Handler (&DoThrowAfxThrowMemoryException);
			Led_Set_BadFormatDataException_Handler (&DoThrowAfxThrowArchiveException);
		}
	}	sMFC_MODULE_INIT;
#endif






#if		qProvideLedStubsForOLEACCDLL
/*
 ********************************************************************************
 ******************************** OLEACC.DLL ************************************
 ********************************************************************************
 */
extern "C"	STDAPI	CreateStdAccessibleObject (HWND hwnd, LONG idObject, REFIID riid, void** ppvObject)
{
	HINSTANCE	oleACCDLL	=	::LoadLibrary (_T ("OLEACC.dll"));
	HRESULT		hr			=	E_FAIL;
	if (oleACCDLL != NULL) {
		HRESULT	 (WINAPI*	pCreateStdAccessibleObject) (HWND, LONG, REFIID, void**) =
			(HRESULT (WINAPI *) (HWND, LONG, REFIID, void**))
							(::GetProcAddress (oleACCDLL, "CreateStdAccessibleObject"));
		if (pCreateStdAccessibleObject != NULL) {
			hr = (pCreateStdAccessibleObject) (hwnd, idObject, riid, ppvObject);
		}
		Led_Verify (::FreeLibrary (oleACCDLL));
	}
	return hr;
}

extern "C"	STDAPI	AccessibleObjectFromWindow (HWND hwnd, DWORD dwId, REFIID riid, void **ppvObject)
{
	HINSTANCE	oleACCDLL	=	::LoadLibrary (_T ("OLEACC.dll"));
	HRESULT		hr			=	E_FAIL;
	if (oleACCDLL != NULL) {
		HRESULT	 (WINAPI*	pAccessibleObjectFromWindow) (HWND, DWORD, REFIID, void**) =
			(HRESULT (WINAPI *) (HWND, DWORD, REFIID, void**))
							(::GetProcAddress (oleACCDLL, "AccessibleObjectFromWindow"));
		if (pAccessibleObjectFromWindow != NULL) {
			hr = (pAccessibleObjectFromWindow) (hwnd, dwId, riid, ppvObject);
		}
		Led_Verify (::FreeLibrary (oleACCDLL));
	}
	return hr;
}

extern "C"	STDAPI_(LRESULT)	LresultFromObject (REFIID riid, WPARAM wParam, LPUNKNOWN punk)
{
	HINSTANCE	oleACCDLL	=	::LoadLibrary (_T ("OLEACC.dll"));
	HRESULT		hr			=	E_FAIL;
	if (oleACCDLL != NULL) {
		LRESULT	 (WINAPI*	pLresultFromObject) (REFIID, WPARAM, LPUNKNOWN) =
			(LRESULT (WINAPI *) (REFIID, WPARAM, LPUNKNOWN))
							(::GetProcAddress (oleACCDLL, "LresultFromObject"));
		if (pLresultFromObject != NULL) {
			hr = (pLresultFromObject) (riid, wParam, punk);
		}
		Led_Verify (::FreeLibrary (oleACCDLL));
	}
	return hr;
}
#endif







/*
 ********************************************************************************
 *********************************** Led_MFC ************************************
 ********************************************************************************
 */

DoDeclare_Led_MFC_Helper_MessageMap(CView,TextInteractor)
DoDeclare_Led_MFC_MimicMFCAPIHelper_MessageMap(Led_MFC_Helper<CView>)
DoDeclare_Led_MFC_OptionalWin32SDKMessageMimicHelper_MessageMap(Led_MFC_MimicMFCAPIHelper<Led_MFC_Helper<CView> >)
DoDeclare_Led_MFC_DragAndDropWindow_MessageMap(Led_MFC_OptionalWin32SDKMessageMimicHelper<Led_MFC_MimicMFCAPIHelper<Led_MFC_Helper<CView> > >)
DoDeclare_Led_MFC_CViewHelper_MessageMap(Led_MFC_DragAndDropWindow<Led_MFC_OptionalWin32SDKMessageMimicHelper<Led_MFC_MimicMFCAPIHelper<Led_MFC_Helper<CView> > > >)


DoDeclare_Led_MFC_Helper_MessageMap(CWnd,TextInteractor);
DoDeclare_Led_MFC_MimicMFCAPIHelper_MessageMap(Led_MFC_Helper<CWnd>)
DoDeclare_Led_MFC_OptionalWin32SDKMessageMimicHelper_MessageMap(Led_MFC_MimicMFCAPIHelper <Led_MFC_Helper<CWnd> >)






/*
 ********************************************************************************
 ******************** Led_MFCReaderDAndDFlavorPackage ***************************
 ********************************************************************************
 */
Led_MFCReaderDAndDFlavorPackage::Led_MFCReaderDAndDFlavorPackage (COleDataObject* dataObject):
	ReaderFlavorPackage (),
	fDataObject (dataObject)
{
	Led_RequireNotNil (dataObject);
}

bool	Led_MFCReaderDAndDFlavorPackage::GetFlavorAvailable (Led_ClipFormat clipFormat) const
{
	Led_AssertNotNil (fDataObject);
	return fDataObject->IsDataAvailable (clipFormat);
}

size_t	Led_MFCReaderDAndDFlavorPackage::GetFlavorSize (Led_ClipFormat clipFormat) const
{
	// is there some more efficeint way todo this? - LGP 960410
	HGLOBAL	hObj	=	fDataObject->GetGlobalData (clipFormat);
	if (hObj == NULL) {
		// Some D&D sources - e.g. "Character Map" application on WinXP - return NULL for GetGlobalData()
		// UNLESS you pass along the full FORMATC. No idea why... SPR#1146
		FORMATETC	formatC;
		fDataObject->BeginEnumFormats ();
		while (fDataObject->GetNextFormat (&formatC)) {
			if (formatC.cfFormat == clipFormat) {
				hObj = fDataObject->GetGlobalData (clipFormat, &formatC);
			}
		}
	}
	if (hObj == NULL) {
		return 0;
	}
	size_t	result	=	::GlobalSize (hObj);
	::GlobalFree (hObj);
	return result;
}

size_t	Led_MFCReaderDAndDFlavorPackage::ReadFlavorData (Led_ClipFormat clipFormat, size_t bufSize, void* buf) const
{
	HGLOBAL	hObj		=	fDataObject->GetGlobalData (clipFormat);
	if (hObj == NULL) {
		// Some D&D sources - e.g. "Character Map" application on WinXP - return NULL for GetGlobalData()
		// UNLESS you pass along the full FORMATC. No idea why... SPR#1146
		FORMATETC	formatC;
		fDataObject->BeginEnumFormats ();
		while (fDataObject->GetNextFormat (&formatC)) {
			if (formatC.cfFormat == clipFormat) {
				hObj = fDataObject->GetGlobalData (clipFormat, &formatC);
			}
		}
	}
	size_t	sizeCopied	=	0;
	if (hObj != NULL) {
		size_t	realSize	=	::GlobalSize (hObj);
		sizeCopied	=	Led_Min (realSize, bufSize);
		(void)::memcpy (buf, ::GlobalLock (hObj), sizeCopied);
		::GlobalUnlock (hObj);
		::GlobalFree (hObj);
	}
	Led_Ensure (sizeCopied <= bufSize);
	return sizeCopied;
}

COleDataObject*	Led_MFCReaderDAndDFlavorPackage::GetOleDataObject () const
{
	return fDataObject;
}







/*
 ********************************************************************************
 ******************** Led_MFCWriterDAndDFlavorPackage ***************************
 ********************************************************************************
 */
Led_MFCWriterDAndDFlavorPackage::Led_MFCWriterDAndDFlavorPackage (COleDataSource* dataObject):
	WriterFlavorPackage (),
	fDataObject (dataObject)
{
	Led_RequireNotNil (dataObject);
}

void	Led_MFCWriterDAndDFlavorPackage::AddFlavorData (Led_ClipFormat clipFormat, size_t bufSize, const void* buf)
{
	HGLOBAL	dataHandle = ::GlobalAlloc (GMEM_DDESHARE, bufSize);
	Led_ThrowIfNull (dataHandle);
	char*	lockedMem	=	(char*)::GlobalLock (dataHandle);
	Led_AssertNotNil (lockedMem);
	memcpy (lockedMem, buf, bufSize);
	::GlobalUnlock (lockedMem);
	fDataObject->CacheGlobalData (clipFormat, dataHandle);

	// Note we needn't delete 'dataHandle' cuz puts the handle into a
	// stgMedium, and on desctuction of ole data source, it calls stgfree(or some such)
	// which appears to delte the global handle...
	// LGP 960412
}

COleDataSource*	Led_MFCWriterDAndDFlavorPackage::GetOleDataSource () const
{
	return fDataObject;
}








/*
 ********************************************************************************
 ***************************** MFC_CommandNumberMapping *************************
 ********************************************************************************
 */
MFC_CommandNumberMapping::MFC_CommandNumberMapping ()
{
	AddAssociation (ID_EDIT_UNDO, TextInteractor::kUndo_CmdID);
	AddAssociation (ID_EDIT_REDO, TextInteractor::kRedo_CmdID);
	AddAssociation (ID_EDIT_SELECT_ALL, TextInteractor::kSelectAll_CmdID);
	AddAssociation (ID_EDIT_CUT, TextInteractor::kCut_CmdID);
	AddAssociation (ID_EDIT_COPY, TextInteractor::kCopy_CmdID);
	AddAssociation (ID_EDIT_PASTE, TextInteractor::kPaste_CmdID);
	AddAssociation (ID_EDIT_CLEAR, TextInteractor::kClear_CmdID);
	AddAssociation (ID_EDIT_FIND, TextInteractor::kFind_CmdID);
}








/*
 ********************************************************************************
 ****************************** Led_MFC_TmpCmdUpdater ***************************
 ********************************************************************************
 */
Led_MFC_TmpCmdUpdater::CommandNumber	Led_MFC_TmpCmdUpdater::GetCmdID () const
{
	return fCmdNum;
}

bool	Led_MFC_TmpCmdUpdater::GetEnabled () const
{
	return fEnabled;
}

void	Led_MFC_TmpCmdUpdater::SetEnabled (bool enabled)
{
	Led_EnsureNotNil (fCmdUI);
	fCmdUI->Enable (enabled);
	fEnabled = enabled;
}

void	Led_MFC_TmpCmdUpdater::SetChecked (bool checked)
{
	Led_EnsureNotNil (fCmdUI);
	fCmdUI->SetCheck (checked);
}

void	Led_MFC_TmpCmdUpdater::SetText (const Led_SDK_Char* text)
{
	Led_EnsureNotNil (fCmdUI);
	fCmdUI->SetText (text);
}









#if		qSupportDrawTextGetTextExtent
/*
@METHOD:		Led_GetTextExtent
@DESCRIPTION:	<p>Instantiate @'Led_GetTextExtent<WORDWRAPPEDTEXTIMAGER,SIMPLETEXTIMAGER,TEXTSTORE>.
		<p>See also @'qSupportDrawTextGetTextExtent',
	@'Led_DrawText',
	@'Led_GetTextExtent<WORDWRAPPEDTEXTIMAGER,SIMPLETEXTIMAGER,TEXTSTORE>'.</p>
*/
CSize	Led_GetTextExtent (CDC* cdc, const Led_tString& text, CRect r, bool wordWrap)
{
	return Led_GetTextExtent<WordWrappedTextImager, SimpleTextImager, SimpleTextStore> (cdc, text, r, wordWrap);
}

/*
@METHOD:		Led_DrawText
@DESCRIPTION:	<p>Instantiate @'Led_DrawText<WORDWRAPPEDTEXTIMAGER,SIMPLETEXTIMAGER,TEXTSTORE>.</p>
		<p>See also @'qSupportDrawTextGetTextExtent',
	@'Led_GetTextExtent',
	@'Led_DrawText<WORDWRAPPEDTEXTIMAGER,SIMPLETEXTIMAGER,TEXTSTORE>'.</p>
*/
void	Led_DrawText (CDC* cdc, const Led_tString& text, CRect r, bool wordWrap)
{
	Led_DrawText<WordWrappedTextImager, SimpleTextImager, SimpleTextStore> (cdc, text, r, wordWrap);
}
#endif






#if		qLedUsesNamespaces
}
#endif




// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***


