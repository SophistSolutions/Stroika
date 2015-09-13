/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__FormatToolbar_h__
#define	__FormatToolbar_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedIt/Headers/FormatToolbar.h,v 1.4 2002/05/06 21:30:57 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: FormatToolbar.h,v $
 *	Revision 1.4  2002/05/06 21:30:57  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 1.3  2001/11/27 00:28:08  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.2  2001/08/29 22:59:16  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.1  2001/05/14 20:54:42  lewis
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
#if		qWindows
#include	<afxext.h>
#endif

#include	"LedGDI.h"

#include	"LedItConfig.h"



#if		qWindows
// abstract callback API
class	FormatToolbarOwner {
	public:
		virtual	Led_IncrementalFontSpecification	GetCurFont () const				=	0;
		virtual	void	SetCurFont (const Led_IncrementalFontSpecification& fsp)	=	0;
};

class	FormatToolbar : public CToolBar {
	public:
		FormatToolbar (FormatToolbarOwner& owner);
	private:
		FormatToolbarOwner&	fOwner;

	public:
		nonvirtual	void	PositionCombos ();
		nonvirtual	void	SyncToView ();

	public:
		class LocalComboBox : public CComboBox {
			public:
				static int m_nFontHeight;
				int			m_nLimitText;
				nonvirtual	BOOL HasFocus()
				{
					HWND hWnd = ::GetFocus();
					return (hWnd == m_hWnd || ::IsChild(m_hWnd, hWnd));
				}
				nonvirtual	void GetTheText(CString& str);
				nonvirtual	void SetTheText(LPCTSTR lpszText,BOOL bMatchExact = FALSE);

				nonvirtual	BOOL LimitText(int nMaxChars);
			public:
				virtual BOOL PreTranslateMessage(MSG* pMsg);
				afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
				DECLARE_MESSAGE_MAP()
		};

		class	FontComboBox : public LocalComboBox {
			public:
				FontComboBox ();

				CBitmap m_bmFontType;

				nonvirtual	void EnumFontFamiliesEx ();
				nonvirtual	void MatchFont (LPCTSTR lpszName);

				static	BOOL	CALLBACK	AFX_EXPORT	CheckIsTrueTypeEnumFamCallback (ENUMLOGFONTEX* pelf, NEWTEXTMETRICEX* /*lpntm*/, int FontType, LPVOID pThis);

				override	void	DrawItem (LPDRAWITEMSTRUCT lpDIS);
				override	void	MeasureItem (LPMEASUREITEMSTRUCT lpMIS);
				override	int		CompareItem (LPCOMPAREITEMSTRUCT lpCIS);
				DECLARE_MESSAGE_MAP()
		};

		class	SizeComboBox : public LocalComboBox {
			public:
				SizeComboBox ();
				int m_nLogVert;
				int m_nTwipsLast;
			public:
				nonvirtual	void	EnumFontSizes(CDC& dc, LPCTSTR pFontName);
				static BOOL FAR PASCAL EnumSizeCallBack(LOGFONT FAR* lplf, LPNEWTEXTMETRIC lpntm,int FontType, LPVOID lpv);
				nonvirtual	void	TwipsToPointString(LPTSTR lpszBuf, int nTwips);
				nonvirtual	void	SetTwipSize(int nSize);
				nonvirtual	int		GetTwipSize();
				nonvirtual	void	InsertSize(int nSize);
		};

	public:
		CSize			m_szBaseUnits;
		FontComboBox	m_comboFontName;
		SizeComboBox	m_comboFontSize;

	public:
		void NotifyOwner (UINT nCode);

	protected:
		override	void	OnUpdateCmdUI (CFrameWnd* pTarget, BOOL bDisableIfNoHndler);

		afx_msg	int		OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void	OnFontNameKillFocus();
		afx_msg void	OnFontSizeKillFocus();
		afx_msg void	OnFontSizeDropDown();
		afx_msg	void	OnComboCloseUp();
		afx_msg	void	OnComboSetFocus();
		DECLARE_MESSAGE_MAP()
};
#endif


#endif	/*__FormatToolbar_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
