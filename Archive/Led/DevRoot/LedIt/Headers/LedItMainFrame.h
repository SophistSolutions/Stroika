/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LedItMainFrame_h__
#define	__LedItMainFrame_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedIt/Headers/LedItMainFrame.h,v 1.5 2002/05/06 21:30:58 lewis Exp $
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
 *	Revision 1.5  2002/05/06 21:30:58  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 1.4  2001/11/27 00:28:09  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.3  2001/08/29 22:59:17  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.2  2001/07/31 15:45:11  lewis
 *	make format bar constructed on freestore instead of direct object to avoid passing in 'this'
 *	arg in ITS constructure before THIS is fully constructed (mostly innocuous compiler warning -
 *	but could someday cause trouble)
 *	
 *	Revision 1.1  2001/05/14 20:54:43  lewis
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
		~LedItMainFrame ();

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
		FormatToolbar*	fFormatBar;
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

