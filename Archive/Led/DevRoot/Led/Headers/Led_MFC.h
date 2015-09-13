/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__Led_MFC_h__
#define	__Led_MFC_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/Led_MFC.h,v 2.167 2004/02/11 22:41:23 lewis Exp $
 */


/*
@MODULE:	Led_MFC
@DESCRIPTION:
		<p>Led MFC specific wrappers.</p>
		<p>Note that since MFC provides no abstract base classes for implementing editor-like
	behavior, Led cannot subclass from CEdit or CEditView.</p>
		<p>Also - for some purposes, it would be better to inherit from CWnd. And for others, better to inherit
	from CView. Versions of Led prior to Led 2.3 inherited from CView.</p>
		<p>Now - through the magic of templates - Led supports (almost) any MFC base class (at least
	CView and CWnd) for a Led-based editor, so long as it supports at least the functionality of CWnd/CView.</p>
		<p>The current Led sample applications (LedIt! and LedLineIt! all use Led inheriting from
	a CView, and that is necessary to get the OLE/embedding funcitonality built in. But Led has also been
	tested as a replacemnt edit control inheriting from CWnd (@LEC).</p>
 */



/*
 * Changes:
 *	$Log: Led_MFC.h,v $
 *	Revision 2.167  2004/02/11 22:41:23  lewis
 *	SPR#1576: update: added 'Replace Again' command to LedIt, LedLineItMFC, ActiveLedIt
 *	
 *	Revision 2.166  2004/02/10 03:05:53  lewis
 *	SPR#1634: Added EnterIdler class to get notified on OnEnterIdle messages. Also - added IdleManager::NonIdleContext class to notify idle-manager of non-idle (eg executing a command) contexts (lots of these usages).
 *	
 *	Revision 2.165  2003/06/03 03:28:05  lewis
 *	SPR#1513: add spellcheck cmd
 *	
 *	Revision 2.164  2003/05/30 03:06:44  lewis
 *	SPR#1517: preliminary version of REPLACE DIALOG support (WIN32 only so far).
 *	
 *	Revision 2.163  2003/05/22 12:29:56  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.162  2003/05/22 02:10:00  lewis
 *	SPR#1498: move Led_GetTickCount call in HandleDragSelect () so you can click out
 *	of a big selection even if the externalize is slow
 *	
 *	Revision 2.161  2003/05/21 12:53:09  lewis
 *	SPR#1495: rename UndoableContextHelper::GetSelStart/End to UndoableContextHelper::GetUndoRegionStart/End
 *	
 *	Revision 2.160  2003/05/20 21:36:38  lewis
 *	Misc cleanups. Plus, code cleanups to D&D code - related to SPR#1493. Lose
 *	SetBeforeUndoSelection. Lose originalSelectionMarker and instead - keep it in
 *	the LedStartDragAndDropContext (fixed bug with preserving selection on D&D).
 *	
 *	Revision 2.159  2003/05/15 12:50:28  lewis
 *	SPR#1487: Add a 'GetEnabled' method to TextInteractor::CommandUpdater and
 *	subclasses (to help with SPR)
 *	
 *	Revision 2.158  2003/05/07 21:10:58  lewis
 *	SPR#1467: implemented various select commands (word/row/paragraph/table/cell/row/col)
 *	
 *	Revision 2.157  2003/05/05 19:00:50  lewis
 *	SPR#1462: Fixed Led_MFC_Helper<>::OnUpdateCommand_MSG to disable commands
 *	not found in target chain.
 *	
 *	Revision 2.156  2003/05/05 13:32:57  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.155  2003/04/18 22:42:59  lewis
 *	Minor tweeks for MSVC80 (VC++ .Net 2003) compatability
 *	
 *	Revision 2.154  2003/04/16 14:04:10  lewis
 *	SPR#1437: cosmetic cleanups - nest code and scope in in braces inside of UndoContext instances,
 *	and use new TextInteractor::BreakInGroupedCommands () wrapper (and other small cleanups)
 *	
 *	Revision 2.153  2003/04/04 19:53:14  lewis
 *	SPR#1407: cleanup new command handling code
 *	
 *	Revision 2.152  2003/04/04 14:41:41  lewis
 *	SPR#1407: More work cleaning up new template-free command update/dispatch code
 *	
 *	Revision 2.151  2003/04/03 22:49:08  lewis
 *	SPR#1407: more work on getting this covnersion to new command handling working
 *	(mostly MacOS/Linux now)
 *	
 *	Revision 2.150  2003/04/03 21:52:44  lewis
 *	SPR#1407: more cleanups of new CommandNumberMapping<> stuff and other new command#
 *	processing support
 *	
 *	Revision 2.149  2003/04/03 16:41:22  lewis
 *	SPR#1407: First cut at major change in command-processing classes. Instead of using templated command
 *	classes, just builtin to TextInteractor/WordProcessor (and instead of template params use new
 *	TextInteractor/WordProcessor::DialogSupport etc
 *	
 *	Revision 2.148  2003/04/01 01:42:43  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.147  2003/04/01 01:41:23  lewis
 *	SPR#1391: Led_MFC_DragAndDropWindow<>::HandleDragSelect now checks a timeout on DROP_EFFECT_NONE
 *	case to see if it should restore old selection or ProcessSimpleClick (to set an empty selection)
 *	
 *	Revision 2.146  2003/03/25 14:39:20  lewis
 *	SPR#1374 - Led_MFC_DragAndDropWindow<BASECLASS>::HandleDragSelect () fixed to reset selection
 *	to orig value rather than empty selection on aborted drag (click on selected region -
 *	like an embedding or table)
 *	
 *	Revision 2.145  2003/03/10 19:09:02  lewis
 *	SPR#1286 - OptionallyAddExtraSpaceForSmartCutAndPasteModeAdds
 *	OptionallyAddExtraSpaceForSmartCutAndPasteModeAdds to better handle case of
 *	when to insert smart spaces.
 *	
 *	Revision 2.144  2003/02/13 22:58:09  lewis
 *	SPR#1297- qTemplatedMemberFunctionsFailWithMFCMessageMaps to get MFC code compiling on MSVC6 again
 *	
 *	Revision 2.143  2003/02/03 20:56:04  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.142  2003/02/03 15:13:00  lewis
 *	SPR#1291 - Added qLedCheckCompilerFlagsConsistency to check for mismatches on compile flags
 *	
 *	Revision 2.141  2003/01/31 23:08:22  lewis
 *	SPR#1278- added InteractiveUpdadeMode and InteractiveModeUpdater so things like AboutToUpdate()
 *	can tell if its a USER change or a program change to text its about to make (and so whether
 *	or not to allow/disallow)
 *	
 *	Revision 2.140  2003/01/31 00:53:35  lewis
 *	SPR#1274- get rid of fKeyboardInputCodePage/WM_INPUTLANGCHANGE/OnInputLangChange_Msg and just
 *	call Win32PrimaryLangIDToCodePage (LOWORD (::GetKeyboardLayout (NULL))) for each character a user types
 *	
 *	Revision 2.139  2003/01/31 00:20:35  lewis
 *	SPR#1275- essentially renamed Led_MFC_Helper<>::SubclassWindow -> ReplaceWindow
 *	
 *	Revision 2.138  2003/01/30 17:58:44  lewis
 *	SPR#1269- Led_MFC_OptionalWin32SDKMessageMimicHelper<> now vectors to new
 *	Led_MFC_OptionalWin32SDKMessageMimicHelper<> and a few other small message map cleanups
 *	
 *	Revision 2.137  2003/01/30 15:01:03  lewis
 *	SPR#1267- get rid of WM_UNICHAR define here - taken care of in Led_Win32
 *	
 *	Revision 2.136  2003/01/29 19:15:06  lewis
 *	SPR#1265- use the keyword typename instead of class in template declarations
 *	
 *	Revision 2.135  2003/01/20 15:25:41  lewis
 *	minor tweeks - getting rid of warnings etc compiling WIN32/MFC on MWERKS MacOS cross-compiler
 *	
 *	Revision 2.134  2003/01/15 15:18:04  lewis
 *	SPR#1239 - qProvideLedStubsForOLEACCDLL to handle OLEACC.DLL dependency in MFC 7.0 (VC.NET)
 *	
 *	Revision 2.133  2003/01/15 15:07:19  lewis
 *	SPR#1238- preliminary WM_UNICHAR support (still investigating why it doesn't work)
 *	
 *	Revision 2.132  2003/01/11 19:28:34  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.131  2002/10/30 15:23:24  lewis
 *	SPR#1153- Added Get/SetPrintMargins () API, and change CalculatePrintRect API to take that into account
 *	
 *	Revision 2.130  2002/10/30 01:04:12  lewis
 *	SPR#1153- work on adding marins to printed pages (still not complete)
 *	
 *	Revision 2.129  2002/09/19 14:13:59  lewis
 *	Lose qSupportLed23CompatAPI (and related backward compat API tags like qOldStyleMarkerBackCompatHelperCode,
 *	qBackwardCompatUndoHelpers, etc).
 *	
 *	Revision 2.128  2002/09/19 12:54:51  lewis
 *	SPR#1099- fix args to OnCreate_Msg() to be JSUT the LPCREATESTRUCT. Override OnCreate() in MFCHELPER wrapper.
 *	Use OnCreate () msg hook to check incoming default window style - and to set the default value for
 *	SetScrollBarType () accordingly.
 *	
 *	Revision 2.127  2002/05/06 21:33:28  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.126  2002/04/02 02:42:41  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.125  2002/04/02 02:42:08  lewis
 *	SPR#1079- more minor tweeks to this bugfix. Removed added extra arg to CalculatePrintingRect() so 
 *	its more backward compatable
 *	
 *	Revision 2.124  2002/04/02 01:04:12  lewis
 *	Misc - related to SPR#1079.
 *	
 *	Revision 2.123  2002/04/02 00:56:19  lewis
 *	SPR#1079 - fix for printing code (page at end cutoff)
 *	
 *	Revision 2.122  2001/11/27 00:29:40  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.121  2001/10/17 20:42:50  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.120  2001/10/17 01:41:41  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.119  2001/10/16 15:34:14  lewis
 *	SPR#1062- as a side-effect of working on this bug - Fix Led_MFC-helper class to work if no D&D (no OleDoc)
 *	
 *	Revision 2.118  2001/10/13 19:52:36  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.117  2001/10/13 19:50:02  lewis
 *	SPR#1059- moved some Led_DrawText<> / Led_GetTextExtent<> code to shared code
 *	in LedHandySimple - GetTextExtent<>/DrawTextBox<>, and docs cleanups
 *	
 *	Revision 2.116  2001/09/05 16:23:30  lewis
 *	SPR#1016- cleanup semantics of ReaderFlavorPackage::GetFlavorSize/ReaderFlavorPackage::ReadFlavorData
 *	
 *	Revision 2.115  2001/09/05 00:07:13  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.114  2001/09/05 00:04:34  lewis
 *	SPR#1013- A few small changes to TextInteractor::UndoableContextHelper to enable its use in Led_MFC_
 *	and Led_PP D&D code. And added UndoableContextHelper::SetBeforeUndoSelection () to fix a small
 *	trouble with that code.
 *	
 *	Revision 2.113  2001/08/29 23:00:15  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.112  2001/08/28 18:43:25  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.111  2001/08/01 23:48:25  lewis
 *	SPR#0965- TextInteractor::BadUserInput class, and throw that from TextInteractor::OnBadUserInput ()
 *	instead of directly calling Led_BeepNotify (). AND - small changes to support SPR#0964 (READONLY/DISABLED)
 *	
 *	Revision 2.110  2001/07/31 15:28:11  lewis
 *	silence warnings from MSVC70B2 and new MFC7)
 *	
 *	Revision 2.109  2001/05/01 16:07:44  lewis
 *	fixed bad names - Get/SetCurClickClickCount to just one CLICK
 *	
 *	Revision 2.108  2001/04/30 21:10:40  lewis
 *	SPR#0891- Support OnInputLangChange_Msg- and keep track of input code page. Then use that in
 *	OnChar() messages. Only needed for non-ISOLATIN charsets which DONT use IME_CHAR messages (and even
 *	then - only for INTERNAL UNICODE - not REAL UNICODE case
 *	
 *	Revision 2.107  2001/04/27 15:23:02  lewis
 *	doc comments
 *	
 *	Revision 2.106  2001/04/26 16:39:27  lewis
 *	delete ifdefed out code fragments (cleanups)
 *	
 *	Revision 2.105  2001/04/17 22:48:20  lewis
 *	SPR#0871- Moved a bunch of code from Led_MacOS and Led_Win32 to TextInteractor. Code was all
 *	relating to TextInteractor::GetCurClickClickCount () etc. Also - improved logic for double
 *	click detection (now shared across platforms).
 *	
 *	Revision 2.104  2001/04/14 17:30:05  lewis
 *	SPR#0869 Improve WYSIWYG nature of printing code
 *	
 *	Revision 2.103  2000/11/18 00:18:31  lewis
 *	SPR#0848- Don't allow D&D to be initiated from a READONLY view in Led_MFC_DragAndDropWindow<BASECLASS>::HandleDragSelect ()
 *	
 *	Revision 2.102  2000/10/19 19:40:05  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.101  2000/10/18 20:39:54  lewis
 *	Added Led_StdDialogHelper_FindDialog support. Works on Windows and prelim for Mac
 *	
 *	Revision 2.100  2000/10/04 13:17:53  lewis
 *	fixup UNDO/REDO support, and add default arg to TextInteractorCommonCommandHelper_MFC template
 *	
 *	Revision 2.99  2000/10/03 13:36:34  lewis
 *	SPR#0839- reorganize the WordProcessorCommonCommandHelper/TextInteractorCommonCommandHelper<> code.
 *	Now MFC wrapper support MUCH simpler, and all this stuff much easier to use from other platforms.
 *	OnPerformCommand/OnUpdateCommand simple virtual overrides, and _MSG simple single msg hooks for MFC.
 *	
 *	Revision 2.98  2000/09/30 19:35:00  lewis
 *	Added TextInteractorCommandHelper_DefaultCmdInfo/TextInteractorCommonCommandHelper<BASECLASS,CMD_INFO,CMD_ENABLER>.
 *	Added TextInteractor::OnSelectAllCommand. Moved Led_MFC_TmpCmdUpdater to Led_MFC.h
 *	
 *	Revision 2.97  2000/08/28 20:11:01  lewis
 *	SPR#0828- use new TextInteractor::UndoableContextHelper to simplify UNDO code. SPR#0827- lose SetXXXCommandName ()
 *	specific methods, and added Led_MFC_DragAndDropWindow<BASECLASS>::CommandNames (etc) instead. Much more
 *	
 *	Revision 2.96  2000/04/25 19:01:35  lewis
 *	Made GetFlavorSize/ReadFlavorData() CONST
 *	
 *	Revision 2.95  2000/04/24 17:29:21  lewis
 *	SPR#0743- added new Led_MFC_WordProcessor module - and broke some stuff from Led_MFC out and into it
 *	
 *	Revision 2.94  2000/04/15 14:32:34  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.93  2000/04/14 22:40:20  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.92  2000/04/14 19:13:19  lewis
 *	SPR##0739- react breakout into FlavorPackage code of that stuff from TextInteractor
 *	
 *	Revision 2.91  2000/04/11 14:25:25  lewis
 *	SPR#0736- WM_MOUSEWHEEL support added
 *	
 *	Revision 2.90  2000/04/04 20:49:56  lewis
 *	SPR# 0732- added  Led_Set_OutOfMemoryException_Handler/Led_Set_BadFormatDataException_Handler/Led_Set_ThrowOSErrException_Handler.
 *	And qGenerateStandardPowerPlantExceptions and qGenerateStandardMFCExceptions defines. The idea is to make it
 *	easier to override these exception throw formats, and get better default behavior for cases other
 *	than MFC and PowerPlant.
 *	
 *	Revision 2.89  2000/04/03 18:29:03  lewis
 *	Add a few more methods to Led_MFC_ExceptionHandlerHelper<> - OnXXXClick() methods
 *	
 *	Revision 2.88  2000/03/28 03:41:03  lewis
 *	more work on qOldStyleMarkerBackCompatHelperCode/SPR#0718. Now StyleMarker class takes extra
 *	RunElement argument. Now this new code is called by default StyledTextImager::DrawText() etc overrides.
 *	And now HiddenText code properly merges at least its own drawing and StandardStyledTextImager::StyleMarker.
 *	All this respecting qOldStyleMarkerBackCompatHelperCode - so old code (not new hidden text code) should still
 *	compile and work roughly as before with this turned on.
 *	
 *	Revision 2.87  2000/01/11 22:14:17  lewis
 *	SPR#0691- fix NT40J IME bug(qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug)
 *	
 *	Revision 2.86  1999/12/28 14:59:48  lewis
 *	SPR#0688- added Led_DrawText<TRIVIALWORDWRAPPEDTEXTIMAGER,SIMPLETEXTIMAGER,TEXTSTORE> ETC
 *	
 *	Revision 2.85  1999/12/27 17:31:34  lewis
 *	SPR#0669 - work on this, but not done. Just added most of the support so this command is
 *	autohandled by WordProcessorCommonCommandHelper<> template. And adjusted the characters
 *	output (at least for UNICODE).
 *	
 *	Revision 2.84  1999/12/27 16:28:35  lewis
 *	renamed WordProcessorSelectionCommandHelper<> -> WordProcessorCommonCommandHelper<>
 *	
 *	Revision 2.83  1999/12/18 03:56:45  lewis
 *	0662- break up StyledTextIO module into many _parts
 *	
 *	Revision 2.82  1999/12/14 21:21:15  lewis
 *	qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug workraround for SPR#0653-
 *	typing Japanese characters in IME for UNICODE Led
 *	
 *	Revision 2.81  1999/12/12 16:16:19  lewis
 *	spr#0651 - added in OnChooseFontCommand () to WordProcessorSelectionCommandHelper/
 *	WordProcessorSelectionCommandHelper_MFC templates (not used on Mac/PP)
 *	
 *	Revision 2.80  1999/12/09 17:28:16  lewis
 *	use Led_SDK_String in one more place to get LedIt! MFC building -D_UNICODE
 *	
 *	Revision 2.79  1999/12/09 03:21:02  lewis
 *	SPR#0645 - preliminary qSDK_UNICODE (_UNICODE) support (mostly using new Led_SDK_String/Led_SDK_Char)
 *	
 *	Revision 2.78  1999/12/08 17:41:02  lewis
 *	(pref message wrong).more work on WordProcessorSelectionCommandHelper_MFCX<BASECLASS,CMD_ENABLER,CMD_INFO>
 *	and related classes (inclduing reversing template args so can do default arg
 *	
 *	Revision 2.77  1999/12/08 17:39:01  lewis
 *	added  WordProcessorSelectionCommandHelper_PPX<BASECLASS,CMD_INFO,CMD_ENABLER = Led_PP_TmpCmdUpdater>
 *	and WordProcessorSelectionCommandHelper_PP<BASECLASS,CMD_INFO,CMD_ENABLER>
 *	
 *	Revision 2.76  1999/12/08 03:19:40  lewis
 *	new - prelim WordProcessorSelectionCommandHelper_MFCX helper with #if qXXX wrapper. Probably move to a new file
 *	
 *	Revision 2.75  1999/12/07 20:10:40  lewis
 *	Added Led_MFC_TmpCmdUpdater / WordProcessorSelectionCommandHelper_MFC<BASECLASS,CMD_ENABLER> as
 *	helpers for newly added WordProcessorSelectionCommandHelper<BASECLASS,CMD_INFO,CMD_ENABLER>. Part of spr#0606.
 *	
 *	Revision 2.74  1999/12/03 23:18:11  lewis
 *	DOCCOMMENTS
 *	
 *	Revision 2.73  1999/11/29 18:59:24  lewis
 *	move most of the remaining code from Led_MFC_Helper<> to Led_Win32_Helper<>. This is now roughly
 *	complete. Both seem to be working fine.
 *	
 *	Revision 2.72  1999/11/28 17:12:56  lewis
 *	Added dependency of Led_MFC_Helper on Led_Win32_Helper. Started moving some appropriate code from
 *	this template to the Led_Win32_Helper template. Still LOTS more todo. Probably most of our code
 *	will eventually go there. But as of now - both seem to be working (Led_MFC_Helper as well
 *	as ever and Led_Win32_Helper displaying HelloWorld
 *	
 *	Revision 2.71  1999/11/28 17:11:46  lewis
 *	Added dependency of Led_MFC_Helper on Led_Win32_Helper. Started moving some appropriate code
 *	
 *	Revision 2.70  1999/11/17 19:01:45  lewis
 *	add calls to EN_CHANGE for Cut/Paste/etc commands
 *	
 *	Revision 2.69  1999/11/15 21:25:28  lewis
 *	added Led_MFC_TabletFromCDC/Led_MFC_CDCFromTablet and make various related changes cuz Led no longer
 *	includes any MFC code - anyplace except in Led_MFC. Means a few fewer calls to ASSERT_VALID, etc here.
 *	
 *	Revision 2.68  1999/11/13 22:33:00  lewis
 *	lose qSupportLed22CompatAPI support
 *	
 *	Revision 2.67  1999/11/13 16:32:17  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.66  1999/08/28 14:14:37  lewis
 *	fix docs
 *	
 *	Revision 2.65  1999/07/21 14:36:34  lewis
 *	remove register of droptarget window if qSupportLed22CompatAPI defined
 *	
 *	Revision 2.64  1999/07/16 22:25:03  lewis
 *	use typename declaration to silence warnings on MWERKS CW5 compiler, and other such
 *	innocuous changes
 *	
 *	Revision 2.63  1999/07/12 13:50:05  lewis
 *	fix spr#0605- pass unsigned char instead of char so typing spanish (and other high-bit set,
 *	but single byte charset) text doesn't get garbled when converted to unicode
 *	
 *	Revision 2.62  1999/06/26 16:10:47  lewis
 *	don't call TextInteractor::Refresh() cuz thats a non-virtual call - in mixin disambiguators - cast
 *	to TextInteractor* and then make virtual call. Also - added VERY PRELIMINARY first cut
 *	at Led_MFC_COleControlHelper<>
 *	
 *	Revision 2.61  1999/06/25 16:06:57  lewis
 *	cosmetic cleanup
 *	
 *	Revision 2.60  1999/06/25 00:46:13  lewis
 *	added Led_MFC_ExceptionHandlerHelper<BASECLASS> template as part of fix for spr#0595
 *	
 *	Revision 2.59  1999/06/15 04:02:17  lewis
 *	spr#0585- break D&D support out of Led_MFC_CViewHelper, and into separate Led_MFC_DragAndDropWindow<>
 *	
 *	Revision 2.58  1999/06/14 22:32:26  lewis
 *	spr#0581- handle WS_TABSTOP/tab characters better for Led in dialogs, using new HandleTabCharacterTyped ()
 *	routine. Also - fixed typo where I was checking GetStyle() && instead of &
 *	
 *	Revision 2.57  1999/06/14 21:47:13  lewis
 *	spr#0582 - OnChar/OnKeyDown should NOT be virtual
 *	
 *	Revision 2.56  1999/06/14 21:44:28  lewis
 *	spr#0580 - when enable flag changes - refresh window
 *	
 *	Revision 2.55  1999/06/14 21:11:34  lewis
 *	spr#0579- send EN_CHANGE message on DELETE key
 *	
 *	Revision 2.54  1999/05/03 22:04:53  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.53  1999/05/03 21:41:20  lewis
 *	Misc minor COSMETIC cleanups - mostly deleting ifdefed out code
 *	
 *	Revision 2.52  1999/04/01 16:31:57  lewis
 *	DOCCOMMENTS
 *	
 *	Revision 2.51  1999/04/01 16:01:31  lewis
 *	fix spr#0569- tripple click with typing between second and third click caused assert error
 *	
 *	Revision 2.50  1999/03/31 19:50:58  lewis
 *	added assertions about fDragAnchor due to problem Remo reported
 *	
 *	Revision 2.49  1999/03/22 20:10:50  lewis
 *	fix case where I had hardwired CView and should have used BASECLASS template arg
 *	
 *	Revision 2.48  1999/03/12 16:01:11  lewis
 *	add spaces to the message map macros around '>' allowed me to undo hacks/typedefs to Declare_Led_MFC_XXX calls
 *	
 *	Revision 2.47  1999/03/10 21:33:36  lewis
 *	move some common drawing code into new routine WindowDrawHelper for sharing, and so the code can
 *	be shared/called out directly from ActiveLedIt! and elsewhere
 *	
 *	Revision 2.46  1999/03/10 21:03:30  lewis
 *	many changes thanx to Sterl to support the ES_READONLY OnMsgReadOnly () stuff - draw gray
 *	background by default, fix problem passing exStyle to CreateEx () in subclassWindow, etc
 *	
 *	Revision 2.45  1999/03/10 16:09:54  lewis
 *	Use MFC_BASE_CLASS in several inherited calls, instead of CWnd. In the SubClassWindow() method,
 *	save the EX style and call CreateEx() (so we get borders working right in edit controls
 *	
 *	Revision 2.44  1999/03/10 15:38:39  lewis
 *	several fixes to the new template support reported by SWIGHT- pertaining to using CWnd-based
 *	Led in a dialog
 *	
 *	Revision 2.43  1999/03/08 22:44:38  lewis
 *	Break out RTFInfo stuff which was really just a repository for type declarations, and
 *	store that into new type RTFIO
 *	
 *	Revision 2.42  1999/03/08 18:22:41  lewis
 *	cleanupdocs
 *	
 *	Revision 2.41  1999/03/07 14:38:50  lewis
 *	massive re-organization of Led_MFC class. Now its a trivial typedef. And all the old code
 *	has been broken into a series of separate, but individually selectable/combinable templates.
 *	Not only allowing you to only compile in the features you want, but also to select a different
 *	baseclass (CWnd instead of CView, for example. SPR#0557
 *	
 *	Revision 2.40  1999/03/02 22:39:25  lewis
 *	fix spr#0554- had h/v reversed reading in OLE objects and scaling, and had NO code to write
 *	out sizes. Still not working great (see spr#0555), but much better (added Led_MFC_ControlItem::GetSize()
 *	to help as well)
 *	
 *	Revision 2.39  1999/03/01 15:05:47  lewis
 *	Added Led_MFC::Get/SetH/VScrollInfo () methods, to replace old qMakeWindowsScrollbarAppearOnlyIfNeeded
 *	
 *	Revision 2.38  1999/02/06 14:27:10  lewis
 *	DOCS
 *	
 *	Revision 2.37  1999/01/26 15:19:57  lewis
 *	revised doc string for  qMakeWindowsScrollbarAppearOnlyIfNeeded
 *	
 *	Revision 2.36  1998/10/30 14:12:33  lewis
 *	Use vector<> instead of Led_Array, and new msvc60 pramga warning stuff.
 *	
 *	Revision 2.35  1998/07/24  00:57:20  lewis
 *	Add Led_MFC::TemporarilyUseTablet
 *	GetHScrollInfo () etc virtual.
 *	GetCurClickClickCount () accessor.
 *	added (and use) virtual CalculatePrintingRect().
 *	(for margins in printing).
 *	IsADragSelect () virtual
 *
 *	Revision 2.34  1998/04/25  01:21:12  lewis
 *	Get/SetInputCodePage () code for UNICODE support.
 *	Docs.
 *
 *	Revision 2.33  1998/03/04  20:19:30  lewis
 *	*** empty log message ***
 *
 *	Revision 2.32  1997/12/24  04:36:55  lewis
 *	*** empty log message ***
 *
 *	Revision 2.31  1997/12/24  03:23:54  lewis
 *	Support RTFInfo::RTFOLEEmbedding API. in Led_MFC_ControlItem (so we can create them from RTF text).
 *
 *	Revision 2.30  1997/09/29  14:32:12  lewis
 *	Lose qSupportLed21CompatAPI
 *
 *	Revision 2.29  1997/07/27  15:56:47  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.28  1997/07/23  23:03:11  lewis
 *	Lots of doc changes.
 *
 *	Revision 2.27  1997/07/15  05:25:15  lewis
 *	AutoDoc content.
 *
 *	Revision 2.26  1997/07/14  15:54:19  lewis
 *	*** empty log message ***
 *
 *	Revision 2.25  1997/07/12  19:59:16  lewis
 *	Led_MFC_ renamed Led_MFC.
 *	Use string class for command names.
 *	AutoDoc.
 *
 *	Revision 2.24  1997/06/18  20:04:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.23  1997/06/18  02:34:27  lewis
 *	Misc cleanups, lose fOldLayoutWidth, IsADragSelect() const method, HandleSharedMouseMoveUpdateSelCode
 *	now obsoleted.
 *	Added new qLedAssertsDefaultToMFCAsserts define (default off cuz I don't like).
 *
 *	Revision 2.22  1997/03/22  13:49:46  lewis
 *	Use WhileTrackingConstrainSelection () hook instead of old peicemail dbl-click tracking code.
 *	Keep fClickCount instead of bool fDidDlbClick. Lose sbar scale (now using 32bit scrollsupport).
 *
 *	Revision 2.21  1997/01/10  03:06:20  lewis
 *	New templated mixin helper Led_MFC_X<> instead of lots of manually specified mixins.
 *	Now use templated Led_MFC_X<> to combine iteractor mixins with
 *	a class library wrapper. Makes much easier to support Led_MFCtogether with all the various
 *	cominations of interactors/imagers you now can use.
 *	Old pre-mixed classes supported via qSupportLed21CompatAPI.
 *
 *	Revision 2.20  1996/12/13  17:54:11  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.19  1996/12/05  19:08:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.18  1996/10/31  00:08:32  lewis
 *	Added qScrollTextDuringThumbTracking define.
 *
 *	Revision 2.17  1996/09/30  14:13:54  lewis
 *	Lose AdjustBounds() and instead redid the UpdateScrollBars() code, etc...
 *
 *	Revision 2.16  1996/09/01  15:30:29  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.15  1996/06/01  02:03:07  lewis
 *	Moved OLEControl stuff here from LedIt application code (Led_MFC_ControlItem).
 *	Lose OnDropFiles stuf.
 *	Work on timer/drag/autoscroll stuff to support autoscroll timer for dragging.
 *
 *	Revision 2.14  1996/05/23  19:20:45  lewis
 *	Added OnDropFiles override (maybe a mistake - maybe not needed and we
 *	should delete again?).
 *
 *	Revision 2.13  1996/04/18  15:03:19  lewis
 *	Moved drag and drop support here from LedIt! code.
 *	Wrote a bunch of new stuff to integrate fully with the new Led
 *	library internalize/externalize code.
 *	more...
 *
 *	Revision 2.12  1996/03/16  18:37:01  lewis
 *	Override OnCreate().
 *
 *	Revision 2.11  1996/02/26  18:36:58  lewis
 *	Moved most MFC dependencies here from around Led - especially the
 *	LedConfig feature defines. Still a few lingering MFC'ims around. Esp in
 *	TextImager.
 *	Renamed TextInteracter->TextInteractor.
 *
 *	Revision 2.10  1996/02/05  04:09:34  lewis
 *	Get rif of fWeOnwTextStore crap - taken care of in class Led now.
 *
 *	Revision 2.9  1996/01/22  05:08:34  lewis
 *	Added overrides for OnPrint, etc... got printing working!
 *
 *	Revision 2.8  1996/01/11  08:14:10  lewis
 *	Added fWordSelStart;/fWordSelEnd; to support new word selection code
 *	(xtra space at end of word and oterh bug fixes to word select).
 *
 *	Revision 2.7  1996/01/03  23:50:49  lewis
 *	Make OnTimer() stuff protected - not private - so subclasses can
 *	do OnTimer () calls and call inherited.
 *
 *	Revision 2.6  1995/12/13  05:53:18  lewis
 *	Lose qUseLocalBoolFlagToKeepTrackOfShowHideCaretForNTJCaretBu
 *
 *	Revision 2.5  1995/12/06  01:30:25  lewis
 *	Instead of handling qMFCRequiresCWndLeftmostBaseClass in comments, really
 *	do ifdef for two different cases.
 *
 *	Revision 2.4  1995/11/25  00:17:13  lewis
 *	Override OnPasteCommand () - this code still needs work...
 *
 *	Revision 2.3  1995/10/19  22:04:28  lewis
 *	Support new StandardStyledWordWrappedLed_MFC.
 *
 *	Revision 2.2  1995/10/09  22:14:19  lewis
 *	No more need for Replace override, and no more include of
 *	Chunked/Simple TextStores
 *
 *	Revision 2.1  1995/09/06  20:52:38  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.18  1995/05/29  23:45:07  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.17  1995/05/29  23:39:59  lewis
 *	Added utility CheckIfDraggingBeepAndReturn () to fix SPR 0299 - crash
 *	on key while drag-selecting.
 *
 *	Revision 1.16  1995/05/25  09:45:14  lewis
 *	fAllocatedTablet now CDC instead of CWindowDC*. Direct object - SPR 286
 *
 *	Revision 1.15  1995/05/24  07:15:01  lewis
 *	Add qUseLocalBoolFlagToKeepTrackOfShowHideCaretForNTJCaretBu
 *	 workaround
 *
 *	Revision 1.14  1995/05/21  17:04:29  lewis
 *	Moved qLotsOfSubclassesFromMixinPointYeildTemporarilyBadVTable back
 *	here since must be done in concrete subclass of TextStore.
 *
 *	Revision 1.13  1995/05/20  04:42:39  lewis
 *	Massive changes.
 *	Mostly now using WordWrappedTextInteractor now. These should all me
 *	much simpler - SPR 0261, 0262.
 *
 *	Revision 1.12  1995/05/12  21:05:14  lewis
 *	Code cleanups on the MFC Serialize/CArchive support.
 *	Added overrides for OnNcCalcSize/OnGetDlgCode/OnNcPaint for
 *	WS_BORDER fixes. Maybe didn't need the NC overrides. OnGetDlgCode ()
 *	was pre-emptive fix to make sure I returned same answer as other
 *	TEs in windows.
 *
 *	Revision 1.11  1995/05/09  23:26:31  lewis
 *	Moved fMultiByteInputCharBuf to TextInteractor_ to share code - SPR#0242.
 *
 *	Revision 1.10  1995/05/08  03:47:21  lewis
 *	Fixed PC-only typos.
 *
 *	Revision 1.9  1995/05/08  03:02:34  lewis
 *	ScrollSoShowing, ScrollByIfRoom no longer return bool. Work more
 *	cleanly - just caching old top row and comparing to see if
 *	they need to redraw.
 *
 *	Revision 1.8  1995/05/06  19:23:07  lewis
 *	Use Led_tChar instead of char* for UNICODE support - SPR 0232.
 *
 *	Revision 1.7  1995/04/16  19:21:28  lewis
 *	Got rid of SimpleTextImager (SPR#0205).
 *
 *	Revision 1.6  1995/03/29  21:02:33  lewis
 *	Code cleanups.
 *	Renamed UpdateCaretState () to InvalidateCaretState() and added
 *	new protected - nonvirtual UpdateCaretState_ ().
 *
 *	Revision 1.5  1995/03/24  01:26:38  lewis
 *	fDidShowCaretCall field no longer needed.
 *
 *	Revision 1.4  1995/03/23  03:46:01  lewis
 *	Fixed SelectWholeLineAfter args - accidentally missed when TextInteacter
 *	base class changed arg list and we stopped really overriding!
 *	And added OnMsgCanUndo/OnMsgUndo/OnMsgEmptyUndoBuffer message handlers.
 *
 *	Revision 1.3  1995/03/17  02:58:00  lewis
 *	Added fScrollBarScale to deal with large documents and fact that
 *	WinSDK sbars only go up to 32K.
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

#include	<afxwin.h>
#include	<afxole.h>
#include	<oleidl.h>


#include	"IdleManager.h"
#include	"Led_Win32.h"
#include	"TextInteractor.h"




#if		qLedUsesNamespaces
namespace	Led {
#endif



/*
 **************** Windows Specific configuration variables **************
 */






