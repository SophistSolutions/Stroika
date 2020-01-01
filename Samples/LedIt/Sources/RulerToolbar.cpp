/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#include <afxpriv.h> //  For WM_SIZEPARENT
#include <afxwin.h>  //  MFC core and standard components

#include "LedItResources.h"
#include "LedItView.h"

#include "RulerToolbar.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::Platform;
using namespace Stroika::Frameworks::Led::StyledTextIO;

using RulerItem       = RulerBar::RulerItem;
using CComboRulerItem = RulerBar::CComboRulerItem;
using CTabRulerItem   = RulerBar::CTabRulerItem;

using IncrementalParagraphInfo = WordProcessor::IncrementalParagraphInfo;

#define HEIGHT 17
#define RULERBARHEIGHT 17

RulerItem::RulerItem (UINT nBitmapID)
{
    m_nAlignment = TA_CENTER;
    m_pDC        = NULL;
    m_bTrack     = FALSE;
    m_hbm        = NULL;
    m_hbmMask    = NULL;
    if (nBitmapID != 0) {
        m_hbmMask = ::LoadBitmap (
            AfxFindResourceHandle (MAKEINTRESOURCE (nBitmapID + 1), RT_BITMAP),
            MAKEINTRESOURCE (nBitmapID + 1));
        ASSERT (m_hbmMask != NULL);
        VERIFY (LoadMaskedBitmap (MAKEINTRESOURCE (nBitmapID)));
        BITMAP bm;
        ::GetObject (m_hbm, sizeof (BITMAP), &bm);
        m_size = CSize (bm.bmWidth, bm.bmHeight);
    }
}

RulerItem::~RulerItem ()
{
    if (m_hbm != NULL)
        ::DeleteObject (m_hbm);
    if (m_hbmMask != NULL)
        ::DeleteObject (m_hbmMask);
}

BOOL RulerItem::LoadMaskedBitmap (LPCTSTR lpszResourceName)
{
    ASSERT (lpszResourceName != NULL);

    if (m_hbm != NULL)
        ::DeleteObject (m_hbm);

    HINSTANCE hInst = AfxFindResourceHandle (lpszResourceName, RT_BITMAP);
    HRSRC     hRsrc = ::FindResource (hInst, lpszResourceName, RT_BITMAP);
    if (hRsrc == NULL)
        return FALSE;

    m_hbm = AfxLoadSysColorBitmap (hInst, hRsrc);
    return (m_hbm != NULL);
}

void RulerItem::SetHorzPosTwips (int nXPos)
{
    if (GetHorzPosTwips () != nXPos) {
        if (m_bTrack)
            DrawFocusLine ();
        Invalidate ();
        m_nXPosTwips = nXPos;
        Invalidate ();
        if (m_bTrack)
            DrawFocusLine ();
    }
}

void RulerItem::TrackHorzPosTwips (int nXPos, BOOL /*bOnRuler*/)
{
    int nMin = GetMin ();
    int nMax = GetMax ();
    if (nXPos < nMin)
        nXPos = nMin;
    if (nXPos > nMax)
        nXPos = nMax;
    SetHorzPosTwips (nXPos);
}

void RulerItem::DrawFocusLine ()
{
    if (GetHorzPosTwips () != 0) {
        m_rcTrack.left = m_rcTrack.right = GetHorzPosPix ();
        ASSERT (m_pDC != NULL);
        int nLeft = m_pRuler->XRulerToClient (m_rcTrack.left);
        m_pDC->MoveTo (nLeft, m_rcTrack.top);
        m_pDC->LineTo (nLeft, m_rcTrack.bottom);
    }
}

void RulerItem::SetTrack (BOOL b)
{
    m_bTrack = b;

    if (m_pDC != NULL) { // just in case we lost focus Capture somewhere
        DrawFocusLine ();
        m_pDC->RestoreDC (-1);
        delete m_pDC;
        m_pDC = NULL;
    }
    if (m_bTrack) {
        CView* pView = m_pRuler->GetView ();
        ASSERT (pView != NULL);
        pView->GetClientRect (&m_rcTrack);
        m_pDC = new CWindowDC (pView);
        m_pDC->SaveDC ();
        m_pDC->SelectObject (&m_pRuler->penFocusLine);
        m_pDC->SetROP2 (R2_XORPEN);
        DrawFocusLine ();
    }
}

