/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItMFC/Sources/LedLineItView.cpp,v 2.67 2004/02/26 03:43:07 lewis Exp $
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
 *	$Log: LedLineItView.cpp,v $
 *	Revision 2.67  2004/02/26 03:43:07  lewis
 *	added qSupportGenRandomCombosCommand hack my dad can more easily generate a bunch of potential namess
 *	
 *	Revision 2.66  2004/02/11 23:58:06  lewis
 *	SPR#1576: Update: added ReplaceAllInSelection functionality (except must re-layout MacOS dialog)
 *	
 *	Revision 2.65  2004/02/11 22:41:39  lewis
 *	SPR#1576: update: added 'Replace Again' command to LedIt, LedLineItMFC, ActiveLedIt
 *	
 *	Revision 2.64  2004/01/26 14:27:44  lewis
 *	SPR#1623: SetControlArrowsScroll (true)
 *	
 *	Revision 2.63  2003/12/31 04:16:12  lewis
 *	change DisplayFindDialog/DisplayReplaceDialog functions for SPR#1580
 *	
 *	Revision 2.62  2003/12/11 03:09:18  lewis
 *	SPR#1585: move spellcheckengine object to application. Use qIncludeBasicSpellcheckEngine define to
 *	control its inclusion. Define default locaiton for LedLineIt spell dict.
 *	
 *	Revision 2.61  2003/11/27 03:03:40  lewis
 *	SPR#1567: gotoline should select the entire line
 *	
 *	Revision 2.60  2003/11/06 22:52:53  lewis
 *	SPR#1561: improved ShiftTab logic for indents (Simones request)
 *	
 *	Revision 2.59  2003/09/22 15:55:19  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.58  2003/06/03 12:31:14  lewis
 *	SPR#1513: hook in spellcheck engine
 *	
 *	Revision 2.57  2003/05/30 03:12:36  lewis
 *	SPR#1517: preliminary version of REPLACE DIALOG support (WIN32 only so far).
 *	
 *	Revision 2.56  2003/05/21 21:34:19  lewis
 *	SPR#1500: use UndoableContextHelper instead of direct PostInteractiveUndoHelper (ETC) methods
 *	
 *	Revision 2.55  2003/05/07 18:29:53  lewis
 *	SPR#1408: override SummarizeStyleMarkers (both overloads) to use StyleMarkerSummarySinkForSingleOwner so we
 *	avoid (innocuous) assert in StyleMarkerSummarySink::CombineElements
 *	
 *	Revision 2.54  2003/04/16 14:13:49  lewis
 *	SPR#1437: cosmetic cleanups - use new TextInteractor::BreakInGroupedCommands () wrapper
 *	
 *	Revision 2.53  2003/04/04 14:42:44  lewis
 *	SPR#1407: More work cleaning up new template-free command update/dispatch code
 *	
 *	Revision 2.52  2003/04/03 22:49:43  lewis
 *	SPR#1407: more work on getting this covnersion to new command handling working (mostly MacOS/Linux now)
 *	
 *	Revision 2.51  2003/04/03 21:53:25  lewis
 *	SPR#1407: more cleanups of new CommandNumberMapping<> stuff and other new command#
 *	processing support
 *	
 *	Revision 2.50  2003/04/03 16:54:27  lewis
 *	SPR#1407: First cut at major change in command-processing classes. Instead of using
 *	templated command classes, just builtin to TextInteractor/WordProcessor (and instead
 *	of template params use new TextInteractor/WordProcessor::DialogSupport etc)
 *	
 *	Revision 2.49  2003/04/01 16:15:38  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.48  2003/03/27 00:56:45  lewis
 *	SPR#1369 - CFontDialog CTOR takes CF_SCALABLEONLY now also to prevent bad font choices like 'Courier'.
 *	Also - hide the ScriptList popup item, since it makes no sense (at least for UNICODE - I believe).
 *	
 *	Revision 2.47  2003/01/23 17:04:39  lewis
 *	SPR#1256 - use new Led_StdDialogHelper_OtherFontSizeDialog
 *	
 *	Revision 2.46  2003/01/20 16:31:10  lewis
 *	minor tweeks noted by MWERKS CW8.3 compiler
 *	
 *	Revision 2.45  2002/10/30 22:46:08  lewis
 *	SPR#1158 - SetDefaultWindowMargins () API now uses Led_TWIPS
 *	
 *	Revision 2.44  2002/05/06 21:31:20  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.43  2001/11/27 00:28:23  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.42  2001/09/17 14:43:07  lewis
 *	SPR#1032- added SetDefaultFont dialog and saved preferences (pref for new docs)
 *	
 *	Revision 2.41  2001/09/12 11:32:48  lewis
 *	use secondary hilight on for MacOS as well
 *	
 *	Revision 2.40  2001/08/30 01:02:01  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.39  2001/04/19 16:12:19  lewis
 *	put back Courier as default font
 *	
 *	Revision 2.38  2001/04/18 21:04:43  lewis
 *	Courier new as default font
 *	
 *	Revision 2.37  2000/10/18 21:00:37  lewis
 *	Lose FindDialog code - and inherit new sahred implm in Led_StdDialogHelper_FindDialog etc
 *	
 *	Revision 2.36  2000/10/06 03:34:48  lewis
 *	SetScrollBarType instead of old SetHasScrollBar
 *	
 *	Revision 2.35  2000/10/04 13:20:22  lewis
 *	lose some code (undo handlers etc) obsoleted by recent code sharing changes
 *	
 *	Revision 2.34  2000/08/31 01:25:19  lewis
 *	get compiling with PostInteractiveUndoHelper change
 *	
 *	Revision 2.33  2000/06/13 16:30:04  lewis
 *	SPR#0767- use TWIPS for tabstops etc inernally in Led
 *	
 *	Revision 2.32  2000/06/08 21:49:49  lewis
 *	SPR#0774- added option to treat TAB as indent-selection command - as MSDEV does
 *	
 *	Revision 2.31  2000/06/08 20:44:31  lewis
 *	SPR#0772- fix LedItView::OnSearchAgain - so if caret just before word being
 *	searched for - we actualyl find it
 *	
 *	Revision 2.30  2000/04/04 15:13:21  lewis
 *	Use new Led_InstalledFonts class instead of manually walking font list
 *	
 *	Revision 2.29  2000/03/28 04:11:55  lewis
 *	fix SynaxColoring code to reacto to changes for SPR#0718
 *	
 *	Revision 2.28  2000/01/11 23:11:11  lewis
 *	use IsASCIISpace instead of directly using isspace - cuz doesn't do right thing for UNICODE
 *	
 *	Revision 2.27  1999/12/25 04:15:52  lewis
 *	Add MENU COMMANDS for the qSupportSyntaxColoring option. And debug it so it worked
 *	properly turning on/off and having multiple windows onto the same doc at the same
 *	time (scrolled to same or differnet) places
 *	
 *	Revision 2.26  1999/12/24 23:13:59  lewis
 *	Add (conditioanl on qSupportSyntaxColoring) new Sytnax coloring support. Based on old
 *	tutorial code - but greatly cleaned up and integrated into Led. Also - use new SetDefaultWindowMargins () code.
 *	
 *	Revision 2.25  1999/12/21 21:04:34  lewis
 *	adjust about box for UNICODE string. And react to cahnge in API for OnTypedNormalCharacter() method
 *	
 *	Revision 2.24  1999/12/09 17:35:27  lewis
 *	lots of cleanups/fixes for new (partial/full) UNICODE support - SPR#0645
 *	
 *	Revision 2.23  1999/12/09 03:29:07  lewis
 *	SPR#0645 - preliminary qSDK_UNICODE (_UNICODE) support (mostly using new Led_SDK_String/Led_SDK_Char)
 *	
 *	Revision 2.22  1999/05/03 22:24:21  lewis
 *	add CF_NOVERTFOTNS to LED CFOngDIalog
 *	
 *	Revision 2.21  1999/03/04 14:36:21  lewis
 *	no longer need to set sTmpHack textstore in LeditView::CTOR - so get rid of the hack
 *	
 *	Revision 2.20  1999/03/01 15:30:52  lewis
 *	misc cleanups and use new SetHScrollbarType ETC instead of setting WS_VSCROLL
 *	stypes in PreCreateWindow
 *	
 *	Revision 2.19  1999/02/22 13:12:04  lewis
 *	add Options module so we can preserve in registry flags like autoindent and smartcutandpaste
 *	
 *	Revision 2.18  1999/02/12 14:09:06  lewis
 *	cleanup
 *	
 *	Revision 2.17  1998/10/30 15:08:09  lewis
 *	lots of little cleanups - using vector instead of Led_Array, mutable instaed of
 *	const cast, new msvc60 warning
 *	stuff, etc.
 *	
 *	Revision 2.16  1998/04/25  17:39:32  lewis
 *	*** empty log message ***
 *
 *	Revision 2.15  1998/04/08  02:55:04  lewis
 *	*** empty log message ***
 *
 *	Revision 2.14  1998/03/04  20:29:54  lewis
 *	DIdUpdateText() invalidates fLayoutWidth (so autoscroll works horizontally).
 *
 *	Revision 2.13  1997/12/24  04:51:19  lewis
 *	*** empty log message ***
 *
 *	Revision 2.12  1997/12/24  04:27:33  lewis
 *	No longer any need to override OnCreate() - done in Led_MFC to setup droptarget.
 *
 *	Revision 2.11  1997/09/29  18:04:14  lewis
 *	Lose qLedFirstIndex.
 *
 *	Revision 2.10  1997/07/27  16:03:03  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.9  1997/07/23  23:21:45  lewis
 *	Lose uneeded OnSize() override
 *
 *	Revision 2.8  1997/07/14  14:59:39  lewis
 *	Renamed TextStore_ to TextStore.
 *
 *	Revision 2.7  1997/06/28  17:36:25  lewis
 *	Add OnInitialUpdate() to replace old gDocBeingCreated hack. Needed for MDI. But much cleaner anyhow.
 *
 *	Revision 2.6  1997/06/24  03:46:24  lewis
 *	Lose DynamiclyAdjustLayoutWidth () and instead override ComputeMaxHScrollPos ()
 *	as part of spr#0450.
 *
 *	Revision 2.5  1997/06/23  16:42:25  lewis
 *	Fix crasher bug / end condition problem with shiftright/left code.
 *
 *	Revision 2.4  1997/06/18  03:48:00  lewis
 *	qIncludePrefixFile and cleanups.
 *	And added shiftleft/shift right support.
 *
 *	Revision 2.3  1997/03/23  01:08:16  lewis
 *	Always use GotoLineDialog - don't jump to status bar edit field - Simone/Doug found
 *	that too confusing.
 *	DynamiclyAdjustLayoutWidth ()
 *	MSVC50 fixes
 *
 *	Revision 2.2  1997/03/04  20:21:24  lewis
 *	*** empty log message ***
 *
 *	Revision 2.1  1997/01/20  05:40:40  lewis
 *	Lose font style setting.
 *	Set default font to Courier 10.
 *	Set tabstops up to be exactly 4 characters wide (was 1/3 inch).
 *
 *	Revision 2.0  1997/01/10  03:38:30  lewis
 *	*** empty log message ***
 *
 *
 *
 *	<========== BasedOnLedIt! 2.1 ==========>
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

