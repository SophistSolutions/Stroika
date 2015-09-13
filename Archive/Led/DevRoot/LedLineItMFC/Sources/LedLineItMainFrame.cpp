/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItMFC/Sources/LedLineItMainFrame.cpp,v 2.21 2003/01/20 16:31:10 lewis Exp $
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
 *	$Log: LedLineItMainFrame.cpp,v $
 *	Revision 2.21  2003/01/20 16:31:10  lewis
 *	minor tweeks noted by MWERKS CW8.3 compiler
 *	
 *	Revision 2.20  2002/09/11 04:24:07  lewis
 *	Fix OnInitPopupMenu - assure we never get a separator left at the bottom of the menu
 *	
 *	Revision 2.19  2002/09/04 02:33:03  lewis
 *	fix warning
 *	
 *	Revision 2.18  2002/05/06 21:31:20  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.17  2001/11/27 00:28:23  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.16  2001/09/08 16:09:41  lewis
 *	SPR#1017- use new EnsureRectOnScreen routine to pick better app frame window default size
 *	
 *	Revision 2.15  2001/08/30 01:02:01  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.14  2000/06/08 21:49:49  lewis
 *	SPR#0774- added option to treat TAB as indent-selection command - as MSDEV does
 *	
 *	Revision 2.13  2000/06/08 20:32:16  lewis
 *	SPR#0771- close and closeall menu items in windows menu
 *	
 *	Revision 2.12  1999/12/25 04:15:51  lewis
 *	Add MENU COMMANDS for the qSupportSyntaxColoring option. And debug it so it worked
 *	properly turning on/off and having multiple windows onto the same doc at the same time
 *	(scrolled to same or differnet) places
 *	
 *	Revision 2.11  1999/12/09 03:29:07  lewis
 *	SPR#0645 - preliminary qSDK_UNICODE (_UNICODE) support (mostly using new Led_SDK_String/Led_SDK_Char)
 *	
 *	Revision 2.10  1998/10/30 15:08:09  lewis
 *	lots of little cleanups - using vector instead of Led_Array, mutable instaed of
 *	const cast, new msvc60 warning
 *	stuff, etc.
 *	
 *	Revision 2.9  1998/04/25  01:48:10  lewis
 *	reset old font object - missing selectobject call
 *
 *	Revision 2.8  1997/12/24  04:51:19  lewis
 *	*** empty log message ***
 *
 *	Revision 2.7  1997/07/27  16:03:03  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.6  1997/06/28  17:36:57  lewis
 *	Be more careful about grabbing the currently active Led view (cuz we're now MDI).
 *	Override OnInitMenuPopup to strip disabled items out of the context menu.
 *
 *	Revision 2.5  1997/06/23  16:41:50  lewis
 *	Fix bug where we crashed on print preview updating mainframe line#.
 *
 *	Revision 2.4  1997/06/18  03:47:44  lewis
 *	qIncludePrefixFile and cleanuips
 *
 *	Revision 2.3  1997/03/23  01:10:04  lewis
 *	Various minor fixes/hacks for the statusbar embedded line# editing.
 *
 *	Revision 2.2  1997/03/04  20:46:37  lewis
 *	*** empty log message ***
 *
 *	Revision 2.1  1997/03/04  20:21:09  lewis
 *	Added support for goto line/line# in status bar.
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
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<stdio.h>

#include	"FontMenu.h"
#include	"Resource.h"
#include	"LedLineItView.h"

#include	"LedLineItMainFrame.h"





	inline	LedLineItView*	GetActiveLedItView ()
		{
			CMDIFrameWnd*	mainFrame	=	dynamic_cast<CMDIFrameWnd*> (::AfxGetMainWnd ());
			if (mainFrame != NULL) {
				CMDIChildWnd*	childFrame	=	mainFrame->MDIGetActive ();
				if (childFrame != NULL) {
					return dynamic_cast<LedLineItView*> (childFrame->GetActiveView ());
				}
			}
			return NULL;
		}

/*
 ********************************************************************************
 ***************** LedLineItMainFrame::StatusBar::GotoEdit **********************
 ********************************************************************************
 */
typedef	LedLineItMainFrame::StatusBar	StatusBar;
typedef	StatusBar::GotoEdit				GotoEdit;
BEGIN_MESSAGE_MAP(GotoEdit, CEdit)
	ON_WM_GETDLGCODE ()
	ON_WM_KEYDOWN	()
END_MESSAGE_MAP()

UINT	GotoEdit::OnGetDlgCode ()
{
	return CEdit::OnGetDlgCode () | DLGC_WANTALLKEYS;
}

void	GotoEdit::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_TAB or nChar == VK_RETURN) {
		LedLineItView*	v	=	GetActiveLedItView ();
		if (v != NULL) {
			v->SetFocus ();
		}
	}
	else {
		CEdit::OnKeyDown (nChar, nRepCnt, nFlags);
	}
}





