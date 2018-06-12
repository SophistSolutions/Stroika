/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#if WIN32
#if _MSC_VER && (qSilenceAnnoyingCompilerWarnings || !defined(qSilenceAnnoyingCompilerWarnings))
#pragma warning(4 : 4786)
#endif

#include <afxole.h>
#include <atlconv.h>
#endif

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/String.h"

//#include  "FontMenu.h"
#include "LedItApplication.h"
#include "LedItResources.h"

#include "FormatToolbar.h"

using namespace Stroika::Foundation;

#if qPlatform_Windows

using LocalComboBox = FormatToolbar::LocalComboBox;

/*
 ********************************************************************************
 ******************************** FormatToolbar *********************************
 ********************************************************************************
 */

BEGIN_MESSAGE_MAP (FormatToolbar, CToolBar)
ON_WM_CREATE ()
ON_WM_DESTROY ()
ON_CBN_DROPDOWN (IDC_FONTSIZE, OnFontSizeDropDown)
ON_CBN_KILLFOCUS (IDC_FONTNAME, OnFontNameKillFocus)
ON_CBN_KILLFOCUS (IDC_FONTSIZE, OnFontSizeKillFocus)
ON_CBN_SETFOCUS (IDC_FONTNAME, OnComboSetFocus)
ON_CBN_SETFOCUS (IDC_FONTSIZE, OnComboSetFocus)
ON_CBN_CLOSEUP (IDC_FONTNAME, OnComboCloseUp)
ON_CBN_CLOSEUP (IDC_FONTSIZE, OnComboCloseUp)
END_MESSAGE_MAP ()

static CSize GetBaseUnits (CFont* pFont)
{
    ASSERT (pFont != NULL);
    ASSERT (pFont->GetSafeHandle () != NULL);
    CWindowDC screenDC (NULL);
    pFont = screenDC.SelectObject (pFont);
    TEXTMETRIC tm;
    VERIFY (screenDC.GetTextMetrics (&tm));
    screenDC.SelectObject (pFont);
    //  return CSize(tm.tmAveCharWidth, tm.tmHeight+tm.tmDescent);
    return CSize (tm.tmAveCharWidth, tm.tmHeight);
}

FormatToolbar::FormatToolbar (FormatToolbarOwner& owner)
    : fOwner (owner)
{
    CFont fnt;
    fnt.Attach (GetStockObject (DEFAULT_GUI_FONT));
    m_szBaseUnits                = GetBaseUnits (&fnt);
    LocalComboBox::m_nFontHeight = m_szBaseUnits.cy;
}

void FormatToolbar::OnUpdateCmdUI (CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
    CToolBar::OnUpdateCmdUI (pTarget, bDisableIfNoHndler);
    // don't update combo boxes if either one has the focus
    if (!m_comboFontName.HasFocus () && !m_comboFontSize.HasFocus ())
        SyncToView ();
}

void FormatToolbar::SyncToView ()
{
    Led_IncrementalFontSpecification fsp = fOwner.GetCurFont ();
    if (fsp.GetFontNameSpecifier_Valid ()) {
        m_comboFontName.MatchFont (fsp.GetFontName ().c_str ());
    }
    else {
        m_comboFontName.SetTheText (_T(""));
    }

    // SetTwipSize only updates if different
    // -1 means selection is not a single point size
    m_comboFontSize.SetTwipSize (fsp.GetPointSize_Valid () ? fsp.GetPointSize () * 20 : -1);
}

void FormatToolbar::OnFontSizeDropDown ()
{
    CString str;
    m_comboFontName.GetTheText (str);
    CString lpszName;
    int     nIndex = m_comboFontName.FindStringExact (-1, str);
    if (nIndex != CB_ERR) {
        m_comboFontName.GetLBText (nIndex, lpszName);
    }

    int nSize = m_comboFontSize.GetTwipSize ();
    if (nSize == -2) { // error
#if 0
        AfxMessageBox(IDS_INVALID_NUMBER, MB_OK | MB_ICONINFORMATION);
#endif
        nSize = m_comboFontSize.m_nTwipsLast;
    }
    else if ((nSize >= 0 && nSize < 20) || nSize > 32760) {
#if 0
        AfxMessageBox(IDS_INVALID_FONTSIZE, MB_OK | MB_ICONINFORMATION);
#endif
        nSize = m_comboFontSize.m_nTwipsLast;
    }

    CWindowDC screenDC (NULL);
    m_comboFontSize.EnumFontSizes (screenDC, (lpszName.GetLength () == 0) ? NULL : LPCTSTR (lpszName));
    m_comboFontSize.SetTwipSize (nSize);
}

