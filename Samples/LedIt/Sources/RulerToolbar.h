/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */

#ifndef __RulerToolbar_h__
#define __RulerToolbar_h__ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <afxext.h>

#include "Stroika/Frameworks/Led/Support.h"
#include "Stroika/Frameworks/Led/WordProcessor.h"

#include "LedItConfig.h"
#include "Units.h"

class LedItView;

class RulerBar : public CControlBar {
public:
    RulerBar (BOOL b3DExt = TRUE);
    ~RulerBar ();

public:
    virtual BOOL Create (CWnd* pParentWnd, DWORD dwStyle, UINT nID);

protected:
    void Update (const WordProcessor::IncrementalParagraphInfo& pf);
    void Update (const PARAFORMAT& pf);
    void Update (CSize sizePaper, const CRect& rectMargins);

    // ruler items include left margin, right margin, indent, and tabs

    // horz positions in twips -- necessary to avoid rounding errors
    // vertical position in pixels
public:
    class RulerItem {
    public:
        RulerItem (UINT nBitmapID = 0);
        ~RulerItem ();
        virtual BOOL HitTestPix (CPoint pt) { return GetHitRectPix ().PtInRect (pt); }
        virtual void Draw (CDC& dc);
        virtual void SetHorzPosTwips (int nXPos);
        virtual void TrackHorzPosTwips (int nXPos, BOOL bOnRuler = TRUE);
        virtual void SetVertPos (int nYPos) { m_nYPosPix = nYPos; }
        virtual void SetAlignment (int nAlign) { m_nAlignment = nAlign; }
        virtual void SetRuler (RulerBar* pRuler) { m_pRuler = pRuler; }
        virtual void SetBounds (int nMin, int nMax)
        {
            m_nMin = nMin;
            m_nMax = nMax;
        }
        int   GetMin () { return m_nMin; }
        int   GetMax () { return m_nMax; }
        void  Invalidate ();
        int   GetVertPosPix () { return m_nYPosPix; }
        int   GetHorzPosTwips () { return m_nXPosTwips; }
        int   GetHorzPosPix ();
        CRect GetHitRectPix ();
        void  DrawFocusLine ();
        void  SetTrack (BOOL b);

        HBITMAP m_hbm;
        HBITMAP m_hbmMask;
        CSize   m_size; // size of item in pixels

        // Operations
        BOOL LoadMaskedBitmap (LPCTSTR lpszResourceName);

    protected:
        int       m_nYPosPix;
        int       m_nXPosTwips;
        int       m_nAlignment;
        BOOL      m_bTrack;
        RulerBar* m_pRuler;
        CRect     m_rcTrack;
        CDC*      m_pDC; // dc used for drawing tracking line
        int       m_nMin, m_nMax;
    };

    class CComboRulerItem : public RulerItem {
    public:
        CComboRulerItem (UINT nBitmapID1, UINT nBitmapID2, RulerItem& item);
        virtual BOOL HitTestPix (CPoint pt);
        virtual void Draw (CDC& dc);
        virtual void SetHorzPosTwips (int nXPos);
        virtual void TrackHorzPosTwips (int nXPos, BOOL bOnRuler = TRUE);
        virtual void SetVertPos (int nYPos);
        virtual void SetAlignment (int nAlign);
        virtual void SetRuler (RulerBar* pRuler);
        virtual void SetBounds (int nMin, int nMax);
        int          GetMin ();
        int          GetMax ();

    protected:
        RulerItem  m_secondary;
        RulerItem& m_link;
        BOOL       m_bHitPrimary;
    };

