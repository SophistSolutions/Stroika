/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/LedIt/Sources/LedItInPlaceFrame.cpp,v 1.6 2003/03/21 15:00:51 lewis Exp $
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
 *	$Log: LedItInPlaceFrame.cpp,v $
 *	Revision 1.6  2003/03/21 15:00:51  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.5  2002/10/24 15:53:15  lewis
 *	SPR#0680- adjust the window margins so text looks a little better. Did for MacOS/XWin/Win -
 *	but only tested on Win so far
 *	
 *	Revision 1.4  2002/05/06 21:31:06  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 1.3  2001/11/27 00:28:15  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.2  2001/08/29 22:59:19  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.1  2001/05/14 20:54:47  lewis
 *	New LedIt! CrossPlatform app - based on merging LedItPP and LedItMFC and parts of LedTextXWindows
 *	
 *
 *
 *
 *	<<<***		Based on LedItMFC project from Led 3.0b6		***>>>
 *
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#if		defined (WIN32)
	#include	<afxwin.h>
#endif

#include	"LedItResources.h"
#include	"LedItInPlaceFrame.h"






/*
 ********************************************************************************
 ******************************* LedItInPlaceFrame ******************************
 ********************************************************************************
 */
IMPLEMENT_DYNCREATE(LedItInPlaceFrame, COleIPFrameWnd)

BEGIN_MESSAGE_MAP(LedItInPlaceFrame, COleIPFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_HELP_FINDER, OnHelpFinder)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND(ID_DEFAULT_HELP, OnHelpFinder)
	ON_COMMAND(ID_CONTEXT_HELP, OnContextHelp)
END_MESSAGE_MAP()

LedItInPlaceFrame::LedItInPlaceFrame ():
	COleIPFrameWnd ()
{
}

LedItInPlaceFrame::~LedItInPlaceFrame ()
{
}

int LedItInPlaceFrame::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
	if (COleIPFrameWnd::OnCreate (lpCreateStruct) == -1)
		return -1;

	// CResizeBar implements in-place resizing.
	if (!m_wndResizeBar.Create(this))
	{
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
BOOL	LedItInPlaceFrame::OnCreateControlBars (CFrameWnd* pWndFrame, CFrameWnd* /*pWndDoc*/)
{
	// Set owner to this window, so messages are delivered to correct app
	m_wndToolBar.SetOwner(this);

	// Create toolbar on client's frame window
	if (!m_wndToolBar.Create(pWndFrame) ||
		!m_wndToolBar.LoadToolBar(IDR_SRVR_INPLACE))
	{
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

BOOL LedItInPlaceFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return COleIPFrameWnd::PreCreateWindow(cs);
}

#ifdef _DEBUG
void LedItInPlaceFrame::AssertValid() const
{
	COleIPFrameWnd::AssertValid();
}

void LedItInPlaceFrame::Dump(CDumpContext& dc) const
{
	COleIPFrameWnd::Dump(dc);
}
#endif //_DEBUG




// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
