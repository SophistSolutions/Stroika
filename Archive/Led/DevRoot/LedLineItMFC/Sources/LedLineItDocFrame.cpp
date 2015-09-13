/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItMFC/Sources/LedLineItDocFrame.cpp,v 2.7 2002/05/06 21:31:19 lewis Exp $
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
 *	$Log: LedLineItDocFrame.cpp,v $
 *	Revision 2.7  2002/05/06 21:31:19  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.6  2001/11/27 00:28:22  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.5  2001/08/30 01:02:00  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.4  1998/10/30 15:08:09  lewis
 *	lots of little cleanups - using vector instead of Led_Array, mutable instaed
 *	of const cast, new msvc60 warning
 *	stuff, etc.
 *	
 *	Revision 2.3  1997/12/24  04:51:19  lewis
 *	*** empty log message ***
 *
 *	Revision 2.2  1997/07/27  16:03:03  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.1  1997/07/23  23:20:42  lewis
 *	OnCreate - if first window being opened, then set style WS_MAXIMIZE. Bridges difference
 *	between SDI and MDI, and makes app work about as simply as SDI, but with power of MDI.
 *
 *	Revision 2.0  1997/06/28  17:38:34  lewis
 *	*** empty log message ***
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<stdio.h>

#include	"Resource.h"
#include	"LedLineItView.h"

#include	"LedLineItDocFrame.h"





/*
 ********************************************************************************
 ******************************** LedLineItDocFrame *****************************
 ********************************************************************************
 */
IMPLEMENT_DYNCREATE(LedLineItDocFrame, CMDIChildWnd)
BEGIN_MESSAGE_MAP(LedLineItDocFrame, LedLineItDocFrame::inherited)
	ON_WM_CREATE			()
END_MESSAGE_MAP()


int		LedLineItDocFrame::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
	Led_RequireNotNil (lpCreateStruct);

	if (inherited::OnCreate (lpCreateStruct) == -1) {
		return -1;
	}

	/*
	 *	When we are creating the first MDI child window, start out with it maximized. No point in
	 *	wasting all that space! I dunno why MDI doesn't do this by default?
	 */
	CMDIFrameWnd*	owningFrame	=	GetMDIFrame ();
	Led_RequireNotNil (owningFrame);
	if (owningFrame->MDIGetActive () == NULL) {
		::SetWindowLong (m_hWnd, GWL_STYLE, GetStyle () | WS_MAXIMIZE);
	}

	return 0;
}





// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
