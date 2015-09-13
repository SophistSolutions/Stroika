/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__ActiveLedItControl_h__
#define	__ActiveLedItControl_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/ActiveLedIt/Headers/ActiveLedItControl.h,v 2.74 2004/02/09 16:25:08 lewis Exp $
 *
 * Description:
 *		Declaration of the ActiveLedItControl ActiveX Control class.
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: ActiveLedItControl.h,v $
 *	Revision 2.74  2004/02/09 16:25:08  lewis
 *	SPR#1599: Added qKeepListOfALInstancesForSPR_1599BWA  for MSIE quit without closing last control bug. For SPR#1641: added assert to check MODULE::m_nObjectCount = 0. Had  to be much more careful about using OLE_GetXXX functions that return IDispatch*. Instead - do non-OLE_ versions that return CComPtr<IDispatch> - so we don't get redundant refcounts.
 *	
 *	Revision 2.73  2004/02/07 01:32:33  lewis
 *	SPR#1629: update: Added font-color icon, and associated the icon with the color popup menu.
 *	Added support to ActiveLedIt_IconButtonToolbarElement to make that work. Including - changed
 *	ToggleButton property to a 'ButtonStyle' property - where one style was 'ICON/POPUP MENU'.
 *	
 *	Revision 2.72  2004/02/06 16:12:15  lewis
 *	SPR#1629: Update: Lose private 'width' property for combobox class, and instead - autocompute
 *	the prefered width based on the attached menu. Improved the automatic LAYOUT code so that changes
 *	to the menu should resize the combobox.
 *	
 *	Revision 2.71  2004/02/06 01:35:59  lewis
 *	SPR#1629: added fairly functional (not perfect) ActiveLedIt_ComboBoxToolbarElement - and used
 *	that to implement first cut at fontname and fontsize combo boxes.
 *	
 *	Revision 2.70  2004/02/03 22:29:27  lewis
 *	SPR#1618: add ToggleButton property to IconButtonToolbarItem, and CommandChecked () method to
 *	ActiveLedIt - to support checked buttons - like BOLD, ITALIC, and UNDERLINE
 *	
 *	Revision 2.69  2004/02/03 02:23:02  lewis
 *	SPR#1618: Added undo/redo buttons, and separator support
 *	
 *	Revision 2.68  2004/02/02 23:02:34  lewis
 *	SPR#1618: Started CommandEnabled support - and got associated command (to toolbar icon) code
 *	working - so for simple cases - at least - it invokes the command)
 *	
 *	Revision 2.67  2004/02/02 21:15:07  lewis
 *	SPR#1618: Added preliminary toolbar support. A small toolbar with a few icons appears - but no
 *	DO command or UPDATE_ENABLE_DISABLE CMD support yet (and just plain icon cmd items)
 *	
 *	Revision 2.66  2004/01/30 04:36:27  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.65  2004/01/25 16:19:20  lewis
 *	SPR#1617: lose old resource-based contextmenu/accelerators, and use new COM-based
 *	accelerator mechanism (part of new COM-based command support)
 *	
 *	Revision 2.64  2004/01/24 03:50:32  lewis
 *	InvokeCommand () functionality implemented
 *	
 *	Revision 2.63  2004/01/23 23:11:14  lewis
 *	SPR#1617: Added preliminary (largely functional) implementation of OLE automation-based
 *	context menus. Tons of new OLE APIs. Can create context menus from scratch, or built from
 *	building blocks of existing submenus (like font menu).
 *	
 *	Revision 2.62  2003/12/17 21:26:19  lewis
 *	SPR#1585: Finishe updating ActiveLedit for various ActiveSpelledIt / SpellCheck API changes
 *	(FindWordBreaks, PeekAtTextBReaksUsed, GetUDInterface, AddWOrdToUserDictionary, etc)
 *	
 *	Revision 2.61  2003/12/06 19:07:59  lewis
 *	SPR#1547: handle OLE_Get/SetEnabled calls - even when window not realized (as I had done
 *	for SetReadOnly)
 *	
 *	Revision 2.60  2003/07/02 21:22:25  lewis
 *	SPR#1546: Supported ReadOnly <PARAM> argument to <OBJECT>
 *	
 *	Revision 2.59  2003/06/12 17:15:43  lewis
 *	SPR#1526: Added COM-based spellchecker support to ActiveLedIt. Now you can access SpellCheck menu
 *	command if there is a COM-spellchecker associated with ActiveLedIt. New property 'SpellChecker'
 *	you can get/set to specify the spell checker. Seems to work as well as with LedIt!
 *	
 *	Revision 2.58  2003/06/03 16:35:20  lewis
 *	SPR#1522: have control goto Active state - rather than UIActive - on OLEVERB_SHOW
 *	
 *	Revision 2.57  2003/06/03 14:42:12  lewis
 *	SPR#1521: added 'SelListStyle' support. Somewhat related - also changed build of ActiveLedIt
 *	to include the _h.h file generated from the IDL for access to enums rather than redefining
 *	them in the ActiveLedItControl sources.
 *	
 *	Revision 2.56  2003/05/30 14:26:14  lewis
 *	SPR#1517: Added find/replace dialog and OLE_FindReplace and OLE_LaunchReplaceDialog
 *	new OLE AUTOMATION methods
 *	
 *	Revision 2.55  2003/05/29 14:56:35  lewis
 *	SPR#1510: dont call GetClientRect in draw - but use WindowRect/GetDefaultWindowMargins
 *	since we may have been called from metafile draw or other. Also - added DrawExtraDesignModeBorder ()
 *	so we don't display border in some cases where it looks ugly (not really related to SPR#1510 -
 *	but I noticed it while debugging and looking at display in MSVC.Net 2003 dialog editor). Careful
 *	about call to GetStyle (since vectors to MFC version in LedItView) - and that fails if window is
 *	NULL (as it is sometimes in MSVC .Net 2003 dialog editor)
 *	
 *	Revision 2.54  2003/05/28 16:56:18  lewis
 *	SPR#1509: Added qFunnyDisplayInDesignMode define to ActiveLedItConfig, and ifdefined (yes by
 *	default), then check for AMBIENT_DESIGN_MODE and change the drawing in LedItView::EraseBackground ()
 *	slightly. Added a small watermarkdisplay saying 'design mode' and a border in the color opposite to
 *	the background. Hooked OnAmbientPropertyChange() to detect change and refresh screen.
 *	
 *	Revision 2.53  2003/05/06 19:43:40  lewis
 *	SPR#1168: work on this SPR. Not complete. Cleaned much up, and narrowed in on problem. Still
 *	not there yet - though.
 *	
 *	Revision 2.52  2003/04/29 21:59:47  lewis
 *	SPR#1168: Added ActiveLedItControl::OnGetNaturalExtent and OnEraseBkgnd to help reduce flicker
 *	and drawing bugs with resizing AL controls in various containers (like MSVS.NET 2002 and ActiveX
 *	Control Container and MS Word XP, and LedIt 3.1a6x)
 *	
 *	Revision 2.51  2003/04/29 13:47:09  lewis
 *	SPR#1168: cleanups to resize/OnDraw code to debug problem with AL resizing occasionally leaving
 *	cruft. Rework AL::OnDraw() - rarely called. Don't need to reset windowRect in that routine anymore.
 *	
 *	Revision 2.50  2003/04/22 15:50:39  lewis
 *	SPR#1447: override LedItView::UpdateScrollBars () so we can disable it during metafile
 *	draws. Override ActiveLedItControl::OnDrawMetafile () since default one doesn't work right
 *	for ActiveLedIt. Now we draw (metafile) properly when embedded in MSWord XP
 *	
 *	Revision 2.49  2003/03/11 02:33:10  lewis
 *	SPR#1288 - use new MakeSophistsAppNameVersionURL and add new CheckForUpdatesWebPageCommand,
 *	and other cleanups to Led_URLManager calls and context menu
 *	
 *	Revision 2.48  2003/03/05 22:39:29  lewis
 *	SPR#1320 - Added stock Appearance and BorderStyle property support. Also, changed my
 *	existing HWND property to using the stock HWND property code.
 *	
 *	Revision 2.47  2003/02/12 15:59:48  lewis
 *	SPR#1298- added ShowSecondaryHilight property to the control, and default to TRUE
 *	
 *	Revision 2.46  2003/01/15 18:57:28  lewis
 *	SPR#1221- added SmartQuoteMode OLE automation
 *	
 *	Revision 2.45  2003/01/09 14:09:21  lewis
 *	SPR#1227- added hWnd read-only property to ActiveLedIt control
 *	
 *	Revision 2.44  2002/12/16 19:23:48  lewis
 *	SPR#1208- added UseSelectEOLBOLRowHilightStyle property
 *	
 *	Revision 2.43  2002/10/30 16:55:35  lewis
 *	SPR#1153- added PrintMargins OLE AUT interface to ActiveLedIt
 *	
 *	Revision 2.42  2002/10/28 21:47:08  lewis
 *	SPR#0678 - Added OLE AUT methods for Get/Set DefaultWindowMargins
 *	
 *	Revision 2.41  2002/05/06 21:34:19  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.40  2001/11/27 00:32:28  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.39  2001/08/30 00:35:56  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.38  2001/08/23 20:47:01  lewis
 *	SPR#0972- new EnableAutoChangesBackgroundColor feature
 *	
 *	Revision 2.37  2001/08/15 21:09:43  lewis
 *	SPR#0965- support OLE_COLOR for HidableTextColor and SelColor attributes
 *	
 *	Revision 2.36  2001/08/01 23:51:42  lewis
 *	use a few more STOCK_PROPRETY DISPIDs. And SPR# 0964 - ReadOnly/Enabled property supported
 *	
 *	Revision 2.35  2001/08/01 22:00:20  lewis
 *	SPR#0963- added support for BackColor stock property
 *	
 *	Revision 2.34  2001/05/30 16:23:03  lewis
 *	SPR#0945- new VersionNumber/ShortVersionString OLEAUT APIs
 *	
 *	Revision 2.33  2001/04/17 16:50:28  lewis
 *	SPR#0610- Added PrinterSetupDialog, PrintDialog and PrintOnce OLE AUT methods.
 *	Also made plain Added PrinterSetupDialog, PrintDialog and PrintOnce OLE AUT methods.
 *	Also made plain
 *	
 *	Revision 2.32  2001/04/09 20:43:33  lewis
 *	SPR#0859- Added LaunchFontSettingsDialog/LaunchParagraphSettingsDialog methods.
 *	
 *	Revision 2.31  2001/02/12 15:02:32  lewis
 *	SPR#0854- added OLE Automation GetHeight method - to compute pixelheight of a given range of text
 *	
 *	Revision 2.30  2000/10/06 03:37:54  lewis
 *	react to code cleanups of showhide scrollbar code in TextInteractor/Led_Win32
 *	
 *	Revision 2.29  2000/08/28 21:51:11  lewis
 *	SPR#0830- added TextColored IDL proprty - so you can use the old or new display algorith
 *	for hidden text. Defaults to MSWord style display.
 *	
 *	Revision 2.28  2000/04/10 16:26:52  lewis
 *	SPR#0735- added SelHidable OLE AUT property
 *	
 *	Revision 2.27  2000/04/03 15:39:56  lewis
 *	SPR#0728- Incorporate HiddenText OLEAUT support into the OCX
 *	
 *	Revision 2.26  2000/04/03 03:44:22  lewis
 *	SPR#0716- added FindUI and Find(noUI) OLEAutomation methods
 *	
 *	Revision 2.25  2000/03/15 21:27:52  lewis
 *	SPR#0715- added a read/write SelJustification property
 *	
 *	Revision 2.24  2000/03/15 19:20:25  lewis
 *	SPR#0714: support IDL dirty flag. Also - change default size of new ActiveLedIt! control
 *	
 *	Revision 2.23  2000/03/15 16:51:27  lewis
 *	Support a bevy of new UNDO released OLE_AUT commands to set maxUndo level etc. SPR#0712 - for RandomHouse
 *	
 *	Revision 2.22  1999/12/27 20:16:26  lewis
 *	SPR#0669- partial - now support these 3 properties for show/hide space/paragraphs etc
 *	
 *	Revision 2.21  1999/12/24 00:55:01  lewis
 *	make the AsTextBitmap() method work better. autosize H-Size of it better. Use auto_ptr<>
 *	to make delete of bitmap safer (if excpetions). Dont call WindowDrawHelper - but directly
 *	call fEditor.Draw() WindowDrawHelper() re-invokes OnSize() code (cuz of scrollbar changes).
 *	
 *	Revision 2.20  1999/12/21 20:51:08  lewis
 *	SPR#0672- AsTextBitmap() support. Plus a few small tweeks.
 *	
 *	Revision 2.19  1999/12/18 04:13:06  lewis
 *	mostly just react to spr#0662- break StyledTextIO into parts
 *	
 *	Revision 2.18  1999/12/09 03:16:52  lewis
 *	spr#0645 - -D_UNICODE support
 *	
 *	Revision 2.17  1999/11/13 16:53:54  lewis
 *	Fixed spr#0623 - added ScrollToSelection IDL method
 *	
 *	Revision 2.16  1999/07/16 21:24:39  lewis
 *	spr#0601- change the SetSBar/GetSBarType IDL methods to operate on TRISTATE - like Led_MFC,
 *	so default can be autoshow sbar. THEN - fix bug where hsbar resizing wasn't working properly -
 *	code from LedItMFC was needed - now moved to common shared template WordProcessorHScrollbarHelper<>
 *	
 *	Revision 2.15  1999/06/28 14:09:04  lewis
 *	spr#0528- Add hooks to various other windows messages, and add wrappers on FireEvent() to
 *	fire the corresponding other events. Borrowed from the pattern in COleControl::ButtonDblClk,
 *	etc, for when to fire which events. Tested under activex control test container, and all the
 *	events seemed to fire properly (with right args).
 *	
 *	Revision 2.14  1999/06/26 20:46:47  lewis
 *	work on spr#0528: Catch when focus is set to owning COleControl and pass it along to LedItView.
 *	And handle SelChange and Change notifications (events). Still several more events I must handle,
 *	since I cannot use the COleControl automatic handling - due to the compiler bug which prevents me
 *	merging tehse into a single class.
 *	
 *	Revision 2.13  1999/06/26 17:09:46  lewis
 *	Proivate LedItViewController::ForceUIActivateHook() so its override OnMouseActiveate can force the
 *	OLEControl to go into UIActivate state when user clicks on us- part of spr# 0596
 *	
 *	Revision 2.12  1999/06/25 14:08:52  lewis
 *	make IDL name MaxInputLength -> MaxLength to match MS RichTextControl 6.0
 *	
 *	Revision 2.11  1999/06/25 00:50:36  lewis
 *	spr#0595- add 'MaximumInputLength property to control. Most of this was trivial. We do a
 *	throw from AboutToUpdateText() method if too much text entered. Easy. But the trick is that
 *	for an OCX - there is no top level catcher to handle these excptions. They go unhandled.
 *	To address this, the Led_MFC_ExceptionHandlerHelper<> template as added
 *	
 *	Revision 2.10  1999/06/23 21:57:40  lewis
 *	add a HorzSBar and WrapToWindow toggle button
 *	
 *	Revision 2.9  1998/03/04 20:43:51  lewis
 *	Major cleanups - Major addition to OLEAUT support.
 *	
 *	Revision 2.8  1997/12/24  04:43:52  lewis
 *	*** empty log message ***
 *
 *	Revision 2.7  1997/12/24  04:29:15  lewis
 *	Added support for AsText/AsTextRTF/AsTextHTML properties
 *
 *	Revision 2.6  1997/07/27  15:59:52  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.5  1997/07/23  23:23:06  lewis
 *	OnBrowseHelpCommand
 *
 *	Revision 2.4  1997/07/14  01:21:27  lewis
 *	*** empty log message ***
 *
 *	Revision 2.3  1997/06/28  17:43:37  lewis
 *	broke LedIt class out into spearate file, and now hook in differently
 *
 *	Revision 2.2  1997/06/24  03:50:03  lewis
 *	override LedItView GetLayoutWidth() for spr#0450 - new layoutwidth strategy.
 *
 *	Revision 2.1  1997/06/23  16:56:16  lewis
 *	*** empty log message ***
 *
 *	Revision 2.0  1997/06/18  03:25:57  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 *
 */
