/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */
#ifndef	__LedItMainFrame_h__
#define	__LedItMainFrame_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItMFC/Headers/LedItMainFrame.h,v 2.8 1997/12/24 04:46:26 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedItMainFrame.h,v $
 *	Revision 2.8  1997/12/24 04:46:26  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.7  1997/12/24  04:06:08  lewis
 *	Added RulerBar, and FormatToolbar. On close, save toobar state.
 *
 *	Revision 2.6  1997/07/27  16:01:09  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.5  1997/06/28  17:27:16  lewis
 *	Hook OnInitMenuPopup to delete disabled popup menu items.
 *
 *	Revision 2.4  1997/01/10  03:28:52  lewis
 *	cleanups
 *
 *	Revision 2.3  1996/12/13  18:02:41  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.2  1996/09/01  15:43:38  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.1  1996/06/01  02:34:49  lewis
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

#include	<afxwin.h>
#include	<afxext.h>

#include	"LedSupport.h"

#include	"FormatToolbar.h"
#include	"LedItConfig.h"
#include	"RulerToolbar.h"


class	LedItMainFrame : public CFrameWnd, private FormatToolbarOwner {
	private:
		typedef	CFrameWnd	inherited;

	protected: // create from serialization only
		LedItMainFrame ();
		DECLARE_DYNCREATE(LedItMainFrame)

	public:
		override	BOOL	PreCreateWindow (CREATESTRUCT& cs);

	public:
		override	Led_IncrementalFontSpecification	GetCurFont () const;
		override	void								SetCurFont (const Led_IncrementalFontSpecification& fsp);

	protected:
		afx_msg	void	OnCharColor ();

	private:
		CStatusBar		fStatusBar;
		CToolBar		fToolBar;
		FormatToolbar	fFormatBar;
		RulerBar		fRulerBar;

	protected:
		afx_msg		int		OnCreate (LPCREATESTRUCT lpCreateStruct);
		afx_msg		void	OnClose ();
		afx_msg		void	OnInitMenuPopup (CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
		DECLARE_MESSAGE_MAP()

	#ifdef _DEBUG
	public:
		virtual void AssertValid() const;
		virtual void Dump(CDumpContext& dc) const;
	#endif
};

#endif	/*__LedItMainFrame_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

