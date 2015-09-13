/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */
#ifndef	__LedItView__
#define	__LedItView__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItPP/Headers/LedItView.h,v 2.29 2000/10/19 15:51:34 lewis Exp $
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
 *	$Log: LedItView.h,v $
 *	Revision 2.29  2000/10/19 15:51:34  lewis
 *	use new portable About code and Find code. Further cleanups/improvements.
 *	
 *	Revision 2.28  2000/10/04 15:50:51  lewis
 *	cleanups and remove unneeded CMDINFO overrides - due to moving stuff to common code (SPR#0839).
 *	
 *	Revision 2.27  2000/10/03 21:51:11  lewis
 *	Lots more work relating to SPR#0839. Eliminated much duplicated code
 *	
 *	Revision 2.26  2000/08/31 20:05:04  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.25  2000/06/14 13:59:50  lewis
 *	get (hopefully) compiling with new PickNewParagraphLineSpacing() cmd enabled- SPR#0785
 *	
 *	Revision 2.24  2000/04/30 14:52:23  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.23  1999/12/28 19:35:33  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.22  1999/12/27 17:32:44  lewis
 *	SPR#0669 - work on this, but not done. Just added most of the support so this command is autohandled by WordProcessorCommonCommandHelper<> template. And adjusted the characters output (at least for UNICODE).
 *	
 *	Revision 2.21  1999/12/14 18:56:46  lewis
 *	added kChooseFontCommand_CmdID
 *	
 *	Revision 2.20  1999/12/08 19:43:00  lewis
 *	Finish removing code that is replaced by new WordProcessorSelectionCommandHelper_PP(X)<> template - SPR#0606
 *	
 *	Revision 2.19  1999/12/08 17:46:13  lewis
 *	move some stuff to Led_PP.h and comment out more code as wev'e templateized it
 *	
 *	Revision 2.18  1999/12/08 03:17:53  lewis
 *	More progress towards getting new WordProcessorSelectionCommandHelper_PPX stuff working (SPR#0606).
 *	
 *	Revision 2.17  1999/12/07 22:38:56  lewis
 *	add LedItViewCommandInfo.  Related to spr#0606
 *	
 *	Revision 2.16  1998/10/30 15:01:23  lewis
 *	Use default template arg (Led_PPView_Traits_Default).
 *	Partial support for chanign to new GetlayoutMargins() code - and supporting margin setting.
 *	May never be a UI on Mac applet though for this.
 *	
 *	Revision 2.15  1997/12/24  04:41:30  lewis
 *	*** empty log message ***
 *
 *	Revision 2.14  1997/09/29  17:56:32  lewis
 *	subclass from WordProcessor instead of StandardStyledWordWrappedTextInteractor.
 *	Add support for justification.
 *
 *	Revision 2.13  1997/07/27  16:01:58  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.12  1997/06/24  03:36:31  lewis
 *	Now override GetLayoutWidth () - see spr#0450.
 *
 *	Revision 2.11  1997/06/18  03:30:03  lewis
 *	Include "WordProcessor.h" instead of "TextInteractorMixins.h"
 *
 *	Revision 2.10  1997/01/20  05:42:27  lewis
 *	Added PickOtherFontColor helper routine.
 *
 *	Revision 2.9  1997/01/10  03:44:01  lewis
 *	New base class - use Led_PPView_X<StandardStyledWordWrappedTextInteractor,...>
 *	throw sepcifiers.
 *	override SetWindowRect () - part of new scrolling support.
 *
 *	Revision 2.8  1996/12/13  18:11:04  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.7  1996/12/05  21:23:57  lewis
 *	*** empty log message ***
 *
 *	Revision 2.6  1996/09/30  15:07:36  lewis
 *	Fixed inheritted --> inherited typo. Override DidUpdateText(), and
 *	lose fLastSearchText etc - moved to SearchParameters in Application object.
 *
 *	Revision 2.5  1996/09/01  15:45:23  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.4  1996/07/03  01:38:14  lewis
 *	Add GetSoleSelectedEmbedding()
 *
 *	Revision 2.3  1996/05/23  20:43:43  lewis
 *	Speed tweeks fCachedCurSelFontSpec - for font menu updating.
 *	Support for PickOtherFontHeight ().
 *
 *	Revision 2.2  1996/03/16  19:16:56  lewis
 *	Added OnEnterFindStringCommand
 *
 *	Revision 2.1  1996/03/05  18:58:27  lewis
 *	Now inherit from StandardStyledWordWrappedLed_PPView instead of
 *	Led_PP.
 *
 *	Revision 2.0  1996/02/26  23:29:06  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */

#include	"Led_PP_WordProcessor.h"

#include	"LedItConfig.h"
#include	"LedItResources.h"



// Someday, maybe improve the UI of the Mac Demo app to include a toolbar. But for now - wrap to the window size.
// Anyhow - this is good demo code - as it shows just how to make this choice easily with Led.
// LGP 980831
const	bool	kWrapToWindow	=	true;
//const	bool	kWrapToWindow	=	false;






class	LedItViewCMDINFO : public WordProcessorCommonCommandHelper_DefaultCmdInfo {
	public:
		typedef	Led_PP_TmpCmdUpdater::CommandNumber			CommandNumber;
		enum	{
			kUndo_CmdID						=	cmd_Undo,
			kRedo_CmdID						=	cmdRedo,
			kSelectAll_CmdID				=	cmd_SelectAll,
			kCut_CmdID						=	cmd_Cut,
			kCopy_CmdID						=	cmd_Copy,
			kPaste_CmdID					=	cmd_Paste,
			kClear_CmdID					=	cmd_Clear,
			kFind_CmdID						=	cmdFind,
			kFindAgain_CmdID				=	cmdFindAgain,
			kEnterFindString_CmdID			=	cmdEnterFindString
		};

	public:
		typedef	Led_FontSpecification::FontNameSpecifier	FontNameSpecifier;

	public:
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
				kFontColorBlack_CmdID			=	kFontColorBlackCmdID,
				kFontColorRed_CmdID				=	kFontColorRedCmdID,
				kFontColorGreen_CmdID			=	kFontColorGreenCmdID,
				kFontColorBlue_CmdID			=	kFontColorBlueCmdID,
				kFontColorCyan_CmdID			=	kFontColorCyanCmdID,
				kFontColorMagenta_CmdID			=	kFontColorMagentaCmdID,
				kFontColorYellow_CmdID			=	kFontColorYellowCmdID,
				kFontColorOther_CmdID			=	kFontColorOtherCmdID,
			kLastFontColor_CmdID			=	kLastFontColorCmdID,

			kFirstJustification_CmdID		=	kJustifyLeftCmdID,
				kJustifyLeft_CmdID				=	kJustifyLeftCmdID,
				kJustifyCenter_CmdID			=	kJustifyCenterCmdID,
				kJustifyRight_CmdID				=	kJustifyRightCmdID,
				kJustifyFull_CmdID				=	kJustifyFullCmdID,
			kLastJustification_CmdID		=	kJustifyFullCmdID,

			kParagraphSpacingCommand_CmdID	=	kParagraphSpacingCmdID,
			
			kFontMenuFirst_CmdID			=	kBaseFontNameCmdID,
			kFontMenuLast_CmdID				=	kLastFontNameCmdID,
			
			kFontStylePlain_CmdID			=	cmd_Plain,
			kFontStyleBold_CmdID			=	cmd_Bold,
			kFontStyleItalic_CmdID			=	cmd_Italic,
			kFontStyleUnderline_CmdID		=	cmd_Underline,
			kFontStyleOutline_CmdID			=	cmd_Outline,
			kFontStyleShadow_CmdID			=	cmd_Shadow,
			kFontStyleCondensed_CmdID		=	cmd_Condense,
			kFontStyleExtended_CmdID		=	cmd_Extend,
//			kFontStyleStrikeout_CmdID		=	0,						// UNUSED on Mac for now... LGP991207
			kSubScriptCommand_CmdID			=	kSubScriptCommand,
			kSuperScriptCommand_CmdID		=	kSuperScriptCommand,

//			kChooseFontCommand_CmdID		=	0,						//	DITTO

			kFirstShowHideGlyph_CmdID		=	kFirstShowHideGlyphCmdID,
			kLastShowHideGlyph_CmdID		=	kLastShowHideGlyphCmdID,
			kShowHideParagraphGlyphs_CmdID	=	kShowHideParagraphGlyphsCmdID,
			kShowHideTabGlyphs_CmdID		=	kShowHideTabGlyphsCmdID,
			kShowHideSpaceGlyphs_CmdID		=	kShowHideSpaceGlyphsCmdID
		};
};

struct	LedItViewCommandInfo : WordProcessorCommonCommandHelper_DefaultCmdImpl<LedItViewCMDINFO> {
	public:
		static		void				DisplayFindDialog (Led_tString* findText, bool* wrapSearch, bool* wholeWordSearch, bool* caseSensative, bool* pressedOK);
		static		FontNameSpecifier	CmdNumToFontName (CommandNumber cmdNum);
		static		Led_Distance		PickOtherFontHeight (Led_Distance origHeight);
		static		bool				PickNewParagraphLineSpacing (Led_TWIPS* spaceBefore, bool* spaceBeforeValid, Led_TWIPS* spaceAfter, bool* spaceAfterValid, Led_LineSpacing* lineSpacing, bool* lineSpacingValid);
};




class	LedItView : public WordProcessorCommonCommandHelper_PP<Led_PPView_X<WordProcessor>,LedItViewCommandInfo> {
	private:
		typedef	WordProcessorCommonCommandHelper_PP<Led_PPView_X<WordProcessor>,LedItViewCommandInfo>	inherited;

	public:
	 	LedItView ();
	 	~LedItView ();

	public:
		override	void	GetLayoutMargins (RowReference row, Led_Coordinate* lhs, Led_Coordinate* rhs) const;
		override	void	SetWindowRect (const Led_Rect& windowRect);

	public:
		override	void	FindCommandStatus (CommandT inCommand, Boolean& outEnabled, Boolean& outUsesMark, Char16& outMark, Str255 outName);
	//	override	Boolean	ObeyCommand (CommandT inCommand, void* ioParam);
	
	public:
		nonvirtual	SimpleEmbeddedObjectStyleMarker*	GetSoleSelectedEmbedding () const;

	protected:
		override	SearchParameters	GetSearchParameters () const;
		override	void				SetSearchParameters (const SearchParameters& sp);

#if 0
	public:
		nonvirtual	void	OnFindCommand ();
		nonvirtual	void	OnFindAgainCommand ();
		nonvirtual	void	OnEnterFindStringCommand ();
#endif
};

#endif	/*__LedItView__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***


