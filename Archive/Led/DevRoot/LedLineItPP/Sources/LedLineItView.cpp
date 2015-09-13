/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItPP/Sources/LedLineItView.cpp,v 2.44 2004/02/12 20:58:02 lewis Exp $
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
 *	Revision 2.44  2004/02/12 20:58:02  lewis
 *	SPR#1576: replaceall in selection supprot for MacOS LedLineIt
 *	
 *	Revision 2.43  2003/12/31 16:31:23  lewis
 *	small changes to get compiling with recent SearchParam changes for find dialog/replace dialog
 *	
 *	Revision 2.42  2003/06/04 15:47:25  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.41  2003/06/04 15:45:59  lewis
 *	SPR#1513: spellcheck engine support (and some other cleanups)
 *	
 *	Revision 2.40  2003/06/04 03:08:27  lewis
 *	SPR#1513: spellcheck engine support
 *	
 *	Revision 2.39  2003/05/30 15:55:47  lewis
 *	SPR#1417: fix replace cmd support
 *	
 *	Revision 2.38  2003/05/30 14:43:33  lewis
 *	SPR#1517: MacOS replace dialog support
 *	
 *	Revision 2.37  2003/05/22 12:39:56  lewis
 *	small tweeks to get compiling again on Mac/MWERKS
 *	
 *	Revision 2.36  2003/05/21 21:34:31  lewis
 *	SPR#1500: use UndoableContextHelper instead of direct PostInteractiveUndoHelper (ETC) methods
 *	
 *	Revision 2.35  2003/04/16 14:14:01  lewis
 *	SPR#1437: cosmetic cleanups - use new TextInteractor::BreakInGroupedCommands () wrapper
 *	
 *	Revision 2.34  2003/04/04 14:47:50  lewis
 *	SPR#1407: More work cleaning up new template-free command update/dispatch code
 *	
 *	Revision 2.33  2003/04/03 22:50:03  lewis
 *	SPR#1407: more work on getting this covnersion to new command handling working (mostly MacOS/Linux now)
 *	
 *	Revision 2.32  2003/04/03 21:53:41  lewis
 *	SPR#1407: more cleanups of new CommandNumberMapping<> stuff and other new command#
 *	processing support
 *	
 *	Revision 2.31  2003/04/03 16:54:37  lewis
 *	SPR#1407: First cut at major change in command-processing classes. Instead of using
 *	templated command classes, just builtin to TextInteractor/WordProcessor (and instead
 *	of template params use new TextInteractor/WordProcessor::DialogSupport etc)
 *	
 *	Revision 2.30  2003/03/11 19:37:50  lewis
 *	SPR#1249 - get compiling with UNICODE/Carbon
 *	
 *	Revision 2.29  2003/01/23 17:10:01  lewis
 *	SPR#1256 - use new Led_StdDialogHelper_OtherFontSizeDialog
 *	
 *	Revision 2.28  2002/05/06 21:31:29  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.27  2001/11/27 00:28:29  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.26  2001/08/30 01:01:08  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.25  2001/07/19 21:06:20  lewis
 *	SPR#0960- CW6Pro support
 *	
 *	Revision 2.24  2000/10/19 15:51:54  lewis
 *	use new portable About code and Find code. Further cleanups/improvements.
 *	
 *	Revision 2.23  2000/10/06 03:34:36  lewis
 *	SetScrollBarType instead of old SetHasScrollBar
 *	
 *	Revision 2.22  2000/10/03 21:52:01  lewis
 *	Lots more work relating to SPR#0839. Eliminated a little duplicated code (undo stuff now
 *	handled genericly via TextInteractorCommandHelper<> template
 *	
 *	Revision 2.21  2000/08/31 23:10:05  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.20  2000/06/13 16:31:46  lewis
 *	SPR#0767- use TWIPS for tabstops etc inernally in Led
 *	
 *	Revision 2.19  2000/06/08 20:46:58  lewis
 *	SPR#0772- fix LedItView::OnSearchAgain - so if caret just before word being searched for -
 *	we actualyl find it
 *	
 *	Revision 2.18  1999/12/24 23:23:53  lewis
 *	add PreLim (qSupportSyntaxColoring) support.
 *	
 *	Revision 2.17  1999/12/21 21:05:49  lewis
 *	react to change in OnTypedNormalCharacter API
 *	
 *	Revision 2.16  1999/11/29 21:33:47  lewis
 *	fix so compiles with new MWERKS Compiler
 *	
 *	Revision 2.15  1998/10/30 20:24:55  lewis
 *	Use vector<> instead of Led_Array<>
 *	
 *	Revision 2.14  1998/10/30  15:10:35  lewis
 *	use mutable instead of const_cast
 *
 *	Revision 2.13  1998/04/08  02:57:47  lewis
 *	*** empty log message ***
 *
 *	Revision 2.12  1998/03/04  20:32:25  lewis
 *	DidUpdateText() override to set fLayoutWidth to kBadDIstance, so autoscrolling horizontally
 *	works right.
 *
 *	Revision 2.11  1997/12/24  04:52:55  lewis
 *	*** empty log message ***
 *
 *	Revision 2.10  1997/12/24  04:01:28  lewis
 *	compat with CWPro2 compiler
 *
 *	Revision 2.9  1997/09/29  18:07:56  lewis
 *	Lose qLedFirstIndex.
 *
 *	Revision 2.8  1997/07/27  16:03:37  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.7  1997/07/14  14:45:32  lewis
 *	Renamed TextStore_ to TextStore.
 *
 *	Revision 2.6  1997/06/24  03:40:25  lewis
 *	Lose DynamiclyAdjustLayoutWidth () and instead override ComputeMaxHScrollPos ()
 *	for this purpose. No longer SetLayoutWidth () - no such routine anymore.
 *	See SPR#0450.
 *
 *	Revision 2.5  1997/06/23  16:23:25  lewis
 *	Fix shift-right/left code end-condition.
 *
 *	Revision 2.4  1997/06/18  03:36:47  lewis
 *	Support shiftleft/shiftright.
 *	Cleanups.
 *	Revised tab support in Led.
 *
 *	Revision 2.3  1997/03/23  00:59:49  lewis
 *	redo algorithm for DynamiclyAdjustLayoutWidth () (slightly)
 *
 *	Revision 2.2  1997/03/04  20:27:41  lewis
 *	*** empty log message ***
 *
 *	Revision 2.1  1997/01/20  05:47:10  lewis
 *	Lose font style support.
 *	Adjust layout width on scrolling so max hscbar value set depending on what shown in window.
 *	override TabletChangedMetrics () so we can set tablstop to be 4 chars wide (best for
 *	programming editing). Was 1/3 inch. 4 chars works best (and only really defined)
 *	for monospace font.
 *	Set default font to Monaco 9.
 *
 *	Revision 2.0  1997/01/10  03:51:52  lewis
 *	*** empty log message ***
 *
 *
 *
 *	<========== BasedOnLedIt! 2.1 ==========>
 *
 *
 */