void FormatToolbar::OnComboCloseUp ()
{
    NotifyOwner (NM_RETURN);
}

void FormatToolbar::OnComboSetFocus ()
{
    NotifyOwner (NM_SETFOCUS);
}

void FormatToolbar::OnFontNameKillFocus ()
{
    USES_CONVERSION;
    // get the current font from the view and update
    NotifyOwner (NM_KILLFOCUS);
    // this will retrieve the font entered in the edit control
    // it tries to match the font to something already present in the combo box
    // this effectively ignores case of a font the user enters
    // if a user enters arial, this will cause it to become Arial
    CString str;
    m_comboFontName.GetTheText (str); // returns "arial"
    m_comboFontName.SetTheText (str); // selects "Arial"
    m_comboFontName.GetTheText (str); // returns "Arial"

    // if font name box is not empty
    if (not str.IsEmpty ()) {
        Led_IncrementalFontSpecification fsp;
        int                              nIndex = m_comboFontName.FindStringExact (-1, str);
        if (nIndex != CB_ERR) {
            CString name;
            m_comboFontName.GetLBText (nIndex, name);
            fsp.SetFontName ((LPCTSTR)name);
        }
        else {
            // unknown font
            //      fsp.SetFontName (T2A((LPTSTR) (LPCTSTR)str));
            fsp.SetFontName ((LPCTSTR)str);
        }
        fOwner.SetCurFont (fsp);
    }
}

void FormatToolbar::OnFontSizeKillFocus ()
{
    NotifyOwner (NM_KILLFOCUS);
    int nSize = m_comboFontSize.GetTwipSize ();
    if (nSize == -2) {
#if 0
        AfxMessageBox(IDS_INVALID_NUMBER, MB_OK | MB_ICONINFORMATION);
#endif
        nSize = m_comboFontSize.m_nTwipsLast;
    }
    else if ((nSize >= 0 && nSize < 20) || nSize > 32760) {
#if 0
        AfxMessageBox(IDS_INVALID_FONTSIZE, MB_OK | MB_ICONINFORMATION);
#endif
        nSize = m_comboFontSize.m_nTwipsLast;
    }
    else if (nSize > 0) {
#if 0
        CCharFormat cf;
        cf.dwMask = CFM_SIZE;
        cf.yHeight = nSize;
        SetCharFormat(cf);
#endif
        Led_IncrementalFontSpecification fsp;
        fsp.SetPointSize (static_cast<Led_IncrementalFontSpecification::FontSize> (nSize / 20));
        fOwner.SetCurFont (fsp);
    }
}

int FormatToolbar::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
    if (CToolBar::OnCreate (lpCreateStruct) == -1)
        return -1;

    CRect rect (0, 0, (3 * LF_FACESIZE * m_szBaseUnits.cx) / 2, 200);
    if (!m_comboFontName.Create (WS_TABSTOP | WS_VISIBLE | WS_TABSTOP |
                                     WS_VSCROLL | CBS_DROPDOWN | CBS_SORT | CBS_AUTOHSCROLL | CBS_HASSTRINGS |
                                     CBS_OWNERDRAWFIXED,
                                 rect, this, IDC_FONTNAME)) {
        TRACE0 ("Failed to create fontname combo-box\n");
        return -1;
    }
    m_comboFontName.LimitText (LF_FACESIZE);

    rect.SetRect (0, 0, 10 * m_szBaseUnits.cx, 200);
    if (!m_comboFontSize.Create (WS_TABSTOP | WS_VISIBLE | WS_TABSTOP |
                                     WS_VSCROLL | CBS_DROPDOWN,
                                 rect, this, IDC_FONTSIZE)) {
        TRACE0 ("Failed to create fontsize combo-box\n");
        return -1;
    }

    m_comboFontSize.LimitText (4);
    m_comboFontName.EnumFontFamiliesEx ();

    static UINT BASED_CODE format[] = {
        // same order as in the bitmap 'format.bmp'
        ID_SEPARATOR, // font name combo box
        ID_SEPARATOR,
        ID_SEPARATOR, // font size combo box
        ID_SEPARATOR,
        kFontStyleBoldCmd,
        kFontStyleItalicCmd,
        kFontStyleUnderlineCmd,
        ID_CHAR_COLOR,
        ID_SEPARATOR,
        kJustifyLeftCmd,
        kJustifyCenterCmd,
        kJustifyRightCmd,
    };

    if (!LoadBitmap (IDB_FORMATBAR) || !SetButtons (format, sizeof (format) / sizeof (UINT))) {
        TRACE0 ("Failed to create FormatBar\n");
        return -1; // fail to create
    }
    SetBarStyle (GetBarStyle () | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | CBRS_HIDE_INPLACE);
    SetSizes (CSize (23, 22), CSize (16, 16));
    CString str;
    str.LoadString (IDS_TITLE_FORMATBAR);
    SetWindowText (str);
    PositionCombos ();

    return 0;
}