void RulerItem::Invalidate ()
{
    CRect rc = GetHitRectPix ();
    m_pRuler->RulerToClient (rc.TopLeft ());
    m_pRuler->RulerToClient (rc.BottomRight ());
    m_pRuler->InvalidateRect (rc);
}

CRect RulerItem::GetHitRectPix ()
{
    int nx = GetHorzPosPix ();
    return CRect (
        CPoint (
            (m_nAlignment == TA_CENTER) ? (nx - m_size.cx / 2) : (m_nAlignment == TA_LEFT) ? nx : nx - m_size.cx, m_nYPosPix),
        m_size);
}

void RulerItem::Draw (CDC& dc)
{
    CDC dcBitmap;
    dcBitmap.CreateCompatibleDC (&dc);
    CPoint pt (GetHorzPosPix (), GetVertPosPix ());

    HGDIOBJ hbm = ::SelectObject (dcBitmap.m_hDC, m_hbmMask);

    // do mask part
    if (m_nAlignment == TA_CENTER)
        dc.BitBlt (pt.x - m_size.cx / 2, pt.y, m_size.cx, m_size.cy, &dcBitmap, 0, 0, SRCAND);
    else if (m_nAlignment == TA_LEFT)
        dc.BitBlt (pt.x, pt.y, m_size.cx, m_size.cy, &dcBitmap, 0, 0, SRCAND);
    else // TA_RIGHT
        dc.BitBlt (pt.x - m_size.cx, pt.y, m_size.cx, m_size.cy, &dcBitmap, 0, 0, SRCAND);

    // do image part
    ::SelectObject (dcBitmap.m_hDC, m_hbm);

    if (m_nAlignment == TA_CENTER)
        dc.BitBlt (pt.x - m_size.cx / 2, pt.y, m_size.cx, m_size.cy, &dcBitmap, 0, 0, SRCINVERT);
    else if (m_nAlignment == TA_LEFT)
        dc.BitBlt (pt.x, pt.y, m_size.cx, m_size.cy, &dcBitmap, 0, 0, SRCINVERT);
    else // TA_RIGHT
        dc.BitBlt (pt.x - m_size.cx, pt.y, m_size.cx, m_size.cy, &dcBitmap, 0, 0, SRCINVERT);

    ::SelectObject (dcBitmap.m_hDC, hbm);
}

CComboRulerItem::CComboRulerItem (UINT nBitmapID1, UINT nBitmapID2, RulerItem& item)
    : RulerItem (nBitmapID1)
    , m_secondary (nBitmapID2)
    , m_link (item)
{
    m_bHitPrimary = TRUE;
}

BOOL CComboRulerItem::HitTestPix (CPoint pt)
{
    m_bHitPrimary = FALSE;
    if (RulerItem::GetHitRectPix ().PtInRect (pt))
        m_bHitPrimary = TRUE;
    else
        return m_secondary.HitTestPix (pt);
    return TRUE;
}

void CComboRulerItem::Draw (CDC& dc)
{
    RulerItem::Draw (dc);
    m_secondary.Draw (dc);
}

void CComboRulerItem::SetHorzPosTwips (int nXPos)
{
    if (m_bHitPrimary) // only change linked items by delta
        m_link.SetHorzPosTwips (m_link.GetHorzPosTwips () + nXPos - GetHorzPosTwips ());
    RulerItem::SetHorzPosTwips (nXPos);
    m_secondary.SetHorzPosTwips (nXPos);
}

void CComboRulerItem::TrackHorzPosTwips (int nXPos, BOOL /*bOnRuler*/)
{
    int nMin = GetMin ();
    int nMax = GetMax ();
    if (nXPos < nMin)
        nXPos = nMin;
    if (nXPos > nMax)
        nXPos = nMax;
    SetHorzPosTwips (nXPos);
}

void CComboRulerItem::SetVertPos (int nYPos)
{
    m_secondary.SetVertPos (nYPos);
    nYPos += m_secondary.GetHitRectPix ().Height ();
    RulerItem::SetVertPos (nYPos);
}

void CComboRulerItem::SetAlignment (int nAlign)
{
    RulerItem::SetAlignment (nAlign);
    m_secondary.SetAlignment (nAlign);
}

void CComboRulerItem::SetRuler (RulerBar* pRuler)
{
    m_pRuler = pRuler;
    m_secondary.SetRuler (pRuler);
}