/*
 ********************************************************************************
 ************************ LedLineItMainFrame::StatusBar *************************
 ********************************************************************************
 */
const	int	kMagicGotoWidgetID	=	9991;
typedef	LedLineItMainFrame::StatusBar	StatusBar;
BEGIN_MESSAGE_MAP(StatusBar, CStatusBar)
	ON_EN_CHANGE		(kMagicGotoWidgetID,		OnMagicEdited)
	ON_EN_KILLFOCUS		(kMagicGotoWidgetID,		OnMagicLoseFocus)
	ON_WM_LBUTTONDOWN	()
END_MESSAGE_MAP()
void	StatusBar::OnLButtonDown (UINT nFlags, CPoint oPoint)
{
	CRect	r;
	GetItemRect (1, &r);
	if (r.PtInRect (oPoint)) {
		TrackInGotoLineField ();
	}
	else {
		inherited::OnLButtonDown (nFlags, oPoint);
	}
}

void	StatusBar::TrackInGotoLineField ()
{
	CRect	r;
	GetItemRect (1, &r);
	// make sure plenty wide..
	r.right = max (r.right, r.left + 50);
	if (fGotoEdit.m_hWnd == 0) {
		Led_Verify (fGotoEdit.Create (WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_CHILD|WS_VISIBLE|ES_NUMBER|ES_AUTOHSCROLL|ES_RIGHT, r, this, kMagicGotoWidgetID));
	}
	fGotoEdit.SetFont (GetFont ());
	fGotoEdit.SetWindowText (GetPaneText (1));
	fGotoEdit.SetSel (0, -1);
	fGotoEdit.SetFocus ();
}

void	StatusBar::OnMagicEdited ()
{
	CString	wt;
	fGotoEdit.GetWindowText (wt);
	int	lines	=	0;
	if (::_stscanf (wt, _T ("%d"), &lines) == 1) {
		LedLineItView*	v	=	GetActiveLedItView ();
		if (v != NULL) {
			v->SetCurUserLine (lines);
		}
	}
	else {
		Led_BeepNotify ();
	}
}

void	StatusBar::OnMagicLoseFocus ()
{
	Led_Verify (fGotoEdit.DestroyWindow ());
}






/*
 ********************************************************************************
 ******************************** LedLineItMainFrame ****************************
 ********************************************************************************
 */
IMPLEMENT_DYNCREATE(LedLineItMainFrame, CMDIFrameWnd)
BEGIN_MESSAGE_MAP(LedLineItMainFrame, LedLineItMainFrame::inherited)
	ON_WM_CREATE			()
	ON_WM_INITMENUPOPUP		()
	ON_COMMAND				(ID_HELP_FINDER,		OnHelpFinder)
	ON_COMMAND				(ID_HELP,				OnHelp)
	ON_COMMAND				(ID_CONTEXT_HELP,		OnContextHelp)
	ON_COMMAND				(ID_DEFAULT_HELP,		OnHelpFinder)
	ON_UPDATE_COMMAND_UI	(ID_LINENUMBER,			OnUpdateLineIndicator)
	ON_COMMAND				(kCloseWindowCmdID,		OnCloseWindowCommand)
	ON_COMMAND				(kCloseAllWindowsCmdID,	OnCloseAllWindowsCommand)
