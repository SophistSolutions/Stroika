/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/ActiveLedIt/Sources/LedItView.cpp,v 2.88 2004/02/11 23:58:07 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedItView.cpp,v $
 *	Revision 2.88  2004/02/11 23:58:07  lewis
 *	SPR#1576: Update: added ReplaceAllInSelection functionality (except must re-layout MacOS dialog)
 *	
 *	Revision 2.87  2004/02/11 22:41:44  lewis
 *	SPR#1576: update: added 'Replace Again' command to LedIt, LedLineItMFC, ActiveLedIt
 *	
 *	Revision 2.86  2004/01/30 04:36:35  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.85  2004/01/25 23:26:58  lewis
 *	SPR#1621: problem with CHecKForUpdatesOnWeb was cuz code to map associated cmd#s returns zero when
 *	a failed mapping occurs, and we had bad association between cmds# between Led internal and LedIt/ActiveLedIt
 *	'system-dependent' values for Win32. Fix those for kFirstPrivateEmbedding_CmdID/kLastSelectedEmbedding_CmdID -
 *	and clean up first/last private range inside of it.
 *	
 *	Revision 2.84  2004/01/25 21:51:47  lewis
 *	SPR#1284: Added new PasteText command (not part of default menus - but builtin. Added example of how
 *	to use this to the HTML test scriptCommandSupportTester.html). Add extra arg to _AcceleratorTable::Add ()
 *	for position to insert. Added IndexOf () method (to COM API - for IALCommandList. Implement the
 *	IALCommandList::Remove method.
 *	
 *	Revision 2.83  2004/01/25 16:19:28  lewis
 *	SPR#1617: lose old resource-based contextmenu/accelerators, and use new COM-based accelerator mechanism
 *	(part of new COM-based command support)
 *	
 *	Revision 2.82  2004/01/23 23:11:25  lewis
 *	SPR#1617: Added preliminary (largely functional) implementation of OLE automation-based context menus.
 *	Tons of new OLE APIs. Can create context menus from scratch, or built from building blocks of existing
 *	submenus (like font menu).
 *	
 *	Revision 2.81  2003/12/31 03:50:13  lewis
 *	SPR#1602: use new OptionsFileHelper support to implement saving find strings in registry. ALso - support
 *	revised Find/Replace dialog APIs (popup list) to save that info as well)
 *	
 *	Revision 2.80  2003/06/12 17:15:46  lewis
 *	SPR#1526: Added COM-based spellchecker support to ActiveLedIt. Now you can access SpellCheck menu
 *	command if there is a COM-spellchecker associated with ActiveLedIt. New property 'SpellChecker'
 *	you can get/set to specify the spell checker. Seems to work as well as with LedIt!
 *	
 *	Revision 2.79  2003/05/30 14:26:16  lewis
 *	SPR#1517: Added find/replace dialog and OLE_FindReplace and OLE_LaunchReplaceDialog
 *	new OLE AUTOMATION methods
 *	
 *	Revision 2.78  2003/05/29 20:11:46  lewis
 *	SPR#1511: Forgot to add DLGTYPE::cvt<> call in EditTablePropertiesDialog () for
 *	ActiveLedIt_DialogSupport.
 *	
 *	Revision 2.77  2003/05/29 14:56:38  lewis
 *	SPR#1510: dont call GetClientRect in draw - but use WindowRect/GetDefaultWindowMargins
 *	since we may have been called from metafile draw or other. Also - added
 *	DrawExtraDesignModeBorder () so we don't display border in some cases where
 *	it looks ugly (not really related to SPR#1510 - but I noticed it while
 *	debugging and looking at display in MSVC.Net 2003 dialog editor). Careful
 *	about call to GetStyle (since vectors to MFC version in LedItView) - and
 *	that fails if window is NULL (as it is sometimes in MSVC .Net 2003 dialog
 *	editor)
 *	
 *	Revision 2.76  2003/05/28 16:56:20  lewis
 *	SPR#1509: Added qFunnyDisplayInDesignMode define to ActiveLedItConfig,
 *	and ifdefined (yes by default), then check for AMBIENT_DESIGN_MODE and
 *	change the drawing in LedItView::EraseBackground () slightly. Added a small
 *	watermarkdisplay saying 'design mode' and a border in the color opposite to
 *	the background. Hooked OnAmbientPropertyChange() to detect change and
 *	refresh screen.
 *	
 *	Revision 2.75  2003/05/13 20:45:23  lewis
 *	SPR#1399: progress checking on table getproperties dialog support
 *	
 *	Revision 2.74  2003/05/12 17:18:32  lewis
 *	SPR#1401: Added GetDialogSupport ().AddNewTableDialog ()
 *	
 *	Revision 2.73  2003/05/08 16:12:49  lewis
 *	SPR#1467: Added Select Menu (moving find/select all under it in context
 *	menu). Also fixed bug with cmd#s (internal# vs led# with CmdToFontName()
 *	
 *	Revision 2.72  2003/04/28 04:07:55  lewis
 *	on demoexpired redirect to DemoExpired.asp instgead of main AL page
 *	
 *	Revision 2.71  2003/04/22 15:50:41  lewis
 *	SPR#1447: override LedItView::UpdateScrollBars () so we can disable it
 *	during metafile draws. Override ActiveLedItControl::OnDrawMetafile ()
 *	since default one doesn't work right for ActiveLedIt. Now we draw (metafile)
 *	properly when embedded in MSWord XP
 *	
 *	Revision 2.70  2003/04/15 23:37:39  lewis
 *	SPR#1425: added RemoveCol/Row commands
 *	
 *	Revision 2.69  2003/04/04 19:45:19  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.68  2003/04/04 19:36:53  lewis
 *	SPR#1407: convert ActiveLedIt to using new command-number vectoring
 *	etc scheme (not template based). Revised cmd #define names in a few cases
 *	for greater consistency across demo apps.
 *	
 *	Revision 2.67  2003/03/11 02:33:18  lewis
 *	SPR#1288 - use new MakeSophistsAppNameVersionURL and add new CheckForUpdatesWebPageCommand,
 *	and other cleanups to Led_URLManager calls and context menu
 *	
 *	Revision 2.66  2003/02/12 15:59:50  lewis
 *	SPR#1298- added ShowSecondaryHilight property to the control, and default to TRUE
 *	
 *	Revision 2.65  2003/01/23 20:50:04  lewis
 *	SPR#1260 - added CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK and CHECK_DEMO_AND_ALERT_AND_RETURNX
 *	and used in a few more places (e.g find dialog)
 *	
 *	Revision 2.64  2003/01/23 20:29:16  lewis
 *	SPR#1256 - use Led_StdDialogHelper_ParagraphSpacingDialog etc instead of local copy. SPR#1186 -
 *	added support for Led_StdDialogHelper_ParagraphIndentsDialog
 *	
 *	Revision 2.63  2003/01/17 18:09:06  lewis
 *	SPR#1241 - be much more aggressive when demo has expired
 *	
 *	Revision 2.62  2002/11/21 16:15:11  lewis
 *	SPR#1182 - a few changes to qDemoMode code. Write bin version# too. Add code to only give one
 *	day additional when changing version numbers. Dialogs come up more often. Small related
 *	logic fixes and disable ESC key from dismissing nag dialog
 *	
 *	Revision 2.61  2002/11/20 15:15:48  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.60  2002/11/14 17:05:29  lewis
 *	SPR#1171- use SetSelection() override instead of obsolete NotificationOf_SelectionChanged routine, and call
 *	fEditor.SetEmptySelectionStyle() in a few places where we may have made changes to the text that warrent
 *	grabbing that selection info.
 *	
 *	Revision 2.59  2002/11/12 22:31:22  lewis
 *	SPR#1170- RButtonDown must fire mousedown events
 *	
 *	Revision 2.58  2002/10/30 22:45:36  lewis
 *	SPR#1158 - SetDefaultWindowMargins () API now uses Led_TWIPS
 *	
 *	Revision 2.57  2002/10/25 14:29:24  lewis
 *	SPR#1150- tweeks of margin code towards getting full margin support
 *	
 *	Revision 2.56  2002/10/23 01:42:47  lewis
 *	SPR#1137- add Insert menu - including insert table and insert URL. Both seem to work as well as with LedIt WIN
 *	
 *	Revision 2.55  2002/10/22 15:43:39  lewis
 *	fix call to ColoredUniformHidableTextMarkerOwner() CTOR - cuz now chnaged # args
 *	
 *	Revision 2.54  2002/09/11 04:24:04  lewis
 *	Fix OnInitPopupMenu - assure we never get a separator left at the bottom of the menu
 *	
 *	Revision 2.53  2002/09/04 02:01:56  lewis
 *	static_cast<> to silence warning
 *	
 *	Revision 2.52  2002/05/06 21:34:31  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.51  2001/11/27 00:32:37  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.50  2001/08/30 00:35:57  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.49  2001/08/27 13:43:43  lewis
 *	SPR#0972- add ListStyle popup menu, and fix LedItView::GetLayoutMargins () so bullets display properly
 *	
 *	Revision 2.48  2001/08/23 20:47:02  lewis
 *	SPR#0972- new EnableAutoChangesBackgroundColor feature
 *	
 *	Revision 2.47  2001/08/17 16:30:31  lewis
 *	SPR#0959- GetInfo (DIB/URL/etc) command support
 *	
 *	Revision 2.46  2001/05/10 13:01:01  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.45  2001/04/17 16:50:29  lewis
 *	SPR#0610- Added PrinterSetupDialog, PrintDialog and PrintOnce OLE AUT methods.
 *	Also made plain Added PrinterSetupDialog, PrintDialog and PrintOnce OLE AUT methods.
 *	Also made plain
 *	
 *	Revision 2.44  2001/04/12 18:58:12  lewis
 *	cleanup DemoMode warning expired code (so easier to include in other apps)
 *	
 *	Revision 2.43  2000/11/09 05:00:58  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.42  2000/10/18 20:59:52  lewis
 *	cleanups. Lose FindDialog code - and inherit new sahred implm in Led_StdDialogHelper_FindDialog etc
 *	
 *	Revision 2.41  2000/10/06 03:37:55  lewis
 *	react to code cleanups of showhide scrollbar code in TextInteractor/Led_Win32
 *	
 *	Revision 2.40  2000/10/05 02:38:22  lewis
 *	moved MyHidableTextMarkerOwner to shared class ColoredUniformHidableTextMarkerOwner in Led.
 *	Wrap some more OLE calls in try/catch to make them more bullet-proof.
 *	
 *	Revision 2.39  2000/10/04 15:49:21  lewis
 *	cleanups and remove unneeded CMDINFO overrides - due to moving stuff to common code (SPR#0839).
 *	
 *	Revision 2.38  2000/10/04 13:21:00  lewis
 *	lose some code (undo handlers etc) obsoleted by recent code sharing changes
 *	
 *	Revision 2.37  2000/10/03 13:41:20  lewis
 *	SPR#0840- new WaterMarkHelper<> template for watermark/demomode. SPR#0839 - lose Cut/Copy/Paste
 *	overrides - now handling automatiucaly
 *	
 *	Revision 2.36  2000/09/30 19:36:30  lewis
 *	lose LedItView::OnSelectAllCommand/nUpdateCutCopyClearComma/OnUpdatePasteCommand etc since now
 *	handled by TextInteractorCommonCommandHelper_MFC
 *	
 *	Revision 2.35  2000/08/13 03:38:40  lewis
 *	fixed the paragrphspacing dialogs - bugs
 *	
 *	Revision 2.34  2000/06/16 02:43:08  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.33  2000/06/14 13:52:53  lewis
 *	cleanups(moved FindDialog etc into scope of function - by getting rid of unused message map
 *	declaration). And SPR#0785 - Added 'Paragraph Spacing cmd/dialog support
 *	
 *	Revision 2.32  2000/06/08 20:44:39  lewis
 *	SPR#0772- fix LedItView::OnSearchAgain - so if caret just before word being searched for -
 *	we actualyl find it
 *	
 *	Revision 2.31  2000/04/14 19:18:08  lewis
 *	lose unneeded override of InternalizeBestFlavor
 *	
 *	Revision 2.30  2000/04/03 15:39:57  lewis
 *	SPR#0728- Incorporate HiddenText OLEAUT support into the OCX
 *	
 *	Revision 2.29  1999/12/29 03:24:10  lewis
 *	SPR#0628- Lose #define qUseGDIScrollbitsForScrolling. Instead have methods
 *	G/SetUseBitmapScrollingOptimization ()
 *	
 *	Revision 2.28  1999/12/27 17:33:09  lewis
 *	SPR#0669 - work on this, but not done. Just added most of the support so this command is
 *	autohandled by WordProcessorCommonCommandHelper<> template. And adjusted the characters
 *	output (at least for UNICODE).
 *	
 *	Revision 2.27  1999/12/24 00:55:01  lewis
 *	make the AsTextBitmap() method work better. autosize H-Size of it better. Use auto_ptr<> to
 *	make delete of bitmap safer (if excpetions). Dont call WindowDrawHelper - but directly call
 *	fEditor.Draw() WindowDrawHelper() re-invokes OnSize() code (cuz of scrollbar changes).
 *	
 *	Revision 2.26  1999/12/21 20:47:01  lewis
 *	SPR# 0655- partialUNICODE and FULLUNICODE support
 *	
 *	Revision 2.25  1999/12/12 16:42:19  lewis
 *	SPR#0651
 *	
 *	Revision 2.24  1999/12/09 03:16:52  lewis
 *	spr#0645 - -D_UNICODE support
 *	
 *	Revision 2.23  1999/12/08 18:08:00  lewis
 *	react to changes in WordProcessorSelectionCommandHelper_MFCX<> template helpers
 *	
 *	Revision 2.22  1999/12/07 21:00:22  lewis
 *	lose code for caching/managing selectedFontSpec - and instead inherit it from
 *	WordProcessorSelectionCommandHelper (SPR#0606)
 *	
 *	Revision 2.21  1999/11/30 20:21:17  lewis
 *	Fix DemoMode EraseRect () display code to react to changes in Led_Tablet definition
 *	
 *	Revision 2.20  1999/11/16 15:17:37  lewis
 *	react to small Led changes - so Led doesn't depend on MFC
 *	
 *	Revision 2.19  1999/09/20 13:17:02  lewis
 *	some hacks todo interum 2.3.2 release - but mostly support for new DEMO_MODE build
 *	
 *	Revision 2.18  1999/07/16 21:24:40  lewis
 *	spr#0601- change the SetSBar/GetSBarType IDL methods to operate on TRISTATE - like Led_MFC,
 *	so default can be autoshow sbar. THEN - fix bug where hsbar resizing wasn't working properly -
 *	code from LedItMFC was needed - now moved to common shared template WordProcessorHScrollbarHelper<>
 *	
 *	Revision 2.17  1999/06/28 14:09:05  lewis
 *	spr#0528- Add hooks to various other windows messages, and add wrappers on FireEvent() to fire
 *	the corresponding other events. Borrowed from the pattern in COleControl::ButtonDblClk, etc,
 *	for when to fire which events. Tested under activex control test container, and all the
 *	events seemed to fire properly (with right args).
 *	
 *	Revision 2.16  1999/06/26 20:46:48  lewis
 *	work on spr#0528: Catch when focus is set to owning COleControl and pass it along to LedItView.
 *	And handle SelChange and Change notifications (events). Still several more events I must handle,
 *	since I cannot use the COleControl automatic handling - due to the compiler bug which prevents
 *	me merging tehse into a single class.
 *	
 *	Revision 2.15  1999/06/26 17:04:16  lewis
 *	override OnMouseActiveate to force the OLEControl to go into UIActivate state when user
 *	clicks on us (maybe SB on GAIN FOCUS?)- part of spr# 0596
 *	
 *	Revision 2.14  1999/06/25 14:08:53  lewis
 *	make IDL name MaxInputLength -> MaxLength to match MS RichTextControl 6.0
 *	
 *	Revision 2.13  1999/06/25 00:50:37  lewis
 *	spr#0595- add 'MaximumInputLength property to control. Most of this was trivial. We do a
 *	throw from AboutToUpdateText() method if too much text entered. Easy. But the trick is
 *	that for an OCX - there is no top level catcher to handle these excptions. They go unhandled.
 *	To address this, the Led_MFC_ExceptionHandlerHelper<> template as added
 *	
 *	Revision 2.12  1999/06/23 21:57:41  lewis
 *	add a HorzSBar and WrapToWindow toggle button
 *	
 *	Revision 2.11  1999/06/16 14:14:44  lewis
 *	cleanup
 *	
 *	Revision 2.10  1999/03/05 15:53:23  lewis
 *	no need for tmp-hack set of textstore anymore
 *	
 *	Revision 2.9  1999/03/01 15:28:48  lewis
 *	use new SetVHScrollbarType API instead of overriding PreCreateWindow to set WS_VSCROLL
 *	
 *	Revision 2.8  1998/11/02 13:23:25  lewis
 *	Changed from usin vector<char*> to vector<string> - and cleaned up font name list gen code slightly.
 *	And lost LedItView::SetWindowRect() overrides - SB very soon obsolete due to new margin/hscroll
 *	support in wordprocessor class.
 *	
 *	Revision 2.7  1998/03/04 20:44:54  lewis
 *	Major cleanups and bugfixes, but mostly fixing/addition of OLEAUT support.
 *	
 *	Revision 2.6  1997/12/24  04:44:53  lewis
 *	*** empty log message ***
 *
 *	Revision 2.5  1997/12/24  04:33:00  lewis
 *	Lose obsoltete OnCreate() override - now takene care of in Led_MFC
 *
 *	Revision 2.4  1997/09/29  18:13:17  lewis
 *	Add Justification support.
 *	Lose qLedFirstIndex support.
 *	Revise SetWindowRect() override to reflect change in TextImager SetWindowRect API.
 *	Lose old StyleDatabase API, and use new API, and add paragrpahDatabase.
 *
 *	Revision 2.3  1997/07/27  16:00:23  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.2  1997/07/23  23:24:02  lewis
 *	browse online help.
 *	Lose CTRL+Z/+R for undo/redo, since we don't support command key equivs yet.
 *
 *	Revision 2.1  1997/07/14  01:22:10  lewis
 *	*** empty log message ***
 *
 * Revision 2.0  1997/06/28  17:45:14  lewis
 * *** empty log message ***
 * 
 *
 *	<========== BasedOnLedLineIt! 2.2b1 ==========>
 *
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<afxwin.h>

#include	"ChunkedArrayTextStore.h"
#include	"LedOptionsSupport.h"
#include	"LedStdDialogs.h"

#include	"DispIDs.h"
#include	"FontMenu.h"
#include	"Resource.h"
#include	"UserConfigCommands.h"

#include	"LedItView.h"


#ifdef _DEBUG
	#define new DEBUG_NEW
#endif


#if		qDemoMode || qFunnyDisplayInDesignMode
	#include	"LedHandySimple.h"
#endif




namespace {
	static	const	TCHAR	kSearchParamsMatchString[]				=	_T ("MatchString");
	static	const	TCHAR	kSearchParamsRecentMatchStrings[]		=	_T ("RecentMatchStrings");
	static	const	TCHAR	kSearchParamsWrapSearch[]				=	_T ("WrapSearch");
	static	const	TCHAR	kSearchParamsWholeWordSearch[]			=	_T ("WholeWordSearch");
	static	const	TCHAR	kSearchParamsCaseSensativeSearch[]		=	_T ("CaseSensativeSearch");
	
	class	MyPrefs : public OptionsFileHelper {
		private:
			typedef	OptionsFileHelper	inherited;
		public:
			MyPrefs ():
				inherited (OpenWithCreateAlongPath (HKEY_CURRENT_USER, _T("Software\\Sophist Solutions, Inc.\\ActiveLedIt!\\Settings")))
				{
				}
	};
	MyPrefs	gMyPrefsFile;
}




class	My_CMDNUM_MAPPING : public MFC_CommandNumberMapping {
	public:
		My_CMDNUM_MAPPING ()
			{
				AddAssociation (kFindAgainCmd,					LedItView::kFindAgain_CmdID);
				AddAssociation (kEnterFindStringCmd,			LedItView::kEnterFindString_CmdID);
				AddAssociation (kReplaceCmd,					LedItView::kReplace_CmdID);
				AddAssociation (kReplaceAgainCmd,				LedItView::kReplaceAgain_CmdID);
				AddAssociation (kSpellCheckCmd,					LedItView::kSpellCheck_CmdID);

				AddAssociation (kSelectWordCmd,					LedItView::kSelectWord_CmdID);
				AddAssociation (kSelectTextRowCmd,				LedItView::kSelectTextRow_CmdID);
				AddAssociation (kSelectParagraphCmd,			LedItView::kSelectParagraph_CmdID);
				AddAssociation (kSelectTableIntraCellAllCmd,	LedItView::kSelectTableIntraCellAll_CmdID);
				AddAssociation (kSelectTableCellCmd,			LedItView::kSelectTableCell_CmdID);
				AddAssociation (kSelectTableRowCmd,				LedItView::kSelectTableRow_CmdID);
				AddAssociation (kSelectTableColumnCmd,			LedItView::kSelectTableColumn_CmdID);
				AddAssociation (kSelectTableCmd,				LedItView::kSelectTable_CmdID);

				AddAssociation (kFontSize9Cmd,					LedItView::kFontSize9_CmdID);
				AddAssociation (kFontSize10Cmd,					LedItView::kFontSize10_CmdID);
				AddAssociation (kFontSize12Cmd,					LedItView::kFontSize12_CmdID);
				AddAssociation (kFontSize14Cmd,					LedItView::kFontSize14_CmdID);
				AddAssociation (kFontSize18Cmd,					LedItView::kFontSize18_CmdID);
				AddAssociation (kFontSize24Cmd,					LedItView::kFontSize24_CmdID);
				AddAssociation (kFontSize36Cmd,					LedItView::kFontSize36_CmdID);
				AddAssociation (kFontSize48Cmd,					LedItView::kFontSize48_CmdID);
				AddAssociation (kFontSize72Cmd,					LedItView::kFontSize72_CmdID);
			#if		qSupportOtherFontSizeDlg
				AddAssociation (kFontSizeOtherCmd,				LedItView::kFontSizeOther_CmdID);
			#endif
				AddAssociation (kFontSizeSmallerCmd,			LedItView::kFontSizeSmaller_CmdID);
				AddAssociation (kFontSizeLargerCmd,				LedItView::kFontSizeLarger_CmdID);
				
				AddAssociation (kBlackColorCmd,					LedItView::kFontColorBlack_CmdID);
				AddAssociation (kMaroonColorCmd,				LedItView::kFontColorMaroon_CmdID);
				AddAssociation (kGreenColorCmd,					LedItView::kFontColorGreen_CmdID);
				AddAssociation (kOliveColorCmd,					LedItView::kFontColorOlive_CmdID);
				AddAssociation (kNavyColorCmd,					LedItView::kFontColorNavy_CmdID);
				AddAssociation (kPurpleColorCmd,				LedItView::kFontColorPurple_CmdID);
				AddAssociation (kTealColorCmd,					LedItView::kFontColorTeal_CmdID);
				AddAssociation (kGrayColorCmd,					LedItView::kFontColorGray_CmdID);
				AddAssociation (kSilverColorCmd,				LedItView::kFontColorSilver_CmdID);
				AddAssociation (kRedColorCmd,					LedItView::kFontColorRed_CmdID);
				AddAssociation (kLimeColorCmd,					LedItView::kFontColorLime_CmdID);
				AddAssociation (kYellowColorCmd,				LedItView::kFontColorYellow_CmdID);
				AddAssociation (kBlueColorCmd,					LedItView::kFontColorBlue_CmdID);
				AddAssociation (kFuchsiaColorCmd,				LedItView::kFontColorFuchsia_CmdID);
				AddAssociation (kAquaColorCmd,					LedItView::kFontColorAqua_CmdID);
				AddAssociation (kWhiteColorCmd,					LedItView::kFontColorWhite_CmdID);
				AddAssociation (kFontColorOtherCmd,				LedItView::kFontColorOther_CmdID);

				AddAssociation (kJustifyLeftCmd,				LedItView::kJustifyLeft_CmdID);
				AddAssociation (kJustifyCenterCmd,				LedItView::kJustifyCenter_CmdID);
				AddAssociation (kJustifyRightCmd,				LedItView::kJustifyRight_CmdID);
				AddAssociation (kJustifyFullCmd,				LedItView::kJustifyFull_CmdID);

			#if		qSupportParagraphSpacingDlg
				AddAssociation (kParagraphSpacingCmd,			LedItView::kParagraphSpacingCommand_CmdID);
			#endif
			#if		qSupportParagraphIndentsDlg
				AddAssociation (kParagraphIndentsCmd,			LedItView::kParagraphIndentsCommand_CmdID);
			#endif

				AddAssociation (kListStyle_NoneCmd,				LedItView::kListStyle_None_CmdID);
				AddAssociation (kListStyle_BulletCmd,			LedItView::kListStyle_Bullet_CmdID);
		
				AddAssociation (kIncreaseIndentCmd,				LedItView::kIncreaseIndent_CmdID);
				AddAssociation (kDecreaseIndentCmd,				LedItView::kDecreaseIndent_CmdID);

				AddRangeAssociation (
								kBaseFontNameCmd, kLastFontNameCmd,
								LedItView::kFontMenuFirst_CmdID, LedItView::kFontMenuLast_CmdID
							);

				AddAssociation (kFontStylePlainCmd,				LedItView::kFontStylePlain_CmdID);
				AddAssociation (kFontStyleBoldCmd,				LedItView::kFontStyleBold_CmdID);
				AddAssociation (kFontStyleItalicCmd,			LedItView::kFontStyleItalic_CmdID);
				AddAssociation (kFontStyleUnderlineCmd,			LedItView::kFontStyleUnderline_CmdID);
				AddAssociation (kFontStyleStrikeoutCmd,			LedItView::kFontStyleStrikeout_CmdID);
				AddAssociation (kSubScriptCmd,					LedItView::kSubScriptCommand_CmdID);
				AddAssociation (kSuperScriptCmd,				LedItView::kSuperScriptCommand_CmdID);
				AddAssociation (kChooseFontDialogCmd,			LedItView::kChooseFontCommand_CmdID);

				AddAssociation (kInsertTableCmd,				LedItView::kInsertTable_CmdID);
				AddAssociation (kInsertTableRowAboveCmd,		LedItView::kInsertTableRowAbove_CmdID);
				AddAssociation (kInsertTableRowBelowCmd,		LedItView::kInsertTableRowBelow_CmdID);
				AddAssociation (kInsertTableColBeforeCmd,		LedItView::kInsertTableColBefore_CmdID);
				AddAssociation (kInsertTableColAfterCmd,		LedItView::kInsertTableColAfter_CmdID);
				AddAssociation (kInsertURLCmd,					LedItView::kInsertURL_CmdID);
				AddAssociation (kInsertSymbolCmd,				LedItView::kInsertSymbol_CmdID);

//				AddAssociation (kPropertiesForSelectionCmd,		LedItView::kSelectedEmbeddingProperties_CmdID);
				AddRangeAssociation (
								kFirstSelectedEmbeddingCmd, kLastSelectedEmbeddingCmd,
								LedItView::kFirstSelectedEmbedding_CmdID, LedItView::kLastSelectedEmbedding_CmdID
							);

// Not 100% sure why this are disabled??? But they were before in AL (as of 2003-04-04 - AL 3.1a6x so leave it for now)
#if 0
				AddAssociation (kHideSelectionCmd,				LedItView::kHideSelection_CmdID);
				AddAssociation (kUnHideSelectionCmd,			LedItView::kUnHideSelection_CmdID);
#endif
				AddAssociation (kRemoveTableRowsCmd,			LedItView::kRemoveTableRows_CmdID);
				AddAssociation (kRemoveTableColumnsCmd,			LedItView::kRemoveTableColumns_CmdID);

// Not 100% sure why this are disabled??? But they were before in AL (as of 2003-04-04 - AL 3.1a6x so leave it for now)
#if 0
				AddAssociation (kShowHideParagraphGlyphsCmd,	LedItView::kShowHideParagraphGlyphs_CmdID);
				AddAssociation (kShowHideTabGlyphsCmd,			LedItView::kShowHideTabGlyphs_CmdID);
				AddAssociation (kShowHideSpaceGlyphsCmd,		LedItView::kShowHideSpaceGlyphs_CmdID);
#endif
			}
};
My_CMDNUM_MAPPING	sMy_CMDNUM_MAPPING;









struct	ActiveLedIt_DialogSupport : TextInteractor::DialogSupport, WordProcessor::DialogSupport {
	public:
		typedef	TextInteractor::DialogSupport::CommandNumber	CommandNumber;

	public:
		ActiveLedIt_DialogSupport ()
			{
				TextInteractor::SetDialogSupport (this);
				WordProcessor::SetDialogSupport (this);
			}
		~ActiveLedIt_DialogSupport ()
			{
				WordProcessor::SetDialogSupport (NULL);
				TextInteractor::SetDialogSupport (NULL);
			}

	//	TextInteractor::DialogSupport
#if		qSupportStdFindDlg
	public:
		override	void	DisplayFindDialog (Led_tString* findText, const vector<Led_tString>& recentFindSuggestions, bool* wrapSearch, bool* wholeWordSearch, bool* caseSensative, bool* pressedOK)
			{
				Led_StdDialogHelper_FindDialog	findDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());

				findDialog.fFindText = *findText;
				findDialog.fRecentFindTextStrings = recentFindSuggestions;
				findDialog.fWrapSearch = *wrapSearch;
				findDialog.fWholeWordSearch = *wholeWordSearch;
				findDialog.fCaseSensativeSearch = *caseSensative;

				findDialog.DoModal ();

				*findText = findDialog.fFindText;
				*wrapSearch = findDialog.fWrapSearch;
				*wholeWordSearch = findDialog.fWholeWordSearch;
				*caseSensative = findDialog.fCaseSensativeSearch;
				*pressedOK = findDialog.fPressedOK;
			}
#endif
#if		qSupportStdReplaceDlg
	public:
		override	ReplaceButtonPressed	DisplayReplaceDialog (Led_tString* findText, const vector<Led_tString>& recentFindSuggestions, Led_tString* replaceText, bool* wrapSearch, bool* wholeWordSearch, bool* caseSensative)
			{
				Led_StdDialogHelper_ReplaceDialog	replaceDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());

				replaceDialog.fFindText = *findText;
				replaceDialog.fRecentFindTextStrings = recentFindSuggestions;
				replaceDialog.fReplaceText = *replaceText;
				replaceDialog.fWrapSearch = *wrapSearch;
				replaceDialog.fWholeWordSearch = *wholeWordSearch;
				replaceDialog.fCaseSensativeSearch = *caseSensative;

				replaceDialog.DoModal ();

				*findText = replaceDialog.fFindText;
				*replaceText = replaceDialog.fReplaceText;
				*wrapSearch = replaceDialog.fWrapSearch;
				*wholeWordSearch = replaceDialog.fWholeWordSearch;
				*caseSensative = replaceDialog.fCaseSensativeSearch;

				switch (replaceDialog.fPressed) {
					case	Led_StdDialogHelper_ReplaceDialog::eCancel:					return eReplaceButton_Cancel;
					case	Led_StdDialogHelper_ReplaceDialog::eFind:					return eReplaceButton_Find;
					case	Led_StdDialogHelper_ReplaceDialog::eReplace:				return eReplaceButton_Replace;
					case	Led_StdDialogHelper_ReplaceDialog::eReplaceAll:				return eReplaceButton_ReplaceAll;
					case	Led_StdDialogHelper_ReplaceDialog::eReplaceAllInSelection:	return eReplaceButton_ReplaceAllInSelection;
				}
				Led_Assert (false); return eReplaceButton_Cancel;
			}
#endif
#if		qSupportStdSpellCheckDlg
	public:
		override	void	DisplaySpellCheckDialog (SpellCheckDialogCallback& callback)
			{
				Led_StdDialogHelper_SpellCheckDialog::CallbackDelegator<SpellCheckDialogCallback>	delegator (callback);
				#if		qMacOS
					Led_StdDialogHelper_SpellCheckDialog	spellCheckDialog (delegator);
				#elif	qWindows
					Led_StdDialogHelper_SpellCheckDialog	spellCheckDialog (delegator, ::AfxGetResourceHandle (), ::GetActiveWindow ());
				#elif	qXWindows
					Led_StdDialogHelper_SpellCheckDialog	spellCheckDialog (delegator, GTK_WINDOW (LedItApplication::Get ().GetAppWindow ()));
				#endif

				spellCheckDialog.DoModal ();
			}
#endif

	//	WordProcessor::DialogSupport
	public:
		override	FontNameSpecifier	CmdNumToFontName (CommandNumber cmdNum)
			{
				Led_Require (cmdNum >= WordProcessor::kFontMenuFirst_CmdID);
				Led_Require (cmdNum <= WordProcessor::kFontMenuLast_CmdID);
				return ::CmdNumToFontName (MFC_CommandNumberMapping::Get ().ReverseLookup (cmdNum)).c_str ();
			}
		#if		qSupportOtherFontSizeDlg
		override		Led_Distance		PickOtherFontHeight (Led_Distance origHeight)
			{
				CHECK_DEMO_AND_ALERT_AND_RETURNX_NO_TIME_CHECK (::GetActiveWindow (), false);
				#if		qMacOS
					Led_StdDialogHelper_OtherFontSizeDialog	dlg;
				#elif	qWindows
					Led_StdDialogHelper_OtherFontSizeDialog	dlg (::AfxGetResourceHandle (), ::GetActiveWindow ());
				#endif
				dlg.InitValues (origHeight);
				if (dlg.DoModal ()) {
					return dlg.fFontSize_Result;
				}
				else {
					return 0;
				}
			}
		#endif
		#if		qSupportParagraphSpacingDlg
		override		bool				PickNewParagraphLineSpacing (Led_TWIPS* spaceBefore, bool* spaceBeforeValid, Led_TWIPS* spaceAfter, bool* spaceAfterValid, Led_LineSpacing* lineSpacing, bool* lineSpacingValid)
			{
				CHECK_DEMO_AND_ALERT_AND_RETURNX_NO_TIME_CHECK (::GetActiveWindow (), false);
				#if		qMacOS
					Led_StdDialogHelper_ParagraphSpacingDialog	dlg;
				#elif	qWindows
					Led_StdDialogHelper_ParagraphSpacingDialog	dlg (::AfxGetResourceHandle (), ::GetActiveWindow ());
				#endif
				dlg.InitValues (*spaceBefore, *spaceBeforeValid, *spaceAfter, *spaceAfterValid, *lineSpacing, *lineSpacingValid);

				if (dlg.DoModal ()) {
					*spaceBeforeValid = dlg.fSpaceBefore_Valid;
					if (*spaceBeforeValid) {
						*spaceBefore = dlg.fSpaceBefore_Result;
					}
					*spaceAfterValid = dlg.fSpaceAfter_Valid;
					if (*spaceAfterValid) {
						*spaceAfter = dlg.fSpaceAfter_Result;
					}
					*lineSpacingValid = dlg.fLineSpacing_Valid;
					if (*lineSpacingValid) {
						*lineSpacing = dlg.fLineSpacing_Result;
					}
					return true;
				}
				else {
					return false;
				}
			}
		#endif
		#if		qSupportParagraphIndentsDlg
		override		bool				PickNewParagraphMarginsAndFirstIndent (Led_TWIPS* leftMargin, bool* leftMarginValid, Led_TWIPS* rightMargin, bool* rightMarginValid, Led_TWIPS* firstIndent, bool* firstIndentValid)
			{
				CHECK_DEMO_AND_ALERT_AND_RETURNX_NO_TIME_CHECK (::GetActiveWindow (), false);
				#if		qMacOS
					Led_StdDialogHelper_ParagraphIndentsDialog	dlg;
				#elif	qWindows
					Led_StdDialogHelper_ParagraphIndentsDialog	dlg (::AfxGetResourceHandle (), ::GetActiveWindow ());
				#endif
				dlg.InitValues (*leftMargin, *leftMarginValid, *rightMargin, *rightMarginValid, *firstIndent, *firstIndentValid);
				if (dlg.DoModal ()) {
					*leftMarginValid = dlg.fLeftMargin_Valid;
					if (*leftMarginValid) {
						*leftMargin = dlg.fLeftMargin_Result;
					}
					*rightMarginValid = dlg.fRightMargin_Valid;
					if (*rightMarginValid) {
						*rightMargin = dlg.fRightMargin_Result;
					}
					*firstIndentValid = dlg.fFirstIndent_Valid;
					if (*firstIndentValid) {
						*firstIndent = dlg.fFirstIndent_Result;
					}
					return true;
				}
				else {
					return false;
				}
			}
		#endif
		override	void				ShowSimpleEmbeddingInfoDialog (const Led_SDK_String& embeddingTypeName)
			{
				CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK (::GetActiveWindow ());
				// unknown embedding...
				Led_StdDialogHelper_UnknownEmbeddingInfoDialog	infoDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
				infoDialog.fEmbeddingTypeName = embeddingTypeName;
				(void)infoDialog.DoModal ();
			}
		override		bool	ShowURLEmbeddingInfoDialog (const Led_SDK_String& embeddingTypeName, Led_SDK_String* urlTitle, Led_SDK_String* urlValue)
			{
				CHECK_DEMO_AND_ALERT_AND_RETURNX_NO_TIME_CHECK (::GetActiveWindow (), false);
				Led_StdDialogHelper_URLXEmbeddingInfoDialog	infoDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
				infoDialog.fEmbeddingTypeName = embeddingTypeName;
				infoDialog.fTitleText = *urlTitle;
				infoDialog.fURLText = *urlValue;
				if (infoDialog.DoModal ()) {
					*urlTitle = infoDialog.fTitleText;
					*urlValue = infoDialog.fURLText;
					return true;
				}
				else {
					return false;
				}
			}
		override		bool	ShowAddURLEmbeddingInfoDialog (Led_SDK_String* urlTitle, Led_SDK_String* urlValue)
			{
				CHECK_DEMO_AND_ALERT_AND_RETURNX_NO_TIME_CHECK (::GetActiveWindow (), false);
				Led_StdDialogHelper_AddURLXEmbeddingInfoDialog	infoDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
				infoDialog.fTitleText = *urlTitle;
				infoDialog.fURLText = *urlValue;
				if (infoDialog.DoModal ()) {
					*urlTitle = infoDialog.fTitleText;
					*urlValue = infoDialog.fURLText;
					return true;
				}
				else {
					return false;
				}
			}
		#if		qSupportAddNewTableDlg
		bool	AddNewTableDialog (size_t* nRows, size_t* nCols)
			{
				Led_RequireNotNil (nRows);
				Led_RequireNotNil (nCols);
				Led_StdDialogHelper_AddNewTableDialog	infoDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
				infoDialog.fRows = *nRows;
				infoDialog.fColumns = *nCols;
				if (infoDialog.DoModal ()) {
					*nRows = infoDialog.fRows;
					*nCols = infoDialog.fColumns;
					return true;
				}
				else {
					return false;
				}
			}
		#endif
		#if		qSupportEditTablePropertiesDlg
		override	bool	EditTablePropertiesDialog (TableSelectionPropertiesInfo* tableProperties)
			{
				Led_RequireNotNil (tableProperties);

				typedef	Led_StdDialogHelper_EditTablePropertiesDialog	DLGTYPE;
				#if		qMacOS
					DLGTYPE	infoDialog;
				#elif	qWindows
					DLGTYPE	infoDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
				#endif
				DLGTYPE::cvt<DLGTYPE::Info, TableSelectionPropertiesInfo> (&infoDialog.fInfo, *tableProperties);
				if (infoDialog.DoModal ()) {
					DLGTYPE::cvt<TableSelectionPropertiesInfo, DLGTYPE::Info> (tableProperties, infoDialog.fInfo);
					return true;
				}
				else {
					return false;
				}
			}
		#endif
};
static	ActiveLedIt_DialogSupport	sActiveLedIt_DialogSupport;













