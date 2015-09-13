/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LedLineItInPlaceFrame_h__
#define	__LedLineItInPlaceFrame_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItMFC/Headers/LedLineItInPlaceFrame.h,v 2.5 2002/05/06 21:31:14 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedLineItInPlaceFrame.h,v $
 *	Revision 2.5  2002/05/06 21:31:14  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.4  2001/11/27 00:28:20  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.3  2001/08/30 01:01:59  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.2  1997/12/24 04:50:02  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.1  1997/07/27  16:02:50  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.0  1997/01/10  03:35:47  lewis
 *	*** empty log message ***
 *
 *
 *
 *	<========== BasedOnLedIt! 2.1 ==========>
 *
 *
 *
 *
 */

#include	<afxole.h>


class LedLineItInPlaceFrame : public COleIPFrameWnd {
		DECLARE_DYNCREATE(LedLineItInPlaceFrame)
	public:
		LedLineItInPlaceFrame();

	public:
		virtual BOOL OnCreateControlBars(CFrameWnd* pWndFrame, CFrameWnd* pWndDoc);
		virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// Implementation
	public:
		virtual ~LedLineItInPlaceFrame();

	protected:
		CToolBar    	m_wndToolBar;
		COleResizeBar   m_wndResizeBar;
		COleDropTarget	m_dropTarget;

	protected:
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		DECLARE_MESSAGE_MAP()

	#ifdef _DEBUG
	public:
		virtual void AssertValid() const;
		virtual void Dump(CDumpContext& dc) const;
	#endif
};

#endif	/*__LedLineItInPlaceFrame_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

