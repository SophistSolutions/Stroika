/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef __LedLineItInPlaceFrame_h__
#define __LedLineItInPlaceFrame_h__ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

DISABLE_COMPILER_MSC_WARNING_START (5054)
#include <afxole.h>
DISABLE_COMPILER_MSC_WARNING_END (5054)

class LedLineItInPlaceFrame : public COleIPFrameWnd {
    DECLARE_DYNCREATE (LedLineItInPlaceFrame)
public:
    LedLineItInPlaceFrame ();

public:
    virtual BOOL OnCreateControlBars (CFrameWnd* pWndFrame, CFrameWnd* pWndDoc);
    virtual BOOL PreCreateWindow (CREATESTRUCT& cs);

    // Implementation
public:
    virtual ~LedLineItInPlaceFrame ();

protected:
    CToolBar       m_wndToolBar;
    COleResizeBar  m_wndResizeBar;
    COleDropTarget m_dropTarget;

protected:
    afx_msg int OnCreate (LPCREATESTRUCT lpCreateStruct);
    DECLARE_MESSAGE_MAP ()

#ifdef _DEBUG
public:
    virtual void AssertValid () const;
    virtual void Dump (CDumpContext& dc) const;
#endif
};

#endif /*__LedLineItInPlaceFrame_h__*/