END_MESSAGE_MAP()

bool	LedLineItMainFrame::GetStatusBarShown () const
{
	#if		qUsePragmaWarningToSilenceNeedlessBoolConversionWarnings
		#pragma	warning (disable : 4800)
	#endif
	return fStatusBar.IsWindowVisible ();
	#if		qUsePragmaWarningToSilenceNeedlessBoolConversionWarnings
		#pragma	warning (default : 4800)
	#endif
}

void	LedLineItMainFrame::SetStatusBarShown (bool shown)
{
	fStatusBar.ShowWindow (shown? SW_SHOWNORMAL: SW_HIDE);
}

void	LedLineItMainFrame::TrackInGotoLineField ()
{
	Led_Require (GetStatusBarShown ());
	fStatusBar.TrackInGotoLineField ();
}

int		LedLineItMainFrame::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
	Led_RequireNotNil (lpCreateStruct);

	if (inherited::OnCreate (lpCreateStruct) == -1) {
		return -1;
	}

	Led_Size	desiredSize	=	Led_Size (
										Led_CvtScreenPixelsFromTWIPSV (Led_TWIPS (1440 * 11)),
										Led_CvtScreenPixelsFromTWIPSH (Led_TWIPS (static_cast<long> (1440 * 8.5)))
								);
	Led_Rect	newBounds	=	Led_Rect (lpCreateStruct->y, lpCreateStruct->x, desiredSize.v, desiredSize.h);
	newBounds = EnsureRectOnScreen (newBounds);
	MoveWindow (CRect (AsRECT (newBounds)));

	CMenu*	menuBar	=	GetMenu ();
	Led_AssertNotNil (menuBar);
	#if		qSupportSyntaxColoring
		FixupFontMenu (menuBar->GetSubMenu (2)->GetSubMenu (7));
	#else
		FixupFontMenu (menuBar->GetSubMenu (2)->GetSubMenu (6));
	#endif

	if (!fToolBar.Create (this) || !fToolBar.LoadToolBar (IDR_MAINFRAME)) {
		TRACE0 ("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	static UINT indicators[] = {
		ID_SEPARATOR,           // status line indicator
		ID_LINENUMBER,
	};
	if (!fStatusBar.Create (this) || !fStatusBar.SetIndicators (indicators, sizeof(indicators)/sizeof(UINT))) {
		TRACE0 ("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	fToolBar.SetBarStyle (fToolBar.GetBarStyle () | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	fToolBar.EnableDocking (CBRS_ALIGN_ANY);
	EnableDocking (CBRS_ALIGN_ANY);
	DockControlBar (&fToolBar);

	return 0;
}

void	LedLineItMainFrame::OnUpdateLineIndicator (CCmdUI* pCmdUI)
{
	Led_RequireNotNil (pCmdUI);
	
	TCHAR	buf[1024];
	{
		LedLineItView*	v	=	GetActiveLedItView ();
		if (v == NULL) {
			// can happen, for example, during PrintPreview
			(void)::_tcscpy (buf, _T ("N/A"));
		}
		else {
			(void)::_stprintf (buf, _T ("%d"), v->GetCurUserLine ());
		}
		pCmdUI->SetText (buf);
	}
	pCmdUI->Enable ();

	// dynamicly size the indicator
	HFONT		hFont	=	(HFONT)fStatusBar.SendMessage (WM_GETFONT);
	CClientDC	dcScreen (NULL);
	HGDIOBJ		oldFont	=	NULL;
	if (hFont != NULL) {
		oldFont	=	dcScreen.SelectObject (hFont);
	}
	UINT	nID	=	0;
	UINT	nStyle	=	0;
	int		cxWidth	=	0;
	fStatusBar.GetPaneInfo (1, nID, nStyle, cxWidth);
	cxWidth = max (dcScreen.GetTextExtent (buf).cx, 20l);
	fStatusBar.SetPaneInfo (1, nID, nStyle, cxWidth);
	if (oldFont != NULL) {
		(void)dcScreen.SelectObject (oldFont);
	}
}

	static	bool	IsPopupInOwningMenu (HMENU popup, HMENU potentialOwner)
	{
		Led_RequireNotNil (popup);
		if (potentialOwner != NULL) {
			int nIndexMax = ::GetMenuItemCount (potentialOwner);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++) {
				HMENU	itsSubMenu	=	::GetSubMenu (potentialOwner, nIndex);
				if (itsSubMenu == popup) {
					return true;
				}
				else if (itsSubMenu != NULL) {
					// See if its inside sub menu.
					if (IsPopupInOwningMenu (popup, itsSubMenu)) {
						return true;
					}
				}
			}
		}
		return false;
	}
void	LedLineItMainFrame::OnInitMenuPopup (CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	// Disable inappropriate commands (done by inherited version)
	inherited::OnInitMenuPopup (pPopupMenu, nIndex, bSysMenu);

	// Check if this is our context menu, or the systemmenu, or one of the
	// applications main window menus.
	//	We only do our remove-disabled hack for the context menu...
	if (bSysMenu or IsPopupInOwningMenu (pPopupMenu->m_hMenu, ::GetMenu (m_hWnd))) {
		return;
	}

	// Remove disabled menu items from the popup (to save space).
	// Remove consecutive (or leading) separators as useless.
	CCmdUI	state;
	state.m_pSubMenu = NULL;
	state.m_pMenu = pPopupMenu;
	Led_Assert (state.m_pOther == NULL);
	Led_Assert (state.m_pParentMenu == NULL);

	state.m_nIndexMax = pPopupMenu->GetMenuItemCount ();

	// Remove disabled items (and unneeded separators)
	{
		bool	prevItemSep	=	true;	// prevent initial separators
		for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; ) {
			state.m_nID = pPopupMenu->GetMenuItemID (state.m_nIndex);
			if (state.m_nID == 0 and prevItemSep) {
				pPopupMenu->RemoveMenu (state.m_nIndex, MF_BYPOSITION);
				state.m_nIndexMax--;
				continue;
			}
			if (state.m_nID != 0) {
				MENUITEMINFO	mInfo;
				memset (&mInfo, 0, sizeof (mInfo));
				mInfo.cbSize = sizeof (mInfo);
				mInfo.fMask = MIIM_STATE;
				Led_Verify (::GetMenuItemInfo (pPopupMenu->GetSafeHmenu (), state.m_nIndex, true, &mInfo));
				if (mInfo.fState & MFS_DISABLED) {
					pPopupMenu->RemoveMenu (state.m_nIndex, MF_BYPOSITION);
					state.m_nIndexMax--;
					continue;
				}
			}
			prevItemSep = bool (state.m_nID == 0);
			state.m_nIndex++;
		}
		// if LAST item is a separator - remove THAT
		if (prevItemSep) {
			pPopupMenu->RemoveMenu (state.m_nIndexMax-1, MF_BYPOSITION);
		}
	}
}

void	LedLineItMainFrame::OnCloseWindowCommand ()
{
	CMDIChildWnd*	childFrame	=	MDIGetActive ();
	if (childFrame != NULL) {
		childFrame->SendMessage (WM_SYSCOMMAND, SC_CLOSE);
	}
}

void	LedLineItMainFrame::OnCloseAllWindowsCommand ()
{
	CMDIChildWnd*	childFrame	=	NULL;
	while ((childFrame =	MDIGetActive ()) != NULL) {
		CDocument* pDocument = childFrame->GetActiveDocument ();
		if (pDocument != NULL) {
			if (pDocument->SaveModified ()) {
				pDocument->OnCloseDocument ();
				continue;
			}
			else {
				return;
			}
		}
		childFrame->SendMessage (WM_SYSCOMMAND, SC_CLOSE);
	}
}




// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