#include	"FontMenu.h"
#include	"LedLineItApplication.h"
#include	"LedLineItDocument.h"
#include	"LedLineItMainFrame.h"
#include	"Options.h"
#include	"Resource.h"

#include	"LedLineItView.h"




#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (disable : 4800)
#endif



class	My_CMDNUM_MAPPING : public MFC_CommandNumberMapping {
	public:
		My_CMDNUM_MAPPING ()
			{
				AddAssociation (ID_EDIT_UNDO,					LedLineItView::kUndo_CmdID);
				AddAssociation (ID_EDIT_REDO,					LedLineItView::kRedo_CmdID);
				AddAssociation (ID_EDIT_SELECT_ALL,				LedLineItView::kSelectAll_CmdID);
				AddAssociation (ID_EDIT_CUT,					LedLineItView::kCut_CmdID);
				AddAssociation (ID_EDIT_COPY,					LedLineItView::kCopy_CmdID);
				AddAssociation (ID_EDIT_PASTE,					LedLineItView::kPaste_CmdID);
				AddAssociation (ID_EDIT_CLEAR,					LedLineItView::kClear_CmdID);
				AddAssociation (kFindCmd,						LedLineItView::kFind_CmdID);
				AddAssociation (kFindAgainCmd,					LedLineItView::kFindAgain_CmdID);
				AddAssociation (kEnterFindStringCmd,			LedLineItView::kEnterFindString_CmdID);
				AddAssociation (kReplaceCmd,					LedLineItView::kReplace_CmdID);
				AddAssociation (kReplaceAgainCmd,				LedLineItView::kReplaceAgain_CmdID);
			#if		qIncludeBasicSpellcheckEngine
				AddAssociation (kSpellCheckCmd,					LedLineItView::kSpellCheck_CmdID);
			#endif
			}
};
My_CMDNUM_MAPPING	sMy_CMDNUM_MAPPING;


