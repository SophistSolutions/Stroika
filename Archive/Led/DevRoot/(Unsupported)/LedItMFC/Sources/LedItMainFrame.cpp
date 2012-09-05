/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItMFC/Sources/LedItMainFrame.cpp,v 2.14 2000/06/17 05:59:55 lewis Exp $
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
 *	$Log: LedItMainFrame.cpp,v $
 *	Revision 2.14  2000/06/17 05:59:55  lewis
 *	SPR#0794- eliminate cruft displayed sometimes around the format bar control bar.
 *	
 *	Revision 2.13  2000/06/08 21:22:18  lewis
 *	SPR#0773- call DragAcceptFiles
 *	
 *	Revision 2.12  1999/03/25 02:45:51  lewis
 *	use new InteractiveSetFont - just a new name wrapper - on old SetDefaultFont
 *	
 *	Revision 2.11  1998/10/30 14:56:05  lewis
 *	mosstly new msvc60 pragma warnign changes
 *	
 *	Revision 2.10  1997/12/24  04:47:51  lewis
 *	*** empty log message ***
 *
 *	Revision 2.9  1997/12/24  04:16:41  lewis
 *	Add new support for ruler, fonttoolbar, and saving state / and restoring state of toolbar.
 *
 *	Revision 2.8  1997/07/27  16:01:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.7  1997/06/28  17:29:40  lewis
 *	Cleanups, and OnInitMenuPopup () to remove disabled menu items from popups.
 *
 *	Revision 2.6  1997/06/18  03:42:43  lewis
 *	qIncludePrefixFile and cleanups
 *
 *	Revision 2.5  1997/01/10  03:32:04  lewis
 *	*** empty log message ***
 *
 *	Revision 2.4  1996/12/13  18:03:20  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.3  1996/12/05  21:18:00  lewis
 *	*** empty log message ***
 *
 *	Revision 2.2  1996/09/01  15:43:56  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.1  1996/06/01  02:46:15  lewis
 *	*** empty log message ***
 *
 *	Revision 2.0  1996/01/04  00:58:05  lewis
 *	*** empty log message ***
 *
 *	Revision 1.3  1996/01/04  00:57:44  lewis
 *	*** empty log message ***
 *
 *	Revision 1.2  1995/12/08  07:47:54  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<afxadv.h>

#include	"ColorMenu.h"
#include	"FontMenu.h"
#include	"Resource.h"
#include	"LedItView.h"
#include	"Options.h"

#include	"LedItMainFrame.h"



	inline	LedItView*	GetActiveLedItView ()
		{
			// For MDI
			#if 0
				CMDIFrameWnd*	mainFrame	=	dynamic_cast<CMDIFrameWnd*> (::AfxGetMainWnd ());
				if (mainFrame != NULL) {
					CMDIChildWnd*	childFrame	=	mainFrame->MDIGetActive ();
					if (childFrame != NULL) {
						return dynamic_cast<LedItView*> (childFrame->GetActiveView ());
					}
				}
			#endif
			// For SDI
				CFrameWnd*	mainFrame	=	dynamic_cast<CFrameWnd*> (::AfxGetMainWnd ());
				if (mainFrame != NULL) {
					return dynamic_cast<LedItView*> (mainFrame->GetActiveView ());
				}
			return NULL;
		}




/*
 ********************************************************************************
 ******************************** LedItMainFrame ********************************
 ********************************************************************************
 */

