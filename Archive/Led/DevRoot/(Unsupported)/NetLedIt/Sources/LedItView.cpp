/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/NetLedIt/Sources/LedItView.cpp,v 1.1 2004/01/01 04:20:32 lewis Exp $
 *
 * Changes:
 *	$Log: LedItView.cpp,v $
 *	Revision 1.1  2004/01/01 04:20:32  lewis
 *	moved NetLedIt to (Unsupported)
 *	
 *	Revision 1.35  2003/04/05 16:01:51  lewis
 *	SPR#1407: support new command handling code - and normalize #define names a bit across projects
 *	
 *	Revision 1.34  2003/03/11 02:35:56  lewis
 *	SPR#1288 - use new MakeSophistsAppNameVersionURL and add new CheckForUpdatesWebPageCommand
 *	
 *	Revision 1.33  2003/01/24 02:39:30  lewis
 *	fix small compile error
 *	
 *	Revision 1.32  2002/10/31 21:10:36  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.31  2002/09/04 12:56:16  lewis
 *	static_cast<> to silence warnings
 *	
 *	Revision 1.30  2002/05/06 21:35:00  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 1.29  2001/11/27 00:33:01  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.28  2001/09/26 15:53:04  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.27  2001/09/24 16:26:47  lewis
 *	SPR#0993- as part of fixing scrollbar flicker with autoshow - lose AlignTextEditRects
 *	and instead call SetAlwaysLeaveSpaceForSizeBox/SetScrollBarsOverlapOwningFrame
 *	
 *	Revision 1.26  2001/09/19 20:09:10  lewis
 *	SPR#1039 (more work on catching/reporting NetLedIt/Mac excpetions)
 *	
 *	Revision 1.25  2001/09/19 18:44:42  lewis
 *	prelim support for catching exceptions on commands (kBallAllocException etc in LedItView)
 *	
 *	Revision 1.24  2001/08/30 00:43:18  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.23  2001/08/27 13:44:29  lewis
 *	SPR#0972- add ListStyle popup menu (windows only), and fix LedItView::GetLayoutMargins ()
 *	so bullets display properly
 *	
 *	Revision 1.22  2001/08/17 17:03:02  lewis
 *	fix Windows to include unknown etc embedding picts/bmps. SPR#0959 - GetInfo support Mac
 *	and Windows (untested on mac yet)
 *	
 *	Revision 1.21  2001/05/29 23:01:06  lewis
 *	SPR#0943- fix to use new color list from combined LedIt! app. Fix a few names/color values.
 *	Fix Windows code to update command names (as in Undo XXX) and checkmarks.
 *	
 *	Revision 1.20  2001/04/19 18:45:04  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.19  2001/04/12 19:46:40  lewis
 *	SPR#0861- Expiration support for demo-mode
 *	
 *	Revision 1.18  2000/10/18 20:58:36  lewis
 *	fixed windoze removemenu DUP menu items on Windows code. Added hooks into new
 *	Led_StdDialogHelper_FindDialog code
 *	
 *	Revision 1.17  2000/10/14 14:31:30  lewis
 *	start implementing help/aboutbox for Mac/Windows. Got the cmds hooked in. Only
 *	tested on Windows (help). Still to write is aboutbox code (for both platoforms)
 *	
 *	Revision 1.16  2000/10/06 12:59:55  lewis
 *	WordProcessorCommonCommandHelper<> and hook various methods to implement
 *	wrap-to-window/no-wrap-towindow
 *	functionalty and make hscrolling work right
 *	
 *	Revision 1.15  2000/10/06 03:35:50  lewis
 *	misc fixes (macos), plus new unified API for SetScrollBarType across mac/pc. And hooked it
 *	into Javscript calls
 *	
 *	Revision 1.14  2000/10/05 17:38:09  lewis
 *	(wrong prev checkin message)- really added qMacOS/SetHasScrollBar prelim support
 *	
 *	Revision 1.13  2000/10/05 17:36:24  lewis
 *	Added kUnsupportedDIBFormatPictID, kUnknownEmbeddingPictID, kURLPictID
 *	
 *	Revision 1.12  2000/10/05 02:40:25  lewis
 *	last message wrong - really added a bunch more support for Java-called methods
 *	(from javascript/netscape). And much more
 *	
 *	Revision 1.11  2000/10/05 02:38:53  lewis
 *	fix javascript to by syntactic, and tested (most) all the functions here
 *	
 *	Revision 1.10  2000/10/04 18:27:45  lewis
 *	fix Mac fontName command handling
 *	
 *	Revision 1.9  2000/10/04 15:46:24  lewis
 *	cleanups and remove unneeded CMDINFO overrides - due to moving stuff to common code (SPR#0839).
 *	
 *	Revision 1.8  2000/10/04 13:19:38  lewis
 *	use new WordProcessorCommonCommandHelper_DefaultCmdImpl<BASECLASS> code more.
 *	Implemented font menu and hiding idsabled context menu items on PC
 *	
 *	Revision 1.7  2000/10/03 21:52:53  lewis
 *	Lots more work relating to SPR#0839. Got a bit more of the context menu stuff
 *	working (about 75% now working on the mac)
 *	
 *	Revision 1.6  2000/10/03 13:40:20  lewis
 *	SPR#0840- new WaterMarkHelper<> template for watermark/demomode. SPR#0839-
 *	use new WordProcessorCommonCommandHelper/TextInteractorCommonCommandHelper<> templates.
 *	A bit more code handling context menus (still more todo)
 *	
 *	Revision 1.5  2000/09/30 20:04:59  lewis
 *	Added preliminary (no display done yet) DemoMode support
 *	
 *	Revision 1.4  2000/09/30 19:37:24  lewis
 *	more work on context menus
 *	
 *	Revision 1.3  2000/09/29 18:06:39  lewis
 *	cleanup first cut at mac context menu code, and did first cut at PC context menu code -
 *	still less than halfway done
 *	
 *	Revision 1.2  2000/09/28 22:00:37  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.1  2000/09/28 21:03:39  lewis
 *	Create separeate LedItView module, and move stuff in there, and start cloning/copying
 *	stuff from ActiveLedIt!
 *	
 *
 *
 *
 */

#if		_MSC_VER
	#pragma	warning (disable : 4786)
#endif

#include	<cstddef>
#include	<cstring>


#if		qMacOS
#include	<list>
#include	<ToolUtils.h>
#endif

#include	"LedStdDialogs.h"

#include	"LedItView.h"


#if		qDemoMode
#include	"LedHandySimple.h"
#endif



#if		qWindows
extern	HINSTANCE	ghInstance;
#endif



#if		qLedUsesNamespaces
using	namespace	Led;
#endif





#if		qMacOS

	#define	STANDARD_NETLEDITAPPLICATION_CATCHERS()\
		catch (bad_alloc) {\
			HandleBadAllocException ();\
		}\
		catch (TextInteractor::BadUserInput&) {\
			HandleBadUserInputException ();\
		}\
		catch (...) {\
			HandleUnknownException ();\
		}

#elif	qWindows

	#define	STANDARD_NETLEDITAPPLICATION_CATCHERS()\
		catch (bad_alloc) {\
			HandleBadAllocException ();\
		}\
		catch (TextInteractor::BadUserInput&) {\
			HandleBadUserInputException ();\
		}\
		catch (...) {\
			HandleUnknownException ();\
		}

#endif




#if		qWindows
	static	Led_InstalledFonts	sInstalledFonts;	// Keep a static copy for speed, and so font#s are static throughout the life of the applet

	static	void	FixupFontMenu (HMENU fontMenu)
		{
			Led_RequireNotNil (fontMenu);
			const vector<Led_SDK_String>&	fontNames	=	sInstalledFonts.GetUsableFontNames ();

			// delete all menu items
			while (::DeleteMenu (fontMenu, 0, MF_BYPOSITION) != 0) {
				;
			}

			for (size_t i = 0; i < fontNames.size (); i++) {
				int	cmdNum	=	kBaseFontNameCmd + i;
				::AppendMenu (fontMenu, MF_STRING, cmdNum, fontNames[i].c_str ());
			}
		}

	static	Led_SDK_String	CmdNumToFontName (UINT cmdNum)
		{
			const vector<Led_SDK_String>&	fontNames	=	sInstalledFonts.GetUsableFontNames ();
			return (fontNames[cmdNum-kBaseFontNameCmd]);
		}
#endif








class	My_CMDNUM_MAPPING : public CommandNumberMapping<int> {
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

				AddAssociation (kPropertiesForSelectionCmd,		LedItView::kSelectedEmbeddingProperties_CmdID);

				AddAssociation (kHideSelectionCmd,				LedItView::kHideSelection_CmdID);
				AddAssociation (kUnHideSelectionCmd,			LedItView::kUnHideSelection_CmdID);

				AddAssociation (kShowHideParagraphGlyphsCmd,	LedItView::kShowHideParagraphGlyphs_CmdID);
				AddAssociation (kShowHideTabGlyphsCmd,			LedItView::kShowHideTabGlyphs_CmdID);
				AddAssociation (kShowHideSpaceGlyphsCmd,		LedItView::kShowHideSpaceGlyphs_CmdID);
			}
};
My_CMDNUM_MAPPING	sMy_CMDNUM_MAPPING;