void FormatToolbar::PositionCombos ()
{
    CRect rect;
    // make font name box same size as font size box
    // this is necessary since font name box is owner draw
    m_comboFontName.SetItemHeight (-1, m_comboFontSize.GetItemHeight (-1));

    m_comboFontName.GetWindowRect (&rect);
    int nHeight = rect.Height ();

    m_comboFontName.GetWindowRect (&rect);
    SetButtonInfo (0, IDC_FONTNAME, TBBS_SEPARATOR, rect.Width ());
    GetItemRect (0, &rect); // FontName ComboBox
    m_comboFontName.SetWindowPos (NULL, rect.left,
                                  ((rect.Height () - nHeight) / 2) + rect.top, 0, 0,
                                  SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);

    m_comboFontSize.GetWindowRect (&rect);
    SetButtonInfo (2, IDC_FONTSIZE, TBBS_SEPARATOR, rect.Width ());
    GetItemRect (2, &rect); // FontSize ComboBox
    m_comboFontSize.SetWindowPos (NULL, rect.left,
                                  ((rect.Height () - nHeight) / 2) + rect.top, 0, 0,
                                  SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

void FormatToolbar::NotifyOwner (UINT nCode)
{
    NMHDR nm;
    nm.hwndFrom = m_hWnd;
    nm.idFrom   = GetDlgCtrlID ();
    nm.code     = nCode;
    GetOwner ()->SendMessage (WM_NOTIFY, nm.idFrom, (LPARAM)&nm);
}

/*
 ********************************************************************************
 ******************************** FontComboBox **********************************
 ********************************************************************************
 */
using FontComboBox = FormatToolbar::FontComboBox;

const int BMW = 16;
const int BMH = 15;

BEGIN_MESSAGE_MAP (FontComboBox, LocalComboBox)
END_MESSAGE_MAP ()

FontComboBox::FontComboBox ()
{
    VERIFY (m_bmFontType.LoadBitmap (IDB_FONTTYPE));
}

void FontComboBox::EnumFontFamiliesEx ()
{
    CString str;
    GetTheText (str);
    ResetContent ();
    vector<Led_SDK_String> fonts = LedItApplication::Get ().GetUsableFontNames ();
    for (vector<Led_SDK_String>::const_iterator i = fonts.begin (); i != fonts.end (); ++i) {
        AddString ((*i).c_str ());
    }
    SetTheText (str);
}

BOOL CALLBACK AFX_EXPORT FontComboBox::CheckIsTrueTypeEnumFamCallback (ENUMLOGFONTEX* /*pelf*/, NEWTEXTMETRICEX* /*lpntm*/, int FontType, LPVOID pThis)
{
    bool* bs = (bool*)pThis;
    if (FontType & TRUETYPE_FONTTYPE) {
        *bs = true;
    }
    return 1;
}

void FontComboBox::DrawItem (LPDRAWITEMSTRUCT lpDIS)
{
    ASSERT (lpDIS->CtlType == ODT_COMBOBOX);
    int id = (int)(WORD)lpDIS->itemID;

    CDC*  pDC = CDC::FromHandle (lpDIS->hDC);
    CRect rc (lpDIS->rcItem);
    if (lpDIS->itemState & ODS_FOCUS)
        pDC->DrawFocusRect (rc);
    int nIndexDC = pDC->SaveDC ();

    CBrush brushFill;
    if (lpDIS->itemState & ODS_SELECTED) {
        brushFill.CreateSolidBrush (::GetSysColor (COLOR_HIGHLIGHT));
        pDC->SetTextColor (::GetSysColor (COLOR_HIGHLIGHTTEXT));
    }
    else
        brushFill.CreateSolidBrush (pDC->GetBkColor ());
    pDC->SetBkMode (TRANSPARENT);
    pDC->FillRect (rc, &brushFill);

    CString strText;
    GetLBText (id, strText);

    bool isTrueType = false;
    {
        LOGFONT lf;
        memset (&lf, 0, sizeof (LOGFONT));
        Characters::CString::Copy (lf.lfFaceName, NEltsOf (lf.lfFaceName), static_cast<const TCHAR*> (strText));
        CWindowDC screenDC (NULL);
        HDC       hDC = screenDC.m_hDC;
        ASSERT (hDC != NULL);
        ::EnumFontFamiliesEx (hDC, &lf, (FONTENUMPROC)CheckIsTrueTypeEnumFamCallback, (LPARAM)&isTrueType, NULL);
    }
    if (isTrueType) {
        CDC dc;
        dc.CreateCompatibleDC (pDC);
        CBitmap* pBitmap = dc.SelectObject (&m_bmFontType);
        pDC->BitBlt (rc.left, rc.top, BMW, BMH, &dc, BMW, 0, SRCAND);
        dc.SelectObject (pBitmap);
    }

    rc.left += BMW + 6;
    pDC->TextOut (rc.left, rc.top, strText, strText.GetLength ());

    pDC->RestoreDC (nIndexDC);
}

void FontComboBox::MeasureItem (LPMEASUREITEMSTRUCT lpMIS)
{
    ASSERT (lpMIS->CtlType == ODT_COMBOBOX);
    ASSERT (m_nFontHeight > 0);
    CRect rc;

    GetWindowRect (&rc);
    lpMIS->itemWidth  = rc.Width ();
    lpMIS->itemHeight = max (BMH, m_nFontHeight);
}

int FontComboBox::CompareItem (LPCOMPAREITEMSTRUCT lpCIS)
{
    ASSERT (lpCIS->CtlType == ODT_COMBOBOX);
    int     id1 = (int)(WORD)lpCIS->itemID1;
    int     id2 = (int)(WORD)lpCIS->itemID2;
    CString str1, str2;
    if (id1 == -1)
        return -1;
    if (id2 == -1)
        return 1;
    GetLBText (id1, str1);
    GetLBText (id2, str2);
    return str1.Collate (str2);
}

// find a font with the face name and charset
void FontComboBox::MatchFont (LPCTSTR lpszName)
{
    int nFirstIndex = FindString (-1, lpszName);
    if (nFirstIndex != CB_ERR) {
        int nIndex = nFirstIndex;
        do {
            CString str;
            GetLBText (nIndex, str);
            if (lstrcmp (lpszName, str) == 0) {
                //got a match
                if (GetCurSel () != nIndex)
                    SetCurSel (nIndex);
                return;
            }
            nIndex = FindString (nIndex, lpszName);
        } while (nIndex != nFirstIndex);
        // loop until found or back to first item again
    }
    //enter font name
    SetTheText (lpszName, TRUE);
}

/*
 ********************************************************************************
 ******************************** SizeComboBox **********************************
 ********************************************************************************
 */
using SizeComboBox = FormatToolbar::SizeComboBox;

SizeComboBox::SizeComboBox ()
{
    m_nTwipsLast = 0;
}

void SizeComboBox::EnumFontSizes (CDC& dc, LPCTSTR pFontName)
{
    ResetContent ();
    if (pFontName == NULL)
        return;
    if (pFontName[0] == NULL)
        return;

    ASSERT (dc.m_hDC != NULL);
    m_nLogVert = dc.GetDeviceCaps (LOGPIXELSY);
    ::EnumFontFamilies (dc.m_hDC, pFontName, (FONTENUMPROC)EnumSizeCallBack, (LPARAM)this);
}

void SizeComboBox::TwipsToPointString (LPTSTR lpszBuf, int nTwips)
{
    ASSERT (lpszBuf != NULL);
    lpszBuf[0] = NULL;
    if (nTwips >= 0) {
        // round to nearest half point
        nTwips = (nTwips + 5) / 10;
        DISABLE_COMPILER_MSC_WARNING_START (4996)
        if ((nTwips % 2) == 0)
            _stprintf (lpszBuf, _T("%ld"), nTwips / 2);
        else
            _stprintf (lpszBuf, _T("%.1f"), (float)nTwips / 2.F);
        DISABLE_COMPILER_MSC_WARNING_END (4996)
    }
}

void SizeComboBox::SetTwipSize (int nTwips)
{
    if (nTwips != GetTwipSize ()) {
        TCHAR buf[10];
        TwipsToPointString (buf, nTwips);
        SetTheText (buf, TRUE);
    }
    m_nTwipsLast = nTwips;
}

int SizeComboBox::GetTwipSize ()
{
    // return values
    // -2 -- error
    // -1 -- edit box empty
    // >=0 -- font size in twips
    CString str;
    GetTheText (str);
    LPCTSTR lpszText = str;

    while (*lpszText == ' ' || *lpszText == '\t')
        lpszText++;

    if (lpszText[0] == NULL)
        return -1; // no text in control

    double d = _tcstod (lpszText, (LPTSTR*)&lpszText);
    while (*lpszText == ' ' || *lpszText == '\t')
        lpszText++;

    if (*lpszText != NULL)
        return -2; // not terminated properly

    return (d < 0.) ? 0 : (int)(d * 20.);
}

BOOL CALLBACK AFX_EXPORT SizeComboBox::EnumSizeCallBack (LOGFONT FAR* /*lplf*/, LPNEWTEXTMETRIC lpntm, int FontType, LPVOID lpv)
{
    static int nFontSizes[] =
        {8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48, 72};

    SizeComboBox* pThis = (SizeComboBox*)lpv;
    ASSERT (pThis != NULL);
    TCHAR buf[10];
    if (
        (FontType & TRUETYPE_FONTTYPE) ||
        !((FontType & TRUETYPE_FONTTYPE) || (FontType & RASTER_FONTTYPE))) { // if truetype or vector font
        // this occurs when there is a truetype and nontruetype version of a font
        if (pThis->GetCount () != 0)
            pThis->ResetContent ();

        for (int i = 0; i < 16; i++) {
            wsprintf (buf, _T("%d"), nFontSizes[i]);
            pThis->AddString (buf);
        }
        return FALSE; // don't call me again
    }
    // calc character height in pixels
    pThis->InsertSize (MulDiv (lpntm->tmHeight - lpntm->tmInternalLeading, 1440, pThis->m_nLogVert));
    return TRUE; // call me again
}

void SizeComboBox::InsertSize (int nSize)
{
    ASSERT (nSize > 0);
    DWORD dwSize = (DWORD)nSize;
    TCHAR buf[10];
    TwipsToPointString (buf, nSize);
    if (FindStringExact (-1, buf) == CB_ERR) {
        int   nIndex = -1;
        int   nPos   = 0;
        DWORD dw;
        while ((dw = GetItemData (nPos)) != CB_ERR) {
            if (dw > dwSize) {
                nIndex = nPos;
                break;
            }
            nPos++;
        }
        nIndex = InsertString (nIndex, buf);
        ASSERT (nIndex != CB_ERR);
        if (nIndex != CB_ERR)
            SetItemData (nIndex, dwSize);
    }
}

/*
 ********************************************************************************
 ******************************** LocalComboBox *********************************
 ********************************************************************************
 */
using LocalComboBox = FormatToolbar::LocalComboBox;

int LocalComboBox::m_nFontHeight = 0;

BEGIN_MESSAGE_MAP (LocalComboBox, CComboBox)
ON_WM_CREATE ()
END_MESSAGE_MAP ()

void LocalComboBox::GetTheText (CString& str)
{
    int nIndex = GetCurSel ();
    if (nIndex == CB_ERR)
        GetWindowText (str);
    else
        GetLBText (nIndex, str);
}

void LocalComboBox::SetTheText (LPCTSTR lpszText, BOOL bMatchExact)
{
    int idx = (bMatchExact) ? FindStringExact (-1, lpszText) : FindString (-1, lpszText);
    SetCurSel ((idx == CB_ERR) ? -1 : idx);
    if (idx == CB_ERR)
        SetWindowText (lpszText);
}

BOOL LocalComboBox::LimitText (int nMaxChars)
{
    BOOL b = CComboBox::LimitText (nMaxChars);
    if (b)
        m_nLimitText = nMaxChars;
    return b;
}

int LocalComboBox::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
    if (CComboBox::OnCreate (lpCreateStruct) == -1)
        return -1;
    SendMessage (WM_SETFONT, (WPARAM)GetStockObject (DEFAULT_GUI_FONT));
    return 0;
}

BOOL LocalComboBox::PreTranslateMessage (MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN) {
        FormatToolbar* pBar = (FormatToolbar*)GetParent ();
        switch (pMsg->wParam) {
            case VK_ESCAPE:
                pBar->SyncToView ();
                pBar->NotifyOwner (NM_RETURN);
                return TRUE;
            case VK_RETURN:
                pBar->NotifyOwner (NM_RETURN);
                return TRUE;
            case VK_TAB:
                pBar->GetNextDlgTabItem (this)->SetFocus ();
                return TRUE;
            case VK_UP:
            case VK_DOWN:
                if ((GetKeyState (VK_MENU) >= 0) && (GetKeyState (VK_CONTROL) >= 0) &&
                    !GetDroppedState ()) {
                    ShowDropDown ();
                    return TRUE;
                }
        }
    }
    return CComboBox::PreTranslateMessage (pMsg);
}
#endif