static	BOOL CALLBACK _AfxAbortProc (HDC, int)
{
	_AFX_WIN_STATE* pWinState = _afxWinState;
	MSG msg;
	while (!pWinState->m_bUserAbort &&
		::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
	{
		if (!AfxGetThread()->PumpMessage())
			return FALSE;   // terminate if WM_QUIT received
	}
	return !pWinState->m_bUserAbort;
}





#if		qDemoMode
/*
 ********************************************************************************
 ******************************* DemoModeAlerter ********************************
 ********************************************************************************
 */
int				DemoModeAlerter::sNextWarningAfter		=	0;
const	float	DemoModeAlerter::kTimeBetweenWarnings	=	0.5*60;	// at most every 1 minute(s) - but really less often - cuz only on draws...

void	DemoModeAlerter::ShowAlert (HWND parentWnd)
{
	class	DemoExpiresDLG : public Led_StdDialogHelper {
		private:
			typedef	Led_StdDialogHelper	inherited;
		public:
			DemoExpiresDLG (HINSTANCE hInstance, HWND parentWnd):
				inherited (hInstance, MAKEINTRESOURCE (kDemoExpired_DialogID), parentWnd)
			{
			}
		override	void	OnOK ()
			{
				inherited::OnOK ();
				Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/Led/ActiveLedIt/DemoExpired.asp", kAppName, string (kURLDemoFlag) + kDemoExpiredExtraArgs));
				//Led_URLManager::Get ().Open (kLedItDemoExpiredURL);
			}
		override	void	OnCancel ()
			{
				// Ignore - force the bugger to hit a button!!!
			}
		override	BOOL	DialogProc (UINT message, WPARAM wParam, LPARAM lParam)
			{
				if (message == WM_COMMAND and wParam == kDemoExpired_Dialog_CancelFieldID) {
					// OK - he did some work todo this - now let him cancel
					inherited::OnCancel ();
					return true;
				}
				else {
					return inherited::DialogProc (message, wParam, lParam);
				}
			}
	};

	if (parentWnd == NULL) {
		parentWnd = ::GetActiveWindow ();
	}
	// Only check if we are visible. Sometimes this gets called - somewhat inexplicably - during the close of the browser
	// window. Avoid somewhat ugly/confusing display of alert.
	if (::IsWindow (parentWnd) and ::IsWindowVisible (parentWnd)) {
		sNextWarningAfter = INT_MAX;	// disable warnings til after Dialog finishes display to prevent multiple dialogs from appearing
		try {
			DemoExpiresDLG	dlg (::AfxGetResourceHandle (), parentWnd);
			dlg.DoModal ();
			int		now						=	time (NULL);
			sNextWarningAfter = static_cast<int> (now + kTimeBetweenWarnings);	// Set here so delay based on when dialog dismissed instead of when dialog comes up.
		}
		catch (...) {
			// ignore any errors with this code...
			int		now						=	time (NULL);
			sNextWarningAfter = static_cast<int> (now + kTimeBetweenWarnings);	// even on failure - reset timer for next call
		}
	}
}

void	DemoModeAlerter::ShowAlertIfItsBeenAWhile (HWND parentWnd)
{
	int		now						=	time (NULL);
	if (now > sNextWarningAfter) {
		ShowAlert (parentWnd);
	}
	else {
		Led_BeepNotify ();
	}
}
#endif








/*
 ********************************************************************************
 ************************** LedItViewController *********************************
 ********************************************************************************
 */
LedItViewController::LedItViewController ():
	fTextStore (),
	fCommandHandler (kMaxNumUndoLevels),
	fHidableTextDatabase ()

{
	fHidableTextDatabase = new ColoredUniformHidableTextMarkerOwner (fTextStore);
}

LedItViewController::~LedItViewController ()
{
}





/*
 ********************************************************************************
 ************************************ LedItView *********************************
 ********************************************************************************
 */

DoDeclare_Led_MFC_ExceptionHandlerHelper_MessageMap(Led_MFC_X<WordProcessor>);
DoDeclare_WordProcessorCommonCommandHelper_MFC_MessageMap (LedItViewAlmostBASE)

BEGIN_MESSAGE_MAP(LedItView, LedItView::inherited)
	ON_WM_MOUSEACTIVATE ()
	ON_WM_INITMENUPOPUP			()
	ON_WM_CONTEXTMENU			()
	ON_WM_CHAR					()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()

	ON_COMMAND					(ID_FILE_PRINT,											OnFilePrint)
	ON_COMMAND					(ID_FILE_PRINT_SETUP,									OnFilePrintSetup)
	ON_COMMAND					(ID_HELP_FINDER,										OnBrowseHelpCommand)
	ON_COMMAND					(kCheckForUpdatesWebPageCmdID,							OnCheckForUpdatesWebPageCommand)
	ON_COMMAND					(ID_APP_ABOUT,											OnAboutBoxCommand)
	ON_COMMAND					(kPasteFormatTextCmdID,									OnPasteAsTextCommand)
	ON_UPDATE_COMMAND_UI		(kPasteFormatTextCmdID,									OnUpdatePasteAsTextCommand)
	ON_COMMAND_RANGE			(kFirstOLEUserCmdCmdID, kLastOLEUserCmdCmdID,			OnOLEUserCommand)
	ON_UPDATE_COMMAND_UI_RANGE	(kFirstOLEUserCmdCmdID, kLastOLEUserCmdCmdID,			OnUpdateOLEUserCommand)
END_MESSAGE_MAP()



LedItView::LedItView ():
	inherited (),
	fController (NULL),
	fSupportContextMenu (true),
	fHideDisabledContextMenuItems (true),
	fWrapToWindow (true),
	fMaxLength (-1),
	fEnableAutoChangesBackgroundColor (true),
	fInDrawMetaFileMode (false)
{
	SetScrollBarType (v, eScrollBarAsNeeded);
	SetScrollBarType (h, eScrollBarAsNeeded);
#if		qDemoMode
	SetUseBitmapScrollingOptimization (false);
#endif
	SetUseSecondaryHilight (true);			// default to TRUE since I think this looks better and maybe a differentiator with other controls
	const	Led_TWIPS	kLedItViewTopMargin		=	Led_TWIPS (120);
	const	Led_TWIPS	kLedItViewBottomMargin	=	Led_TWIPS (0);
	const	Led_TWIPS	kLedItViewLHSMargin		=	Led_TWIPS (150);
	const	Led_TWIPS	kLedItViewRHSMargin		=	Led_TWIPS (0);
	SetDefaultWindowMargins (Led_TWIPS_Rect (kLedItViewTopMargin, kLedItViewLHSMargin, kLedItViewBottomMargin-kLedItViewTopMargin, kLedItViewRHSMargin-kLedItViewLHSMargin));
}

LedItView::~LedItView()
{
	SetController (NULL);
}

void	LedItView::SetController (LedItViewController* controller)
{
	if (fController != NULL) {
		SetHidableTextDatabase (NULL);
		SetCommandHandler (NULL);
		SpecifyTextStore (NULL);
	}
	fController = controller;
	if (fController != NULL) {
		SpecifyTextStore (&fController->fTextStore);
		SetCommandHandler (&fController->fCommandHandler);
		SetHidableTextDatabase (fController->fHidableTextDatabase);
	}
}

void	LedItView::SetSupportContextMenu (bool allowContextMenu)
{
	fSupportContextMenu = allowContextMenu;
}

void	LedItView::SetHideDisabledContextMenuItems (bool hideDisabledContextMenuItems)
{
	fHideDisabledContextMenuItems = hideDisabledContextMenuItems;
}

void	LedItView::SetWrapToWindow (bool wrapToWindow)
{
	if (fWrapToWindow != wrapToWindow) {
		fWrapToWindow = wrapToWindow;
		InvalidateAllCaches ();
		Refresh ();
	}
}

void	LedItView::SetMaxLength (long maxLength)
{
	fMaxLength = maxLength;
}

void	LedItView::GetLayoutMargins (RowReference row, Led_Coordinate* lhs, Led_Coordinate* rhs) const
{
	if (GetWrapToWindow ()) {
		// Make the layout right margin of each line (paragraph) equal to the windowrect. Ie, wrap to the
		// edge of the window. NB: because of this choice, we must 'InvalidateAllCaches' when the
		// WindowRect changes in our SetWindowRect() override.
		Led_Coordinate	l	=	0;
		Led_Coordinate	r	=	0;
		inherited::GetLayoutMargins (row, &l, &r);
		r = Led_Max (static_cast<Led_Coordinate> (GetWindowRect ().GetWidth ()), l+1);
		Led_Ensure (r > l);
		if (lhs != NULL) {
			*lhs = l;
		}
		if (rhs != NULL) {
			*rhs = r;
		}
	}
	else {
		inherited::GetLayoutMargins (row, lhs, rhs);
	}
}

void	LedItView::SetWindowRect (const Led_Rect& windowRect)
{
	// Hook all changes in the window width, so we can invalidate the word-wrap info (see LedItView::GetLayoutMargins)
	Led_Rect	oldWindowRect	=	GetWindowRect ();
	if (windowRect != oldWindowRect) {
		WordWrappedTextImager::SetWindowRect (windowRect);		//	NB: use XX instead of inherited to avoid infinite recurse, due to sloppy mixin ambiguity resoltion in base classes (LGP990623)
		if (GetWrapToWindow () and windowRect.GetSize () != oldWindowRect.GetSize ()) {
			InvalidateAllCaches ();
		}
	}
}

Led_Distance	LedItView::CalculateFarthestRightMarginInWindow () const
{
	if (fWrapToWindow) {
		return GetWindowRect ().GetWidth ();
	}
	else {
		return inherited::CalculateFarthestRightMarginInWindow ();
	}
}

void	LedItView::PostNcDestroy ()
{
}

int		LedItView::OnMouseActivate (CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	if (fController != NULL) {
		fController->ForceUIActive ();
	}
	return inherited::OnMouseActivate (pDesktopWnd, nHitTest, message);
}

void	LedItView::OnInitMenuPopup (CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	// Disable inappropriate commands.
	// Remove disabled menu items from the popup (to save space).
	// Remove consecutive (or leading) separators as useless.

	CCmdUI	state;
	state.m_pSubMenu = NULL;
	state.m_pMenu = pPopupMenu;
	Led_Assert (state.m_pOther == NULL);
	Led_Assert (state.m_pParentMenu == NULL);

	// Set the enable/disable state of each menu item.
	state.m_nIndexMax = pPopupMenu->GetMenuItemCount ();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++) {
		state.m_nID = pPopupMenu->GetMenuItemID (state.m_nIndex);
		if (state.m_nID != 0) {
			Led_Assert (state.m_pOther == NULL);
			Led_Assert (state.m_pMenu != NULL);
			state.DoUpdate (this, true);
		}
	}

	// Remove disabled items (and unneeded separators)
	if (GetHideDisabledContextMenuItems ()) {
		bool	prevItemSep	=	true;	// prevent initial separators
		for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; ) {
			state.m_nID = pPopupMenu->GetMenuItemID (state.m_nIndex);
			if (state.m_nID == 0 and prevItemSep) {
				pPopupMenu->RemoveMenu (state.m_nIndex, MF_BYPOSITION);
				state.m_nIndexMax--;
				continue;
			}
			if (state.m_nID != 0) {
				MENUITEMINFO	mInfo;
				memset (&mInfo, 0, sizeof (mInfo));
				mInfo.cbSize = sizeof (mInfo);
				mInfo.fMask = MIIM_STATE;
				Led_Verify (::GetMenuItemInfo (pPopupMenu->GetSafeHmenu (), state.m_nIndex, true, &mInfo));
				if (mInfo.fState & MFS_DISABLED) {
					pPopupMenu->RemoveMenu (state.m_nIndex, MF_BYPOSITION);
					state.m_nIndexMax--;
					continue;
				}
			}
			prevItemSep = bool (state.m_nID == 0);
			state.m_nIndex++;
		}
		// if LAST item is a separator - remove THAT
		if (prevItemSep) {
			pPopupMenu->RemoveMenu (state.m_nIndexMax-1, MF_BYPOSITION);
		}
	}

	inherited::OnInitMenuPopup (pPopupMenu, nIndex, bSysMenu);
}