#include	<cctype>
#include	<cstdlib>

#include	<AERegistry.h>
#include	<Dialogs.h>
#include	<TextUtils.h>

#include	<PP_Messages.h>
#include	<LMenu.h>
#include	<LMenuBar.h>
#include	<LEditField.h>
#include	<LStdControl.h>
#include	<UAppleEventsMgr.h>
#include	<UModalDialogs.h>
#include	<LDialogBox.h>

#include	"LedStdDialogs.h"

#include	"LedLineItResources.h"
#include	"LedLineItApplication.h"

#include	"LedLineItView.h"






class	My_CMDNUM_MAPPING : public PP_CommandNumberMapping {
	public:
		My_CMDNUM_MAPPING ()
			{
				AddAssociation (cmd_Undo,						LedLineItView::kUndo_CmdID);
				AddAssociation (cmdRedo,						LedLineItView::kRedo_CmdID);
				AddAssociation (cmd_SelectAll,					LedLineItView::kSelectAll_CmdID);
				AddAssociation (cmd_Cut,						LedLineItView::kCut_CmdID);
				AddAssociation (cmd_Copy,						LedLineItView::kCopy_CmdID);
				AddAssociation (cmd_Paste,						LedLineItView::kPaste_CmdID);
				AddAssociation (cmd_Clear,						LedLineItView::kClear_CmdID);
				AddAssociation (cmdFind,						LedLineItView::kFind_CmdID);
				AddAssociation (cmdFindAgain,					LedLineItView::kFindAgain_CmdID);
				AddAssociation (cmdEnterFindString,				LedLineItView::kEnterFindString_CmdID);
				AddAssociation (kReplaceCmd,					LedLineItView::kReplace_CmdID);
				AddAssociation (kReplaceAgainCmd,				LedLineItView::kReplaceAgain_CmdID);
				AddAssociation (kSpellCheckCmd,					LedLineItView::kSpellCheck_CmdID);
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
		override	void	DisplayFindDialog (Led_tString* findText, const vector<Led_tString>& /*recentFindSuggestions*/, bool* wrapSearch, bool* wholeWordSearch, bool* caseSensative, bool* pressedOK)
			{
				Led_StdDialogHelper_FindDialog	findDialog;

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
#endif
#if		qSupportStdReplaceDlg
	public:
		override	ReplaceButtonPressed	DisplayReplaceDialog (Led_tString* findText, const vector<Led_tString>& /*recentFindSuggestions*/, Led_tString* replaceText, bool* wrapSearch, bool* wholeWordSearch, bool* caseSensative)
			{
				Led_StdDialogHelper_ReplaceDialog	replaceDialog;

				replaceDialog.fFindText = *findText;
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







inline	short	FontCmdToFontID (CommandT cmd)
	{
		static	LMenu*	fontMenu	=	LMenuBar::GetCurrentMenuBar()->FetchMenu (cmd_FontMenu);
		static	vector<short>	sFontIDMapCache;	// OK to keep static cuz never changes during run of app

		Led_Require (cmd >= kBaseFontNameCmdID);
		Led_Require (cmd <= kLastFontNameCmdID);
		size_t	idx	=	cmd-kBaseFontNameCmdID;

		// Pre-fill cache - at least to the cmd were looking for...
		for (size_t i = sFontIDMapCache.size (); i <= idx; i++) {
			Str255		pFontName = {0};
			SInt16		menuItem		=	fontMenu->IndexFromCommand (i+kBaseFontNameCmdID);
			::GetMenuItemText (fontMenu->GetMacMenuH (), menuItem, pFontName);
			short	familyID	=	0;
			::GetFNum (pFontName, &familyID);
			sFontIDMapCache.push_back (familyID);
		}
		return sFontIDMapCache[idx];
	}

inline	short	FontCmdToSize (CommandT cmd)
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
const	int	kCharsPerTab	=	4;



const	Led_Distance	kBadDistance	=	Led_Distance (-1);






/*
 ********************************************************************************
 ********************************* LedLineItView ********************************
 ********************************************************************************
 */
LedLineItView::LedLineItView ():
	inherited (),
	fTabStopList (Led_TWIPS (1440/3)),
	fLayoutWidth (kBadDistance)
#if		qSupportSyntaxColoring
	,fSyntaxColoringMarkerOwner (NULL)
#endif
{
	// A good default font for LedLineIt - since its oriented towards programmer/type
	// text editing... is a monospace font...
	Led_FontSpecification	fsp	=	GetDefaultFont ();
	fsp.SetFontNameSpecifier (kFontIDMonaco);
	fsp.SetPointSize (9);
	SetDefaultFont (fsp);

	SetScrollBarType (h, eScrollBarAlways);
	SetScrollBarType (v, eScrollBarAlways);
	
	SetSmartCutAndPasteMode (qSupportSmartCutNPaste);
}

LedLineItView::~LedLineItView ()
{
	SetCommandHandler (NULL);
	SpecifyTextStore (NULL);
	SetSpellCheckEngine (NULL);
	#if		qSupportSyntaxColoring
		Led_Assert (fSyntaxColoringMarkerOwner == NULL);
	#endif
}

#if		qSupportSyntaxColoring
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
//	static	const	TrivialRGBSyntaxAnalyzer			kAnalyzer;
	static	const	TableDrivenKeywordSyntaxAnalyzer	kAnalyzer (TableDrivenKeywordSyntaxAnalyzer::kCPlusPlusKeywords);
//	static	const	TableDrivenKeywordSyntaxAnalyzer	kAnalyzer (TableDrivenKeywordSyntaxAnalyzer::kVisualBasicKeywords);
	#if		qSupportOnlyMarkersWhichOverlapVisibleRegion
		fSyntaxColoringMarkerOwner = new WindowedSyntaxColoringMarkerOwner (*this, GetTextStore (), kAnalyzer);
	#else
		fSyntaxColoringMarkerOwner = new SimpleSyntaxColoringMarkerOwner (*this, GetTextStore (), kAnalyzer);
	#endif
}
#endif

void	LedLineItView::OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed, bool controlPressed, bool altKeyPressed)
{
	inherited::OnTypedNormalCharacter (theChar, optionPressed, shiftPressed, commandPressed, controlPressed, altKeyPressed);
	#if		qSupportAutoIndent
	if (theChar == '\n' and GetSelectionStart () == GetSelectionEnd ()) {
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
				if (not isspace (buf[i])) {
					break;
				}
				nTChars++;
			}
			InteractiveReplace (buf, nTChars);
		}
	}
	#endif
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
	fLayoutWidth = kBadDistance;
	#if		qSupportSyntaxColoring
		if (fSyntaxColoringMarkerOwner != NULL) {
			fSyntaxColoringMarkerOwner->RecheckAll ();
		}
	#endif
}

