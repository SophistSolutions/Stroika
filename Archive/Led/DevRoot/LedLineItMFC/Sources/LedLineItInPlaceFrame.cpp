/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItMFC/Sources/LedLineItInPlaceFrame.cpp,v 2.7 2002/05/06 21:31:20 lewis Exp $
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
 *	$Log: LedLineItInPlaceFrame.cpp,v $
 *	Revision 2.7  2002/05/06 21:31:20  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.6  2001/11/27 00:28:22  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.5  2001/08/30 01:02:01  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.4  1998/10/30 15:08:09  lewis
 *	lots of little cleanups - using vector instead of Led_Array, mutable
 *	instaed of const cast, new msvc60 warning
 *	stuff, etc.
 *	
 *	Revision 2.3  1997/12/24  04:51:19  lewis
 *	*** empty log message ***
 *
 *	Revision 2.2  1997/07/27  16:03:03  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.1  1997/06/18  03:47:34  lewis
 *	qIncludePrefixFile and cleanups
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
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	"Resource.h"

#include	"LedLineItInPlaceFrame.h"






/*
 ********************************************************************************
 *************************** LedLineItInPlaceFrame ******************************
 ********************************************************************************
 */
IMPLEMENT_DYNCREATE(LedLineItInPlaceFrame, COleIPFrameWnd)

BEGIN_MESSAGE_MAP(LedLineItInPlaceFrame, COleIPFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_HELP_FINDER, OnHelpFinder)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND(ID_DEFAULT_HELP, OnHelpFinder)
	ON_COMMAND(ID_CONTEXT_HELP, OnContextHelp)
END_MESSAGE_MAP()

LedLineItInPlaceFrame::LedLineItInPlaceFrame ():
	COleIPFrameWnd ()
{
}

LedLineItInPlaceFrame::~LedLineItInPlaceFrame ()
{
}

int LedLineItInPlaceFrame::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
	if (COleIPFrameWnd::OnCreate (lpCreateStruct) == -1)
		return -1;

	// CResizeBar implements in-place resizing.
	if (!m_wndResizeBar.Create(this)) {
		TRACE0("Failed to create resize bar\n");
		return -1;      // fail to create
	}

	// By default, it is a good idea to register a drop-target that does
	//  nothing with your frame window.  This prevents drops from
	//  "falling through" to a container that supports drag-drop.
	m_dropTarget.Register(this);

	return 0;
}

// OnCreateControlBars is called by the framework to create control bars on the
//  container application's windows.  pWndFrame is the top level frame window of
//  the container and is always non-NULL.  pWndDoc is the doc level frame window
//  and will be NULL when the container is an SDI application.  A server
//  application can place MFC control bars on either window.
BOOL	LedLineItInPlaceFrame::OnCreateControlBars (CFrameWnd* pWndFrame, CFrameWnd* /*pWndDoc*/)
{
	// Set owner to this window, so messages are delivered to correct app
	m_wndToolBar.SetOwner(this);

	// Create toolbar on client's frame window
	if (!m_wndToolBar.Create(pWndFrame) || !m_wndToolBar.LoadToolBar(IDR_SRVR_INPLACE)) {
		TRACE0("Failed to create toolbar\n");
		return FALSE;
	}

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	pWndFrame->EnableDocking(CBRS_ALIGN_ANY);
	pWndFrame->DockControlBar(&m_wndToolBar);

	return TRUE;
}

BOOL LedLineItInPlaceFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return COleIPFrameWnd::PreCreateWindow(cs);
}

#ifdef _DEBUG
void LedLineItInPlaceFrame::AssertValid() const
{
	COleIPFrameWnd::AssertValid();
}

void LedLineItInPlaceFrame::Dump(CDumpContext& dc) const
{
	COleIPFrameWnd::Dump(dc);
}
#endif //_DEBUG




// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