void	LedItView::OnContextMenu (CWnd* /*pWnd*/, CPoint pt) 
{
	if (GetSupportContextMenu ()) {
		CMenu menu;
		Led_AssertNotNil (fController);
		if (menu.Attach (fController->GenerateContextMenu ())) {
			menu.TrackPopupMenu (TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
		}
	}
}
extern	short AFXAPI _AfxShiftState ();

void	LedItView::OnSysKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (fController != NULL) {
		USHORT	shortChar	=	nChar;
		fController->FireKeyDown (&shortChar, _AfxShiftState());
	}
	inherited::OnSysKeyDown (nChar, nRepCnt, nFlags);
}

void	LedItView::OnSysKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (fController != NULL) {
		USHORT	shortChar	=	nChar;
		fController->FireKeyUp (&shortChar, _AfxShiftState());
	}
	inherited::OnSysKeyUp (nChar, nRepCnt, nFlags);
}

void	LedItView::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (fController != NULL) {
		USHORT	shortChar	=	nChar;
		fController->FireKeyDown (&shortChar, _AfxShiftState());
	}
	inherited::OnKeyDown (nChar, nRepCnt, nFlags);
}

void	LedItView::OnKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (fController != NULL) {
		USHORT	shortChar	=	nChar;
		fController->FireKeyUp (&shortChar, _AfxShiftState());
	}
	inherited::OnKeyUp (nChar, nRepCnt, nFlags);
}