#include	<set>
#include	<afxctl.h>

#include	"SpellCheckEngine.h"
#include	"StyledTextIO_HTML.h"
#include	"StyledTextIO_RTF.h"

#include	"ActiveLedItConfig.h"
#include	"LedItView.h"

#include	"ActiveLedIt_h.h"



enum	Led_FileFormat {
	eTextFormat,
	eLedPrivateFormat, 
	eRTFFormat, 
	eHTMLFormat, 
	eUnknownFormat,
	eDefaultFormat		=	eRTFFormat,
};



class	COMBased_SpellCheckEngine : public SpellCheckEngine, 
									private SpellCheckEngine::UDInterface,
									private TextBreaks
					{
	private:
		typedef	SpellCheckEngine	inherited;
	public:
		COMBased_SpellCheckEngine (IDispatch* engine);

	private:
		CComPtr<IDispatch>	fEngine;

	public:
		override	bool	ScanForUndefinedWord (const Led_tChar* startBuf, const Led_tChar* endBuf, const Led_tChar** cursor,
												const Led_tChar** wordStartResult, const Led_tChar** wordEndResult
												);

	protected:
		override		bool	LookupWord_ (const Led_tString& checkWord, Led_tString* matchedWordResult);

	public:
		override	vector<Led_tString>	GenerateSuggestions (const Led_tString& misspelledWord);

	public:
		override	TextBreaks*		PeekAtTextBreaksUsed ();

	// From TextBreaks
	public:
		override	void	FindWordBreaks (const Led_tChar* startOfText, size_t lengthOfText, size_t textOffsetToStartLookingForWord,
											size_t* wordStartResult, size_t* wordEndResult, bool* wordReal
										) const;
		override	void	FindLineBreaks (const Led_tChar* startOfText, size_t lengthOfText, size_t textOffsetToStartLookingForWord,
											size_t* wordEndResult, bool* wordReal
										) const;

	public:
		#if		qMixinDisambiguatingNameInBothBug
			typedef	inherited::UDInterface	UDInterface;
		#endif
		override	UDInterface*	GetUDInterface ();

	// From SpellCheckEngine::UDInterface
	public:
		override	bool	AddWordToUserDictionarySupported () const;
		override	void	AddWordToUserDictionary (const Led_tString& word);
};




