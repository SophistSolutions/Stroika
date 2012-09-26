/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */

#ifndef __LedItMainFrame_h__
#define __LedItMainFrame_h__    1

#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <afxwin.h>
#include    <afxext.h>

#include    "Stroika/Frameworks/Led/Support.h"

#include    "FormatToolbar.h"
#include    "LedItConfig.h"
#include    "RulerToolbar.h"


class   LedItMainFrame : public CFrameWnd, private FormatToolbarOwner {
private:
    typedef CFrameWnd   inherited;

protected: // create from serialization only
    LedItMainFrame ();
    DECLARE_DYNCREATE(LedItMainFrame)
public:
    ~LedItMainFrame ();

public:
    override    BOOL    PreCreateWindow (CREATESTRUCT& cs);

public:
    override    Led_IncrementalFontSpecification    GetCurFont () const;
    override    void                                SetCurFont (const Led_IncrementalFontSpecification& fsp);

protected:
    afx_msg void    OnCharColor ();

private:
    CStatusBar      fStatusBar;
    CToolBar        fToolBar;
    FormatToolbar*  fFormatBar;
    RulerBar        fRulerBar;

protected:
    afx_msg     int     OnCreate (LPCREATESTRUCT lpCreateStruct);
    afx_msg     void    OnClose ();
    afx_msg     void    OnInitMenuPopup (CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
    DECLARE_MESSAGE_MAP()

#ifdef _DEBUG
public:
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
};

#endif  /*__LedItMainFrame_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