struct	LedLineIt_DialogSupport : TextInteractor::DialogSupport {
	public:
		LedLineIt_DialogSupport ()
			{
				TextInteractor::SetDialogSupport (this);
			}
		~LedLineIt_DialogSupport ()
			{
				TextInteractor::SetDialogSupport (NULL);
			}
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

};
static	LedLineIt_DialogSupport	sLedLineIt_DialogSupport;



inline	short	FontCmdToSize (UINT cmd)
	{
		switch (cmd) {
			case	kFontSize9CmdID:		return 9;
			case	kFontSize10CmdID:		return 10;
			case	kFontSize12CmdID:		return 12;
			case	kFontSize14CmdID:		return 14;
			case	kFontSize18CmdID:		return 18;
			case	kFontSize24CmdID:		return 24;
			case	kFontSize36CmdID:		return 36;
			case	kFontSize48CmdID:		return 48;
			case	kFontSize72CmdID:		return 72;
			case	kFontSizeOtherCmdID:	return 0;
			case	kFontSizeSmallerCmdID:	return 0;
			case	kFontSizeLargerCmdID:	return 0;
			default:						Led_Assert (false); return 0;
		}
	}
inline	bool	IsPredefinedFontSize (int size)
	{
		switch (size) {
			case	9:			return true;
			case	10:			return true;
			case	12:			return true;
			case	14:			return true;
			case	18:			return true;
			case	24:			return true;
			case	36:			return true;
			case	48:			return true;
			case	72:			return true;
			default:			return false;
		}
	}



inline	bool	IsASCIISpace (int c)
	{
		return isascii (c) and isspace (c);
	}














// Perhaps replace this with a user-configuration option in the options dialog?
const	unsigned int	kCharsPerTab	=	4;




const	Led_Distance	kBadDistance	=	Led_Distance (-1);




class	GotoLineDialog : public CDialog {
	public:
		GotoLineDialog (size_t origLine) :
			CDialog (kGotoLine_DialogID),
			fOrigLine (origLine),
			fResultLine (0)
			{
			}
		override	BOOL OnInitDialog ()
			{
				BOOL	result	=	CDialog::OnInitDialog();
				SetDlgItemInt (kGotoLine_Dialog_LineNumberEditFieldID, fOrigLine);
				return (result);
			}
		override	void	OnOK ()
			{
				BOOL	trans	=	false;
				fResultLine = GetDlgItemInt (kGotoLine_Dialog_LineNumberEditFieldID, &trans);
				if (not trans) {
					fResultLine = 0;
				}
				CDialog::OnOK ();
			}
	public:
		size_t	fOrigLine;
		size_t	fResultLine;

	protected:
		DECLARE_MESSAGE_MAP()
};
BEGIN_MESSAGE_MAP(GotoLineDialog, CDialog)
END_MESSAGE_MAP()




#if		qSupportGenRandomCombosCommand
namespace {
	struct	AtStartup {
		AtStartup ()
			{
				::srand ((unsigned)::time (NULL));
			}
	}	X_AtStartup;
}
#endif



/*
 ********************************************************************************
 ************************************ LedLineItView *****************************
 ********************************************************************************
 */
IMPLEMENT_DYNCREATE(LedLineItView, CView)