IMPLEMENT_DYNCREATE(LedItMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(LedItMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_INITMENUPOPUP			()
	ON_COMMAND					(ID_HELP_FINDER, 	OnHelpFinder)
	ON_COMMAND					(ID_HELP, 			OnHelp)
	ON_COMMAND					(ID_CONTEXT_HELP, 	OnContextHelp)
	ON_COMMAND					(ID_DEFAULT_HELP, 	OnHelpFinder)
	ON_COMMAND					(ID_CHAR_COLOR,		OnCharColor)
	ON_COMMAND_EX				(ID_VIEW_FORMATBAR, OnBarCheck)
	ON_UPDATE_COMMAND_UI		(ID_VIEW_FORMATBAR,	OnUpdateControlBarMenu)
	ON_COMMAND_EX				(ID_VIEW_RULER,		OnBarCheck)
	ON_UPDATE_COMMAND_UI		(ID_VIEW_RULER,		OnUpdateControlBarMenu)
END_MESSAGE_MAP()

LedItMainFrame::LedItMainFrame ():
	inherited (),
	fStatusBar (),
	fToolBar (),
	fFormatBar (*this)
{
}

int LedItMainFrame::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
	if (inherited::OnCreate (lpCreateStruct) == -1) {
		return -1;
	}

	CMenu*	menuBar	=	GetMenu ();
	FixupFontMenu (menuBar->GetSubMenu (2)->GetSubMenu (0));

	if (!fToolBar.Create(this) || !fToolBar.LoadToolBar(IDR_MAINFRAME)) {
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!fFormatBar.Create (this, WS_CHILD|WS_VISIBLE|CBRS_TOP, ID_VIEW_FORMATBAR)) {
		TRACE0("Failed to create FormatBar\n");
		return -1;      // fail to create
	}

	const	UINT indicators[] = {
		ID_SEPARATOR,           // status line indicator
		ID_INDICATOR_CAPS,
		ID_INDICATOR_NUM,
		ID_INDICATOR_SCRL,
	};
	if (!fStatusBar.Create (this) || !fStatusBar.SetIndicators (indicators, sizeof(indicators)/sizeof(UINT))) {
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	fToolBar.SetBarStyle (fToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	EnableDocking (CBRS_ALIGN_ANY);

	if (!fRulerBar.Create (this, WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_HIDE_INPLACE, ID_VIEW_RULER)) {
		TRACE0("Failed to create ruler\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	fToolBar.EnableDocking (CBRS_ALIGN_ANY);
	fFormatBar.EnableDocking (CBRS_ALIGN_TOP|CBRS_ALIGN_BOTTOM);
	DockControlBar (&fToolBar);
	DockControlBar (&fFormatBar);
	SetDockState (Options ().GetDocBarState ());

	DragAcceptFiles ();

	return 0;
}

void	LedItMainFrame::OnClose ()
{
	CDockState	dockState;
	GetDockState (dockState);
	Options ().SetDocBarState (dockState);
	inherited::OnClose ();
}

BOOL	LedItMainFrame::PreCreateWindow (CREATESTRUCT& cs)
{
	return inherited::PreCreateWindow (cs);
}

Led_IncrementalFontSpecification	LedItMainFrame::GetCurFont () const
{
	Led_IncrementalFontSpecification	ifsp;
	LedItView*	liv	=	GetActiveLedItView ();
	if (liv != NULL) {
		ifsp = liv->GetCurSelFontSpec ();
	}
	return ifsp;
}

void	LedItMainFrame::SetCurFont (const Led_IncrementalFontSpecification& fsp)
{
	LedItView*	liv	=	GetActiveLedItView ();
	if (liv != NULL) {
		liv->InteractiveSetFont (fsp);
	}
}

void	LedItMainFrame::OnCharColor () 
{
	ColorMenu colorMenu;
	CRect rc;
	int index = fFormatBar.CommandToIndex (ID_CHAR_COLOR);
	fFormatBar.GetItemRect (index, &rc);
	fFormatBar.ClientToScreen (rc);
	colorMenu.TrackPopupMenu (TPM_LEFTALIGN|TPM_LEFTBUTTON,rc.left,rc.bottom, this);
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
void	LedItMainFrame::OnInitMenuPopup (CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
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
	}
}

#ifdef _DEBUG
void	LedItMainFrame::AssertValid () const
{
	inherited::AssertValid ();
}

void	LedItMainFrame::Dump (CDumpContext& dc) const
{
	inherited::Dump (dc);
}
#endif //_DEBUG




// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