void CComboRulerItem::SetBounds (int nMin, int nMax)
{
    RulerItem::SetBounds (nMin, nMax);
    m_secondary.SetBounds (nMin, nMax);
}

int CComboRulerItem::GetMin ()
{
    if (m_bHitPrimary) {
        int nPDist = GetHorzPosTwips () - RulerItem::GetMin ();
        int nLDist = m_link.GetHorzPosTwips () - m_link.GetMin ();
        return GetHorzPosTwips () - min (nPDist, nLDist);
    }
    else
        return RulerItem::GetMin ();
}

int CComboRulerItem::GetMax ()
{
    if (m_bHitPrimary) {
        int nPDist   = RulerItem::GetMax () - GetHorzPosTwips ();
        int nLDist   = m_link.GetMax () - m_link.GetHorzPosTwips ();
        int nMinDist = (nPDist < nLDist) ? nPDist : nLDist;
        return GetHorzPosTwips () + nMinDist;
    }
    else
        return RulerItem::GetMax ();
}

void CTabRulerItem::TrackHorzPosTwips (int nXPos, BOOL bOnRuler)
{
    if (bOnRuler)
        RulerItem::TrackHorzPosTwips (nXPos, bOnRuler);
    else
        RulerItem::TrackHorzPosTwips (0, bOnRuler);
}

BEGIN_MESSAGE_MAP (RulerBar, CControlBar)
//{{AFX_MSG_MAP(RulerBar)
ON_WM_LBUTTONDOWN ()
ON_WM_LBUTTONUP ()
ON_WM_MOUSEMOVE ()
ON_WM_SYSCOLORCHANGE ()
ON_WM_WINDOWPOSCHANGING ()
ON_WM_SHOWWINDOW ()
ON_WM_WINDOWPOSCHANGED ()
//}}AFX_MSG_MAP
ON_MESSAGE (WM_SIZEPARENT, OnSizeParent)
// Global help commands
END_MESSAGE_MAP ()

RulerBar::RulerBar (BOOL b3DExt)
    : m_leftmargin (IDB_RULER_BLOCK, IDB_RULER_UP, m_indent)
    , m_indent (IDB_RULER_DOWN)
    , m_rightmargin (IDB_RULER_UP)
    , m_tabItem (IDB_RULER_TAB)
{
    m_bDeferInProgress = FALSE;
    m_leftmargin.SetRuler (this);
    m_indent.SetRuler (this);
    m_rightmargin.SetRuler (this);

    // all of the tab stops share handles
    for (int i = 0; i < MAX_TAB_STOPS; i++) {
        m_pTabItems[i].m_hbm     = m_tabItem.m_hbm;
        m_pTabItems[i].m_hbmMask = m_tabItem.m_hbmMask;
        m_pTabItems[i].m_size    = m_tabItem.m_size;
    }

    m_unit.m_nTPU = 0;
    m_nScroll     = 0;

    LOGFONT m_lf;
    {
        HFONT hFont = (HFONT)GetStockObject (DEFAULT_GUI_FONT);
        if (hFont == NULL)
            hFont = (HFONT)GetStockObject (ANSI_VAR_FONT);
        VERIFY (GetObject (hFont, sizeof (LOGFONT), &m_lf));
    }

    LOGFONT lf;
    memcpy (&lf, &m_lf, sizeof (LOGFONT));
    lf.lfHeight = -8;
    lf.lfWidth  = 0;
    VERIFY (fnt.CreateFontIndirect (&lf));

    m_nTabs = 0;
    m_leftmargin.SetVertPos (9);
    m_indent.SetVertPos (-1);
    m_rightmargin.SetVertPos (9);

    //  m_cxLeftBorder = 0;
    m_cxLeftBorder = Led_CvtScreenPixelsFromTWIPSH (kLedItViewLHSMargin);
    m_bDraw3DExt   = b3DExt;

    m_cyTopBorder    = 4;
    m_cyBottomBorder = 6;

    m_pSelItem = NULL;

    CWindowDC screenDC (NULL);
    m_logx = screenDC.GetDeviceCaps (LOGPIXELSX);

    CreateGDIObjects ();
}

RulerBar::~RulerBar ()
{
    // set handles to NULL to avoid deleting twice
    for (int i = 0; i < MAX_TAB_STOPS; i++) {
        m_pTabItems[i].m_hbm     = NULL;
        m_pTabItems[i].m_hbmMask = NULL;
    }
}