BEGIN_MESSAGE_MAP(LedLineItView, LedLineItView::inherited)

	ON_WM_SETFOCUS				()
	ON_WM_CONTEXTMENU			()
	ON_COMMAND					(ID_CANCEL_EDIT_CNTR,						OnCancelEditCntr)
	ON_COMMAND					(ID_CANCEL_EDIT_SRVR,						OnCancelEditSrvr)
	ON_COMMAND					(ID_FILE_PRINT,								OnFilePrint)
	ON_COMMAND					(ID_FILE_PRINT_DIRECT,						OnFilePrint)
	ON_COMMAND					(ID_FILE_PRINT_PREVIEW,						OnFilePrintPreview)

	ON_COMMAND					(kGotoLineCmdID,							OnGotoLineCommand)
	#if		qSupportGenRandomCombosCommand
	ON_COMMAND					(kGenRandomCombosCmdID,						OnGenRandomCombosCommand)
	#endif
	ON_COMMAND					(kShiftLeftCmdID,							OnShiftLeftCommand)
	ON_COMMAND					(kShiftRightCmdID,							OnShiftRightCommand)

	ON_COMMAND					(cmdChooseFontDialog,						OnChooseFontCommand)

	ON_UPDATE_COMMAND_UI_RANGE	(cmdFontMenuFirst,	cmdFontMenuLast,		OnUpdateFontNameChangeCommand)
	ON_COMMAND_RANGE			(cmdFontMenuFirst,	cmdFontMenuLast,		OnFontNameChangeCommand)

	ON_UPDATE_COMMAND_UI_RANGE	(kBaseFontSizeCmdID,	kLastFontSizeCmdID,	OnUpdateFontSizeChangeCommand)
	ON_COMMAND_RANGE			(kBaseFontSizeCmdID,	kLastFontSizeCmdID,	OnFontSizeChangeCommand)
END_MESSAGE_MAP()


LedLineItView::LedLineItView ():
	inherited (),
	fTabStopList (Led_TWIPS (1440/3)),
	fCachedLayoutWidth (kBadDistance)
#if		qSupportSyntaxColoring
	,fSyntaxColoringMarkerOwner (NULL)
#endif
{
	SetDefaultFont (Options ().GetDefaultNewDocFont ());

	SetSmartCutAndPasteMode (Options ().GetSmartCutAndPaste ());
	SetControlArrowsScroll (true);
	SetScrollBarType (h, eScrollBarAlways);
	SetScrollBarType (v, eScrollBarAlways);

	/*
	 *	No logic to picking these margins (I know of). Just picked them based on what looked good.
	 *	You can comment this out entirely to get the default (zero margins).
	 */
	const	Led_TWIPS	kLedItViewTopMargin		=	Led_TWIPS (60);
	const	Led_TWIPS	kLedItViewBottomMargin	=	Led_TWIPS (0);
	const	Led_TWIPS	kLedItViewLHSMargin		=	Led_TWIPS (60);
	const	Led_TWIPS	kLedItViewRHSMargin		=	Led_TWIPS (60);
#if		qWindows
	// This SHOULD be available on other platforms, but only now done for WIN32
	SetDefaultWindowMargins (Led_TWIPS_Rect (kLedItViewTopMargin, kLedItViewLHSMargin, kLedItViewBottomMargin-kLedItViewTopMargin, kLedItViewRHSMargin-kLedItViewLHSMargin));
#endif

#if		qMacOS || qWindows
	SetUseSecondaryHilight (true);
#endif
}

LedLineItView::~LedLineItView()
{
	SpecifyTextStore (NULL);
	SetCommandHandler (NULL);
	#if		qIncludeBasicSpellcheckEngine
		SetSpellCheckEngine (NULL);
	#endif
	#if		qSupportSyntaxColoring
		Led_Assert (fSyntaxColoringMarkerOwner == NULL);
	#endif
}

void	LedLineItView::OnInitialUpdate ()
{
	inherited::OnInitialUpdate ();
	SpecifyTextStore (&GetDocument ().GetTextStore ());
	SetCommandHandler (&GetDocument ().GetCommandHandler ());
	#if		qIncludeBasicSpellcheckEngine
		SetSpellCheckEngine (&LedLineItApplication::Get ().fSpellCheckEngine);
	#endif
}

#if		qSupportSyntaxColoring
void	LedLineItView::ResetSyntaxColoringTable ()
{
	if (PeekAtTextStore () != NULL) {
	//	static	const	TrivialRGBSyntaxAnalyzer			kAnalyzer;
		static	const	TableDrivenKeywordSyntaxAnalyzer	kCPlusPlusAnalyzer (TableDrivenKeywordSyntaxAnalyzer::kCPlusPlusKeywords);
		static	const	TableDrivenKeywordSyntaxAnalyzer	kVisualBasicAnalyzer (TableDrivenKeywordSyntaxAnalyzer::kVisualBasicKeywords);

		const SyntaxAnalyzer*	analyzer	=	NULL;
		switch (Options ().GetSyntaxColoringOption ()) {
			case	Options::eSyntaxColoringNone:		/* nothing - analyzer already NULL*/ break;
			case	Options::eSyntaxColoringCPlusPlus:	analyzer = &kCPlusPlusAnalyzer; break;
			case	Options::eSyntaxColoringVB:			analyzer = &kVisualBasicAnalyzer;  break;
		}

		delete fSyntaxColoringMarkerOwner;
		fSyntaxColoringMarkerOwner = NULL;

		if (analyzer != NULL) {
			#if		qSupportOnlyMarkersWhichOverlapVisibleRegion
				fSyntaxColoringMarkerOwner = new WindowedSyntaxColoringMarkerOwner (*this, GetTextStore (), *analyzer);
			#else
				fSyntaxColoringMarkerOwner = new SimpleSyntaxColoringMarkerOwner (*this, GetTextStore (), *analyzer);
			#endif
			fSyntaxColoringMarkerOwner->RecheckAll ();
		}
	}
}

