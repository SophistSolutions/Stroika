/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LedLineItMainFrame_h__
#define	__LedLineItMainFrame_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItMFC/Headers/LedLineItMainFrame.h,v 2.9 2002/05/06 21:31:14 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedLineItMainFrame.h,v $
 *	Revision 2.9  2002/05/06 21:31:14  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.8  2001/11/27 00:28:20  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.7  2001/08/30 01:01:59  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.6  2000/06/08 20:32:15  lewis
 *	SPR#0771- close and closeall menu items in windows menu
 *	
 *	Revision 2.5  1997/12/24 04:50:02  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.4  1997/07/27  16:02:50  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.3  1997/06/28  17:33:50  lewis
 *	Add OnInitMenuPopup () override to delete disabled commands from context menu.
 *
 *	Revision 2.2  1997/03/23  01:07:19  lewis
 *	GotoEdit subclassed to handle tab/return key
 *
 *	Revision 2.1  1997/03/04  20:18:36  lewis
 *	goto line / status line support
 *
 *	Revision 2.0  1997/01/10  03:35:47  lewis
 *	*** empty log message ***
 *
 *	Revision 2.3  1996/12/13  18:02:41  lewis
 *
 *
 *	<========== BasedOnLedIt! 2.1 ==========>
 *
 *
 *
 *
 */

#include	<afxwin.h>
#include	<afxext.h>

#include	"LedSupport.h"

#include	"LedLineItConfig.h"


class	LedLineItMainFrame : public CMDIFrameWnd {
	private:
		typedef	CMDIFrameWnd	inherited;

	public:
		LedLineItMainFrame ();
	protected:
		DECLARE_DYNCREATE (LedLineItMainFrame)

	public:
		nonvirtual	bool	GetStatusBarShown () const;
		nonvirtual	void	SetStatusBarShown (bool shown);

	public:
		nonvirtual	void	TrackInGotoLineField ();

	public:
		class	StatusBar : public CStatusBar {
			private:
				typedef	CStatusBar	inherited;
			public:
				StatusBar ():
					CStatusBar (),
					fGotoEdit ()
					{
					}
			protected:
				afx_msg		void	OnLButtonDown (UINT nFlags, CPoint oPoint);
				afx_msg		void	OnMagicLoseFocus ();
				afx_msg		void	OnMagicEdited ();
			protected:
				nonvirtual	void	TrackInGotoLineField ();
		
			public:
				class	GotoEdit: public CEdit {
					public:
						afx_msg	UINT	OnGetDlgCode ();
						afx_msg	void	OnKeyDown (UINT nChar, UINT /*nRepCnt*/, UINT /*nFlags*/);
						DECLARE_MESSAGE_MAP()
				};
				GotoEdit	fGotoEdit;

			DECLARE_MESSAGE_MAP()
			friend	class	LedLineItMainFrame;
		};
		StatusBar	fStatusBar;
		CToolBar    fToolBar;

	protected:
		afx_msg		int		OnCreate (LPCREATESTRUCT lpCreateStruct);
		afx_msg		void	OnInitMenuPopup (CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);

	protected:
		afx_msg	void	OnUpdateLineIndicator (CCmdUI* pCmdUI);
		afx_msg	void	OnCloseWindowCommand ();
		afx_msg	void	OnCloseAllWindowsCommand ();

	protected:
		DECLARE_MESSAGE_MAP()
};


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
	inline	LedLineItMainFrame::LedLineItMainFrame ():
		inherited (),
		fStatusBar (),
		fToolBar ()
	{
	}



#endif	/*__LedLineItMainFrame_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

