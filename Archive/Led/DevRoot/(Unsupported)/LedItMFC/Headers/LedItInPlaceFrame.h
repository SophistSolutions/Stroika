/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */
#ifndef	__LedItInPlaceFrame_h__
#define	__LedItInPlaceFrame_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItMFC/Headers/LedItInPlaceFrame.h,v 2.5 1997/12/24 04:46:26 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedItInPlaceFrame.h,v $
 *	Revision 2.5  1997/12/24 04:46:26  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.4  1997/07/27  16:01:09  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.3  1996/12/13  18:02:41  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.2  1996/09/01  15:43:38  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.1  1996/06/01  02:34:54  lewis
 *	*** empty log message ***
 *
 *	Revision 2.0  1996/01/04  00:55:44  lewis
 *	*** empty log message ***
 *
 *	Revision 1.3  1996/01/04  00:55:22  lewis
 *	*** empty log message ***
 *
 *	Revision 1.2  1995/12/08  07:46:23  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */

#include	<afxole.h>


class LedItInPlaceFrame : public COleIPFrameWnd
{
	DECLARE_DYNCREATE(LedItInPlaceFrame)
public:
	LedItInPlaceFrame();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(LedItInPlaceFrame)
	public:
	virtual BOOL OnCreateControlBars(CFrameWnd* pWndFrame, CFrameWnd* pWndDoc);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~LedItInPlaceFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CToolBar    m_wndToolBar;
	COleResizeBar   m_wndResizeBar;
	COleDropTarget m_dropTarget;

// Generated message map functions
protected:
	//{{AFX_MSG(LedItInPlaceFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif	/*__LedItInPlaceFrame_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