/*
@CONFIGVAR:		qLedAssertsDefaultToMFCAsserts
@DESCRIPTION:	<p>By default when using MFC, make the Led asserts fall-thru into the default
	MFC assertion code. Some customers have indicated they prefer this (spr#0424).</p>
		<p>Originally I made this default to TRUE, as the SPR requested. But I personally
	find the MFC Assert stuff a major pain in the donkey. Whenever you get an assert
	due to a display bug, it brings up a dialog, and dimissing that alert triggers
	(due to extra redisplay) extra re-enters into the same code which had the assert
	doing displays.</p>
		<p>Since it was requested, I'll continue to make it easy for others to turn this on.
	But I leave it off by default.</p>
 */
#ifndef	qLedAssertsDefaultToMFCAsserts
	#define	qLedAssertsDefaultToMFCAsserts					0
#endif





/*
@CONFIGVAR:		qSupportDrawTextGetTextExtent
@DESCRIPTION:	
		<p>Turn OFF by default (because requires including SimpleTextStore and SimpleTextImager and WordWrappedTextImager
	in your projects - which you may not want to do).
		See @'Led_DrawText' and @'Led_GetTextExtent'.</p>
 */
#ifndef	qSupportDrawTextGetTextExtent
#define	qSupportDrawTextGetTextExtent						0
#endif





/*
@CONFIGVAR:		qGenerateStandardMFCExceptions
@DESCRIPTION:	
		<p>MFC apps generally throw AfxMemoryException () etc for internal exceptions. And there are standard MFC catchers for
	these types. To make Led utilize the @'Led_Set_OutOfMemoryException_Handler' etc mechanism to use the MFC exception types - 
	define this to TRUE. To use more standard Standard C++ or your own types - you may wish to shut this off.</p>
		<p>Defaults to ON.</p>
 */
#ifndef	qGenerateStandardMFCExceptions
#define	qGenerateStandardMFCExceptions						1
#endif






/*
@CONFIGVAR:		qProvideLedStubsForOLEACCDLL
@DESCRIPTION:	
		<p>Windows 98 has a problem with OLEACC.DLL. Though it is <em>supposed</em> to be included with Win98-
	people somehow frequently come up with systems that don't have it, or have it corrupted.
		(for example, see MSFT knowledge base article
			http://support.microsoft.com/default.aspx?scid=kb%3Ben-us%3B810684
		)</p>
		<p>As of version 7 of MFC (with Visual Studio.Net), Microsoft added virtual functions that refer to
	this DLL. The virtual function CWnd::EnsureStdObj calls CreateStdAccessibleObject (from OLEACC.DLL),
	the virtual function CWnd::CreateAccessibleProxy calls LresultFromObject (from OLEACC.DLL),
	and CWnd::GetAccessibleChild () which calls AccessibleObjectFromWindow (from OLEACC.DLL). This creates
	a dependency of OLEACC.DLL which is unfortunate for applications that link against MFC and really don't need
	those functions.
		</p>
		<p>We provide a workaround for this problem by providing our own - friendlier stub routines for these
	two OLEACC.DLL functions.</p>
		<p>Defaults to ON if _MFC_VER>=0x0700.</p>
 */
#ifndef	qProvideLedStubsForOLEACCDLL
#define	qProvideLedStubsForOLEACCDLL						(_MFC_VER>=0x0700)
#endif







/*
 **************** MFC bug workaround defines **************
 */


// Still broken In MSVC 6.0 (LGP 990304) - CFrameWnd::CreateView() still does cast of void* to CWnd* which breaks unless CWnd* is leftmost
// Still broken In MSVC 5.0 (LGP 970318) - now the bug is in CFrameWnd::CreateView() though...
// Still broken In MSVC 4.0 (LGP 951125)
	// (only not broken for old LedTestMFC app - breaks in new LedIt! app however. Maybe
	// should re-investigate???)
//	LGP 941129 (updated 950506)
//	The bug is with MFC 3.0/3.1, in CFrameWnd::OnCreateClient()
//	It did a cast of the result of CreateObject() to CWnd*, without
//	knowing the real type it started with, so that code looses if
//	CWnd is not the left-most base class.
#ifndef	qMFCRequiresCWndLeftmostBaseClass
#define	qMFCRequiresCWndLeftmostBaseClass						1
#endif