void RulerBar::CreateGDIObjects ()
{
    penFocusLine.DeleteObject ();
    penBtnHighLight.DeleteObject ();
    penBtnShadow.DeleteObject ();
    penWindowFrame.DeleteObject ();
    penBtnText.DeleteObject ();
    penBtnFace.DeleteObject ();
    penWindowText.DeleteObject ();
    penWindow.DeleteObject ();
    brushWindow.DeleteObject ();
    brushBtnFace.DeleteObject ();

    penFocusLine.CreatePen (PS_DOT, 1, GetSysColor (COLOR_WINDOWTEXT));
    penBtnHighLight.CreatePen (PS_SOLID, 0, GetSysColor (COLOR_BTNHIGHLIGHT));
    penBtnShadow.CreatePen (PS_SOLID, 0, GetSysColor (COLOR_BTNSHADOW));
    penWindowFrame.CreatePen (PS_SOLID, 0, GetSysColor (COLOR_WINDOWFRAME));
    penBtnText.CreatePen (PS_SOLID, 0, GetSysColor (COLOR_BTNTEXT));
    penBtnFace.CreatePen (PS_SOLID, 0, GetSysColor (COLOR_BTNFACE));
    penWindowText.CreatePen (PS_SOLID, 0, GetSysColor (COLOR_WINDOWTEXT));
    penWindow.CreatePen (PS_SOLID, 0, GetSysColor (COLOR_WINDOW));
    brushWindow.CreateSolidBrush (GetSysColor (COLOR_WINDOW));
    brushBtnFace.CreateSolidBrush (GetSysColor (COLOR_BTNFACE));
}

LedItView* RulerBar::GetView ()
{
    ASSERT (GetParent () != NULL);
    return dynamic_cast<LedItView*> (((CFrameWnd*)GetParent ())->GetActiveView ());
}

void RulerBar::OnUpdateCmdUI (CFrameWnd* /*pTarget*/, BOOL /*bDisableIfNoHndler*/)
{
    ASSERT_VALID (this);
    //Get the page size and see if changed -- from document
    //get margins and tabs and see if changed -- from view
    if (m_pSelItem == NULL) { // only update if not in middle of dragging
        LedItView* pView = GetView ();

        //TMPHACK
        CSize fakePaperSize (0x2fd0, 0x3de0);
        CRect fakeMargins (0x5a0, 0x05a0, 0x708, 0x708);
        Update (fakePaperSize, fakeMargins);
        //PARAFORMAT    fakeParaFormat;
        //memset (&fakeParaFormat. 0, sizeof (fakeParaFormat));
        //Update(fakeParaFormat);
        Update (pView->GetParaFormatSelection ());

#if 0
// must add ability to get this crap from Led!!!

        ASSERT(pView != NULL);
        Update(pView->GetPaperSize(), pView->GetMargins());
        Update(pView->GetParaFormatSelection());
        CRect rect;
        pView->GetRichEditCtrl().GetRect(&rect);
        CPoint pt = rect.TopLeft();
        pView->ClientToScreen(&pt);
        ScreenToClient(&pt);
        if (m_cxLeftBorder != pt.x) {
            m_cxLeftBorder = pt.x;
            Invalidate();
        }
        int nScroll = pView->GetScrollPos(SB_HORZ);
        if (nScroll != m_nScroll) {
            m_nScroll = nScroll;
            Invalidate();
        }
#endif
    }
}

CSize RulerBar::GetBaseUnits ()
{
    ASSERT (fnt.GetSafeHandle () != NULL);
    CWindowDC  screenDC (NULL);
    CFont*     pFont = screenDC.SelectObject (&fnt);
    TEXTMETRIC tm;
    VERIFY (screenDC.GetTextMetrics (&tm) == TRUE);
    screenDC.SelectObject (pFont);
    return CSize (tm.tmAveCharWidth, tm.tmHeight);
}

