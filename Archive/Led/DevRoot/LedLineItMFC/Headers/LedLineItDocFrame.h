/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LedLineItDocFrame_h__
#define	__LedLineItDocFrame_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItMFC/Headers/LedLineItDocFrame.h,v 2.6 2002/05/06 21:31:14 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedLineItDocFrame.h,v $
 *	Revision 2.6  2002/05/06 21:31:14  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.5  2001/11/27 00:28:19  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.4  2001/08/30 01:01:59  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.3  1997/12/24 04:50:02  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.2  1997/07/27  16:02:50  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.1  1997/07/23  23:18:26  lewis
 *	hook OnCreate() to set maximize flag sometimes
 *
 *	Revision 2.0  1997/06/28  17:33:33  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */

#include	<afxwin.h>
#include	<afxext.h>

#include	"LedSupport.h"

#include	"LedLineItConfig.h"


class	LedLineItDocFrame : public CMDIChildWnd {
	private:
		typedef	CMDIChildWnd	inherited;

	public:
		LedLineItDocFrame ();
	protected:
		DECLARE_DYNCREATE (LedLineItDocFrame)

	public:
		afx_msg	int		OnCreate (LPCREATESTRUCT lpCreateStruct);

	protected:
		DECLARE_MESSAGE_MAP()
};


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
	inline	LedLineItDocFrame::LedLineItDocFrame ():
		inherited ()
	{
	}



#endif	/*__LedLineItDocFrame_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

