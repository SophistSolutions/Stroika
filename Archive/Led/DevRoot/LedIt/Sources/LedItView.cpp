/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/LedIt/Sources/LedItView.cpp,v 1.58 2004/02/11 23:58:04 lewis Exp $
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
 *	Revision 1.58  2004/02/11 23:58:04  lewis
 *	SPR#1576: Update: added ReplaceAllInSelection functionality (except must re-layout MacOS dialog)
 *	
 *	Revision 1.57  2004/02/11 22:41:36  lewis
 *	SPR#1576: update: added 'Replace Again' command to LedIt, LedLineItMFC, ActiveLedIt
 *	
 *	Revision 1.56  2004/02/11 01:14:23  lewis
 *	SPR#1635: qTemplatedMemberFunctionsFailBug BWA. Also - disabled spellcheck cmd if qIncludeBakedInDictionaries not true (MSVC60)
 *	
 *	Revision 1.55  2004/01/25 23:25:43  lewis
 *	SPR#1621: problem with CHecKForUpdatesOnWeb was cuz code to map associated cmd#s returns zero when a failed mapping occurs, and we had bad association between cmds# between Led internal and LedIt/ActiveLedIt 'system-dependent' values for Win32. Fix those for kFirstPrivateEmbedding_CmdID/kLastSelectedEmbedding_CmdID - and clean up first/last private range inside of it.
 *	
 *	Revision 1.54  2004/01/09 18:50:32  lewis
 *	SPR#1614: since we changed WordProcessor::CalculateFarthestRightMarginInWindow () to call GetLayoutWidth () - we no longer need the override in LedItView
 *	
 *	Revision 1.53  2003/12/31 04:15:28  lewis
 *	change DisplayFindDialog/DisplayReplaceDialog functions for SPR#1580
 *	
 *	Revision 1.52  2003/12/09 21:05:02  lewis
 *	use new SpellCheckEngine_Basic_Simple class and call fSpellCheckEngine.SetUserDictionary
 *	to specify UD name
 *	
 *	Revision 1.51  2003/11/06 16:08:48  lewis
 *	SPR#1559: change CalculateFarthestRightMarginInWindow to CalculateFarthestRightMargin
 *	and return TWIPS. SPR#1560: lose WordProcessorHScrollbarHelper<> template.
 *	
 *	Revision 1.50  2003/06/02 16:15:49  lewis
 *	SPR#1513: Simple spellcheck engine and spell check dialog support (windows only so far)
 *	
 *	Revision 1.49  2003/05/30 03:12:04  lewis
 *	SPR#1517: preliminary version of REPLACE DIALOG support (WIN32 only so far).
 *	
 *	Revision 1.48  2003/05/29 20:12:12  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.47  2003/05/13 21:49:58  lewis
 *	SPR#1399: added qSupportEditTablePropertiesDlg support
 *	
 *	Revision 1.46  2003/05/12 17:18:14  lewis
 *	SPR#1401: Added GetDialogSupport ().AddNewTableDialog ()
 *	
 *	Revision 1.45  2003/05/08 00:11:27  lewis
 *	SPR#1467: added kSelectTableIntraCellAll_CmdID and cleaned up menu command name strings for new select commands(win32)
 *	
 *	Revision 1.44  2003/05/07 21:14:09  lewis
 *	SPR#1467: new Select menu (and context menu) for Win32
 *	
 *	Revision 1.43  2003/05/05 19:30:30  lewis
 *	SPR#1462: Must override LedItView::OnUpdateCommand () to handle toolbar CMD#s and add them to command# list
 *	
 *	Revision 1.42  2003/04/16 14:13:25  lewis
 *	SPR#1437: cosmetic cleanups - nest code and scope in in braces inside of UndoContext instances, and use new TextInteractor::BreakInGroupedCommands () wrapper
 *	
 *	Revision 1.41  2003/04/15 22:36:42  lewis
 *	SPR#1425: added new 'Remove' menu and added to it RemoveTableRows/Cols, and moved Show/Hide Selection there
 *	
 *	Revision 1.40  2003/04/07 14:11:21  lewis
 *	SPR#1414: finished fixing liststyle support for MacOS
 *	
 *	Revision 1.39  2003/04/04 14:42:24  lewis
 *	SPR#1407: More work cleaning up new template-free command update/dispatch code
 *	
 *	Revision 1.38  2003/04/04 00:55:35  lewis
 *	SPR#1407: more work on getting this covnersion to new command handling working (mostly MacOS/Linux now)
 *	
 *	Revision 1.37  2003/04/03 22:49:22  lewis
 *	SPR#1407: more work on getting this covnersion to new command handling working (mostly MacOS/Linux now)
 *	
 *	Revision 1.36  2003/04/03 21:53:10  lewis
 *	SPR#1407: more cleanups of new CommandNumberMapping<> stuff and other new command# processing support
 *	
 *	Revision 1.35  2003/04/03 16:54:12  lewis
 *	SPR#1407: First cut at major change in command-processing classes. Instead of using templated
 *	command classes, just builtin to TextInteractor/WordProcessor (and instead of template params
 *	use new TextInteractor/WordProcessor::DialogSupport etc)
 *	
 *	Revision 1.34  2003/03/21 15:01:33  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.33  2003/01/23 18:05:16  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.32  2003/01/23 17:03:46  lewis
 *	lose commneted out dlg code
 *	
 *	Revision 1.31  2003/01/23 01:16:25  lewis
 *	fix revised dialog calls for MacOS (SPR#1186,SPR#1256)
 *	
 *	Revision 1.30  2003/01/22 01:08:26  lewis
 *	SPR#1256 - OtherSize&ParagraphSpacing dialogs moved to LedStdDialogs(untested on Mac).
 *	
 *	Revision 1.29  2003/01/21 13:25:56  lewis
 *	SPR#1186 - first cut at Indents dialog support
 *	
 *	Revision 1.28  2003/01/17 18:50:24  lewis
 *	fix for CW8 compat (powerplant)
 *	
 *	Revision 1.27  2002/11/14 17:14:00  lewis
 *	SPR#1171- use new SetEmptySelectionStyle() no-arg overload for when we load a new
 *	document (untested on macos/linux - but thats in SPR#1172)
 *	
 *	Revision 1.26  2002/10/30 22:44:01  lewis
 *	SPR#1158 - SetDefaultWindowMargins () API now uses Led_TWIPS
 *	
 *	Revision 1.25  2002/10/25 17:57:03  lewis
 *	SPR#1146 - Added 'Insert Symbol' menu item
 *	
 *	Revision 1.24  2002/10/25 01:55:37  lewis
 *	SPR#1148 - change COleInsertDialog usage so we can insert ActiveX controls like ActiveLedIt
 *	
 *	Revision 1.23  2002/10/24 15:53:15  lewis
 *	SPR#0680- adjust the window margins so text looks a little better. Did for MacOS/XWin/Win -
 *	but only tested on Win so far
 *	
 *	Revision 1.22  2002/10/22 00:43:25  lewis
 *	SPR#1136- Add URL support
 *	
 *	Revision 1.21  2002/10/20 20:04:39  lewis
 *	added invariant
 *	
 *	Revision 1.20  2002/10/12 04:33:25  lewis
 *	SPR#1127- fix UNDO info for insert of OLE embedding
 *	
 *	Revision 1.19  2002/09/11 04:16:00  lewis
 *	SPR#1094- VERY preliminary Table support - an insert menu and InsertTable command
 *	
 *	Revision 1.18  2002/05/06 21:31:07  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 1.17  2001/11/27 00:28:15  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.16  2001/10/16 16:13:50  lewis
 *	fix LedItView::GetLayoutMargins () code so it handles fWrapToWindow better (only changes RHS wrappoint)
 *	
 *	Revision 1.15  2001/09/26 15:41:47  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.14  2001/09/18 19:13:43  lewis
 *	SPR#0791- added support for toggle (use smart cut&paste, wrap-to-window,show hidden text)
 *	commands for MacOS
 *	
 *	Revision 1.13  2001/09/17 14:42:17  lewis
 *	SPR#1032- added SetDefaultFont dialog and saved preferences (pref for new docs)
 *	
 *	Revision 1.12  2001/09/12 11:33:15  lewis
 *	use secondary hilight on for MacOS as well
 *	
 *	Revision 1.11  2001/08/29 22:59:19  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.10  2001/08/17 15:59:39  lewis
 *	disable qSupportUnknownEmbeddingInfoDlg/qSupportURLXEmbeddingInfoDlg for XWindows (SPR#0967)
 *	
 *	Revision 1.9  2001/07/19 02:22:39  lewis
 *	SPR#0960/0961- CW6Pro support, and preliminary Carbon SDK support.
 *	
 *	Revision 1.8  2001/07/17 19:08:04  lewis
 *	lose GetSoleSelectedEmbedding () implemenation (now inherited) - and SPR#0959- added preliminary
 *	RightClick-to-show-edit-embedding-properties support
 *	
 *	Revision 1.7  2001/06/05 15:44:07  lewis
 *	SPR#0950- enable Find dialog for X-Windows/Gtk. Also fixed some small find bugs on Mac/Win that
 *	were introduced by merging of code, and some cleanups enabled by merging
 *	
 *	Revision 1.6  2001/05/24 21:34:01  lewis
 *	fix mac compile problem
 *	
 *	Revision 1.5  2001/05/22 21:44:33  lewis
 *	MAJOR changes - now supporting checked menu items on X-Windows and enabling (SPR#0924). Also, much
 *	improved dialog code - including dialog for aboutbox, font/color pickers etc (SPR#0923)
 *	
 *	Revision 1.4  2001/05/18 23:00:12  lewis
 *	small cleanups - and gotoWebPage(help menu) support for XWindows
 *	
 *	Revision 1.3  2001/05/16 16:03:19  lewis
 *	more massive changes. Make Options () stuff portable (though fake impl om mac/X). Got rid of AppWindow
 *	class - and now X-WIndows fully using LedDoc/LedView. Much more common code between implementations -
 *	but still a lot todo
 *	
 *	Revision 1.2  2001/05/15 16:43:18  lewis
 *	SPR#0920- lots more misc cleanups to share more code across platforms and standardize cmd names etc
 *	
 *	Revision 1.1  2001/05/14 20:54:48  lewis
 *	New LedIt! CrossPlatform app - based on merging LedItPP and LedItMFC and parts of LedTextXWindows
 *	
 *
 *
 *
 *	<<<***		Based on LedItMFC/LedItPP project from Led 3.0b6		***>>>
 *
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#if		qMacOS
	#include	<AERegistry.h>
	#include	<ColorPicker.h>
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
#elif	defined (WIN32)
	#include	<afxodlgs.h>       // MFC OLE dialog classes
#endif

#include	"LedStdDialogs.h"

#include	"ColorMenu.h"
#if		qWindows
	#include	"LedItControlItem.h"
#endif
#include	"LedItDocument.h"
#include	"Options.h"
#include	"LedItResources.h"

#include	"LedItApplication.h"

#include	"LedItView.h"




#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (4 : 4800)	//qUsePragmaWarningToSilenceNeedlessBoolConversionWarnings
#endif












class	My_CMDNUM_MAPPING : public 
		#if		qWindows
			MFC_CommandNumberMapping
		#elif	qMacOS
			PP_CommandNumberMapping
		#elif	qXWindows
			Gtk_CommandNumberMapping
		#endif
	{
	public:
		My_CMDNUM_MAPPING ()
			{
				AddAssociation (kCmdUndo,						LedItView::kUndo_CmdID);
				AddAssociation (kCmdRedo,						LedItView::kRedo_CmdID);
				AddAssociation (kCmdSelectAll,					LedItView::kSelectAll_CmdID);
				AddAssociation (kCmdCut,						LedItView::kCut_CmdID);
				AddAssociation (kCmdCopy,						LedItView::kCopy_CmdID);
				AddAssociation (kCmdPaste,						LedItView::kPaste_CmdID);
				AddAssociation (kCmdClear,						LedItView::kClear_CmdID);
				AddAssociation (kFindCmd,						LedItView::kFind_CmdID);
				AddAssociation (kFindAgainCmd,					LedItView::kFindAgain_CmdID);
				AddAssociation (kEnterFindStringCmd,			LedItView::kEnterFindString_CmdID);
				AddAssociation (kReplaceCmd,					LedItView::kReplace_CmdID);
				AddAssociation (kReplaceAgainCmd,				LedItView::kReplaceAgain_CmdID);
			#if		qIncludeBakedInDictionaries
				// If we have no dictionaries - assume no spellcheck command should be enabled (mapped)
				AddAssociation (kSpellCheckCmd,					LedItView::kSpellCheck_CmdID);
			#endif
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
			#if		qMacOS
				AddAssociation (kFontStyleOutlineCmd,			LedItView::kFontStyleOutline_CmdID);
				AddAssociation (kFontStyleShadowCmd,			LedItView::kFontStyleShadow_CmdID);
				AddAssociation (kFontStyleCondensedCmd,			LedItView::kFontStyleCondensed_CmdID);
				AddAssociation (kFontStyleExtendedCmd,			LedItView::kFontStyleExtended_CmdID);
			#endif
			#if		qWindows
				AddAssociation (kFontStyleStrikeoutCmd,			LedItView::kFontStyleStrikeout_CmdID);
			#endif
				AddAssociation (kSubScriptCmd,					LedItView::kSubScriptCommand_CmdID);
				AddAssociation (kSuperScriptCmd,				LedItView::kSuperScriptCommand_CmdID);
			#if		qWindows || qXWindows
				AddAssociation (kChooseFontDialogCmd,			LedItView::kChooseFontCommand_CmdID);
			#endif

				AddAssociation (kInsertTableCmd,				LedItView::kInsertTable_CmdID);
				AddAssociation (kInsertTableRowAboveCmd,		LedItView::kInsertTableRowAbove_CmdID);
				AddAssociation (kInsertTableRowBelowCmd,		LedItView::kInsertTableRowBelow_CmdID);
				AddAssociation (kInsertTableColBeforeCmd,		LedItView::kInsertTableColBefore_CmdID);
				AddAssociation (kInsertTableColAfterCmd,		LedItView::kInsertTableColAfter_CmdID);
				AddAssociation (kInsertURLCmd,					LedItView::kInsertURL_CmdID);
			#if		qWindows
				AddAssociation (kInsertSymbolCmd,				LedItView::kInsertSymbol_CmdID);
			#endif

//				AddAssociation (kPropertiesForSelectionCmd,		LedItView::kSelectedEmbeddingProperties_CmdID);
				AddRangeAssociation (
								kFirstSelectedEmbeddingCmd, kLastSelectedEmbeddingCmd,
								LedItView::kFirstSelectedEmbedding_CmdID, LedItView::kLastSelectedEmbedding_CmdID
							);

				AddAssociation (kHideSelectionCmd,				LedItView::kHideSelection_CmdID);
				AddAssociation (kUnHideSelectionCmd,			LedItView::kUnHideSelection_CmdID);
				AddAssociation (kRemoveTableRowsCmd,			LedItView::kRemoveTableRows_CmdID);
				AddAssociation (kRemoveTableColumnsCmd,			LedItView::kRemoveTableColumns_CmdID);

				AddAssociation (kShowHideParagraphGlyphsCmd,	LedItView::kShowHideParagraphGlyphs_CmdID);
				AddAssociation (kShowHideTabGlyphsCmd,			LedItView::kShowHideTabGlyphs_CmdID);
				AddAssociation (kShowHideSpaceGlyphsCmd,		LedItView::kShowHideSpaceGlyphs_CmdID);

#if		qWindows
				AddAssociation (IDC_FONTSIZE,	IDC_FONTSIZE);
				AddAssociation (IDC_FONTNAME,	IDC_FONTNAME);
#endif
			}
};
My_CMDNUM_MAPPING	sMy_CMDNUM_MAPPING;









struct	LedIt_DialogSupport : TextInteractor::DialogSupport, WordProcessor::DialogSupport {
	public:
		typedef	TextInteractor::DialogSupport::CommandNumber	CommandNumber;

	public:
		LedIt_DialogSupport ()
			{
				TextInteractor::SetDialogSupport (this);
				WordProcessor::SetDialogSupport (this);
			}
		~LedIt_DialogSupport ()
			{
				WordProcessor::SetDialogSupport (NULL);
				TextInteractor::SetDialogSupport (NULL);
			}

	//	TextInteractor::DialogSupport
#if		qSupportStdFindDlg
	public:
		override	void	DisplayFindDialog (Led_tString* findText, const vector<Led_tString>& recentFindSuggestions, bool* wrapSearch, bool* wholeWordSearch, bool* caseSensative, bool* pressedOK)
			{
				#if		qMacOS
					Led_StdDialogHelper_FindDialog	findDialog;
				#elif	qWindows
					Led_StdDialogHelper_FindDialog	findDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
				#elif	qXWindows
					Led_StdDialogHelper_FindDialog	findDialog (GTK_WINDOW (LedItApplication::Get ().GetAppWindow ()));
				#endif

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
				#if		qMacOS
					Led_StdDialogHelper_ReplaceDialog	replaceDialog;
				#elif	qWindows
					Led_StdDialogHelper_ReplaceDialog	replaceDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
				#elif	qXWindows
					Led_StdDialogHelper_ReplaceDialog	replaceDialog (GTK_WINDOW (LedItApplication::Get ().GetAppWindow ()));
				#endif

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
				#if		qMacOS
					static	LMenu*			fontMenu	=	LMenuBar::GetCurrentMenuBar()->FetchMenu (cmd_FontMenu);
					static	vector<short>	sFontIDMapCache;	// OK to keep static cuz never changes during run of app

					size_t	idx	=	cmdNum-WordProcessor::kFontMenuFirst_CmdID;

					// Pre-fill cache - at least to the cmd were looking for...
					for (size_t i = sFontIDMapCache.size (); i <= idx; i++) {
						Str255		pFontName	=	{0};
						UInt16		menuItem	=	fontMenu->IndexFromCommand (i+WordProcessor::kFontMenuFirst_CmdID);
						::GetMenuItemText (fontMenu->GetMacMenuH (), menuItem, pFontName);
						short	familyID	=	0;
						::GetFNum (pFontName, &familyID);
						sFontIDMapCache.push_back (familyID);
					}
					return sFontIDMapCache[idx];
				#elif	qWindows
					return LedItApplication::Get ().CmdNumToFontName (MFC_CommandNumberMapping::Get ().ReverseLookup (cmdNum)).c_str ();
				#elif	qXWindows
					const vector<Led_SDK_String>&	fontNames	=	LedItApplication::Get ().fInstalledFonts.GetUsableFontNames ();
					Led_Assert (cmdNum - LedItView::kFontMenuFirst_CmdID < fontNames.size ());
					return (fontNames[cmdNum - LedItView::kFontMenuFirst_CmdID]);
				#endif
			}
		#if		qSupportOtherFontSizeDlg
		override		Led_Distance		PickOtherFontHeight (Led_Distance origHeight)
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
		#endif
		#if		qSupportParagraphSpacingDlg
		override		bool				PickNewParagraphLineSpacing (Led_TWIPS* spaceBefore, bool* spaceBeforeValid, Led_TWIPS* spaceAfter, bool* spaceAfterValid, Led_LineSpacing* lineSpacing, bool* lineSpacingValid)
			{
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
		#if		qXWindows
		override		bool				PickOtherFontColor (Led_Color* color)
			{
				StdColorPickBox	dlg (GTK_WINDOW (LedItApplication::Get ().GetAppWindow ()), *color);
				dlg.DoModal ();
				if (dlg.GetWasOK ()) {
					*color = dlg.fColor;
					return true;
				}
				return false;
			}
		#endif
		#if		qXWindows
		override		bool				ChooseFont (Led_IncrementalFontSpecification* font)
			{
				StdFontPickBox	dlg (GTK_WINDOW (LedItApplication::Get ().GetAppWindow ()), *font);
				dlg.DoModal ();
				if (dlg.GetWasOK ()) {
					*font = dlg.fFont;
					return true;
				}
				return false;
			}
		#endif
		#if		qMacOS || qWindows
		override	void				ShowSimpleEmbeddingInfoDialog (const Led_SDK_String& embeddingTypeName)
			{
				// unknown embedding...
				#if		qMacOS
					Led_StdDialogHelper_UnknownEmbeddingInfoDialog	infoDialog;
				#elif	qWindows
					Led_StdDialogHelper_UnknownEmbeddingInfoDialog	infoDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
				#endif
				#if		qMacOS || qWindows
					infoDialog.fEmbeddingTypeName = embeddingTypeName;
					(void)infoDialog.DoModal ();
				#endif
			}
		#endif
		#if		qMacOS || qWindows
		override		bool	ShowURLEmbeddingInfoDialog (const Led_SDK_String& embeddingTypeName, Led_SDK_String* urlTitle, Led_SDK_String* urlValue)
			{
				#if		qMacOS
					Led_StdDialogHelper_URLXEmbeddingInfoDialog	infoDialog;
				#elif	qWindows
					Led_StdDialogHelper_URLXEmbeddingInfoDialog	infoDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
				#endif
				#if		qMacOS || qWindows
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
				#else
					return false;
				#endif
			}
		#endif
		#if		qMacOS || qWindows
		override		bool	ShowAddURLEmbeddingInfoDialog (Led_SDK_String* urlTitle, Led_SDK_String* urlValue)
			{
				#if		qMacOS
					Led_StdDialogHelper_AddURLXEmbeddingInfoDialog	infoDialog;
				#elif	qWindows
					Led_StdDialogHelper_AddURLXEmbeddingInfoDialog	infoDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
				#endif
				#if		qMacOS || qWindows
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
				#else
					return false;
				#endif
			}
		#endif
		#if		qSupportAddNewTableDlg
		bool	AddNewTableDialog (size_t* nRows, size_t* nCols)
			{
				Led_RequireNotNil (nRows);
				Led_RequireNotNil (nCols);
				#if		qMacOS
					Led_StdDialogHelper_AddNewTableDialog	infoDialog;
				#elif	qWindows
					Led_StdDialogHelper_AddNewTableDialog	infoDialog (::AfxGetResourceHandle (), ::GetActiveWindow ());
				#endif
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
				#if		qTemplatedMemberFunctionsFailBug
					Led_StdDialogHelper_EditTablePropertiesDialog_cvt<DLGTYPE::Info, TableSelectionPropertiesInfo> (&infoDialog.fInfo, *tableProperties);
				#else
					DLGTYPE::cvt<DLGTYPE::Info, TableSelectionPropertiesInfo> (&infoDialog.fInfo, *tableProperties);
				#endif
				if (infoDialog.DoModal ()) {
					#if		qTemplatedMemberFunctionsFailBug
						Led_StdDialogHelper_EditTablePropertiesDialog_cvt<TableSelectionPropertiesInfo, DLGTYPE::Info> (tableProperties, infoDialog.fInfo);
					#else
						DLGTYPE::cvt<TableSelectionPropertiesInfo, DLGTYPE::Info> (tableProperties, infoDialog.fInfo);
					#endif
					return true;
				}
				else {
					return false;
				}
			}
		#endif
};
static	LedIt_DialogSupport	sLedIt_DialogSupport;






/*
 ********************************************************************************
 ************************************ LedItView *********************************
 ********************************************************************************
 */
#if		qWindows
DoDeclare_WordProcessorCommonCommandHelper_MFC_MessageMap (LedItViewAlmostBASE)

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
#endif

LedItView::LedItView (
				#if		qXWindows
				  LedItDocument* owningDoc
				#endif
				):
	inherited (),
	fWrapToWindow (Options ().GetWrapToWindow ())
{
	SetSmartCutAndPasteMode (Options ().GetSmartCutAndPaste ());

	SetShowParagraphGlyphs (Options ().GetShowParagraphGlyphs ());
	SetShowTabGlyphs (Options ().GetShowTabGlyphs ());
	SetShowSpaceGlyphs (Options ().GetShowSpaceGlyphs ());
#if		qMacOS
	SetScrollBarType (h, fWrapToWindow? eScrollBarNever: eScrollBarAsNeeded);
	SetScrollBarType (v, eScrollBarAlways);
#elif	qWindows
	SetScrollBarType (h, fWrapToWindow? eScrollBarNever: eScrollBarAsNeeded);
	SetScrollBarType (v, eScrollBarAlways);
#elif	qXWindows
	SpecifyTextStore (&owningDoc->GetTextStore ());
	SetStyleDatabase (owningDoc->GetStyleDatabase ());
	SetParagraphDatabase (owningDoc->GetParagraphDatabase ());
	SetHidableTextDatabase (owningDoc->GetHidableTextDatabase ());
	//SetShowHiddenText (Options ().GetShowHiddenText ());
	SetCommandHandler (&owningDoc->GetCommandHandler ());
	SetSpellCheckEngine (&LedItApplication::Get ().fSpellCheckEngine);
#endif
#if		qMacOS || qWindows
	SetUseSecondaryHilight (true);
#endif
#if		qWindows
	// SHOULD be supported on other platforms, but only Win32 for now...
	SetDefaultWindowMargins (Led_TWIPS_Rect (kLedItViewTopMargin, kLedItViewLHSMargin, kLedItViewBottomMargin-kLedItViewTopMargin, kLedItViewRHSMargin-kLedItViewLHSMargin));
#endif
}

LedItView::~LedItView()
{
	SpecifyTextStore (NULL);
	SetCommandHandler (NULL);
	SetSpellCheckEngine (NULL);
}

#if		qWindows
void	LedItView::OnInitialUpdate ()
{
	inherited::OnInitialUpdate ();
	SpecifyTextStore (&GetDocument ().GetTextStore ());
	SetStyleDatabase (GetDocument ().GetStyleDatabase ());
	SetParagraphDatabase (GetDocument ().GetParagraphDatabase ());
	SetHidableTextDatabase (GetDocument ().GetHidableTextDatabase ());
	SetShowHiddenText (Options ().GetShowHiddenText ());
	SetCommandHandler (&GetDocument ().GetCommandHandler ());
	SetSpellCheckEngine (&LedItApplication::Get ().fSpellCheckEngine);

	{
		// Don't let this change the docs IsModified flag
		bool	docModified	=	GetDocument ().IsModified ();
		// For an empty doc - grab from the default, and otherwise grab from the document itself
		if (GetEnd () == 0) {
			SetEmptySelectionStyle (Options ().GetDefaultNewDocFont ());
		}
		else {
			SetEmptySelectionStyle ();
		}
		InvalidateAllCaches ();	// under rare circumstances, the caches don't all get cleared without this...
		GetDocument ().SetModifiedFlag (docModified);
	}
	Invariant ();
}
#endif

#if		qWindows
bool	LedItView::OnUpdateCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	if (inherited::OnUpdateCommand (enabler)) {
		return true;
	}
	// See SPR#1462 - yet assure these items in the formatBar remain enabled...
	switch (enabler->GetCmdID ()) {
		case	IDC_FONTSIZE:			{	enabler->SetEnabled (true);		return true;	}
		case	IDC_FONTNAME:			{	enabler->SetEnabled (true);		return true;	}
	}
	return false;
}
#endif

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
		// Make the layout width of each line (paragraph) equal to the windowrect. Ie, wrap to the
		// edge of the window. NB: because of this choice, we must 'InvalidateAllCaches' when the
		// WindowRect changes in our SetWindowRect() override.
		if (rhs != NULL) {
			*rhs = (Led_Max (GetWindowRect ().GetWidth (), 1));
		}
	}
}

void	LedItView::SetWindowRect (const Led_Rect& windowRect)
{
	Led_Rect	oldWindowRect	=	GetWindowRect ();
	// Hook all changes in the window width, so we can invalidate the word-wrap info (see LedItView::GetLayoutWidth)
	if (windowRect != oldWindowRect) {
		// NB: call "WordWrappedTextImager::SetWindowRect() instead of base class textinteractor to avoid infinite recursion"
		WordWrappedTextImager::SetWindowRect (windowRect);
		if (fWrapToWindow and windowRect.GetSize () != oldWindowRect.GetSize ()) {
			InvalidateAllCaches ();
		}
	}
}

#if		qMacOS
void	LedItView::FindCommandStatus (CommandT inCommand, Boolean& outEnabled, Boolean& outUsesMark, UInt16& outMark, Str255 outName)
{
	outUsesMark = false;
	switch (inCommand) {
		case	cmd_ListStyleMenu:		outEnabled = true;											break;
		case	cmd_FontMenu:			outEnabled = true;											break;
		case	cmd_SizeMenu:			outEnabled = true;											break;
		case	cmd_StyleMenu:			outEnabled = true;											break;
		case	cmd_ColorMenu:			outEnabled = true;											break;
		case	cmd_JustificationMenu:	outEnabled = true;											break;

		default: {
			inherited::FindCommandStatus (inCommand, outEnabled, outUsesMark, outMark, outName);
		}
		break;
	}
}
#endif

#if		qWindows
void	LedItView::OnContextMenu (CWnd* /*pWnd*/, CPoint pt) 
{
	CMenu menu;
	if (menu.LoadMenu (kContextMenu)) {
		CMenu*	popup = menu.GetSubMenu (0);
		Led_AssertNotNil (popup);
		LedItApplication::Get ().FixupFontMenu (popup->GetSubMenu (16));
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
#endif

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

#if		qWindows
void	LedItView::OnInsertObject ()
{
	// Invoke the standard Insert Object dialog box to obtain information
	//  for new LedItControlItem object.
	COleInsertDialog dlg (IOF_SELECTCREATECONTROL | IOF_SHOWINSERTCONTROL | IOF_VERIFYSERVERSEXIST);
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

		dlg.m_io.dwFlags |= IOF_SELECTCREATENEW;
		// Initialize the item from the dialog data.
		if (!dlg.CreateItem (pItem)) {
			Led_ThrowBadFormatDataException ();
		}
		ASSERT_VALID(pItem);

		BreakInGroupedCommands ();
		UndoableContextHelper	context (*this, Led_SDK_TCHAROF ("Insert OLE Embedding"), false);
			{
				AddEmbedding (pItem, GetTextStore (), GetSelectionStart (), GetDocument ().GetStyleDatabase ());
				SetSelection (GetSelectionStart ()+1, GetSelectionStart () + 1);
			}
		context.CommandComplete ();
		BreakInGroupedCommands ();

		// If item created from class list (not from file) then launch
		//  the server to edit the item.
		if (dlg.GetSelectionType () == COleInsertDialog::createNewItem) {
			pItem->DoVerb (OLEIVERB_SHOW, this);
		}

		ASSERT_VALID (pItem);

		doc.UpdateAllViews (NULL);
	}
	CATCH (CException, e) {
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
#endif

#if		qWindows
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
#endif




// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
