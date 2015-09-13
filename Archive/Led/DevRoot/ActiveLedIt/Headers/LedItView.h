/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LedItView_h__
#define	__LedItView_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/ActiveLedIt/Headers/LedItView.h,v 2.70 2004/01/30 04:36:29 lewis Exp $
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
 *	Revision 2.70  2004/01/30 04:36:29  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.69  2004/01/25 21:51:42  lewis
 *	SPR#1284: Added new PasteText command (not part of default menus - but builtin. Added example of
 *	how to use this to the HTML test scriptCommandSupportTester.html). Add extra arg to _AcceleratorTable::Add ()
 *	for position to insert. Added IndexOf () method (to COM API - for IALCommandList. Implement the
 *	IALCommandList::Remove method.
 *	
 *	Revision 2.68  2004/01/23 23:11:16  lewis
 *	SPR#1617: Added preliminary (largely functional) implementation of OLE automation-based context
 *	menus. Tons of new OLE APIs. Can create context menus from scratch, or built from building
 *	blocks of existing submenus (like font menu).
 *	
 *	Revision 2.67  2003/12/31 03:49:43  lewis
 *	SPR#1602: use new OptionsFileHelper support to implement saving find strings in registry
 *	
 *	Revision 2.66  2003/11/06 16:39:21  lewis
 *	SPR#1560: lose WordProcessorHScrollbarHelper<>
 *	
 *	Revision 2.65  2003/06/10 14:31:13  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.64  2003/05/30 14:26:14  lewis
 *	SPR#1517: Added find/replace dialog and OLE_FindReplace and OLE_LaunchReplaceDialog
 *	new OLE AUTOMATION methods
 *	
 *	Revision 2.63  2003/05/29 14:56:36  lewis
 *	SPR#1510: dont call GetClientRect in draw - but use WindowRect/GetDefaultWindowMargins
 *	since we may have been called from metafile draw or other. Also - added DrawExtraDesignModeBorder ()
 *	so we don't display border in some cases where it looks ugly (not really related to SPR#1510 - but
 *	I noticed it while debugging and looking at display in MSVC.Net 2003 dialog editor). Careful about
 *	call to GetStyle (since vectors to MFC version in LedItView) - and that fails if window is NULL
 *	(as it is sometimes in MSVC .Net 2003 dialog editor)
 *	
 *	Revision 2.62  2003/05/28 16:56:18  lewis
 *	SPR#1509: Added qFunnyDisplayInDesignMode define to ActiveLedItConfig, and ifdefined (yes
 *	by default), then check for AMBIENT_DESIGN_MODE and change the drawing in LedItView::EraseBackground ()
 *	slightly. Added a small watermarkdisplay saying 'design mode' and a border in the color opposite to the
 *	background. Hooked OnAmbientPropertyChange() to detect change and refresh screen.
 *	
 *	Revision 2.61  2003/04/22 15:50:40  lewis
 *	SPR#1447: override LedItView::UpdateScrollBars () so we can disable it during metafile draws.
 *	Override ActiveLedItControl::OnDrawMetafile () since default one doesn't work right for ActiveLedIt.
 *	Now we draw (metafile) properly when embedded in MSWord XP
 *	
 *	Revision 2.60  2003/04/04 19:35:19  lewis
 *	SPR#1410: ActiveLedIt now properly saves BINARY version# in DemoMode, and now LOOKS AT binary version#
 *	(so < compare works right). SPR#1407: convert ActiveLedIt to using new command-number vectoring
 *	etc scheme (not template based)
 *	
 *	Revision 2.59  2003/03/11 02:33:10  lewis
 *	SPR#1288 - use new MakeSophistsAppNameVersionURL and add new CheckForUpdatesWebPageCommand,
 *	and other cleanups to Led_URLManager calls and context menu
 *	
 *	Revision 2.58  2003/01/23 20:50:03  lewis
 *	SPR#1260 - added CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK and CHECK_DEMO_AND_ALERT_AND_RETURNX
 *	and used in a few more places (e.g find dialog)
 *	
 *	Revision 2.57  2003/01/23 20:29:14  lewis
 *	SPR#1256 - use Led_StdDialogHelper_ParagraphSpacingDialog etc instead of local copy.
 *	SPR#1186 - added support for Led_StdDialogHelper_ParagraphIndentsDialog
 *	
 *	Revision 2.56  2003/01/17 18:09:05  lewis
 *	SPR#1241 - be much more aggressive when demo has expired
 *	
 *	Revision 2.55  2002/11/21 16:13:09  lewis
 *	SPR#1182 - a few changes to qDemoMode code. Write bin version# too. Add code to only give
 *	one day additional when changing version numbers. Dialogs come up more often. Small related
 *	logic fixes and disable ESC key from dismissing nag dialog
 *	
 *	Revision 2.54  2002/11/21 14:35:56  lewis
 *	SPR#1177- add insert table row/col menu items
 *	
 *	Revision 2.53  2002/11/14 17:05:23  lewis
 *	SPR#1171- use SetSelection() override instead of obsolete NotificationOf_SelectionChanged routine, and
 *	call fEditor.SetEmptySelectionStyle() in a few places where we may have made changes to the text that
 *	warrent grabbing that selection info.
 *	
 *	Revision 2.52  2002/11/12 22:31:15  lewis
 *	SPR#1170- RButtonDown must fire mousedown events
 *	
 *	Revision 2.51  2002/10/25 17:27:07  lewis
 *	SPR#1146 - Added 'Insert Symbol' menu item
 *	
 *	Revision 2.50  2002/10/23 01:42:41  lewis
 *	SPR#1137- add Insert menu - including insert table and insert URL. Both seem to work as well as with LedIt WIN
 *	
 *	Revision 2.49  2002/05/06 21:34:20  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.48  2001/11/27 00:32:29  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.47  2001/08/30 00:35:56  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.46  2001/08/27 13:43:42  lewis
 *	SPR#0972- add ListStyle popup menu, and fix LedItView::GetLayoutMargins () so bullets display properly
 *	
 *	Revision 2.45  2001/08/23 20:47:02  lewis
 *	SPR#0972- new EnableAutoChangesBackgroundColor feature
 *	
 *	Revision 2.44  2001/08/20 22:23:01  lewis
 *	SPR#0818- new display of URL objects (now just blue underlined slightly enlarged text). Can use
 *	qStandardURLStyleMarkerNewDisplayMode to get old code
 *	
 *	Revision 2.43  2001/08/17 16:30:31  lewis
 *	SPR#0959- GetInfo (DIB/URL/etc) command support
 *	
 *	Revision 2.42  2001/07/31 15:38:44  lewis
 *	qTypedefConfusedAccessCheckingCompilerBug AND silence compiler warnings with static_cast<>
 *	
 *	Revision 2.41  2001/05/29 23:12:45  lewis
 *	SPR#0943- new font color list (from ledit)
 *	
 *	Revision 2.40  2001/04/17 16:50:28  lewis
 *	SPR#0610- Added PrinterSetupDialog, PrintDialog and PrintOnce OLE AUT methods. Also made plain
 *	Added PrinterSetupDialog, PrintDialog and PrintOnce OLE AUT methods. Also made plain
 *	
 *	Revision 2.39  2001/04/12 19:47:03  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.38  2001/04/12 18:58:11  lewis
 *	cleanup DemoMode warning expired code (so easier to include in other apps)
 *	
 *	Revision 2.37  2001/04/09 20:43:33  lewis
 *	SPR#0859- Added LaunchFontSettingsDialog/LaunchParagraphSettingsDialog methods.
 *	
 *	Revision 2.36  2000/10/18 20:59:52  lewis
 *	cleanups. Lose FindDialog code - and inherit new sahred implm in Led_StdDialogHelper_FindDialog etc
 *	
 *	Revision 2.35  2000/10/05 02:38:22  lewis
 *	moved MyHidableTextMarkerOwner to shared class ColoredUniformHidableTextMarkerOwner in
 *	Led. Wrap some more OLE calls in try/catch to make them more bullet-proof.
 *	
 *	Revision 2.34  2000/10/04 15:49:21  lewis
 *	cleanups and remove unneeded CMDINFO overrides - due to moving stuff to common code (SPR#0839).
 *	
 *	Revision 2.33  2000/10/04 13:21:00  lewis
 *	lose some code (undo handlers etc) obsoleted by recent code sharing changes
 *	
 *	Revision 2.32  2000/10/03 13:41:20  lewis
 *	SPR#0840- new WaterMarkHelper<> template for watermark/demomode. SPR#0839 - lose Cut/Copy/Paste overrides -
 *	now handling automatiucaly
 *	
 *	Revision 2.31  2000/09/30 19:36:29  lewis
 *	lose LedItView::OnSelectAllCommand/nUpdateCutCopyClearComma/OnUpdatePasteCommand etc since now handled
 *	by TextInteractorCommonCommandHelper_MFC
 *	
 *	Revision 2.30  2000/08/28 21:51:11  lewis
 *	SPR#0830- added TextColored IDL proprty - so you can use the old or new display algorith for hidden text.
 *	Defaults to MSWord style display.
 *	
 *	Revision 2.29  2000/08/28 20:36:31  lewis
 *	use new LightUnderlineHidableTextMarker() instead of FontSpecHidableTextMarker- and still mor estuiff
 *	related to this todo.
 *	
 *	Revision 2.28  2000/06/14 13:52:53  lewis
 *	cleanups(moved FindDialog etc into scope of function - by getting rid of unused message
 *	map declaration). And SPR#0785 - Added 'Paragraph Spacing cmd/dialog support
 *	
 *	Revision 2.27  2000/04/24 17:25:32  lewis
 *	SPR#0743 use new Led_MFC_WordProcessor.h module
 *	
 *	Revision 2.26  2000/04/14 19:18:08  lewis
 *	lose unneeded override of InternalizeBestFlavor
 *	
 *	Revision 2.25  2000/04/03 15:39:56  lewis
 *	SPR#0728- Incorporate HiddenText OLEAUT support into the OCX
 *	
 *	Revision 2.24  1999/12/28 13:09:14  lewis
 *	set kShowHideTabGlyphs_CmdID / kShowHideSpaceGlyphs_CmdID to ZERO - since WP
 *	template now supports that
 *	
 *	Revision 2.23  1999/12/27 20:16:26  lewis
 *	SPR#0669- partial - now support these 3 properties for show/hide space/paragraphs etc
 *	
 *	Revision 2.22  1999/12/27 17:33:09  lewis
 *	SPR#0669 - work on this, but not done. Just added most of the support so this command
 *	is autohandled by WordProcessorCommonCommandHelper<> template. And adjusted the characters
 *	output (at least for UNICODE).
 *	
 *	Revision 2.21  1999/12/24 00:55:01  lewis
 *	make the AsTextBitmap() method work better. autosize H-Size of it better. Use auto_ptr<>
 *	to make delete of bitmap safer (if excpetions). Dont call WindowDrawHelper - but directly
 *	call fEditor.Draw() WindowDrawHelper() re-invokes OnSize() code (cuz of scrollbar changes).
 *	
 *	Revision 2.20  1999/12/21 20:51:09  lewis
 *	SPR#0672- AsTextBitmap() support. Plus a few small tweeks.
 *	
 *	Revision 2.19  1999/12/12 16:42:19  lewis
 *	SPR#0651
 *	
 *	Revision 2.18  1999/12/08 18:08:00  lewis
 *	react to changes in WordProcessorSelectionCommandHelper_MFCX<> template helpers
 *	
 *	Revision 2.17  1999/12/07 21:00:22  lewis
 *	lose code for caching/managing selectedFontSpec - and instead inherit it from
 *	WordProcessorSelectionCommandHelper (SPR#0606)
 *	
 *	Revision 2.16  1999/09/20 13:17:00  lewis
 *	some hacks todo interum 2.3.2 release - but mostly support for new DEMO_MODE build
 *	
 *	Revision 2.15  1999/07/16 21:24:39  lewis
 *	spr#0601- change the SetSBar/GetSBarType IDL methods to operate on TRISTATE - like Led_MFC,
 *	so default can be autoshow sbar. THEN - fix bug where hsbar resizing wasn't working properly -
 *	code from LedItMFC was needed - now moved to common shared template WordProcessorHScrollbarHelper<>
 *	
 *	Revision 2.14  1999/06/28 14:09:04  lewis
 *	spr#0528- Add hooks to various other windows messages, and add wrappers on FireEvent() to fire
 *	the corresponding other events. Borrowed from the pattern in COleControl::ButtonDblClk, etc,
 *	for when to fire which events. Tested under activex control test container, and all the events seemed
 *	to fire properly (with right args).
 *	
 *	Revision 2.13  1999/06/26 20:46:48  lewis
 *	work on spr#0528: Catch when focus is set to owning COleControl and pass it along to LedItView.
 *	And handle SelChange and Change notifications (events). Still several more events I must handle,
 *	since I cannot use the COleControl automatic handling - due to the compiler bug which prevents me
 *	merging tehse into a single class.
 *	
 *	Revision 2.12  1999/06/26 17:04:15  lewis
 *	override OnMouseActiveate to force the OLEControl to go into UIActivate state when user clicks
 *	on us (maybe SB on GAIN FOCUS?)- part of spr# 0596
 *	
 *	Revision 2.11  1999/06/25 14:08:52  lewis
 *	make IDL name MaxInputLength -> MaxLength to match MS RichTextControl 6.0
 *	
 *	Revision 2.10  1999/06/25 00:50:36  lewis
 *	spr#0595- add 'MaximumInputLength property to control. Most of this was trivial. We do a
 *	throw from AboutToUpdateText() method if too much text entered. Easy. But the trick is
 *	that for an OCX - there is no top level catcher to handle these excptions. They go unhandled.
 *	To address this, the Led_MFC_ExceptionHandlerHelper<> template as added
 *	
 *	Revision 2.9  1999/06/23 21:57:40  lewis
 *	add a HorzSBar and WrapToWindow toggle button
 *	
 *	Revision 2.8  1999/03/01 15:28:47  lewis
 *	use new SetVHScrollbarType API instead of overriding PreCreateWindow to set WS_VSCROLL
 *	
 *	Revision 2.7  1998/11/02 13:23:17  lewis
 *	Changed from usin vector<char*> to vector<string> - and cleaned up font name list gen code slightly.
 *	And lost LedItView::SetWindowRect() overrides - SB very soon obsolete due to new margin/hscroll
 *	support in wordprocessor class.
 *	
 *	Revision 2.6  1998/03/04 20:43:51  lewis
 *	Major cleanups - Major addition to OLEAUT support.
 *	
 *	Revision 2.5  1997/12/24  04:43:52  lewis
 *	*** empty log message ***
 *
 *	Revision 2.4  1997/12/24  04:29:41  lewis
 *	Lose OnCreate - now handled in Led_MFC (droptarget setting).
 *
 *	Revision 2.3  1997/09/29  18:10:08  lewis
 *	Inherit from WordProcessor instead of StandardStyledWordWrappedTextInteractor.
 *	Add Justifcation menu support.
 *	Revised which SetWindowRect () override we do, for new WindowRect API.
 *
 *	Revision 2.2  1997/07/27  15:59:52  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.1  1997/07/23  23:23:06  lewis
 *	OnBrowseHelpCommand
 *
 *	Revision 2.0  1997/06/28  17:43:14  lewis
 *	*** empty log message ***
 *
 *
 *	<========== BasedOnLedLineIt! 2.2b1 ==========>
 *
 *
 *
 *
 */

#include	<afxctl.h>

#include	<atlbase.h>

#include	"ChunkedArrayTextStore.h"
#include	"LedOptionsSupport.h"
#include	"Led_MFC_WordProcessor.h"
#include	"WordProcessor.h"

#include	"ActiveLedItConfig.h"
#include	"Resource.h"



class	LedItViewController {
	public:
		LedItViewController ();
		~LedItViewController ();

	public:
		virtual	void	OnBrowseHelpCommand ()								=	0;
		virtual	void	OnAboutBoxCommand ()								=	0;
		virtual	void	ForceUIActive ()									=	0;
		virtual	void	FireOLEEvent (DISPID eventID)						=	0;
		virtual	void	FireOLEEvent (DISPID dispid, BYTE* pbParams, ...)	=	0;
#if		qFunnyDisplayInDesignMode
		virtual	bool	IsInDesignMode () const								=	0;
		virtual	bool	DrawExtraDesignModeBorder () const					=	0;
#endif
		virtual	HMENU	GenerateContextMenu ()								=	0;

	public:
		nonvirtual	void	FireKeyDown (USHORT* pnChar, short nShiftState);
		nonvirtual	void	FireKeyUp (USHORT* pnChar, short nShiftState);
		nonvirtual	void	FireKeyPress (USHORT* pnChar);
		nonvirtual	void	FireMouseDown (short nButton, short nShiftState, OLE_XPOS_PIXELS x, OLE_YPOS_PIXELS y);
		nonvirtual	void	FireMouseUp (short nButton, short nShiftState, OLE_XPOS_PIXELS x, OLE_YPOS_PIXELS y);
		nonvirtual	void	FireMouseMove (short nButton, short nShiftState, OLE_XPOS_PIXELS x, OLE_YPOS_PIXELS y);
		nonvirtual	void	FireClick ();
		nonvirtual	void	FireDblClick ();
		virtual		void	FireUpdateUserCommand (const wstring& internalCmdName, VARIANT_BOOL* enabled, VARIANT_BOOL* checked, wstring* name)	=	0;
		virtual		void	FireUserCommand (const wstring& internalCmdName)																	=	0;

	public:
		ChunkedArrayTextStore							fTextStore;
		MultiLevelUndoCommandHandler					fCommandHandler;
		WordProcessor::HidableTextDatabasePtr			fHidableTextDatabase;
};






#if		qTypedefConfusedAccessCheckingCompilerBug
	class	LedItViewAlmostBASE : public Led_MFC_ExceptionHandlerHelper< Led_MFC_X <WordProcessor> > {
	};
#else
	typedef	Led_MFC_ExceptionHandlerHelper< Led_MFC_X <WordProcessor> >	LedItViewAlmostBASE;
#endif






#if		qQuiteAnnoyingDominanceWarnings
	#pragma warning (disable : 4250)
#endif


class	LedItView : public WordProcessorCommonCommandHelper_MFC <LedItViewAlmostBASE> {
	private:
		typedef	WordProcessorCommonCommandHelper_MFC <LedItViewAlmostBASE>	inherited;

	public:
		LedItView ();

	public:
		nonvirtual	void	SetController (LedItViewController* controller);
	private:
		LedItViewController*	fController;

	public:
		virtual ~LedItView ();

	public:
		nonvirtual	bool	GetSupportContextMenu () const;
		nonvirtual	void	SetSupportContextMenu (bool allowContextMenu);
	private:
		bool	fSupportContextMenu;

	public:
		nonvirtual	bool	GetHideDisabledContextMenuItems () const;
		nonvirtual	void	SetHideDisabledContextMenuItems (bool hideDisabledContextMenuItems);
	private:
		bool	fHideDisabledContextMenuItems;

	public:
		nonvirtual	bool	GetWrapToWindow () const;
		nonvirtual	void	SetWrapToWindow (bool wrapToWindow);
	private:
		bool	fWrapToWindow;


	public:
		nonvirtual	long	GetMaxLength () const;
		nonvirtual	void	SetMaxLength (long maxLength);
	private:
		long	fMaxLength;

	public:
		override	void	GetLayoutMargins (RowReference row, Led_Coordinate* lhs, Led_Coordinate* rhs) const;
		override	void	SetWindowRect (const Led_Rect& windowRect);
	public:
		override	Led_Distance	CalculateFarthestRightMarginInWindow () const;

	protected:
		afx_msg	int OnMouseActivate (CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	public:
		override	void	PostNcDestroy ();
		afx_msg		void	OnInitMenuPopup (CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
		afx_msg		void	OnContextMenu (CWnd* /*pWnd*/, CPoint /*point*/);

	// Reflect events (call FireEvent) for these events. Must be done here cuz COleControl code for same doesn't
	// work cuz our focus (LedItView) isn't the same as the COleControl).
	public:
		afx_msg		void	OnSysKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg		void	OnSysKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg		void	OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg		void	OnKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags);
		override	void	OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed, bool controlPressed, bool altKeyPressed);
		afx_msg		void	OnLButtonDown (UINT nFlags, CPoint oPoint);
		afx_msg		void	OnLButtonUp (UINT nFlags, CPoint oPoint);
		afx_msg		void	OnLButtonDblClk (UINT nFlags, CPoint oPoint);
		afx_msg		void	OnRButtonDown (UINT nFlags, CPoint oPoint);
		afx_msg		void	OnRButtonUp (UINT nFlags, CPoint oPoint);
		afx_msg		void	OnMouseMove (UINT nFlags, CPoint oPoint);

	public:
		afx_msg		void	OnPasteAsTextCommand ();
		afx_msg		void	OnUpdatePasteAsTextCommand (CCmdUI* pCmdUI);
	public:
		afx_msg		void	OnOLEUserCommand (UINT nID);
		afx_msg		void	OnUpdateOLEUserCommand (CCmdUI* pCmdUI);

	protected:
		override	SearchParameters	GetSearchParameters () const;
		override	void				SetSearchParameters (const SearchParameters& sp);

	public:
		override	void	SetSelection (size_t start, size_t end);
		using	TextInteractor::SetSelection;

	protected:
		override	void	AboutToUpdateText (const UpdateInfo& updateInfo);

	public:
		override	void	EraseBackground (Led_Tablet tablet, const Led_Rect& subsetToDraw, bool printing);

	public:
		bool	fEnableAutoChangesBackgroundColor;

	public:
		nonvirtual	long	OLE_FindReplace (long searchFrom, const Led_tString& findText, const Led_tString& replaceText, BOOL wrapSearch, BOOL wholeWordSearch, BOOL caseSensativeSearch);

	public:
		bool	fInDrawMetaFileMode;
	public:
		nonvirtual	void	UpdateScrollBars ();

	protected:
		afx_msg	void	OnBrowseHelpCommand ();
		afx_msg	void	OnCheckForUpdatesWebPageCommand ();
		afx_msg	void	OnAboutBoxCommand ();
	public:
		afx_msg	void	OnFilePrintOnce ();
		afx_msg	void	OnFilePrint ();
		afx_msg	void	OnFilePrintSetup ();
	private:
		nonvirtual	void	DoPrintHelper (bool showPrintDlg);

		DECLARE_MESSAGE_MAP ()


	#ifdef 	_DEBUG
	public:
		override	void	AssertValid () const;
		override	void	Dump (CDumpContext& dc) const;
	#endif
};
#if		qQuiteAnnoyingDominanceWarnings
	#pragma warning (default : 4250)
#endif






#if		qDemoMode
/*
 *	As of Version 3.1a2 - also write out "DemoVersionNumber". So we can better compare version#s in the future (by string
 *	isn't reliable). Also - in 3.1a2 - we introduced kDaysToExpireForNewVersion check.
 */
class	DemoPrefs : public OptionsFileHelper {
	public:
		class	RegistryEntryNotFound {};
		enum	{ kDaysToExpire = 7 };
		enum	{ kDaysToExpireForNewVersion = 1 };	// if they had an old version and IT expired, and tried a new version - give shorter deadline

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
				commonPrefsKey.Create (sophistsKey, _T ("ActiveLedIt!"));
				if (commonPrefsKey == NULL) {
					throw RegistryEntryNotFound ();
				}
				return commonPrefsKey.Detach ();
			}
		static	inline	unsigned	GetDemoDaysLeftHelper (unsigned demoExpiresAt)
			{
				time_t		curTime		=	time (NULL);
				if (static_cast<time_t> (demoExpiresAt) < curTime) {
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
				int		demoVersion		=	0;
				int		demoExpiresAt	=	0;
				if (LookupPref ("DemoExpiresAt", &demoExpiresAt) and LookupPref ("DemoVersionNumber", &demoVersion)) {
					UINT	daysLeft	=	GetDemoDaysLeftHelper (demoExpiresAt);
					if (daysLeft > 0) {
						return daysLeft;
					}

					/*
					 *	If they install a NEWER version - then given them a little extra time.
					 */
					if (static_cast<unsigned int> (demoVersion) < kActiveLedItDWORDVersion) {
						demoExpiresAt = time (NULL) + kDaysToExpireForNewVersion*24*60*60;
						StorePref ("DemoExpiresAt", demoExpiresAt);
						StorePref ("DemoVersionString", qLed_ShortVersionString);
						StorePref ("DemoVersionNumber", static_cast<int> (kActiveLedItDWORDVersion));
						return kDaysToExpireForNewVersion;
					}
					return daysLeft;
				}
				else {
					/*
					 *	New installation.
					 */
					demoExpiresAt = time (NULL) + kDaysToExpire*24*60*60;
					StorePref ("DemoExpiresAt", demoExpiresAt);
					StorePref ("DemoVersionString", qLed_ShortVersionString);
					StorePref ("DemoVersionNumber", static_cast<int> (kActiveLedItDWORDVersion));
					return kDaysToExpire;
				}
			}
};
#endif


#if		qDemoMode
class	DemoModeAlerter {
	public:
		static	void	ShowAlert (HWND parentWnd = 0);
		static	void	ShowAlertIfItsBeenAWhile (HWND parentWnd = 0);
		static	int		sNextWarningAfter;
		static	const	float	kTimeBetweenWarnings;	// n SECONDs minimum delay between warnings 
};
#endif



#if		qDemoMode
	#define	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(w)\
		if (DemoPrefs ().GetDemoDaysLeft () <= 0) {\
			DemoModeAlerter::ShowAlert (w);\
			return;\
		}
	#define	CHECK_DEMO_AND_ALERT_AND_RETURNX_NO_TIME_CHECK(w,x)\
		if (DemoPrefs ().GetDemoDaysLeft () <= 0) {\
			DemoModeAlerter::ShowAlert (w);\
			return (x);\
		}
	#define	CHECK_DEMO_AND_ALERT_AND_RETURNX(w,x)\
		if (DemoPrefs ().GetDemoDaysLeft () <= 0) {\
			DemoModeAlerter::ShowAlertIfItsBeenAWhile (w);\
			return (x);\
		}
	#define	CHECK_DEMO_AND_ALERT_AND_RETURN(w)\
		if (DemoPrefs ().GetDemoDaysLeft () <= 0) {\
			DemoModeAlerter::ShowAlertIfItsBeenAWhile (w);\
			return;\
		}
	#define	CHECK_DEMO_AND_BEEP_AND_RETURN()\
		if (DemoPrefs ().GetDemoDaysLeft () <= 0) {\
			Led_BeepNotify ();\
			return;\
		}
#else
	#define	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(w)
	#define	CHECK_DEMO_AND_ALERT_AND_RETURNX_NO_TIME_CHECK(w,x)
	#define	CHECK_DEMO_AND_ALERT_AND_RETURN(w)
	#define	CHECK_DEMO_AND_ALERT_AND_RETURNX(w,x)
	#define	CHECK_DEMO_AND_BEEP_AND_RETURN()
#endif





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


//	class	LedItViewController
	inline void LedItViewController::FireKeyDown (USHORT* pnChar, short nShiftState)
		{
			FireOLEEvent (DISPID_KEYDOWN, EVENT_PARAM(VTS_PI2 VTS_I2), pnChar, nShiftState);
		}
	inline void LedItViewController::FireKeyUp (USHORT* pnChar, short nShiftState)
		{
			FireOLEEvent (DISPID_KEYUP, EVENT_PARAM(VTS_PI2 VTS_I2), pnChar, nShiftState);
		}
	inline	void LedItViewController::FireKeyPress (USHORT* pnChar)
		{
			FireOLEEvent (DISPID_KEYPRESS, EVENT_PARAM(VTS_PI2), pnChar);
		}
	inline void LedItViewController::FireMouseDown (short nButton, short nShiftState, OLE_XPOS_PIXELS x, OLE_YPOS_PIXELS y)
		{
			FireOLEEvent (DISPID_MOUSEDOWN, EVENT_PARAM(VTS_I2 VTS_I2 VTS_XPOS_PIXELS VTS_YPOS_PIXELS), nButton, nShiftState, x, y);
		}
	inline void LedItViewController::FireMouseUp (short nButton, short nShiftState, OLE_XPOS_PIXELS x, OLE_YPOS_PIXELS y)
		{
			FireOLEEvent (DISPID_MOUSEUP, EVENT_PARAM(VTS_I2 VTS_I2 VTS_XPOS_PIXELS VTS_YPOS_PIXELS), nButton, nShiftState, x, y);
		}
	inline void LedItViewController::FireMouseMove (short nButton, short nShiftState, OLE_XPOS_PIXELS x, OLE_YPOS_PIXELS y)
		{
			FireOLEEvent (DISPID_MOUSEMOVE, EVENT_PARAM(VTS_I2 VTS_I2 VTS_XPOS_PIXELS VTS_YPOS_PIXELS), nButton, nShiftState, x, y);
		}
	inline void LedItViewController::FireClick ()
		{
			FireOLEEvent (DISPID_CLICK, EVENT_PARAM(VTS_NONE));
		}
	inline void LedItViewController::FireDblClick ()
		{
			FireOLEEvent (DISPID_DBLCLICK, EVENT_PARAM(VTS_NONE));
		}




//	class	LedItView
	inline	bool	LedItView::GetWrapToWindow () const
		{
			return fWrapToWindow;
		}
	inline	bool	LedItView::GetSupportContextMenu () const
		{
			return fSupportContextMenu;
		}
	inline	bool	LedItView::GetHideDisabledContextMenuItems () const
		{
			return fHideDisabledContextMenuItems;
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

