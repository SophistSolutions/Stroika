/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */

#ifndef __LedItMainFrame_h__
#define __LedItMainFrame_h__ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <afxext.h>
#include <afxwin.h>

#include "Stroika/Frameworks/Led/Support.h"

#include "FormatToolbar.h"
#include "LedItConfig.h"
#include "RulerToolbar.h"

class LedItMainFrame : public CFrameWnd, private FormatToolbarOwner {
private:
    using inherited = CFrameWnd;

protected: // create from serialization only
    LedItMainFrame ();
    DECLARE_DYNCREATE (LedItMainFrame)
public:
    ~LedItMainFrame ();

public:
    virtual BOOL PreCreateWindow (CREATESTRUCT& cs) override;

public:
    virtual Led_IncrementalFontSpecification GetCurFont () const override;
    virtual void                             SetCurFont (const Led_IncrementalFontSpecification& fsp) override;

protected:
    afx_msg void OnCharColor ();

private:
    CStatusBar     fStatusBar;
    CToolBar       fToolBar;
    FormatToolbar* fFormatBar;
    RulerBar       fRulerBar;

protected:
    afx_msg int  OnCreate (LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnClose ();
    afx_msg void OnInitMenuPopup (CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
    DECLARE_MESSAGE_MAP ()

#ifdef _DEBUG
public:
    virtual void AssertValid () const;
    virtual void Dump (CDumpContext& dc) const;
#endif
};

#endif /*__LedItMainFrame_h__*/
