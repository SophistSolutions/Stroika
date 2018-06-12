/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#include <afxadv.h>

#include "ColorMenu.h"
#include "LedItResources.h"
#include "LedItView.h"
#include "Options.h"

#if qPlatform_Windows
#include "LedItApplication.h"
#endif

#include "LedItMainFrame.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::Platform;
using namespace Stroika::Frameworks::Led::StyledTextIO;

inline LedItView* GetActiveLedItView ()
{
// For MDI
#if 0
    CMDIFrameWnd*   mainFrame   =   dynamic_cast<CMDIFrameWnd*> (::AfxGetMainWnd ());
    if (mainFrame != NULL) {
        CMDIChildWnd*   childFrame  =   mainFrame->MDIGetActive ();
        if (childFrame != NULL) {
            return dynamic_cast<LedItView*> (childFrame->GetActiveView ());
        }
    }
#endif
    // For SDI
    CFrameWnd* mainFrame = dynamic_cast<CFrameWnd*> (::AfxGetMainWnd ());
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

IMPLEMENT_DYNCREATE (LedItMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP (LedItMainFrame, CFrameWnd)
ON_WM_CREATE ()
ON_WM_CLOSE ()
ON_WM_INITMENUPOPUP ()
ON_COMMAND (ID_HELP_FINDER, OnHelpFinder)
ON_COMMAND (ID_HELP, OnHelp)
ON_COMMAND (ID_CONTEXT_HELP, OnContextHelp)
ON_COMMAND (ID_DEFAULT_HELP, OnHelpFinder)
ON_COMMAND (ID_CHAR_COLOR, OnCharColor)
ON_COMMAND_EX (ID_VIEW_FORMATBAR, OnBarCheck)
ON_UPDATE_COMMAND_UI (ID_VIEW_FORMATBAR, OnUpdateControlBarMenu)
ON_COMMAND_EX (ID_VIEW_RULER, OnBarCheck)
ON_UPDATE_COMMAND_UI (ID_VIEW_RULER, OnUpdateControlBarMenu)
END_MESSAGE_MAP ()

LedItMainFrame::LedItMainFrame ()
    : inherited ()
    , fStatusBar ()
    , fToolBar ()
    , fFormatBar (NULL)
{
    fFormatBar = new FormatToolbar (*this);
}

LedItMainFrame::~LedItMainFrame ()
{
    delete fFormatBar;
}

int LedItMainFrame::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
    if (inherited::OnCreate (lpCreateStruct) == -1) {
        return -1;
    }

    Led_Size desiredSize = Led_Size (
        Led_CvtScreenPixelsFromTWIPSV (Led_TWIPS (1440 * 11)),
        //Led_CvtScreenPixelsFromTWIPSH (Led_TWIPS (1440 * 8.5))
        Led_CvtScreenPixelsFromTWIPSH (Led_TWIPS (static_cast<long> (1440 * 7.5))));
    Led_Rect newBounds = Led_Rect (lpCreateStruct->y, lpCreateStruct->x, desiredSize.v, desiredSize.h);
    newBounds          = EnsureRectOnScreen (newBounds);
    MoveWindow (CRect (AsRECT (newBounds)));

    CMenu* menuBar = GetMenu ();
    LedItApplication::Get ().FixupFontMenu (menuBar->GetSubMenu (4)->GetSubMenu (0));

    if (!fToolBar.Create (this) || !fToolBar.LoadToolBar (IDR_MAINFRAME)) {
        TRACE0 ("Failed to create toolbar\n");
        return -1; // fail to create
    }

    AssertNotNull (fFormatBar);
    if (!fFormatBar->Create (this, WS_CHILD | WS_VISIBLE | CBRS_TOP, ID_VIEW_FORMATBAR)) {
        TRACE0 ("Failed to create FormatBar\n");
        return -1; // fail to create
    }

    const UINT indicators[] = {
        ID_SEPARATOR, // status line indicator
        ID_INDICATOR_CAPS,
        ID_INDICATOR_NUM,
        ID_INDICATOR_SCRL,
    };
    if (!fStatusBar.Create (this) || !fStatusBar.SetIndicators (indicators, sizeof (indicators) / sizeof (UINT))) {
        TRACE0 ("Failed to create status bar\n");
        return -1; // fail to create
    }

    // TODO: Remove this if you don't want tool tips or a resizeable toolbar
    fToolBar.SetBarStyle (fToolBar.GetBarStyle () | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

    EnableDocking (CBRS_ALIGN_ANY);

    if (!fRulerBar.Create (this, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_HIDE_INPLACE, ID_VIEW_RULER)) {
        TRACE0 ("Failed to create ruler\n");
        return -1; // fail to create
    }

    // TODO: Delete these three lines if you don't want the toolbar to
    //  be dockable
    fToolBar.EnableDocking (CBRS_ALIGN_ANY);
    AssertNotNull (fFormatBar);
    fFormatBar->EnableDocking (CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM);
    DockControlBar (&fToolBar);
    DockControlBar (fFormatBar);
    SetDockState (Options ().GetDocBarState ());

    DragAcceptFiles ();

    return 0;
}

void LedItMainFrame::OnClose ()
{
    CDockState dockState;
    GetDockState (dockState);
    Options ().SetDocBarState (dockState);
    inherited::OnClose ();
}

BOOL LedItMainFrame::PreCreateWindow (CREATESTRUCT& cs)
{
    return inherited::PreCreateWindow (cs);
}

Led_IncrementalFontSpecification LedItMainFrame::GetCurFont () const
{
    Led_IncrementalFontSpecification ifsp;
    LedItView*                       liv = GetActiveLedItView ();
    if (liv != NULL) {
        ifsp = liv->GetCurSelFontSpec ();
    }
    return ifsp;
}

void LedItMainFrame::SetCurFont (const Led_IncrementalFontSpecification& fsp)
{
    LedItView* liv = GetActiveLedItView ();
    if (liv != NULL) {
        liv->InteractiveSetFont (fsp);
    }
}

void LedItMainFrame::OnCharColor ()
{
    ColorMenu colorMenu;
    CRect     rc;
    AssertNotNull (fFormatBar);
    int index = fFormatBar->CommandToIndex (ID_CHAR_COLOR);
    fFormatBar->GetItemRect (index, &rc);
    fFormatBar->ClientToScreen (rc);
    colorMenu.TrackPopupMenu (TPM_LEFTALIGN | TPM_LEFTBUTTON, rc.left, rc.bottom, this);
}

static bool IsPopupInOwningMenu (HMENU popup, HMENU potentialOwner)
{
    RequireNotNull (popup);
    if (potentialOwner != NULL) {
        int nIndexMax = ::GetMenuItemCount (potentialOwner);
        for (int nIndex = 0; nIndex < nIndexMax; nIndex++) {
            HMENU itsSubMenu = ::GetSubMenu (potentialOwner, nIndex);
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
void LedItMainFrame::OnInitMenuPopup (CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
    // Disable inappropriate commands (done by inherited version)
    inherited::OnInitMenuPopup (pPopupMenu, nIndex, bSysMenu);

    // Check if this is our context menu, or the systemmenu, or one of the
    // applications main window menus.
    //  We only do our remove-disabled hack for the context menu...
    if (bSysMenu or IsPopupInOwningMenu (pPopupMenu->m_hMenu, ::GetMenu (m_hWnd))) {
        return;
    }

    // Remove disabled menu items from the popup (to save space).
    // Remove consecutive (or leading) separators as useless.
    CCmdUI state;
    state.m_pSubMenu = NULL;
    state.m_pMenu    = pPopupMenu;
    Assert (state.m_pOther == NULL);
    Assert (state.m_pParentMenu == NULL);

    state.m_nIndexMax = pPopupMenu->GetMenuItemCount ();

    // Remove disabled items (and unneeded separators)
    {
        bool prevItemSep = true; // prevent initial separators
        for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;) {
            state.m_nID = pPopupMenu->GetMenuItemID (state.m_nIndex);
            if (state.m_nID == 0 and prevItemSep) {
                pPopupMenu->RemoveMenu (state.m_nIndex, MF_BYPOSITION);
                state.m_nIndexMax--;
                continue;
            }
            if (state.m_nID != 0) {
                MENUITEMINFO mInfo;
                memset (&mInfo, 0, sizeof (mInfo));
                mInfo.cbSize = sizeof (mInfo);
                mInfo.fMask  = MIIM_STATE;
                Verify (::GetMenuItemInfo (pPopupMenu->GetSafeHmenu (), state.m_nIndex, true, &mInfo));
                if (mInfo.fState & MFS_DISABLED) {
                    pPopupMenu->RemoveMenu (state.m_nIndex, MF_BYPOSITION);
                    state.m_nIndexMax--;
                    continue;
                }
            }
            prevItemSep = bool(state.m_nID == 0);
            state.m_nIndex++;
        }
        // if LAST item is a separator - remove THAT
        if (prevItemSep) {
            pPopupMenu->RemoveMenu (state.m_nIndexMax - 1, MF_BYPOSITION);
        }
    }
}

#ifdef _DEBUG
void LedItMainFrame::AssertValid () const
{
    inherited::AssertValid ();
}

void LedItMainFrame::Dump (CDumpContext& dc) const
{
    inherited::Dump (dc);
}
#endif //_DEBUG
