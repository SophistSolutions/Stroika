/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItMFC/Sources/LedItView.cpp,v 2.74 2001/04/14 17:06:34 lewis Exp $
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
 *	Revision 2.74  2001/04/14 17:06:34  lewis
 *	Lose MFC PrintPreview support cuz doesn't work - and not worth trying to resurect (esp since rewriting print support myself). SPR#0868
 *	
 *	Revision 2.73  2000/10/18 21:00:20  lewis
 *	Lose FindDialog code - and inherit new sahred implm in Led_StdDialogHelper_FindDialog etc
 *	
 *	Revision 2.72  2000/10/06 03:41:44  lewis
 *	react to code cleanups of showhide scrollbar code in TextInteractor/Led_Win32
 *	
 *	Revision 2.71  2000/10/04 15:46:34  lewis
 *	cleanups and remove unneeded CMDINFO overrides - due to moving stuff to common code (SPR#0839).
 *	
 *	Revision 2.70  2000/10/04 13:20:05  lewis
 *	lose some code (undo handlers etc) obsoleted by recent code sharing changes
 *	
 *	Revision 2.69  2000/10/03 13:42:23  lewis
 *	lose class WordProcessorCommonCommandHelper_MFCX- instead just WordProcessorCommonCommandHelper_MFC suffices. And lose OnUpdateCutCopyClearCommand stuff
 *	
 *	Revision 2.68  2000/09/30 19:36:07  lewis
 *	lose LedItView::OnSelectAllCommand/nUpdateCutCopyClearComma/OnUpdatePasteCommand etc since now handled by TextInteractorCommonCommandHelper_MFC
 *	
 *	Revision 2.67  2000/08/28 22:15:44  lewis
 *	SPR#0831- call AddEmbedding with styledatabase instead of this as marker owner for newly created embeddings.
 *	
 *	Revision 2.66  2000/08/28 20:37:52  lewis
 *	SPR#0826- use new WordProcessorCommonCommandHelper_DefaultCmdInfo, and support kHideSelection_CmdID/unhide using new shared code.
 *	
 *	Revision 2.65  2000/08/13 03:38:19  lewis
 *	extra arg to AddEmebdding, and fixed the paragrphspacing dialogs - bugs
 *	
 *	Revision 2.64  2000/06/16 02:16:11  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.63  2000/06/13 22:59:06  lewis
 *	cleanups. And SPR#0785- at least a crufty UI for adjusting line spacing params
 *	
 *	Revision 2.62  2000/06/13 16:29:32  lewis
 *	SPR#0767- use TWIPS for tabstops etc inernally in Led
 *	
 *	Revision 2.61  2000/06/08 20:44:51  lewis
 *	SPR#0772- fix LedItView::OnSearchAgain - so if caret just before word being searched for - we actualyl find it
 *	
 *	Revision 2.60  2000/04/26 16:01:34  lewis
 *	lose unneeded LedItView::InternalizeBestFlavor/inherited::HookLosingTextStore/LedItView::HookGainedNewTextStore overrides
 *	
 *	Revision 2.59  2000/04/24 17:28:21  lewis
 *	SPR#0743 use new Led_MFC_WordProcessor.h module. And lose override of InternalizeBestFlavor () - see SPR# 0744
 *	
 *	Revision 2.58  2000/04/14 19:08:50  lewis
 *	SPR#0739- Added FlavorPackage support
 *	
 *	Revision 2.57  2000/03/31 21:18:35  lewis
 *	use new GetHidableRegionsContiguous () API to properly enable/disable the show/hide selection commands - related to spr#0717
 *	
 *	Revision 2.56  2000/03/31 00:51:37  lewis
 *	SPR#0717 - more changes to adapt to new HiddenText support. Rip out some of the code I had added to LedItView - and instead keep a HiddenTextDatabase with the document (like the paragraphdatabase). Now we support REAADING RTF files with hidden text. Next todo is writing RTF with hidden text
 *	
 *	Revision 2.55  2000/03/30 16:28:36  lewis
 *	HiddenMarker code rename
 *	
 *	Revision 2.54  2000/03/18 03:45:36  lewis
 *	for proper safe destruction of the fHiddenTextOwner - must HookLosing/GainingTextStore as place to create/destroy - NOT in the OnInitialUpdate method
 *	
 *	Revision 2.53  2000/03/17 22:40:14  lewis
 *	SPR#0717- preliminary support - for showhiddentext testing and UI
 *	
 *	Revision 2.52  1999/12/28 17:19:01  lewis
 *	save prefrences for ShowPara/Tab/SpaceGlyphs.
 *	
 *	Revision 2.51  1999/12/27 17:32:57  lewis
 *	SPR#0669 - work on this, but not done. Just added most of the support so this command is autohandled by WordProcessorCommonCommandHelper<> template. And adjusted the characters output (at least for UNICODE).
 *	
 *	Revision 2.50  1999/12/24 00:56:21  lewis
 *	Lose LedItView::OnEraseBkgnd override and much of OnSize() override - cuz not handled automatically via new SetDefaultWindowMargins method - SPR#0679
 *	
 *	Revision 2.49  1999/12/12 16:36:35  lewis
 *	SPR#0651
 *	
 *	Revision 2.48  1999/12/09 17:29:21  lewis
 *	lots of cleanups/fixes for new (partial/full) UNICODE support - SPR#0645
 *	
 *	Revision 2.47  1999/12/08 17:46:51  lewis
 *	reverse args and add default args to WordProcessorSelectionCommandHelper_MFCX template
 *	
 *	Revision 2.46  1999/12/08 03:20:08  lewis
 *	cleanup and use new WordProcessorSelectionCommandHelper_MFCX wrapper
 *	
 *	Revision 2.45  1999/12/07 20:12:00  lewis
 *	remove alot of code for mananaging setting font selection menus etc. Moved to shared template WordProcessorSelectionCommandHelper<BASECLASS,CMD_ENABLER,CMD_INFO> as part of SPR#0606
 *	
 *	Revision 2.44  1999/07/16 21:10:22  lewis
 *	move some code from LedItMFC\LedItView to the template WordProcessorHScrollbarHelper<>, so that it can be shared in ActiveLedIt. REALLY - we should do much more of this sharing of code, but this is a start
 *	
 *	Revision 2.43  1999/05/03 22:23:48  lewis
 *	add CF_NOVERTFOTNS to LED CFOngDIalog
 *	
 *	Revision 2.42  1999/04/28 15:16:50  lewis
 *	must also manually call InvalidateAllCaches in OnIntiailUpdate - for rare cases when we still have line-height info cached, but no need to Do/DidUpdate calls cuz no change in effective font of emptyselfont
 *	
 *	Revision 2.41  1999/04/28 14:59:03  lewis
 *	instead of InteractiveSetFont - just call new SetEmptySelectionStyle - and be careful to preseve the DOC modified flag
 *	
 *	Revision 2.40  1999/04/28 13:56:14  lewis
 *	Fix OnNewDocument() to properly reset fonts/paragraph info (and cursel font) to defaults
 *	
 *	Revision 2.39  1999/03/25 02:45:51  lewis
 *	use new InteractiveSetFont - just a new name wrapper - on old SetDefaultFont
 *	
 *	Revision 2.38  1999/03/24 23:01:04  lewis
 *	call new InteractiveXXX (like InteractiveSetJustification) instead of the XXX methods, so commands undoable
 *	
 *	Revision 2.37  1999/03/04 14:32:21  lewis
 *	no longer need to set sTmpHack textstore in LeditView::CTOR - so get rid of the hack
 *	
 *	Revision 2.36  1999/03/01 15:29:54  lewis
 *	use new SetVHScrollbarType API instead of overriding PreCreateWindow to set WS_VSCROLL, and also lose (really moved into Led) gross hack I HAD needed for changing sbar type when I changed wrap-to-window flag
 *	
 *	Revision 2.35  1999/02/12 21:43:49  lewis
 *	Fix(hook in/finish implementing) horizontal scrolling, and make h-scrollbar disapear when in wrap-to-window mode.
 *	
 *	Revision 2.34  1999/02/11 02:41:13  lewis
 *	cosmetic code cleanups
 *	
 *	Revision 2.33  1999/02/10 16:22:18  lewis
 *	call/pass along SetMargins/SetFirstIndent calls to/from ruler and to/from leditview. Now really works (though not perfectly, pretty close
 *	
 *	Revision 2.32  1999/02/08 22:31:36  lewis
 *	Support new optional 'wrap-to-window' feature. Used to be we ALWAYS did that. Now can be on/off. Next todo is to fix the OFF case to properly use ruler
 *	
 *	Revision 2.31  1998/10/30 14:54:09  lewis
 *	Use vector<> instead of Led_Array.
 *	Turn on V&H scrolling.
 *	New MSVC60 compiler warnign BWA
 *	
 *	Revision 2.30  1998/04/25  17:38:44  lewis
 *	*** empty log message ***
 *
 *	Revision 2.29  1998/04/25  01:51:37  lewis
 *	Get/Set ParaFormatSelection added - to support tabstop UI from Ruler.
 *
 *	Revision 2.28  1998/04/09  01:33:08  lewis
 *	work around MultiRowTextImager hack where its Partition keeps ptrs into the view - so in MFC View DTOR - must set ParagraphDBase's Partition toNULL
 *
 *	Revision 2.27  1998/04/08  02:18:03  lewis
 *	*** empty log message ***
 *
 *	Revision 2.26  1997/12/24  04:47:51  lewis
 *	*** empty log message ***
 *
 *	Revision 2.25  1997/12/24  04:11:03  lewis
 *	Support for toolbars - formatbar, and ruler. And now search params in Options module (and
 *	persisted using registry).
 *	Lose OnCreate () (done in MFC now - registering Droptarget).
 *
 *	Revision 2.24  1997/09/29  16:01:45  lewis
 *	Lose qLedFirstIndex support.
 *	Added Justification menu support.
 *	New styleDatabase API support (lose SPecicy and add Set) and added similar new ParagraphDbase support.
 *
 *	Revision 2.23  1997/07/27  16:01:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.22  1997/07/23  23:16:59  lewis
 *	Support margin around text.
 *
 *	Revision 2.21  1997/07/14  14:50:48  lewis
 *	Renamed TextStore_ to TextStore.
 *
 *	Revision 2.20  1997/06/28  17:30:25  lewis
 *	Changed FixupFontMenu call cuz moved menus around(context menu).
 *	Lose gDocBeingCreated global in favor of much cleaner OnInitailUpdate() mechnaism (works better for MDI).
 *
 *	Revision 2.19  1997/06/24  03:43:50  lewis
 *	Override GetLayoutWidth() and call InvalidateAllCahces() in SetWindowRect() instead of SetLayoutWidth (now gone).
 *	for spr#0450.
 *
 *	Revision 2.18  1997/06/18  03:42:58  lewis
 *	qIncludePrefixFile and cleanups and sub/superscrpit support
 *
 *	Revision 2.17  1997/03/23  01:02:35  lewis
 *	MSVC50 fixes
 *
 *	Revision 2.16  1997/03/04  20:16:22  lewis
 *	*** empty log message ***
 *
 *	Revision 2.15  1997/01/20  05:36:00  lewis
 *	Added new font color menu, and enabled font color setting from usual CFontDialog call.
 *
 *	Revision 2.14  1997/01/10  03:31:05  lewis
 *	cleanups
 *	throw speciiers.
 *	override SetWindowREct() to set the layout widht.
 *	Fix messagemap/dyncrate for new base class.
 *
 *	Revision 2.13  1996/12/13  18:03:20  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.12  1996/12/05  21:18:00  lewis
 *	*** empty log message ***
 *
 *	Revision 2.11  1996/09/30  14:59:03  lewis
 *	Share some undo/redo code.
 *	Use new TextStore::Find() API - and keep new SearchParameters struct with the application.
 *
 *	Revision 2.10  1996/09/01  15:43:56  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.9  1996/07/03  01:59:08  lewis
 *	Add GetSoleSelectedEmbedding () method.
 *	Fix kFontSizeSmallerCmdID/Larger to do right thing in run of text with mixed
 *	sizes.
 *
 *	Revision 2.8  1996/06/01  02:46:34  lewis
 *	Lose no-RTTI crap.
 *	fixed bug with font size and choose font dialog.
 *	overrde LedItView::DidUpdateText () to inval fCachedCurSelFontSpecValid
 *	instaed of doing so in a zillion routines.
 *	Cleanups?
 *
 *	Revision 2.7  1996/05/23  20:20:31  lewis
 *	Totally revised and substnatilly improved font menu code. Now use new Led_IncremnetalFontSpeciations, and cache
 *	last result of GetContienusStuleInfo().
 *	And support larger/smaller commands, and support OTHER SIZE command.
 *	Now simpler, and much more correct.
 *
 *	Revision 2.6  1996/04/18  15:55:03  lewis
 *	Lots of cleanups and re-organizeations. But mostly redid the drag/drop
 *	etc flavor support, but then moved it into Led_MFC.
 *
 *	Revision 2.5  1996/03/16  19:11:44  lewis
 *	Added OnUpdateEnterFindStringCommand/OnEnterFindString.
 *
 *	Revision 2.4  1996/02/26  22:49:29  lewis
 *	AddEmbedding no longer takes StyleDatabaes arg.
 *	Fix PC drag code to behave more reasonably when you click on a selection
 *	but don't drag it anywhere (deselect text). This code could still use
 *	work, but its better now. Now 100% sure what it really should do.
 *	Use kBadIndex instead of size_t (-1), and new Led_Point etc conversion functions.
 *	Use gDocBeingCreated textstore instead of static text store.
 *
 *	Revision 2.3  1996/02/05  04:49:50  lewis
 *	Lots of changes, but mainly adding find dialog, and fixing up
 *	choose font dialog, and font menu/style menu, and font size menus
 *	to check/uncheck items properly, etc.
 *
 *	Revision 2.2  1996/01/22  05:43:04  lewis
 *	MANY changes.
 *	New drag/drop support.
 *	New (proto) find/replace support.
 *	Fixed prining and moved to Led_MFC.
 *	Added hooks to new Command/Undo support. Using MultiLevel undo.
 *
 *	Revision 2.1  1996/01/11  08:32:52  lewis
 *	Do DoVerb call AFTER AddEmbedding - so when we get rezize method (drom DoVerb) we are all setup.
 *
 *	Revision 2.0  1996/01/04  00:58:05  lewis
 *	*** empty log message ***
 *
 *	Revision 1.4  1996/01/04  00:57:44  lewis
 *	*** empty log message ***
 *
 *	Revision 1.3  1995/12/09  05:58:42  lewis
 *	specify explicity Led(NULL) ctor so we get NO textstore created.
 *	Avoids bug in MSVC with calls to virtual methods before our vtable
 *	intact.
 *
 *	Revision 1.2  1995/12/08  07:47:54  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<afxodlgs.h>       // MFC OLE dialog classes

#include	"LedStdDialogs.h"

#include	"ColorMenu.h"
#include	"FontMenu.h"
#include	"LedItControlItem.h"
#include	"LedItDocument.h"
#include	"Options.h"
#include	"Resource.h"

#include	"LedItView.h"




#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (4 : 4800)	//qUsePragmaWarningToSilenceNeedlessBoolConversionWarnings
#endif









/*
 ********************************************************************************
 ******************************* LedItViewCommandInfo ***************************
 ********************************************************************************
 */
void	LedItViewCommandInfo::DisplayFindDialog (Led_tString* findText, bool* wrapSearch, bool* wholeWordSearch, bool* caseSensative, bool* pressedOK)
{
	Led_StdDialogHelper_FindDialog	findDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());

	findDialog.fFindText = *findText;
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

LedItViewCommandInfo::FontNameSpecifier	LedItViewCommandInfo::CmdNumToFontName (CommandNumber cmdNum)
{
	return ::CmdNumToFontName (cmdNum).c_str ();
}

Led_Distance	LedItViewCommandInfo::PickOtherFontHeight (Led_Distance origHeight)
{
	class	OtherFontSizeDialog : public CDialog {
		private:
			typedef	CDialog	inherited;
		public:
			OtherFontSizeDialog (Led_Distance origFontSize) :
				inherited (kOtherFontSize_DialogID),
				fOrigFontSize (origFontSize),
				fResultFontSize (0)
				{
				}
			override	BOOL OnInitDialog ()
				{
					BOOL	result	=	inherited::OnInitDialog ();
					SetDlgItemInt (kOtherFontSize_Dialog_FontSizeEditFieldID, fOrigFontSize);
					return (result);
				}
			override	void	OnOK ()
				{
					BOOL	trans	=	false;
					fResultFontSize = GetDlgItemInt (kOtherFontSize_Dialog_FontSizeEditFieldID, &trans);
					if (not trans) {
						fResultFontSize = 0;
					}
					inherited::OnOK ();
				}
		public:
			Led_Distance	fOrigFontSize;
			Led_Distance	fResultFontSize;

	};

	OtherFontSizeDialog	dlg (origHeight);

	if (dlg.DoModal () == IDOK) {
		return dlg.fResultFontSize;
	}
	else {
		return 0;
	}
}

bool	LedItViewCommandInfo::PickNewParagraphLineSpacing (Led_TWIPS* spaceBefore, bool* spaceBeforeValid, Led_TWIPS* spaceAfter, bool* spaceAfterValid, Led_LineSpacing* lineSpacing, bool* lineSpacingValid)
{
	class	ParagraphSpacingDialog : public CDialog {
		private:
			typedef	CDialog	inherited;
		public:
			ParagraphSpacingDialog (Led_TWIPS spaceBefore, bool spaceBeforeValid, Led_TWIPS spaceAfter, bool spaceAfterValid, Led_LineSpacing lineSpacing, bool lineSpacingValid) :
				inherited (kParagraphSpacing_DialogID),
				fLineSpaceMode (),
				fSpaceBefore_Valid (spaceBeforeValid),
				fSpaceBefore_Orig (spaceBefore),
				fSpaceBefore_Result (spaceBefore),
				fSpaceAfter_Valid (spaceAfterValid),
				fSpaceAfter_Orig (spaceAfter),
				fSpaceAfter_Result (spaceAfter),
				fLineSpacing_Valid (lineSpacingValid),
				fLineSpacing_Orig (lineSpacing),
				fLineSpacing_Result (lineSpacing)
				{
				}
			virtual ~ParagraphSpacingDialog ()
				{
					fLineSpaceMode.Detach ();
				}
			override	BOOL OnInitDialog ()
				{
					BOOL	result	=	inherited::OnInitDialog ();
					if (fSpaceBefore_Valid) {
						SetDlgItemInt (kParagraphSpacing_Dialog_SpaceBeforeFieldID, fSpaceBefore_Orig);
					}
					if (fSpaceAfter_Valid) {
						SetDlgItemInt (kParagraphSpacing_Dialog_SpaceAfterFieldID, fSpaceAfter_Orig);
					}
					VERIFY (fLineSpaceMode.Attach (GetDlgItem (kParagraphSpacing_Dialog_LineSpaceModeFieldID)->m_hWnd));
					fLineSpaceMode.ResetContent ();
					fLineSpaceMode.AddString (_T ("Single"));
					fLineSpaceMode.AddString (_T ("1.5 lines"));
					fLineSpaceMode.AddString (_T ("Double"));
					fLineSpaceMode.AddString (_T ("At Least (TWIPS)"));
					fLineSpaceMode.AddString (_T ("Exact (TWIPS)"));
					fLineSpaceMode.AddString (_T ("Exact (1/20 lines)"));
					if (fLineSpacing_Valid) {
						fLineSpaceMode.SetCurSel (fLineSpacing_Orig.fRule);
						if (fLineSpacing_Orig.fRule == Led_LineSpacing::eAtLeastTWIPSSpacing or fLineSpacing_Orig.fRule == Led_LineSpacing::eExactTWIPSSpacing or fLineSpacing_Orig.fRule == Led_LineSpacing::eExactLinesSpacing) {
							SetDlgItemInt (kParagraphSpacing_Dialog_LineSpaceArgFieldID, fLineSpacing_Orig.fArg);
						}
					}
					return (result);
				}
			override	void	OnOK ()
				{
					BOOL	trans	=	false;
					fSpaceBefore_Result = Led_TWIPS (GetDlgItemInt (kParagraphSpacing_Dialog_SpaceBeforeFieldID, &trans));
					fSpaceBefore_Valid = trans;
					if (not trans) {
						fSpaceBefore_Result = fSpaceBefore_Orig;
					}
					
					fSpaceAfter_Result = Led_TWIPS (GetDlgItemInt (kParagraphSpacing_Dialog_SpaceAfterFieldID, &trans));
					fSpaceAfter_Valid = trans;
					if (not trans) {
						fSpaceAfter_Result = fSpaceAfter_Orig;
					}

					int	r	=	fLineSpaceMode.GetCurSel ();
					if (r >= 0 and r <= 5) {
						fLineSpacing_Valid = true;
						if (r == Led_LineSpacing::eAtLeastTWIPSSpacing or r == Led_LineSpacing::eExactTWIPSSpacing) {
							fLineSpacing_Result = Led_LineSpacing (Led_LineSpacing::Rule (r), Led_TWIPS (GetDlgItemInt (kParagraphSpacing_Dialog_LineSpaceArgFieldID, &trans)));
							if (not trans) {
								fLineSpacing_Valid = false;
							}
						}
						else if (r == Led_LineSpacing::eExactLinesSpacing) {
							fLineSpacing_Result = Led_LineSpacing (Led_LineSpacing::Rule (r), GetDlgItemInt (kParagraphSpacing_Dialog_LineSpaceArgFieldID, &trans));
							if (not trans) {
								fLineSpacing_Valid = false;
							}
						}
						else {
							fLineSpacing_Result = Led_LineSpacing::Rule (r);
						}
					}

					inherited::OnOK ();
				}
		public:
			CComboBox		fLineSpaceMode;
			bool			fSpaceBefore_Valid;
			Led_TWIPS		fSpaceBefore_Orig;
			Led_TWIPS		fSpaceBefore_Result;
			bool			fSpaceAfter_Valid;
			Led_TWIPS		fSpaceAfter_Orig;
			Led_TWIPS		fSpaceAfter_Result;
			bool			fLineSpacing_Valid;
			Led_LineSpacing	fLineSpacing_Orig;
			Led_LineSpacing	fLineSpacing_Result;
	};

	ParagraphSpacingDialog	dlg (*spaceBefore, *spaceBeforeValid, *spaceAfter, *spaceAfterValid, *lineSpacing, *lineSpacingValid);

	if (dlg.DoModal () == IDOK) {
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

Led_Color	LedItViewCommandInfo::FontCmdToColor (CommandNumber cmd)
{
	return ColorMenu::FontCmdToColor (cmd);
}

LedItViewCommandInfo::CommandNumber	LedItViewCommandInfo::FontColorToCmd (Led_Color color)
{
	return ColorMenu::FontColorToCmd (color);
}







/*
 ********************************************************************************
 ************************************ LedItView *********************************
 ********************************************************************************
 */

DoDeclare_WordProcessorCommonCommandHelper_MFC_MessageMapX (LedItViewAlmostBASE,LedItViewCommandInfo)

IMPLEMENT_DYNCREATE(LedItView,	CView)

BEGIN_MESSAGE_MAP(LedItView, LedItView::inherited)
	ON_WM_SETFOCUS				()
	ON_WM_SIZE					()
	ON_WM_CONTEXTMENU			()
	ON_COMMAND					(ID_OLE_INSERT_NEW,										OnInsertObject)
	ON_COMMAND					(ID_CANCEL_EDIT_CNTR,									OnCancelEditCntr)
	ON_COMMAND					(ID_CANCEL_EDIT_SRVR,									OnCancelEditSrvr)
	ON_COMMAND					(ID_FILE_PRINT,											OnFilePrint)
	ON_COMMAND					(ID_FILE_PRINT_DIRECT,									OnFilePrint)
	ON_NOTIFY					(NM_RETURN,					ID_VIEW_FORMATBAR,			OnBarReturn)
END_MESSAGE_MAP()



LedItView::LedItView ():
	inherited (),
	fWrapToWindow (Options ().GetWrapToWindow ())
{
	SetSmartCutAndPasteMode (Options ().GetSmartCutAndPaste ());

	SetShowParagraphGlyphs (Options ().GetShowParagraphGlyphs ());
	SetShowTabGlyphs (Options ().GetShowTabGlyphs ());
	SetShowSpaceGlyphs (Options ().GetShowSpaceGlyphs ());

	SetScrollBarType (h, fWrapToWindow? eScrollBarNever: eScrollBarAsNeeded);
	SetScrollBarType (v, eScrollBarAlways);

	const	Led_Coordinate	kLedItViewTopMargin		=	8;
	const	Led_Coordinate	kLedItViewBottomMargin	=	0;
	#if 1
		const	Led_Coordinate	kLedItViewLHSMargin		=	0;
		const	Led_Coordinate	kLedItViewRHSMargin		=	0;
	#else
		//Margins may look slightly better (though 8 maybe too much) - but we need to adjust the ruler layout code first...
		// so things line up...
		const	Led_Coordinate	kLedItViewLHSMargin		=	8;
		const	Led_Coordinate	kLedItViewRHSMargin		=	8;
	#endif
	SetDefaultWindowMargins (kLedItViewTopMargin, kLedItViewLHSMargin, kLedItViewBottomMargin, kLedItViewRHSMargin);
}

LedItView::~LedItView()
{
	SpecifyTextStore (NULL);
	SetCommandHandler (NULL);
}

void	LedItView::OnInitialUpdate ()
{
	inherited::OnInitialUpdate ();
	SpecifyTextStore (&GetDocument ().GetTextStore ());
	SetStyleDatabase (GetDocument ().GetStyleDatabase ());
	SetParagraphDatabase (GetDocument ().GetParagraphDatabase ());
	SetHidableTextDatabase (GetDocument ().GetHidableTextDatabase ());
	SetShowHiddenText (Options ().GetShowHiddenText ());
	SetCommandHandler (&GetDocument ().GetCommandHandler ());

	{
		// Don't let this change the docs IsModified flag
		bool	docModified	=	GetDocument ().IsModified ();
		SetEmptySelectionStyle (TextImager::GetStaticDefaultFont ());
		InvalidateAllCaches ();	// under rare circumstances, the caches don't all get cleared without this...
		GetDocument ().SetModifiedFlag (docModified);
	}
}

void	LedItView::SetWrapToWindow (bool wrapToWindow)
{
	if (fWrapToWindow != wrapToWindow) {
		fWrapToWindow = wrapToWindow;
		SetScrollBarType (h, fWrapToWindow? eScrollBarNever: eScrollBarAsNeeded);
		InvalidateAllCaches ();
		InvalidateScrollBarParameters ();
		Refresh ();
	}
}

void	LedItView::GetLayoutMargins (RowReference row, Led_Coordinate* lhs, Led_Coordinate* rhs) const
{
	inherited::GetLayoutMargins (row, lhs, rhs);
	if (fWrapToWindow) {
		if (rhs != NULL) {
//HMM. UNCLEAR IF WE WANT TO SET TO WINDOWRECT WIDTH OR RHS?
			*rhs = (Led_Max (GetWindowRect ().GetWidth (), 1));
		}
	}
}

void	LedItView::SetWindowRect (const Led_Rect& windowRect)
{
	// Hook all changes in the window width, so we can invalidate the word-wrap info (see LedItView::GetLayoutWidth)
	if (windowRect != GetWindowRect ()) {
		// NB: call "WordWrappedTextImager::SetWindowRect() instead of base class textinteractor to avoid infinite recursion"
		WordWrappedTextImager::SetWindowRect (windowRect);
		if (fWrapToWindow) {
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

void	LedItView::OnContextMenu (CWnd* /*pWnd*/, CPoint pt) 
{
	CMenu menu;
	if (menu.LoadMenu (kContextMenu)) {
		CMenu*	popup = menu.GetSubMenu (0);
		Led_AssertNotNil (popup);
		FixupFontMenu (popup->GetSubMenu (11));
		popup->TrackPopupMenu (TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, ::AfxGetMainWnd ());
	}
}

BOOL	LedItView::IsSelected (const CObject* pDocItem) const
{
	// The implementation below is adequate if your selection consists of
	//  only LedItControlItem objects.  To handle different selection
	//  mechanisms, the implementation here should be replaced.

	// TODO: implement this function that tests for a selected OLE client item
	return pDocItem == GetSoleSelectedOLEEmbedding ();
}

WordProcessor::IncrementalParagraphInfo	LedItView::GetParaFormatSelection ()
{
//COULD SPEED TWEEK THIS - LIKE I DID FOR fCachedCurSelJustificationUnique
	IncrementalParagraphInfo	ipi;
	StandardTabStopList	tabstops;
	if (GetStandardTabStopList (GetSelectionStart (), GetSelectionEnd (), &tabstops)) {
		ipi.SetTabStopList (tabstops);
	}
	Led_TWIPS	lhsMargin	=	Led_TWIPS (0);
	Led_TWIPS	rhsMargin	=	Led_TWIPS (0);
	if (GetMargins (GetSelectionStart (), GetSelectionEnd (), &lhsMargin, &rhsMargin)) {
		ipi.SetMargins (lhsMargin, rhsMargin);
	}
	Led_TWIPS	firstIndent	=	Led_TWIPS (0);
	if (GetFirstIndent (GetSelectionStart (), GetSelectionEnd (), &firstIndent)) {
		ipi.SetFirstIndent (firstIndent);
	}
	return ipi;
}

void	LedItView::SetParaFormatSelection (const IncrementalParagraphInfo& pf)
{
	if (pf.GetTabStopList_Valid ()) {
		InteractiveSetStandardTabStopList (pf.GetTabStopList ());
	}
	if (pf.GetMargins_Valid () and pf.GetFirstIndent_Valid ()) {
		InteractiveSetMarginsAndFirstIndent (pf.GetLeftMargin (), pf.GetRightMargin (), pf.GetFirstIndent ());
	}
	else {
		if (pf.GetMargins_Valid ()) {
			InteractiveSetMargins (pf.GetLeftMargin (), pf.GetRightMargin ());
		}
		if (pf.GetFirstIndent_Valid ()) {
			InteractiveSetFirstIndent (pf.GetFirstIndent ());
		}
	}
}

void	LedItView::OnShowHideGlyphCommand (CommandNumber cmdNum)
{
	inherited::OnShowHideGlyphCommand (cmdNum);

	Options ().SetShowParagraphGlyphs (GetShowParagraphGlyphs ());
	Options ().SetShowTabGlyphs (GetShowTabGlyphs ());
	Options ().SetShowSpaceGlyphs (GetShowSpaceGlyphs ());
}

LedItView::SearchParameters	LedItView::GetSearchParameters () const
{
	return Options ().GetSearchParameters ();
}

void	LedItView::SetSearchParameters (const SearchParameters& sp)
{
	Options ().SetSearchParameters (sp);
}

void	LedItView::SetShowHiddenText (bool showHiddenText)
{
	if (showHiddenText) {
		GetHidableTextDatabase ()->ShowAll ();
	}
	else {
		GetHidableTextDatabase ()->HideAll ();
	}
}

void	LedItView::OnInsertObject ()
{
	// Invoke the standard Insert Object dialog box to obtain information
	//  for new LedItControlItem object.
	COleInsertDialog dlg;
	if (dlg.DoModal() != IDOK) {
		return;
	}

	CWaitCursor	busy;

	LedItControlItem* pItem = NULL;
	TRY {
		// Create new item connected to this document.
		LedItDocument&	doc = GetDocument ();
		pItem = new LedItControlItem (&doc);
		ASSERT_VALID (pItem);

		// Initialize the item from the dialog data.
		if (!dlg.CreateItem (pItem)) {
			Led_ThrowBadFormatDataException ();
		}
		ASSERT_VALID(pItem);

		AddEmbedding (pItem, GetTextStore (), GetSelectionStart (), GetDocument ().GetStyleDatabase ());

		// If item created from class list (not from file) then launch
		//  the server to edit the item.
		if (dlg.GetSelectionType () == COleInsertDialog::createNewItem) {
			pItem->DoVerb (OLEIVERB_SHOW, this);
		}

		ASSERT_VALID (pItem);

		// As an arbitrary user interface design, this sets the selection
		//  to the last item inserted.
// SHOULD WE???


		// TODO: reimplement selection as appropriate for your application

		doc.UpdateAllViews (NULL);
	}
	CATCH(CException, e) {
		if (pItem != NULL) {
			ASSERT_VALID(pItem);
			pItem->Delete();
		}
		AfxMessageBox (IDP_FAILED_TO_CREATE);
	}
	END_CATCH
}

// The following command handler provides the standard keyboard
//  user interface to cancel an in-place editing session.  Here,
//  the container (not the server) causes the deactivation.
void	LedItView::OnCancelEditCntr ()
{
	// Close any in-place active item on this view.
	COleClientItem* pActiveItem = GetDocument ().GetInPlaceActiveItem (this);
	if (pActiveItem != NULL) {
		pActiveItem->Close ();
	}
	ASSERT (GetDocument ().GetInPlaceActiveItem(this) == NULL);
}

// Special handling of OnSetFocus and OnSize are required for a container
//  when an object is being edited in-place.
void	LedItView::OnSetFocus (CWnd* pOldWnd)
{
	COleClientItem* pActiveItem = GetDocument ().GetInPlaceActiveItem (this);
	if (pActiveItem != NULL && pActiveItem->GetItemState() == COleClientItem::activeUIState) {
		// need to set focus to this item if it is in the same view
		CWnd* pWnd = pActiveItem->GetInPlaceWindow ();
		if (pWnd != NULL) {
			pWnd->SetFocus ();   // don't call the base class
			return;
		}
	}
	inherited::OnSetFocus (pOldWnd);
}

void	LedItView::OnSize (UINT nType, int cx, int cy)
{
	inherited::OnSize (nType, cx, cy);

	// ajust any active OLE embeddings, as needed
	COleClientItem* pActiveItem = GetDocument ().GetInPlaceActiveItem (this);
	if (pActiveItem != NULL) {
		pActiveItem->SetItemRects ();
	}
}

void	LedItView::OnCancelEditSrvr ()
{
	GetDocument ().OnDeactivateUI (FALSE);
}

SimpleEmbeddedObjectStyleMarker*	LedItView::GetSoleSelectedEmbedding () const
{
	size_t	selStart	=	GetSelectionStart ();
	size_t	selEnd		=	GetSelectionEnd ();
	Led_Require (selStart <= selEnd);
	if (selEnd - selStart == 1) {
		vector<SimpleEmbeddedObjectStyleMarker*>	embeddings	=	CollectAllEmbeddingMarkersInRange (selStart, selEnd);
		if (embeddings.size () == 1) {
			return (dynamic_cast<SimpleEmbeddedObjectStyleMarker*> (embeddings[0]));
		}
		else {
			return NULL;
		}
	}
	else {
		return NULL;
	}
}

LedItControlItem*	LedItView::GetSoleSelectedOLEEmbedding () const
{
	return dynamic_cast<LedItControlItem*> (GetSoleSelectedEmbedding ());
}

void	LedItView::OnBarReturn (NMHDR*, LRESULT*)
{
	// When we return from doing stuff in format bar, reset focus to our edit view. Try it without and
	// you'll see how awkward it is...
	SetFocus ();
}

#ifdef _DEBUG
void	LedItView::AssertValid () const
{
	inherited::AssertValid();
}

void	LedItView::Dump (CDumpContext& dc) const
{
	inherited::Dump (dc);
}

LedItDocument&	LedItView::GetDocument () const// non-debug version is inline
{
	ASSERT (m_pDocument->IsKindOf(RUNTIME_CLASS(LedItDocument)));
	ASSERT_VALID (m_pDocument);
	return *(LedItDocument*)m_pDocument;
}
#endif //_DEBUG





// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