BOOL RulerBar::Create (CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
    ASSERT_VALID (pParentWnd); // must have a parent

    dwStyle |= WS_CLIPSIBLINGS;
    // force WS_CLIPSIBLINGS (avoids SetWindowPos bugs)
    m_dwStyle = (dwStyle & CBRS_ALL);

    // create the HWND
    CRect rect;
    rect.SetRectEmpty ();
    LPCTSTR lpszClass = AfxRegisterWndClass (0, ::LoadCursor (NULL, IDC_ARROW),
                                             (HBRUSH) (COLOR_BTNFACE + 1), NULL);

    if (!CWnd::Create (lpszClass, NULL, dwStyle, rect, pParentWnd, nID))
        return FALSE;
    // NOTE: Parent must resize itself for control bar to be resized

    int i;
    int nMax = 100;
    for (i = 0; i < MAX_TAB_STOPS; i++) {
        m_pTabItems[i].SetRuler (this);
        m_pTabItems[i].SetVertPos (8);
        m_pTabItems[i].SetHorzPosTwips (0);
        m_pTabItems[i].SetBounds (0, nMax);
    }
    return TRUE;
}

CSize RulerBar::CalcFixedLayout (BOOL bStretch, BOOL bHorz)
{
    ASSERT (bHorz);
    CSize m_size = CControlBar::CalcFixedLayout (bStretch, bHorz);
    CRect rectSize;
    rectSize.SetRectEmpty ();
    CalcInsideRect (rectSize, bHorz); // will be negative size
    m_size.cy = RULERBARHEIGHT - rectSize.Height ();
    return m_size;
}

void RulerBar::Update (const WordProcessor::IncrementalParagraphInfo& pf)
{
    if (pf.GetTabStopList_Valid ()) {
        TextImager::StandardTabStopList tabStops = pf.GetTabStopList ();
        Led_Distance                    tabSoFar = 0;
        Require (tabStops.fTabStops.size () <= MAX_TAB_STOPS);
        size_t i = 0;
        for (; i < tabStops.fTabStops.size (); ++i) {
            tabSoFar += tabStops.fTabStops[i];
            m_pTabItems[i].SetHorzPosTwips (tabSoFar);
        }
        for (; i < MAX_TAB_STOPS; i++) {
            m_pTabItems[i].SetHorzPosTwips (0);
        }
    }

    if (pf.GetMargins_Valid ()) {
        m_leftmargin.SetHorzPosTwips (pf.GetLeftMargin ());
        m_rightmargin.SetHorzPosTwips (pf.GetRightMargin ());
        if (pf.GetFirstIndent_Valid ()) {
            m_indent.SetHorzPosTwips (pf.GetLeftMargin () + pf.GetFirstIndent ());
        }
    }
}

void RulerBar::Update (CSize sizePaper, const CRect& rectMargins)
{
    if ((sizePaper != m_sizePaper) || (rectMargins != m_rectMargin)) {
        m_sizePaper  = sizePaper;
        m_rectMargin = rectMargins;
        Invalidate ();
    }

    if (m_unit.GetTPU () != StandardUnits::GetCurrentUnits ().GetTPU ()) {
        m_unit = StandardUnits::GetCurrentUnits ();
        Invalidate ();
    }
#if 0
    // MUST ADD ABILITY TO GET THSI CRAP FROM LED??

    if (m_unit.m_nTPU != theApp.GetTPU()) {
        m_unit = theApp.GetUnit();
        Invalidate();
    }
#endif
}

void RulerBar::FillInParaFormat (WordProcessor::IncrementalParagraphInfo& pf)
{
    pf = IncrementalParagraphInfo ();
    //we don't set the justification - just the tabstops (AND SOON THE START/RIGHT INDENT).
    SortTabs ();
    int               nPos = 0;
    vector<Led_TWIPS> v;
    int               soFar = 0;
    for (size_t i = 0; i < MAX_TAB_STOPS; i++) {
        // get rid of zeroes and multiples
        // i.e. if we have 0,0,0,1,2,3,4,4,5
        // we will get tabs at 1,2,3,4,5
        if (nPos != m_pTabItems[i].GetHorzPosTwips ()) {
            nPos = m_pTabItems[i].GetHorzPosTwips ();
            Assert (nPos > soFar);
            //pf.rgxTabs[pf.cTabCount++] = nPos;
            v.push_back (Led_TWIPS (nPos - soFar));
            soFar = nPos;
        }
    }
    pf.SetTabStopList (TextImager::StandardTabStopList (v));

    pf.SetMargins (Led_TWIPS (m_leftmargin.GetHorzPosTwips ()), Led_TWIPS (m_rightmargin.GetHorzPosTwips ()));
    pf.SetFirstIndent (Led_TWIPS (m_indent.GetHorzPosTwips () - m_leftmargin.GetHorzPosTwips ()));
}