void	LedItView::OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed, bool controlPressed, bool altKeyPressed)
{
	if (fController != NULL) {
		USHORT	shortChar	=	theChar;
		fController->FireKeyPress (&shortChar);
	}
	inherited::OnTypedNormalCharacter (theChar, optionPressed, shiftPressed, commandPressed, controlPressed, altKeyPressed);
}

void	LedItView::OnLButtonDown (UINT nFlags, CPoint oPoint)
{
	if (fController != NULL) {
		fController->FireMouseDown (LEFT_BUTTON, _AfxShiftState (), oPoint.x, oPoint.y);
	}
	inherited::OnLButtonDown (nFlags, oPoint);
}

void	LedItView::OnLButtonUp (UINT nFlags, CPoint oPoint)
{
	if (fController != NULL) {
		fController->FireMouseUp (LEFT_BUTTON, _AfxShiftState (), oPoint.x, oPoint.y);
		if (GetCurClickCount () == 1) {
			fController->FireClick ();
		}
	}
	inherited::OnLButtonUp (nFlags, oPoint);
}

void	LedItView::OnLButtonDblClk (UINT nFlags, CPoint oPoint)
{
	if (fController != NULL) {
		fController->FireDblClick ();
	}
	inherited::OnLButtonDblClk (nFlags, oPoint);
}