void	LedLineItView::HookLosingTextStore ()
{
	delete fSyntaxColoringMarkerOwner;
	fSyntaxColoringMarkerOwner = NULL;
	inherited::HookLosingTextStore ();
}

void	LedLineItView::HookGainedNewTextStore ()
{
	Led_Assert (fSyntaxColoringMarkerOwner == NULL);
	inherited::HookGainedNewTextStore ();
	ResetSyntaxColoringTable ();
}
#endif

#if		qSupportSyntaxColoring
vector<LedLineItView::RunElement>	LedLineItView::SummarizeStyleMarkers (size_t from, size_t to) const
{
	// See SPR#1293 - may want to get rid of this eventually
	StyleMarkerSummarySinkForSingleOwner summary (*fSyntaxColoringMarkerOwner, from, to);
	GetTextStore ().CollectAllMarkersInRangeInto (from, to, TextStore::kAnyMarkerOwner, summary);
	return summary.ProduceOutputSummary ();
}

vector<LedLineItView::RunElement>	LedLineItView::SummarizeStyleMarkers (size_t from, size_t to, const TextLayoutBlock& text) const
{
	StyleMarkerSummarySinkForSingleOwner summary (*fSyntaxColoringMarkerOwner, from, to, text);
	GetTextStore ().CollectAllMarkersInRangeInto (from, to, TextStore::kAnyMarkerOwner, summary);
	return summary.ProduceOutputSummary ();
}
#endif

size_t	LedLineItView::GetCurUserLine () const
{
	// Either do caching here, or inside of GetRowContainingPosition () so this is quick.
	// Called ALOT - even for very large documents
	// LGP 970303
	return GetRowContainingPosition (GetSelectionStart ()) + 1;	// unclear if we should use selstart or selEnd - pick arbitrarily...
}

void	LedLineItView::SetCurUserLine (size_t newCurLine)
{
	// Assume line chosen 1 based, regardless of how Led is built. This is the most common UI.
	// And pin to ends if user choses too large / too small line number
	size_t	selPos	=	0;
	if (newCurLine >= 1) {
		if (newCurLine > GetRowCount ()) {
			newCurLine = GetRowCount ();
		}
		selPos = GetStartOfRow (newCurLine + (0-1));
	}
	size_t	endPos	=	min (GetRealEndOfRowContainingPosition (selPos), GetLength ());
	SetSelection (selPos, endPos);
	ScrollToSelection ();
}

const LedLineItView::TabStopList&	LedLineItView::GetTabStopList (size_t /*containingPos*/) const
{
	return fTabStopList;
}

void	LedLineItView::TabletChangedMetrics ()
{
	inherited::TabletChangedMetrics ();

	Tablet_Acquirer	tablet_ (this);
	Led_Tablet		tablet	=	tablet_;
	fTabStopList.fTWIPSPerTabStop = tablet->CvtToTWIPSH (kCharsPerTab * GetFontMetricsAt (0).GetMaxCharacterWidth ());
	fCachedLayoutWidth = kBadDistance;
	#if		qSupportSyntaxColoring
		if (fSyntaxColoringMarkerOwner != NULL) {
			fSyntaxColoringMarkerOwner->RecheckAll ();
		}
	#endif
}

void	LedLineItView::DidUpdateText (const UpdateInfo& updateInfo) throw ()
{
	inherited::DidUpdateText (updateInfo);
	fCachedLayoutWidth = kBadDistance;
}

void	LedLineItView::UpdateScrollBars ()
{
	// scrolling can change the longest row in window, so update our LayoutWidth
	fCachedLayoutWidth = kBadDistance;
	inherited::UpdateScrollBars ();
	#if		qSupportSyntaxColoring && qSupportOnlyMarkersWhichOverlapVisibleRegion
		if (fSyntaxColoringMarkerOwner != NULL) {
			fSyntaxColoringMarkerOwner->RecheckScrolling ();
		}
	#endif
}

Led_Distance	LedLineItView::ComputeMaxHScrollPos () const
{
	if (fCachedLayoutWidth == kBadDistance) {
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
		fCachedLayoutWidth = Led_Max (width, 1);
	}
	Led_Distance	wWidth	=	GetWindowRect ().GetWidth ();
	if (fCachedLayoutWidth > wWidth) {
		return (fCachedLayoutWidth - wWidth);
	}
	else {
		return 0;
	}
}