// simple bubble sort is adequate for small number of tabs
void RulerBar::SortTabs ()
{
    int i, j, nPos;
    for (i = 0; i < MAX_TAB_STOPS - 1; i++) {
        for (j = i + 1; j < MAX_TAB_STOPS; j++) {
            if (m_pTabItems[j].GetHorzPosTwips () < m_pTabItems[i].GetHorzPosTwips ()) {
                nPos = m_pTabItems[j].GetHorzPosTwips ();
                m_pTabItems[j].SetHorzPosTwips (m_pTabItems[i].GetHorzPosTwips ());
                m_pTabItems[i].SetHorzPosTwips (nPos);
            }
        }
    }
}

void RulerBar::DoPaint (CDC* pDC)
{
    CControlBar::DoPaint (pDC); // CControlBar::DoPaint -- draws border
    if (m_unit.m_nTPU != 0) {
        pDC->SaveDC ();
        // offset coordinate system
        CPoint pointOffset (0, 0);
        RulerToClient (pointOffset);
        pDC->SetViewportOrg (pointOffset);

        DrawFace (*pDC);
        DrawTickMarks (*pDC);

        DrawTabs (*pDC);
        m_leftmargin.Draw (*pDC);
        m_indent.Draw (*pDC);
        m_rightmargin.Draw (*pDC);

        pDC->RestoreDC (-1);
    }
    // Do not call CControlBar::OnPaint() for painting messages
}

void RulerBar::DrawTabs (CDC& dc)
{
    int i;
    int nPos = 0;
    for (i = 0; i < MAX_TAB_STOPS; i++) {
        if (m_pTabItems[i].GetHorzPosTwips () > nPos)
            nPos = (m_pTabItems[i].GetHorzPosTwips ());
        m_pTabItems[i].Draw (dc);
    }
    int nPageWidth = PrintWidth ();
    nPos           = nPos - nPos % 720 + 720;
    dc.SelectObject (&penBtnShadow);
    for (; nPos < nPageWidth; nPos += 720) {
        int nx = XTwipsToRuler (nPos);
        dc.MoveTo (nx, HEIGHT - 1);
        dc.LineTo (nx, HEIGHT + 1);
    }
}

void RulerBar::DrawFace (CDC& dc)
{
    int nPageWidth = XTwipsToRuler (PrintWidth ());
    int nPageEdge  = XTwipsToRuler (PrintWidth () + m_rectMargin.right);

    dc.SaveDC ();

    dc.SelectObject (&penBtnShadow);
    dc.MoveTo (0, 0);
    dc.LineTo (nPageEdge - 1, 0);
    dc.LineTo (nPageEdge - 1, HEIGHT - 2);
    dc.LineTo (nPageWidth - 1, HEIGHT - 2);
    dc.LineTo (nPageWidth - 1, 1);
    dc.LineTo (nPageWidth, 1);
    dc.LineTo (nPageWidth, HEIGHT - 2);

    dc.SelectObject (&penBtnHighLight);
    dc.MoveTo (nPageWidth, HEIGHT - 1);
    dc.LineTo (nPageEdge, HEIGHT - 1);
    dc.MoveTo (nPageWidth + 1, HEIGHT - 3);
    dc.LineTo (nPageWidth + 1, 1);
    dc.LineTo (nPageEdge - 1, 1);

    dc.SelectObject (&penWindow);
    dc.MoveTo (0, HEIGHT - 1);
    dc.LineTo (nPageWidth, HEIGHT - 1);

    dc.SelectObject (&penBtnFace);
    dc.MoveTo (1, HEIGHT - 2);
    dc.LineTo (nPageWidth - 1, HEIGHT - 2);

    dc.SelectObject (&penWindowFrame);
    dc.MoveTo (0, HEIGHT - 2);
    dc.LineTo (0, 1);
    dc.LineTo (nPageWidth - 1, 1);

    dc.FillRect (CRect (1, 2, nPageWidth - 1, HEIGHT - 2), &brushWindow);
    dc.FillRect (CRect (nPageWidth + 2, 2, nPageEdge - 1, HEIGHT - 2), &brushBtnFace);

    CRect rcClient;
    GetClientRect (&rcClient);
    ClientToRuler (rcClient);
    rcClient.top    = HEIGHT;
    rcClient.bottom = HEIGHT + 2;
    dc.FillRect (rcClient, &brushBtnFace);

    CRect rectFill (rcClient.left, HEIGHT + 4, rcClient.right, HEIGHT + 9);
    dc.FillRect (rectFill, &brushWindow);

    if (m_bDraw3DExt) { // draws the 3D extension into the view
        dc.SelectObject (&penBtnShadow);
        dc.MoveTo (rcClient.left, HEIGHT + 8);
        dc.LineTo (rcClient.left, HEIGHT + 2);
        dc.LineTo (rcClient.right - 1, HEIGHT + 2);

        dc.SelectObject (&penWindowFrame);
        dc.MoveTo (rcClient.left + 1, HEIGHT + 8);
        dc.LineTo (rcClient.left + 1, HEIGHT + 3);
        dc.LineTo (rcClient.right - 2, HEIGHT + 3);

        dc.SelectObject (&penBtnHighLight);
        dc.MoveTo (rcClient.right - 1, HEIGHT + 2);
        dc.LineTo (rcClient.right - 1, HEIGHT + 8);

        dc.SelectObject (&penBtnFace);
        dc.MoveTo (rcClient.right - 2, HEIGHT + 3);
        dc.LineTo (rcClient.right - 2, HEIGHT + 8);
    }
    else {
        dc.SelectObject (&penBtnShadow);
        dc.MoveTo (rcClient.left, HEIGHT + 2);
        dc.LineTo (rcClient.right, HEIGHT + 2);

        dc.SelectObject (&penWindowFrame);
        dc.MoveTo (rcClient.left, HEIGHT + 3);
        dc.LineTo (rcClient.right, HEIGHT + 3);
    }

    dc.RestoreDC (-1);
}

