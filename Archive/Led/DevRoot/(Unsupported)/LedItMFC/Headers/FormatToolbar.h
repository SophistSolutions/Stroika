/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */
#ifndef	__FormatToolbar_h__
#define	__FormatToolbar_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItMFC/Headers/FormatToolbar.h,v 1.4 1999/12/09 17:29:20 lewis Exp $
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
 *	Revision 1.4  1999/12/09 17:29:20  lewis
 *	lots of cleanups/fixes for new (partial/full) UNICODE support - SPR#0645
 *	
 *	Revision 1.3  1999/11/15 21:35:26  lewis
 *	react to some small Led changes so that Led itself no longer deends on MFC (though this demo app clearly DOES)
 *	
 *	Revision 1.2  1997/12/24 04:46:26  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.1  1997/12/24  04:07:11  lewis
 *	Initial revision
 *
 *
 *
 *	<<Based on MFC / MSDEVSTUDIO 5.0 Sample Wordpad - LGP 971212>>
 *
 *
 */
#include	<afxext.h>

#include	"LedGDI.h"

#include	"LedItConfig.h"



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

#endif	/*__FormatToolbar_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