void	LedLineItView::DidUpdateText (const UpdateInfo& updateInfo) throw ()
{
	inherited::DidUpdateText (updateInfo);
	fLayoutWidth = kBadDistance;
}

void	LedLineItView::UpdateScrollBars ()
{
	// scrolling can change the longest row in window, so update our LayoutWidth
	fLayoutWidth = kBadDistance;
	inherited::UpdateScrollBars ();
	#if		qSupportSyntaxColoring && qSupportOnlyMarkersWhichOverlapVisibleRegion
		Led_AssertNotNil (fSyntaxColoringMarkerOwner);
		fSyntaxColoringMarkerOwner->RecheckScrolling ();
	#endif
}

Led_Distance	LedLineItView::ComputeMaxHScrollPos () const
{
	if (fLayoutWidth == kBadDistance) {
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
		fLayoutWidth = Led_Max (width, 1);
	}
	Led_Distance	wWidth	=	GetWindowRect ().GetWidth ();
	if (fLayoutWidth > wWidth) {
		return (fLayoutWidth - wWidth);
	}
	else {
		return 0;
	}
}

void	LedLineItView::FindCommandStatus (CommandT inCommand, Boolean& outEnabled, Boolean& outUsesMark, UInt16& outMark, Str255 outName)
{
	outUsesMark = false;
	switch (inCommand) {
		case	kGotoLineCmdID:		outEnabled = true; 											break;
		case	kShiftLeftCmdID:	outEnabled = true;											break;
		case	kShiftRightCmdID:	outEnabled = true;											break;
		case	cmd_FontMenu:		outEnabled = true;											break;
		case	cmd_SizeMenu:		outEnabled = true;											break;
		case	cmd_StyleMenu:		outEnabled = true;											break;

		default: {
			if (FindCommandStatus_Font (inCommand, outEnabled, outUsesMark, outMark, outName)) {
				return;
			}
			inherited::FindCommandStatus (inCommand, outEnabled, outUsesMark, outMark, outName);
		}
		break;
	}
}