void RulerBar::DrawTickMarks (CDC& dc)
{
    dc.SaveDC ();

    dc.SelectObject (&penWindowText);
    dc.SelectObject (&fnt);
    dc.SetTextColor (GetSysColor (COLOR_WINDOWTEXT));
    dc.SetBkMode (TRANSPARENT);

    DrawDiv (dc, m_unit.m_nSmallDiv, m_unit.m_nLargeDiv, 2);
    DrawDiv (dc, m_unit.m_nMediumDiv, m_unit.m_nLargeDiv, 5);
    DrawNumbers (dc, m_unit.m_nLargeDiv, m_unit.m_nTPU);

    dc.RestoreDC (-1);
}

void RulerBar::DrawNumbers (CDC& dc, int nInc, int nTPU)
{
    int   nPageWidth = PrintWidth ();
    int   nPageEdge  = nPageWidth + m_rectMargin.right;
    TCHAR buf[10];

    int nTwips, nPixel, nLen;

    for (nTwips = nInc; nTwips < nPageEdge; nTwips += nInc) {
        if (nTwips == nPageWidth)
            continue;
        nPixel = XTwipsToRuler (nTwips);
        wsprintf (buf, _T("%d"), nTwips / nTPU);
        nLen     = lstrlen (buf);
        CSize sz = dc.GetTextExtent (buf, nLen);
        dc.ExtTextOut (nPixel - sz.cx / 2, HEIGHT / 2 - sz.cy / 2, 0, NULL, buf, nLen, NULL);
    }
}

void RulerBar::DrawDiv (CDC& dc, int nInc, int nLargeDiv, int nLength)
{
    int nPageWidth = PrintWidth ();
    int nPageEdge  = nPageWidth + m_rectMargin.right;

    int nTwips, nPixel;

    for (nTwips = nInc; nTwips < nPageEdge; nTwips += nInc) {
        if (nTwips == nPageWidth || nTwips % nLargeDiv == 0)
            continue;
        nPixel = XTwipsToRuler (nTwips);
        dc.MoveTo (nPixel, HEIGHT / 2 - nLength / 2);
        dc.LineTo (nPixel, HEIGHT / 2 - nLength / 2 + nLength);
    }
}

void RulerBar::OnLButtonDown (UINT nFlags, CPoint point)
{
    CPoint pt = point;
    ClientToRuler (pt);

    m_pSelItem = NULL;
    if (m_leftmargin.HitTestPix (pt))
        m_pSelItem = &m_leftmargin;
    else if (m_indent.HitTestPix (pt))
        m_pSelItem = &m_indent;
    else if (m_rightmargin.HitTestPix (pt))
        m_pSelItem = &m_rightmargin;
    else
        m_pSelItem = GetHitTabPix (pt);
    if (m_pSelItem == NULL)
        m_pSelItem = GetFreeTab ();
    if (m_pSelItem == NULL)
        return;
    SetCapture ();

    m_pSelItem->SetTrack (TRUE);
    SetMarginBounds ();
    OnMouseMove (nFlags, point);
}