    class CTabRulerItem : public RulerItem {
    public:
        CTabRulerItem () { SetAlignment (TA_LEFT); }
        virtual void Draw (CDC& dc)
        {
            if (GetHorzPosTwips () != 0)
                RulerItem::Draw (dc);
        }
        virtual void TrackHorzPosTwips (int nXPos, BOOL bOnRuler = TRUE);
        virtual BOOL HitTestPix (CPoint pt) { return (GetHorzPosTwips () != 0) ? RulerItem::HitTestPix (pt) : FALSE; }
    };

private:
    BOOL            m_bDeferInProgress;
    BOOL            m_bDraw3DExt;
    Unit            m_unit;
    RulerItem*      m_pSelItem;
    CFont           fnt;
    CSize           GetBaseUnits ();
    CComboRulerItem m_leftmargin;
    RulerItem       m_indent;
    RulerItem       m_rightmargin;
    RulerItem       m_tabItem;
    CTabRulerItem   m_pTabItems[MAX_TAB_STOPS];
    CSize           m_sizePaper;
    CRect           m_rectMargin;
    int             PrintWidth ()
    {
        return m_sizePaper.cx - m_rectMargin.left -
               m_rectMargin.right;
    }
    int m_nTabs;
    int m_logx;
    int m_nLinePos;
    int m_nScroll; // in pixels

    CPen   penFocusLine;
    CPen   penBtnHighLight;
    CPen   penBtnShadow;
    CPen   penWindowFrame;
    CPen   penBtnText;
    CPen   penBtnFace;
    CPen   penWindowText;
    CPen   penWindow;
    CBrush brushWindow;
    CBrush brushBtnFace;

    // Implementation
public:
    virtual void  DoPaint (CDC* pDC);
    virtual CSize CalcFixedLayout (BOOL bStretch, BOOL bHorz);
    void          ClientToRuler (CPoint& pt) { pt.Offset (-m_cxLeftBorder + m_nScroll, -m_cyTopBorder); }
    void          ClientToRuler (CRect& rect) { rect.OffsetRect (-m_cxLeftBorder + m_nScroll, -m_cyTopBorder); }
    void          RulerToClient (CPoint& pt) { pt.Offset (m_cxLeftBorder - m_nScroll, m_cyTopBorder); }
    void          RulerToClient (CRect& rect) { rect.OffsetRect (m_cxLeftBorder - m_nScroll, m_cyTopBorder); }

    int XTwipsToClient (int nT) { return MulDiv (nT, m_logx, 1440) + m_cxLeftBorder - m_nScroll; }
    int XClientToTwips (int nC) { return MulDiv (nC - m_cxLeftBorder + m_nScroll, 1440, m_logx); }

    int XTwipsToRuler (int nT) { return MulDiv (nT, m_logx, 1440); }
    int XRulerToTwips (int nR) { return MulDiv (nR, 1440, m_logx); }

    int XRulerToClient (int nR) { return nR + m_cxLeftBorder - m_nScroll; }
    int XClientToRuler (int nC) { return nC - m_cxLeftBorder + m_nScroll; }

protected:
    virtual void OnUpdateCmdUI (CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
    void         CreateGDIObjects ();
    void         DrawFace (CDC& dc);
    void         DrawTickMarks (CDC& dC);
    void         DrawNumbers (CDC& dc, int nInc, int nTPU);
    void         DrawDiv (CDC& dc, int nInc, int nLargeDiv, int nLength);
    void         DrawTabs (CDC& dc);
    void         FillInParaFormat (WordProcessor::IncrementalParagraphInfo& pf);
    void         FillInParaFormat (PARAFORMAT& pf);
    void         SortTabs ();
    void         SetMarginBounds ();
    RulerItem*   GetFreeTab ();
    LedItView*   GetView ();

    CTabRulerItem* GetHitTabPix (CPoint pt);
    afx_msg void   OnLButtonDown (UINT nFlags, CPoint point);
    afx_msg void   OnLButtonUp (UINT nFlags, CPoint point);
    afx_msg void   OnMouseMove (UINT nFlags, CPoint point);
    afx_msg void   OnSysColorChange ();
    afx_msg void   OnWindowPosChanging (WINDOWPOS FAR* lpwndpos);
    afx_msg void   OnShowWindow (BOOL bShow, UINT nStatus);
    afx_msg void   OnWindowPosChanged (WINDOWPOS FAR* lpwndpos);
    afx_msg LRESULT OnSizeParent (WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP ()
private:
    friend class RulerItem;
};

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
inline int RulerBar::RulerItem::GetHorzPosPix ()
{
    return m_pRuler->XTwipsToRuler (m_nXPosTwips);
}

#endif /*__RulerToolbar_h__*/
