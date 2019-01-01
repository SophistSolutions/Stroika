/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef __LedLineItMainFrame_h__
#define __LedLineItMainFrame_h__ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <afxext.h>
#include <afxwin.h>

#include "Stroika/Frameworks/Led/Support.h"

#include "LedLineItConfig.h"

class LedLineItMainFrame : public CMDIFrameWnd {
private:
    using inherited = CMDIFrameWnd;

public:
    LedLineItMainFrame ();

protected:
    DECLARE_DYNCREATE (LedLineItMainFrame)

public:
    nonvirtual bool GetStatusBarShown () const;
    nonvirtual void SetStatusBarShown (bool shown);

public:
    nonvirtual void TrackInGotoLineField ();

public:
    class StatusBar : public CStatusBar {
    private:
        using inherited = CStatusBar;

    public:
        StatusBar ()
            : CStatusBar ()
            , fGotoEdit ()
        {
        }

    protected:
        afx_msg void OnLButtonDown (UINT nFlags, CPoint oPoint);
        afx_msg void OnMagicLoseFocus ();
        afx_msg void OnMagicEdited ();

    protected:
        nonvirtual void TrackInGotoLineField ();

    public:
        class GotoEdit : public CEdit {
        public:
            afx_msg UINT OnGetDlgCode ();
            afx_msg void OnKeyDown (UINT nChar, UINT /*nRepCnt*/, UINT /*nFlags*/);
            DECLARE_MESSAGE_MAP ()
        };
        GotoEdit fGotoEdit;

        DECLARE_MESSAGE_MAP ()
        friend class LedLineItMainFrame;
    };
    StatusBar fStatusBar;
    CToolBar  fToolBar;

protected:
    afx_msg int  OnCreate (LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnInitMenuPopup (CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);

protected:
    afx_msg void OnUpdateLineIndicator (CCmdUI* pCmdUI);
    afx_msg void OnCloseWindowCommand ();
    afx_msg void OnCloseAllWindowsCommand ();

protected:
    DECLARE_MESSAGE_MAP ()
};

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
inline LedLineItMainFrame::LedLineItMainFrame ()
    : inherited ()
    , fStatusBar ()
    , fToolBar ()
{
}

#endif /*__LedLineItMainFrame_h__*/