void	LedItView::OnRButtonDown (UINT nFlags, CPoint oPoint)
{
	if (fController != NULL) {
		fController->FireMouseDown (RIGHT_BUTTON, _AfxShiftState (), oPoint.x, oPoint.y);
	}
	inherited::OnRButtonDown (nFlags, oPoint);
}

void	LedItView::OnRButtonUp (UINT nFlags, CPoint oPoint)
{
	if (fController != NULL) {
		fController->FireMouseUp (RIGHT_BUTTON, _AfxShiftState (), oPoint.x, oPoint.y);
	}
	inherited::OnRButtonUp (nFlags, oPoint);
}

void	LedItView::OnMouseMove (UINT nFlags, CPoint oPoint)
{
	if (fController != NULL) {
		fController->FireMouseMove (LEFT_BUTTON, _AfxShiftState (), oPoint.x, oPoint.y);
	}
	inherited::OnMouseMove (nFlags, oPoint);
}

void	LedItView::OnPasteAsTextCommand ()
{
	InteractiveModeUpdater	iuMode (*this);
	BreakInGroupedCommands ();

	if (OnPasteCommand_Before ()) {
		try {
			UndoableContextHelper	undoContext (*this, Led_SDK_TCHAROF ("Paste Text"), false);
				{
					OnPasteCommand_PasteFlavor_Specific (kTEXTClipFormat);
				}
			undoContext.CommandComplete ();
		}
		catch (...) {
			OnPasteCommand_After ();
			throw;
		}
		OnPasteCommand_After ();
	}
	BreakInGroupedCommands ();
}