void	LedLineItView::OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed, bool controlPressed, bool altKeyPressed)
{
	if (theChar == '\t' and Options ().GetTreatTabAsIndentChar ()) {
		// Check if the selection looks like its likely an auto-indent situation...
		bool	shiftSituation	=	GetSelectionStart () != GetSelectionEnd ();
		if (shiftSituation) {
			// If we select an entire line, or multiple lines - then the user likely intended to auto-indent. But if the selection
			// is strictly INTRALINE - he probably intedned to replace the selected text with a tab.
			size_t	selStart	=	GetSelectionStart ();
			size_t	selEnd		=	GetSelectionEnd ();
			size_t	selStartRowStart	=	GetStartOfRowContainingPosition (selStart);
			size_t	selEndRowStart		=	GetStartOfRowContainingPosition (selEnd);
			size_t	selEndRowEnd		=	GetEndOfRowContainingPosition (selEnd);

			// The logic is simpler to understand this way - but terser to express in the negative - I choose clarity over brevity here...
			if (selStartRowStart != selEndRowStart) {
				// OK - in case of multiple rows - we shift
			}
			else if (selStartRowStart == selEndRowStart and selStart == selStartRowStart and selEnd == selEndRowEnd) {
				// if its a single line - but fully selected- then OK - we shift
			}
			else {
				// otherwise - its an intrarow selection - and we treat that as NOT a shift - but just a replace
				shiftSituation = false;
			}
		}

		if (shiftSituation) {
			if (shiftPressed) {
				OnShiftLeftCommand ();
			}
			else {
				OnShiftRightCommand ();
			}
			return;
		}
	}
	
	inherited::OnTypedNormalCharacter (theChar, optionPressed, shiftPressed, commandPressed, controlPressed, altKeyPressed);
	if (theChar == '\n' and GetSelectionStart () == GetSelectionEnd () and Options ().GetAutoIndent ()) {
		// Find the "indent level" of the previous line, and insert that much space here.
		size_t	rowStart		=	GetStartOfRowContainingPosition (GetSelectionEnd ());
		size_t	prevRowStart	=	GetStartOfPrevRowFromRowContainingPosition (GetSelectionEnd ());
		// Check first we're not on the first line - don't want to auto-indent there..
		if (rowStart != prevRowStart) {
			size_t	prevRowEnd	=	GetEndOfRowContainingPosition (prevRowStart);
			Led_Assert (prevRowEnd >= prevRowStart);
			size_t	prevRowLen	=	prevRowEnd-prevRowStart;
			Led_SmallStackBuffer<Led_tChar>	buf (prevRowLen);
			CopyOut (prevRowStart, prevRowLen, buf);
			size_t	nTChars	=	0;
			for (size_t i = 0; i < prevRowLen; ++i) {
				// use ANY space characters to auto-indent - should only use SPACE and TAB?
				if (not IsASCIISpace (buf[i])) {
					break;
				}
				nTChars++;
			}
			InteractiveReplace (buf, nTChars);
		}
	}
}

void	LedLineItView::OnContextMenu (CWnd* /*pWnd*/, CPoint pt) 
{
	CMenu menu;
	if (menu.LoadMenu (kContextMenu)) {
		CMenu*	popup = menu.GetSubMenu (0);
		Led_AssertNotNil (popup);
		popup->TrackPopupMenu (TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, ::AfxGetMainWnd ());
	}
}

BOOL	LedLineItView::IsSelected (const CObject* pDocItem) const
{
	// The implementation below is adequate if your selection consists of
	//  only LedLineItControlItem objects.  To handle different selection
	//  mechanisms, the implementation here should be replaced.

	// TODO: implement this function that tests for a selected OLE client item
	//return pDocItem == GetSoleSelectedOLEEmbedding ();
	BOOL	test	=	inherited::IsSelected (pDocItem);
	return false;// should I even need to override this? Probably NO!!!
}

void	LedLineItView::OnUpdateFontNameChangeCommand (CCmdUI* pCmdUI)
{
	Led_RequireNotNil (pCmdUI);

	// check the item iff it is the currently selected font.
	// But always enable them...
	Led_SDK_String	fontName	=	CmdNumToFontName (pCmdUI->m_nID);

	pCmdUI->SetCheck (fontName == GetDefaultFont ().GetFontName ());
	pCmdUI->Enable (true);
}

void	LedLineItView::OnFontNameChangeCommand (UINT cmdNum)
{
	Led_SDK_String	fontName	=	CmdNumToFontName (cmdNum);
	Led_IncrementalFontSpecification	applyFontSpec;
	applyFontSpec.SetFontName (fontName);
	SetDefaultFont (applyFontSpec);
}

void	LedLineItView::OnUpdateFontSizeChangeCommand (CCmdUI* pCmdUI)
{
	Led_RequireNotNil (pCmdUI);
	int	chosenFontSize	=	FontCmdToSize (pCmdUI->m_nID);

	if (chosenFontSize == 0) {
		switch (pCmdUI->m_nID) {
			case	kFontSizeSmallerCmdID:
			case	kFontSizeLargerCmdID: {
				pCmdUI->Enable (true);
			}
			break;

			case	kFontSizeOtherCmdID: {
				TCHAR	nameBuf[1024];
				_tcscpy (nameBuf, _T ("Other"));
				pCmdUI->SetCheck (false);
				{
					int	pointSize	=	GetDefaultFont ().GetPointSize ();
					if (not IsPredefinedFontSize (pointSize)) {
						(void)::_tcscat (nameBuf, _T (" ("));
						TCHAR	nBuf[100];
						_stprintf (nBuf, _T ("%d"), GetDefaultFont ().GetPointSize ());
						(void)::_tcscat (nameBuf, nBuf);
						(void)::_tcscat (nameBuf, _T (")"));
						pCmdUI->SetCheck (true);
					}
				}
				(void)::_tcscat (nameBuf, _T ("..."));
				pCmdUI->SetText (nameBuf);
			}
			break;
		}
	}
	else {
		pCmdUI->SetCheck (GetDefaultFont ().GetPointSize () == chosenFontSize);
		pCmdUI->Enable (true);
	}
}