void RulerBar::SetMarginBounds ()
{
    m_leftmargin.SetBounds (0, m_rightmargin.GetHorzPosTwips ());
    m_indent.SetBounds (0, m_rightmargin.GetHorzPosTwips ());

    int nMin = (m_leftmargin.GetHorzPosTwips () > m_indent.GetHorzPosTwips ()) ? m_leftmargin.GetHorzPosTwips () : m_indent.GetHorzPosTwips ();
    int nMax = PrintWidth () + m_rectMargin.right;
    m_rightmargin.SetBounds (nMin, nMax);

    // tabs can go from zero to the right page edge
    for (int i = 0; i < MAX_TAB_STOPS; i++)
        m_pTabItems[i].SetBounds (0, nMax);
}

RulerItem* RulerBar::GetFreeTab ()
{
    int i;
    for (i = 0; i < MAX_TAB_STOPS; i++) {
        if (m_pTabItems[i].GetHorzPosTwips () == 0)
            return &m_pTabItems[i];
    }
    return NULL;
}

CTabRulerItem* RulerBar::GetHitTabPix (CPoint point)
{
    int i;
    for (i = 0; i < MAX_TAB_STOPS; i++) {
        if (m_pTabItems[i].HitTestPix (point))
            return &m_pTabItems[i];
    }
    return NULL;
}

void RulerBar::OnLButtonUp (UINT nFlags, CPoint point)
{
    if (::GetCapture () != m_hWnd)
        return;
    OnMouseMove (nFlags, point);
    m_pSelItem->SetTrack (FALSE);
    ReleaseCapture ();
    LedItView*               pView = GetView ();
    IncrementalParagraphInfo pf    = pView->GetParaFormatSelection ();
    FillInParaFormat (pf);
    pView->SetParaFormatSelection (pf);

    m_pSelItem = NULL;
}

void RulerBar::OnMouseMove (UINT nFlags, CPoint point)
{
    CControlBar::OnMouseMove (nFlags, point);
    // use ::GetCapture to avoid creating temporaries
    if (::GetCapture () != m_hWnd)
        return;
    ASSERT (m_pSelItem != NULL);
    CRect rc (0, 0, XTwipsToRuler (PrintWidth () + m_rectMargin.right), HEIGHT);
    RulerToClient (rc);
    BOOL bOnRuler = rc.PtInRect (point);

    // snap to minimum movement
    point.x = XClientToTwips (point.x);
    point.x += m_unit.m_nMinMove / 2;
    point.x -= point.x % m_unit.m_nMinMove;

    m_pSelItem->TrackHorzPosTwips (point.x, bOnRuler);
    UpdateWindow ();
}

void RulerBar::OnSysColorChange ()
{
    CControlBar::OnSysColorChange ();
    CreateGDIObjects ();
    Invalidate ();
}

void RulerBar::OnWindowPosChanging (WINDOWPOS FAR* lpwndpos)
{
    CControlBar::OnWindowPosChanging (lpwndpos);
    CRect rect;
    GetClientRect (rect);
    int minx = min (rect.Width (), lpwndpos->cx);
    int maxx = max (rect.Width (), lpwndpos->cx);
    rect.SetRect (minx - 2, rect.bottom - 6, minx, rect.bottom);
    InvalidateRect (rect);
    rect.SetRect (maxx - 2, rect.bottom - 6, maxx, rect.bottom);
    InvalidateRect (rect);
}

void RulerBar::OnShowWindow (BOOL bShow, UINT nStatus)
{
    CControlBar::OnShowWindow (bShow, nStatus);
    m_bDeferInProgress = FALSE;
}

void RulerBar::OnWindowPosChanged (WINDOWPOS FAR* lpwndpos)
{
    CControlBar::OnWindowPosChanged (lpwndpos);
    m_bDeferInProgress = FALSE;
}

LRESULT RulerBar::OnSizeParent (WPARAM wParam, LPARAM lParam)
{
    BOOL bVis = GetStyle () & WS_VISIBLE;
    if ((bVis && (m_nStateFlags & delayHide)) ||
        (!bVis && (m_nStateFlags & delayShow))) {
        m_bDeferInProgress = TRUE;
    }
    return CControlBar::OnSizeParent (wParam, lParam);
}