void	LedItView::OnUpdatePasteAsTextCommand (CCmdUI* pCmdUI)
{
	OnUpdatePasteCommand (Led_MFC_TmpCmdUpdater (pCmdUI));
}

void	LedItView::OnOLEUserCommand (UINT nID)
{
	if (fController != NULL) {
		fController->FireUserCommand (UserCommandNameNumberRegistry::Get ().Lookup (nID));
	}
}

void	LedItView::OnUpdateOLEUserCommand (CCmdUI* pCmdUI)
{
	if (fController != NULL) {
		VARIANT_BOOL	enabled	=	true;
		VARIANT_BOOL	checked	=	false;
		wstring			name;
// SHOULD DO BETTER TO GRAB THIS VALUE FROM THE pCMDUI GUY- BUT NOT CRITICAL...
		fController->FireUpdateUserCommand (UserCommandNameNumberRegistry::Get ().Lookup (pCmdUI->m_nID), &enabled, &checked, &name);
		pCmdUI->Enable (enabled);
		pCmdUI->SetCheck (checked);
		if (not name.empty ()) {
			pCmdUI->SetText (Led_Wide2SDKString (name).c_str ());
		}
	}
}

LedItView::SearchParameters	LedItView::GetSearchParameters () const
{
	SearchParameters	sp;
	sp.fMatchString = gMyPrefsFile.GetPref (kSearchParamsMatchString, sp.fMatchString);
	sp.fRecentFindStrings = gMyPrefsFile.GetPref (kSearchParamsRecentMatchStrings, sp.fRecentFindStrings);
	sp.fWrapSearch = gMyPrefsFile.GetPref (kSearchParamsWrapSearch, sp.fWrapSearch);
	sp.fWholeWordSearch = gMyPrefsFile.GetPref (kSearchParamsWholeWordSearch, sp.fWholeWordSearch);
	sp.fCaseSensativeSearch = gMyPrefsFile.GetPref (kSearchParamsCaseSensativeSearch, sp.fCaseSensativeSearch);
	return sp;
}

