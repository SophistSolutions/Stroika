/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#if defined(WIN32)
#include <afxwin.h>
#endif

#include "LedItResources.h"

#include "ColorMenu.h"

#if qPlatform_Windows
/*
 ********************************************************************************
 *********************************** ColorMenu **********************************
 ********************************************************************************
 */

ColorMenu::ColorMenu ()
{
    VERIFY (CreatePopupMenu ());
    ASSERT (GetMenuItemCount () == 0);
    for (int i = 0; i <= 15; i++) {
        VERIFY (AppendMenu (MF_OWNERDRAW, kBaseFontColorCmd + i, (LPCTSTR) (kBaseFontColorCmd + i)));
    }
    // ADD:
    //          MenuItem    SEPARATOR
    //      MenuItem    "Other...",             kFontSizeOtherCmdID
}

COLORREF ColorMenu::GetColor (UINT id)
{
    return FontCmdToColor (id).GetOSRep ();
}

Led_Color ColorMenu::FontCmdToColor (UINT cmd)
{
    ASSERT (cmd >= kBaseFontColorCmd);
    ASSERT (cmd <= kLastFontNamedColorCmd);
    switch (cmd) {
        case kBlackColorCmd:
            return Led_Color::kBlack;
        case kMaroonColorCmd:
            return Led_Color::kMaroon;
        case kGreenColorCmd:
            return Led_Color::kDarkGreen;
        case kOliveColorCmd:
            return Led_Color::kOlive;
        case kNavyColorCmd:
            return Led_Color::kNavyBlue;
        case kPurpleColorCmd:
            return Led_Color::kPurple;
        case kTealColorCmd:
            return Led_Color::kTeal;
        case kGrayColorCmd:
            return Led_Color::kGray;
        case kSilverColorCmd:
            return Led_Color::kSilver;
        case kRedColorCmd:
            return Led_Color::kRed;
        case kLimeColorCmd:
            return Led_Color::kGreen;
        case kYellowColorCmd:
            return Led_Color::kYellow;
        case kBlueColorCmd:
            return Led_Color::kBlue;
        case kFuchsiaColorCmd:
            return Led_Color::kMagenta;
        case kAquaColorCmd:
            return Led_Color::kCyan;
        case kWhiteColorCmd:
            return Led_Color::kWhite;
        default: /*Led_Assert (false);*/
            return Led_Color::kBlack;
    }
    //  return Led_Color (GetColor (cmd));
}

UINT ColorMenu::FontColorToCmd (Led_Color color)
{
    for (UINT i = kBaseFontColorCmd; i <= kLastFontNamedColorCmd; ++i) {
        if (FontCmdToColor (i) == color) {
            return i;
        }
    }
    return kFontColorOtherCmd;
}

void ColorMenu::DrawItem (LPDRAWITEMSTRUCT lpDIS)
{
    ASSERT (lpDIS->CtlType == ODT_MENU);
    UINT id = (UINT) (WORD)lpDIS->itemID;
    ASSERT (id == lpDIS->itemData);
    ASSERT (id >= kBaseFontColorCmd);
    ASSERT (id <= kLastFontNamedColorCmd);
    CDC dc;
    dc.Attach (lpDIS->hDC);

    CRect rc (lpDIS->rcItem);
    ASSERT (rc.Width () < 500);
    if (lpDIS->itemState & ODS_FOCUS)
        dc.DrawFocusRect (&rc);

    COLORREF cr = (lpDIS->itemState & ODS_SELECTED) ? ::GetSysColor (COLOR_HIGHLIGHT) : dc.GetBkColor ();

    CBrush brushFill (cr);
    cr = dc.GetTextColor ();

    if (lpDIS->itemState & ODS_SELECTED)
        dc.SetTextColor (::GetSysColor (COLOR_HIGHLIGHTTEXT));

    int nBkMode = dc.SetBkMode (TRANSPARENT);
    dc.FillRect (&rc, &brushFill);

    rc.left += 50;
    CString strColor;
    strColor.LoadString (id);
    dc.TextOut (rc.left, rc.top, strColor, strColor.GetLength ());
    rc.left -= 45;
    rc.top += 2;
    rc.bottom -= 2;
    rc.right = rc.left + 40;
    CBrush  brush (GetColor (id));
    CBrush* pOldBrush = dc.SelectObject (&brush);
    dc.Rectangle (rc);

    dc.SelectObject (pOldBrush);
    dc.SetTextColor (cr);
    dc.SetBkMode (nBkMode);

    dc.Detach ();
}

void ColorMenu::MeasureItem (LPMEASUREITEMSTRUCT lpMIS)
{
    ASSERT (lpMIS->CtlType == ODT_MENU);
    UINT id = (UINT) (WORD)lpMIS->itemID;
    ASSERT (id == lpMIS->itemData);
    ASSERT (id >= kBaseFontColorCmd);
    ASSERT (id <= kLastFontNamedColorCmd);
    class CDisplayIC : public CDC {
    public:
        CDisplayIC () { CreateIC (_T("DISPLAY"), NULL, NULL, NULL); }
    };
    CDisplayIC dc;
    CString    strColor;
    strColor.LoadString (id);
    CSize sizeText = dc.GetTextExtent (strColor, strColor.GetLength ());
    ASSERT (sizeText.cx < 500);
    lpMIS->itemWidth  = sizeText.cx + 50;
    lpMIS->itemHeight = sizeText.cy;
}
#endif
