/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItMFC/Sources/ColorMenu.cpp,v 1.4 1998/10/30 14:56:05 lewis Exp $
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
 *	$Log: ColorMenu.cpp,v $
 *	Revision 1.4  1998/10/30 14:56:05  lewis
 *	mosstly new msvc60 pragma warnign changes
 *	
 *	Revision 1.3  1998/05/05  00:33:44  lewis
 *	*** empty log message ***
 *
 *	Revision 1.2  1997/12/24  04:47:51  lewis
 *	*** empty log message ***
 *
 *	Revision 1.1  1997/12/24  04:17:04  lewis
 *	Initial revision
 *
 *
 *
 *	<<Based on MFC / MSDEVSTUDIO 5.0 Sample Wordpad - LGP 971212>>
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	"Resource.h"

#include	"ColorMenu.h"





/*
 ********************************************************************************
 *********************************** ColorMenu **********************************
 ********************************************************************************
 */

ColorMenu::ColorMenu ()
{
	VERIFY (CreatePopupMenu());
	ASSERT (GetMenuItemCount()==0);
	for (int i=0; i<=15;i++) {
		VERIFY(AppendMenu (MF_OWNERDRAW, ID_COLOR0+i, (LPCTSTR)(ID_COLOR0+i)));
	}

// ADD:
//			MenuItem	SEPARATOR
//		MenuItem	"Other...",				kFontSizeOtherCmdID


}

COLORREF	ColorMenu::GetColor (UINT id)
{
	const	int  kIndexMap[16] = {
		0, 		//black
		1, 		//dark red
		2, 		//dark green
		3, 		//light brown
		4, 		//dark blue
		5, 		//purple
		6, 		//dark cyan
		12, 	//gray
		7, 		//light gray
		13, 	//red
		14, 	//green
		15, 	//yellow
		16, 	//blue
		17, 	//magenta
		18, 	//cyan
		19, 	//white
	};
	ASSERT(id >= ID_COLOR0);
	ASSERT(id <= ID_COLOR15);
	CPalette* pPal = CPalette::FromHandle( (HPALETTE) GetStockObject(DEFAULT_PALETTE));
	ASSERT(pPal != NULL);
	PALETTEENTRY pe;
	if (pPal->GetPaletteEntries(kIndexMap[id-ID_COLOR0], 1, &pe) == 0)
		return ::GetSysColor (COLOR_WINDOWTEXT);
	else
		return RGB (pe.peRed,pe.peGreen,pe.peBlue);
}

Led_Color	ColorMenu::FontCmdToColor (UINT cmd)
{
	return Led_Color (GetColor (cmd));
}

UINT		ColorMenu::FontColorToCmd (Led_Color color)
{
	for (UINT i = ID_COLOR0; i <= ID_COLOR15; ++i) {
		if (FontCmdToColor (i) == color) {
			return i;
		}
	}
	return kFontColorOtherCmdID;
}

void	ColorMenu::DrawItem (LPDRAWITEMSTRUCT lpDIS)
{
	ASSERT(lpDIS->CtlType == ODT_MENU);
	UINT id = (UINT)(WORD)lpDIS->itemID;
	ASSERT(id == lpDIS->itemData);
	ASSERT(id >= ID_COLOR0);
	ASSERT(id <= ID_COLOR15);
	CDC dc;
	dc.Attach(lpDIS->hDC);

	CRect rc(lpDIS->rcItem);
	ASSERT(rc.Width() < 500);
	if (lpDIS->itemState & ODS_FOCUS)
		dc.DrawFocusRect(&rc);

	COLORREF cr = (lpDIS->itemState & ODS_SELECTED) ? 
		::GetSysColor(COLOR_HIGHLIGHT) :
		dc.GetBkColor();

	CBrush brushFill(cr);
	cr = dc.GetTextColor();

	if (lpDIS->itemState & ODS_SELECTED)
		dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));

	int nBkMode = dc.SetBkMode(TRANSPARENT);
	dc.FillRect(&rc, &brushFill);

	rc.left += 50;
	CString strColor;
	strColor.LoadString(id);
	dc.TextOut(rc.left,rc.top,strColor,strColor.GetLength());
	rc.left -= 45;
	rc.top += 2;
	rc.bottom -= 2;
	rc.right = rc.left + 40;
	CBrush brush(GetColor(id));
	CBrush* pOldBrush = dc.SelectObject(&brush);
	dc.Rectangle(rc);

	dc.SelectObject(pOldBrush);
	dc.SetTextColor(cr);
	dc.SetBkMode(nBkMode);
	
	dc.Detach();
}

void	ColorMenu::MeasureItem (LPMEASUREITEMSTRUCT lpMIS)
{
	ASSERT(lpMIS->CtlType == ODT_MENU);
	UINT id = (UINT)(WORD)lpMIS->itemID;
	ASSERT(id == lpMIS->itemData);
	ASSERT(id >= ID_COLOR0);
	ASSERT(id <= ID_COLOR15);
	class CDisplayIC : public CDC
	{
		public:
			CDisplayIC() { CreateIC(_T("DISPLAY"), NULL, NULL, NULL); }
	};
	CDisplayIC dc;
	CString strColor;
	strColor.LoadString(id);
	CSize sizeText = dc.GetTextExtent(strColor,strColor.GetLength());
	ASSERT(sizeText.cx < 500);
	lpMIS->itemWidth = sizeText.cx + 50;
	lpMIS->itemHeight = sizeText.cy;
}


// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