#if		qMacOS
	static	vector<short>	sFontIDMapCache;	// OK to keep static cuz never changes during run of app
#endif


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
	public:
		override	void	DisplayFindDialog (Led_tString* findText, bool* wrapSearch, bool* wholeWordSearch, bool* caseSensative, bool* pressedOK)
			{
				#if		qMacOS
					Led_StdDialogHelper_FindDialog	findDialog;
				#elif	qWindows
					Led_StdDialogHelper_FindDialog	findDialog (ghInstance, ::GetActiveWindow ());
				#elif	qXWindows
					Led_StdDialogHelper_FindDialog	findDialog (GTK_WINDOW (LedItApplication::Get ().GetAppWindow ()));
				#endif

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

	//	WordProcessor::DialogSupport
	public:
		override	FontNameSpecifier	CmdNumToFontName (CommandNumber cmdNum)
			{
				Led_Require (cmdNum >= WordProcessor::kFontMenuFirst_CmdID);
				Led_Require (cmdNum <= WordProcessor::kFontMenuLast_CmdID);
				#if		qMacOS
					size_t	idx	=	cmdNum-WordProcessor::kFontMenuFirst_CmdID;
					Led_Assert (idx < sFontIDMapCache.size ());
					return sFontIDMapCache[idx];
				#elif	qWindows
					return ::CmdNumToFontName (cmdNum).c_str ();
				#endif
			}
		#if		qSupportOtherFontSizeDlg
		override		Led_Distance		PickOtherFontHeight (Led_Distance origHeight)
			{
				#if		qMacOS
					Led_StdDialogHelper_OtherFontSizeDialog	dlg;
				#elif	qWindows
					Led_StdDialogHelper_OtherFontSizeDialog	dlg (ghInstance, ::GetActiveWindow ());
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
					Led_StdDialogHelper_ParagraphSpacingDialog	dlg (ghInstance, ::GetActiveWindow ());
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
					Led_StdDialogHelper_ParagraphIndentsDialog	dlg (ghInstance, ::GetActiveWindow ());
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
				// unknown embedding...
				#if		qMacOS
					Led_StdDialogHelper_UnknownEmbeddingInfoDialog	infoDialog;
				#elif	qWindows
					Led_StdDialogHelper_UnknownEmbeddingInfoDialog	infoDialog (ghInstance, ::GetActiveWindow ());
				#endif
				#if		qMacOS || qWindows
					infoDialog.fEmbeddingTypeName = embeddingTypeName;
					(void)infoDialog.DoModal ();
				#endif
			}
		override		bool	ShowURLEmbeddingInfoDialog (const Led_SDK_String& embeddingTypeName, Led_SDK_String* urlTitle, Led_SDK_String* urlValue)
			{
				#if		qMacOS
					Led_StdDialogHelper_URLXEmbeddingInfoDialog	infoDialog;
				#elif	qWindows
					Led_StdDialogHelper_URLXEmbeddingInfoDialog	infoDialog (ghInstance, ::GetActiveWindow ());
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
		override		bool	ShowAddURLEmbeddingInfoDialog (Led_SDK_String* urlTitle, Led_SDK_String* urlValue)
			{
				#if		qMacOS
					Led_StdDialogHelper_AddURLXEmbeddingInfoDialog	infoDialog;
				#elif	qWindows
					Led_StdDialogHelper_AddURLXEmbeddingInfoDialog	infoDialog (ghInstance, ::GetActiveWindow ());
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
};
static	LedIt_DialogSupport	sLedIt_DialogSupport;




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
 ******************************* LedItViewCommandInfo ***************************
 ********************************************************************************
 */
 

#if 0
void	LedItViewCommandInfo::DisplayFindDialog (Led_tString* findText, bool* wrapSearch, bool* wholeWordSearch, bool* caseSensative, bool* pressedOK)
{
	#if		qMacOS
		Led_StdDialogHelper_FindDialog	findDialog;
	#elif	qWindows
		Led_StdDialogHelper_FindDialog	findDialog (ghInstance, ::GetActiveWindow ());
	#endif

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
	Led_Require (cmdNum >= kFontMenuFirst_CmdID);
	Led_Require (cmdNum <= kFontMenuLast_CmdID);
#if		qMacOS
	size_t	idx	=	cmdNum-kFontMenuFirst_CmdID;
	Led_Assert (idx < sFontIDMapCache.size ());
	return sFontIDMapCache[idx];
#elif	qWindows
	return ::CmdNumToFontName (cmdNum).c_str ();
#endif
}

Led_Distance	LedItViewCommandInfo::PickOtherFontHeight (Led_Distance origHeight)
{
#if 1
	return 12;
#else
	class	OtherFontSizeDialog : public CDialog {
		public:
			OtherFontSizeDialog (Led_Distance origFontSize) :
				CDialog (kOtherFontSize_DialogID),
				fOrigFontSize (origFontSize),
				fResultFontSize (0)
				{
				}
			override	BOOL OnInitDialog()
				{
					BOOL	result	=	CDialog::OnInitDialog();
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
					CDialog::OnOK ();
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
#endif
}

bool	LedItViewCommandInfo::PickNewParagraphLineSpacing (Led_TWIPS* spaceBefore, bool* spaceBeforeValid, Led_TWIPS* spaceAfter, bool* spaceAfterValid, Led_LineSpacing* lineSpacing, bool* lineSpacingValid)
{
	return false;
#if 0
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
#endif
}

void	LedItViewCommandInfo::ShowSimpleEmbeddingInfoDialog (const Led_SDK_String& embeddingTypeName)
{
	// unknown embedding...
	#if		qMacOS
		Led_StdDialogHelper_UnknownEmbeddingInfoDialog	infoDialog;
	#elif	qWindows
		Led_StdDialogHelper_UnknownEmbeddingInfoDialog	infoDialog (ghInstance, ::GetActiveWindow ());
	#endif
	#if		qMacOS || qWindows
		infoDialog.fEmbeddingTypeName = embeddingTypeName;
		(void)infoDialog.DoModal ();
	#endif
}

bool	LedItViewCommandInfo::ShowURLEmbeddingInfoDialog (const Led_SDK_String& embeddingTypeName, Led_SDK_String* urlTitle, Led_SDK_String* urlValue)
{
	#if		qMacOS
		Led_StdDialogHelper_URLXEmbeddingInfoDialog	infoDialog;
	#elif	qWindows
		Led_StdDialogHelper_URLXEmbeddingInfoDialog	infoDialog (ghInstance, ::GetActiveWindow ());
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







/*
 ********************************************************************************
 ************************************ LedItView *********************************
 ********************************************************************************
 */
#if		qMacOS
LedItView::MenuNumCmdMapRec	LedItView::kDefaultMenuNumCmdMapRecs[]		=	{
//kContextMenu
	{	kContextMenu,	1,	kCmdUndo },
	{	kContextMenu,	2,	kCmdRedo },
	{	kContextMenu,	4,	kCmdCut },
	{	kContextMenu,	5,	kCmdCopy },
	{	kContextMenu,	6,	kCmdPaste },
	{	kContextMenu,	7,	kCmdClear },
	{	kContextMenu,	9,	kCmdSelectAll },
	{	kContextMenu,	11,	kFindCmd },
	{	kContextMenu,	12,	kFindAgainCmd },
	{	kContextMenu,	13,	kEnterFindStringCmd },
	{	kContextMenu,	21,	kParagraphSpacingCmd },
	{	kContextMenu,	23,	kPropertiesForSelectionCmd },
	{	kContextMenu,	24,	kFirstPrivateEmbeddingCmd },
	{	kContextMenu,	26,	ID_HELP },
	{	kContextMenu,	27,	kCheckForUpdatesWebPageCmdID },
	{	kContextMenu,	29,	kAboutBoxCmd },

//cmd_StyleMenu
	{	cmd_StyleMenu,	1,	kFontStylePlainCmd },
	{	cmd_StyleMenu,	3,	kFontStyleBoldCmd },
	{	cmd_StyleMenu,	4,	kFontStyleItalicCmd },
	{	cmd_StyleMenu,	5,	kFontStyleUnderlineCmd },
	{	cmd_StyleMenu,	6,	kFontStyleOutlineCmd },
	{	cmd_StyleMenu,	7,	kFontStyleShadowCmd },
	{	cmd_StyleMenu,	8,	kFontStyleCondensedCmd },
	{	cmd_StyleMenu,	9,	kFontStyleExtendedCmd },
	{	cmd_StyleMenu,	10,	kSubScriptCmd },
	{	cmd_StyleMenu,	11,	kSubScriptCmd },

//cmd_SizeMenu
	{	cmd_SizeMenu,	1,	kFontSize9Cmd },
	{	cmd_SizeMenu,	2,	kFontSize10Cmd },
	{	cmd_SizeMenu,	3,	kFontSize12Cmd },
	{	cmd_SizeMenu,	4,	kFontSize14Cmd },
	{	cmd_SizeMenu,	5,	kFontSize18Cmd },
	{	cmd_SizeMenu,	6,	kFontSize24Cmd },
	{	cmd_SizeMenu,	7,	kFontSize36Cmd },
	{	cmd_SizeMenu,	8,	kFontSize48Cmd },
	{	cmd_SizeMenu,	9,	kFontSize72Cmd },
	{	cmd_SizeMenu,	11,	kFontSizeSmallerCmd },
	{	cmd_SizeMenu,	12,	kFontSizeLargerCmd },
	{	cmd_SizeMenu,	14,	kFontSizeOtherCmd },

//cmd_ColorMenu
	{	cmd_ColorMenu,	1,	kBlackColorCmd },
	{	cmd_ColorMenu,	2,	kMaroonColorCmd },
	{	cmd_ColorMenu,	3,	kGreenColorCmd },
	{	cmd_ColorMenu,	4,	kOliveColorCmd },
	{	cmd_ColorMenu,	5,	kNavyColorCmd },
	{	cmd_ColorMenu,	6,	kPurpleColorCmd },
	{	cmd_ColorMenu,	7,	kTealColorCmd },
	{	cmd_ColorMenu,	8,	kGrayColorCmd },
	{	cmd_ColorMenu,	9,	kSilverColorCmd },
	{	cmd_ColorMenu,	10,	kRedColorCmd },
	{	cmd_ColorMenu,	11,	kLimeColorCmd },
	{	cmd_ColorMenu,	12,	kYellowColorCmd },
	{	cmd_ColorMenu,	13,	kBlueColorCmd },
	{	cmd_ColorMenu,	14,	kFuchsiaColorCmd },
	{	cmd_ColorMenu,	15,	kAquaColorCmd },
	{	cmd_ColorMenu,	16,	kWhiteColorCmd },
	{	cmd_ColorMenu,	18,	kFontColorOtherCmd },

//cmd_JustificationMenu
	{	cmd_JustificationMenu,	1,	kJustifyLeftCmd },
	{	cmd_JustificationMenu,	2,	kJustifyCenterCmd },
	{	cmd_JustificationMenu,	3,	kJustifyRightCmd },
	{	cmd_JustificationMenu,	4,	kJustifyFullCmd },

};
#endif

LedItView::LedItView ():
	inherited (),
	fController (NULL),
#if		qMacOS
	fCurMenuNumCmdMapRecList (),
#endif
	fSupportContextMenu (true),
	fWrapToWindow (true),
	fMaxLength (-1)
#if		qMacOS
	,fFontMenu (NULL),
	fFontSizeMenu (NULL),
	fFontStyleMenu (NULL),
	fFontColorMenu (NULL),
	fFontJustificationMenu (NULL)
#endif
{
	SetScrollBarType (v, eScrollBarAsNeeded);
	SetScrollBarType (h, eScrollBarAsNeeded);

#if		qDemoMode
	SetUseBitmapScrollingOptimization (false);
#endif

#if		qMacOS
	SetAlwaysLeaveSpaceForSizeBox (false);			// on is good for when embedded in a window
	SetScrollBarsOverlapOwningFrame (false);		// ditto
#endif
}

LedItView::~LedItView()
{
#if		qMacOS
	Led_Assert (fFontMenu == NULL);
	Led_Assert (fFontSizeMenu == NULL);
	Led_Assert (fFontStyleMenu == NULL);
	Led_Assert (fFontColorMenu == NULL);
	Led_Assert (fFontJustificationMenu == NULL);
#endif
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

void	LedItView::SetWrapToWindow (bool wrapToWindow)
{
	if (fWrapToWindow != wrapToWindow) {
		fWrapToWindow = wrapToWindow;
		InvalidateAllCaches ();
		TabletChangedMetrics ();
		Refresh ();
	}
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
	// Hook all changes in the window width, so we can invalidate the word-wrap info (see LedItView::GetLayoutWidth)
	if (windowRect != GetWindowRect ()) {
		WordWrappedTextImager::SetWindowRect (windowRect);		//	NB: use XX instead of inherited to avoid infinite recurse, due to sloppy mixin ambiguity resoltion in base classes (LGP990623)
		if (GetWrapToWindow ()) {
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

void	LedItView::SetMaxLength (long maxLength)
{
	fMaxLength = maxLength;
}

#if		qMacOS
void	LedItView::SetupCurrentGrafPort () const
{
	if (fController != NULL) {
		fController->SetupCurrentGrafPort ();
	}
}

void	LedItView::UpdateWindowRect_ (const Led_Rect& windowRectArea) const
{
	if (fController != NULL) {
		fController->UpdateWindowRect_ (windowRectArea);
	}
}
#endif

#if		qDemoMode
void	LedItView::EraseBackground (Led_Tablet tablet, const Led_Rect& subsetToDraw, bool printing)
{
	inherited::EraseBackground (tablet, subsetToDraw, printing);
	static	WaterMarkHelper<>	waterMarkerImager (LED_TCHAR_OF ("Demo Mode"));	// make this static - just as a performance hack. Also could be an instance variable of 'this'.
	waterMarkerImager.DrawWatermark (tablet, GetWindowRect (), subsetToDraw);
}
#endif

#if		qDemoMode
void	LedItView::AboutToUpdateText (const UpdateInfo& updateInfo)
{
	try {
#if		qWindows
		static	int	sNextWarningAfter		=	0;
		int			now						=	time (NULL);
		const	int	kTimeBetweenWarnings	=	2*60;	// at most every 2 minutes - but really less often - cuz only on set focus...

		if (now > sNextWarningAfter) {
			// Only check if we are visible. Sometimes this gets called - somewhat inexplicably - during the close of the browser
			// window. Avoid somewhat ugly/confusing display of alert.
			HWND	w		=	GetHWND ();
			HWND	owner	=	::GetWindow (w, GW_OWNER);
			if (::IsWindow (w) and ::IsWindowVisible (w) and DemoPrefs ().GetDemoDaysLeft () <= 0) {
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
							//Led_URLManager::Get ().Open (kNetLedItDemoExpiredURL);
							Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/Led/NetLedIt/Default.asp", kAppName, string (kURLDemoFlag) + kDemoExpiredExtraArgs));
						}
				};
				sNextWarningAfter = now + kTimeBetweenWarnings;	// Set here just in case failure during dialog - still want to prevent repeat call too quickly
				DemoExpiresDLG	dlg (ghInstance, GetHWND ());
				dlg.DoModal ();
				sNextWarningAfter = now + kTimeBetweenWarnings;	// Set here so delay based on when dialog dismissed instead of when dialog comes up.
			}
		}
#endif
	}
	catch (...) {
		// ignore any errors with this code...
	}
	inherited::AboutToUpdateText (updateInfo);
}
#endif

#if		qWindows
LRESULT	LedItView::WndProc (UINT message, WPARAM wParam, LPARAM lParam)
{
	#define MY_GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
	#define MY_GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))
	switch (message) {
		case	WM_CONTEXTMENU: {
			OnContextMenu (Led_Point (MY_GET_Y_LPARAM (lParam), MY_GET_X_LPARAM (lParam)));
			return 0;
		}
		break;
		case	WM_KEYDOWN:
		case	WM_SYSKEYDOWN: {
			if (OnContextMenuKey (message, wParam, lParam)) {
				return true;
			}
			else {
				return inherited::WndProc (message, wParam, lParam);
			}
		}
		break;
		case	WM_INITMENUPOPUP: {
			if (OnInitMenuPopup (reinterpret_cast<HMENU> (wParam), LOWORD (lParam), HIWORD(lParam))) {
				return 0;
			}
			else {
				return inherited::WndProc (message, wParam, lParam);
			}
		}
		break;
		default: {
			return inherited::WndProc (message, wParam, lParam);
		}
		break;
	}
}
#endif

void	LedItView::OnContextMenu (const Led_Point& clickAt) 
{
	if (GetSupportContextMenu ()) {
		osMenuObj	menu	=	LoadContextMenu ();
		try {
			unsigned	cmdNum	=	TrackPopup (menu, clickAt);
			UnloadContextMenu (menu);
			menu = NULL;
			if (cmdNum != 0) {
				HandleCommand (cmdNum);
			}
		}
		catch (...) {
			if (menu != NULL) {
				UnloadContextMenu (menu);
			}
			throw;
		}
	}
}

#if		qMacOS
bool	LedItView::OnContextMenuKey (const EventRecord& inKeyEvent)
{
	if (inKeyEvent.modifiers & cmdKey) {
		osMenuObj	menu	=	LoadContextMenu ();
		try {
			unsigned	cmdNum	=	TrackMenuKey (menu, inKeyEvent.message & charCodeMask);
			UnloadContextMenu (menu);
			menu = NULL;
			if (cmdNum != 0) {
				HandleCommand (cmdNum);
			}
		}
		catch (...) {
			if (menu != NULL) {
				UnloadContextMenu (menu);
			}
			throw;
		}
		return true;
	}
	return false;
}
#elif	qWindows
bool	LedItView::OnContextMenuKey (UINT message, WPARAM wParam, LPARAM lParam)
{
	/*
	 *	Cons up a MSG structure, and pass that to TranslateAccelerator () to generate the appropriate command
	 *	events. The good news - is that WORKS! The had news - is that Netscape is already pre-grabbing all these
	 *	events through ITS accelerator table - so ITS getting the events for things like CMD-X etc. VERY BAD!
	 *	Not sure what I can do about it....
	 */
	MSG	msg;
	memset (&msg, 0, sizeof (msg));
	msg.hwnd = GetHWND ();
	msg.lParam = lParam;
	msg.wParam = wParam;
	msg.message = message;
	msg.time = GetTickCount ();//hack to see if fixes stuff??
	HACCEL	hAccelTable	=	::LoadAccelerators (ghInstance, MAKEINTRESOURCE (kAcceleratorTableID));
	return ::TranslateAccelerator (msg.hwnd, hAccelTable, &msg);
}
#endif

LedItView::osMenuObj	LedItView::LoadContextMenu ()
{
#if		qMacOS
	Led_Assert (fFontMenu == NULL);
	Led_Assert (fFontSizeMenu == NULL);
	Led_Assert (fFontStyleMenu == NULL);
	Led_Assert (fFontColorMenu == NULL);
	Led_Assert (fFontJustificationMenu == NULL);

	MenuHandle	menu	=	::GetMenu (kContextMenu);
	Led_AssertNotNil (menu);

	::MacInsertMenu (menu, -1);

	fFontMenu	=	::GetMenu (cmd_FontMenu);
	::MacInsertMenu (fFontMenu, -1);
	fFontSizeMenu	=	::GetMenu (cmd_SizeMenu);
	::MacInsertMenu (fFontSizeMenu, -1);
	fFontStyleMenu	=	::GetMenu (cmd_StyleMenu);
	::MacInsertMenu (fFontStyleMenu, -1);
	fFontColorMenu	=	::GetMenu (cmd_ColorMenu);
	::MacInsertMenu (fFontColorMenu, -1);
	fFontJustificationMenu	=	::GetMenu (cmd_JustificationMenu);
	::MacInsertMenu (fFontJustificationMenu, -1);

	// Add in static commands
	fCurMenuNumCmdMapRecList = vector<MenuNumCmdMapRec> (kDefaultMenuNumCmdMapRecs, kDefaultMenuNumCmdMapRecs + sizeof (kDefaultMenuNumCmdMapRecs)/sizeof (kDefaultMenuNumCmdMapRecs[0]));

	// Add in dynamic commands (FONTMENU)
	{
		::AppendResMenu (fFontMenu, 'FONT');
		size_t	nMenuItems	=	::CountMItems (fFontMenu);
		sFontIDMapCache.clear ();
		for (size_t i = 1; i <= nMenuItems; i++) {
			MenuNumCmdMapRec	mapRec;
			mapRec.fCmdNum = i-1+kBaseFontNameCmd;
			mapRec.fItemNum = i;
			mapRec.fMenuNum = cmd_FontMenu;
			fCurMenuNumCmdMapRecList.push_back (mapRec);	
			{
				Str255		pFontName = {0};
				::GetMenuItemText (fFontMenu, i, pFontName);
				short	familyID	=	0;
				::GetFNum (pFontName, &familyID);
				sFontIDMapCache.push_back (familyID);
			}
		}
	}

	// Strip disabled menu items...
	{
		list<MenuNumCmdMapRec>	badVec;
		vector<MenuNumCmdMapRec>	goodVec;

		// Separate cmdRecs to enabled/disabled. Then walk bad list (which should be short) - and for each item in it, see
		// if any items in good list need adjustment
		for (vector<MenuNumCmdMapRec>::const_iterator i = fCurMenuNumCmdMapRecList.begin (); i != fCurMenuNumCmdMapRecList.end (); ++i) {
			SimpleCommandUpdater	updater (My_CMDNUM_MAPPING::Get ().Lookup (i->fCmdNum));
			OnUpdateCommand (&updater);
			if (updater.fChecked) {
				osMenuObj	mh	=	::GetMenu (i->fMenuNum);
				Led_AssertNotNil (mh);
				::SetItemMark (mh, i->fItemNum, checkMark);
			}
			if (updater.fText.length () != 0) {
				osMenuObj	mh	=	::GetMenu (i->fMenuNum);
				Led_AssertNotNil (mh);
				Str255	p;
				p[0] = updater.fText.length ();
				(void)::memcpy (&p[1], updater.fText.c_str (), p[0]);
				::SetMenuItemText (mh, i->fItemNum, p);
			}
			if (updater.fEnabled) {
				goodVec.push_back (*i);
			}
			else {
				badVec.push_front (*i);
			}
		}
		// Strip disabled menu items.
		for (list<MenuNumCmdMapRec>::const_iterator i = badVec.begin (); i != badVec.end (); ++i) {
			// and actually strip the menu now!!!
			Led_AssertNotNil (::GetMenuHandle (i->fMenuNum));
			Led_Assert (::CountMenuItems (::GetMenuHandle (i->fMenuNum)) >= i->fItemNum);
			::DeleteMenuItem (::GetMenuHandle (i->fMenuNum), i->fItemNum);	// delete 
			for (vector<MenuNumCmdMapRec>::iterator j = goodVec.begin (); j != goodVec.end (); ++j) {
				if (i->fMenuNum == j->fMenuNum and i->fItemNum < j->fItemNum) {
					--j->fItemNum;	// cuz item prev-to it stripped
				}
			}
		}
		// Strip extraneous separators
		{
// Half-assed - only did in TOP-LEVEL menu
			size_t	nMenuItems	=	::CountMItems (fFontMenu);
			bool	lastItemSep	=	true;
			for (size_t i = nMenuItems; i >= 1; i--) {
				Str255	p;
				::GetMenuItemText (menu, i, p);
				bool	thisItemSep	=	p[0] == 1 and p[1] == '-';
				if (thisItemSep) {
					if (lastItemSep or i == 1) {
						::DeleteMenuItem (menu, i);	// delete 
						for (vector<MenuNumCmdMapRec>::iterator j = goodVec.begin (); j != goodVec.end (); ++j) {
							if (kContextMenu == j->fMenuNum and i < j->fItemNum) {
								--j->fItemNum;	// cuz item prev-to it stripped
							}
						}
					}
				}
				lastItemSep = thisItemSep;
			}
			
		}

		fCurMenuNumCmdMapRecList = goodVec;
	}

	return menu;
#elif	qWindows
	HMENU	menu		=	::LoadMenu (ghInstance, MAKEINTRESOURCE (kContextMenu));
	Led_AssertNotNil (menu);
	HMENU	realMenu	=	::GetSubMenu (menu, 0);
	Led_AssertNotNil (realMenu);
			// Sad to have to hardwire this number, but I'm not sure how to find it simply, and reliably dynamicly...
	FixupFontMenu (::GetSubMenu (realMenu, 14));
	return realMenu;
#endif
}

#if		qWindows
bool	LedItView::OnInitMenuPopup (HMENU popupMenu, UINT index, BOOL sysMenu)
{
	// Disable inappropriate commands.
	// Remove disabled menu items from the popup (to save space).
	// Remove consecutive (or leading) separators as useless.

	size_t	itemCount	=	::GetMenuItemCount (popupMenu);
	bool	lastItemSep	=	true;
	for (size_t i = itemCount; i > 0; --i) {
		UINT	cmdNum		= ::GetMenuItemID (popupMenu, i-1);
		bool	isSep		=	(cmdNum == 0);
		bool	isSubMenu	=	(cmdNum == -1);
		if (isSep) {
			if (lastItemSep or i == 1) {
				::RemoveMenu (popupMenu, i-1, MF_BYPOSITION);
				isSep = lastItemSep;		// If I delete a menu item, then my 'isSep' values becomes same as lastSep value...
			}
		}
		else if (not isSubMenu) {
			SimpleCommandUpdater	updater (My_CMDNUM_MAPPING::Get ().Lookup (cmdNum));
			OnUpdateCommand (&updater);
			if (not updater.fEnabled) {
				::RemoveMenu (popupMenu, i-1, MF_BYPOSITION);
				isSep = lastItemSep;		// If I delete a menu item, then my 'isSep' values becomes same as lastSep value...
			}
			if (updater.fChecked) {
				MENUITEMINFO	mii;
				memset (&mii, 0, sizeof (mii));
				mii.cbSize = sizeof (mii);
				mii.fMask = MIIM_STATE;
				mii.fState = MFS_CHECKED | MFS_ENABLED;
				Led_Verify (::SetMenuItemInfo (popupMenu, i-1, MF_BYPOSITION, &mii));
			}
			if (updater.fText.length () != 0) {
				MENUITEMINFO	mii;
				memset (&mii, 0, sizeof (mii));
				mii.cbSize = sizeof (mii);
				mii.fMask = MIIM_TYPE;
				mii.fType = MFT_STRING;
				mii.dwTypeData = const_cast<char*> (updater.fText.c_str ());
				mii.cch = updater.fText.length ();
				Led_Verify (::SetMenuItemInfo (popupMenu, i-1, MF_BYPOSITION, &mii));
			}
		}
		lastItemSep = isSep;
	}
	if (lastItemSep) {
		Led_Assert (::GetMenuItemCount (popupMenu) > 0);
		::RemoveMenu (popupMenu, 0, MF_BYPOSITION);
	}
	// return true iff HANDLED
	return true;
}
#endif

unsigned	LedItView::TrackPopup (osMenuObj menu, Led_Point clickedAt)
{
	#if		qMacOS
		return MapMacLResultToCmdNum (::PopUpMenuSelect (menu, clickedAt.v, clickedAt.h, 0));
	#elif	qWindows
		//TrackPopupMenu declared as returning BOOL - but really returns an INT!!!! (if you specify TPM_RETURN_CMD
		return ::TrackPopupMenu (menu, TPM_LEFTALIGN | TPM_RETURNCMD, clickedAt.h, clickedAt.v, 0, GetHWND (), NULL);
	#endif
}

void		LedItView::UnloadContextMenu (osMenuObj menu)
{
#if		qMacOS
	::MacDeleteMenu (cmd_FontMenu);
	::DisposeMenu (fFontMenu);
	fFontMenu = NULL;
	::MacDeleteMenu (cmd_SizeMenu);
	::DisposeMenu (fFontSizeMenu);
	fFontSizeMenu = NULL;
	::MacDeleteMenu (cmd_StyleMenu);
	::DisposeMenu (fFontStyleMenu);
	fFontStyleMenu = NULL;
	::MacDeleteMenu (cmd_ColorMenu);
	::DisposeMenu (fFontColorMenu);
	fFontColorMenu = NULL;
	::MacDeleteMenu (cmd_JustificationMenu);
	::DisposeMenu (fFontJustificationMenu);
	fFontJustificationMenu = NULL;

	::MacDeleteMenu (kContextMenu);
	::DisposeMenu (menu);
#elif	qWindows
// MUST DO MORE FOR /WIN!!!
#endif
}

#if		qMacOS
unsigned	LedItView::MapMacLResultToCmdNum (long menuResult) const
{
	int	menuNum	=	HiWord (menuResult);
	int	itemNum	=	LoWord (menuResult);

	for (vector<MenuNumCmdMapRec>::const_iterator i = fCurMenuNumCmdMapRecList.begin (); i != fCurMenuNumCmdMapRecList.end (); ++i) {
		if (menuNum == i->fMenuNum and itemNum == i->fItemNum) {
			return i->fCmdNum;
		}
	}
	Led_Assert (menuResult == 0);	// only way this can happen? If we have our table right???
	return 0;
}
#endif

#if		qMacOS
unsigned	LedItView::TrackMenuKey (osMenuObj /*menu*/, CharParameter ch)
{
	return MapMacLResultToCmdNum ( ::MenuKey (ch));
}
#endif

#if		qMacOS
void	LedItView::HandleMouseDownEvent (const EventRecord& inMouseDownEvent)
{
	if (inMouseDownEvent.modifiers & cmdKey) {
		OnContextMenu (Led_Point (inMouseDownEvent.where.v, inMouseDownEvent.where.h));
		return;
	}
	return inherited::HandleMouseDownEvent (inMouseDownEvent);
}

bool	LedItView::HandleKeyEvent (const EventRecord& inKeyEvent)
{
	if (OnContextMenuKey (inKeyEvent)) {
		return true;
	}
	return inherited::HandleKeyEvent (inKeyEvent);
}
#endif

void	LedItView::HandleCommand (unsigned cmdNum)
{
	try {
		if (not OnPerformCommand (cmdNum)) {
			#if		qWindows
			// Not sure what todo by default - but this is as good as anything???
			(void)::PostMessage (GetHWND (), WM_COMMAND, (unsigned short)cmdNum, 0);
			#endif
		}
	}
	STANDARD_NETLEDITAPPLICATION_CATCHERS();
}

bool	LedItView::OnUpdateCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	switch (enabler->GetCmdID ()) {
		case	kAboutBoxCmd:					enabler->SetEnabled (true); return true;
		case	ID_HELP:						enabler->SetEnabled (true); return true;
		case	kCheckForUpdatesWebPageCmdID:	enabler->SetEnabled (true); return true;
		default:								return inherited::OnUpdateCommand (enabler);
	}
}

bool	LedItView::OnPerformCommand (CommandNumber commandNumber)
{
	switch (commandNumber) {
		case	kAboutBoxCmd:					Led_AssertNotNil (fController); fController->OnAboutBoxCommand (); return true;
		case	ID_HELP:						Led_AssertNotNil (fController); fController->OnBrowseHelpCommand (); return true;
		case	kCheckForUpdatesWebPageCmdID:	Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/Led/CheckForUpdates.asp", kAppName, kURLDemoFlag)); return true;
		default:				return inherited::OnPerformCommand (My_CMDNUM_MAPPING::Get ().Lookup (commandNumber));
	}
}

void	LedItView::HandleBadAllocException () throw ()
{
	try {
		#if		qMacOS
			Led_StdAlertHelper dlg (kBadAllocExceptionOnCmdDialogID);
		#elif	qWindows
			Led_StdDialogHelper dlg (ghInstance, MAKEINTRESOURCE(kBadAllocExceptionOnCmdDialogID), ::GetActiveWindow ());
		#endif
		dlg.DoModal ();
	}
	catch (...) {
		Led_BeepNotify ();
	}
}

void	LedItView::HandleBadUserInputException () throw ()
{
	try {
		#if		qMacOS
			Led_StdAlertHelper dlg (kBadUserInputExceptionOnCmdDialogID);
		#elif	qWindows
			Led_StdDialogHelper dlg (ghInstance, MAKEINTRESOURCE(kBadUserInputExceptionOnCmdDialogID), ::GetActiveWindow ());
		#endif
		dlg.DoModal ();
	}
	catch (...) {
		Led_BeepNotify ();
	}
}

void	LedItView::HandleUnknownException () throw ()
{
	try {
		#if		qMacOS
			Led_StdAlertHelper dlg (kUnknownExceptionOnCmdDialogID);
		#elif	qWindows
			Led_StdDialogHelper dlg (ghInstance, MAKEINTRESOURCE(kUnknownExceptionOnCmdDialogID), ::GetActiveWindow ());
		#endif
		dlg.DoModal ();
	}
	catch (...) {
		Led_BeepNotify ();
	}
}