Boolean	LedLineItView::ObeyCommand (CommandT inCommand, void* ioParam)
{
	switch (inCommand) {
		case kGotoLineCmdID:		OnGotoLineCommand ();			return true;
		case kShiftLeftCmdID:		OnShiftLeftCommand ();			return true;
		case kShiftRightCmdID:		OnShiftRightCommand ();			return true;

		default: {
			if (ObeyCommand_Font (inCommand, ioParam)) {
				return true;
			}
			return inherited::ObeyCommand (inCommand, ioParam);
		}
	}

	Led_Assert (false);	return false;
}

bool	LedLineItView::FindCommandStatus_Font (CommandT inCommand, Boolean& outEnabled, Boolean& outUsesMark, UInt16& outMark, Str255 outName)
{
	Led_FontSpecification	fontSpec	=	GetDefaultFont ();
	if ((inCommand >= kBaseFontNameCmdID) and (inCommand <= kLastFontNameCmdID)) {
		short	familyID	=	FontCmdToFontID (inCommand);
		outMark = (familyID == fontSpec.GetFontNameSpecifier ())? checkMark: 0;
		goto handled;
	}

	if ((inCommand >= kBaseFontSizeCmdID) and (inCommand <= kLastFontSizeCmdID)) {
		int	chosenFontSize	=	FontCmdToSize (inCommand);
		if (chosenFontSize == 0) {
			switch (inCommand) {
				case	kFontSizeOtherCmdID: {
					char	nameBuf[1024];
					strcpy (nameBuf, "Other");
					outMark = 0;
					{
						int	pointSize	=	fontSpec.GetPointSize ();
						if (not IsPredefinedFontSize (pointSize)) {
							strcat (nameBuf, " (");
							Str255	nBuf;
							::NumToString (fontSpec.GetPointSize (), nBuf);
							size_t	len	=	strlen (nameBuf);
							memcpy (&nameBuf[len], &nBuf[1], nBuf[0]);
							len += nBuf[0];
							nameBuf[len] = '\0';
							strcat (nameBuf, ")");
							outMark = checkMark;
						}
					}
					strcat (nameBuf, "É");
					outName[0] = strlen (nameBuf);
					memcpy (&outName[1], nameBuf, outName[0]);
				}
				break;
			}
		}
		else {
			outMark = (fontSpec.GetPointSize () == chosenFontSize)? checkMark: 0;
			#if 0
	// Consider some hack like this to outline the font sizes we have bitmap fonts for...
				static	LMenu*	fontSizeMenu	=	LMenuBar::GetCurrentMenuBar()->FetchMenu (cmd_SizeMenu);
				if (fontSpec.GetFontNameSpecifier_Valid ()) {
					::SetItemStyle (fontSizeMenu, 1, (::RealFont (fontSpec.fFontID, 9) ? outline : normal)); 
				}
				else {
					::SetItemStyle (fontSizeMenu, 1, normal);
				}
			#endif
		}

		goto handled;
	}

	return false;

handled:
	outEnabled = true;
	outUsesMark = true;
	return true;
}

