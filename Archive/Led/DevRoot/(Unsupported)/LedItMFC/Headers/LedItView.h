/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */
#ifndef	__LedItView_h__
#define	__LedItView_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItMFC/Headers/LedItView.h,v 2.50 2000/10/18 21:00:19 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedItView.h,v $
 *	Revision 2.50  2000/10/18 21:00:19  lewis
 *	Lose FindDialog code - and inherit new sahred implm in Led_StdDialogHelper_FindDialog etc
 *	
 *	Revision 2.49  2000/10/04 15:46:34  lewis
 *	cleanups and remove unneeded CMDINFO overrides - due to moving stuff to common code (SPR#0839).
 *	
 *	Revision 2.48  2000/10/04 13:20:05  lewis
 *	lose some code (undo handlers etc) obsoleted by recent code sharing changes
 *	
 *	Revision 2.47  2000/10/03 13:42:23  lewis
 *	lose class WordProcessorCommonCommandHelper_MFCX- instead just WordProcessorCommonCommandHelper_MFC suffices. And lose OnUpdateCutCopyClearCommand stuff
 *	
 *	Revision 2.46  2000/09/30 19:36:06  lewis
 *	lose LedItView::OnSelectAllCommand/nUpdateCutCopyClearComma/OnUpdatePasteCommand etc since now handled by TextInteractorCommonCommandHelper_MFC
 *	
 *	Revision 2.45  2000/08/28 20:37:51  lewis
 *	SPR#0826- use new WordProcessorCommonCommandHelper_DefaultCmdInfo, and support kHideSelection_CmdID/unhide using new shared code.
 *	
 *	Revision 2.44  2000/08/13 03:38:18  lewis
 *	extra arg to AddEmebdding, and fixed the paragrphspacing dialogs - bugs
 *	
 *	Revision 2.43  2000/06/13 22:59:06  lewis
 *	cleanups. And SPR#0785- at least a crufty UI for adjusting line spacing params
 *	
 *	Revision 2.42  2000/04/26 16:01:34  lewis
 *	lose unneeded LedItView::InternalizeBestFlavor/inherited::HookLosingTextStore/LedItView::HookGainedNewTextStore overrides
 *	
 *	Revision 2.41  2000/04/24 17:28:21  lewis
 *	SPR#0743 use new Led_MFC_WordProcessor.h module. And lose override of InternalizeBestFlavor () - see SPR# 0744
 *	
 *	Revision 2.40  2000/04/14 19:08:50  lewis
 *	SPR#0739- Added FlavorPackage support
 *	
 *	Revision 2.39  2000/03/31 00:51:37  lewis
 *	SPR#0717 - more changes to adapt to new HiddenText support. Rip out some of the code I had added to LedItView - and instead keep a HiddenTextDatabase with the document (like the paragraphdatabase). Now we support REAADING RTF files with hidden text. Next todo is writing RTF with hidden text
 *	
 *	Revision 2.38  2000/03/30 16:28:35  lewis
 *	HiddenMarker code rename
 *	
 *	Revision 2.37  2000/03/18 03:45:36  lewis
 *	for proper safe destruction of the fHiddenTextOwner - must HookLosing/GainingTextStore as place to create/destroy - NOT in the OnInitialUpdate method
 *	
 *	Revision 2.36  2000/03/17 22:40:13  lewis
 *	SPR#0717- preliminary support - for showhiddentext testing and UI
 *	
 *	Revision 2.35  1999/12/28 17:19:01  lewis
 *	save prefrences for ShowPara/Tab/SpaceGlyphs.
 *	
 *	Revision 2.34  1999/12/27 17:32:57  lewis
 *	SPR#0669 - work on this, but not done. Just added most of the support so this command is autohandled by WordProcessorCommonCommandHelper<> template. And adjusted the characters output (at least for UNICODE).
 *	
 *	Revision 2.33  1999/12/24 00:56:21  lewis
 *	Lose LedItView::OnEraseBkgnd override and much of OnSize() override - cuz not handled automatically via new SetDefaultWindowMargins method - SPR#0679
 *	
 *	Revision 2.32  1999/12/12 16:32:36  lewis
 *	SPR#0651
 *	
 *	Revision 2.31  1999/12/08 17:46:51  lewis
 *	reverse args and add default args to WordProcessorSelectionCommandHelper_MFCX template
 *	
 *	Revision 2.30  1999/12/08 03:20:08  lewis
 *	cleanup and use new WordProcessorSelectionCommandHelper_MFCX wrapper
 *	
 *	Revision 2.29  1999/12/07 20:11:59  lewis
 *	remove alot of code for mananaging setting font selection menus etc. Moved to shared template WordProcessorSelectionCommandHelper<BASECLASS,CMD_ENABLER,CMD_INFO> as part of SPR#0606
 *	
 *	Revision 2.28  1999/07/16 21:10:22  lewis
 *	move some code from LedItMFC\LedItView to the template WordProcessorHScrollbarHelper<>, so that it can be shared in ActiveLedIt. REALLY - we should do much more of this sharing of code, but this is a start
 *	
 *	Revision 2.27  1999/03/01 15:29:53  lewis
 *	use new SetVHScrollbarType API instead of overriding PreCreateWindow to set WS_VSCROLL, and also lose (really moved into Led) gross hack I HAD needed for changing sbar type when I changed wrap-to-window flag
 *	
 *	Revision 2.26  1999/02/12 21:43:48  lewis
 *	Fix(hook in/finish implementing) horizontal scrolling, and make h-scrollbar disapear when in wrap-to-window mode.
 *	
 *	Revision 2.25  1999/02/08 22:31:34  lewis
 *	Support new optional 'wrap-to-window' feature. Used to be we ALWAYS did that. Now can be on/off. Next todo is to fix the OFF case to properly use ruler
 *	
 *	Revision 2.24  1998/10/30 14:50:15  lewis
 *	GetLayoutWidth() override no longer needed and OnCreate() wasnt for a while.
 *	
 *	Revision 2.23  1998/04/25  01:49:59  lewis
 *	Get/Set ParaFormatSelection - support for getting/setting tabstops from ruler
 *
 *	Revision 2.22  1997/12/24  04:46:26  lewis
 *	*** empty log message ***
 *
 *	Revision 2.21  1997/12/24  04:05:22  lewis
 *	No nore need to override OnCreatre() (drag/target done in LedMFC)
 *	GetCurSelFontSpec() added for fontbar.
 *	OnBarReturn added for fontbar support
 *
 *	Revision 2.20  1997/09/29  15:58:09  lewis
 *	Use WordProcessor instead of StandardStyledWordWrappedTextInteractor, and add menu updaters for
 *	justication (and cached justifccation etc) support.
 *	Changed SetWindowRect() override code (react to Led change).
 *
 *	Revision 2.19  1997/07/27  16:01:09  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.18  1997/07/23  23:14:19  lewis
 *	OnEraseBkgrnd () to implement margin.
 *
 *	Revision 2.17  1997/06/28  17:27:34  lewis
 *	Use new OnInitialUpdate () techniqu instead of global variable (better for MDI)
 *
 *	Revision 2.16  1997/06/24  03:42:58  lewis
 *	override GetLayoutWidth () as part of SPR#0450.
 *
 *	Revision 2.15  1997/06/18  03:40:19  lewis
 *	sub/superscript commands
 *	Include "WordProcessor.h" instead of "TextInteractorMixins.h"
 *
 *	Revision 2.14  1997/03/04  20:14:47  lewis
 *	Context menu. Disable cut/copy/etc commands with empty selection.
 *
 *	Revision 2.13  1997/01/20  05:32:01  lewis
 *	Support for font-color. Other cleanups
 *
 *	Revision 2.12  1997/01/10  03:27:14  lewis
 *	Use new Led_MFC_X<StandardStyledWordWrappedTextInteractor>
 *	override SetWindowRect() to SetLayoutWidth() (part of new scrolling support).
 *	throw specifiers.
 *
 *	Revision 2.11  1996/12/13  18:02:41  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.10  1996/12/05  21:16:12  lewis
 *	*** empty log message ***
 *
 *	Revision 2.9  1996/09/30  14:54:43  lewis
 *	Lose char* fLastSearchText etc. Moved to field of application (SearchParameters).
 *
 *	Revision 2.8  1996/09/01  15:43:38  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.7  1996/07/03  01:51:36  lewis
 *	Add GetSoleSelectedEmbedding method.
 *
 *	Revision 2.6  1996/06/01  02:33:47  lewis
 *	lose OnInitialUpdate.
 *	cleanups
 *	Lose onLBUttonDown override.
 *	override DidUpdateText?
 *
 *	Revision 2.5  1996/05/23  20:15:24  lewis
 *	Cleanups, added a few methods (mostly font menu related, a cache fCachedCurSelFontSpec
 *	for font menu etc).
 *
 *	Revision 2.4  1996/04/18  15:48:02  lewis
 *	Cleanups
 *	override InternalizeBestFlavor () to hook into new embedding flavor code.
 *
 *	Revision 2.3  1996/03/16  19:08:23  lewis
 *	Add OnEnterFindString/OnUpdateEnterFindStringCommand.
 *
 *	Revision 2.2  1996/02/05  04:40:04  lewis
 *	Lose OnReplaceCommand.
 *	Added support for OnFindCommand
 *	Added command support for setting fonts from menus (Format Menu).
 *	use typedef inherited.
 *	new baseclass - StandardStyledWordWrappedLed_MFC - not Led.
 *
 *	Revision 2.1  1996/01/22  05:37:46  lewis
 *	Lots of support for drag/drop.
 *	Fixed paste command enabling.
 *	Proto-find/replace support.
 *	Printing support fixed, and mostly moved to Led_MFC.
 *	Undo support.
 *
 *	Revision 2.0  1996/01/04  00:55:44  lewis
 *	*** empty log message ***
 *
 *	Revision 1.3  1996/01/04  00:55:22  lewis
 *	*** empty log message ***
 *
 *	Revision 1.2  1995/12/08  07:46:23  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */

#include	"Led_MFC_WordProcessor.h"


#include	"Resource.h"





struct	LedItViewCMDINFO :  public WordProcessorCommonCommandHelper_DefaultCmdInfo {
	public:
		typedef	Led_MFC_TmpCmdUpdater::CommandNumber		CommandNumber;
		enum {
			kUndo_CmdID						=	ID_EDIT_UNDO,
			kRedo_CmdID						=	ID_EDIT_REDO,
			kSelectAll_CmdID				=	ID_EDIT_SELECT_ALL,
			kCut_CmdID						=	ID_EDIT_CUT,
			kCopy_CmdID						=	ID_EDIT_COPY,
			kPaste_CmdID					=	ID_EDIT_PASTE,
			kClear_CmdID					=	ID_EDIT_CLEAR,
			kFind_CmdID						=	cmdFind,
			kFindAgain_CmdID				=	cmdFindAgain,
			kEnterFindString_CmdID			=	cmdEnterFindString
		};

	public:
		typedef	Led_FontSpecification::FontNameSpecifier	FontNameSpecifier;
		enum {
			kBaseFontSize_CmdID				=	kBaseFontSizeCmdID,
				kFontSize9_CmdID				=	kFontSize9CmdID,
				kFontSize10_CmdID				=	kFontSize10CmdID,
				kFontSize12_CmdID				=	kFontSize12CmdID,
				kFontSize14_CmdID				=	kFontSize14CmdID,
				kFontSize18_CmdID				=	kFontSize18CmdID,
				kFontSize24_CmdID				=	kFontSize24CmdID,
				kFontSize36_CmdID				=	kFontSize36CmdID,
				kFontSize48_CmdID				=	kFontSize48CmdID,
				kFontSize72_CmdID				=	kFontSize72CmdID,
				kFontSizeOther_CmdID				=	kFontSizeOtherCmdID,
				kFontSizeSmaller_CmdID				=	kFontSizeSmallerCmdID,
				kFontSizeLarger_CmdID				=	kFontSizeLargerCmdID,
			kLastFontSize_CmdID				=	kLastFontSizeCmdID,
		
			kBaseFontColor_CmdID			=	kBaseFontColorCmdID,
				kFontColorOther_CmdID			=	kFontColorOtherCmdID,
			kLastFontColor_CmdID			=	kLastFontColorCmdID,

			kFirstJustification_CmdID		=	kFirstJustificationCmdID,
				kJustifyLeft_CmdID				=	kJustifyLeftCmdID,
				kJustifyCenter_CmdID			=	kJustifyCenterCmdID,
				kJustifyRight_CmdID				=	kJustifyRightCmdID,
				kJustifyFull_CmdID				=	kJustifyFullCmdID,
			kLastJustification_CmdID		=	kLastJustificationCmdID,

			kParagraphSpacingCommand_CmdID	=	kParagraphSpacingCmdID,
			
			kFontMenuFirst_CmdID			=	cmdFontMenuFirst,
			kFontMenuLast_CmdID				=	cmdFontMenuLast,

			kFontStylePlain_CmdID			=	cmdFontStylePlain,
			kFontStyleBold_CmdID			=	cmdFontStyleBold,
			kFontStyleItalic_CmdID			=	cmdFontStyleItalic,
			kFontStyleUnderline_CmdID		=	cmdFontStyleUnderline,
			kFontStyleStrikeout_CmdID		=	cmdFontStyleStrikeout,
			kSubScriptCommand_CmdID			=	kSubScriptCommand,
			kSuperScriptCommand_CmdID		=	kSuperScriptCommand,
			
			kChooseFontCommand_CmdID		=	cmdChooseFontDialog,
			
			kHideSelection_CmdID			=	kHideSelectionCmd,
			kUnHideSelection_CmdID			=	kUnHideSelectionCmd,
			kFirstShowHideGlyph_CmdID		=	kFirstShowHideGlyphCmdID,
			kLastShowHideGlyph_CmdID		=	kLastShowHideGlyphCmdID,
			kShowHideParagraphGlyphs_CmdID	=	kShowHideParagraphGlyphsCmdID,
			kShowHideTabGlyphs_CmdID		=	kShowHideTabGlyphsCmdID,
			kShowHideSpaceGlyphs_CmdID		=	kShowHideSpaceGlyphsCmdID,
		};
};

struct	LedItViewCommandInfo : WordProcessorCommonCommandHelper_DefaultCmdImpl<LedItViewCMDINFO> {
	public:
		static		void				DisplayFindDialog (Led_tString* findText, bool* wrapSearch, bool* wholeWordSearch, bool* caseSensative, bool* pressedOK);
		static		FontNameSpecifier	CmdNumToFontName (CommandNumber cmdNum);
		static		Led_Distance		PickOtherFontHeight (Led_Distance origHeight);
		static		bool				PickNewParagraphLineSpacing (Led_TWIPS* spaceBefore, bool* spaceBeforeValid, Led_TWIPS* spaceAfter, bool* spaceAfterValid, Led_LineSpacing* lineSpacing, bool* lineSpacingValid);
		static		Led_Color			FontCmdToColor (CommandNumber cmd);
		static		CommandNumber		FontColorToCmd (Led_Color color);
};







class	LedItControlItem;
class	LedItDocument;



typedef	WordProcessorHScrollbarHelper <Led_MFC_X <WordProcessor> > LedItViewAlmostBASE;


#if		qQuiteAnnoyingDominanceWarnings
	#pragma warning (disable : 4250)
#endif
class	LedItView : public WordProcessorCommonCommandHelper_MFC <LedItViewAlmostBASE, LedItViewCommandInfo> {
	private:
		typedef	WordProcessorCommonCommandHelper_MFC <LedItViewAlmostBASE, LedItViewCommandInfo>	inherited;

	protected: // create from serialization only
		LedItView ();
		DECLARE_DYNCREATE (LedItView)

	public:
		virtual ~LedItView ();

	protected:
		override	void	OnInitialUpdate ();
	public:
		override	void	SetWindowRect (const Led_Rect& windowRect);

	protected:
		override	Led_Distance	CalculateFarthestRightMarginInWindow () const;

	public:
		nonvirtual	void	SetWrapToWindow (bool wrapToWindow);
	private:
		bool	fWrapToWindow;

	public:
		override	void	GetLayoutMargins (RowReference row, Led_Coordinate* lhs, Led_Coordinate* rhs) const;

	public:
		nonvirtual	LedItDocument&		GetDocument () const;

		nonvirtual	SimpleEmbeddedObjectStyleMarker*	GetSoleSelectedEmbedding () const;
		nonvirtual	LedItControlItem*					GetSoleSelectedOLEEmbedding () const;

	public:
		afx_msg		void	OnContextMenu (CWnd* /*pWnd*/, CPoint /*point*/);

	protected:
		override	BOOL	IsSelected (const CObject* pDocItem) const;	// support for CView/OLE

	public:
		nonvirtual		IncrementalParagraphInfo	GetParaFormatSelection ();
		nonvirtual		void						SetParaFormatSelection (const IncrementalParagraphInfo& pf);

	public:
		override	void	OnShowHideGlyphCommand (CommandNumber cmdNum);

	protected:
		override	SearchParameters	GetSearchParameters () const;
		override	void				SetSearchParameters (const SearchParameters& sp);

	public:
		nonvirtual	void	SetShowHiddenText (bool showHiddenText);

	protected:
		afx_msg void	OnSetFocus (CWnd* pOldWnd);
		afx_msg void	OnSize (UINT nType, int cx, int cy);
		afx_msg void	OnInsertObject ();
		afx_msg void	OnCancelEditCntr ();
		afx_msg void	OnCancelEditSrvr ();
		afx_msg	void	OnBarReturn (NMHDR*, LRESULT*);
		DECLARE_MESSAGE_MAP()


	#ifdef _DEBUG
	public:
		override	void	AssertValid () const;
		override	void	Dump (CDumpContext& dc) const;
	#endif
};
#if		qQuiteAnnoyingDominanceWarnings
	#pragma warning (default : 4250)
#endif




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
	#if		!qDebug
		inline	LedItDocument&	LedItView::GetDocument () const
	   		{
	   			return *(LedItDocument*)m_pDocument;
	   		}
	#endif

#endif	/*__LedItView_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

