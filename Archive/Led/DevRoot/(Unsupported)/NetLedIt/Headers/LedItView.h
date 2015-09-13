/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LedItView_h__
#define	__LedItView_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/NetLedIt/Headers/LedItView.h,v 1.1 2004/01/01 04:20:21 lewis Exp $
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
 *	Revision 1.1  2004/01/01 04:20:21  lewis
 *	moved NetLedIt to (Unsupported)
 *	
 *	Revision 1.30  2003/06/10 14:39:07  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.29  2003/04/05 16:01:51  lewis
 *	SPR#1407: support new command handling code - and normalize #define names a bit across projects
 *	
 *	Revision 1.28  2003/01/24 02:39:30  lewis
 *	fix small compile error
 *	
 *	Revision 1.27  2002/05/06 21:34:56  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 1.26  2001/11/27 00:32:57  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.25  2001/09/26 15:53:03  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.24  2001/09/24 16:26:47  lewis
 *	SPR#0993- as part of fixing scrollbar flicker with autoshow - lose AlignTextEditRects
 *	and instead call SetAlwaysLeaveSpaceForSizeBox/SetScrollBarsOverlapOwningFrame
 *	
 *	Revision 1.23  2001/09/24 14:25:34  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.22  2001/09/19 18:44:42  lewis
 *	prelim support for catching exceptions on commands (kBallAllocException etc in LedItView)
 *	
 *	Revision 1.21  2001/08/30 01:10:15  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.20  2001/08/30 00:43:17  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.19  2001/08/27 13:44:29  lewis
 *	SPR#0972- add ListStyle popup menu (windows only), and fix LedItView::GetLayoutMargins ()
 *	so bullets display properly
 *	
 *	Revision 1.18  2001/08/17 17:03:02  lewis
 *	fix Windows to include unknown etc embedding picts/bmps. SPR#0959 - GetInfo support Mac and
 *	Windows (untested on mac yet)
 *	
 *	Revision 1.17  2001/05/29 23:01:06  lewis
 *	SPR#0943- fix to use new color list from combined LedIt! app. Fix a few names/color values.
 *	Fix Windows code to update command names (as in Undo XXX) and checkmarks.
 *	
 *	Revision 1.16  2001/04/12 19:46:39  lewis
 *	SPR#0861- Expiration support for demo-mode
 *	
 *	Revision 1.15  2000/10/18 20:58:36  lewis
 *	fixed windoze removemenu DUP menu items on Windows code. Added hooks into new
 *	Led_StdDialogHelper_FindDialog code
 *	
 *	Revision 1.14  2000/10/14 14:31:30  lewis
 *	start implementing help/aboutbox for Mac/Windows. Got the cmds hooked in. Only tested
 *	on Windows (help). Still to write is aboutbox code (for both platoforms)
 *	
 *	Revision 1.13  2000/10/06 12:59:55  lewis
 *	WordProcessorCommonCommandHelper<> and hook various methods to implement
 *	wrap-to-window/no-wrap-towindow functionalty and make hscrolling work right
 *	
 *	Revision 1.12  2000/10/05 02:40:25  lewis
 *	last message wrong - really added a bunch more support for Java-called methods
 *	(from javascript/netscape). And much more
 *	
 *	Revision 1.11  2000/10/05 02:38:53  lewis
 *	fix javascript to by syntactic, and tested (most) all the functions here
 *	
 *	Revision 1.10  2000/10/04 18:27:33  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.9  2000/10/04 15:46:24  lewis
 *	cleanups and remove unneeded CMDINFO overrides - due to moving stuff to common code (SPR#0839).
 *	
 *	Revision 1.8  2000/10/04 13:19:38  lewis
 *	use new WordProcessorCommonCommandHelper_DefaultCmdImpl<BASECLASS> code more. Implemented
 *	font menu and hiding idsabled context menu items on PC
 *	
 *	Revision 1.7  2000/10/03 21:52:53  lewis
 *	Lots more work relating to SPR#0839. Got a bit more of the context menu stuff
 *	working (about 75% now working on the mac)
 *	
 *	Revision 1.6  2000/10/03 13:40:19  lewis
 *	SPR#0840- new WaterMarkHelper<> template for watermark/demomode. SPR#0839- use new
 *	WordProcessorCommonCommandHelper/TextInteractorCommonCommandHelper<> templates.
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
 *	Revision 1.2  2000/09/29 00:18:50  lewis
 *	override AlignTextEditRects() to eliminate space for growbox etc
 *	
 *	Revision 1.1  2000/09/28 21:03:39  lewis
 *	Create separeate LedItView module, and move stuff in there, and start cloning/copying
 *	stuff from ActiveLedIt!
 *	
 *
 *
 *
 */

#include	<time.h>

#if		qWindows
#include	<atlbase.h>
#endif

#include	"ChunkedArrayTextStore.h"
#include	"LedOptionsSupport.h"
#include	"WordProcessor.h"

#include	"Resource.h"

#if		qWindows
#include	"Led_Win32.h"
#elif	qMacOS
#include	"Led_MacOS.h"
#endif




class	LedItViewController {
	public:
		LedItViewController ();
		~LedItViewController ();

	public:
		virtual	void	OnBrowseHelpCommand ()								=	0;
		virtual	void	OnAboutBoxCommand ()								=	0;

	#if		qMacOS
	public:
		override	void	UpdateWindowRect_ (const Led_Rect& windowRectArea) const	=	0;
		override	void	SetupCurrentGrafPort () const								=	0;
	#endif

	public:
		ChunkedArrayTextStore							fTextStore;
		MultiLevelUndoCommandHandler					fCommandHandler;
		WordProcessor::HidableTextDatabasePtr			fHidableTextDatabase;
};





#if 0
class	MyTmpCmdUpdater {
	public:
		explicit MyTmpCmdUpdater (CommandNumber cmdNum): fCmdNum (cmdNum),fEnabled(false),fItemChecked (false),fItemText () {}
	public:
		nonvirtual	CommandNumber	GetCmdID () const { return fCmdNum; };
		nonvirtual	void			SetEnabled (bool enabled) { fEnabled = enabled;}
		nonvirtual	void			SetChecked (bool checked) { fItemChecked = checked; }
		nonvirtual	void			SetText (const Led_SDK_Char* text) {fItemText = text; }


	public:
		CommandNumber	fCmdNum;
		bool			fEnabled;
		bool			fItemChecked;
		Led_SDK_String	fItemText;
};
#endif




#if 0
struct	LedItViewCMDINFO : public WordProcessorCommonCommandHelper_DefaultCmdInfo {
	public:
		typedef	MyTmpCmdUpdater::CommandNumber		CommandNumber;

	public:
		enum	{
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

		enum	{
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

			kBaseFontColor_CmdID			=	kBaseFontColorCmd,
				kFontColorBlack_CmdID			=	kBlackColorCmd,
				kFontColorMaroon_CmdID			=	kMaroonColorCmd,
				kFontColorGreen_CmdID			=	kGreenColorCmd,
				kFontColorOlive_CmdID			=	kOliveColorCmd,
				kFontColorNavy_CmdID			=	kNavyColorCmd,
				kFontColorPurple_CmdID			=	kPurpleColorCmd,
				kFontColorTeal_CmdID			=	kTealColorCmd,
				kFontColorGray_CmdID			=	kGrayColorCmd,
				kFontColorSilver_CmdID			=	kSilverColorCmd,
				kFontColorRed_CmdID				=	kRedColorCmd,
				kFontColorLime_CmdID			=	kLimeColorCmd,
				kFontColorYellow_CmdID			=	kYellowColorCmd,
				kFontColorBlue_CmdID			=	kBlueColorCmd,
				kFontColorFuchsia_CmdID			=	kFuchsiaColorCmd,
				kFontColorAqua_CmdID			=	kAquaColorCmd,
				kFontColorWhite_CmdID			=	kWhiteColorCmd,
			kLastNamedFontColor_CmdID		=	kLastFontNamedColorCmd,
				kFontColorOther_CmdID			=	kFontColorOtherCmd,
			kLastFontColor_CmdID			=	kFontColorOtherCmd,
		
			kFirstJustification_CmdID		=	kFirstJustificationCmdID,
				kJustifyLeft_CmdID				=	kJustifyLeftCmdID,
				kJustifyCenter_CmdID			=	kJustifyCenterCmdID,
				kJustifyRight_CmdID				=	kJustifyRightCmdID,
				kJustifyFull_CmdID				=	kJustifyFullCmdID,
			kLastJustification_CmdID		=	kLastJustificationCmdID,
		
//NOT YET SUPPORTED...			kParagraphSpacingCommand_CmdID	=	kParagraphSpacingCmdID,

	
			kFirstListStyle_CmdID			=	kListStyle_NoneCmd,
				kListStyle_None_CmdID			=	kListStyle_NoneCmd,
				kListStyle_Bullet_CmdID			=	kListStyle_BulletCmd,
			kLastListStyle_CmdID			=	kListStyle_BulletCmd,

			
			kFontMenuFirst_CmdID			=	cmdFontMenuFirst,
			kFontMenuLast_CmdID				=	cmdFontMenuLast,
			
			kFontStylePlain_CmdID			=	cmdFontStylePlain,
			kFontStyleBold_CmdID			=	cmdFontStyleBold,
			kFontStyleItalic_CmdID			=	cmdFontStyleItalic,
			kFontStyleUnderline_CmdID		=	cmdFontStyleUnderline,
			kFontStyleStrikeout_CmdID		=	cmdFontStyleStrikeout,
			kFontStyleOutline_CmdID			=	cmdFontStyleOutline,
			kFontStyleShadow_CmdID			=	cmdFontStyleShadow,
			kFontStyleCondensed_CmdID		=	cmdFontStyleCondense,
			kFontStyleExtended_CmdID		=	cmdFontStyleExtend,
			kSubScriptCommand_CmdID			=	kSubScriptCommand,
			kSuperScriptCommand_CmdID		=	kSuperScriptCommand,

			#if		qWindows
				kChooseFontCommand_CmdID		=	cmdChooseFontDialog,
			#endif

			kFirstSelectedEmbedding_CmdID	=	kFirstSelectedEmbeddingCmd,
				kFirstPrivateEmbedding_CmdID	=	kFirstPrivateEmbeddingCmd,
				kSelectedEmbeddingProperties_CmdID	=	kPropertiesForSelectionCmd,
			kLastSelectedEmbedding_CmdID	=	kLastSelectedEmbeddingCmd
		};
};


struct	LedItViewCommandInfo : WordProcessorCommonCommandHelper_DefaultCmdImpl<LedItViewCMDINFO> {
	public:
		static		void				DisplayFindDialog (Led_tString* findText, bool* wrapSearch, bool* wholeWordSearch, bool* caseSensative, bool* pressedOK);
		static		FontNameSpecifier	CmdNumToFontName (CommandNumber cmdNum);
		static		Led_Distance		PickOtherFontHeight (Led_Distance origHeight);
		static		bool				PickNewParagraphLineSpacing (Led_TWIPS* spaceBefore, bool* spaceBeforeValid, Led_TWIPS* spaceAfter, bool* spaceAfterValid, Led_LineSpacing* lineSpacing, bool* lineSpacingValid);
		static		void				ShowSimpleEmbeddingInfoDialog (const Led_SDK_String& embeddingTypeName);
		static		bool				ShowURLEmbeddingInfoDialog (const Led_SDK_String& embeddingTypeName, Led_SDK_String* urlTitle, Led_SDK_String* urlValue);
};
#endif




#if		qMacOS
	typedef	Led_MacOS_Helper<WordProcessorHScrollbarHelper <WordProcessor> >									MyBase;
#elif	qWindows
	typedef	Led_Win32_SimpleWndProc_Helper<Led_Win32_Helper<WordProcessorHScrollbarHelper <WordProcessor> > >	MyBase;
#endif


class	LedItView : public MyBase {
	private:
		typedef	MyBase	inherited;
	public:
		LedItView ();
		~LedItView ();
			
	public:
		nonvirtual	void	SetController (LedItViewController* controller);
	private:
		LedItViewController*	fController;


	#if		qDemoMode
		public:
			override	void	EraseBackground (Led_Tablet tablet, const Led_Rect& subsetToDraw, bool printing);
	#endif
	#if		qDemoMode
		public:
			override	void	AboutToUpdateText (const UpdateInfo& updateInfo);
	#endif

	#if		qMacOS
	public:
		override	void	UpdateWindowRect_ (const Led_Rect& windowRectArea) const;
		override	void	SetupCurrentGrafPort () const;
	#endif

#if		qMacOS
	public:
		override	void	HandleMouseDownEvent (const EventRecord& inMouseDownEvent);
		override	bool	HandleKeyEvent (const EventRecord& inKeyEvent);
#endif


#if		qMacOS
	private:
		struct	MenuNumCmdMapRec {
			int	fMenuNum;
			int	fItemNum;
			int	fCmdNum;
		};
	private:
		vector<MenuNumCmdMapRec>	fCurMenuNumCmdMapRecList;
	private:
		static	MenuNumCmdMapRec	kDefaultMenuNumCmdMapRecs[];

#endif
	private:
		nonvirtual	void	HandleCommand (unsigned cmdNum);


#if		qWindows
	public:
		override	LRESULT	WndProc (UINT message, WPARAM wParam, LPARAM lParam);
#endif

	public:
		nonvirtual	bool	GetSupportContextMenu () const;
		nonvirtual	void	SetSupportContextMenu (bool allowContextMenu);
	private:
		bool	fSupportContextMenu;

	public:
		nonvirtual	bool	GetWrapToWindow () const;
		nonvirtual	void	SetWrapToWindow (bool wrapToWindow);
	private:
		bool	fWrapToWindow;

	public:
		override	void	GetLayoutMargins (RowReference row, Led_Coordinate* lhs, Led_Coordinate* rhs) const;
		override	void	SetWindowRect (const Led_Rect& windowRect);
	public:
		override	Led_Distance	CalculateFarthestRightMarginInWindow () const;

	public:
		nonvirtual	long	GetMaxLength () const;
		nonvirtual	void	SetMaxLength (long maxLength);
	private:
		long	fMaxLength;

	private:
		nonvirtual	void	OnContextMenu (const Led_Point& clickAt);
		#if		qMacOS
			nonvirtual	bool	OnContextMenuKey (const EventRecord& inKeyEvent);
		#elif	qWindows
			nonvirtual	bool	OnContextMenuKey (UINT message, WPARAM wParam, LPARAM lParam);
		#endif

	private:
#if		qMacOS
		typedef	MenuHandle	osMenuObj;
#elif	qWindows
		typedef	HMENU		osMenuObj;
#endif
		nonvirtual	osMenuObj	LoadContextMenu ();
		#if		qWindows
		nonvirtual	bool		OnInitMenuPopup (HMENU popupMenu, UINT index, BOOL sysMenu);
		#endif
		nonvirtual	unsigned	TrackPopup (osMenuObj menu, Led_Point clickedAt);	// return 0 if no cmd
		nonvirtual	void		UnloadContextMenu (osMenuObj menu);
#if		qMacOS
	private:
		MenuHandle	fFontMenu;
		MenuHandle	fFontSizeMenu;
		MenuHandle	fFontStyleMenu;
		MenuHandle	fFontColorMenu;
		MenuHandle	fFontJustificationMenu;
#endif
		
#if		qMacOS
		nonvirtual	unsigned	MapMacLResultToCmdNum (long menuResult) const;
		nonvirtual	unsigned	TrackMenuKey (osMenuObj menu, CharParameter ch);	// return 0 if no cmd
#endif


	public:
		override	bool	OnUpdateCommand (CommandUpdater* enabler);
		override	bool	OnPerformCommand (CommandNumber commandNumber);


	public:
		nonvirtual	void	HandleBadAllocException () throw ();
		nonvirtual	void	HandleBadUserInputException () throw ();
		nonvirtual	void	HandleUnknownException () throw ();
};




#if		qDemoMode && qWindows
class	DemoPrefs : public OptionsFileHelper {
	public:
		class	RegistryEntryNotFound {};
		enum	{ kDaysToExpire = 7 };

	public:
		DemoPrefs ():
			OptionsFileHelper (OpenTheKey ())
			{
			}
		HKEY	OpenTheKey ()
			{
				CRegKey	softwarePrefs;
				softwarePrefs.Open (HKEY_CURRENT_USER, _T ("Software"));
				if (softwarePrefs == NULL) {
					throw RegistryEntryNotFound ();
				}
				CRegKey	sophistsKey;
				sophistsKey.Create (softwarePrefs, _T ("Sophist Solutions, Inc."));
				if (sophistsKey == NULL) {
					throw RegistryEntryNotFound ();
				}
				CRegKey	commonPrefsKey;
				commonPrefsKey.Create (sophistsKey, _T ("NetLedIt"));
				if (commonPrefsKey == NULL) {
					throw RegistryEntryNotFound ();
				}
				return commonPrefsKey.Detach ();
			}
		static	inline	unsigned	GetDemoDaysLeftHelper (unsigned demoExpiresAt)
			{
				time_t		curTime		=	time (NULL);
				if (demoExpiresAt < curTime) {
					return 0;
				}
				unsigned	dayCount	=	(demoExpiresAt - curTime)/ (60*60*24);
				dayCount += 1;	// cuz above code rounds down
				if (dayCount > kDaysToExpire+1) {
					// cheaters never propser!
					return 0;
				}
				return dayCount;
			}
		UINT	GetDemoDaysLeft ()
			{
				string	demoVersion;
				int		demoExpiresAt	=	0;
				if (LookupPref ("DemoExpiresAt", &demoExpiresAt) and LookupPref ("DemoVersionString", &demoVersion) and demoVersion == qLed_ShortVersionString) {
					return GetDemoDaysLeftHelper (demoExpiresAt);
				}
				else {
					demoExpiresAt = time (NULL) + kDaysToExpire*24*60*60;
					StorePref ("DemoExpiresAt", demoExpiresAt);
					StorePref ("DemoVersionString", qLed_ShortVersionString);
					return kDaysToExpire;
				}
			}
};
#endif



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


//	class	LedItViewController

//	class	LedItView
	inline	bool	LedItView::GetWrapToWindow () const
		{
			return fWrapToWindow;
		}
	inline	bool	LedItView::GetSupportContextMenu () const
		{
			return fSupportContextMenu;
		}
	inline	long	LedItView::GetMaxLength () const
		{
			return fMaxLength;
		}

#endif	/*__LedItView_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