void	LedItView::SetSearchParameters (const SearchParameters& sp)
{
	gMyPrefsFile.StorePref (kSearchParamsMatchString, sp.fMatchString);
	gMyPrefsFile.StorePref (kSearchParamsRecentMatchStrings, sp.fRecentFindStrings);
	gMyPrefsFile.StorePref (kSearchParamsWrapSearch, sp.fWrapSearch);
	gMyPrefsFile.StorePref (kSearchParamsWholeWordSearch, sp.fWholeWordSearch);
	gMyPrefsFile.StorePref (kSearchParamsCaseSensativeSearch, sp.fCaseSensativeSearch);
}

void	LedItView::SetSelection (size_t start, size_t end)
{
	bool	changed	=	start != GetSelectionStart () or end != GetSelectionEnd ();
	inherited::SetSelection (start, end);
	if (changed and fController != NULL) {
		fController->FireOLEEvent (DISPID_SelChange);
	}
}

void	LedItView::AboutToUpdateText (const UpdateInfo& updateInfo)
{
	CHECK_DEMO_AND_ALERT_AND_RETURN(GetHWND ());

	if (GetMaxLength () != -1) {
		long	textAdded	=	long (updateInfo.fTextLength) - (updateInfo.fReplaceTo - updateInfo.fReplaceFrom);
		if (textAdded + long (GetLength ()) > GetMaxLength ()) {
			throw "";
		}
	}

	inherited::AboutToUpdateText (updateInfo);
}

void	LedItView::EraseBackground (Led_Tablet tablet, const Led_Rect& subsetToDraw, bool printing)
{
	if (fEnableAutoChangesBackgroundColor) {
		inherited::EraseBackground (tablet, subsetToDraw, printing);
	}
	else {
		TextImager::EraseBackground (tablet, subsetToDraw, printing);
	}
#if		qDemoMode
	static	bool	demoExpired	=	DemoPrefs ().GetDemoDaysLeft () <= 0;
	Led_tString		demoModeMessage	=	demoExpired? LED_TCHAR_OF ("Demo EXPIRED Mode"): LED_TCHAR_OF ("Demo Mode");
	static	WaterMarkHelper<>	waterMarkerImager (demoModeMessage);	// make this static - just as a performance hack. Also could be an instance variable of 'this'.
	waterMarkerImager.SetWatermarkColor (demoExpired? Led_Color::kRed: Led_Color::kYellow);
	waterMarkerImager.DrawWatermark (tablet, GetWindowRect (), subsetToDraw);
#endif

#if		qFunnyDisplayInDesignMode
	if (fController->IsInDesignMode ()) {
		Led_tString		message	=	LED_TCHAR_OF ("Design Mode");
		static	WaterMarkHelper<>	waterMarkerImager (message);	// make this static - just as a performance hack. Also could be an instance variable of 'this'.
		waterMarkerImager.SetWatermarkColor (Led_Color::kYellow);
		Led_Rect	designModeRect	=	Led_Rect (0, 0, 20, 150);
		Led_Rect	wr				=	GetWindowRect ();
		{
			if (static_cast<Led_Distance> (designModeRect.bottom) > wr.GetHeight ()) {
				designModeRect.bottom = wr.GetHeight ();
			}
			if (static_cast<Led_Distance> (designModeRect.right) > wr.GetWidth ()) {
				designModeRect.right = wr.GetWidth ();
			}
			designModeRect = CenterRectInRect (designModeRect, wr);
			Led_Distance	h	=	designModeRect.GetHeight ();
			designModeRect.bottom = wr.bottom;
			designModeRect.top = designModeRect.bottom - h;
		}
		waterMarkerImager.DrawWatermark (tablet, designModeRect, subsetToDraw);
		if (fController->DrawExtraDesignModeBorder ()) {
			Led_Color	useBorderColor	=	Led_Color::kBlack;
			{
				Led_Color	defBackgrndColor	=	Led_GetTextBackgroundColor ();
				if (GetHWND () != NULL) {
					DWORD	dwStyle = GetStyle ();
					if (((dwStyle & WS_DISABLED) or (dwStyle & ES_READONLY)) and (not printing)) {
						defBackgrndColor	=	Led_Color (::GetSysColor (COLOR_BTNFACE));
					}
				}
				// opposite of background color
				useBorderColor = Led_Color (~defBackgrndColor.GetRed (), ~defBackgrndColor.GetGreen (), ~defBackgrndColor.GetBlue ());
			}

			Led_Rect		wmr	=	tablet->CvtFromTWIPS (GetDefaultWindowMargins ());
			Led_Rect		cr	=	wr;
			cr.top -= wmr.GetTop ();
			cr.left -= wmr.GetLeft ();
			cr.bottom += wmr.GetBottom ();
			cr.right += wmr.GetRight ();
			tablet->FrameRectangle (cr, useBorderColor, 1);
		}
	}
#endif
}

long	LedItView::OLE_FindReplace (long searchFrom, const Led_tString& findText, const Led_tString& replaceText, BOOL wrapSearch, BOOL wholeWordSearch, BOOL caseSensativeSearch)
{
	TextStore::SearchParameters	parameters;
	parameters.fMatchString			=	findText;
	parameters.fWrapSearch			=	wrapSearch;
	parameters.fWholeWordSearch		=	wholeWordSearch;
	parameters.fCaseSensativeSearch =	caseSensativeSearch;

	size_t	whereTo	=	GetTextStore ().Find (parameters, searchFrom);
	if (whereTo != kBadIndex) {
		size_t	replaceStart	=	whereTo;
		size_t	replaceEnd		=	whereTo + parameters.fMatchString.length ();
		TextInteractor::UndoableContextHelper	undoContext (*this, TextInteractor::GetCommandNames ().fReplaceCommandName, replaceStart, replaceEnd, GetSelectionStart (), GetSelectionEnd (), false);
			{
				InteractiveReplace_ (undoContext.GetUndoRegionStart (), undoContext.GetUndoRegionEnd (), replaceText.c_str (), replaceText.length ());
			}
		undoContext.CommandComplete ();
		return whereTo + replaceText.length ();
	}

	return -1;
}

void	LedItView::UpdateScrollBars ()
{
	if (fInDrawMetaFileMode) {
		return;	// ignore while in print mode...
	}
	inherited::UpdateScrollBars ();
}

void	LedItView::OnBrowseHelpCommand ()
{
	if (fController != NULL) {
		fController->OnBrowseHelpCommand ();
	}
}

void	LedItView::OnCheckForUpdatesWebPageCommand ()
{
	Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/Led/CheckForUpdates.asp", kAppName, kURLDemoFlag));
}

void	LedItView::OnAboutBoxCommand ()
{
	if (fController != NULL) {
		fController->OnAboutBoxCommand ();
	}
}

void	LedItView::OnFilePrintOnce ()
{
	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(GetHWND ());
	DoPrintHelper (false);
}