CPoint		AsCPoint (Led_Point p);
Led_Point	AsLedPoint (CPoint p);

CRect		AsCRect (Led_Rect r);
Led_Rect	AsLedRect (CRect r);

CSize		AsCSize (Led_Size s);






/*
@CLASS:			Led_MFC_TmpCDCFromTablet
@DESCRIPTION:	<p>Helper class to convert a @'Led_Tablet' to an MFC CDC. Just creates a temporary wrapper.</p>
	<p><strong>Caution</strong>:
	You must create a NAMED temporary.
	A named temporary has lifetime til the end of the enclosing scope.
	An unnamed temporary has lifetime only til the next sequence point (less or equal to the rest of the current statement - I believe).</p>
*/
class	Led_MFC_CDCFromTablet {
	public:
		Led_MFC_CDCFromTablet (Led_Tablet t);
		~Led_MFC_CDCFromTablet ();
		operator CDC* ();

	private:
		CDC	fCDC;
};




/*
@CLASS:			Led_MFC_TabletFromCDC
@DESCRIPTION:	<p>Helper class to convert an MFC CDC to a @'Led_Tablet'. Just creates a temporary wrapper.</p>
	<p><strong>Caution</strong>:
	You must create a NAMED temporary.
	A named temporary has lifetime til the end of the enclosing scope.
	An unnamed temporary has lifetime only til the next sequence point (less or equal to the rest of the current statement - I believe).</p>
*/
class	Led_MFC_TabletFromCDC {
	public:
		Led_MFC_TabletFromCDC (CDC* pDC);
		~Led_MFC_TabletFromCDC ();

	public:
		operator Led_Tablet ();
		Led_Tablet operator-> ();

	private:
		Led_Tablet_	fTablet;
};




#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (push)
	#pragma	warning (disable : 4250)
#endif









/*
@CLASS:			Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>
@BASES:			MFC_BASE_CLASS=@'CWnd', @'Led_Win32_Helper<BASE_INTERACTOR>', BASE_INTERACTOR=@'TextInteractor'
@DESCRIPTION:	<p>There are two obvious ways to hook into MFC. One is to create a subclass of CWnd. The other is
	to create a subclass of CView. Because CView inherits from CWnd, but not virtually, you are strongly encouraged
	to pick one or the other. Alas - neither is really a good choice for ALL applications. For a big, main view, associated
	with a document, you want to subclass from CView. For a little control (as in a dialog), you want to subclass from CWnd.</p>
		<p>This template makes it easier (soon easy, but for now, just easier) to subclass from either one. The default one, and
	the one Led mainly uses is CView. But soon I'll support (better) subclassing from CWnd.</p>
		<p>NB: You must invoke the macro DoDeclare_Led_MFC_Helper_MessageMap(MFC_BASE_CLASS, BASE_INTERACTOR) to
	generate the code for the message
	map for this template, unless its already been done so for your particular MFC_BASE_CLASS in Led_MFC.cpp.</p>
*/
template	<typename	MFC_BASE_CLASS = CWnd, typename BASE_INTERACTOR = TextInteractor>	class	Led_MFC_Helper :
	#if		qMFCRequiresCWndLeftmostBaseClass
		public MFC_BASE_CLASS, public Led_Win32_Helper<BASE_INTERACTOR>
	#else
		public Led_Win32_Helper<BASE_INTERACTOR>, public MFC_BASE_CLASS
	#endif
{
	private:
		typedef	void*	inherited;	// avoid using this keyword since this is mixin of two classes
	private:
		typedef	Led_Win32_Helper<BASE_INTERACTOR>	LED_WIN32_HELPER;
	public:
		typedef	MarkerOwner::UpdateInfo	UpdateInfo;

	protected:
		Led_MFC_Helper ();

	public:
		virtual ~Led_MFC_Helper ();

#if		qSupportLed30CompatAPI
	public:
		nonvirtual	BOOL	SubclassWindow (HWND hWnd);
#endif
	public:
		nonvirtual	BOOL	ReplaceWindow (HWND hWnd);


	// Led_Win32_Helper Hooks
	public:
		override	HWND	GetHWND () const;



	// Window message hooks NEEDED to get basic editor functionality
	protected:
		afx_msg	int		OnCreate (LPCREATESTRUCT lpCreateStruct);
		afx_msg	void	OnPaint ();
		afx_msg	BOOL	OnSetCursor (CWnd* pWnd, UINT nHitTest, UINT message);
		afx_msg	UINT	OnGetDlgCode ();
		afx_msg	void	OnChar (UINT nChar, UINT nRepCnt, UINT /*nFlags*/);
		afx_msg	LRESULT	OnUniChar (WPARAM wParam, LPARAM lParam);
		afx_msg	LRESULT	OnIMEChar (WPARAM wParam, LPARAM lParam);
		afx_msg	LRESULT	OnIME_COMPOSITION (WPARAM wParam, LPARAM lParam);
		afx_msg	LRESULT	OnIME_ENDCOMPOSITION (WPARAM wParam, LPARAM lParam);
		afx_msg	void	OnKeyDown (UINT nChar, UINT nRepCnt, UINT /*nFlags*/);
		afx_msg	void	OnMouseMove (UINT /*nFlags*/, CPoint oPoint);
		afx_msg	void	OnLButtonDown (UINT nFlags, CPoint oPoint);
		afx_msg	void	OnLButtonUp (UINT /*nFlags*/, CPoint oPoint);
		afx_msg	void	OnLButtonDblClk (UINT /*nFlags*/, CPoint oPoint);
		afx_msg	void	OnSetFocus (CWnd* pOldWnd);
		afx_msg	void	OnKillFocus (CWnd* pNewWnd);
		afx_msg	void	OnSize (UINT nType, int cx, int cy);
		afx_msg	BOOL	OnEraseBkgnd (CDC* pDC);
		afx_msg	void	OnVScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg	void	OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg	BOOL	OnMouseWheel (UINT fFlags, short zDelta, CPoint point);
		afx_msg	void	OnEnable (BOOL bEnable);
		afx_msg	void	OnTimer (UINT nEventID);


	// Disambigouate mixins
	public:
		override	Led_Rect	GetWindowRect () const;
		nonvirtual	void		GetWindowRect (LPRECT lpRect) const;
	public:
		nonvirtual	DWORD	GetStyle () const;

	// Command handing - map MFC commands to Led/TextInteractor commands
	public:
		afx_msg	void	OnUpdateCommand_MSG (CCmdUI* pCmdUI);
		afx_msg	void	OnPerformCommand_MSG (UINT commandNumber);

	protected:
		DECLARE_MESSAGE_MAP ()
};