void	LedLineItView::OnFontSizeChangeCommand (UINT cmdNum)
{
	int	chosenFontSize	=	FontCmdToSize (cmdNum);
	if (chosenFontSize == 0) {
		switch (cmdNum) {
			case	kFontSizeSmallerCmdID: {
				Led_IncrementalFontSpecification	applyFontSpec;
				applyFontSpec.SetPointSizeIncrement (-1);
				SetDefaultFont (applyFontSpec);
				return;
			}
			break;
			case	kFontSizeLargerCmdID: {
				Led_IncrementalFontSpecification	applyFontSpec;
				applyFontSpec.SetPointSizeIncrement (1);
				SetDefaultFont (applyFontSpec);
				return;
			}
			break;
			case	kFontSizeOtherCmdID: {
				Led_Distance	oldSize	=	GetDefaultFont ().GetPointSize ();
				chosenFontSize = PickOtherFontHeight (oldSize);
			}
			break;
		}
	}
	if (chosenFontSize != 0) {
		Led_IncrementalFontSpecification	applyFontSpec;
		applyFontSpec.SetPointSize (chosenFontSize);
		SetDefaultFont (applyFontSpec);
	}
}

Led_Distance	LedLineItView::PickOtherFontHeight (Led_Distance origHeight)
{
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

LedLineItView::SearchParameters	LedLineItView::GetSearchParameters () const
{
	return Options ().GetSearchParameters ();
}

void	LedLineItView::SetSearchParameters (const SearchParameters& sp)
{
	Options ().SetSearchParameters (sp);
}

void	LedLineItView::OnGotoLineCommand ()
{
	/*
	 *	compute current line, and use that as the default for the dialog box. Put up the dlog box,
	 *	prompting user. Then move the selection to that line (and scroll to selection).
	 */
	size_t	rowNumber	=	GetCurUserLine ();

	/*
	 *	I had coded this before to check if the stats bar was shown, and treat this
	 *	as jumping the focus to the status bar. But Simone and Doug barfed violently
	 *	on this idea. So lets me more traditional.
	 *		--	LGP 970315
	 */
	GotoLineDialog	dlg (rowNumber);

	if (dlg.DoModal () == IDOK) {
		SetCurUserLine (dlg.fResultLine);
	}
}

#if		qSupportGenRandomCombosCommand
void	LedLineItView::OnGenRandomCombosCommand ()
{
	vector<Led_tString>	srcFrags;

	{
		size_t	lastStart	=	static_cast<size_t> (-1);
		for (size_t i = GetStartOfRowContainingPosition (GetSelectionStart ());
			 i < GetSelectionEnd () and lastStart != i;
			 lastStart = i, i = GetStartOfNextRowFromRowContainingPosition (i)
			) {
			size_t	rowEnd	=	GetEndOfRowContainingPosition (i);
			size_t	rowLen	=	rowEnd-i;
			Led_SmallStackBuffer<Led_tChar> buf (rowLen + 1);
			CopyOut (i, rowLen, buf);
			buf[rowLen] = '\0';
			srcFrags.push_back (static_cast<Led_tString> (buf));
		}
	}

	const	unsigned	kGeneratedCount	=	100;
	const	unsigned	kMaxComboLen	=	4;
	set<Led_tString>	resultFrags;
	if (srcFrags.size () != 0) {
		for (size_t i = 0; i < kGeneratedCount; ++i) {
			unsigned int	n	=	(rand () % kMaxComboLen) + 1;
			Led_tString		tmpWord;
			for (size_t ni = 0; ni < n; ++ni) {
				// now pick a random srcWord
				unsigned int	wi	=	(rand () % srcFrags.size ());
				tmpWord += srcFrags[wi];
			}
			resultFrags.insert (tmpWord);
		}
	}

	CDocManager*	docMgr	=	AfxGetApp ()->m_pDocManager;
	Led_AssertNotNil (docMgr);
	CDocTemplate*	pTemplate = NULL;
	{
		POSITION	p	=	docMgr->GetFirstDocTemplatePosition ();
		pTemplate = docMgr->GetNextDocTemplate (p);
	}
	Led_AssertNotNil (pTemplate);
	CDocument*		newDoc	=	pTemplate->OpenDocumentFile (NULL);
	newDoc->SetTitle (newDoc->GetTitle () + Led_SDK_TCHAROF (" {generated-words}"));
	LedLineItDocument*	lNewDoc	=	dynamic_cast<LedLineItDocument*> (newDoc);
	Led_AssertNotNil (lNewDoc);
	TextStore&	ts	=	lNewDoc->GetTextStore ();
	for (set<Led_tString>::const_iterator i = resultFrags.begin (); i != resultFrags.end (); ++i) {
		ts.Replace (ts.GetEnd (), ts.GetEnd (), (*i).c_str (), (*i).length ());
		ts.Replace (ts.GetEnd (), ts.GetEnd (), LED_TCHAR_OF ("\n"), 1);
	}
}
#endif

void	LedLineItView::OnShiftLeftCommand ()
{
	OnShiftNCommand (false);
}

void	LedLineItView::OnShiftRightCommand ()
{
	OnShiftNCommand (true);
}

void	LedLineItView::OnShiftNCommand (bool shiftRight)
{
	BreakInGroupedCommands ();

	// Find the entire set of rows which encompass the selection
	PartitionMarker*	firstPM	=		GetPartitionMarkerContainingPosition (GetSelectionStart ());
	PartitionMarker*	lastPM	=		GetPartitionMarkerContainingPosition (GetSelectionEnd ());
	Led_AssertNotNil (firstPM);
	Led_AssertNotNil (lastPM);
	if (lastPM != firstPM and lastPM->GetStart () == GetSelectionEnd ()) {
		lastPM = lastPM->GetPrevious ();
		Led_AssertNotNil (lastPM);
	}

	TextInteractor::UndoableContextHelper	undoContext (
														*this,
														shiftRight? Led_SDK_TCHAROF ("Shift Right"): Led_SDK_TCHAROF ("Shift Left"),
														firstPM->GetStart (), min (lastPM->GetEnd (), GetTextStore ().GetEnd ()),
														GetSelectionStart (), GetSelectionEnd (),
														false
												);
		{
			SetSelection (firstPM->GetStart (), min (lastPM->GetEnd (), GetTextStore ().GetEnd ()));

			// Go through each PM, and either shift it right or left.
			for (PartitionMarker* pm = firstPM; pm != lastPM->GetNext (); pm = pm->GetNext ()) {
				if (shiftRight) {
					InteractiveReplace_ (pm->GetStart (), pm->GetStart (), LED_TCHAR_OF ("\t"), 1, false, false, eDefaultUpdate);
				}
				else {
					size_t	pmLength	=	pm->GetLength ();
					size_t	lookAtLength = min (pmLength-1, kCharsPerTab);
					Led_SmallStackBuffer<Led_tChar>	buf (lookAtLength);
					CopyOut (pm->GetStart (), lookAtLength, buf);
					size_t	deleteLength	=	lookAtLength;	// default to deleting all if all whitespace..
					for (size_t i = 0; i < lookAtLength; ++i) {
						if (buf[i] == '\t') {
							deleteLength = i+1;
							break;
						}
						if (not (IsASCIISpace (buf[i]))) {
							deleteLength = i;
							break;
						}
					}
					
					// delete the first lookAtLength characters.
					InteractiveReplace_ (pm->GetStart (), pm->GetStart () + deleteLength, LED_TCHAR_OF (""), 0, false, false, eDefaultUpdate);
				}
			}

			// Select the entire set of rows updated.
			SetSelection (firstPM->GetStart (), min (lastPM->GetEnd (), GetTextStore ().GetEnd ()));
		}
	undoContext.CommandComplete ();

	BreakInGroupedCommands ();
}

// The following command handler provides the standard keyboard
//  user interface to cancel an in-place editing session.  Here,
//  the container (not the server) causes the deactivation.
void	LedLineItView::OnCancelEditCntr ()
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
void	LedLineItView::OnSetFocus (CWnd* pOldWnd)
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

void	LedLineItView::OnCancelEditSrvr ()
{
	GetDocument ().OnDeactivateUI (FALSE);
}

void	LedLineItView::OnSelectAllCommand ()
{
	SetSelection (0, GetLength ());
}

void	LedLineItView::OnChooseFontCommand ()
{
	// Copy each valid attribute into the LOGFONT to initialize the CFontDialog
	LOGFONT	lf;
	(void)::memset (&lf, 0, sizeof (lf));
	{
		(void)::_tcscpy (lf.lfFaceName, GetDefaultFont ().GetFontNameSpecifier ().fName);
		Led_Assert (::_tcslen (lf.lfFaceName) < sizeof (lf.lfFaceName));	// cuz our cached entry - if valid - always short enuf...
	}
	lf.lfWeight = (GetDefaultFont ().GetStyle_Bold ())? FW_BOLD: FW_NORMAL;
	lf.lfItalic = (GetDefaultFont ().GetStyle_Italic ());
	lf.lfUnderline = (GetDefaultFont ().GetStyle_Underline ());
	lf.lfStrikeOut = (GetDefaultFont ().GetStyle_Strikeout ());

	lf.lfHeight = GetDefaultFont ().PeekAtTMHeight ();

	FontDlgWithNoColorNoStyles	dlog (&lf);
	if (dlog.DoModal () == IDOK) {
		SetDefaultFont (Led_FontSpecification (*dlog.m_cf.lpLogFont));
	}
}

#ifdef _DEBUG
void	LedLineItView::AssertValid () const
{
	inherited::AssertValid();
}

void	LedLineItView::Dump(CDumpContext& dc) const
{
	inherited::Dump(dc);
}

LedLineItDocument&	LedLineItView::GetDocument () const// non-debug version is inline
{
	ASSERT (m_pDocument->IsKindOf(RUNTIME_CLASS(LedLineItDocument)));
	ASSERT_VALID (m_pDocument);
	return *(LedLineItDocument*)m_pDocument;
}
#endif //_DEBUG






/*
 ********************************************************************************
 ************************* FontDlgWithNoColorNoStyles ***************************
 ********************************************************************************
 */
FontDlgWithNoColorNoStyles::FontDlgWithNoColorNoStyles (LOGFONT* lf):
	CFontDialog (lf, CF_SCREENFONTS | CF_SCALABLEONLY | CF_NOVERTFONTS)
{
	//  SPR#1369 : Either CF_SCALABLEONLY or CF_TTONLY seems to get rid of bad fonts that don't work right with tabs
}

BOOL	FontDlgWithNoColorNoStyles::OnInitDialog ()
{
	BOOL	result	=	CFontDialog::OnInitDialog ();

	// hide the widgets for the font-style choices
	// Gee - I hope Microslop doesn't change these values!!! LGP 970118
	
	// Font STYLE list LABEL
	if (GetDlgItem (stc2) != NULL) {
		GetDlgItem (stc2)->ShowWindow (SW_HIDE);
	}
	// Font STYLE list
	if (GetDlgItem (cmb2) != NULL) {
		GetDlgItem (cmb2)->ShowWindow (SW_HIDE);
	}

	// Script list LABEL
	if (GetDlgItem (stc7) != NULL) {
		GetDlgItem (stc7)->ShowWindow (SW_HIDE);
	}
	// Script list
	if (GetDlgItem (cmb5) != NULL) {
		GetDlgItem (cmb5)->ShowWindow (SW_HIDE);
	}
	return result;
}


// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