#if		qQuiteAnnoyingDominanceWarnings
	#pragma	warning (disable : 4250)
#endif

class	ActiveLedItControl : public COleControl, public MarkerOwner, public LedItViewController {
	private:
		DECLARE_DYNCREATE(ActiveLedItControl)

	public:
		ActiveLedItControl ();

	protected:
		~ActiveLedItControl();


	#if		qKeepListOfALInstancesForSPR_1599BWA
		public:
			static	const set<ActiveLedItControl*>&	GetAll ();
		private:
			static	set<ActiveLedItControl*>	sAll;
	#endif

	protected:
		override	void		DidUpdateText (const UpdateInfo& /*updateInfo*/) throw ();
		override	TextStore*	PeekAtTextStore () const;

	public:
		override	void	OnDraw (CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
		override	void	OnDrawMetafile (CDC* pDC, const CRect& rcBounds);
		afx_msg		BOOL	OnEraseBkgnd (CDC* pDC);
		override	void	DoPropExchange (CPropExchange* pPX);
		override	void	OnResetState ();
		override	DWORD	GetControlFlags ();
		override	void	OnGetControlInfo (LPCONTROLINFO pControlInfo);
		override	BOOL	PreTranslateMessage (MSG* pMsg);
		override	void	AddFrameLevelUI ();
		override	void	RemoveFrameLevelUI ();
		override	BOOL	OnSetObjectRects(LPCRECT lprcPosRect, LPCRECT lprcClipRect);
		override	BOOL	OnGetNaturalExtent (DWORD /* dwAspect */, LONG /* lindex */,
												DVTARGETDEVICE* /* ptd */, HDC /* hicTargetDev */,
												DVEXTENTINFO* /* pExtentInfo */, LPSIZEL /* psizel */
											);


	private:
		struct	OnCreateExtras {
			OnCreateExtras ();
			bool	fReadOnly;
			bool	fEnabled;
		};
		auto_ptr<OnCreateExtras>	fOnCreateExtras;

	private:
		nonvirtual	void	ExchangeTextAsRTFBlob (CPropExchange* pPX);

	protected:
		DECLARE_OLECREATE_EX(ActiveLedItControl)    // Class factory and guid
		DECLARE_OLETYPELIB(ActiveLedItControl)      // GetTypeInfo
		DECLARE_PROPPAGEIDS(ActiveLedItControl)     // Property page IDs
		DECLARE_OLECTLTYPE(ActiveLedItControl)		// Type name and misc status

	// Reflect to OWNED window, since we have two separate windows. If we ever merge these, then most of this reflecting code can die.
	protected:
		afx_msg		int		OnCreate (LPCREATESTRUCT lpCreateStruct);
		afx_msg		void	OnWindowPosChanged (WINDOWPOS* lpwndpos);
		afx_msg		void	OnSetFocus (CWnd* pOldWnd);

	protected:
		nonvirtual	void	Layout ();

	protected:
		afx_msg void		OLE_InvalidateLayout ();

	#if		qDontUIActivateOnOpen
	protected:
		override	HRESULT	OnOpen (BOOL bTryInPlace, LPMSG pMsg);
	#endif

	protected:
		override	void	OnBackColorChanged ();
#if		qFunnyDisplayInDesignMode
		override	void	OnAmbientPropertyChange (DISPID dispid);
#endif

	protected:
		afx_msg	UINT	OLE_VersionNumber ();
		afx_msg	BSTR	OLE_ShortVersionString ();
		afx_msg	BOOL	OLE_GetReadOnly ();
		afx_msg void	OLE_SetReadOnly (BOOL bNewValue);
		afx_msg	BOOL	OLE_GetEnabled ();
		afx_msg void	OLE_SetEnabled (BOOL bNewValue);
		afx_msg	BOOL	OLE_GetEnableAutoChangesBackgroundColor ();
		afx_msg void	OLE_SetEnableAutoChangesBackgroundColor (BOOL bNewValue);
		afx_msg	int		OLE_GetWindowMarginTop ();
		afx_msg	void	OLE_SetWindowMarginTop (int windowMarginTop);
		afx_msg	int		OLE_GetWindowMarginLeft ();
		afx_msg	void	OLE_SetWindowMarginLeft (int windowMarginLeft);
		afx_msg	int		OLE_GetWindowMarginBottom ();
		afx_msg	void	OLE_SetWindowMarginBottom (int windowMarginBottom);
		afx_msg	int		OLE_GetWindowMarginRight ();
		afx_msg	void	OLE_SetWindowMarginRight (int windowMarginRight);
		afx_msg	int		OLE_GetPrintMarginTop ();
		afx_msg	void	OLE_SetPrintMarginTop (int printMarginTop);
		afx_msg	int		OLE_GetPrintMarginLeft ();
		afx_msg	void	OLE_SetPrintMarginLeft (int printMarginLeft);
		afx_msg	int		OLE_GetPrintMarginBottom ();
		afx_msg	void	OLE_SetPrintMarginBottom (int printMarginBottom);
		afx_msg	int		OLE_GetPrintMarginRight ();
		afx_msg	void	OLE_SetPrintMarginRight (int printMarginRight);
		afx_msg	UINT	GetHasVerticalScrollBar ();
		afx_msg void	SetHasVerticalScrollBar (UINT bNewValue);
		afx_msg UINT	GetHasHorizontalScrollBar ();
		afx_msg void	SetHasHorizontalScrollBar (UINT bNewValue);
		afx_msg BSTR	GetBufferText();
		afx_msg void	SetBufferText(LPCTSTR text);
		afx_msg BSTR	GetBufferTextCRLF();
		afx_msg void	SetBufferTextCRLF(LPCTSTR text);
		afx_msg BSTR	GetBufferTextAsRTF();
		afx_msg string	GetBufferTextAsRTF_();
		afx_msg void	SetBufferTextAsRTF(LPCTSTR text);
		afx_msg BSTR	GetBufferTextAsHTML();
		afx_msg void	SetBufferTextAsHTML(LPCTSTR text);
		afx_msg	VARIANT	GetBufferTextAsDIB ();
		afx_msg long	GetBufferLength ();
		afx_msg long	GetMaxLength ();
		afx_msg void	SetMaxLength (long maxLength);
		afx_msg BOOL	GetSupportContextMenu();
		afx_msg void	SetSupportContextMenu(BOOL bNewValue);
		afx_msg BOOL	OLE_GetHideDisabledContextMenuItems ();
		afx_msg void	OLE_SetHideDisabledContextMenuItems (BOOL bNewValue);
		afx_msg BOOL	GetSmartCutAndPaste();
		afx_msg void	SetSmartCutAndPaste(BOOL bNewValue);
		afx_msg BOOL	OLE_GetSmartQuoteMode ();
		afx_msg void	OLE_SetSmartQuoteMode (BOOL bNewValue);
		afx_msg BOOL	GetWrapToWindow ();
		afx_msg void	SetWrapToWindow (BOOL bNewValue);
		afx_msg BOOL	GetShowParagraphGlyphs ();
		afx_msg void	SetShowParagraphGlyphs (BOOL bNewValue);
		afx_msg BOOL	GetShowTabGlyphs ();
		afx_msg void	SetShowTabGlyphs (BOOL bNewValue);
		afx_msg BOOL	GetShowSpaceGlyphs ();
		afx_msg void	SetShowSpaceGlyphs (BOOL bNewValue);
		afx_msg BOOL	OLE_GetUseSelectEOLBOLRowHilightStyle ();
		afx_msg void	OLE_SetUseSelectEOLBOLRowHilightStyle (BOOL bNewValue);
		afx_msg BOOL	OLE_GetShowSecondaryHilight ();
		afx_msg void	OLE_SetShowSecondaryHilight (BOOL bNewValue);

	// Hidable text
	protected:
		afx_msg BOOL		OLE_GetShowHidableText ();
		afx_msg void		OLE_SetShowHidableText (BOOL bNewValue);
		afx_msg OLE_COLOR	OLE_GetHidableTextColor ();
		afx_msg void		OLE_SetHidableTextColor (OLE_COLOR color);
		afx_msg BOOL		OLE_GetHidableTextColored ();
		afx_msg void		OLE_SetHidableTextColored (BOOL bNewValue);

	// Spell checking
	protected:
		afx_msg VARIANT		OLE_GetSpellChecker ();
		afx_msg void		OLE_SetSpellChecker (VARIANT& newValue);
	private:
		IDispatch*					fSpellChecker;
		COMBased_SpellCheckEngine	fLedSpellCheckWrapper;
	private:
		nonvirtual	void	ChangedSpellCheckerCOMObject ();

	// Context Menu (and commands) support
	private:
		CComPtr<IDispatch>	fConextMenu;
		CComPtr<IDispatch>	fToolbarList;
		CComPtr<IDispatch>	fBuiltinCommands;
		CComPtr<IDispatch>	fPredefinedMenus;
		CComPtr<IDispatch>	fAcceleratorTable;
	protected:
		afx_msg VARIANT		OLE_GetContextMenu ();
		afx_msg void		OLE_SetContextMenu (VARIANT& newValue);
		afx_msg VARIANT		OLE_GetToolbarList ();
		afx_msg void		OLE_SetToolbarList (VARIANT& newValue);
		afx_msg	VARIANT		OLE_GetBuiltinCommands ();
		afx_msg	VARIANT		OLE_GetPredefinedMenus ();
		afx_msg IDispatch*	OLE_GetDefaultContextMenu ();
		afx_msg	IDispatch*	OLE_GetDefaultAcceleratorTable ();
		afx_msg IDispatch*	OLE_MakeNewPopupMenuItem ();
		afx_msg IDispatch*	OLE_MakeNewUserMenuItem ();
		afx_msg IDispatch*	OLE_MakeNewAcceleratorElement ();
		afx_msg void		OLE_InvokeCommand (const VARIANT& command);
		afx_msg BOOL		OLE_CommandEnabled (const VARIANT& command);
		afx_msg BOOL		OLE_CommandChecked (const VARIANT& command);
		afx_msg IDispatch*	OLE_MakeNewToolbarList ();
		nonvirtual	CComPtr<IDispatch>	MakeNewToolbar ();
		afx_msg		IDispatch*			OLE_MakeNewToolbar ();
		afx_msg		IDispatch*			OLE_MakeIconButtonToolbarItem ();
		nonvirtual	CComPtr<IDispatch>	MakeSeparatorToolbarItem ();
		afx_msg		IDispatch*			OLE_MakeSeparatorToolbarItem ();
		nonvirtual	CComPtr<IDispatch>	MakeBuiltinToolbar (LPCOLESTR builtinToolbarName);
		afx_msg		IDispatch*			OLE_MakeBuiltinToolbar (LPCOLESTR builtinToolbarName);
		nonvirtual	CComPtr<IDispatch>	MakeBuiltinToolbarItem (LPCOLESTR builtinToolbarItemName);
		afx_msg		IDispatch*			OLE_MakeBuiltinToolbarItem (LPCOLESTR builtinToolbarItemName);
		afx_msg		VARIANT				OLE_GetAcceleratorTable ();
		afx_msg		void				OLE_SetAcceleratorTable (VARIANT& newValue);

	private:
		struct	ToolBarIconSpec {
			const TCHAR*	fIconName;
			int				fIconResId;
			const TCHAR*	fCmdName;
			IconButtonStyle	fButtonStyle;
		};
		nonvirtual	CComPtr<IDispatch>	mkIconElement (int iconResID);
		nonvirtual	CComPtr<IDispatch>	mkIconElement (const ToolBarIconSpec& s);
		nonvirtual	CComPtr<IDispatch>	mkIconElement (int iconResID, CComPtr<IDispatch> cmdList);
		nonvirtual	CComPtr<IDispatch>	MakeBuiltinComboBoxToolbarItem (CComPtr<IDispatch> cmdList);

	private:
		nonvirtual	HACCEL	GetCurrentWin32AccelTable ();
	private:
		HACCEL	fWin32AccelTable;
		float	fLastAccelTableUpdateAt;	// speed tweek


	protected:
		afx_msg VARIANT		OLE_GetCurrentEventArguments ();
	private:
		CComPtr<IDispatch>	fCurrentEventArguments;

	protected:
		//selection-based APIs
		afx_msg long		GetSelStart ();
		afx_msg void		SetSelStart (long start);
		afx_msg long		GetSelLength ();
		afx_msg void		SetSelLength (long length);
		afx_msg BSTR		GetSelText();
		afx_msg void		SetSelText(LPCTSTR text);
		afx_msg BSTR		GetSelTextAsRTF();
		afx_msg void		SetSelTextAsRTF(LPCTSTR text);
		afx_msg BSTR		GetSelTextAsHTML ();
		afx_msg void		SetSelTextAsHTML(LPCTSTR text);
		afx_msg OLE_COLOR	GetSelColor ();
		afx_msg void		SetSelColor (OLE_COLOR color);
		afx_msg BSTR		GetSelFontFace ();
		afx_msg void		SetSelFontFace (LPCTSTR fontFace);
		afx_msg long		GetSelFontSize ();
		afx_msg void		SetSelFontSize (long size);
		afx_msg long		GetSelBold ();
		afx_msg void		SetSelBold (long bold);
		afx_msg long		GetSelItalic ();
		afx_msg void		SetSelItalic (long italic);
		afx_msg long		GetSelStrikeThru ();
		afx_msg void		SetSelStrikeThru (long strikeThru);
		afx_msg long		GetSelUnderline ();
		afx_msg void		SetSelUnderline (long underline);
		afx_msg UINT		OLE_GetSelJustification ();
		afx_msg void		OLE_SetSelJustification (UINT justification);
		afx_msg UINT		OLE_GetSelListStyle ();
		afx_msg void		OLE_SetSelListStyle (UINT listStyle);
		afx_msg UINT		OLE_GetSelHidable ();
		afx_msg void		OLE_SetSelHidable (UINT hidable);

		afx_msg		void	AboutBox ();
		afx_msg		void	LoadFile (LPCTSTR filename);
		afx_msg		void	SaveFile (LPCTSTR filename);
		afx_msg		void	SaveFileCRLF (LPCTSTR filename);
		afx_msg		void	SaveFileRTF (LPCTSTR filename);
		afx_msg		void	SaveFileHTML (LPCTSTR filename);
		afx_msg		void	Refresh ();
		afx_msg		void	ScrollToSelection ();

	//UNDO support
	public:
		afx_msg long	OLE_GetMaxUndoLevel ();
		afx_msg void	OLE_SetMaxUndoLevel (long maxUndoLevel);
		afx_msg BOOL	OLE_GetCanUndo ();
		afx_msg BOOL	OLE_GetCanRedo ();
		afx_msg void	OLE_Undo ();
		afx_msg void	OLE_Redo ();
		afx_msg void	OLE_CommitUndo ();

	public:
		afx_msg	void	OLE_LaunchFontSettingsDialog ();
		afx_msg	void	OLE_LaunchParagraphSettingsDialog ();

	// Find dialog
	public:
		afx_msg	void	OLE_LaunchFindDialog ();
		afx_msg	long	OLE_Find (long searchFrom, const VARIANT& findText, BOOL wrapSearch, BOOL wholeWordSearch, BOOL caseSensativeSearch);

	// Replace dialog
	public:
		afx_msg	void	OLE_LaunchReplaceDialog ();
		afx_msg	long	OLE_FindReplace (long searchFrom, const VARIANT& findText, const VARIANT& replaceText, BOOL wrapSearch, BOOL wholeWordSearch, BOOL caseSensativeSearch);

	// Printing support
	public:
		afx_msg	void	OLE_PrinterSetupDialog ();
		afx_msg	void	OLE_PrintDialog ();
		afx_msg	void	OLE_PrintOnce ();
 
	// GetHeight
	public:
		afx_msg	long	OLE_GetHeight (long from, long to);

	public:
		afx_msg BOOL	OLE_GetDirty ();
		afx_msg void	OLE_SetDirty (BOOL dirty);
	private:
		bool	fDataDirty;		// keep separate flag - instead of using IsModfied/SetModifiedFlag () - cuz those set for other properties than the text (like control size etc). This is only for DOCUMENT data being dirty.

	//LedItViewController callbacks
	public:
		override	void	OnBrowseHelpCommand ();
		override	void	OnAboutBoxCommand ();
		override	void	ForceUIActive ();
		override	void	FireOLEEvent (DISPID eventID);
		override	void	FireOLEEvent (DISPID dispid, BYTE* pbParams, ...);
		override	void	FireUpdateUserCommand (const wstring& internalCmdName, VARIANT_BOOL* enabled, VARIANT_BOOL* checked, wstring* name);
		override	void	FireUserCommand (const wstring& internalCmdName);
#if		qFunnyDisplayInDesignMode
		override	bool	IsInDesignMode () const;
		override	bool	DrawExtraDesignModeBorder () const;
#endif
		override	HMENU	GenerateContextMenu ();


	protected:
		DECLARE_DISPATCH_MAP()
		DECLARE_MESSAGE_MAP()
		DECLARE_EVENT_MAP()

	private:
		nonvirtual	Led_FileFormat	GuessFormatFromName (LPCTSTR name);
		nonvirtual	void			DoReadFile (LPCTSTR filename, Led_SmallStackBuffer<char>* buffer, size_t* size);
		nonvirtual	void			WriteBytesToFile (LPCTSTR filename, const void* buffer, size_t size);

	// Dispatch and event IDs
	public:
		LedItView		fEditor;
		HTMLInfo		fHTMLInfo;

};


#if		qQuiteAnnoyingDominanceWarnings
	#pragma	warning (default : 4250)
#endif




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//	class	ActiveLedItControl::OnCreateExtras
	inline	ActiveLedItControl::OnCreateExtras::OnCreateExtras ():
		fReadOnly (false),
		fEnabled (true)
		{
		}


#endif	/*__ActiveLedItControl_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