/*
@CLASS:			Led_MFC_MimicMFCAPIHelper<BASECLASS>
@BASES:			BASECLASS = @'Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>'
@DESCRIPTION:	<p>Mimicry of the MFC CEdit methods - to facilitate using this class in place of any
	existing CEdit usage. These methods simply do send-message calls just as their
	CEdit counterparts. Then messages are then trapped in the OnMsgXXX handlers.</p>
		<p>NB: Use of this class almost ALWAYS requires also mixing in the template
	@'Led_MFC_OptionalWin32SDKMessageMimicHelper<BASECLASS>'. since that is what (by default) implements
	the OnMsgXXX helpers.</p>
		<p>NB: You must invoke the macro DoDeclare_Led_MFC_MimicMFCAPIHelper_MessageMap(BASECLASS) somewhere to
	generate the message map code for this template, unless thats already been done for your particular BASECLASS
	in Led_MFC.cpp.</p>
*/
template	<typename	BASECLASS = Led_MFC_Helper<> >	class	Led_MFC_MimicMFCAPIHelper :
	public BASECLASS
{
	protected:
		Led_MFC_MimicMFCAPIHelper ();

	public:
		nonvirtual	BOOL	Create (DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
		nonvirtual	BOOL	CanUndo () const;
		nonvirtual	int		GetLineCount_CEdit () const;		// HAD to rename cuz of conflict...
		nonvirtual	BOOL	GetModify () const;
		nonvirtual	void	SetModify (BOOL bModified = TRUE);
		nonvirtual	void	GetRect (LPRECT lpRect) const;
		nonvirtual	DWORD	GetSel () const;
		nonvirtual	void	GetSel (int& nStartChar, int& nEndChar) const;
		nonvirtual	int		GetLine (int nIndex, LPTSTR lpszBuffer) const;
		nonvirtual	int		GetLine (int nIndex, LPTSTR lpszBuffer, int nMaxLength) const;
		nonvirtual	void	EmptyUndoBuffer ();
		nonvirtual	BOOL	FmtLines (BOOL bAddEOL);
		nonvirtual	void	LimitText (int nChars = 0);
		nonvirtual	int		LineFromChar (int nIndex = -1) const;
		nonvirtual	int		LineIndex (int nLine = -1) const;
		nonvirtual	int		LineLength (int nLine = -1) const;
		nonvirtual	void	LineScroll (int nLines, int nChars = 0);
		nonvirtual	void	ReplaceSel (LPCTSTR lpszNewText);
		nonvirtual	void	SetPasswordChar (TCHAR ch);
		nonvirtual	void	SetRect (LPCRECT lpRect);
		nonvirtual	void	SetRectNP (LPCRECT lpRect);
		nonvirtual	void	SetSel (DWORD dwSelection, BOOL bNoScroll = FALSE);
		nonvirtual	void	SetSel (int nStartChar, int nEndChar, BOOL bNoScroll = FALSE);
		nonvirtual	BOOL	SetTabStops (int nTabStops, LPINT rgTabStops);
		nonvirtual	void	SetTabStops ();
		nonvirtual	BOOL	SetTabStops (const int& cxEachStop);    // takes an 'int'
		nonvirtual	BOOL	Undo ();
		nonvirtual	void	Clear ();
		nonvirtual	void	Copy ();
		nonvirtual	void	Cut ();
		nonvirtual	void	Paste ();
		nonvirtual	BOOL	SetReadOnly (BOOL bReadOnly = TRUE);
		nonvirtual	int		GetFirstVisibleLine () const;
		nonvirtual	TCHAR	GetPasswordChar () const;
	protected:
		DECLARE_MESSAGE_MAP ()
};




#if		qTemplatedMemberFunctionsFailWithMFCMessageMaps
	#define	OnMFCSDKMessageDispatcherBWA_DECLARE(MESSAGE_NUMBER)\
		afx_msg	LRESULT	OnMFCSDKMessageDispatcher_##MESSAGE_NUMBER (WPARAM wParam, LPARAM lParam)\
			{\
				LRESULT	result	=	0;\
				Led_Verify (HandleMessage (MESSAGE_NUMBER, wParam, lParam, &result));\
				return result;\
			}
#endif



/*
@CLASS:			Led_MFC_OptionalWin32SDKMessageMimicHelper<BASECLASS>
@BASES:			BASECLASS = @'Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>'
@DESCRIPTION:	<p>Mimicry of the starndard Win32 messages sent to an edit control. We cannot
	mimic ALL the messages. Some just don't make sense (like GETHANDLE). But for those that do, we
	do our best.</p>
		<p>NB: You must declare DoDeclare_Led_MFC_OptionalWin32SDKMessageMimicHelper_MessageMap (BASECLASS) to get the
	message map for this class generated, unless this has already been done in Led_MFC.cpp (for your particular BASECLASS).</p>
*/
template	<typename	BASECLASS = Led_MFC_Helper<> >	class	Led_MFC_OptionalWin32SDKMessageMimicHelper :
	public Led_Win32_Win32SDKMessageMimicHelper <BASECLASS>
{
	private:
		typedef	Led_Win32_Win32SDKMessageMimicHelper <BASECLASS>	inherited;

	protected:
		Led_MFC_OptionalWin32SDKMessageMimicHelper ();

	public:
		template	<int	MESSAGE_NUMBER>
			afx_msg	LRESULT	OnMFCSDKMessageDispatcher (WPARAM wParam, LPARAM lParam)
				{
					LRESULT	result	=	0;
					Led_Verify (HandleMessage (MESSAGE_NUMBER, wParam, lParam, &result));
					return result;
				}

#if		qTemplatedMemberFunctionsFailWithMFCMessageMaps
	public:
		OnMFCSDKMessageDispatcherBWA_DECLARE(WM_SETTEXT)
		OnMFCSDKMessageDispatcherBWA_DECLARE(WM_GETTEXT)
		OnMFCSDKMessageDispatcherBWA_DECLARE(WM_GETTEXTLENGTH)
		OnMFCSDKMessageDispatcherBWA_DECLARE(EM_GETSEL)
		OnMFCSDKMessageDispatcherBWA_DECLARE(EM_SETREADONLY)
		OnMFCSDKMessageDispatcherBWA_DECLARE(EM_GETFIRSTVISIBLELINE)
		OnMFCSDKMessageDispatcherBWA_DECLARE(EM_LINEINDEX)
		OnMFCSDKMessageDispatcherBWA_DECLARE(EM_GETLINECOUNT)
		OnMFCSDKMessageDispatcherBWA_DECLARE(EM_CANUNDO)
		OnMFCSDKMessageDispatcherBWA_DECLARE(EM_UNDO)
		OnMFCSDKMessageDispatcherBWA_DECLARE(EM_EMPTYUNDOBUFFER)
		OnMFCSDKMessageDispatcherBWA_DECLARE(WM_CLEAR)
		OnMFCSDKMessageDispatcherBWA_DECLARE(WM_CUT)
		OnMFCSDKMessageDispatcherBWA_DECLARE(WM_COPY)
		OnMFCSDKMessageDispatcherBWA_DECLARE(WM_PASTE)
		OnMFCSDKMessageDispatcherBWA_DECLARE(EM_LINEFROMCHAR)
		OnMFCSDKMessageDispatcherBWA_DECLARE(EM_LINELENGTH)
		OnMFCSDKMessageDispatcherBWA_DECLARE(EM_LINESCROLL)
		OnMFCSDKMessageDispatcherBWA_DECLARE(EM_REPLACESEL)
		OnMFCSDKMessageDispatcherBWA_DECLARE(EM_SETSEL)
		OnMFCSDKMessageDispatcherBWA_DECLARE(EM_SCROLLCARET)
		OnMFCSDKMessageDispatcherBWA_DECLARE(WM_GETFONT)
		OnMFCSDKMessageDispatcherBWA_DECLARE(WM_SETFONT)
#endif

	protected:
		DECLARE_MESSAGE_MAP ()
};






/*
@CLASS:			Led_MFC_DragAndDropWindow<BASECLASS>
@BASES:			BASECLASS
@DESCRIPTION:	<p>NB: BASECLASS <em>must</em> derive (possibly indirectly) from CWnd.</p>
		<p>To use this class, you must also
	define DoDeclare_Led_MFC_DragAndDropWindow_MessageMap() to declare the actual message map for the template (unless its already done for yours
	in Led_MFC.cpp).</p>
		<p>Probably eventually migrate more stuff from @'Led_MFC_CViewHelper<BASECLASS>' here to this class. But for now,
	I'm risk averse. I don't want to break the class library. And this is all that is needed to move out to meet the immediate
	complaints.</p>
*/
template	<typename	BASECLASS = Led_MFC_Helper<> >	class	Led_MFC_DragAndDropWindow :
	public BASECLASS
{
	private:
		typedef	BASECLASS	inherited;

	protected:
		Led_MFC_DragAndDropWindow ();
	public:
		~Led_MFC_DragAndDropWindow ();

	public:
		/*
		@CLASS:			Led_MFC_DragAndDropWindow<BASECLASS>::CommandNames
		@DESCRIPTION:	<p>Command names for each of the user-visible commands produced by the @'TextInteractor' module.
					This name is used used in the constructed Undo command name, as
					in, "Undo Drag and Drop". You can replace this name with whatever you like.
					You change this value with @'TextInteractor::SetCommandNames'.</p>
						<p> The point of this is to allow for different UI-language localizations,
					without having to change Led itself.</p>
						<p>See also @'Led_MFC_DragAndDropWindow<BASECLASS>::GetCommandNames'.</p>
						<p>See also @'TextInteractor::CommandNames'.</p>
						<p>Note - also - this class must be declared lexically inside the
					outer template -and not outside - do to compiler bugs with MSVC60(SP4).
					Didn't take the time to trace them down carefully - LGP 2000-08-19.</p>
		*/
			struct	CommandNames {
				CommandNames ():
					fDragCommandName (Led_SDK_TCHAROF ("Drag")),
					fDropCommandName (Led_SDK_TCHAROF ("Drop")),
					fDragNDropCommandName (Led_SDK_TCHAROF ("Drag and Drop"))
					{
					}

				Led_SDK_String	fDragCommandName;
				Led_SDK_String	fDropCommandName;
				Led_SDK_String	fDragNDropCommandName;
			};

	// This class builds commands with command names. The UI may wish to change these
	// names (eg. to customize for particular languages, etc)
	// Just patch these strings here, and commands will be created with these names.
	// (These names appear in text of undo menu item)
	public:
		static	const CommandNames&	GetCommandNames ();
		static	void				SetCommandNames (const CommandNames& cmdNames);
	private:
		static	CommandNames	sCommandNames;

	protected:
		COleDropTarget	fDropTarget;

	protected:
		afx_msg		int		OnCreate (LPCREATESTRUCT lpCreateStruct);

	// mouse down tracking... and drag and drop...
	protected:
		virtual		bool		IsADragSelect (Led_Point clickedWhere) const;
		nonvirtual	void		HandleDragSelect (UINT nFlags, CPoint oPoint);
		nonvirtual	void		HandleSharedDragOverUpdateSelCode ();
		nonvirtual	DROPEFFECT	HelperDragEnter (COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
		nonvirtual	DROPEFFECT	HelperDragOver (COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
		nonvirtual	BOOL		HelperDrop (COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
		nonvirtual	void		HelperDragLeave ();

	protected:
		CPoint			fDragPoint;		// current position
		CSize			fDragSize;		// size of dragged object
		CSize			fDragOffset;	// offset to focus rect
		DROPEFFECT		fPrevDropEffect;

	protected:
		nonvirtual	bool	GetObjectInfo (COleDataObject* pDataObject, CSize* pSize, CSize* pOffset);

	protected:
		afx_msg		void	OnTimer (UINT nEventID);
	private:
		nonvirtual	void	StartDragAutoscrollTimer ();
		nonvirtual	void	StopDragAutoscrollTimer ();
		enum	{ eAutoscrolling4DragTimerEventID	=	435 };	// Magic#
		UINT	fDragAutoScrollTimerID;							// zero means no timer	

	private:
		struct	LedStartDragAndDropContext {
			public:
				LedStartDragAndDropContext (Led_MFC_DragAndDropWindow<BASECLASS>* ledMFCView):
					fOurDrag (false),
					fOurDragStart (0),
					fOurDragEnd (0),
					fWeRecievedDrop (false),
					fLedMFCView (ledMFCView),
					fOrigSelection (ledMFCView->GetTextStore (), ledMFCView->GetSelectionStart (), ledMFCView->GetSelectionEnd ())
				{
				}
			public:
				bool									fOurDrag;
				size_t									fOurDragStart;
				size_t									fOurDragEnd;
				bool									fWeRecievedDrop;
				Led_MFC_DragAndDropWindow<BASECLASS>*	fLedMFCView;
				TempMarker								fOrigSelection;
		};

		static	LedStartDragAndDropContext*	sCurrentDragInfo;

	protected:
		DECLARE_MESSAGE_MAP ()
};








/*
@CLASS:			Led_MFC_CViewHelper<BASECLASS>
@BASES:			BASECLASS
@DESCRIPTION:	<p>NB: BASECLASS <em>must</em> derive (possibly indirectly) from CView.</p>
		<p>To use this class, you must also
	define DoDeclare_Led_MFC_CViewHelper_MessageMap() to declare the actual message map for the template (unless its already done for yours
	in Led_MFC.cpp).</p>
*/
template	<typename	BASECLASS = Led_MFC_DragAndDropWindow<> >	class	Led_MFC_CViewHelper :
	public BASECLASS
{
	private:
		typedef	BASECLASS	inherited;

	protected:
		Led_MFC_CViewHelper ();

	public:
		virtual ~Led_MFC_CViewHelper ();


	// CEditView storage methods.
	public:
		virtual		void	DeleteContents ();
		virtual		void	Serialize (CArchive& ar);
		nonvirtual	void	SerializeRaw (CArchive& ar);
		nonvirtual	void	ReadFromArchive (CArchive& ar, UINT nLen);
		nonvirtual	void	WriteToArchive (CArchive& ar);

	protected:
		afx_msg		void	OnPaint ();
		override	void	OnDraw (CDC* pDC);
		afx_msg		void	OnLButtonDown (UINT nFlags, CPoint oPoint);
		afx_msg		int		OnCreate (LPCREATESTRUCT lpCreateStruct);
		afx_msg		void	OnVScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg		void	OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	public:
		nonvirtual	Led_TWIPS_Rect	GetPrintMargins () const;
		nonvirtual	void			SetPrintMargins (const Led_TWIPS_Rect& printMargins);
	private:
		Led_TWIPS_Rect	fPrintMargins;

	protected:
		override	BOOL	OnPreparePrinting (CPrintInfo* pInfo);
		override	void	OnBeginPrinting (CDC* pDC, CPrintInfo* pInfo);
		override	void	OnPrint (CDC* pDC, CPrintInfo* pInfo);
		override	void	OnEndPrinting (CDC* pDC, CPrintInfo* pInfo);
	public:
		override	void	OnPrepareDC (CDC* pDC, CPrintInfo* pInfo);
	protected:
		virtual		Led_Rect	CalculatePrintingRect (CDC* pDC) const;
	protected:
		struct	PrintInfo {
			PrintInfo (BASECLASS& editor, CDC* useTablet, Led_Rect oldWindowRect, size_t savedScrollPos, bool savedForceAlLRowsFlag):
					fTmpTablet (useTablet),
					fOldUpdateTablet (editor, fTmpTablet, TemporarilyUseTablet::eDontDoTextMetricsChangedCall),
					fOldWindowRect (oldWindowRect),
					fSavedScrollPos (savedScrollPos),
					fSavedForceAllRowsFlag (savedForceAlLRowsFlag),
					fWindowStarts ()
				{
				}
			Led_MFC_TabletFromCDC						fTmpTablet;
			typename BASECLASS::TemporarilyUseTablet	fOldUpdateTablet;
			Led_Rect									fOldWindowRect;
			size_t										fSavedScrollPos;
			bool										fSavedForceAllRowsFlag;
			vector<size_t>								fWindowStarts;
		};
		PrintInfo*	fPrintInfo;		// Non-NULL only during printing...


	// mouse down tracking... and drag and drop...
	protected:
		override	DROPEFFECT	OnDragEnter (COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
		override	DROPEFFECT	OnDragOver (COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
		override	BOOL		OnDrop (COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
		override	void		OnDragLeave ();

	public:
		override	void	InvalidateScrollBarParameters ();

	protected:
		override	void	UpdateScrollBars ();

	protected:
		DECLARE_MESSAGE_MAP ()
};





/*
@CLASS:			Led_MFC
@BASES:			@'Led_MFC_CViewHelper<BASECLASS>', where BASECLASS=@'Led_MFC_OptionalWin32SDKMessageMimicHelper<BASECLASS>, where BASECLASS=@'Led_MFC_MimicMFCAPIHelper<BASECLASS>', where BASECLASS == @'Led_MFC_Helper<BASECLASS>', where BASECLASS=CView
@DESCRIPTION:	<p>This template is provided partly as a convenience, for such a common case. But MOSTLY as backwards compatability
	for Led 2.2. In Led 2.2, Led_MFC was the SOLE class provided to integrated Led with MFC. Now there is a whole suite of
	individually selectable templates to provide that interfacing.</p>
*/
typedef	Led_MFC_CViewHelper<Led_MFC_DragAndDropWindow<Led_MFC_OptionalWin32SDKMessageMimicHelper <Led_MFC_MimicMFCAPIHelper <Led_MFC_Helper<CView,TextInteractor> > > > >	Led_MFC;






/*
@CLASS:			Led_MFC_COleControlHelper<BASECLASS>
@BASES:			BASECLASS
@DESCRIPTION:	<p>NB: BASECLASS <em>must</em> derive (possibly indirectly) from CView.</p>
		<p>To use this class, you must also
	define DoDeclare_Led_MFC_COleControlHelper_MessageMap() to declare the actual message map for the template (unless its already done for yours
	in Led_MFC.cpp).</p>
*/
template	<typename	BASECLASS = Led_MFC_DragAndDropWindow<> >	class	Led_MFC_COleControlHelper :
	public BASECLASS
{
	private:
		typedef	BASECLASS	inherited;
	public:
		typedef	TextInteractor::UpdateMode	UpdateMode;

	protected:
		Led_MFC_COleControlHelper () {}

	public:
		virtual ~Led_MFC_COleControlHelper () {}

	public:
		nonvirtual	void	Refresh (UpdateMode updateMode = TextInteractor::eDefaultUpdate) const
			{
				const TextInteractor*	ti	=	this;
				ti->Refresh (updateMode);
			}
		nonvirtual	void	Refresh (const Led_Rect& area, UpdateMode updateMode = TextInteractor::eDefaultUpdate) const
			{
				const TextInteractor*	ti	=	this;
				ti->Refresh (area, updateMode);
			}
		nonvirtual	void	Refresh (size_t from, size_t to, UpdateMode updateMode = TextInteractor::eDefaultUpdate) const
			{
				const TextInteractor*	ti	=	this;
				ti->Refresh (from, to, updateMode);
			}
		nonvirtual	void	Refresh (const Marker* range, UpdateMode updateMode = TextInteractor::eDefaultUpdate) const
			{
				const TextInteractor*	ti	=	this;
				ti->Refresh (range, updateMode);
			}

	protected:
		DECLARE_MESSAGE_MAP ()
};


//	class	Led_MFC_COleControlHelper<BASECLASS>
#define	DoDeclare_Led_MFC_COleControlHelper_MessageMap(MFC_BASE_CLASS)\
	BEGIN_TEMPLATE_MESSAGE_MAP(Led_MFC_COleControlHelper, MFC_BASE_CLASS, MFC_BASE_CLASS)\
	END_MESSAGE_MAP()




/*
@CLASS:			Led_MFC_ExceptionHandlerHelper<BASECLASS>
@BASES:			BASECLASS = @'Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>'
@DESCRIPTION:	<p>This helper is designed to manage exception handling, when the Led class is being used in a context (say OCX)
	where it cannot throw exceptions in error situations.</p>
		<p>This class wraps many messages the control is likely to get, and when there is an exception, it calls a virtual
	exception-handling method (which by default - beeps), and then returns normally.</p>
		<p>To use this class, you must also
	define DoDeclare_Led_MFC_ExceptionHandlerHelper_MessageMap() to declare the actual message map for the template.</p>
*/
template	<typename	BASECLASS = Led_MFC >	class	Led_MFC_ExceptionHandlerHelper :
	public BASECLASS
{
	private:
		typedef	BASECLASS	inherited;

	protected:
		virtual	void	HandleException () const;

	public:
		afx_msg		void	OnChar (UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg		LRESULT	OnIMEChar (WPARAM wParam, LPARAM lParam);
		afx_msg		void	OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg		LRESULT	OnMsgPaste (WPARAM wParam, LPARAM lParam);
		afx_msg		void	OnLButtonDown (UINT nFlags, CPoint oPoint);
		afx_msg		void	OnLButtonUp (UINT /*nFlags*/, CPoint oPoint);
		afx_msg		void	OnLButtonDblClk (UINT /*nFlags*/, CPoint oPoint);
		afx_msg		BOOL	OnCmdMsg (UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

	protected:
		DECLARE_MESSAGE_MAP ()
};









/*
@CLASS:			Led_MFC_X
@BASES:			ChosenInteractor, @'Led_MFC'
@DESCRIPTION:	<p>Utility template to mix together Led_MFC an a <code>ChosenInteractor</code> which already has support
	for a particular TextImager mixed in.</p>
*/
template	<typename	ChosenInteractor, typename LEDMFC = Led_MFC>	class	Led_MFC_X :
	#if		qMFCRequiresCWndLeftmostBaseClass
		public LEDMFC, public ChosenInteractor
	#else
		public ChosenInteractor, public LEDMFC
	#endif
{
	public:
		Led_MFC_X ();

	public:
		override	void	AboutToUpdateText (const MarkerOwner::UpdateInfo& updateInfo);
		override	void	DidUpdateText (const MarkerOwner::UpdateInfo& updateInfo) throw ();
};













/*
@CLASS:			Led_MFCReaderDAndDFlavorPackage
@BASES:			@'ReaderFlavorPackage'
@DESCRIPTION:	<p>ReaderFlavorPackage which reads from a COleDataObject as
	its underlying storage. Useful for Drop (Drag and Drop) processing.</p>
*/
class	Led_MFCReaderDAndDFlavorPackage : public ReaderFlavorPackage {
	public:
		Led_MFCReaderDAndDFlavorPackage (COleDataObject* dataObject);

		override	bool	GetFlavorAvailable (Led_ClipFormat clipFormat) const;
		override	size_t	GetFlavorSize (Led_ClipFormat clipFormat) const;
		override	size_t	ReadFlavorData (Led_ClipFormat clipFormat, size_t bufSize, void* buf) const;

		nonvirtual	COleDataObject*	GetOleDataObject () const;

	private:
		COleDataObject*	fDataObject;
};






/*
@CLASS:			Led_MFCWriterDAndDFlavorPackage
@BASES:			@'WriterFlavorPackage'
@DESCRIPTION:	<p>A WriterFlavorPackage which writes to a COleDataObject as
	its underlying storage. Useful for Drag (Drag and Drop) processing.</p>
*/
class	Led_MFCWriterDAndDFlavorPackage : public WriterFlavorPackage {
	public:
		Led_MFCWriterDAndDFlavorPackage (COleDataSource* dataObject);

		override	void	AddFlavorData (Led_ClipFormat clipFormat, size_t bufSize, const void* buf);

		nonvirtual	COleDataSource*	GetOleDataSource () const;

	private:
		COleDataSource*	fDataObject;
};










template	<typename	TRIVIALWORDWRAPPEDTEXTIMAGER, typename SIMPLETEXTIMAGER, typename TEXTSTORE>
	CSize	Led_GetTextExtent (CDC* cdc, const Led_tString& text, CRect r, bool wordWrap);

template	<typename	TRIVIALWORDWRAPPEDTEXTIMAGER, typename SIMPLETEXTIMAGER, typename TEXTSTORE>
	void	Led_DrawText (CDC* cdc, const Led_tString& text, CRect r, bool wordWrap);

#if		qSupportDrawTextGetTextExtent
	CSize	Led_GetTextExtent (CDC* cdc, const Led_tString& text, CRect r, bool wordWrap);
	void	Led_DrawText (CDC* cdc, const Led_tString& text, CRect r, bool wordWrap);
#endif








/*
@CLASS:			MFC_CommandNumberMapping
@DESCRIPTION:	<p></p>
*/
class	MFC_CommandNumberMapping : public CommandNumberMapping<UINT> {
	protected:
		MFC_CommandNumberMapping ();
};






/*
@CLASS:			Led_MFC_TmpCmdUpdater
@BASES:			@'TextInteractor::CommandUpdater'
@DESCRIPTION:	<p>Helper used in @'Led_Win32_Helper<BASE_INTERACTOR>'</p>
*/
class	Led_MFC_TmpCmdUpdater : public TextInteractor::CommandUpdater {
	public:
		explicit Led_MFC_TmpCmdUpdater (CCmdUI* pCmdUI);

	public:
		override	CommandNumber	GetCmdID () const;
		override	bool			GetEnabled () const;
		override	void			SetEnabled (bool enabled);
		override	void			SetChecked (bool checked);
		override	void			SetText (const Led_SDK_Char* text);

	public:
		operator CCmdUI* ();
		operator Led_MFC_TmpCmdUpdater* ();

	private:
		CommandNumber	fCmdNum;
		CCmdUI*			fCmdUI;
		bool			fEnabled;
};






/*
@CLASS:			LED_MFC_HANDLE_COMMAND
@DESCRIPTION:	<p>Trivial helper for building MFC message maps.</p>
*/
#define	LED_MFC_HANDLE_COMMAND(A)	\
					ON_COMMAND_RANGE(A,A,OnPerformCommand_MSG)\
					ON_UPDATE_COMMAND_UI(A,OnUpdateCommand_MSG)



/*
@CLASS:			LED_MFC_HANDLE_COMMAND_RANGE
@DESCRIPTION:	<p>Trivial helper for building MFC message maps.</p>
*/
#define	LED_MFC_HANDLE_COMMAND_RANGE(A,B)	\
					ON_COMMAND_RANGE(A,B,OnPerformCommand_MSG)\
					ON_UPDATE_COMMAND_UI_RANGE(A,B,OnUpdateCommand_MSG)






/*
@CLASS:			LED_MFC_HANDLE_COMMAND_M
@DESCRIPTION:	<p></p>
*/
#define	LED_MFC_HANDLE_COMMAND_M(A)	\
					LED_MFC_HANDLE_COMMAND(MFC_CommandNumberMapping::Get ().ReverseLookup (A))

/*
@CLASS:			LED_MFC_HANDLE_COMMAND_RANGE_M
@DESCRIPTION:	<p></p>
*/
#define	LED_MFC_HANDLE_COMMAND_RANGE_M(A,B)	\
					LED_MFC_HANDLE_COMMAND_RANGE(MFC_CommandNumberMapping::Get ().ReverseLookup (A), MFC_CommandNumberMapping::Get ().ReverseLookup (B))













/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#if		qLedCheckCompilerFlagsConsistency
	namespace LedCheckCompilerFlags_Led_MFC {
		extern	int	LedCheckCompilerFlags_(qMFCRequiresCWndLeftmostBaseClass);

		struct	FlagsChecker {
			FlagsChecker ()
				{
					/*
					 *	See the docs on @'qLedCheckCompilerFlagsConsistency' if this ever fails.
					 */
					if (LedCheckCompilerFlags_(qMFCRequiresCWndLeftmostBaseClass) != qMFCRequiresCWndLeftmostBaseClass)		{			abort ();		}
				}
		};
		static	struct	FlagsChecker	sFlagsChecker;
	}
#endif


	/*
	@METHOD:		AsCPoint
	@DESCRIPTION:	Convert a @'Led_Point' to an MFC CPoint.
		<p>See also @'AsLedPoint', @'AsCRect', @'AsLedRect'.
	*/
	inline	CPoint	AsCPoint (Led_Point p)
		{
			return CPoint (p.h, p.v);
		}
	/*
	@METHOD:		AsLedPoint
	@DESCRIPTION:	Convert an MFC CPoint to a @'Led_Point'.
		<p>See also @'AsCPoint', @'AsCRect', @'AsLedRect'.
	*/
	inline	Led_Point	AsLedPoint (CPoint p)
		{
			return Led_Point (p.y, p.x);
		}
	/*
	@METHOD:		AsCRect
	@DESCRIPTION:	Convert a Led_Rect to an MFC CRect.
		<p>See also @'AsCPoint', @'AsLedPoint', @'AsLedRect'.
	*/
	inline	CRect	AsCRect (Led_Rect r)
		{
			return CRect (r.left, r.top, r.right, r.bottom);
		}
	/*
	@METHOD:		AsLedRect
	@DESCRIPTION:	Convert an MFC CRect to a Led_Rect.
		<p>See also @'AsCPoint', @'AsLedPoint', @'AsCRect'.
	*/
	inline	Led_Rect	AsLedRect (CRect r)
		{
			return Led_Rect (r.top, r.left, r.Height (), r.Width ());
		}
	/*
	@METHOD:		AsCSize
	@DESCRIPTION:	Convert a Led_Size to an MFC CSize.
	*/
	inline	CSize	AsCSize (Led_Size s)
		{
			return CSize (s.h, s.v);
		}





//	class	Led_MFC_CDCFromTablet
	inline	Led_MFC_CDCFromTablet::Led_MFC_CDCFromTablet (Led_Tablet t):
		fCDC ()
		{
			Led_RequireNotNil (t);
			fCDC.m_hDC = t->m_hDC;
			fCDC.m_hAttribDC = t->m_hAttribDC;
			fCDC.m_bPrinting = t->m_bPrinting;
		}
	inline	Led_MFC_CDCFromTablet::~Led_MFC_CDCFromTablet ()
		{
			fCDC.Detach ();
		}
	inline	Led_MFC_CDCFromTablet::operator CDC* ()
		{
			return &fCDC;
		}



//	class	Led_MFC_TabletFromCDC
	inline	Led_MFC_TabletFromCDC::Led_MFC_TabletFromCDC (CDC* pDC):
		fTablet ()
		{
			Led_RequireNotNil (pDC);
			fTablet.m_hDC = pDC->m_hDC;
			fTablet.m_hAttribDC = pDC->m_hAttribDC;
			fTablet.m_bPrinting = pDC->m_bPrinting;
		}
	inline	Led_MFC_TabletFromCDC::~Led_MFC_TabletFromCDC ()
		{
			fTablet.Detach ();
		}
	inline	Led_MFC_TabletFromCDC::operator Led_Tablet ()
		{
			return &fTablet;
		}
	inline	Led_Tablet Led_MFC_TabletFromCDC::operator-> ()
		{
			return &fTablet;
		}



#define BEGIN_TEMPLATE_2_MESSAGE_MAP(theClass, type_name1, type_name2, baseClass)			\
	PTM_WARNING_DISABLE														\
	template < typename type_name1, typename type_name2 >											\
	const AFX_MSGMAP* theClass< type_name1, type_name2 >::GetMessageMap() const			\
		{ return GetThisMessageMap(); }										\
	template < typename type_name1, typename type_name2 >											\
	const AFX_MSGMAP* PASCAL theClass< type_name1, type_name2 >::GetThisMessageMap()		\
	{																		\
		typedef theClass< type_name1, type_name2 > ThisClass;							\
		typedef baseClass TheBaseClass;										\
		static const AFX_MSGMAP_ENTRY _messageEntries[] =					\
		{



//	class	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>
#define	DoDeclare_Led_MFC_Helper_MessageMap(MFC_BASE_CLASS,BASE_INTERACTOR)\
	BEGIN_TEMPLATE_2_MESSAGE_MAP(Led_MFC_Helper, MFC_BASE_CLASS, BASE_INTERACTOR, MFC_BASE_CLASS)\
		ON_WM_CREATE()\
		ON_WM_CHAR()\
		ON_WM_TIMER()\
		ON_WM_KEYDOWN()\
		ON_WM_PAINT	()\
		ON_WM_MOUSEMOVE()\
		ON_WM_LBUTTONDOWN()\
		ON_WM_SETCURSOR()\
		ON_WM_GETDLGCODE ()\
		ON_WM_LBUTTONUP()\
		ON_WM_LBUTTONDBLCLK()\
		ON_WM_SETFOCUS()\
		ON_WM_KILLFOCUS()\
		ON_WM_SIZE		()\
		ON_WM_ERASEBKGND()\
		ON_WM_VSCROLL	()\
		ON_WM_HSCROLL	()\
		ON_WM_MOUSEWHEEL()\
		ON_WM_ENABLE	()\
		ON_MESSAGE (WM_UNICHAR, OnUniChar)\
		ON_MESSAGE (WM_IME_CHAR, OnIMEChar)\
		ON_MESSAGE (WM_IME_COMPOSITION, OnIME_COMPOSITION)\
		ON_MESSAGE (WM_IME_ENDCOMPOSITION, OnIME_ENDCOMPOSITION)\
		\
		LED_MFC_HANDLE_COMMAND_M	(BASE_INTERACTOR::kUndo_CmdID)\
		LED_MFC_HANDLE_COMMAND_M	(BASE_INTERACTOR::kRedo_CmdID)\
		LED_MFC_HANDLE_COMMAND_M	(BASE_INTERACTOR::kSelectAll_CmdID)\
		LED_MFC_HANDLE_COMMAND_M	(BASE_INTERACTOR::kSelectWord_CmdID)\
		LED_MFC_HANDLE_COMMAND_M	(BASE_INTERACTOR::kSelectTextRow_CmdID)\
		LED_MFC_HANDLE_COMMAND_M	(BASE_INTERACTOR::kSelectParagraph_CmdID)\
		LED_MFC_HANDLE_COMMAND_M	(BASE_INTERACTOR::kCut_CmdID)\
		LED_MFC_HANDLE_COMMAND_M	(BASE_INTERACTOR::kCopy_CmdID)\
		LED_MFC_HANDLE_COMMAND_M	(BASE_INTERACTOR::kPaste_CmdID)\
		LED_MFC_HANDLE_COMMAND_M	(BASE_INTERACTOR::kClear_CmdID)\
		LED_MFC_HANDLE_COMMAND_M	(BASE_INTERACTOR::kFind_CmdID)\
		LED_MFC_HANDLE_COMMAND_M	(BASE_INTERACTOR::kFindAgain_CmdID)\
		LED_MFC_HANDLE_COMMAND_M	(BASE_INTERACTOR::kEnterFindString_CmdID)\
		LED_MFC_HANDLE_COMMAND_M	(BASE_INTERACTOR::kReplace_CmdID)\
		LED_MFC_HANDLE_COMMAND_M	(BASE_INTERACTOR::kReplaceAgain_CmdID)\
		LED_MFC_HANDLE_COMMAND_M	(BASE_INTERACTOR::kSpellCheck_CmdID)\
	END_MESSAGE_MAP()


#if		qSupportLed30CompatAPI
	#define	DoDeclare_TextInteractorCommonCommandHelper_MFC_MessageMap(BASECLASS,CMD_INFO,CMD_ENABLER)
#endif


	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		inline	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::Led_MFC_Helper ():
				LED_WIN32_HELPER (),
				MFC_BASE_CLASS ()
			{
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		inline	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::~Led_MFC_Helper ()
			{
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		HWND	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::GetHWND () const
			{
				return m_hWnd;
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		int	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnCreate (LPCREATESTRUCT lpCreateStruct)
			{
				Led_RequireNotNil (lpCreateStruct);
				if (MFC_BASE_CLASS::OnCreate (lpCreateStruct) != 0) {
					return -1;	// failed to create
				}
				TabletChangedMetrics ();		// maybe should catch failures here, and return -1?
				if ((lpCreateStruct->style & WS_VSCROLL) and GetScrollBarType (v) == eScrollBarNever) {
					SetScrollBarType (v, eScrollBarAlways);
				}
				if ((lpCreateStruct->style & WS_HSCROLL) and GetScrollBarType (h) == eScrollBarNever) {
					SetScrollBarType (h, eScrollBarAlways);
				}
				return 0;	// sucess
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		void	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnPaint ()
			{
				CPaintDC	dc (this);
				CRect		boundsRect	=	CRect (0, 0, 0, 0);
				Led_Verify (dc.GetClipBox (&boundsRect) != ERROR);
				WindowDrawHelper (Led_MFC_TabletFromCDC (&dc), AsLedRect (boundsRect), false);
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		/*
		@METHOD:		Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnSetCursor
		@DESCRIPTION:	Hook the Win32 SDK WM_SETCURSOR message to handle set the cursor to an I-Beam, as appropriate. When over
			draggable text, instead use a standard arrow cursor.
		*/
		BOOL	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnSetCursor (CWnd* pWnd, UINT nHitTest, UINT message)
			{
				return OnSetCursor_Msg (pWnd->GetSafeHwnd (), nHitTest, message);
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		UINT	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnGetDlgCode ()
			{
				return OnGetDlgCode_Msg ();
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		void	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnChar (UINT nChar, UINT nRepCnt, UINT nFlags)
		{
			OnChar_Msg (nChar, nRepCnt + (nFlags<<16));
		}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		void	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
		{
			OnKeyDown_Msg (nChar, nRepCnt + (nFlags<<16));
		}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		void	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnMouseMove (UINT nFlags, CPoint oPoint)
			{
				OnMouseMove_Msg (nFlags, oPoint.x, oPoint.y);
			}
#if		qSupportLed30CompatAPI
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		BOOL	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::SubclassWindow (HWND hWnd)
			{
				/*
				 *	We've never bothered really supporting the SubclassWindow mechanism since it
				 *	didn't seem to work very well. In Led 3.0 and earlier, we REALLY just did
				 *	a REPLACE. Now just cleanup our nomenclature, but keep this routine name
				 *	around for backward compatability.
				 */
				return ReplaceWindow (hWnd);
			}
#endif
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		/*
		@METHOD:		Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::ReplaceWindow
		@DESCRIPTION:	<p>Similar to @'Led_Win32_SimpleWndProc_Helper<BASE_INTERACTOR>::ReplaceWindow'
					except that it works with MFC windows and MFC's CWnd message maps etc.
					</p>
		*/
		BOOL	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::ReplaceWindow (HWND hWnd)
			{
				CWnd*	parent		=	MFC_BASE_CLASS::FromHandle (hWnd)->GetParent ();
				HWND	hwndParent	=	parent->GetSafeHwnd ();
				if (hwndParent == NULL) {
					return false;
				}

				int id = ::GetWindowLong (hWnd, GWL_ID);

				HWND hwndEdit = ::GetDlgItem (hwndParent, id);
				Led_AssertNotNil (hwndEdit);

				DWORD	dwStyle = ::GetWindowLong (hwndEdit, GWL_STYLE);
				DWORD	exStyle = ::GetWindowLong (hwndEdit, GWL_EXSTYLE);

				// Assume edit widget's position.
				WINDOWPLACEMENT wp;
				memset (&wp, 0, sizeof (wp));
				wp.length = sizeof (wp);
				Led_Verify (::GetWindowPlacement (hwndEdit, &wp));

				// Delete the edit widget window.
				::DestroyWindow (hwndEdit);

				return CreateEx (exStyle, NULL, NULL, dwStyle | WS_CHILD,
						wp.rcNormalPosition.left, wp.rcNormalPosition.top,
						wp.rcNormalPosition.right - wp.rcNormalPosition.left, wp.rcNormalPosition.bottom - wp.rcNormalPosition.top,
						parent->GetSafeHwnd (), (HMENU)id, NULL
					);

			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		/*
		@METHOD:		Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::GetWindowRect
		@DESCRIPTION:	Return the Led WindowRect. This name is a somewhat unfortunate choice for Windows, because WindowRect means something
			vaguely similar, but substantially different in the Win32 SDK.
				<p>In the future, I may need to consider changing this name. But for now, I can think of none better.
				<p>I provide two overloads of this routine. The one that returns  a Led_Rect returns the Led WindowRect. And the one
			that takes an LPRECT parameter returns the Win32 SDK WindowRect.
		*/
		inline	Led_Rect	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::GetWindowRect () const
			{
				return (TextImager::GetWindowRect ());
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		inline	void	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::GetWindowRect (LPRECT lpRect) const
			{
				// The CWnd version of GetWindowRect ()
				const CWnd*	cwnd	=	this;
				CWnd*		wnd		=	const_cast<CWnd*> (cwnd);
				wnd->GetWindowRect (lpRect);
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		inline	DWORD	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::GetStyle () const
			{
				return MFC_BASE_CLASS::GetStyle ();
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		void	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnLButtonDown (UINT nFlags, CPoint oPoint)
			{
				OnLButtonDown_Msg (nFlags, oPoint.x, oPoint.y);
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		void	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnLButtonUp (UINT nFlags, CPoint oPoint)
			{
				OnLButtonUp_Msg (nFlags, oPoint.x, oPoint.y);
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		void	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnLButtonDblClk (UINT nFlags, CPoint oPoint)
			{
				OnLButtonDblClk_Msg (nFlags, oPoint.x, oPoint.y);
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		void	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnSetFocus (CWnd* pOldWnd)
			{
				MFC_BASE_CLASS::OnSetFocus (pOldWnd);
				OnSetFocus_Msg (pOldWnd->GetSafeHwnd ());
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		void	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnKillFocus (CWnd* pNewWnd)
			{
				OnKillFocus_Msg (pNewWnd->GetSafeHwnd ());
				MFC_BASE_CLASS::OnKillFocus (pNewWnd);
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		void	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnSize (UINT nType, int cx, int cy)
			{
				MFC_BASE_CLASS::OnSize (nType, cx, cy);
				OnSize_Msg ();
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		BOOL	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnEraseBkgnd (CDC* pDC)
			{
				return OnEraseBkgnd_Msg (pDC->GetSafeHdc ());
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		void	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnVScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
			{
				OnVScroll_Msg (nSBCode, nPos, pScrollBar->GetSafeHwnd ());
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		void	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
			{
				OnHScroll_Msg (nSBCode, nPos, pScrollBar->GetSafeHwnd ());
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		BOOL	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnMouseWheel (UINT fFlags, short zDelta, CPoint point)
			{
				return OnMouseWheel_Msg (MAKEWPARAM (fFlags, zDelta), MAKELPARAM (point.x, point.y));
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		void	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnEnable (BOOL bEnable)
			{
				OnEnable_Msg (bEnable);
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		void	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnTimer (UINT nEventID)
			{
				OnTimer_Msg (nEventID, NULL);
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		inline	LRESULT	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnUniChar (WPARAM wParam, LPARAM lParam)
			{
				#if		qWideCharacters
					return LED_WIN32_HELPER::OnUniChar_Msg (wParam, lParam);
				#else
					return MFC_BASE_CLASS::Default ();
				#endif
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		inline	LRESULT	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnIMEChar (WPARAM wParam, LPARAM lParam)
			{
				#if		qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug
					return LED_WIN32_HELPER::OnIMEChar_Msg (wParam, lParam);
				#else
					return MFC_BASE_CLASS::Default ();
				#endif
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		inline	LRESULT	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnIME_COMPOSITION (WPARAM wParam, LPARAM lParam)
			{
				#if		qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug
					return LED_WIN32_HELPER::OnIME_COMPOSITION_Msg (wParam, lParam);
				#else
					return MFC_BASE_CLASS::Default ();
				#endif
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		inline	LRESULT	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnIME_ENDCOMPOSITION (WPARAM wParam, LPARAM lParam)
			{
				#if		qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug
					return LED_WIN32_HELPER::OnIME_ENDCOMPOSITION_Msg (wParam, lParam);
				#else
					return MFC_BASE_CLASS::Default ();
				#endif
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		void	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnUpdateCommand_MSG (CCmdUI* pCmdUI)
			{
				Led_RequireNotNil (pCmdUI);
				if (not OnUpdateCommand (Led_MFC_TmpCmdUpdater (pCmdUI))) {
					pCmdUI->Enable (false);		// disable commands by default (SPR#1462)
				}
			}
	template	<typename	MFC_BASE_CLASS, typename BASE_INTERACTOR>
		void	Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnPerformCommand_MSG (UINT commandNumber)
			{
				IdleManager::NonIdleContext	nonIdleContext;
				(void)OnPerformCommand (MFC_CommandNumberMapping::Get ().Lookup (commandNumber));
			}






//	class	Led_MFC_MimicMFCAPIHelper<BASECLASS>
#define	DoDeclare_Led_MFC_MimicMFCAPIHelper_MessageMap(MFC_BASE_CLASS)\
	BEGIN_TEMPLATE_MESSAGE_MAP(Led_MFC_MimicMFCAPIHelper, MFC_BASE_CLASS, MFC_BASE_CLASS)\
	END_MESSAGE_MAP()
	template	<typename	BASECLASS>
		inline	Led_MFC_MimicMFCAPIHelper<BASECLASS>::Led_MFC_MimicMFCAPIHelper ()
			{
			}
	template	<typename	BASECLASS>
		inline	BOOL	Led_MFC_MimicMFCAPIHelper<BASECLASS>::Create (DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
			{
				return (BASECLASS::Create (NULL, NULL, dwStyle, rect, pParentWnd, nID));
			}
	template	<typename	BASECLASS>
		inline	BOOL	Led_MFC_MimicMFCAPIHelper<BASECLASS>::CanUndo () const
			{
				Led_Assert (::IsWindow (m_hWnd));
				return (BOOL)::SendMessage (m_hWnd, EM_CANUNDO, 0, 0);
			}
	template	<typename	BASECLASS>
		inline	int		Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetLineCount_CEdit () const
			{
				Led_Assert (::IsWindow (m_hWnd));
				return (int)::SendMessage (m_hWnd, EM_GETLINECOUNT, 0, 0);
			}
	template	<typename	BASECLASS>
		inline	BOOL	Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetModify () const
			{
				Led_Assert (false);	// though THIS code is fine - we haven't yet hooked the callback, so this call will
									// produce poor results... LGP 941129
				Led_Assert (::IsWindow (m_hWnd));
				return (BOOL)::SendMessage (m_hWnd, EM_GETMODIFY, 0, 0);
			}
	template	<typename	BASECLASS>
		inline	void	Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetModify (BOOL bModified)
			{
				Led_Assert (false);	// though THIS code is fine - we haven't yet hooked the callback, so this call will
									// produce poor results... LGP 941129
				Led_Assert (::IsWindow (m_hWnd));
				::SendMessage (m_hWnd, EM_SETMODIFY, bModified, 0);
			}
	template	<typename	BASECLASS>
		inline	void	Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetRect (LPRECT lpRect) const
			{
				Led_Assert (false);	// though THIS code is fine - we haven't yet hooked the callback, so this call will
									// produce poor results... LGP 941129
				Led_Assert (::IsWindow (m_hWnd));
				::SendMessage (m_hWnd, EM_GETRECT, 0, (LPARAM)lpRect);
			}
	template	<typename	BASECLASS>
		inline	void	Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetSel (int& nStartChar, int& nEndChar) const
			{
				Led_Assert (::IsWindow (m_hWnd));
				::SendMessage (m_hWnd, EM_GETSEL, (WPARAM)&nStartChar,(LPARAM)&nEndChar);
			}
	template	<typename	BASECLASS>
		inline	DWORD	Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetSel () const
			{
				Led_Assert (::IsWindow (m_hWnd));
				return ::SendMessage (m_hWnd, EM_GETSEL, 0, 0);
			}
	template	<typename	BASECLASS>
		inline	int Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetLine (int nIndex, LPTSTR lpszBuffer) const
			{
				Led_Assert (false);		// though THIS code is fine - we haven't yet hooked the callback, so this call will
										// produce poor results... LGP 941129
				Led_Assert (::IsWindow (m_hWnd));
				return (int)::SendMessage (m_hWnd, EM_GETLINE, nIndex, (LPARAM)lpszBuffer);
			}
	template	<typename	BASECLASS>
		inline	int Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetLine (int nIndex, LPTSTR lpszBuffer, int nMaxLength) const
			{
				Led_Assert (false);		// though THIS code is fine - we haven't yet hooked the callback, so this call will
										// produce poor results... LGP 941129
				Led_Assert (::IsWindow (m_hWnd));
				*(LPWORD)lpszBuffer = (WORD)nMaxLength;
				return (int)::SendMessage (m_hWnd, EM_GETLINE, nIndex, (LPARAM)lpszBuffer);
			}
	template	<typename	BASECLASS>
		inline	void	Led_MFC_MimicMFCAPIHelper<BASECLASS>::EmptyUndoBuffer ()
			{
				Led_Assert (::IsWindow (m_hWnd));
				::SendMessage (m_hWnd, EM_EMPTYUNDOBUFFER, 0, 0);
			}
	template	<typename	BASECLASS>
		inline	BOOL	Led_MFC_MimicMFCAPIHelper<BASECLASS>::FmtLines (BOOL bAddEOL)
			{
				Led_Assert (false);		// though THIS code is fine - we haven't yet hooked the callback, so this call will
										// produce poor results... LGP 941129
				Led_Assert (::IsWindow (m_hWnd));
				return (BOOL)::SendMessage (m_hWnd, EM_FMTLINES, bAddEOL, 0);
			}
	template	<typename	BASECLASS>
		inline	void	Led_MFC_MimicMFCAPIHelper<BASECLASS>::LimitText (int nChars)
			{
				Led_Assert (false);		// though THIS code is fine - we haven't yet hooked the callback, so this call will
										// produce poor results... LGP 941129
				Led_Assert (::IsWindow (m_hWnd));
				::SendMessage (m_hWnd, EM_LIMITTEXT, nChars, 0);
			}
	template	<typename	BASECLASS>
		inline	int		Led_MFC_MimicMFCAPIHelper<BASECLASS>::LineFromChar (int nIndex) const
			{
				Led_Assert (::IsWindow (m_hWnd));
				return (int)::SendMessage (m_hWnd, EM_LINEFROMCHAR, nIndex, 0);
			}
	template	<typename	BASECLASS>
		inline	int		Led_MFC_MimicMFCAPIHelper<BASECLASS>::LineIndex (int nLine) const
			{
				Led_Assert (::IsWindow (m_hWnd));
				return (int)::SendMessage (m_hWnd, EM_LINEINDEX, nLine, 0);
			}
	template	<typename	BASECLASS>
		inline	int		Led_MFC_MimicMFCAPIHelper<BASECLASS>::LineLength (int nLine) const
			{
				Led_Assert (::IsWindow (m_hWnd));
				return (int)::SendMessage (m_hWnd, EM_LINELENGTH, nLine, 0);
			}
	template	<typename	BASECLASS>
		inline	void	Led_MFC_MimicMFCAPIHelper<BASECLASS>::LineScroll (int nLines, int nChars)
			{
				Led_Assert (::IsWindow (m_hWnd));
				::SendMessage (m_hWnd, EM_LINESCROLL, nChars, nLines);
			}
	template	<typename	BASECLASS>
		inline	void	Led_MFC_MimicMFCAPIHelper<BASECLASS>::ReplaceSel (LPCTSTR lpszNewText)
			{
				Led_AssertNotNil (lpszNewText);
				Led_Assert (::IsWindow (m_hWnd));
				::SendMessage (m_hWnd, EM_REPLACESEL, 0, (LPARAM)lpszNewText);
			}
	template	<typename	BASECLASS>
		inline	void	Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetPasswordChar (TCHAR ch)
			{
				Led_Assert (false);		// though THIS code is fine - we haven't yet hooked the callback, so this call will
										// produce poor results... LGP 941129
				Led_Assert (::IsWindow (m_hWnd));
				::SendMessage (m_hWnd, EM_SETPASSWORDCHAR, ch, 0);
			}
	template	<typename	BASECLASS>
		inline	void	Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetRect (LPCRECT lpRect)
			{
				Led_Assert (false);		// though THIS code is fine - we haven't yet hooked the callback, so this call will
										// produce poor results... LGP 941129
				Led_Assert (::IsWindow (m_hWnd));
				::SendMessage (m_hWnd, EM_SETRECT, 0, (LPARAM)lpRect);
			}
	template	<typename	BASECLASS>
		inline	void	Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetRectNP (LPCRECT lpRect)
			{
				Led_Assert (false);		// though THIS code is fine - we haven't yet hooked the callback, so this call will
										// produce poor results... LGP 941129
				Led_Assert (::IsWindow (m_hWnd));
				::SendMessage (m_hWnd, EM_SETRECTNP, 0, (LPARAM)lpRect);
			}
	template	<typename	BASECLASS>
		inline	void	Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetSel (DWORD dwSelection, BOOL bNoScroll)
			{
				Led_Assert (::IsWindow (m_hWnd));
				::SendMessage (m_hWnd, EM_SETSEL, LOWORD(dwSelection), HIWORD(dwSelection));
				if (not bNoScroll) {
					::SendMessage (m_hWnd, EM_SCROLLCARET, 0, 0);
				}
			}
	template	<typename	BASECLASS>
		inline	void	Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetSel (int nStartChar, int nEndChar, BOOL bNoScroll)
			{
				Led_Assert (::IsWindow (m_hWnd));
				::SendMessage (m_hWnd, EM_SETSEL, nStartChar, nEndChar);
				if (not bNoScroll) {
					::SendMessage (m_hWnd, EM_SCROLLCARET, 0, 0);
				}
			}
	template	<typename	BASECLASS>
		inline	BOOL	Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetTabStops (int nTabStops, LPINT rgTabStops)
			{
				Led_Assert (false);		// though THIS code is fine - we haven't yet hooked the callback, so this call will
										// produce poor results... LGP 941129
				Led_Assert (::IsWindow (m_hWnd));
				return (BOOL)::SendMessage (m_hWnd, EM_SETTABSTOPS, nTabStops, (LPARAM)rgTabStops);
			}
	template	<typename	BASECLASS>
		inline	void	Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetTabStops ()
			{
				Led_Assert (false);		// though THIS code is fine - we haven't yet hooked the callback, so this call will
										// produce poor results... LGP 941129
				Led_Assert (::IsWindow (m_hWnd));
				Led_Verify (::SendMessage (m_hWnd, EM_SETTABSTOPS, 0, 0));
			}
	template	<typename	BASECLASS>
		inline	BOOL	Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetTabStops (const int& cxEachStop)
			{
				Led_Assert (false);		// though THIS code is fine - we haven't yet hooked the callback, so this call will
										// produce poor results... LGP 941129
				Led_Assert (::IsWindow (m_hWnd));
				return (BOOL)::SendMessage (m_hWnd, EM_SETTABSTOPS, 1, (LPARAM)(LPINT)&cxEachStop);
			}
	template	<typename	BASECLASS>
		inline	BOOL	Led_MFC_MimicMFCAPIHelper<BASECLASS>::Undo ()
			{
				Led_Assert (::IsWindow (m_hWnd));
				return (BOOL)::SendMessage (m_hWnd, EM_UNDO, 0, 0);
			}
	template	<typename	BASECLASS>
		inline	void	Led_MFC_MimicMFCAPIHelper<BASECLASS>::Clear()
			{
				Led_Assert (::IsWindow (m_hWnd));
				::SendMessage (m_hWnd, WM_CLEAR, 0, 0);
			}
	template	<typename	BASECLASS>
		inline	void	Led_MFC_MimicMFCAPIHelper<BASECLASS>::Copy ()
			{
				Led_Assert (::IsWindow (m_hWnd));
				::SendMessage (m_hWnd, WM_COPY, 0, 0);
			}
	template	<typename	BASECLASS>
		inline	void Led_MFC_MimicMFCAPIHelper<BASECLASS>::Cut ()
			{
				Led_Assert (::IsWindow (m_hWnd));
				::SendMessage (m_hWnd, WM_CUT, 0, 0);
			}
	template	<typename	BASECLASS>
		inline	void	Led_MFC_MimicMFCAPIHelper<BASECLASS>::Paste ()
			{
				Led_Assert (::IsWindow (m_hWnd));
				::SendMessage (m_hWnd, WM_PASTE, 0, 0);
			}
	template	<typename	BASECLASS>
		inline	BOOL	Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetReadOnly (BOOL bReadOnly )
			{
				Led_Assert (::IsWindow (m_hWnd));
				return (BOOL)::SendMessage (m_hWnd, EM_SETREADONLY, bReadOnly, 0L);
			}
	template	<typename	BASECLASS>
		inline	int Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetFirstVisibleLine () const
			{
				Led_Assert (::IsWindow (m_hWnd));
				return (int)::SendMessage (m_hWnd, EM_GETFIRSTVISIBLELINE, 0, 0L);
			}
	template	<typename	BASECLASS>
		inline	TCHAR	Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetPasswordChar () const
			{
				Led_Assert (false);		// though THIS code is fine - we haven't yet hooked the callback, so this call will
										// produce poor results... LGP 941129
				Led_Assert (::IsWindow (m_hWnd));
				return (TCHAR)::SendMessage (m_hWnd, EM_GETPASSWORDCHAR, 0, 0L);
			}










//	class	Led_MFC_OptionalWin32SDKMessageMimicHelper<BASECLASS>
#if		qTemplatedMemberFunctionsFailWithMFCMessageMaps
	#define	DoDeclare_Led_MFC_OptionalWin32SDKMessageMimicHelper_MessageMap(MFC_BASE_CLASS)\
		BEGIN_MESSAGE_MAP(Led_MFC_OptionalWin32SDKMessageMimicHelper, MFC_BASE_CLASS, Led_MFC_OptionalWin32SDKMessageMimicHelper<MFC_BASE_CLASS >::inherited)\
			ON_MESSAGE		(WM_SETTEXT,					OnMFCSDKMessageDispatcher_##WM_SETTEXT)\
			ON_MESSAGE		(WM_GETTEXT,					OnMFCSDKMessageDispatcher_##WM_GETTEXT)\
			ON_MESSAGE		(WM_GETTEXTLENGTH,				OnMFCSDKMessageDispatcher_##WM_GETTEXTLENGTH)\
			ON_MESSAGE		(EM_GETSEL,						OnMFCSDKMessageDispatcher_##EM_GETSEL)\
			ON_MESSAGE		(EM_SETREADONLY,				OnMFCSDKMessageDispatcher_##EM_SETREADONLY)\
			ON_MESSAGE		(EM_GETFIRSTVISIBLELINE,		OnMFCSDKMessageDispatcher_##EM_GETFIRSTVISIBLELINE)\
			ON_MESSAGE		(EM_LINEINDEX,					OnMFCSDKMessageDispatcher_##EM_LINEINDEX)\
			ON_MESSAGE		(EM_GETLINECOUNT,				OnMFCSDKMessageDispatcher_##EM_GETLINECOUNT)\
			ON_MESSAGE		(EM_CANUNDO,					OnMFCSDKMessageDispatcher_##EM_CANUNDO)\
			ON_MESSAGE		(EM_UNDO,						OnMFCSDKMessageDispatcher_##EM_UNDO)\
			ON_MESSAGE		(EM_EMPTYUNDOBUFFER,			OnMFCSDKMessageDispatcher_##EM_EMPTYUNDOBUFFER)\
			ON_MESSAGE		(WM_CLEAR,						OnMFCSDKMessageDispatcher_##WM_CLEAR)\
			ON_MESSAGE		(WM_CUT,						OnMFCSDKMessageDispatcher_##WM_CUT)\
			ON_MESSAGE		(WM_COPY,						OnMFCSDKMessageDispatcher_##WM_COPY)\
			ON_MESSAGE		(WM_PASTE,						OnMFCSDKMessageDispatcher_##WM_PASTE)\
			ON_MESSAGE		(EM_LINEFROMCHAR,				OnMFCSDKMessageDispatcher_##EM_LINEFROMCHAR)\
			ON_MESSAGE		(EM_LINELENGTH,					OnMFCSDKMessageDispatcher_##EM_LINELENGTH)\
			ON_MESSAGE		(EM_LINESCROLL,					OnMFCSDKMessageDispatcher_##EM_LINESCROLL)\
			ON_MESSAGE		(EM_REPLACESEL,					OnMFCSDKMessageDispatcher_##EM_REPLACESEL)\
			ON_MESSAGE		(EM_SETSEL,						OnMFCSDKMessageDispatcher_##EM_SETSEL)\
			ON_MESSAGE		(EM_SCROLLCARET,				OnMFCSDKMessageDispatcher_##EM_SCROLLCARET)\
			ON_MESSAGE		(WM_GETFONT,					OnMFCSDKMessageDispatcher_##WM_GETFONT)\
			ON_MESSAGE		(WM_SETFONT,					OnMFCSDKMessageDispatcher_##WM_SETFONT)\
		END_MESSAGE_MAP()
#else
	#define	DoDeclare_Led_MFC_OptionalWin32SDKMessageMimicHelper_MessageMap(MFC_BASE_CLASS)\
		BEGIN_TEMPLATE_MESSAGE_MAP(Led_MFC_OptionalWin32SDKMessageMimicHelper, MFC_BASE_CLASS, Led_MFC_OptionalWin32SDKMessageMimicHelper<MFC_BASE_CLASS >::inherited)\
			ON_MESSAGE		(WM_SETTEXT,					OnMFCSDKMessageDispatcher<WM_SETTEXT>)\
			ON_MESSAGE		(WM_GETTEXT,					OnMFCSDKMessageDispatcher<WM_GETTEXT>)\
			ON_MESSAGE		(WM_GETTEXTLENGTH,				OnMFCSDKMessageDispatcher<WM_GETTEXTLENGTH>)\
			ON_MESSAGE		(EM_GETSEL,						OnMFCSDKMessageDispatcher<EM_GETSEL>)\
			ON_MESSAGE		(EM_SETREADONLY,				OnMFCSDKMessageDispatcher<EM_SETREADONLY>)\
			ON_MESSAGE		(EM_GETFIRSTVISIBLELINE,		OnMFCSDKMessageDispatcher<EM_GETFIRSTVISIBLELINE>)\
			ON_MESSAGE		(EM_LINEINDEX,					OnMFCSDKMessageDispatcher<EM_LINEINDEX>)\
			ON_MESSAGE		(EM_GETLINECOUNT,				OnMFCSDKMessageDispatcher<EM_GETLINECOUNT>)\
			ON_MESSAGE		(EM_CANUNDO,					OnMFCSDKMessageDispatcher<EM_CANUNDO>)\
			ON_MESSAGE		(EM_UNDO,						OnMFCSDKMessageDispatcher<EM_UNDO>)\
			ON_MESSAGE		(EM_EMPTYUNDOBUFFER,			OnMFCSDKMessageDispatcher<EM_EMPTYUNDOBUFFER>)\
			ON_MESSAGE		(WM_CLEAR,						OnMFCSDKMessageDispatcher<WM_CLEAR>)\
			ON_MESSAGE		(WM_CUT,						OnMFCSDKMessageDispatcher<WM_CUT>)\
			ON_MESSAGE		(WM_COPY,						OnMFCSDKMessageDispatcher<WM_COPY>)\
			ON_MESSAGE		(WM_PASTE,						OnMFCSDKMessageDispatcher<WM_PASTE>)\
			ON_MESSAGE		(EM_LINEFROMCHAR,				OnMFCSDKMessageDispatcher<EM_LINEFROMCHAR>)\
			ON_MESSAGE		(EM_LINELENGTH,					OnMFCSDKMessageDispatcher<EM_LINELENGTH>)\
			ON_MESSAGE		(EM_LINESCROLL,					OnMFCSDKMessageDispatcher<EM_LINESCROLL>)\
			ON_MESSAGE		(EM_REPLACESEL,					OnMFCSDKMessageDispatcher<EM_REPLACESEL>)\
			ON_MESSAGE		(EM_SETSEL,						OnMFCSDKMessageDispatcher<EM_SETSEL>)\
			ON_MESSAGE		(EM_SCROLLCARET,				OnMFCSDKMessageDispatcher<EM_SCROLLCARET>)\
			ON_MESSAGE		(WM_GETFONT,					OnMFCSDKMessageDispatcher<WM_GETFONT>)\
			ON_MESSAGE		(WM_SETFONT,					OnMFCSDKMessageDispatcher<WM_SETFONT>)\
		END_MESSAGE_MAP()
#endif
	template	<typename	BASECLASS>
		Led_MFC_OptionalWin32SDKMessageMimicHelper<BASECLASS>::Led_MFC_OptionalWin32SDKMessageMimicHelper ():
			inherited ()
			{
			}








//	class	Led_MFC_DragAndDropWindow<BASECLASS>
	#define	DoDeclare_Led_MFC_DragAndDropWindow_MessageMap(MFC_BASE_CLASS)\
		BEGIN_TEMPLATE_MESSAGE_MAP(Led_MFC_DragAndDropWindow, MFC_BASE_CLASS, Led_MFC_DragAndDropWindow<MFC_BASE_CLASS >::inherited)\
			ON_WM_CREATE	()\
			ON_WM_TIMER		()\
		END_MESSAGE_MAP()

	template	<typename	BASECLASS>
		typename	Led_MFC_DragAndDropWindow<BASECLASS>::LedStartDragAndDropContext*	Led_MFC_DragAndDropWindow<BASECLASS>::sCurrentDragInfo	=	NULL;
	template	<typename	BASECLASS>
		Led_MFC_DragAndDropWindow<BASECLASS>::Led_MFC_DragAndDropWindow ():
			inherited (),
			fDropTarget (),
			fDragPoint (),
			fDragSize (),
			fDragOffset (),
			fPrevDropEffect (DROPEFFECT_NONE),
			fDragAutoScrollTimerID (0)
			{
			}
	template	<typename	BASECLASS>
		Led_MFC_DragAndDropWindow<BASECLASS>::~Led_MFC_DragAndDropWindow ()
			{
				Led_Assert (fDragAutoScrollTimerID == 0);
			}
	template	<typename	BASECLASS>
		/*
		@METHOD:		Led_MFC_DragAndDropWindow<BASECLASS>::GetCommandNames
		@DESCRIPTION:	Returns command name for each of the user-visible commands produced by this module.
			This name is used used in the constructed Undo command name, as
			in, "Undo Drag and Drop". You can replace this name with whatever you like.You change this value with
			WordProcessor::SetCommandNames.
				<p> The point of this is to allow for different UI-language localizations,
					without having to change Led itself.
				<p>See also @'Led_MFC_DragAndDropWindow<BASECLASS>::CommandNames'.
		*/
		inline	typename const Led_MFC_DragAndDropWindow<BASECLASS>::CommandNames&	Led_MFC_DragAndDropWindow<BASECLASS>::GetCommandNames ()
			{
				return sCommandNames;
			}
	template	<typename	BASECLASS>
		/*
		@METHOD:		Led_MFC_DragAndDropWindow<BASECLASS>::SetCommandNames
		@DESCRIPTION:	See @'Led_MFC_DragAndDropWindow<BASECLASS>::GetCommandNames'.
		*/
		inline	void	Led_MFC_DragAndDropWindow<BASECLASS>::SetCommandNames (const typename Led_MFC_DragAndDropWindow<BASECLASS>::CommandNames& cmdNames)
			{
				sCommandNames = cmdNames;
			}
	template	<typename	BASECLASS>
		typename Led_MFC_DragAndDropWindow<BASECLASS>::CommandNames	Led_MFC_DragAndDropWindow<BASECLASS>::sCommandNames;
	template	<typename	BASECLASS>
		int	Led_MFC_DragAndDropWindow<BASECLASS>::OnCreate (LPCREATESTRUCT lpCreateStruct)
			{
				if (inherited::OnCreate (lpCreateStruct) != 0) {
					return -1;	// failed to create
				}
				if (not fDropTarget.Register (this)) {
					/*
					 *	This can happen if the document associated with the view is not a COleDocument, for
					 *	example. Shouldn't prevent Led view from being used. D&D just won't work...
					 */
					TRACE (_T ("Warning: failed to register window as drop target\n"));
					return 0;	// treat this as OK - but warn above...
				}
				return 0;	// success
			}
	template	<typename	BASECLASS>
		bool	Led_MFC_DragAndDropWindow<BASECLASS>::IsADragSelect (Led_Point clickedWhere) const
			{
				/*
				 *	This seems to be what most people do. But I think this algorithm should
				 *	be embellished, so we detect that the mouse is down for a while (a half second?)
				 *	and then melt the drag into a regular selection. Also, pay attention to keys
				 *	pressed etc as a hint of intent. But immediately return yes if we are moving
				 *	what we've selected.
				 *
				 *	For now though, just do it the simple way...
				 */
				Led_Region	r;
				GetSelectionWindowRegion (&r, GetSelectionStart (), GetSelectionEnd ());
				return r.PtInRegion (AsCPoint (clickedWhere));
			}
	template	<typename	BASECLASS>
		void	Led_MFC_DragAndDropWindow<BASECLASS>::HandleDragSelect (UINT /*nFlags*/, CPoint oPoint)
			{
				const float	kTimeEnoughToRestoreSelection	=	0.3f;

				if (GetStyle () & WS_DISABLED) {
					return;
				}
				if (GetFocus () != this)	{
					SetFocus ();
				}

				/*
				 *	Be sure to reset these so on a double click after what looked like a single
				 *	click we don't grab the old anchor (see spr#0438).
				 */
				fMouseTrackingLastPoint = AsLedPoint (oPoint);
				fDragAnchor = GetCharAtClickLocation (fMouseTrackingLastPoint);

				Led_Assert (sCurrentDragInfo == NULL);
				sCurrentDragInfo = new LedStartDragAndDropContext (this);

				sCurrentDragInfo->fOurDragStart = GetSelectionStart ();
				sCurrentDragInfo->fOurDragEnd = GetSelectionEnd ();

				try {
					COleDataSource		dataSrc;
					Led_MFCWriterDAndDFlavorPackage	flavorPackage (&dataSrc);
					ExternalizeFlavors (flavorPackage);

					Led_Region	selectionRegion;
					GetSelectionWindowRegion (&selectionRegion, GetSelectionStart (), GetSelectionEnd ());

					float	startDragSelectAt	=	Led_GetTickCount ();	// Grab it after the ExternalizeFlavors call in case thats slow (SPR#1498).
					DROPEFFECT	dropResult	=	DROPEFFECT_COPY;
					if (not (GetStyle () & ES_READONLY)) {
						// Assure we don't change read-only text.
						dropResult |= DROPEFFECT_MOVE;
					}
					dropResult	=	dataSrc.DoDragDrop (dropResult);
					#if		qDebug
						{
							if (GetStyle () & ES_READONLY) {
								Led_Assert (not (dropResult & DROPEFFECT_MOVE));
							}
						}
					#endif
					switch (dropResult) {
						case	DROPEFFECT_NONE: {
							Led_AssertNotNil (sCurrentDragInfo);
							if (Led_GetTickCount () - startDragSelectAt > kTimeEnoughToRestoreSelection) {
								/*
								*	SPR#1374 - on aborted drag - restore the selection to its original value, rather
								*	than setting it to an empty selection where the user had clicked.
								*/
								SetSelection (sCurrentDragInfo->fOurDragStart, sCurrentDragInfo->fOurDragEnd);
							}
							else {
								// for a very short click - just treat it as a regular click - changing the
								// selection to an insertion point where the user clicked.
								size_t	ignored	=	0;
								(void)ProcessSimpleClick (fMouseTrackingLastPoint, 1, false, &ignored);
							}
						}
						break;
						case	DROPEFFECT_MOVE: {
							/*
							 *	delete the original text on a MOVE
							 */
							InteractiveModeUpdater	iuMode (*this);
							Led_AssertNotNil (sCurrentDragInfo);
							UndoableContextHelper	undoContext (*this,
																sCurrentDragInfo->fWeRecievedDrop? Led_SDK_String (): GetCommandNames ().fDragCommandName,
																sCurrentDragInfo->fOrigSelection.GetStart (),
																sCurrentDragInfo->fOrigSelection.GetEnd (),
																true
															);
								{
									TextInteractor::InteractiveReplace_ (undoContext.GetUndoRegionStart (), undoContext.GetUndoRegionEnd (), LED_TCHAR_OF (""), 0, false, false, eDefaultUpdate);
								}
							undoContext.CommandComplete (sCurrentDragInfo->fOrigSelection.GetEnd ());
						}
						break;
						default: {
							// we do nothing then - I think....
						}
					}

					// must do here, even if NOT a MOVE - cuz we skipped this in the OnDrop code...
					BreakInGroupedCommands ();
				}
				catch (...) {
					delete sCurrentDragInfo; sCurrentDragInfo = NULL;
					throw;
				}

				delete sCurrentDragInfo; sCurrentDragInfo = NULL;
			}
	template	<typename	BASECLASS>
		void	Led_MFC_DragAndDropWindow<BASECLASS>::HandleSharedDragOverUpdateSelCode ()
			{
				CClientDC	dc (this);
				if (fPrevDropEffect != DROPEFFECT_NONE) {
					// erase previous focus rect
					dc.DrawFocusRect (CRect (fDragPoint, fDragSize));
				}

				size_t	newPos		=	GetCharAtClickLocation (AsLedPoint (fDragPoint));

				// Auto-scroll if necessary...
				if (newPos < GetMarkerPositionOfStartOfWindow ()) {
					ScrollByIfRoom (-1, eImmediateUpdate);
				}
				else if (newPos > GetMarkerPositionOfEndOfWindow ()) {
					ScrollByIfRoom (1, eImmediateUpdate);
				}

				if (fPrevDropEffect != DROPEFFECT_NONE) {
					// draw new focus rect
					dc.DrawFocusRect (CRect (fDragPoint, fDragSize));
				}
			}
	template	<typename	BASECLASS>
		DROPEFFECT	Led_MFC_DragAndDropWindow<BASECLASS>::HelperDragEnter (COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
			{
				// We allow dropping any time - I think... - See page 444 of MSVC4.0 Visual C++ Tutorial for hints on how to check if we accept...
				StartDragAutoscrollTimer ();
				if (not GetObjectInfo (pDataObject, &fDragSize, &fDragOffset)) {
					return DROPEFFECT_NONE;
				}
				CClientDC	dc (NULL);
				dc.HIMETRICtoDP (&fDragSize);
				dc.HIMETRICtoDP (&fDragOffset);
				DROPEFFECT	result	=	HelperDragOver (pDataObject, dwKeyState, point);
				return result;
			}
	template	<typename	BASECLASS>
		DROPEFFECT	Led_MFC_DragAndDropWindow<BASECLASS>::HelperDragOver (COleDataObject* /*pDataObject*/, DWORD dwKeyState, CPoint point)
			{
				point -= fDragOffset;	//	adjust target rect by cursor offset

				// See  See page 444 of MSVC4.0 Visual C++ Tutorial for hints on how to check if we accept...

				// now do draw of hilite...
				DROPEFFECT	de	=	DROPEFFECT_NONE;

				if ((dwKeyState & (MK_CONTROL|MK_SHIFT)) == (MK_CONTROL|MK_SHIFT)) {
					de = DROPEFFECT_NONE;	// we don't support linking...
				}
				else if ((dwKeyState & MK_CONTROL) == MK_CONTROL) {
					de = DROPEFFECT_COPY;
				}
				else if ((dwKeyState & MK_ALT) == MK_ALT) {
					de = DROPEFFECT_MOVE;
				}
				else {
					de = DROPEFFECT_MOVE;	// an unfortunate choice, but windows defaults to this apparently...
				}

				if (point != fDragPoint) {
					// cursor has moved
					CClientDC	dc (this);
					if (fPrevDropEffect != DROPEFFECT_NONE) {
						// erase previous focus rect
						dc.DrawFocusRect (CRect (fDragPoint, fDragSize));
					}

					size_t	newPos		=	GetCharAtClickLocation (AsLedPoint (point));

					// Disalow dragging selection ontop of itself...
					if (sCurrentDragInfo != NULL and sCurrentDragInfo->fLedMFCView == this and (newPos >= sCurrentDragInfo->fOurDragStart and newPos <= sCurrentDragInfo->fOurDragEnd)) {
						SetSelection (sCurrentDragInfo->fOurDragStart, sCurrentDragInfo->fOurDragEnd);
						Update ();
						fPrevDropEffect = DROPEFFECT_NONE;
						return DROPEFFECT_NONE;
					}
				
					// either scroll, or adjust the selection...
					if (newPos < GetMarkerPositionOfStartOfWindow ()) {
						ScrollByIfRoom (-1, eImmediateUpdate);
					}
					else if (newPos > GetMarkerPositionOfEndOfWindow ()) {
						ScrollByIfRoom (1, eImmediateUpdate);
					}
				
					if (newPos >= GetMarkerPositionOfStartOfWindow () and
						newPos <= GetMarkerPositionOfEndOfWindow ()
						) {
						SetSelection (newPos, newPos);
						Update ();
					}

					fPrevDropEffect = de;
					if (fPrevDropEffect != DROPEFFECT_NONE) {
						// draw new focus rect
						fDragPoint = point;
						dc.DrawFocusRect (CRect (fDragPoint, fDragSize));
					}
				}

				return de;
			}
	template	<typename	BASECLASS>
		BOOL	Led_MFC_DragAndDropWindow<BASECLASS>::HelperDrop (COleDataObject* pDataObject, DROPEFFECT /*dropEffect*/, CPoint /*point*/)
			{
				Led_RequireNotNil (pDataObject);

				// Cleanup focus rect...
				HelperDragLeave ();

				if (sCurrentDragInfo != NULL and sCurrentDragInfo->fLedMFCView == this) {
					sCurrentDragInfo->fWeRecievedDrop = true;
				}

				#if		0
					// debugging hack- leave in for now in case helpful to see whats in drop package...
					// LGP 960502
					{
						pDataObject->BeginEnumFormats ();
						FORMATETC	formatC;
						while (pDataObject->GetNextFormat (&formatC)) {
							char	buf[1024];
							int		nChars	=	::GetClipboardFormatName (formatC.cfFormat, buf, sizeof buf);
							int		barf	=	1;	// set breakpoint here
						}
					}
				#endif

				CWaitCursor	busy;

				Led_MFCReaderDAndDFlavorPackage	flavors (pDataObject);

				bool			doSmartCNP	=	GetSmartCutAndPasteMode () and flavors.GetFlavorAvailable_TEXT ();
				SmartCNPInfo	smartCNPInfo;
				if (doSmartCNP) {
					size_t			length		=	flavors.GetFlavorSize (kTEXTClipFormat);
					Led_ClipFormat	textFormat	=	kTEXTClipFormat;
					#if		qWideCharacters && qWorkAroundWin95BrokenUNICODESupport
					if (length == 0) {
						textFormat = CF_TEXT;
						length = flavors.GetFlavorSize (textFormat);
					}
					#endif
					Led_SmallStackBuffer<Led_tChar> buf (length);	// really could use smaller buffer
					length = flavors.ReadFlavorData (textFormat, length, buf);
					if (doSmartCNP) {
						#if		qWideCharacters && qWorkAroundWin95BrokenUNICODESupport
							if (textFormat != kTEXTClipFormat) {
								// then we must manually convert the clipboard text to UNICODE
								string	tmp			=	string ((const char*)(const void*)buf, length);
								length	= ::MultiByteToWideChar (CP_ACP, 0, tmp.c_str (), tmp.length (), buf, length) * sizeof (Led_tChar);
							}
						#endif
						size_t	nTChars	=	length / sizeof (Led_tChar);
						if (nTChars > 0) {
							nTChars--;	// on windows, the text buffer contains a trailing NUL-byte
						}
						doSmartCNP = LooksLikeSmartPastableText (buf, nTChars, &smartCNPInfo);
					}
				}

				BreakInGroupedCommands ();

				InteractiveModeUpdater	iuMode (*this);
				size_t					ucSelStart	=	(sCurrentDragInfo != NULL and sCurrentDragInfo->fLedMFCView == this)?
															sCurrentDragInfo->fOrigSelection.GetStart (): GetSelectionStart ()
														;
				size_t					ucSelEnd	=	(sCurrentDragInfo != NULL and sCurrentDragInfo->fLedMFCView == this)?
															sCurrentDragInfo->fOrigSelection.GetEnd (): GetSelectionEnd ()
														;
				UndoableContextHelper	undoContext (
														*this, 
														(sCurrentDragInfo != NULL and sCurrentDragInfo->fLedMFCView == this)? GetCommandNames ().fDragNDropCommandName: GetCommandNames ().fDropCommandName,
														GetSelectionStart (), GetSelectionEnd (),
														ucSelStart, ucSelEnd,
														false
													);
					{
						try {
							InternalizeBestFlavor (flavors);
						}
						catch (...) {
							#if		_MFC_VER > 0x0710
								// Known broken in 0x0421 and 0x0600 anx 0x700 (MSVC.Net), and 0x710 (MSVC.NET 2003)
								#pragma message "See if MFC has fixed this bug yet"
							#endif
							// Cannot re-throw here, due to MFC bug in COleDropTarget::XDropTarget::Drop (), where
							// it fails to do a RELEASE() call in case we throw here. Returning FALSE still signifies the drop fialed, but
							// then the calling APP may not see a good message about what failed. Too bad.
							return false;
							throw;
						}
						if (doSmartCNP) {
							OptionallyAddExtraSpaceForSmartCutAndPasteModeAdds (undoContext.GetUndoRegionStart (), smartCNPInfo);
						}
					}
				undoContext.CommandComplete ();

				// If WE INITED DRAG - don't do break here, do it in HandleDragSelect () so it can incorporate a DELETE TEXT on MOVE...
				if (sCurrentDragInfo == NULL or sCurrentDragInfo->fLedMFCView != this) {
					BreakInGroupedCommands ();
				}

				return true;
			}
	template	<typename	BASECLASS>
		void	Led_MFC_DragAndDropWindow<BASECLASS>::HelperDragLeave ()
			{
				StopDragAutoscrollTimer ();
				if (fPrevDropEffect != DROPEFFECT_NONE) {
					CClientDC	dc (this);
					// erase previous focus rect
					dc.DrawFocusRect (CRect (fDragPoint, fDragSize));
					fPrevDropEffect = DROPEFFECT_NONE;
				}
			}
	template	<typename	BASECLASS>
		bool	Led_MFC_DragAndDropWindow<BASECLASS>::GetObjectInfo (COleDataObject* pDataObject, CSize* pSize, CSize* pOffset)
			{
				Led_RequireNotNil (pDataObject);
				Led_RequireNotNil (pSize);
				Led_RequireNotNil (pOffset);

				static	CLIPFORMAT	kObjectDescriptorFormatTag	=	(CLIPFORMAT)::RegisterClipboardFormat (_T("Object Descriptor"));

				HGLOBAL	hObjDesc	=	pDataObject->GetGlobalData (kObjectDescriptorFormatTag);
				if (hObjDesc == NULL) {
					*pSize = CSize (0, 0);
					*pOffset = CSize (0, 0);
					return false;
				}
				else {
					LPOBJECTDESCRIPTOR	pObjDesc	=	(LPOBJECTDESCRIPTOR)::GlobalLock (hObjDesc);
					Led_AssertNotNil (pObjDesc);
					pSize->cx = (int)pObjDesc->sizel.cx;
					pSize->cy = (int)pObjDesc->sizel.cy;
					pOffset->cx = (int)pObjDesc->pointl.x;
					pOffset->cy = (int)pObjDesc->pointl.y;
					::GlobalUnlock (hObjDesc);
					::GlobalFree (hObjDesc);
					return true;
				}
			}
	template	<typename	BASECLASS>
		void	Led_MFC_DragAndDropWindow<BASECLASS>::StartDragAutoscrollTimer ()
			{
				Led_Assert (fDragAutoScrollTimerID == 0);// not sure about this - just for debug sake??? - LGP 960530
				if (fDragAutoScrollTimerID == 0) {
					//const	int	kTimeout	=	25;	// 25 milliseconds - update autoscroll every 1/40
					const	int	kTimeout	=	20;	// 20 milliseconds - update autoscroll every 1/50
													// second.
					Led_Verify (fDragAutoScrollTimerID = SetTimer (eAutoscrolling4DragTimerEventID, kTimeout, NULL));
				}
			}
	template	<typename	BASECLASS>
		void	Led_MFC_DragAndDropWindow<BASECLASS>::StopDragAutoscrollTimer ()
			{
				if (fDragAutoScrollTimerID != 0) {
					Led_Verify (KillTimer (eAutoscrolling4DragTimerEventID));
					fDragAutoScrollTimerID = 0;
				}
			}
	template	<typename	BASECLASS>
		void	Led_MFC_DragAndDropWindow<BASECLASS>::OnTimer (UINT nEventID)
			{
				if (nEventID == eAutoscrolling4DragTimerEventID) {
					HandleSharedDragOverUpdateSelCode ();
				}
				else {
					inherited::OnTimer (nEventID);
				}
			}






//	class	Led_MFC_CViewHelper<BASECLASS>
#define	DoDeclare_Led_MFC_CViewHelper_MessageMap(MFC_BASE_CLASS)\
	BEGIN_TEMPLATE_MESSAGE_MAP(Led_MFC_CViewHelper, MFC_BASE_CLASS, Led_MFC_CViewHelper<MFC_BASE_CLASS >::inherited)\
		ON_WM_PAINT()\
		ON_WM_LBUTTONDOWN()\
		ON_WM_CREATE	()\
		ON_WM_VSCROLL	()\
		ON_WM_HSCROLL	()\
	END_MESSAGE_MAP()
	template	<typename	BASECLASS>
		Led_MFC_CViewHelper<BASECLASS>::Led_MFC_CViewHelper ():
			inherited (),
			// NB: CTOR for Led_TWIPS_Rect is TOP/LEFT/HEIGHT/WIDTH, so we set height/width to zero to get same TLBR.
			#if		qTypedefOfTemplatedTypeCannotBeUsedAsCTORBug
				fPrintMargins (Rect_Base<Led_TWIPS_Point,Led_TWIPS_Point> (Led_TWIPS (1440), Led_TWIPS (1440), Led_TWIPS (0), Led_TWIPS (0))),
			#else
				fPrintMargins (Led_TWIPS_Rect (Led_TWIPS (1440), Led_TWIPS (1440), Led_TWIPS (0), Led_TWIPS (0))),
			#endif
			fPrintInfo (NULL)
			{
			}
	template	<typename	BASECLASS>
		Led_MFC_CViewHelper<BASECLASS>::~Led_MFC_CViewHelper ()
			{
				Led_Assert (fPrintInfo == NULL);
			}
	template	<typename	BASECLASS>
		/*
		@METHOD:		Led_MFC_CViewHelper<BASECLASS>::GetPrintMargins
		@DESCRIPTION:	<p>Return the print margins. These are used by @'Led_MFC_CViewHelper<BASECLASS>::CalculatePrintingRect'.
					See also @'Led_MFC_CViewHelper<BASECLASS>::SetPrintMargins'.
		*/
		Led_TWIPS_Rect	Led_MFC_CViewHelper<BASECLASS>::GetPrintMargins () const
			{
				return fPrintMargins;
			}
	template	<typename	BASECLASS>
		/*
		@METHOD:		Led_MFC_CViewHelper<BASECLASS>::SetPrintMargins
		@DESCRIPTION:	<p>See also @'Led_MFC_CViewHelper<BASECLASS>::GetPrintMargins'.
		*/
		void	Led_MFC_CViewHelper<BASECLASS>::SetPrintMargins (const Led_TWIPS_Rect& printMargins)
			{
				fPrintMargins = printMargins;
			}
	template	<typename	BASECLASS>
		/*
		@METHOD:		Led_MFC_CViewHelper<BASECLASS>::OnPreparePrinting
		@DESCRIPTION:	Hook the MFC OnPreparePrinting () method to handle printing in the standard MFC fasion.
			 Don't call this directly.
		*/
		BOOL	Led_MFC_CViewHelper<BASECLASS>::OnPreparePrinting (CPrintInfo* pInfo)
			{
				// default preparation
				return DoPreparePrinting (pInfo);
			}
	template	<typename	BASECLASS>
		/*
		@METHOD:		Led_MFC_CViewHelper<BASECLASS>::OnBeginPrinting
		@DESCRIPTION:	Hook the MFC OnBeginPrinting () method to handle printing in the standard MFC fasion.
			Also, keep track of some internals we will use later in printing, and setup SetForceAllRowsShowing
			so when we print the last page, we can see lots of nice whatspace at the end. Don't call this directly.
		*/
		void	Led_MFC_CViewHelper<BASECLASS>::OnBeginPrinting (CDC* pDC, CPrintInfo* pInfo)
			{
				// THIS CODE IS KINDOF HACKISH - SHOULD HAVE A PAGENATE STAGE/FUNCTION!!! TO FILL THIS ARRAY!!!
				Led_Assert (fPrintInfo == NULL);
				BASECLASS::OnBeginPrinting (pDC, pInfo);
				fPrintInfo = new PrintInfo (*this, pDC, GetWindowRect (), GetMarkerPositionOfStartOfWindow (), GetForceAllRowsShowing ());
				SetForceAllRowsShowing (false);
			}
	template	<typename	BASECLASS>
		void	Led_MFC_CViewHelper<BASECLASS>::OnPrint (CDC* pDC, CPrintInfo* /*pInfo*/)
			{
				Led_AssertNotNil (fPrintInfo);
			// Consider different print loop - maybe differnt print-oriented entry in TextImager?
			// That doesn't have todo with scrolling? Just DrawFromSize_tOffset? Maybe - into rect.
			// Make the MRTI::Draw() vector to that?
				ASSERT_VALID (pDC);
				Led_MFC_TabletFromCDC	tablet (pDC);
				TemporarilyUseTablet	tmpUseTablet (*this, tablet, TemporarilyUseTablet::eDontDoTextMetricsChangedCall);
				CRect	boundsRect=	CRect (0, 0, 0, 0);
				Led_Verify (pDC->GetClipBox (&boundsRect) != ERROR);
				Draw (AsLedRect (boundsRect), true);
			}
	template	<typename	BASECLASS>
		void	Led_MFC_CViewHelper<BASECLASS>::OnEndPrinting (CDC* pDC, CPrintInfo* pInfo)
			{
				if (fPrintInfo == NULL) {
					// Can be NULL if print fails, cuz no printer installed
					BASECLASS::OnEndPrinting (pDC, pInfo);
				}
				else {
					SetForceAllRowsShowing (fPrintInfo->fSavedForceAllRowsFlag);
					SetWindowRect (fPrintInfo->fOldWindowRect);
					SetTopRowInWindowByMarkerPosition (fPrintInfo->fSavedScrollPos);
					delete fPrintInfo; fPrintInfo = NULL;
					BASECLASS::OnEndPrinting (pDC, pInfo);
				}
			}
	template	<typename	BASECLASS>
		void	Led_MFC_CViewHelper<BASECLASS>::OnPrepareDC (CDC* pDC, CPrintInfo* pInfo)
			{
				if (pInfo != NULL and pDC->IsPrinting ()) {	// For some STUPID reason, MFC calls this even if we aren't printing...
					{
						Led_WindowDC	screenDC (NULL);
						(void)pDC->SetMapMode (MM_ANISOTROPIC);
						(void)pDC->SetWindowExt (screenDC.GetDeviceCaps (LOGPIXELSX), screenDC.GetDeviceCaps (LOGPIXELSY));
						(void)pDC->SetViewportExt (pDC->GetDeviceCaps (LOGPIXELSX), pDC->GetDeviceCaps (LOGPIXELSY));
						// I REMOVED calls to SetWindowRect/TabletChangedMetrics for WYSYWIG (SPR#0869),
						// This created a new problem - SPR#1079 - which is now fixed (or largely worked around here).
						// Probably there is still a real latent problem inside TabletChangedMetrics - but I dont
						// need to debug that now, and this whole thing will go away if I rewrite the printing code
						// after 3.0 - as is anticipated.
						// I changed the args to CalculateWindowRect() to accomodate this bugfix.
						//
						Led_Rect	oldWR	=	GetWindowRect ();
						Led_Rect	newWR	=	CalculatePrintingRect (pDC);
						SetWindowRect (newWR);
					}
					CView::OnPrepareDC (pDC, pInfo);
					if (fPrintInfo->fWindowStarts.size () == 0) {
						fPrintInfo->fWindowStarts.push_back (0);
						SetTopRowInWindowByMarkerPosition (0, eNoUpdate);
					}
					pInfo->m_bContinuePrinting = true;	// Assume scroll forward succeeded
					// First page into our fWindowStarts cache enuf pages to satisfy this 'goto page' request. NB:
					// In Print (page n..m) requests - this can be starting well past fWindowStarts.size () - contrary
					// to earlier - incorrect - Led comments - LGP 981007
					while (fPrintInfo->fWindowStarts.size () < pInfo->m_nCurPage) {
						// advanced one page...
						if (GetMarkerPositionOfEndOfWindow () == GetTextStore ().GetEnd ()) {
							pInfo->m_bContinuePrinting = false;	// scroll forward failed
							return;
						}
						SetTopRowInWindowByMarkerPosition (FindNextCharacter (GetMarkerPositionOfEndOfWindow ()), eNoUpdate);
						size_t	newStart	=	GetMarkerPositionOfStartOfWindow ();
						if (newStart == fPrintInfo->fWindowStarts.back ()) {
							pInfo->m_bContinuePrinting = false;	// scroll forward failed
							return;
						}
						else {
							fPrintInfo->fWindowStarts.push_back (newStart);
						}
					}
					// scroll forward/back to some page in our (now filled in cache) page list
					SetTopRowInWindowByMarkerPosition (fPrintInfo->fWindowStarts[pInfo->m_nCurPage-1]);
					if (GetMarkerPositionOfEndOfWindow () == GetTextStore ().GetEnd ()) {
						pInfo->SetMaxPage (pInfo->m_nCurPage);
						return;
					}
				}
			}
	template	<typename	BASECLASS>
		/*
		@METHOD:		Led_MFC_CViewHelper<BASECLASS>::CalculatePrintingRect
		@DESCRIPTION:	Hook function to change the default size Led will use for printing. Defautlts to
			FULL page. Override to inset for margins, and/or headers/footers, or call @'Led_MFC_CViewHelper<BASECLASS>::SetPrintMargins'.
				<p>Don't call directly. Just called after DC setup from @'Led_MFC_CViewHelper<BASECLASS>::OnPrepareDC'.</p>
				<p>See also @'Led_MFC_CViewHelper<BASECLASS>::GetPrintMargins' to specify the margins that are used by this
			routine by default.</p>
		*/
		Led_Rect	Led_MFC_CViewHelper<BASECLASS>::CalculatePrintingRect (CDC* pDC) const
			{
				Led_RequireNotNil (pDC);
				/*
				 *	See SPR#1079, and SPR#1153 about whats going on here.
				 */
				CRect		winRect	=	CRect (0, 0, pDC->GetDeviceCaps (HORZRES), pDC->GetDeviceCaps (VERTRES));
				pDC->DPtoLP (&winRect);
				Led_Rect	useRect		=	AsLedRect (winRect);
				Led_Rect	marginRect	=	Led_MFC_TabletFromCDC (pDC)->CvtFromTWIPS (fPrintMargins);
				useRect = Led_Rect (useRect.GetTop () + marginRect.GetTop (),
									useRect.GetLeft () + marginRect.GetLeft (),
									useRect.GetHeight () - (marginRect.GetTop () + marginRect.GetBottom ()),
									useRect.GetWidth () - (marginRect.GetLeft () + marginRect.GetRight ())
									);
				return useRect;
			}
	template	<typename	BASECLASS>
		void	Led_MFC_CViewHelper<BASECLASS>::InvalidateScrollBarParameters ()
			{
				if (fPrintInfo == NULL) {
					inherited::InvalidateScrollBarParameters ();
				}
			}
	template	<typename	BASECLASS>
		/*
		@METHOD:		Led_MFC_CViewHelper<BASECLASS>::UpdateScrollBars
		@DESCRIPTION:	Avoid errors updating sbars while printing.
		*/
		void	Led_MFC_CViewHelper<BASECLASS>::UpdateScrollBars ()
			{
				if (fPrintInfo != NULL) {
					return;	// ignore while in print mode...
				}
				inherited::UpdateScrollBars ();
			}
	template	<typename	BASECLASS>
		/*
		@METHOD:		Led_MFC_CViewHelper<BASECLASS>::DeleteContents
		@DESCRIPTION:	Hook the MFC DeleteContents () routine, and simulate the user having deleted all the text in the
			buffer.
		*/
		void	Led_MFC_CViewHelper<BASECLASS>::DeleteContents ()
			{
				ASSERT_VALID (this);
				Led_AssertNotNil (m_hWnd);
				SetWindowText (NULL);
				ASSERT_VALID (this);
			}
	template	<typename	BASECLASS>
		void	Led_MFC_CViewHelper<BASECLASS>::Serialize (CArchive& ar)
			{
				// Probably not the right thing todo, but a simple enuf hack for now - LGP 950511
				SerializeRaw (ar);
			}
	template	<typename	BASECLASS>
		void	Led_MFC_CViewHelper<BASECLASS>::SerializeRaw (CArchive& ar)
			{
				ASSERT_VALID (this);
				Led_AssertNotNil (m_hWnd);
				if (ar.IsStoring()) {
					WriteToArchive (ar);
				}
				else {
					CFile*	file	=	ar.GetFile ();
					ASSERT_VALID (file);
					DWORD	dwLen = static_cast<DWORD> (file->GetLength ());				// maybe should subtract current offset?
					ReadFromArchive (ar, (UINT)dwLen);
				}
				ASSERT_VALID (this);
			}
	template	<typename	BASECLASS>
		void	Led_MFC_CViewHelper<BASECLASS>::ReadFromArchive (CArchive& ar, UINT nLen)
			{
				ASSERT_VALID (this);

				Led_SmallStackBuffer<Led_tChar>	buf (nLen);
				if (ar.Read (buf, nLen*sizeof(Led_tChar)) != nLen*sizeof(Led_tChar)) {
					AfxThrowArchiveException (CArchiveException::endOfFile);
				}
				// Replace the editing edit buffer with the newly loaded data
				nLen = Led_NormalizeTextToNL (buf, nLen, buf, nLen);
				if (ValidateTextForCharsetConformance (buf, nLen)) {
					Replace (0, 0, buf, nLen);
				}
				else {
					OnBadUserInput ();
				}

				Invalidate ();

				ASSERT_VALID (this);
			}
	template	<typename	BASECLASS>
		void	Led_MFC_CViewHelper<BASECLASS>::WriteToArchive (CArchive& ar)
			{
				ASSERT_VALID (this);

				UINT nLen = GetLength();
				Led_SmallStackBuffer<Led_tChar>	buf (nLen);
				CopyOut (0, nLen, buf);
				Led_SmallStackBuffer<Led_tChar>	buf2 (2*nLen);
				nLen = Led_NLToNative (buf, nLen, buf2, 2*nLen);
				ar.Write (buf2, nLen*sizeof(Led_tChar));

				ASSERT_VALID (this);
			}
	template	<typename	BASECLASS>
		void	Led_MFC_CViewHelper<BASECLASS>::OnLButtonDown (UINT nFlags, CPoint oPoint)
			{
				UpdateClickCount (Led_GetTickCount (), AsLedPoint (oPoint));

				if (m_pDocument != NULL and dynamic_cast<COleDocument*> (m_pDocument) != NULL) {
					// Any time we get a click, make sure there are no active in-place activations.
					// Any click anyplace outside the active item is ingored, except for deactivating
					// that item.
					Led_AssertMember (m_pDocument, COleDocument);	//
	   				COleDocument&	doc		=	*(COleDocument*)m_pDocument;
					COleClientItem*	pItem	=	doc.GetInPlaceActiveItem (this);
					if (pItem != NULL) {
						pItem->Close ();
						return;
					}
				}

				if (GetCurClickCount () == 1 and IsADragSelect (AsLedPoint (oPoint))) {
					HandleDragSelect (nFlags, oPoint);
				}
				else {
					OnNormalLButtonDown (nFlags, AsLedPoint (oPoint));
				}
			}
	template	<typename	BASECLASS>
		DROPEFFECT	Led_MFC_CViewHelper<BASECLASS>::OnDragEnter (COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
			{
				return HelperDragEnter (pDataObject, dwKeyState, point);
			}
	template	<typename	BASECLASS>
		DROPEFFECT	Led_MFC_CViewHelper<BASECLASS>::OnDragOver (COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
			{
				return HelperDragOver (pDataObject, dwKeyState, point);
			}
	template	<typename	BASECLASS>
		BOOL	Led_MFC_CViewHelper<BASECLASS>::OnDrop (COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
			{
				return HelperDrop (pDataObject, dropEffect, point);
			}
	template	<typename	BASECLASS>
		void	Led_MFC_CViewHelper<BASECLASS>::OnDragLeave ()
			{
				HelperDragLeave ();
			}
	template	<typename	BASECLASS>
		void	Led_MFC_CViewHelper<BASECLASS>::OnPaint ()
			{
				CView::OnPaint ();	// don't use inherited/Led_MFC_CViewHelper<BASECLASS>_Helper<T> version - cuz misses some MFC hooks
			}
	template	<typename	BASECLASS>
		/*
		@METHOD:		Led_MFC_CViewHelper<BASECLASS>::OnDraw
		@DESCRIPTION:	Hook the MFC OnDraw () method to invoke the Led drawing mechanism, and redisplay the window.
		*/
		void	Led_MFC_CViewHelper<BASECLASS>::OnDraw (CDC* pDC)
			{
				ASSERT_VALID (pDC);

				/*
				 *	Since we currently use the same textimager for printing as for display, we will screw
				 *	up all our cached info for line breaks etc if we display while we are printing.
				 *	If it is desired to make this work then we must use a NEW imager (as with LedIt! PowerPlant)
				 *	to print to.
				 */
				if (fPrintInfo != NULL) {
					return;
				}

				CRect		boundsRect	=	CRect (0, 0, 0, 0);
				Led_Verify (pDC->GetClipBox (&boundsRect) != ERROR);
				WindowDrawHelper (Led_MFC_TabletFromCDC (pDC), AsLedRect (boundsRect), false);
			}
	template	<typename	BASECLASS>
		int	Led_MFC_CViewHelper<BASECLASS>::OnCreate (LPCREATESTRUCT lpCreateStruct)
			{
				if (inherited::OnCreate (lpCreateStruct) != 0) {
					return -1;	// failed to create
				}
				TabletChangedMetrics ();		// maybe should catch failures here, and return -1?
				return 0;	// sucess
			}
	template	<typename	BASECLASS>
		void	Led_MFC_CViewHelper<BASECLASS>::OnVScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
			{
				if (m_pDocument != NULL and dynamic_cast<COleDocument*> (m_pDocument) != NULL) {
					// Don't allow scrolling while there is an in-place active item.
					// Wish we had a better interface for this... Maybe we should be deactivating
					// SBARS when we activate item??? For now, interpret scroll attempt as a request
					// to deactivate...
					// LGP 960530
					Led_AssertMember (m_pDocument, COleDocument);
	   				COleDocument&	doc		=	*(COleDocument*)m_pDocument;
					COleClientItem*	pItem	=	doc.GetInPlaceActiveItem (this);
					if (pItem != NULL) {
						pItem->Close ();
						return;
					}
				}
				inherited::OnVScroll (nSBCode, nPos, pScrollBar);
			}
	template	<typename	BASECLASS>
		void	Led_MFC_CViewHelper<BASECLASS>::OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
			{
				if (m_pDocument != NULL and dynamic_cast<COleDocument*> (m_pDocument) != NULL) {
					// Don't allow scrolling while there is an in-place active item.
					// Wish we had a better interface for this... Maybe we should be deactivating
					// SBARS when we activate item??? For now, interpret scroll attempt as a request
					// to deactivate...
					// LGP 960530
					Led_AssertMember (m_pDocument, COleDocument);	//
	   				COleDocument&	doc		=	*(COleDocument*)m_pDocument;
					COleClientItem*	pItem	=	doc.GetInPlaceActiveItem (this);
					if (pItem != NULL) {
						pItem->Close ();
						return;
					}
				}
				inherited::OnHScroll (nSBCode, nPos, pScrollBar);
			}







//	class	Led_MFC_ExceptionHandlerHelper<BASECLASS>
#define	DoDeclare_Led_MFC_ExceptionHandlerHelper_MessageMap(MFC_BASE_CLASS)\
	BEGIN_TEMPLATE_MESSAGE_MAP(Led_MFC_ExceptionHandlerHelper, MFC_BASE_CLASS, MFC_BASE_CLASS)\
		ON_WM_CHAR	()\
		ON_MESSAGE			(WM_IME_CHAR,	OnIMEChar)\
		ON_WM_KEYDOWN		()\
		ON_MESSAGE			(WM_PASTE,		OnMsgPaste)\
		ON_WM_LBUTTONDOWN	()\
		ON_WM_LBUTTONUP		()\
		ON_WM_LBUTTONDBLCLK	()\
	END_MESSAGE_MAP()
	template	<typename	BASECLASS>
		/*
		@METHOD:		Led_MFC_ExceptionHandlerHelper<BASECLASS>::HandleException
		@DESCRIPTION:	Override this to provide different exception handling. By default, this calls @'Led_BeepNotify'.
		*/
		void	Led_MFC_ExceptionHandlerHelper<BASECLASS>::HandleException () const
			{
				Led_BeepNotify ();
			}
	template	<typename	BASECLASS>
		void	Led_MFC_ExceptionHandlerHelper<BASECLASS>::OnChar (UINT nChar, UINT nRepCnt, UINT nFlags)
			{
				try {
					inherited::OnChar (nChar, nRepCnt, nFlags);
				}
				catch (...) {
					HandleException ();
				}
			}
	template	<typename	BASECLASS>
		LRESULT	Led_MFC_ExceptionHandlerHelper<BASECLASS>::OnIMEChar (WPARAM wParam, LPARAM lParam)
			{
				try {
					return inherited::OnIMEChar (wParam, lParam);
				}
				catch (...) {
					HandleException ();
					return 0;
				}
			}
	template	<typename	BASECLASS>
		void	Led_MFC_ExceptionHandlerHelper<BASECLASS>::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
			{
				try {
					inherited::OnKeyDown (nChar, nRepCnt, nFlags);
				}
				catch (...) {
					HandleException ();
				}
			}
	template	<typename	BASECLASS>
		LRESULT	Led_MFC_ExceptionHandlerHelper<BASECLASS>::OnMsgPaste (WPARAM wParam, LPARAM lParam)
			{
				try {
					return inherited::OnMsgPaste (wParam, lParam);
				}
				catch (...) {
					HandleException ();
					return 0;
				}
			}
	template	<typename	BASECLASS>
		void	Led_MFC_ExceptionHandlerHelper<BASECLASS>::OnLButtonDown (UINT nFlags, CPoint oPoint)
			{
				try {
					inherited::OnLButtonDown (nFlags, oPoint);
				}
				catch (...) {
					HandleException ();
				}
			}
	template	<typename	BASECLASS>
		void	Led_MFC_ExceptionHandlerHelper<BASECLASS>::OnLButtonUp (UINT nFlags, CPoint oPoint)
			{
				try {
					inherited::OnLButtonUp (nFlags, oPoint);
				}
				catch (...) {
					HandleException ();
				}
			}
	template	<typename	BASECLASS>
		void	Led_MFC_ExceptionHandlerHelper<BASECLASS>::OnLButtonDblClk (UINT nFlags, CPoint oPoint)
			{
				try {
					inherited::OnLButtonDblClk (nFlags, oPoint);
				}
				catch (...) {
					HandleException ();
				}
			}
	template	<typename	BASECLASS>
		BOOL	Led_MFC_ExceptionHandlerHelper<BASECLASS>::OnCmdMsg (UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
			{
				try {
					return inherited::OnCmdMsg (nID, nCode, pExtra, pHandlerInfo);
				}
				catch (...) {
					HandleException ();
					return 1;
				}
			}





//	class	Led_MFC_X<ChosenInteractor,LEDMFC>
	template	<typename	ChosenInteractor, typename LEDMFC>
		inline	Led_MFC_X<ChosenInteractor,LEDMFC>::Led_MFC_X ():
			ChosenInteractor (),
			LEDMFC ()
			{
			}
	/*
	@METHOD:		Led_MFC_X<ChosenInteractor,LEDMFC>::AboutToUpdateText
	@DESCRIPTION:	Override to handle mixin ambiguity.
	*/
	template	<typename	ChosenInteractor, typename LEDMFC>
		void	Led_MFC_X<ChosenInteractor,LEDMFC>::AboutToUpdateText (const MarkerOwner::UpdateInfo& updateInfo)
			{
				LEDMFC::AboutToUpdateText (updateInfo);
				ChosenInteractor::AboutToUpdateText (updateInfo);
			}
	/*
	@METHOD:		Led_MFC_X<ChosenInteractor,LEDMFC>::DidUpdateText
	@DESCRIPTION:	Override to handle mixin ambiguity.
	*/
	template	<typename	ChosenInteractor, typename LEDMFC>
		void	Led_MFC_X<ChosenInteractor,LEDMFC>::DidUpdateText (const MarkerOwner::UpdateInfo& updateInfo) throw ()
			{
				LEDMFC::DidUpdateText_ (updateInfo);
				ChosenInteractor::DidUpdateText (updateInfo);
			}










//	FUNCTION	Led_GetTextExtent ()
	/*
	@METHOD:		Led_GetTextExtent<TRIVIALWORDWRAPPEDTEXTIMAGER,SIMPLETEXTIMAGER,TEXTSTORE>
	@DESCRIPTION:	Trivial wrapper on TrivialImager<TEXTSTORE,IMAGER> etc, except it handles
		case of word wrapping as a parameter. So this is roughly a replacement for the Win32 SDK
		routine GetTextExtent() - except its implemented by Led (and so UNICODE friendly, for example).
			<p>Note - this is done as a template - rather than directly as a function - so as to avoid forcing
		people who include Led_MFC from also including all these other modules required for this. There is
		a global function version of this function (@'Led_GetTextExtent') which will be enabled/included in
		your program if you define @'qSupportDrawTextGetTextExtent'.
			<p>See also Led_DrawText<TRIVIALWORDWRAPPEDTEXTIMAGER,SIMPLETEXTIMAGER,TEXTSTORE>
	*/
	template	<typename	TRIVIALWORDWRAPPEDTEXTIMAGER, typename SIMPLETEXTIMAGER, typename TEXTSTORE>
		CSize	Led_GetTextExtent (CDC* cdc, const Led_tString& text, CRect r, bool wordWrap)
			{
				Led_RequireNotNil (cdc);
				Led_MFC_TabletFromCDC	tmpTablet (cdc);
				return AsCSize (GetTextExtent<TRIVIALWORDWRAPPEDTEXTIMAGER,SIMPLETEXTIMAGER,TEXTSTORE> (tmpTablet, text, AsLedRect (r), wordWrap));
			}

//	FUNCTION	Led_DrawText ()
	/*
	@METHOD:		Led_DrawText<TRIVIALWORDWRAPPEDTEXTIMAGER,SIMPLETEXTIMAGER,TEXTSTORE>
	@DESCRIPTION:	Trivial wrapper on TrivialImager<TEXTSTORE,IMAGER> etc, except it handles
		case of word wrapping as a parameter. So this is roughly a replacement for the Win32 SDK
		routine DrawText () - except its implemented by Led (and so UNICODE friendly, for example).
			<p>Note - this is done as a template - rather than directly as a function - so as to avoid forcing
		people who include Led_MFC from also including all these other modules required for this. There is
		a global function version of this function (@'Led_DrawText') which will be enabled/included in
		your program if you define @'qSupportDrawTextGetTextExtent'.
			<p>See also Led_GetTextExtent<TRIVIALWORDWRAPPEDTEXTIMAGER,SIMPLETEXTIMAGER,TEXTSTORE>
	*/
	template	<typename	TRIVIALWORDWRAPPEDTEXTIMAGER, typename SIMPLETEXTIMAGER, typename TEXTSTORE>
		void	Led_DrawText (CDC* cdc, const Led_tString& text, CRect r, bool wordWrap)
			{
				Led_RequireNotNil (cdc);
				Led_MFC_TabletFromCDC	tmpTablet (cdc);
				DrawTextBox<TRIVIALWORDWRAPPEDTEXTIMAGER,SIMPLETEXTIMAGER,TEXTSTORE> (tmpTablet, text, AsLedRect (r), wordWrap);
			}







//	class	Led_MFC_TmpCmdUpdater
	inline	Led_MFC_TmpCmdUpdater::Led_MFC_TmpCmdUpdater (CCmdUI* pCmdUI):
			fCmdUI (pCmdUI),
			fCmdNum (MFC_CommandNumberMapping::Get ().Lookup (pCmdUI->m_nID)),
			fEnabled (false)
		{
			Led_RequireNotNil (fCmdUI);
		}
	inline	Led_MFC_TmpCmdUpdater::operator Led_MFC_TmpCmdUpdater* ()
		{
			return this;
		}
	inline	Led_MFC_TmpCmdUpdater::operator CCmdUI* ()
		{
			Led_EnsureNotNil (fCmdUI);
			return fCmdUI;
		}











#if		qLedUsesNamespaces
}
#endif

#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (pop)
#endif



#endif	/*__Led_MFC_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