bool	LedLineItView::ObeyCommand_Font (CommandT inCommand, void* /*ioParam*/)
{
	Led_FontSpecification	fontSpec	=	GetDefaultFont ();
	Led_IncrementalFontSpecification	applyFontSpec;
	if ((inCommand >= kBaseFontNameCmdID) and (inCommand <= kLastFontNameCmdID)) {
		short	familyID	=	FontCmdToFontID (inCommand);
		applyFontSpec.SetFontNameSpecifier (familyID);
		goto handled;
	}
	if ((inCommand >= kBaseFontSizeCmdID) and (inCommand <= kLastFontSizeCmdID)) {
		int	chosenFontSize	=	FontCmdToSize (inCommand);
		if (chosenFontSize == 0) {
			switch (inCommand) {
				case	kFontSizeSmallerCmdID: {
					applyFontSpec.SetPointSizeIncrement (-1);
					goto handled;
				}
				break;
				case	kFontSizeLargerCmdID: {
					applyFontSpec.SetPointSizeIncrement (1);
					goto handled;
				}
				break;
				case	kFontSizeOtherCmdID: {
					Led_Distance	oldSize	=	fontSpec.GetPointSize ();
					chosenFontSize = PickOtherFontHeight (oldSize);
				}
				break;
			}
		}
		if (chosenFontSize != 0) {
			applyFontSpec.SetPointSize (chosenFontSize);
		}
		goto handled;
	}
	return false;

handled:
	SetDefaultFont (applyFontSpec);
	return true;
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
	return LedLineItApplication::Get ().GetSearchParameters ();
}

void	LedLineItView::SetSearchParameters (const SearchParameters& sp)
{
	LedLineItApplication::Get ().SetSearchParameters (sp);
}

void	LedLineItView::OnGotoLineCommand ()
{
	/*
	 *	compute current line, and use that as the default for the dialog box. Put up the dlog box,
	 *	prompting user. Then move the selection to that line (and scroll to selection).
	 */
	size_t	rowNumber	=	GetRowContainingPosition (GetSelectionStart ());	// unclear if we should use selstart or selEnd - pick arbitrarily...

	StDialogHandler	findDialog (kGotoLine_DialogID, NULL);

	LEditField*		lineText	=	(LEditField*)findDialog.GetDialog ()->FindPaneByID ('line');
	Led_AssertNotNil (lineText);

	lineText->SetValue (rowNumber);

	lineText->SelectAll (); SwitchTarget (lineText);

	bool	hitOK	=	false;
	while (true) {
		MessageT	hitMessage = findDialog.DoDialog ();
		hitOK = (hitMessage == 'ok  ');
		if (hitMessage != 0) {
			break;
		}
	}

	if (hitOK) {
		long	gotoLine	=	lineText->GetValue ();

		// Assume line chosen 1 based, regardless of how Led is built. This is the most common UI.
		// And pin to ends if user choses too large / too small line number
		size_t	selPos	=	0;
		if (gotoLine >= 1) {
			if (gotoLine > GetRowCount ()) {
				gotoLine = GetRowCount ();
			}
			selPos = GetStartOfRow (gotoLine);
		}
		SetSelection (selPos, selPos);
		ScrollToSelection ();
	}
}

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
					Led_Assert (pmLength >= 1);
					size_t	lookAtLength = min (static_cast<size_t> (pmLength-1), static_cast<size_t> (kCharsPerTab));
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





// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
