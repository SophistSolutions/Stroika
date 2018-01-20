/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

#ifndef __LedItInPlaceFrame_h__
#define __LedItInPlaceFrame_h__ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <afxole.h>

class LedItInPlaceFrame : public COleIPFrameWnd {
    DECLARE_DYNCREATE (LedItInPlaceFrame)
public:
    LedItInPlaceFrame ();

    // Attributes
public:
    // Operations
public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(LedItInPlaceFrame)
public:
    virtual BOOL OnCreateControlBars (CFrameWnd* pWndFrame, CFrameWnd* pWndDoc);
    virtual BOOL PreCreateWindow (CREATESTRUCT& cs);
    //}}AFX_VIRTUAL

    // Implementation
public:
    virtual ~LedItInPlaceFrame ();
#ifdef _DEBUG
    virtual void AssertValid () const;
    virtual void Dump (CDumpContext& dc) const;
#endif

protected:
    CToolBar       m_wndToolBar;
    COleResizeBar  m_wndResizeBar;
    COleDropTarget m_dropTarget;

    // Generated message map functions
protected:
    //{{AFX_MSG(LedItInPlaceFrame)
    afx_msg int OnCreate (LPCREATESTRUCT lpCreateStruct);
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP ()
};

#endif /*__LedItInPlaceFrame_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
