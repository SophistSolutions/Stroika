/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItMFC/Sources/LedItInPlaceFrame.cpp,v 2.8 1998/10/30 14:56:05 lewis Exp $
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
 *	Revision 2.8  1998/10/30 14:56:05  lewis
 *	mosstly new msvc60 pragma warnign changes
 *	
 *	Revision 2.7  1997/12/24  04:47:51  lewis
 *	*** empty log message ***
 *
 *	Revision 2.6  1997/07/27  16:01:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.5  1997/06/18  03:42:35  lewis
 *	qIncludePrefixFile and cleanups
 *
 *	Revision 2.4  1996/12/13  18:03:20  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.3  1996/12/05  21:18:00  lewis
 *	*** empty log message ***
 *
 *	Revision 2.2  1996/09/01  15:43:56  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.1  1996/06/01  02:46:04  lewis
 *	*** empty log message ***
 *
 *	Revision 2.0  1996/01/04  00:58:05  lewis
 *	*** empty log message ***
 *
 *	Revision 1.3  1996/01/04  00:57:44  lewis
 *	*** empty log message ***
 *
 *	Revision 1.2  1995/12/08  07:47:54  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	"Resource.h"

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