void	LedItView::OnFilePrint ()
{
	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(GetHWND ());
	DoPrintHelper (true);
}

void	LedItView::OnFilePrintSetup ()
{
	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(GetHWND ());
	CPrintDialog pd(TRUE);
	AfxGetApp ()->DoPrintDialog (&pd);
}

/*
 *	Based on CView::OnFilePrint() - but with one bug fixed (at least originally). That code was buggy when used in an OCX cuz there
 *	is no doc and no frame window (perhaps I could have created one - but I wasn't sure what other bugs/problems that might create).
 *
 *		Then - when I tried to compile that code here - there were all sorts of things that wouldn't compile. So I had to make a bunch of
 *	OTHER changes due to the fact that I don't include all the private MFC headers here. And as long as I was making such bug changes - I threw away
 *	a bunch of detritus as well.
 *
 *		-	LGP 2001-04-17
 */
void	LedItView::DoPrintHelper (bool showPrintDlg)
{
	class CPrintingDialog : public CDialog {
		public:
			enum { IDD = AFX_IDD_PRINTDLG };
			CPrintingDialog (CWnd* pParent)
				{
					Create (CPrintingDialog::IDD, pParent);      // modeless !
					_afxWinState->m_bUserAbort = FALSE;
				}
			virtual ~CPrintingDialog() { }
			virtual BOOL OnInitDialog()
				{
					SetWindowText(AfxGetAppName());
					CenterWindow();
					return CDialog::OnInitDialog();
				}
			virtual void OnCancel ()
				{
					_afxWinState->m_bUserAbort = TRUE;  // flag that user aborted print
					CDialog::OnCancel();
				}
	};

	// get default print info
	CPrintInfo printInfo;
	ASSERT(printInfo.m_pPD != NULL);    // must be set

	if (not showPrintDlg) {
		printInfo.m_bDirect = true;
		printInfo.m_dwFlags &= ~PRINTFLAG_PROMPTUSER;
	}

	if (OnPreparePrinting (&printInfo)) {
		// hDC must be set (did you remember to call DoPreparePrinting?)
		ASSERT(printInfo.m_pPD->m_pd.hDC != NULL);

		// gather file to print to if print-to-file selected
		CString strOutput;
		if (printInfo.m_pPD->m_pd.Flags & PD_PRINTTOFILE && !printInfo.m_bDocObject) {
			// construct CFileDialog for browsing
			CString strDef(MAKEINTRESOURCE(AFX_IDS_PRINTDEFAULTEXT));
			CString strPrintDef(MAKEINTRESOURCE(AFX_IDS_PRINTDEFAULT));
			CString strFilter(MAKEINTRESOURCE(AFX_IDS_PRINTFILTER));
			CString strCaption(MAKEINTRESOURCE(AFX_IDS_PRINTCAPTION));
			CFileDialog dlg(FALSE, strDef, strPrintDef,
				OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, strFilter);
			dlg.m_ofn.lpstrTitle = strCaption;

			if (dlg.DoModal() != IDOK)
				return;

			// set output device to resulting path name
			strOutput = dlg.GetPathName ();
		}

		// set up document info and start the document printing process
		CString strTitle	=	"ActiveLedIt! document";
		DOCINFO docInfo;
		memset(&docInfo, 0, sizeof(DOCINFO));
		docInfo.cbSize = sizeof(DOCINFO);
		docInfo.lpszDocName = strTitle;
		CString strPortName;
		int nFormatID;
		if (strOutput.IsEmpty()) {
			docInfo.lpszOutput = NULL;
			strPortName = printInfo.m_pPD->GetPortName();
			nFormatID = AFX_IDS_PRINTONPORT;
		}
		else {
			docInfo.lpszOutput = strOutput;
			extern	UINT AFXAPI AfxGetFileTitle (LPCTSTR lpszPathName, LPTSTR lpszTitle, UINT nMax);
			AfxGetFileTitle(strOutput, strPortName.GetBuffer(_MAX_PATH), _MAX_PATH);
			nFormatID = AFX_IDS_PRINTTOFILE;
		}

		// setup the printing DC
		CDC dcPrint;
		if (!printInfo.m_bDocObject) {
			dcPrint.Attach(printInfo.m_pPD->m_pd.hDC);  // attach printer dc
			dcPrint.m_bPrinting = TRUE;
		}
		OnBeginPrinting (&dcPrint, &printInfo);

		if (!printInfo.m_bDocObject) {
			dcPrint.SetAbortProc(_AfxAbortProc);
		}

		// disable main window while printing & init printing status dialog
		HWND	mainWnd	=	AfxGetMainWnd ()->m_hWnd;
		::EnableWindow (mainWnd, FALSE);
		CPrintingDialog dlgPrintStatus (this);
		BOOL bError = FALSE;
		try {
			CString strTemp;
			dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_DOCNAME, strTitle);
			dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_PRINTERNAME, printInfo.m_pPD->GetDeviceName());
			AfxFormatString1(strTemp, nFormatID, strPortName);
			dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_PORTNAME, strTemp);
			dlgPrintStatus.ShowWindow(SW_SHOW);
			dlgPrintStatus.UpdateWindow();

			// start document printing process
			if (!printInfo.m_bDocObject && dcPrint.StartDoc(&docInfo) == SP_ERROR) {
				// enable main window before proceeding
				::EnableWindow (mainWnd, true);

				// cleanup and show error message
				OnEndPrinting(&dcPrint, &printInfo);
				dlgPrintStatus.DestroyWindow();
				dcPrint.Detach();   // will be cleaned up by CPrintInfo destructor
				AfxMessageBox(AFX_IDP_FAILED_TO_START_PRINT);
				return;
			}

			// Guarantee values are in the valid range
			UINT nEndPage = printInfo.GetToPage();
			UINT nStartPage = printInfo.GetFromPage();

			if (nEndPage < printInfo.GetMinPage())
				nEndPage = printInfo.GetMinPage();
			if (nEndPage > printInfo.GetMaxPage())
				nEndPage = printInfo.GetMaxPage();

			if (nStartPage < printInfo.GetMinPage())
				nStartPage = printInfo.GetMinPage();
			if (nStartPage > printInfo.GetMaxPage())
				nStartPage = printInfo.GetMaxPage();

			int nStep = (nEndPage >= nStartPage) ? 1 : -1;
			nEndPage = (nEndPage == 0xffff) ? 0xffff : nEndPage + nStep;

			VERIFY(strTemp.LoadString(AFX_IDS_PRINTPAGENUM));

			// If it's a doc object, we don't loop page-by-page
			// because doc objects don't support that kind of levity.

			if (printInfo.m_bDocObject) {
				OnPrepareDC(&dcPrint, &printInfo);
				OnPrint(&dcPrint, &printInfo);
			}
			else {
				// begin page printing loop
				for (printInfo.m_nCurPage = nStartPage; printInfo.m_nCurPage != nEndPage; printInfo.m_nCurPage += nStep) {
					OnPrepareDC(&dcPrint, &printInfo);

					// check for end of print
					if (!printInfo.m_bContinuePrinting)
						break;

					// write current page
					TCHAR szBuf[80];
					wsprintf(szBuf, strTemp, printInfo.m_nCurPage);
					dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_PAGENUM, szBuf);

					// set up drawing rect to entire page (in logical coordinates)
					printInfo.m_rectDraw.SetRect(0, 0,
						dcPrint.GetDeviceCaps(HORZRES),
						dcPrint.GetDeviceCaps(VERTRES));
					dcPrint.DPtoLP(&printInfo.m_rectDraw);

					// attempt to start the current page
					if (dcPrint.StartPage() < 0) {
						bError = TRUE;
						break;
					}

					// must call OnPrepareDC on newer versions of Windows because
					// StartPage now resets the device attributes.
					BOOL	bMarked4	=	false;
					{
						DWORD	dwVersion = ::GetVersion();
						BOOL	bWin4 = (BYTE)dwVersion >= 4;
						// determine various metrics based on EXE subsystem version mark
						if (bWin4) {
							bMarked4 = (GetProcessVersion(0) >= 0x00040000);
						}
					}
					if (bMarked4)
						OnPrepareDC(&dcPrint, &printInfo);

					ASSERT(printInfo.m_bContinuePrinting);

					// page successfully started, so now render the page
					OnPrint(&dcPrint, &printInfo);
					if (dcPrint.EndPage() < 0 || !_AfxAbortProc(dcPrint.m_hDC, 0))
					{
						bError = TRUE;
						break;
					}
				}
			}
		}
		catch (...) {
			// cleanup document printing process
			if (!printInfo.m_bDocObject) {
				dcPrint.AbortDoc();
			}

			::EnableWindow (mainWnd, true);

			OnEndPrinting (&dcPrint, &printInfo);    // clean up after printing
			throw;
		}

		// cleanup document printing process
		if (!printInfo.m_bDocObject) {
			if (!bError)
				dcPrint.EndDoc();
			else
				dcPrint.AbortDoc();
		}

		::EnableWindow (mainWnd, true);

		OnEndPrinting(&dcPrint, &printInfo);    // clean up after printing
		dlgPrintStatus.DestroyWindow();

		dcPrint.Detach();   // will be cleaned up by CPrintInfo destructor
	}
}

#ifdef	_DEBUG
void	LedItView::AssertValid () const
{
	inherited::AssertValid ();
}

void	LedItView::Dump (CDumpContext& dc) const
{
	inherited::Dump (dc);
}
#endif //_DEBUG





// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
