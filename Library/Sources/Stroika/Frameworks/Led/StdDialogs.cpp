/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdio>

#if qStroika_Foundation_Common_Platform_Windows
#include <windows.h>

#include <commdlg.h>
#endif

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/LineEndings.h"
#include "Stroika/Frameworks/Led/GDI.h"

#include "StdDialogs.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

using Memory::StackBuffer;

namespace {
    SDKString FormatINTAsString (int t)
    {
        return Characters::CString::Format (Led_SDK_TCHAROF ("%d"), t);
    }
    bool ParseStringToINT (const SDKString& s, int* t)
    {
#if 1
        // get rid of this function - COULD do better API - but this is fine for how we currently use it...
        // -- LGP 2012-09-19
        RequireNotNull (t);
        *t = Characters::CString::String2Int<int> (s);
        return true;
#else
        int l = 0;
#if qTargetPlatformSDKUseswchar_t
        if (::swscanf (s.c_str (), L"%d", &l) < 1) {
            return false;
        }
        else {
            *t = TWIPS{l};
            return true;
        }
#else
        if (::sscanf (s.c_str (), "%d", &l) < 1) {
            return false;
        }
        else {
            *t = TWIPS (l);
            return true;
        }
#endif
#endif
    }
    // Later revise these so they take into account UNITS - like CM, or IN, or TWIPS, or pt, etc...
    SDKString FormatTWIPSAsString (TWIPS t)
    {
        return FormatINTAsString (t);
    }
    bool ParseStringToTWIPS (const SDKString& s, TWIPS* t)
    {
        int  i = 0;
        bool r = ParseStringToINT (s, &i);
        if (r) {
            *t = TWIPS{i};
        }
        return r;
    }
}

/*
 ********************************************************************************
 ******************************* StdColorPopupHelper ****************************
 ********************************************************************************
 */
StdColorPopupHelper::StdColorPopupHelper (bool allowNone)
    : fIsSelectedColor{allowNone}
    , fAllowNone{allowNone}
#if qStroika_Foundation_Common_Platform_Windows
    , fHWnd{NULL}
#endif
{
    if constexpr (qDebug) {
        [[maybe_unused]] size_t offset = fAllowNone ? 1 : 0;
        for (size_t i = 0; i < 16; ++i) {
            Assert (MapColorIdx (MapColorIdx (i + offset)) == i + offset);
        }
    }
}
StdColorPopupHelper::~StdColorPopupHelper ()
{
}

bool StdColorPopupHelper::GetSelectedColor (Color* c) const
{
    if (fIsSelectedColor and c != NULL) {
        *c = fSelectedColor;
    }
    return fIsSelectedColor;
}

void StdColorPopupHelper::SetSelectedColor (const Color& c)
{
    fSelectedColor   = c;
    fIsSelectedColor = true;
#if qStroika_Foundation_Common_Platform_Windows
    if (fHWnd != NULL) {
        Verify (::SendMessage (fHWnd, CB_SETCURSEL, MapColorIdx (c), 0) != CB_ERR);
    }
#endif
}

void StdColorPopupHelper::SetNoSelectedColor ()
{
    Require (fAllowNone);
    fIsSelectedColor = false;
#if qStroika_Foundation_Common_Platform_Windows
    if (fHWnd != NULL) {
        Verify (::SendMessage (fHWnd, CB_SETCURSEL, 0, 0) != CB_ERR);
    }
#endif
}

size_t StdColorPopupHelper::MapColorIdx (const Color& c) const
{
    size_t offset = fAllowNone ? 1 : 0;
    if (c == Color::kBlack) {
        return offset + 0;
    };
    if (c == Color::kMaroon) {
        return offset + 1;
    };
    if (c == Color::kGreen) {
        return offset + 2;
    };
    if (c == Color::kOlive) {
        return offset + 3;
    };
    if (c == Color::kNavyBlue) {
        return offset + 4;
    };
    if (c == Color::kPurple) {
        return offset + 5;
    };
    if (c == Color::kTeal) {
        return offset + 6;
    };
    if (c == Color::kGray) {
        return offset + 7;
    };
    if (c == Color::kSilver) {
        return offset + 8;
    };
    if (c == Color::kRed) {
        return offset + 9;
    };
    if (c == Color::kLimeGreen) {
        return offset + 10;
    };
    if (c == Color::kYellow) {
        return offset + 11;
    };
    if (c == Color::kBlue) {
        return offset + 12;
    };
    if (c == Color::kFuchsia) {
        return offset + 13;
    };
    if (c == Color::kAqua) {
        return offset + 14;
    };
    if (c == Color::kWhite) {
        return offset + 15;
    };
    return offset + 16; // OTHER
}

Color StdColorPopupHelper::MapColorIdx (size_t i) const
{
    size_t offset = fAllowNone ? 1 : 0;
    switch (i - offset) {
        case 0:
            return Color::kBlack;
        case 1:
            return Color::kMaroon;
        case 2:
            return Color::kGreen;
        case 3:
            return Color::kOlive;
        case 4:
            return Color::kNavyBlue;
        case 5:
            return Color::kPurple;
        case 6:
            return Color::kTeal;
        case 7:
            return Color::kGray;
        case 8:
            return Color::kSilver;
        case 9:
            return Color::kRed;
        case 10:
            return Color::kLimeGreen;
        case 11:
            return Color::kYellow;
        case 12:
            return Color::kBlue;
        case 13:
            return Color::kFuchsia;
        case 14:
            return Color::kAqua;
        case 15:
            return Color::kWhite;
        default:
            Assert (false);
            return Color::kBlack;
    }
}

#if qStroika_Foundation_Common_Platform_Windows
void StdColorPopupHelper::Attach (HWND popup)
{
    Require (fHWnd == NULL);
    RequireNotNull (popup);
    Require (::IsWindow (popup));
    fHWnd = popup;
    (void)::SendMessage (popup, CB_RESETCONTENT, 0, 0);
    DoMenuAppends ();
}
#endif

#if qStroika_Foundation_Common_Platform_Windows
void StdColorPopupHelper::OnSelChange ()
{
#if qStroika_Foundation_Common_Platform_Windows
    Require (::IsWindow (fHWnd));
    int r = static_cast<int> (::SendMessage (fHWnd, CB_GETCURSEL, 0, 0));
#endif

    int otherIdx = fAllowNone ? 17 : 16;
    if (fAllowNone and r == 0) {
        SetNoSelectedColor ();
    }
    else if (r == otherIdx) {
        StdColorPickBox colorPicker (fSelectedColor);
        if (colorPicker.DoModal ()) {
            SetSelectedColor (colorPicker.fColor);
        }
    }
    else {
        Color c = MapColorIdx (r);
        SetSelectedColor (c);
    }
}
#endif

#if qStroika_Foundation_Common_Platform_Windows
void StdColorPopupHelper::DoMenuAppends ()
{
    if (fAllowNone) {
        AppendMenuString (Led_SDK_TCHAROF (""));
    }
    AppendMenuString (Led_SDK_TCHAROF ("Black"));
    AppendMenuString (Led_SDK_TCHAROF ("Maroon"));
    AppendMenuString (Led_SDK_TCHAROF ("Green"));
    AppendMenuString (Led_SDK_TCHAROF ("Olive"));
    AppendMenuString (Led_SDK_TCHAROF ("Navy"));
    AppendMenuString (Led_SDK_TCHAROF ("Purple"));
    AppendMenuString (Led_SDK_TCHAROF ("Teal"));
    AppendMenuString (Led_SDK_TCHAROF ("Gray"));
    AppendMenuString (Led_SDK_TCHAROF ("Silver"));
    AppendMenuString (Led_SDK_TCHAROF ("Red"));
    AppendMenuString (Led_SDK_TCHAROF ("Lime"));
    AppendMenuString (Led_SDK_TCHAROF ("Yellow"));
    AppendMenuString (Led_SDK_TCHAROF ("Blue"));
    AppendMenuString (Led_SDK_TCHAROF ("Fuchsia"));
    AppendMenuString (Led_SDK_TCHAROF ("Aqua"));
    AppendMenuString (Led_SDK_TCHAROF ("White"));
    AppendMenuString (Led_SDK_TCHAROF ("Other..."));
}
#endif

#if qStroika_Foundation_Common_Platform_Windows
void StdColorPopupHelper::AppendMenuString (const SDKString& s)
{
#if qStroika_Foundation_Common_Platform_Windows
    Require (::IsWindow (fHWnd));
    Verify (::SendMessage (fHWnd, CB_ADDSTRING, 0, reinterpret_cast<LPARAM> (s.c_str ())) != CB_ERR);
#endif
}
#endif

#if qSupportLedDialogWidgets
/*
 ********************************************************************************
 ********************************* LedDialogWidget ******************************
 ********************************************************************************
 */
LedDialogWidget::LedDialogWidget ()
    : inherited ()
    , fTextStore ()
{
    SetDefaultWindowMargins (TWIPS_Rect (TWIPS::kPoint, TWIPS::kPoint, TWIPS::kPoint, TWIPS::kPoint));
    SpecifyTextStore (&fTextStore);
    SetCommandHandler (&fCommandHandler);
}

LedDialogWidget::LedDialogWidget (TS_SET_OUTSIDE_BWA)
    : inherited ()
    , fTextStore ()
{
    SetDefaultWindowMargins (TWIPS_Rect (TWIPS::kPoint, TWIPS::kPoint, TWIPS::kPoint, TWIPS::kPoint));
    SetCommandHandler (&fCommandHandler);
}

LedDialogWidget::~LedDialogWidget ()
{
    SetCommandHandler (NULL);
    SpecifyTextStore (NULL);
}

/*
@METHOD:        LedDialogWidget::OnBadUserInput
@DESCRIPTION:   <p>Override @'TextInteractor::OnBadUserInputn' - to just beep - not throw (cuz a throw messes up dialogs).</p>
*/
void LedDialogWidget::OnBadUserInput ()
{
    Led_BeepNotify ();
}

void LedDialogWidget::OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed,
                                              bool controlPressed, bool altKeyPressed)
{
    CommandNumber c = CharToCommand (theChar);
    if (c == 0) {
        inherited::OnTypedNormalCharacter (theChar, optionPressed, shiftPressed, commandPressed, controlPressed, altKeyPressed);
    }
    else {
        OnPerformCommand (c);
    }
}

LedDialogWidget::CommandNumber LedDialogWidget::CharToCommand (Led_tChar theChar) const
{
#define CNTRL(x) (x - 'a' + 1)
    switch (theChar) {
        case CNTRL ('a'):
            return kSelectAll_CmdID;
        case CNTRL ('c'):
            return kCopy_CmdID;
        case CNTRL ('v'):
            return kPaste_CmdID;
        case CNTRL ('x'):
            return kCut_CmdID;
        case CNTRL ('z'):
            return kUndo_CmdID;
        default:
            return 0;
    }
#undef CNTRL
}

Led_tString LedDialogWidget::GetText () const
{
    size_t                 len = GetLength ();
    StackBuffer<Led_tChar> buf{Memory::eUninitialized, len + 1};
    CopyOut (0, len, buf.data ());
    buf[len]                    = '\0';
    size_t                 len2 = 2 * len + 1;
    StackBuffer<Led_tChar> buf2{Memory::eUninitialized, len2};
    len2       = Characters::NLToNative<Led_tChar> (buf.data (), len, buf2.data (), len2);
    buf2[len2] = '\0';
    return Led_tString{buf2.data ()};
}

void LedDialogWidget::SetText (const Led_tString& t)
{
    size_t                 len = t.length ();
    StackBuffer<Led_tChar> buf{Memory::eUninitialized, len};
    len = Characters::NormalizeTextToNL<Led_tChar> (t.c_str (), len, buf.data (), len);
    Replace (0, GetEnd (), buf.data (), len);
}
#endif

#if qSupportLedDialogWidgets
/*
 ********************************************************************************
 ************************** LedComboBoxWidget::MyButton *************************
 ********************************************************************************
 */
LedComboBoxWidget::MyButton::MyButton ()
    : fComboBox (NULL)
    , fDropDownArrow ()
{
#if qStroika_Foundation_Common_Platform_Windows
// In case compiled without #define OEMRESOURCE
#ifndef OBM_COMBO
#define OBM_COMBO 32738
#endif
    fDropDownArrow.LoadBitmap (NULL, MAKEINTRESOURCE (OBM_COMBO));
#endif
}

#if qStroika_Foundation_Common_Platform_Windows
LRESULT LedComboBoxWidget::MyButton::WndProc (UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_SETFOCUS: {
            AssertNotNull (fComboBox);
            ::SetFocus (fComboBox->fTextWidget.GetHWND ());
            return 0;
        }
        case WM_LBUTTONDOWN: {
            SendMessage (BM_SETSTYLE, BS_BITMAP, true); // for some reason - clicking on this button sets the BS_DEFPUSHBUTTON style - which looks bad,
            // and is even worse for the fact that its only set after the first click on the button.
            // Just unset it... LGP 2003-12-28
            AssertNotNull (fComboBox);
            fComboBox->TogglePopupShown ();
        }
        default:
            return inherited::WndProc (message, wParam, lParam);
    }
}
#endif

#endif

#if qSupportLedDialogWidgets
/*
 ********************************************************************************
 ****************** LedComboBoxWidget::MyComboListBoxPopup **********************
 ********************************************************************************
 */

LedComboBoxWidget::MyComboListBoxPopup::MyComboListBoxPopup ()
    : fComboBox (NULL)
{
}

#if qStroika_Foundation_Common_Platform_Windows
LRESULT LedComboBoxWidget::MyComboListBoxPopup::WndProc (UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_CHAR: {
            if (wParam == '\t') {
                SendMessage (WM_NEXTDLGCTL, !!(::GetKeyState (VK_SHIFT) & 0x8000), false);
                return 0; // return zero to indicate processed the message
            }
            else if (wParam == VK_RETURN) {
                MadeSelection ();
                return 0; // to indicate proceessed the message
            }
            else {
                return inherited::WndProc (message, wParam, lParam);
            }
        }
        case WM_MOUSEMOVE: {
            /*
             *  As the mosue moves over the control - automatically select the item (like a menu behaivor)
             */
            LRESULT dw = SendMessage (LB_ITEMFROMPOINT, 0, lParam);
            if (not HIWORD (dw)) {
                int pos = LOWORD (dw);
                Verify (SendMessage (LB_SETCURSEL, pos, 0) != LB_ERR);
            }
            return 0; // to indicate proceessed the message
        }
        case WM_LBUTTONDOWN: {
            /*
                 *  Treat a click as a selection and close the popup.
                 */
            Led_Rect clientRect;
            {
                RECT cr;
                ::GetClientRect (GetHWND (), &cr);
                clientRect = AsLedRect (cr);
            }

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lParam) ((int)(short)LOWORD (lParam))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lParam) ((int)(short)HIWORD (lParam))
#endif
            if (clientRect.Contains (Led_Point (GET_Y_LPARAM (lParam), GET_X_LPARAM (lParam)))) {
                MadeSelection ();
            }
            AssertNotNull (fComboBox);
            fComboBox->HidePopup ();
            return 0; // to indicate proceessed the message
        }
        case WM_KILLFOCUS: {
            LRESULT lr = inherited::WndProc (message, wParam, lParam);
            AssertNotNull (fComboBox);
            fComboBox->HidePopup ();
            return lr;
        }
        default:
            return inherited::WndProc (message, wParam, lParam);
    }
}
#endif

void LedComboBoxWidget::MyComboListBoxPopup::UpdatePopupItems ()
{
    RequireNotNull (fComboBox);
    if (IsWindowRealized ()) {
        SendMessage (LB_RESETCONTENT, 0, 0);
        bool usingUNICODEWnd = IsWindowUNICODE ();
        for (auto i = fComboBox->fPopupItems.begin (); i != fComboBox->fPopupItems.end (); ++i) {
            if (usingUNICODEWnd) {
                SendMessage (LB_ADDSTRING, 0, reinterpret_cast<LPARAM> (Led_tString2WideString (*i).c_str ()));
            }
            else {
                SendMessage (LB_ADDSTRING, 0, reinterpret_cast<LPARAM> (Led_tString2SDKString (*i).c_str ()));
            }
        }
    }
}

void LedComboBoxWidget::MyComboListBoxPopup::MadeSelection ()
{
    RequireNotNull (fComboBox);
    LRESULT curSelRel = SendMessage (LB_GETCURSEL, 0, 0);
    if (curSelRel != LB_ERR) {
        size_t curSel = static_cast<size_t> (curSelRel);
        if (curSel < fComboBox->fPopupItems.size ()) {
            fComboBox->fTextWidget.SetText (fComboBox->fPopupItems[curSel]);
            fComboBox->SendMessage (EM_SETSEL, 0, -1);
        }
    }
    fComboBox->HidePopup ();
}

void LedComboBoxWidget::MyComboListBoxPopup::ComputePreferedHeight (DistanceType* prefHeight, size_t* nEltsShown) const
{
    RequireNotNull (prefHeight);
    RequireNotNull (nEltsShown);
    // Should tune this code to take into account popup position, and height of screen (so not offscreen)
    const size_t kMaxElts = 10; // don't show more than this - use scrollbar...
    RequireNotNull (fComboBox);
    size_t nElts = fComboBox->fPopupItems.size ();
    if (nElts == 0) {
        nElts = 1;
    }
    if (nElts >= kMaxElts) {
        nElts = kMaxElts;
    }
    DistanceType           singleEltHeight  = static_cast<DistanceType> (::SendMessage (GetHWND (), LB_GETITEMHEIGHT, 0, 0));
    constexpr DistanceType kEmpiricalKludge = 2; // LGP 2003-12-28
    *prefHeight                             = static_cast<DistanceType> (nElts * singleEltHeight + kEmpiricalKludge);
    *nEltsShown                             = nElts;
}
#endif

#if qSupportLedDialogWidgets
/*
 ********************************************************************************
 *********************** LedComboBoxWidget::MyTextWidget ************************
 ********************************************************************************
 */
LedComboBoxWidget::MyTextWidget::MyTextWidget ()
    : inherited (eTS_SET_OUTSIDE_BWA)
    , fComboBox (NULL)
{
    SpecifyTextStore (&fTextStore);
}

LedComboBoxWidget::MyTextWidget::~MyTextWidget ()
{
    SpecifyTextStore (NULL);
}

#if qStroika_Foundation_Common_Platform_Windows
LRESULT LedComboBoxWidget::MyTextWidget::WndProc (UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_KEYDOWN: {
            AssertNotNull (fComboBox);
            if (wParam == VK_UP or wParam == VK_DOWN) {
                fComboBox->ShowPopup ();
                return fComboBox->fComboListBoxPopup.SendMessage (message, wParam, lParam);
            }
            return inherited::WndProc (message, wParam, lParam);
        }
        case WM_CHAR: {
            AssertNotNull (fComboBox);
            // Not called - unfortunately - with MFC - cuz the PreTranslateMessage () maps the ESC key to a CANCEL message
            // before the event makes it here - LGP 2003-12-27
            if (wParam == VK_ESCAPE) {
                fComboBox->HidePopup ();
                return 0; // to indicate message processed
            }
            return inherited::WndProc (message, wParam, lParam);
        }
        default:
            return inherited::WndProc (message, wParam, lParam);
    }
}
#endif
#endif

#if qSupportLedDialogWidgets
/*
 ********************************************************************************
 ******************************* LedComboBoxWidget ******************************
 ********************************************************************************
 */
LedComboBoxWidget::LedComboBoxWidget ()
    : fPopupButton ()
    , fComboListBoxPopup ()
    , fTextWidget ()
    , fUseWidgetFont (NULL)
{
    fPopupButton.fComboBox       = this;
    fComboListBoxPopup.fComboBox = this;
    fTextWidget.fComboBox        = this;
}

LedComboBoxWidget::~LedComboBoxWidget ()
{
    delete fUseWidgetFont;
}

#if qStroika_Foundation_Common_Platform_Windows
bool LedComboBoxWidget::ReplaceWindow (HWND hWnd)
{
    Require (GetHWND () == NULL); // don't call after already created! - use this instead of SetHWnd ()!!!
    RequireNotNull (hWnd);
    Require (::IsWindow (hWnd));

    HWND parent = ::GetParent (hWnd);
    int  id     = ::GetWindowLong (hWnd, GWL_ID);
    Assert (hWnd == ::GetDlgItem (parent, id));

    DWORD dwStyle = ::GetWindowLong (hWnd, GWL_STYLE);
    DWORD exStyle = ::GetWindowLong (hWnd, GWL_EXSTYLE);

    // Assume previous widget's position.
    WINDOWPLACEMENT wp;
    (void)::memset (&wp, 0, sizeof (wp));
    wp.length = sizeof (wp);
    Verify (::GetWindowPlacement (hWnd, &wp));

    // Save (clone since we destory HWND before we use the set it into the new object) HFONT object,
    // so we can set it into our new control at the end of this procedure.
    bool       useNonSysFont = false;
    FontObject nonSysFont;
    {
        HFONT hFont = NULL;
        if ((hFont = (HFONT)::SendMessage (hWnd, WM_GETFONT, 0, 0L)) != NULL) {
            LOGFONT useFont;
            if (::GetObject (hFont, sizeof (LOGFONT), &useFont) != 0) {
                Verify (nonSysFont.CreateFontIndirect (&useFont));
                useNonSysFont = true;
            }
        }
    }

    // Delete the old widget window.
    ::DestroyWindow (hWnd);

    DISABLE_COMPILER_MSC_WARNING_START (4312)
    Create (exStyle, NULL, NULL, dwStyle | WS_TABSTOP | WS_CHILD, wp.rcNormalPosition.left, wp.rcNormalPosition.top,
            wp.rcNormalPosition.right - wp.rcNormalPosition.left, wp.rcNormalPosition.bottom - wp.rcNormalPosition.top, parent, (HMENU)id, NULL);
    DISABLE_COMPILER_MSC_WARNING_END (4312)

    /*
     *  Copy the font value from the original widget and set it into the replaced one.
     */
    (void)SendMessage (WM_SETFONT, useNonSysFont ? reinterpret_cast<WPARAM> (static_cast<HFONT> (nonSysFont)) : NULL, false);
    return true;
}
#endif

Led_tString LedComboBoxWidget::GetText () const
{
    return fTextWidget.GetText ();
}

void LedComboBoxWidget::SetText (const Led_tString& t)
{
    fTextWidget.SetText (t);
}

void LedComboBoxWidget::SetPopupItems (const vector<Led_tString>& pi)
{
    fPopupItems = pi;
    fComboListBoxPopup.UpdatePopupItems ();
}

#if qStroika_Foundation_Common_Platform_Windows
LRESULT LedComboBoxWidget::WndProc (UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_SETFOCUS: {
            (void)::SetFocus (fTextWidget.GetHWND ());
            return 0;
        }
        case WM_GETFONT: {
            return fUseWidgetFont == NULL ? NULL : reinterpret_cast<LRESULT> (static_cast<HFONT> (*fUseWidgetFont));
        }
        case WM_SETFONT: {
            delete fUseWidgetFont;
            fUseWidgetFont = NULL;

            HFONT copyFont = reinterpret_cast<HFONT> (wParam);
            if (copyFont != NULL) {
                LOGFONT useFont;
                if (::GetObject (copyFont, sizeof (LOGFONT), &useFont) != 0) {
                    Assert (fUseWidgetFont == NULL);
                    fUseWidgetFont = new FontObject ();
                    Verify (fUseWidgetFont->CreateFontIndirect (&useFont));
                }
            }

            HFONT fontToPassDown = fUseWidgetFont == NULL ? NULL : static_cast<HFONT> (*fUseWidgetFont);
            (void)fPopupButton.SendMessage (WM_SETFONT, reinterpret_cast<WPARAM> (fontToPassDown), lParam);
            (void)::SendMessage (fTextWidget.GetHWND (), WM_SETFONT, reinterpret_cast<WPARAM> (fontToPassDown), lParam);
            (void)fComboListBoxPopup.SendMessage (WM_SETFONT, reinterpret_cast<WPARAM> (fontToPassDown), lParam);
            return 0;
        }
        case WM_CREATE: {
            return OnCreate_Msg (wParam, lParam);
        }
        case WM_SIZE: {
            return OnSize_Msg (wParam, lParam);
        }
        // Forward these to the text widget only
        case EM_GETSEL:
        case EM_SETSEL:
        case WM_GETDLGCODE:
        case WM_GETTEXT:
        case WM_SETTEXT:
        case WM_GETTEXTLENGTH: {
            return ::SendMessage (fTextWidget.GetHWND (), message, wParam, lParam);
        }
        // Forward these to all widgets
        case WM_ENABLE: {
            LRESULT lr = inherited::WndProc (message, wParam, lParam);
            (void)fPopupButton.SendMessage (message, wParam, lParam);
            (void)::SendMessage (fTextWidget.GetHWND (), message, wParam, lParam);
            (void)fComboListBoxPopup.SendMessage (message, wParam, lParam);
            return lr;
        }
        default:
            return inherited::WndProc (message, wParam, lParam);
    }
}

LRESULT LedComboBoxWidget::OnCreate_Msg (WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = inherited::WndProc (WM_CREATE, wParam, lParam);
    if (lr == 0) {
        int   id      = ::GetWindowLong (GetHWND (), GWL_ID);
        DWORD dwStyle = ::GetWindowLong (GetHWND (), GWL_STYLE);

        // Shrink it slightly to make room for the popup control
        DISABLE_COMPILER_MSC_WARNING_START (4312)
        fTextWidget.Create (0, NULL, NULL, dwStyle | WS_VISIBLE | WS_CHILD | WS_TABSTOP, 0, 0, 0, 0, GetHWND (), (HMENU)id, NULL);
        DISABLE_COMPILER_MSC_WARNING_END (4312)

        // Next - create the POPUP BUTTON
        fPopupButton.SubclassWindow (
            ::CreateWindowEx (0, _T("BUTTON"), _T (""), WS_CHILD | WS_VISIBLE | BS_BITMAP, 0, 0, 0, 0, GetHWND (), NULL, NULL, NULL));
        fPopupButton.SendMessage (BM_SETIMAGE, static_cast<WPARAM> (IMAGE_BITMAP),
                                  reinterpret_cast<LPARAM> (static_cast<HBITMAP> (fPopupButton.fDropDownArrow)));

        // Next - create the POPUP Listbox (hidden initially)
        {
#if !qTargetPlatformSDKUseswchar_t
            // Try creating a UNICODE popup window - even if we aren't building a UNICODE app. This is OK - since this window isn't
            // handled directly through MFC (which doesn't support UNICODE windows in non-UNICODE apps, but just through
            // Led's code directly - which does support this - LGP 2003-12-29
            HWND uniWnd = ::CreateWindowExW (WS_EX_TOPMOST, L"ListBox", NULL, WS_POPUP | LBS_NOTIFY | WS_BORDER | WS_TABSTOP, 0, 0, 0, 0,
                                             GetHWND (), NULL, NULL, NULL);
            if (uniWnd != NULL) {
                fComboListBoxPopup.SubclassWindow (uniWnd);
                goto alreadyCreated;
            }
#endif
            fComboListBoxPopup.SubclassWindow (::CreateWindowEx (WS_EX_TOPMOST, _T("ListBox"), NULL, WS_POPUP | LBS_NOTIFY | WS_BORDER | WS_TABSTOP,
                                                                 0, 0, 0, 0, GetHWND (), NULL, NULL, NULL));
#if !qTargetPlatformSDKUseswchar_t
        alreadyCreated:;
#endif
        }
        fComboListBoxPopup.UpdatePopupItems ();
    }
    return lr;
}

LRESULT LedComboBoxWidget::OnSize_Msg (WPARAM wParam, LPARAM lParam)
{
    const DistanceType kPopupIconWidth = fPopupButton.fDropDownArrow.GetImageSize ().h + 4;

    Led_Rect clientRect;
    {
        RECT cr;
        ::GetClientRect (GetHWND (), &cr);
        clientRect = AsLedRect (cr);
    }

    // Shrink it slightly to make room for the popup control
    Verify (::MoveWindow (fTextWidget.GetHWND (), clientRect.left, clientRect.top, clientRect.GetWidth () - kPopupIconWidth,
                          clientRect.GetHeight (), true));
    // Next - the POPUP BUTTON
    Verify (::MoveWindow (fPopupButton.GetHWND (), clientRect.right - kPopupIconWidth, clientRect.top, kPopupIconWidth, clientRect.GetHeight (), true));
    return inherited::WndProc (WM_SIZE, wParam, lParam);
}
#endif

void LedComboBoxWidget::ShowPopup ()
{
    if (not PopupShown ()) {
        // Set initial value based on current combo box text
        vector<Led_tString>::const_iterator i = find (fPopupItems.begin (), fPopupItems.end (), GetText ());
        if (i == fPopupItems.end ()) {
            fComboListBoxPopup.SendMessage (LB_SETCURSEL, static_cast<WPARAM> (-1), 0); // none
        }
        else {
            fComboListBoxPopup.SendMessage (LB_SETCURSEL, static_cast<WPARAM> (i - fPopupItems.begin ()), 0);
        }
    }

    // Must place the window each time popped up - since the parent could have been moved (esp in global
    // coordinates)
    {
        POINT zero; // convert popup position to screen coordinates...
        (void)::memset (&zero, 0, sizeof (zero));
        Verify (::ClientToScreen (::GetParent (GetHWND ()), &zero));
        Led_Rect clientRect;
        {
            RECT cr;
            ::GetClientRect (GetHWND (), &cr);
            clientRect = AsLedRect (cr);
        }
        DistanceType prefHeight = 0;
        size_t       nEltsShown = 0;
        fComboListBoxPopup.ComputePreferedHeight (&prefHeight, &nEltsShown);

        if (nEltsShown >= fPopupItems.size ()) {
            ::SetWindowLong (fComboListBoxPopup.GetHWND (), GWL_STYLE, ::GetWindowLong (fComboListBoxPopup.GetHWND (), GWL_STYLE) & ~WS_VSCROLL);
        }
        else {
            ::SetWindowLong (fComboListBoxPopup.GetHWND (), GWL_STYLE, ::GetWindowLong (fComboListBoxPopup.GetHWND (), GWL_STYLE) | WS_VSCROLL);
        }

        WINDOWPLACEMENT wp;
        (void)::memset (&wp, 0, sizeof (wp));
        wp.length = sizeof (wp);
        Verify (::GetWindowPlacement (GetHWND (), &wp));
        Verify (::MoveWindow (fComboListBoxPopup.GetHWND (), wp.rcNormalPosition.left + zero.x, wp.rcNormalPosition.bottom + zero.y,
                              clientRect.GetWidth (), prefHeight, false));
    }

    /*
     *  Subtle points. We show the window with SW_SHOWNOACTIVEATE so the combobox text
     *  box is still shown activated and so the parent dialog is not deactivated.
     *  Note that thats a logical requiement - just what MS Windows does.
     *
     *  Also - we do a SetCapture (which we release in HidePopup) so that we can catch any
     *  mouse clicks outside of our window - and hide the popup in that case. Again - this is really
     *  just mimicking the MS Windows UI behavior.
     */
    (void)::ShowWindow (fComboListBoxPopup.GetHWND (), SW_SHOWNOACTIVATE);
    (void)::SetCapture (fComboListBoxPopup.GetHWND ());
}

void LedComboBoxWidget::HidePopup ()
{
    if (::GetCapture () == fComboListBoxPopup.GetHWND ()) {
        Verify (::ReleaseCapture ());
    }
    fComboListBoxPopup.SetWindowVisible (false);
}

bool LedComboBoxWidget::PopupShown () const
{
    return fComboListBoxPopup.IsWindowShown ();
}

void LedComboBoxWidget::TogglePopupShown ()
{
    if (PopupShown ()) {
        HidePopup ();
    }
    else {
        ShowPopup ();
    }
}
#endif

/*
 ********************************************************************************
 ********************************* Led_StdDialogHelper **************************
 ********************************************************************************
 */
#if qStroika_Foundation_Common_Platform_Windows
Led_StdDialogHelper::Led_StdDialogHelper (HINSTANCE hInstance, const Characters::SDKChar* resID, HWND parentWnd)
    : fSetFocusItemCalled{false}
{
    fHWnd      = NULL;
    fHINSTANCE = hInstance;
    fResID     = resID;
    fParentWnd = parentWnd;
    fWasOK     = false;
}
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
Led_StdDialogHelper::Led_StdDialogHelper (GtkWindow* parentWindow)
    : fWindow{NULL}
    , fParentWindow{parentWindow}
    , fOKButton{NULL}
    , fCancelButton{NULL}
    , fWasOK{false}
{
}
#endif

Led_StdDialogHelper::~Led_StdDialogHelper ()
{
#if qStroika_Foundation_Common_Platform_Windows
    if (GetHWND () != NULL) {
        // maybe did a throw out of the scope where this object was declared? Anyhow - blow away the window!
        ::DestroyWindow (GetHWND ());
    }
#endif
}

bool Led_StdDialogHelper::GetWasOK () const
{
    return fWasOK;
}

bool Led_StdDialogHelper::DoModal ()
{
#if qStroika_Foundation_Common_Platform_Windows
    HWND oldFocusWnd = ::GetFocus ();
#if qNO_INT_PTR_DefinedCompilerBug
    using INT_PTR = int;
#endif
    [[maybe_unused]] INT_PTR x =
        ::DialogBoxParam (fHINSTANCE, fResID, fParentWnd, reinterpret_cast<DLGPROC> (StaticDialogProc), reinterpret_cast<LPARAM> (this));
    if (oldFocusWnd != NULL) {
        ::SetFocus (oldFocusWnd);
    }
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
    /* create the main window, and attach delete_event signal to terminating
    the application */
    GtkWidget* window = MakeWindow ();
    SetWindow (window);

    PreDoModalHook ();
    gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (fParentWindow));
    gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER_ALWAYS); // or GTK_WIN_POS_CENTER?
    gtk_window_set_modal (GTK_WINDOW (window), true);
    gtk_widget_show (window);

    gtk_signal_connect_after (GTK_OBJECT (window), "delete_event", GTK_SIGNAL_FUNC (Static_OnWindowDeleteRequest), (gpointer)this);

    if (fOKButton != NULL) {
        gtk_signal_connect (GTK_OBJECT (fOKButton), "clicked", GTK_SIGNAL_FUNC (Static_OnOKButtonClick), (gpointer)this);
        gtk_widget_show (fOKButton);
    }
    if (fCancelButton != NULL) {
        gtk_signal_connect (GTK_OBJECT (fCancelButton), "clicked", GTK_SIGNAL_FUNC (Static_OnCancelButtonClick), (gpointer)this);
        gtk_widget_show (fCancelButton);
    }
    gtk_main ();
    gtk_widget_destroy (window);
#endif
    return GetWasOK ();
}

void Led_StdDialogHelper::ReplaceAllTokens (SDKString* m, const SDKString& token, const SDKString& with)
{
    using Characters::String;
    RequireNotNull (m);
    *m = String::FromSDKString (*m).ReplaceAll (String::FromSDKString (token), String::FromSDKString (with)).AsSDKString ();
}

void Led_StdDialogHelper::PreDoModalHook ()
{
}

#if qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
GtkWidget* Led_StdDialogHelper::MakeWindow ()
{
    //  return gtk_window_new (GTK_WINDOW_TOPLEVEL);
    return gtk_dialog_new ();
}
#endif

#if qStroika_Foundation_Common_Platform_Windows
BOOL Led_StdDialogHelper::OnInitDialog ()
{
    Led_CenterWindowInParent (GetHWND ());
    PreDoModalHook ();
    return fSetFocusItemCalled ? false : true; // let windows set input focus if we have not
}
#endif

#if qStroika_Foundation_Common_Platform_Windows
BOOL CALLBACK Led_StdDialogHelper::StaticDialogProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_INITDIALOG) {
        [[maybe_unused]] LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
        AssertNotNull (lpcs);
        Led_StdDialogHelper* pThis = reinterpret_cast<Led_StdDialogHelper*> (lParam);
        Assert (pThis->GetHWND () == NULL); // cuz not set yet...
        pThis->SetHWND (hWnd);

        return pThis->OnInitDialog ();
    }

    Led_StdDialogHelper* pThis = reinterpret_cast<Led_StdDialogHelper*> (::GetWindowLongPtr (hWnd, GWLP_USERDATA));

    if (pThis == NULL) {
        return false;
    }

    if (message == WM_DESTROY) {
        pThis->SetHWND (NULL); // should maybe use OnDestroy () method???
        return true;
    }

    Assert (pThis != NULL);
    Assert (pThis->GetHWND () == hWnd);
    return pThis->DialogProc (message, wParam, lParam);
}

BOOL Led_StdDialogHelper::DialogProc (UINT message, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam)
{
    switch (message) {
        case WM_COMMAND: {
            switch (wParam) {
                case IDOK: {
                    OnOK ();
                    return true;
                } break;
                case IDCANCEL: {
                    OnCancel ();
                    return true;
                } break;
            }
        } break;
    }
    return false;
}
#endif

#if qStroika_Foundation_Common_Platform_Windows || (qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs)
SDKString Led_StdDialogHelper::GetItemText (DialogItemID itemID) const
{
#if qStroika_Foundation_Common_Platform_Windows
    Characters::SDKChar widgetText[2 * 1024]; // sb big enough for the most part???
    (void)::GetDlgItemText (GetHWND (), itemID, widgetText, static_cast<UINT> (Memory::NEltsOf (widgetText)));
    return widgetText;
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
    return (char*)gtk_entry_get_text (GTK_ENTRY (itemID)); // gtk returns internal pointer - DON'T FREE
#endif
}

void Led_StdDialogHelper::SetItemText (DialogItemID itemID, const SDKString& text)
{
#if qStroika_Foundation_Common_Platform_Windows
    (void)::SetDlgItemText (GetHWND (), itemID, text.c_str ());
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
    gtk_entry_set_text (GTK_ENTRY (itemID), text.c_str ());
#endif
}

void Led_StdDialogHelper::SelectItemText (DialogItemID itemID, size_t from, size_t to)
{
#if qStroika_Foundation_Common_Platform_Windows
    ::SendDlgItemMessage (GetHWND (), itemID, EM_SETSEL, from, to);
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
    gtk_entry_select_region (GTK_ENTRY (itemID), from, to);
#endif
}

bool Led_StdDialogHelper::GetItemChecked (DialogItemID itemID) const
{
#if qStroika_Foundation_Common_Platform_Windows
    return !!::IsDlgButtonChecked (GetHWND (), itemID);
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
    Assert (false); //NYI
    return false;
#endif
}

void Led_StdDialogHelper::SetItemChecked (DialogItemID itemID, bool checked)
{
#if qStroika_Foundation_Common_Platform_Windows
    ::CheckDlgButton (GetHWND (), itemID, checked);
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
    //gtk_entry_set_text (GTK_ENTRY (itemID), text.c_str ());
    Assert (false); //NYI
#endif
}

bool Led_StdDialogHelper::GetItemEnabled (DialogItemID itemID) const
{
#if qStroika_Foundation_Common_Platform_Windows
    return !!::IsWindowEnabled (GetDlgItem (GetHWND (), itemID));
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs && 0
    Assert (false); //NYI
    return false;
#else
    Assert (false);
    return true; // by default - if NYI...
#endif
}

void Led_StdDialogHelper::SetItemEnabled (DialogItemID itemID, bool enabled)
{
#if qStroika_Foundation_Common_Platform_Windows
    ::EnableWindow (GetDlgItem (GetHWND (), itemID), enabled);
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
    //gtk_entry_set_text (GTK_ENTRY (itemID), text.c_str ());
    Assert (false); //NYI
#endif
}

void Led_StdDialogHelper::SetFocusedItem (DialogItemID itemID)
{
#if qStroika_Foundation_Common_Platform_Windows
    HWND dlgItem = ::GetDlgItem (GetHWND (), itemID);
    Assert (dlgItem != NULL);
    ::SetFocus (dlgItem);
    fSetFocusItemCalled = true;
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
    gtk_widget_grab_focus (itemID);
#endif
}
#endif

#if qStroika_Foundation_Common_Platform_Windows
void Led_StdDialogHelper::SetHWND (HWND hWnd)
{
    if (fHWnd != NULL) {
        ::SetWindowLongPtr (fHWnd, GWLP_USERDATA, 0); // reset back to original value
    }
    fHWnd = hWnd;
    if (fHWnd != NULL) {
        ::SetWindowLongPtr (fHWnd, GWLP_USERDATA, reinterpret_cast<DWORD_PTR> (this));
    }
}

HWND Led_StdDialogHelper::GetHWND () const
{
    return fHWnd;
}
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
GtkWidget* Led_StdDialogHelper::GetWindow () const
{
    return fWindow;
}

void Led_StdDialogHelper::SetWindow (GtkWidget* w)
{
    if (fWindow != w) {
        if (fWindow != NULL) {
            gtk_object_set_user_data (GTK_OBJECT (fWindow), NULL);
        }
        fWindow = w;
        if (fWindow != NULL) {
            gtk_object_set_user_data (GTK_OBJECT (fWindow), this);
        }
    }
}
GtkWidget* Led_StdDialogHelper::GetOKButton () const
{
    return fOKButton;
}
void Led_StdDialogHelper::SetOKButton (GtkWidget* okButton)
{
    fOKButton = okButton;
}

GtkWidget* Led_StdDialogHelper::GetCancelButton () const
{
    return fWindow;
}
void Led_StdDialogHelper::SetCancelButton (GtkWidget* cancelButton)
{
    fCancelButton = cancelButton;
}
#endif

void Led_StdDialogHelper::OnOK ()
{
    fWasOK = true;
#if qStroika_Foundation_Common_Platform_Windows
    ::EndDialog (GetHWND (), IDOK);
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
    gtk_main_quit ();
#endif
}

void Led_StdDialogHelper::OnCancel ()
{
#if qStroika_Foundation_Common_Platform_Windows
    ::EndDialog (GetHWND (), IDCANCEL);
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
    gtk_main_quit ();
#endif
}

#if qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
void Led_StdDialogHelper::Static_OnOKButtonClick (GtkWidget* widget, gpointer data)
{
    Led_StdDialogHelper* dlg = reinterpret_cast<Led_StdDialogHelper*> (data);
    dlg->OnOK ();
}

void Led_StdDialogHelper::Static_OnCancelButtonClick (GtkWidget* widget, gpointer data)
{
    Led_StdDialogHelper* dlg = reinterpret_cast<Led_StdDialogHelper*> (data);
    dlg->OnCancel ();
}

void Led_StdDialogHelper::Static_OnWindowDeleteRequest (GtkWidget* widget)
{
    Led_StdDialogHelper* dlg = reinterpret_cast<Led_StdDialogHelper*> (gtk_object_get_user_data (GTK_OBJECT (widget)));
    Assert (dlg->fWindow == widget);
    dlg->OnCancel ();
}
#endif

/*
 ********************************************************************************
 *************************** Led_StdDialogHelper_AboutBox ***********************
 ********************************************************************************
 */
#if qStroika_Foundation_Common_Platform_Windows
Led_StdDialogHelper_AboutBox::Led_StdDialogHelper_AboutBox (HINSTANCE hInstance, HWND parentWnd, const Characters::SDKChar* resID)
    : inherited (hInstance, resID, parentWnd)
{
}
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
Led_StdDialogHelper_AboutBox::Led_StdDialogHelper_AboutBox (GtkWindow* parentWindow)
    : inherited (parentWindow)
{
}
#endif

#if qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
GtkWidget* Led_StdDialogHelper_AboutBox::MakeWindow ()
{
    return gtk_window_new (GTK_WINDOW_TOPLEVEL);
}
#endif

#if qStroika_Foundation_Common_Platform_Windows
BOOL Led_StdDialogHelper_AboutBox::DialogProc (UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_COMMAND: {
            switch (wParam) {
                case kLedStdDlg_AboutBox_InfoLedFieldID:
                    OnClickInInfoField ();
                    return true;
                case kLedStdDlg_AboutBox_LedWebPageFieldID:
                    OnClickInLedWebPageField ();
                    return true;
            }
        }
    }
    return inherited::DialogProc (message, wParam, lParam);
}
#endif

#if qSupportStdAboutBoxDlg
void Led_StdDialogHelper_AboutBox::OnClickInInfoField ()
{
    OnOK ();
}

void Led_StdDialogHelper_AboutBox::OnClickInLedWebPageField ()
{
    OnOK ();
}
#endif

#if qSupportStdFindDlg
/*
 ********************************************************************************
 *************************** Led_StdDialogHelper_FindDialog *********************
 ********************************************************************************
 */

#if qStroika_Foundation_Common_Platform_Windows
Led_StdDialogHelper_FindDialog::Led_StdDialogHelper_FindDialog (HINSTANCE hInstance, HWND parentWnd, const Characters::SDKChar* resID)
    : inherited (hInstance, resID, parentWnd)
    , fFindText ()
    , fRecentFindTextStrings ()
    , fWrapSearch (false)
    , fWholeWordSearch (false)
    , fCaseSensativeSearch (false)
    , fPressedOK (false)
#if qSupportLedDialogWidgets
    , fFindTextWidget ()
#endif
{
}
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
Led_StdDialogHelper_FindDialog::Led_StdDialogHelper_FindDialog (GtkWindow* parentWindow)
    : inherited (parentWindow)
    , fFindText ()
    , fRecentFindTextStrings ()
    , fWrapSearch (false)
    , fWholeWordSearch (false)
    , fCaseSensativeSearch (false)
    , fPressedOK (false)
    , fLookupTextWidget (NULL)
{
}
#endif

#if qStroika_Foundation_Common_Platform_Windows
BOOL Led_StdDialogHelper_FindDialog::DialogProc (UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_COMMAND: {
            switch (wParam) {
                case kLedStdDlg_FindBox_Find:
                    OnFindButton ();
                    return true;
                case kLedStdDlg_FindBox_Cancel:
                    OnDontFindButton ();
                    return true;
            }
        }
    }
    return inherited::DialogProc (message, wParam, lParam);
}
#endif

void Led_StdDialogHelper_FindDialog::PreDoModalHook ()
{
#if qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
    GtkWidget* actionArea = GTK_DIALOG (GetWindow ())->action_area;
    {
        fLookupTextWidget = gtk_entry_new ();
        gtk_container_add (GTK_CONTAINER (actionArea), fLookupTextWidget);
        gtk_widget_show (fLookupTextWidget);
        //          gtk_entry_set_text (GTK_ENTRY (fLookupTextWidget), Led_tString2SDKString (fFindText).c_str ());
        //          gtk_entry_select_region (GTK_ENTRY (fLookupTextWidget), 0, -1);
        //          gtk_widget_grab_focus (fLookupTextWidget);
    }
    {
        GtkWidget* button = gtk_button_new_with_label ("Find");
        gtk_container_add (GTK_CONTAINER (actionArea), button);
        GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
        gtk_widget_show (button);
        gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (Static_OnFindButtonClick), (gpointer)this);
    }
    {
        GtkWidget* button = gtk_button_new_with_label ("Close");
        gtk_container_add (GTK_CONTAINER (actionArea), button);
        GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
        gtk_widget_show (button);
        gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (Static_OnDontFindButtonClick), (gpointer)this);
    }
#endif

#if qStroika_Foundation_Common_Platform_Windows && qSupportLedDialogWidgets
    /*
     *  ReplaceWindow seems to work better than SubclassWindow - for reasons I don't FULLY understand.
     *  (see SPR#1266).
     */
    fFindTextWidget.ReplaceWindow (::GetDlgItem (GetHWND (), kLedStdDlg_FindBox_FindText));
#endif

#if qStroika_Foundation_Common_Platform_Windows
    DialogItemID findText = kLedStdDlg_FindBox_FindText;
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
    DialogItemID findText = fLookupTextWidget;
#endif

#if qSupportLedDialogWidgets
    fFindTextWidget.SetText (fFindText);
    fFindTextWidget.SetPopupItems (fRecentFindTextStrings);
#else
    SetItemText (findText, Led_tString2SDKString (fFindText));
#endif
    SelectItemText (findText);
    SetFocusedItem (findText);

#if qStroika_Foundation_Common_Platform_Windows
    SetItemChecked (kLedStdDlg_FindBox_WrapAtEndOfDoc, fWrapSearch);
    SetItemChecked (kLedStdDlg_FindBox_WholeWord, fWholeWordSearch);
    SetItemChecked (kLedStdDlg_FindBox_IgnoreCase, not fCaseSensativeSearch);
#endif
    inherited::PreDoModalHook ();
}

void Led_StdDialogHelper_FindDialog::OnFindButton ()
{
    Led_StdDialogHelper_FindDialog::OnDontFindButton (); // for code sharing - could put that common stuff in other routine...
    fPressedOK = true;
}

void Led_StdDialogHelper_FindDialog::OnDontFindButton ()
{
#if qSupportLedDialogWidgets
    fFindText = fFindTextWidget.GetText ();
#elif qStroika_Foundation_Common_Platform_Windows
    fFindText = Led_SDKString2tString (GetItemText (kLedStdDlg_FindBox_FindText));
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
    fFindText = Led_SDKString2tString (GetItemText (fLookupTextWidget));
#endif

#if qStroika_Foundation_Common_Platform_Windows
    fWrapSearch          = GetItemChecked (kLedStdDlg_FindBox_WrapAtEndOfDoc);
    fWholeWordSearch     = GetItemChecked (kLedStdDlg_FindBox_WholeWord);
    fCaseSensativeSearch = not GetItemChecked (kLedStdDlg_FindBox_IgnoreCase);
#endif
    OnOK ();
}

#if qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
void Led_StdDialogHelper_FindDialog::Static_OnFindButtonClick (GtkWidget* widget, gpointer data)
{
    Led_StdDialogHelper_FindDialog* dlg = reinterpret_cast<Led_StdDialogHelper_FindDialog*> (data);
    dlg->OnFindButton ();
}

void Led_StdDialogHelper_FindDialog::Static_OnDontFindButtonClick (GtkWidget* widget, gpointer data)
{
    Led_StdDialogHelper_FindDialog* dlg = reinterpret_cast<Led_StdDialogHelper_FindDialog*> (data);
    dlg->OnDontFindButton ();
}
#endif

#endif

#if qSupportStdReplaceDlg
/*
 ********************************************************************************
 ************************* Led_StdDialogHelper_ReplaceDialog ********************
 ********************************************************************************
 */
#if qStroika_Foundation_Common_Platform_Windows
Led_StdDialogHelper_ReplaceDialog::Led_StdDialogHelper_ReplaceDialog (HINSTANCE hInstance, HWND parentWnd, const Characters::SDKChar* resID)
    : inherited (hInstance, resID, parentWnd)
    , fFindText ()
    , fRecentFindTextStrings ()
    , fReplaceText ()
    , fWrapSearch (false)
    , fWholeWordSearch (false)
    , fCaseSensativeSearch (false)
    , fPressed (eCancel)
#if qSupportLedDialogWidgets
    , fFindTextWidget ()
    , fReplaceTextWidget ()
#endif
{
}
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
Led_StdDialogHelper_ReplaceDialog::Led_StdDialogHelper_ReplaceDialog (GtkWindow* parentWindow)
    : inherited (parentWindow)
    , fFindText ()
    , fRecentFindTextStrings ()
    , fReplaceText ()
    , fWrapSearch (false)
    , fWholeWordSearch (false)
    , fCaseSensativeSearch (false)
    , fPressed (eCancel)
    , fLookupTextWidget (NULL)
    , fReplaceTextWidget (NULL)
{
}
#endif

#if qStroika_Foundation_Common_Platform_Windows
BOOL Led_StdDialogHelper_ReplaceDialog::DialogProc (UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_COMMAND: {
            switch (wParam) {
                case kLedStdDlg_ReplaceBox_Find:
                    OnFindButton ();
                    return true;
                case kLedStdDlg_ReplaceBox_Replace:
                    OnReplaceButton ();
                    return true;
                case kLedStdDlg_ReplaceBox_ReplaceAll:
                    OnReplaceAllButton ();
                    return true;
                case kLedStdDlg_ReplaceBox_ReplaceAllInSelection:
                    OnReplaceAllInSelectionButton ();
                    return true;
                case kLedStdDlg_ReplaceBox_Cancel:
                    OnDontFindButton ();
                    return true;
            }
        }
    }
    return inherited::DialogProc (message, wParam, lParam);
}
#endif

void Led_StdDialogHelper_ReplaceDialog::PreDoModalHook ()
{
#if qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
    GtkWidget* actionArea = GTK_DIALOG (GetWindow ())->action_area;
    {
        fLookupTextWidget = gtk_entry_new ();
        gtk_container_add (GTK_CONTAINER (actionArea), fLookupTextWidget);
        gtk_widget_show (fLookupTextWidget);
        fReplaceTextWidget = gtk_entry_new ();
        gtk_container_add (GTK_CONTAINER (actionArea), fReplaceTextWidget);
        gtk_widget_show (fReplaceTextWidget);
        //          gtk_entry_set_text (GTK_ENTRY (fLookupTextWidget), Led_tString2SDKString (fFindText).c_str ());
        //          gtk_entry_select_region (GTK_ENTRY (fLookupTextWidget), 0, -1);
        //          gtk_widget_grab_focus (fLookupTextWidget);
    }
    {
        GtkWidget* button = gtk_button_new_with_label ("Find");
        gtk_container_add (GTK_CONTAINER (actionArea), button);
        GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
        gtk_widget_show (button);
        gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (Static_OnFindButtonClick), (gpointer)this);
    }
    {
        GtkWidget* button = gtk_button_new_with_label ("Close");
        gtk_container_add (GTK_CONTAINER (actionArea), button);
        GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
        gtk_widget_show (button);
        gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (Static_OnDontFindButtonClick), (gpointer)this);
    }
    {
        GtkWidget* button = gtk_button_new_with_label ("Replace");
        gtk_container_add (GTK_CONTAINER (actionArea), button);
        GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
        gtk_widget_show (button);
        gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (Static_OnReplaceButtonClick), (gpointer)this);
    }
    {
        GtkWidget* button = gtk_button_new_with_label ("Replace All");
        gtk_container_add (GTK_CONTAINER (actionArea), button);
        GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
        gtk_widget_show (button);
        gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (Static_OnReplaceAllButtonClick), (gpointer)this);
    }
    {
        GtkWidget* button = gtk_button_new_with_label ("Replace All in Selection");
        gtk_container_add (GTK_CONTAINER (actionArea), button);
        GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
        gtk_widget_show (button);
        gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (Static_OnReplaceAllInSelectionButtonClick), (gpointer)this);
    }
#endif

#if qStroika_Foundation_Common_Platform_Windows && qSupportLedDialogWidgets
    /*
     *  ReplaceWindow seems to work better than SubclassWindow - for reasons I don't FULLY understand.
     *  (see SPR#1266).
     */
    fFindTextWidget.ReplaceWindow (::GetDlgItem (GetHWND (), kLedStdDlg_ReplaceBox_FindText));
    fReplaceTextWidget.ReplaceWindow (::GetDlgItem (GetHWND (), kLedStdDlg_ReplaceBox_ReplaceText));
#endif

#if qStroika_Foundation_Common_Platform_Windows
    DialogItemID findText    = kLedStdDlg_ReplaceBox_FindText;
    DialogItemID replaceText = kLedStdDlg_ReplaceBox_ReplaceText;
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
    DialogItemID findText    = fLookupTextWidget;
    DialogItemID replaceText = fReplaceTextWidget;
#endif

#if qSupportLedDialogWidgets
    fFindTextWidget.SetText (fFindText);
    fFindTextWidget.SetPopupItems (fRecentFindTextStrings);
    fReplaceTextWidget.SetText (fReplaceText);
#else
    SetItemText (findText, Led_tString2SDKString (fFindText));
    SetItemText (replaceText, Led_tString2SDKString (fReplaceText));
#endif
    SelectItemText (findText);
    SelectItemText (replaceText);
    SetFocusedItem (findText);

#if qStroika_Foundation_Common_Platform_Windows
    SetItemChecked (kLedStdDlg_ReplaceBox_WrapAtEndOfDoc, fWrapSearch);
    SetItemChecked (kLedStdDlg_ReplaceBox_WholeWord, fWholeWordSearch);
    SetItemChecked (kLedStdDlg_ReplaceBox_IgnoreCase, not fCaseSensativeSearch);
#endif
    inherited::PreDoModalHook ();
}

void Led_StdDialogHelper_ReplaceDialog::OnFindButton ()
{
    SaveItems ();
    OnOK ();
    fPressed = eFind;
}

void Led_StdDialogHelper_ReplaceDialog::OnReplaceButton ()
{
    SaveItems ();
    OnOK ();
    fPressed = eReplace;
}

void Led_StdDialogHelper_ReplaceDialog::OnReplaceAllButton ()
{
    SaveItems ();
    OnOK ();
    fPressed = eReplaceAll;
}

void Led_StdDialogHelper_ReplaceDialog::OnReplaceAllInSelectionButton ()
{
    SaveItems ();
    OnOK ();
    fPressed = eReplaceAllInSelection;
}

void Led_StdDialogHelper_ReplaceDialog::OnDontFindButton ()
{
    SaveItems ();
    OnCancel ();
}

void Led_StdDialogHelper_ReplaceDialog::SaveItems ()
{
#if qSupportLedDialogWidgets
    fFindText    = fFindTextWidget.GetText ();
    fReplaceText = fReplaceTextWidget.GetText ();
#elif qStroika_Foundation_Common_Platform_Windows
    fFindText    = Led_SDKString2tString (GetItemText (kLedStdDlg_ReplaceBox_FindText));
    fReplaceText = Led_SDKString2tString (GetItemText (kLedStdDlg_ReplaceBox_ReplaceText));
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
    fFindText    = Led_SDKString2tString (GetItemText (fLookupTextWidget));
    fReplaceText = Led_SDKString2tString (GetItemText (fReplaceTextWidget));
#endif

#if qStroika_Foundation_Common_Platform_Windows
    fWrapSearch          = GetItemChecked (kLedStdDlg_ReplaceBox_WrapAtEndOfDoc);
    fWholeWordSearch     = GetItemChecked (kLedStdDlg_ReplaceBox_WholeWord);
    fCaseSensativeSearch = not GetItemChecked (kLedStdDlg_ReplaceBox_IgnoreCase);
#endif
}

#if qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
void Led_StdDialogHelper_ReplaceDialog::Static_OnFindButtonClick (GtkWidget* widget, gpointer data)
{
    Led_StdDialogHelper_ReplaceDialog* dlg = reinterpret_cast<Led_StdDialogHelper_ReplaceDialog*> (data);
    dlg->OnFindButton ();
}

void Led_StdDialogHelper_ReplaceDialog::Static_OnDontFindButtonClick (GtkWidget* widget, gpointer data)
{
    Led_StdDialogHelper_ReplaceDialog* dlg = reinterpret_cast<Led_StdDialogHelper_ReplaceDialog*> (data);
    dlg->OnDontFindButton ();
}

void Led_StdDialogHelper_ReplaceDialog::Static_OnReplaceButtonClick (GtkWidget* widget, gpointer data)
{
    Led_StdDialogHelper_ReplaceDialog* dlg = reinterpret_cast<Led_StdDialogHelper_ReplaceDialog*> (data);
    dlg->OnReplaceButton ();
}

void Led_StdDialogHelper_ReplaceDialog::Static_OnReplaceAllButtonClick (GtkWidget* widget, gpointer data)
{
    Led_StdDialogHelper_ReplaceDialog* dlg = reinterpret_cast<Led_StdDialogHelper_ReplaceDialog*> (data);
    dlg->OnReplaceAllButton ();
}

void Led_StdDialogHelper_ReplaceDialog::Static_OnReplaceAllInSelectionButtonClick (GtkWidget* widget, gpointer data)
{
    Led_StdDialogHelper_ReplaceDialog* dlg = reinterpret_cast<Led_StdDialogHelper_ReplaceDialog*> (data);
    dlg->OnReplaceAllInSelectionButton ();
}
#endif

#endif

#if qUseGTKForLedStandardDialogs && qStroika_FeatureSupported_XWindows
/*
 ********************************************************************************
 ********************************** StdFontPickBox ******************************
 ********************************************************************************
 */
StdFontPickBox::StdFontPickBox (GtkWindow* modalParentWindow, const FontSpecification& initialFont)
    : inherited (modalParentWindow)
    , fFont (initialFont)
{
}

GtkWidget* StdFontPickBox::MakeWindow ()
{
    return gtk_font_selection_dialog_new ("Select font");
}

void StdFontPickBox::PreDoModalHook ()
{
    inherited::PreDoModalHook ();
    SetOKButton (GTK_FONT_SELECTION_DIALOG (GetWindow ())->ok_button);
    SetCancelButton (GTK_FONT_SELECTION_DIALOG (GetWindow ())->cancel_button);
    Verify (gtk_font_selection_dialog_set_font_name (GTK_FONT_SELECTION_DIALOG (GetWindow ()), fFont.GetOSRep ().c_str ()));
}

void StdFontPickBox::OnOK ()
{
    inherited::OnOK ();
    gchar* newFontOSName = gtk_font_selection_dialog_get_font_name (GTK_FONT_SELECTION_DIALOG (GetWindow ()));
    if (newFontOSName != NULL) {
        fFont.SetFromOSRep (newFontOSName);
        g_free (newFontOSName);
    }
}
#endif

#if qSupportStdColorPickBox
/*
 ********************************************************************************
 ********************************** StdColorPickBox *****************************
 ********************************************************************************
 */
#if qStroika_Foundation_Common_Platform_Windows
StdColorPickBox::StdColorPickBox (const Color& initialColor)
    : fColor (initialColor)
    , fParentWnd (::GetActiveWindow ()) // a good default...
{
}

StdColorPickBox::StdColorPickBox ([[maybe_unused]] HINSTANCE hInstance, HWND parentWnd, const Color& initialColor)
    : fColor (initialColor)
    , fParentWnd (parentWnd)
{
}
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
StdColorPickBox::StdColorPickBox (GtkWindow* modalParentWindow, const Color& initialColor)
    : inherited (modalParentWindow)
    , fColor (initialColor)
{
}
#endif

#if qStroika_Foundation_Common_Platform_Windows
bool StdColorPickBox::DoModal ()
{
#if qStroika_Foundation_Common_Platform_Windows
    CHOOSECOLOR cc;
    memset (&cc, 0, sizeof (cc));
    cc.lStructSize = sizeof (cc);
    cc.Flags |= CC_ANYCOLOR;
    cc.rgbResult = fColor.GetOSRep ();
    cc.Flags |= CC_RGBINIT;
    cc.Flags |= CC_FULLOPEN;

    cc.Flags |= CC_ENABLEHOOK;
    cc.lpfnHook = ColorPickerINITPROC;

    static COLORREF sCustomColors[16];
    cc.lpCustColors = sCustomColors;

    cc.hwndOwner = fParentWnd;

    if (::ChooseColor (&cc)) {
        fColor = Color (cc.rgbResult);
        return true;
    }
#endif
    return false;
}
#endif

#if qStroika_Foundation_Common_Platform_Windows
UINT_PTR CALLBACK StdColorPickBox::ColorPickerINITPROC (HWND hWnd, UINT message, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam)
{
    if (hWnd != NULL and message == WM_INITDIALOG) {
        Led_CenterWindowInParent (hWnd);
    }
    return 0;
}
#endif

#if qUseGTKForLedStandardDialogs && qStroika_FeatureSupported_XWindows
GtkWidget* StdColorPickBox::MakeWindow ()
{
    return gtk_color_selection_dialog_new ("Select color");
}

void StdColorPickBox::PreDoModalHook ()
{
    inherited::PreDoModalHook ();
    SetOKButton (GTK_COLOR_SELECTION_DIALOG (GetWindow ())->ok_button);
    SetCancelButton (GTK_COLOR_SELECTION_DIALOG (GetWindow ())->cancel_button);
    gdouble colors[4];
    colors[0] = static_cast<double> (fColor.GetRed ()) / Color::kColorValueMax;
    colors[1] = static_cast<double> (fColor.GetGreen ()) / Color::kColorValueMax;
    colors[2] = static_cast<double> (fColor.GetBlue ()) / Color::kColorValueMax;
    colors[3] = 0;
    gtk_color_selection_set_color (GTK_COLOR_SELECTION (GTK_COLOR_SELECTION_DIALOG (GetWindow ())->colorsel), colors);
}

void StdColorPickBox::OnOK ()
{
    inherited::OnOK ();
    gdouble colors[4];
    gtk_color_selection_get_color (GTK_COLOR_SELECTION (GTK_COLOR_SELECTION_DIALOG (GetWindow ())->colorsel), colors);
    using CV = Color::ColorValue;
    fColor   = Color (static_cast<CV> (colors[0] * Color::kColorValueMax), static_cast<CV> (colors[1] * Color::kColorValueMax),
                      static_cast<CV> (colors[2] * Color::kColorValueMax));
}
#endif
#endif

#if qSupportStdFileDlg && defined(__cplusplus)
/*
 ********************************************************************************
 ********************************** StdFilePickBox ******************************
 ********************************************************************************
 */
StdFilePickBox::StdFilePickBox (GtkWindow* modalParentWindow, const SDKString& title, bool saveDialog, const SDKString& fileName)
    : inherited (modalParentWindow)
    , fTitle (title)
    , fSaveDialog (saveDialog)
    , fFileName (fileName)
{
}

GtkWidget* StdFilePickBox::MakeWindow ()
{
    return gtk_file_selection_new (fTitle.c_str ());
}

void StdFilePickBox::PreDoModalHook ()
{
    inherited::PreDoModalHook ();

    SetOKButton (GTK_FILE_SELECTION (GetWindow ())->ok_button);
    SetCancelButton (GTK_FILE_SELECTION (GetWindow ())->cancel_button);

    gtk_file_selection_hide_fileop_buttons (GTK_FILE_SELECTION (GetWindow ())); // they look terrible...
    if (fSaveDialog) {
        gtk_file_selection_set_filename (GTK_FILE_SELECTION (GetWindow ()), fFileName.empty () ? Led_SDK_TCHAROF ("untitled") : fFileName.c_str ());
    }
}

void StdFilePickBox::OnOK ()
{
    inherited::OnOK ();

    SDKString fileName = gtk_file_selection_get_filename (GTK_FILE_SELECTION (GetWindow ()));
    try {
        if (fileName.empty ()) {
            throw "EMPTY";
        }
        fFileName = fileName;
    }
    catch (...) {
        // should print error dialog on errors...
    }
}

SDKString StdFilePickBox::GetFileName () const
{
    return fFileName;
}
#endif

#if qSupportUpdateWin32FileAssocDlg
/*
 ********************************************************************************
 ******************* Led_StdDialogHelper_UpdateWin32FileAssocsDialog ************
 ********************************************************************************
 */

Led_StdDialogHelper_UpdateWin32FileAssocsDialog::Led_StdDialogHelper_UpdateWin32FileAssocsDialog (HINSTANCE hInstance, HWND parentWnd,
                                                                                                  const Characters::SDKChar* resID)
    : inherited (hInstance, resID, parentWnd)
    , fAppName ()
    , fTypeList ()
    , fKeepChecking (false)
{
}

void Led_StdDialogHelper_UpdateWin32FileAssocsDialog::PreDoModalHook ()
{
#if qStroika_Foundation_Common_Platform_Windows
    ::SetForegroundWindow (GetHWND ());
    Characters::SDKChar messageText[1024];
    (void)::GetDlgItemText (GetHWND (), kLedStdDlg_UpdateWin32FileAssocsDialog_Msg, messageText, static_cast<int> (Memory::NEltsOf (messageText)));
    SDKString m = messageText;
    ReplaceAllTokens (&m, Led_SDK_TCHAROF ("%0"), fAppName);
    ReplaceAllTokens (&m, Led_SDK_TCHAROF ("%1"), fTypeList);
    (void)::SetDlgItemText (GetHWND (), kLedStdDlg_UpdateWin32FileAssocsDialog_Msg, m.c_str ());

    (void)::GetDlgItemText (GetHWND (), kLedStdDlg_UpdateWin32FileAssocsDialog_KeepCheckingCheckboxMsg, messageText,
                            static_cast<int> (Memory::NEltsOf (messageText)));
    m = messageText;
    ReplaceAllTokens (&m, Led_SDK_TCHAROF ("%0"), fAppName);
    ReplaceAllTokens (&m, Led_SDK_TCHAROF ("%1"), fTypeList);
    (void)::SetDlgItemText (GetHWND (), kLedStdDlg_UpdateWin32FileAssocsDialog_KeepCheckingCheckboxMsg, m.c_str ());

    ::CheckDlgButton (GetHWND (), kLedStdDlg_UpdateWin32FileAssocsDialog_KeepCheckingCheckboxMsg, fKeepChecking);
#endif
    inherited::PreDoModalHook ();
}

void Led_StdDialogHelper_UpdateWin32FileAssocsDialog::OnOK ()
{
    fKeepChecking = !!::IsDlgButtonChecked (GetHWND (), kLedStdDlg_UpdateWin32FileAssocsDialog_KeepCheckingCheckboxMsg);
    inherited::OnOK ();
}

void Led_StdDialogHelper_UpdateWin32FileAssocsDialog::OnCancel ()
{
    fKeepChecking = !!::IsDlgButtonChecked (GetHWND (), kLedStdDlg_UpdateWin32FileAssocsDialog_KeepCheckingCheckboxMsg);
    inherited::OnCancel ();
}
#endif

#if qSupportParagraphIndentsDlg
/*
 ********************************************************************************
 ******************* Led_StdDialogHelper_ParagraphIndentsDialog *****************
 ********************************************************************************
 */
#if qStroika_Foundation_Common_Platform_Windows
Led_StdDialogHelper_ParagraphIndentsDialog::Led_StdDialogHelper_ParagraphIndentsDialog (HINSTANCE hInstance, HWND parentWnd,
                                                                                        const Characters::SDKChar* resID)
    : inherited (hInstance, resID, parentWnd)
    , fLeftMargin_Valid (false)
    , fLeftMargin_Orig (TWIPS{0})
    , fLeftMargin_Result (TWIPS{0})
    , fRightMargin_Valid (false)
    , fRightMargin_Orig (TWIPS{0})
    , fRightMargin_Result (TWIPS{0})
    , fFirstIndent_Valid (false)
    , fFirstIndent_Orig (TWIPS{0})
    , fFirstIndent_Result (TWIPS{0})
{
}
#endif

void Led_StdDialogHelper_ParagraphIndentsDialog::InitValues (TWIPS leftMargin, bool leftMarginValid, TWIPS rightMargin,
                                                             bool rightMarginValid, TWIPS firstIndent, bool firstIndentValid)
{
    fLeftMargin_Valid   = leftMarginValid;
    fLeftMargin_Orig    = leftMargin;
    fLeftMargin_Result  = leftMargin;
    fRightMargin_Valid  = rightMarginValid;
    fRightMargin_Orig   = rightMargin;
    fRightMargin_Result = rightMargin;
    fFirstIndent_Valid  = firstIndentValid;
    fFirstIndent_Orig   = firstIndent;
    fFirstIndent_Result = firstIndent;
}

void Led_StdDialogHelper_ParagraphIndentsDialog::PreDoModalHook ()
{
    if (fLeftMargin_Valid) {
        SetItemText (kLedStdDlg_ParagraphIndents_LeftMarginFieldID, FormatTWIPSAsString (fLeftMargin_Orig));
    }
    if (fRightMargin_Valid) {
        SetItemText (kLedStdDlg_ParagraphIndents_RightMarginFieldID, FormatTWIPSAsString (fRightMargin_Orig));
    }
    if (fFirstIndent_Valid) {
        SetItemText (kLedStdDlg_ParagraphIndents_FirstIndentFieldID, FormatTWIPSAsString (fFirstIndent_Orig));
    }
    SetFocusedItem (kLedStdDlg_ParagraphIndents_LeftMarginFieldID);
    SelectItemText (kLedStdDlg_ParagraphIndents_LeftMarginFieldID);
    inherited::PreDoModalHook ();
}

DISABLE_COMPILER_MSC_WARNING_START (4706)
void Led_StdDialogHelper_ParagraphIndentsDialog::OnOK ()
{
    SDKString leftMargin = GetItemText (kLedStdDlg_ParagraphIndents_LeftMarginFieldID);
    if (not(fLeftMargin_Valid = ParseStringToTWIPS (leftMargin, &fLeftMargin_Result))) {
        fLeftMargin_Result = fLeftMargin_Orig;
    }

    SDKString rightMargin = GetItemText (kLedStdDlg_ParagraphIndents_RightMarginFieldID);
    if (not(fRightMargin_Valid = ParseStringToTWIPS (rightMargin, &fRightMargin_Result))) {
        fRightMargin_Result = fRightMargin_Orig;
    }

    SDKString firstIndent = GetItemText (kLedStdDlg_ParagraphIndents_FirstIndentFieldID);
    if (not(fLeftMargin_Valid = ParseStringToTWIPS (firstIndent, &fFirstIndent_Result))) {
        fFirstIndent_Result = fFirstIndent_Orig;
    }
    inherited::OnOK ();
}
DISABLE_COMPILER_MSC_WARNING_END (4706)
#endif

#if qSupportParagraphSpacingDlg
/*
 ********************************************************************************
 ******************* Led_StdDialogHelper_ParagraphSpacingDialog *****************
 ********************************************************************************
 */
#if qStroika_Foundation_Common_Platform_Windows
Led_StdDialogHelper_ParagraphSpacingDialog::Led_StdDialogHelper_ParagraphSpacingDialog (HINSTANCE hInstance, HWND parentWnd,
                                                                                        const Characters::SDKChar* resID)
    : inherited (hInstance, resID, parentWnd)
    , fSpaceBefore_Valid (false)
    , fSpaceBefore_Orig (TWIPS{0})
    , fSpaceBefore_Result (TWIPS{0})
    , fSpaceAfter_Valid (false)
    , fSpaceAfter_Orig (TWIPS{0})
    , fSpaceAfter_Result (TWIPS{0})
    , fLineSpacing_Valid (false)
    , fLineSpacing_Orig ()
    , fLineSpacing_Result ()
{
}
#endif

void Led_StdDialogHelper_ParagraphSpacingDialog::InitValues (TWIPS spaceBefore, bool spaceBeforeValid, TWIPS spaceAfter,
                                                             bool spaceAfterValid, LineSpacing lineSpacing, bool lineSpacingValid)
{
    fSpaceBefore_Valid  = spaceBeforeValid;
    fSpaceBefore_Orig   = spaceBefore;
    fSpaceBefore_Result = spaceBefore;
    fSpaceAfter_Valid   = spaceAfterValid;
    fSpaceAfter_Orig    = spaceAfter;
    fSpaceAfter_Result  = spaceAfter;
    fLineSpacing_Valid  = lineSpacingValid;
    fLineSpacing_Orig   = lineSpacing;
    fLineSpacing_Result = lineSpacing;
}

void Led_StdDialogHelper_ParagraphSpacingDialog::PreDoModalHook ()
{
#if qStroika_Foundation_Common_Platform_Windows
    HWND popup = ::GetDlgItem (GetHWND (), kParagraphSpacing_Dialog_LineSpaceModeFieldID);
    AssertNotNull (popup);
    Assert (::IsWindow (popup));
    (void)::SendMessage (popup, CB_RESETCONTENT, 0, 0);
    Verify (::SendMessage (popup, CB_ADDSTRING, 0, reinterpret_cast<LPARAM> (_T ("Single"))) != CB_ERR);
    Verify (::SendMessage (popup, CB_ADDSTRING, 0, reinterpret_cast<LPARAM> (_T ("1.5 lines"))) != CB_ERR);
    Verify (::SendMessage (popup, CB_ADDSTRING, 0, reinterpret_cast<LPARAM> (_T ("Double"))) != CB_ERR);
    Verify (::SendMessage (popup, CB_ADDSTRING, 0, reinterpret_cast<LPARAM> (_T ("At Least (TWIPS)"))) != CB_ERR);
    Verify (::SendMessage (popup, CB_ADDSTRING, 0, reinterpret_cast<LPARAM> (_T ("Exact (TWIPS)"))) != CB_ERR);
    Verify (::SendMessage (popup, CB_ADDSTRING, 0, reinterpret_cast<LPARAM> (_T ("Exact (1/20 lines)"))) != CB_ERR);
#endif

    if (fSpaceBefore_Valid) {
        SetItemText (kParagraphSpacing_Dialog_SpaceBeforeFieldID, FormatTWIPSAsString (fSpaceBefore_Orig));
    }
    if (fSpaceAfter_Valid) {
        SetItemText (kParagraphSpacing_Dialog_SpaceAfterFieldID, FormatTWIPSAsString (fSpaceAfter_Orig));
    }
    if (fLineSpacing_Valid) {
#if qStroika_Foundation_Common_Platform_Windows
        Verify (::SendMessage (popup, CB_SETCURSEL, fLineSpacing_Orig.fRule, 0) != CB_ERR);
#endif
        if (fLineSpacing_Orig.fRule == LineSpacing::eAtLeastTWIPSSpacing or fLineSpacing_Orig.fRule == LineSpacing::eExactTWIPSSpacing or
            fLineSpacing_Orig.fRule == LineSpacing::eExactLinesSpacing) {
            SetItemText (kParagraphSpacing_Dialog_LineSpaceArgFieldID, FormatTWIPSAsString (TWIPS (fLineSpacing_Orig.fArg)));
        }
    }
    SetFocusedItem (kParagraphSpacing_Dialog_SpaceBeforeFieldID);
    SelectItemText (kParagraphSpacing_Dialog_SpaceBeforeFieldID);
    inherited::PreDoModalHook ();
}

DISABLE_COMPILER_MSC_WARNING_START (4706)
void Led_StdDialogHelper_ParagraphSpacingDialog::OnOK ()
{
    SDKString spaceBefore = GetItemText (kParagraphSpacing_Dialog_SpaceBeforeFieldID);
    if (not(fSpaceBefore_Valid = ParseStringToTWIPS (spaceBefore, &fSpaceBefore_Result))) {
        fSpaceBefore_Result = fSpaceBefore_Orig;
    }

    SDKString spaceAfter = GetItemText (kParagraphSpacing_Dialog_SpaceAfterFieldID);
    if (not(fSpaceAfter_Valid = ParseStringToTWIPS (spaceAfter, &fSpaceAfter_Result))) {
        fSpaceAfter_Result = fSpaceAfter_Orig;
    }

#if qStroika_Foundation_Common_Platform_Windows
    HWND popup = ::GetDlgItem (GetHWND (), kParagraphSpacing_Dialog_LineSpaceModeFieldID);
    AssertNotNull (popup);
    Assert (::IsWindow (popup));
    int r = static_cast<int> (::SendMessage (popup, CB_GETCURSEL, 0, 0));
#endif
    if (r >= 0 and r <= 5) {
        fLineSpacing_Valid = true;
        if (r == LineSpacing::eAtLeastTWIPSSpacing or r == LineSpacing::eExactTWIPSSpacing) {
            SDKString arg  = GetItemText (kParagraphSpacing_Dialog_SpaceAfterFieldID);
            TWIPS     argT = TWIPS{0};
            if (ParseStringToTWIPS (arg, &argT)) {
                fLineSpacing_Result = LineSpacing (LineSpacing::Rule (r), argT);
            }
            else {
                fLineSpacing_Valid = false;
            }
        }
        else if (r == LineSpacing::eExactLinesSpacing) {
            SDKString arg  = GetItemText (kParagraphSpacing_Dialog_LineSpaceArgFieldID);
            int       argI = 0;
            if (ParseStringToINT (arg, &argI)) {
                fLineSpacing_Result = LineSpacing (LineSpacing::Rule (r), argI);
            }
            else {
                fLineSpacing_Valid = false;
            }
        }
        else {
            fLineSpacing_Result = LineSpacing::Rule (r);
        }
    }

    inherited::OnOK ();
}
DISABLE_COMPILER_MSC_WARNING_END (4706)
#endif

#if qSupportOtherFontSizeDlg
/*
 ********************************************************************************
 ******************** Led_StdDialogHelper_OtherFontSizeDialog *******************
 ********************************************************************************
 */
#if qStroika_Foundation_Common_Platform_Windows
Led_StdDialogHelper_OtherFontSizeDialog::Led_StdDialogHelper_OtherFontSizeDialog (HINSTANCE hInstance, HWND parentWnd, const Characters::SDKChar* resID)
    : inherited (hInstance, resID, parentWnd)
    , fFontSize_Orig (0)
    , fFontSize_Result (0)
{
}
#endif

void Led_StdDialogHelper_OtherFontSizeDialog::InitValues (DistanceType origFontSize)
{
    fFontSize_Orig   = origFontSize;
    fFontSize_Result = origFontSize;
}

void Led_StdDialogHelper_OtherFontSizeDialog::PreDoModalHook ()
{
    SetItemText (kOtherFontSize_Dialog_FontSizeEditFieldID, FormatINTAsString (fFontSize_Orig));
    SetFocusedItem (kOtherFontSize_Dialog_FontSizeEditFieldID);
    SelectItemText (kOtherFontSize_Dialog_FontSizeEditFieldID);
    inherited::PreDoModalHook ();
}

void Led_StdDialogHelper_OtherFontSizeDialog::OnOK ()
{
    int r = 0;
    if (ParseStringToINT (GetItemText (kOtherFontSize_Dialog_FontSizeEditFieldID), &r)) {
        fFontSize_Result = r;
    }
    inherited::OnOK ();
}
#endif

#if qSupportUnknownEmbeddingInfoDlg
/*
 ********************************************************************************
 ******************* Led_StdDialogHelper_UnknownEmbeddingInfoDialog *************
 ********************************************************************************
 */
#if qStroika_Foundation_Common_Platform_Windows
Led_StdDialogHelper_UnknownEmbeddingInfoDialog::Led_StdDialogHelper_UnknownEmbeddingInfoDialog (HINSTANCE hInstance, HWND parentWnd,
                                                                                                const Characters::SDKChar* resID)
    : inherited (hInstance, resID, parentWnd)
    , fEmbeddingTypeName ()
{
}
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
Led_StdDialogHelper_UnknownEmbeddingInfoDialog::Led_StdDialogHelper_UnknownEmbeddingInfoDialog (GtkWindow* parentWindow)
    : inherited (parentWindow)
    , fEmbeddingTypeName ()
{
}
#endif

void Led_StdDialogHelper_UnknownEmbeddingInfoDialog::PreDoModalHook ()
{
#if qStroika_Foundation_Common_Platform_Windows
    Characters::SDKChar messageText[1024];
    (void)::GetDlgItemText (GetHWND (), kLedStdDlg_UnknownEmbeddingInfoBox_TypeTextMsg, messageText, static_cast<int> (Memory::NEltsOf (messageText)));

    SDKString m = messageText;
    ReplaceAllTokens (&m, Led_SDK_TCHAROF ("%0"), fEmbeddingTypeName);
    (void)::SetDlgItemText (GetHWND (), kLedStdDlg_UnknownEmbeddingInfoBox_TypeTextMsg, m.c_str ());
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
    GtkWidget* window = GetWindow ();
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);

    string     message = "Selected object is of type '" + fEmbeddingTypeName + "'.";
    GtkWidget* label   = gtk_label_new (message.c_str ());

    gtk_widget_show (label);

    /* a button to contain the pixmap widget */
    GtkWidget* button = gtk_button_new_with_label ("OK");
    GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
    gtk_widget_show (button);
    SetOKButton (button);
#endif
    inherited::PreDoModalHook ();
}
#endif

#if qSupportURLXEmbeddingInfoDlg
/*
 ********************************************************************************
 ******************* Led_StdDialogHelper_URLXEmbeddingInfoDialog ****************
 ********************************************************************************
 */
#if qStroika_Foundation_Common_Platform_Windows
Led_StdDialogHelper_URLXEmbeddingInfoDialog::Led_StdDialogHelper_URLXEmbeddingInfoDialog (HINSTANCE hInstance, HWND parentWnd,
                                                                                          const Characters::SDKChar* resID)
    : inherited (hInstance, resID, parentWnd)
    , fEmbeddingTypeName ()
    , fTitleText ()
    , fURLText ()
{
}
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
Led_StdDialogHelper_URLXEmbeddingInfoDialog::Led_StdDialogHelper_URLXEmbeddingInfoDialog (GtkWindow* parentWindow)
    : inherited (parentWindow)
    , fTitleTextWidget (NULL)
    , fURLTextWidget (NULL)
    , fEmbeddingTypeName ()
    , fTitleText ()
    , fURLText ()
{
}
#endif

void Led_StdDialogHelper_URLXEmbeddingInfoDialog::PreDoModalHook ()
{
#if qStroika_Foundation_Common_Platform_Windows
    Characters::SDKChar messageText[1024];
    (void)::GetDlgItemText (GetHWND (), kLedStdDlg_URLXEmbeddingInfoBox_TypeTextMsg, messageText, static_cast<int> (Memory::NEltsOf (messageText)));

    SDKString m = messageText;
    ReplaceAllTokens (&m, Led_SDK_TCHAROF ("%0"), fEmbeddingTypeName);
    (void)::SetDlgItemText (GetHWND (), kLedStdDlg_URLXEmbeddingInfoBox_TypeTextMsg, m.c_str ());

    (void)::SetDlgItemText (GetHWND (), kLedStdDlg_URLXEmbeddingInfoBox_TitleText, fTitleText.c_str ());
    (void)::SetDlgItemText (GetHWND (), kLedStdDlg_URLXEmbeddingInfoBox_URLText, fURLText.c_str ());
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
    // MUST FIX THIS FOR X-WINDOWS!!!!

    GtkWidget* window = GetWindow ();
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);

    string     message = "Selected object is of type '" + fEmbeddingTypeName + "'.";
    GtkWidget* label   = gtk_label_new (message.c_str ());

    gtk_widget_show (label);

    /* a button to contain the pixmap widget */
    GtkWidget* button = gtk_button_new_with_label ("OK");
    GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
    gtk_widget_show (button);
    SetOKButton (button);
#endif
#if qStroika_Foundation_Common_Platform_Windows
    SelectItemText (kLedStdDlg_URLXEmbeddingInfoBox_TitleText);
#endif
    inherited::PreDoModalHook ();
}

void Led_StdDialogHelper_URLXEmbeddingInfoDialog::OnOK ()
{
#if qStroika_Foundation_Common_Platform_Windows
    Characters::SDKChar bufText[1024];
    (void)::GetDlgItemText (GetHWND (), kLedStdDlg_URLXEmbeddingInfoBox_TitleText, bufText, static_cast<int> (Memory::NEltsOf (bufText)));
    fTitleText = bufText;
    (void)::GetDlgItemText (GetHWND (), kLedStdDlg_URLXEmbeddingInfoBox_URLText, bufText, static_cast<int> (Memory::NEltsOf (bufText)));
    fURLText = bufText;
#endif
#if qStroika_Foundation_Common_Platform_Windows
    SelectItemText (kLedStdDlg_URLXEmbeddingInfoBox_TitleText);
#endif
    inherited::OnOK ();
}
#endif

#if qSupportURLXEmbeddingInfoDlg
/*
 ********************************************************************************
 ******************* Led_StdDialogHelper_AddURLXEmbeddingInfoDialog *************
 ********************************************************************************
 */
#if qStroika_Foundation_Common_Platform_Windows
Led_StdDialogHelper_AddURLXEmbeddingInfoDialog::Led_StdDialogHelper_AddURLXEmbeddingInfoDialog (HINSTANCE hInstance, HWND parentWnd,
                                                                                                const Characters::SDKChar* resID)
    : inherited (hInstance, resID, parentWnd)
    , fTitleText ()
    , fURLText ()
{
}
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
Led_StdDialogHelper_AddURLXEmbeddingInfoDialog::Led_StdDialogHelper_AddURLXEmbeddingInfoDialog (GtkWindow* parentWindow)
    : inherited (parentWindow)
    , fTitleTextWidget (NULL)
    , fURLTextWidget (NULL)
    , fTitleText ()
    , fURLText ()
{
}
#endif

void Led_StdDialogHelper_AddURLXEmbeddingInfoDialog::PreDoModalHook ()
{
#if qStroika_Foundation_Common_Platform_Windows
    (void)::SetDlgItemText (GetHWND (), kLedStdDlg_AddURLXEmbeddingInfoBox_TitleText, fTitleText.c_str ());
    (void)::SetDlgItemText (GetHWND (), kLedStdDlg_AddURLXEmbeddingInfoBox_URLText, fURLText.c_str ());
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
    // MUST FIX THIS FOR X-WINDOWS!!!!

    GtkWidget* window = GetWindow ();
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);

    string     message = "ADD URL.";
    GtkWidget* label   = gtk_label_new (message.c_str ());

    gtk_widget_show (label);

    /* a button to contain the pixmap widget */
    GtkWidget* button = gtk_button_new_with_label ("OK");
    GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
    gtk_widget_show (button);
    SetOKButton (button);
#endif
    inherited::PreDoModalHook ();
}

void Led_StdDialogHelper_AddURLXEmbeddingInfoDialog::OnOK ()
{
#if qStroika_Foundation_Common_Platform_Windows
    Characters::SDKChar bufText[1024];
    (void)::GetDlgItemText (GetHWND (), kLedStdDlg_AddURLXEmbeddingInfoBox_TitleText, bufText, static_cast<int> (Memory::NEltsOf (bufText)));
    fTitleText = bufText;
    (void)::GetDlgItemText (GetHWND (), kLedStdDlg_AddURLXEmbeddingInfoBox_URLText, bufText, static_cast<int> (Memory::NEltsOf (bufText)));
    fURLText = bufText;
#endif
    inherited::OnOK ();
}
#endif

#if qSupportAddNewTableDlg
/*
 ********************************************************************************
 ********************* Led_StdDialogHelper_AddNewTableDialog ********************
 ********************************************************************************
 */
#if qStroika_Foundation_Common_Platform_Windows
Led_StdDialogHelper_AddNewTableDialog::Led_StdDialogHelper_AddNewTableDialog (HINSTANCE hInstance, HWND parentWnd, const Characters::SDKChar* resID)
    : inherited (hInstance, resID, parentWnd)
    , fRows (0)
    , fColumns (0)
{
}
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
Led_StdDialogHelper_AddNewTableDialog::Led_StdDialogHelper_AddNewTableDialog (GtkWindow* parentWindow)
    : inherited (parentWindow)
    , fRows (0)
    , fColumns (0)
{
}
#endif

void Led_StdDialogHelper_AddNewTableDialog::PreDoModalHook ()
{
#if qStroika_Foundation_Common_Platform_Windows
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
#endif
#if qStroika_Foundation_Common_Platform_Windows
    SetItemText (kLedStdDlg_AddNewTableBox_RowCount, FormatINTAsString (static_cast<int> (fRows)));
    SetItemText (kLedStdDlg_AddNewTableBox_ColCount, FormatINTAsString (static_cast<int> (fColumns)));
    SetFocusedItem (kLedStdDlg_AddNewTableBox_RowCount);
    SelectItemText (kLedStdDlg_AddNewTableBox_RowCount);
#endif

    inherited::PreDoModalHook ();
}

void Led_StdDialogHelper_AddNewTableDialog::OnOK ()
{
#if qStroika_Foundation_Common_Platform_Windows
    int r = 0;
    int c = 0;
    if (ParseStringToINT (GetItemText (kLedStdDlg_AddNewTableBox_RowCount), &r) and
        ParseStringToINT (GetItemText (kLedStdDlg_AddNewTableBox_ColCount), &c) and r > 0 and c > 0 and r <= 100 and c <= 25) {
        fRows    = r;
        fColumns = c;
        inherited::OnOK ();
    }
    else {
        Led_BeepNotify ();
    }
#endif
}
#endif

#if qSupportEditTablePropertiesDlg
/*
 ********************************************************************************
 ***************** Led_StdDialogHelper_EditTablePropertiesDialog ****************
 ********************************************************************************
 */
#if qStroika_Foundation_Common_Platform_Windows
Led_StdDialogHelper_EditTablePropertiesDialog::Led_StdDialogHelper_EditTablePropertiesDialog (HINSTANCE hInstance, HWND parentWnd,
                                                                                              const Characters::SDKChar* resID)
    : inherited (hInstance, resID, parentWnd)
    , fInfo ()
    , fBorderColorPopup (false)
    , fCellBackgroundColorPopup (true)
{
}
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
Led_StdDialogHelper_EditTablePropertiesDialog::Led_StdDialogHelper_EditTablePropertiesDialog (GtkWindow* parentWindow)
    : inherited (parentWindow)
    , fInfo ()
    , fBorderColorPopup (false)
    , fCellBackgroundColorPopup (true)
{
}
#endif

void Led_StdDialogHelper_EditTablePropertiesDialog::PreDoModalHook ()
{
#if qStroika_Foundation_Common_Platform_Windows
    fBorderColorPopup.Attach (::GetDlgItem (GetHWND (), kLedStdDlg_EditTablePropertiesBox_BorderColor));
    fCellBackgroundColorPopup.Attach (::GetDlgItem (GetHWND (), kLedStdDlg_EditTablePropertiesBox_CellBackgroundColor));
#endif

    fBorderColorPopup.SetSelectedColor (fInfo.fTableBorderColor);
    if (fInfo.fCellBackgroundColor_Common) {
        fCellBackgroundColorPopup.SetSelectedColor (fInfo.fCellBackgroundColor);
    }
    else {
        fCellBackgroundColorPopup.SetNoSelectedColor ();
    }

#if qStroika_Foundation_Common_Platform_Windows
    SetItemText (kLedStdDlg_EditTablePropertiesBox_BorderWidth, FormatINTAsString (fInfo.fTableBorderWidth));

    SetItemText (kLedStdDlg_EditTablePropertiesBox_CellMarginTop, FormatINTAsString (fInfo.fDefaultCellMargins.top));
    SetItemText (kLedStdDlg_EditTablePropertiesBox_CellMarginLeft, FormatINTAsString (fInfo.fDefaultCellMargins.left));
    SetItemText (kLedStdDlg_EditTablePropertiesBox_CellMarginBottom, FormatINTAsString (fInfo.fDefaultCellMargins.bottom));
    SetItemText (kLedStdDlg_EditTablePropertiesBox_CellMarginRight, FormatINTAsString (fInfo.fDefaultCellMargins.right));

    SetItemText (kLedStdDlg_EditTablePropertiesBox_DefaultCellSpacing, FormatINTAsString (fInfo.fCellSpacing));

    if (fInfo.fCellWidth_Common) {
        SetItemText (kLedStdDlg_EditTablePropertiesBox_ColumnWidth, FormatINTAsString (fInfo.fCellWidth));
    }
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
#endif
#if qStroika_Foundation_Common_Platform_Windows
    SetFocusedItem (kLedStdDlg_EditTablePropertiesBox_BorderWidth);
    SelectItemText (kLedStdDlg_EditTablePropertiesBox_BorderWidth);
#endif
    inherited::PreDoModalHook ();
}

#if qStroika_Foundation_Common_Platform_Windows
BOOL Led_StdDialogHelper_EditTablePropertiesDialog::DialogProc (UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_COMMAND: {
            if (HIWORD (wParam) == CBN_SELCHANGE) {
                int controlID = LOWORD (wParam);
                switch (controlID) {
                    case kLedStdDlg_EditTablePropertiesBox_BorderColor:
                        fBorderColorPopup.OnSelChange ();
                        break;
                    case kLedStdDlg_EditTablePropertiesBox_CellBackgroundColor:
                        fCellBackgroundColorPopup.OnSelChange ();
                        break;
                }
            }
        } break;
    }
    return inherited::DialogProc (message, wParam, lParam);
}
#endif

void Led_StdDialogHelper_EditTablePropertiesDialog::OnOK ()
{
#if qStroika_Foundation_Common_Platform_Windows
    Info result    = fInfo;
    bool dataValid = true;

    dataValid = dataValid and ParseStringToTWIPS (GetItemText (kLedStdDlg_EditTablePropertiesBox_BorderWidth), &result.fTableBorderWidth) and
                result.fTableBorderWidth >= 0 and result.fTableBorderWidth <= 1440;

    dataValid = dataValid and ParseStringToTWIPS (GetItemText (kLedStdDlg_EditTablePropertiesBox_CellMarginTop), &result.fDefaultCellMargins.top) and
                result.fDefaultCellMargins.top >= 0 and result.fDefaultCellMargins.top <= 2 * 1440;
    dataValid = dataValid and
                ParseStringToTWIPS (GetItemText (kLedStdDlg_EditTablePropertiesBox_CellMarginLeft), &result.fDefaultCellMargins.left) and
                result.fDefaultCellMargins.left >= 0 and result.fDefaultCellMargins.left <= 2 * 1440;
    dataValid = dataValid and
                ParseStringToTWIPS (GetItemText (kLedStdDlg_EditTablePropertiesBox_CellMarginBottom), &result.fDefaultCellMargins.bottom) and
                result.fDefaultCellMargins.bottom >= 0 and result.fDefaultCellMargins.bottom <= 2 * 1440;
    dataValid = dataValid and
                ParseStringToTWIPS (GetItemText (kLedStdDlg_EditTablePropertiesBox_CellMarginRight), &result.fDefaultCellMargins.right) and
                result.fDefaultCellMargins.right >= 0 and result.fDefaultCellMargins.right <= 2 * 1440;

    dataValid = dataValid and ParseStringToTWIPS (GetItemText (kLedStdDlg_EditTablePropertiesBox_DefaultCellSpacing), &result.fCellSpacing) and
                result.fCellSpacing >= 0 and result.fCellSpacing <= 2 * 1440;

    result.fCellWidth_Common = ParseStringToTWIPS (GetItemText (kLedStdDlg_EditTablePropertiesBox_ColumnWidth), &result.fCellWidth) and
                               result.fCellWidth >= 50 and result.fCellWidth <= 8 * 1440;

    result.fCellBackgroundColor_Common = fCellBackgroundColorPopup.GetSelectedColor (&result.fCellBackgroundColor);
    (void)fBorderColorPopup.GetSelectedColor (&result.fTableBorderColor);

    if (dataValid) {
        fInfo = result;
        inherited::OnOK ();
    }
    else {
        Led_BeepNotify ();
    }
#endif
}
#endif

#if qSupportStdSpellCheckDlg
/*
 ********************************************************************************
 ************************ Led_StdDialogHelper_SpellCheckDialog ******************
 ********************************************************************************
 */
#if qStroika_Foundation_Common_Platform_Windows
Led_StdDialogHelper_SpellCheckDialog::Led_StdDialogHelper_SpellCheckDialog (SpellCheckDialogCallback& callback, HINSTANCE hInstance,
                                                                            HWND parentWnd, const Characters::SDKChar* resID)
    : inherited (hInstance, resID, parentWnd)
    , fCallback (callback)
    , fCurrentMisspellInfo (NULL)
#if qSupportLedDialogWidgets
    , fUndefinedWordWidget ()
    , fChangeTextWidget ()
#endif
{
}
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
Led_StdDialogHelper_SpellCheckDialog::Led_StdDialogHelper_SpellCheckDialog (SpellCheckDialogCallback& callback, GtkWindow* parentWindow)
    : inherited (parentWindow)
    , fCallback (callback)
    , fCurrentMisspellInfo (NULL)
    , fLookupTextWidget (NULL)
    , fChangeTextWidget (NULL)
{
}
#endif

Led_StdDialogHelper_SpellCheckDialog::~Led_StdDialogHelper_SpellCheckDialog ()
{
    delete fCurrentMisspellInfo;
}

#if qStroika_Foundation_Common_Platform_Windows
BOOL Led_StdDialogHelper_SpellCheckDialog::DialogProc (UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_COMMAND: {
            WORD notificationCode = HIWORD (wParam);
            WORD itemID           = LOWORD (wParam);
            switch (itemID) {
                case kLedStdDlg_SpellCheckBox_IgnoreOnce:
                    OnIgnoreButton ();
                    return true;
                case kLedStdDlg_SpellCheckBox_IgnoreAll:
                    OnIgnoreAllButton ();
                    return true;
                case kLedStdDlg_SpellCheckBox_ChangeOnce:
                    OnChangeButton ();
                    return true;
                case kLedStdDlg_SpellCheckBox_ChangeAll:
                    OnChangeAllButton ();
                    return true;
                case kLedStdDlg_SpellCheckBox_AddDictionary:
                    OnAddToDictionaryButton ();
                    return true;
                case kLedStdDlg_SpellCheckBox_LookupOnWeb:
                    OnLookupOnWebButton ();
                    return true;
                case kLedStdDlg_SpellCheckBox_Options:
                    OnOptionsDialogButton ();
                    return true;
                case kLedStdDlg_SpellCheckBox_Close:
                    OnCloseButton ();
                    return true;
                case kLedStdDlg_SpellCheckBox_SuggestedList: {
                    switch (notificationCode) {
                        case LBN_DBLCLK:
                            OnSuggestionListDoubleClick ();
                            return true;
                        case LBN_SELCHANGE:
                            OnSuggestionListChangeSelection ();
                            return true;
                    }
                    // fall through and do default...
                }
            }
        }
    }
    return inherited::DialogProc (message, wParam, lParam);
}
#endif

void Led_StdDialogHelper_SpellCheckDialog::PreDoModalHook ()
{
#if qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
    GtkWidget* actionArea = GTK_DIALOG (GetWindow ())->action_area;
    {
        fLookupTextWidget = gtk_entry_new ();
        gtk_container_add (GTK_CONTAINER (actionArea), fLookupTextWidget);
        gtk_widget_show (fLookupTextWidget);
        fChangeTextWidget = gtk_entry_new ();
        gtk_container_add (GTK_CONTAINER (actionArea), fChangeTextWidget);
        gtk_widget_show (fChangeTextWidget);
        //          gtk_entry_set_text (GTK_ENTRY (fLookupTextWidget), Led_tString2SDKString (fUndefinedWordText).c_str ());
        //          gtk_entry_select_region (GTK_ENTRY (fLookupTextWidget), 0, -1);
        //          gtk_widget_grab_focus (fLookupTextWidget);
    }
    {
        GtkWidget* button = gtk_button_new_with_label ("Ignore");
        gtk_container_add (GTK_CONTAINER (actionArea), button);
        GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
        gtk_widget_show (button);
        gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (Static_OnIgnoreButtonClick), (gpointer)this);
    }
    {
        GtkWidget* button = gtk_button_new_with_label ("Ignore All");
        gtk_container_add (GTK_CONTAINER (actionArea), button);
        GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
        gtk_widget_show (button);
        gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (Static_OnIgnoreAllButtonClick), (gpointer)this);
    }
    {
        GtkWidget* button = gtk_button_new_with_label ("Change");
        gtk_container_add (GTK_CONTAINER (actionArea), button);
        GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
        gtk_widget_show (button);
        gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (Static_OnChangeButtonClick), (gpointer)this);
    }
    {
        GtkWidget* button = gtk_button_new_with_label ("Change All");
        gtk_container_add (GTK_CONTAINER (actionArea), button);
        GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
        gtk_widget_show (button);
        gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (Static_OnChangeAllButtonClick), (gpointer)this);
    }
    {
        GtkWidget* button = gtk_button_new_with_label ("Add to Dictionary");
        gtk_container_add (GTK_CONTAINER (actionArea), button);
        GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
        gtk_widget_show (button);
        gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (Static_OnAddToDictionaryButtonClick), (gpointer)this);
    }
    {
        GtkWidget* button = gtk_button_new_with_label ("Lookup on Web");
        gtk_container_add (GTK_CONTAINER (actionArea), button);
        GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
        gtk_widget_show (button);
        gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (Static_OnLookupOnWebButtonClick), (gpointer)this);
    }
    {
        GtkWidget* button = gtk_button_new_with_label ("Options...");
        gtk_container_add (GTK_CONTAINER (actionArea), button);
        GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
        gtk_widget_show (button);
        gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (Static_OnOptionsDialogButtonClick), (gpointer)this);
    }
    {
        GtkWidget* button = gtk_button_new_with_label ("Close");
        gtk_container_add (GTK_CONTAINER (actionArea), button);
        GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
        gtk_widget_show (button);
        gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (Static_OnCloseButtonClick), (gpointer)this);
    }
#endif

#if qStroika_Foundation_Common_Platform_Windows && qSupportLedDialogWidgets
    /*
     *  ReplaceWindow seems to work better than SubclassWindow - for reasons I don't FULLY understand.
     *  (see SPR#1266).
     */
    fUndefinedWordWidget.ReplaceWindow (::GetDlgItem (GetHWND (), kLedStdDlg_SpellCheckBox_UnknownWordText));
    fChangeTextWidget.ReplaceWindow (::GetDlgItem (GetHWND (), kLedStdDlg_SpellCheckBox_ChangeText));
#endif

#if qStroika_Foundation_Common_Platform_Windows
    SetItemEnabled (kLedStdDlg_SpellCheckBox_Options, fCallback.OptionsDialogEnabled ());
#endif

    DoFindNextCall ();

    inherited::PreDoModalHook ();
}

void Led_StdDialogHelper_SpellCheckDialog::OnIgnoreButton ()
{
    fCallback.DoIgnore ();
    DoFindNextCall ();
}

void Led_StdDialogHelper_SpellCheckDialog::OnIgnoreAllButton ()
{
    fCallback.DoIgnoreAll ();
    DoFindNextCall ();
}

void Led_StdDialogHelper_SpellCheckDialog::OnChangeButton ()
{
#if qSupportLedDialogWidgets
    Led_tString changeText = fChangeTextWidget.GetText ();
#elif qStroika_Foundation_Common_Platform_Windows
    Led_tString changeText = Led_SDKString2tString (GetItemText (kLedStdDlg_SpellCheckBox_ChangeText));
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
    Led_tString changeText = Led_SDKString2tString (GetItemText (fChangeTextWidget));
#endif
    fCallback.DoChange (changeText);
    DoFindNextCall ();
}

void Led_StdDialogHelper_SpellCheckDialog::OnChangeAllButton ()
{
#if qSupportLedDialogWidgets
    Led_tString changeText = fChangeTextWidget.GetText ();
#elif qStroika_Foundation_Common_Platform_Windows
    Led_tString changeText = Led_SDKString2tString (GetItemText (kLedStdDlg_SpellCheckBox_ChangeText));
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
    Led_tString changeText = Led_SDKString2tString (GetItemText (fChangeTextWidget));
#endif
    fCallback.DoChangeAll (changeText);
    DoFindNextCall ();
}

void Led_StdDialogHelper_SpellCheckDialog::OnAddToDictionaryButton ()
{
#if qSupportLedDialogWidgets
    Led_tString undefinedWordText = fUndefinedWordWidget.GetText ();
#elif qStroika_Foundation_Common_Platform_Windows
    Led_tString undefinedWordText = Led_SDKString2tString (GetItemText (kLedStdDlg_SpellCheckBox_UnknownWordText));
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
    Led_tString undefinedWordText = Led_SDKString2tString (GetItemText (fLookupTextWidget));
#endif
    fCallback.AddToDictionary (undefinedWordText);
    DoFindNextCall ();
}

void Led_StdDialogHelper_SpellCheckDialog::OnLookupOnWebButton ()
{
#if qSupportLedDialogWidgets
    Led_tString undefinedWordText = fUndefinedWordWidget.GetText ();
#elif qStroika_Foundation_Common_Platform_Windows
    Led_tString undefinedWordText = Led_SDKString2tString (GetItemText (kLedStdDlg_SpellCheckBox_UnknownWordText));
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
    Led_tString undefinedWordText = Led_SDKString2tString (GetItemText (fLookupTextWidget));
#endif
    fCallback.LookupOnWeb (undefinedWordText);
}

void Led_StdDialogHelper_SpellCheckDialog::OnOptionsDialogButton ()
{
    fCallback.OptionsDialog ();
}

void Led_StdDialogHelper_SpellCheckDialog::OnCloseButton ()
{
    OnCancel ();
}

void Led_StdDialogHelper_SpellCheckDialog::OnSuggestionListChangeSelection ()
{
    if (fCurrentMisspellInfo != NULL) {
#if qStroika_Foundation_Common_Platform_Windows
        DialogItemID changeTextItem = kLedStdDlg_SpellCheckBox_ChangeText;
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
        DialogItemID changeTextItem = fChangeTextWidget;
#endif

#if qStroika_Foundation_Common_Platform_Windows
        LRESULT itemSelResult = ::SendMessage (GetDlgItem (GetHWND (), kLedStdDlg_SpellCheckBox_SuggestedList), LB_GETCURSEL, 0, 0);
#else
        Assert (false); // REALLY NYI!!!
        int itemSelResult = 0;
#endif
        if (itemSelResult >= 0 and static_cast<size_t> (itemSelResult) < fCurrentMisspellInfo->fSuggestions.size ()) {
            Led_tString changeText = fCurrentMisspellInfo->fSuggestions[itemSelResult];
#if qSupportLedDialogWidgets
            fChangeTextWidget.SetText (changeText);
#else
            SetItemText (changeTextItem, Led_tString2SDKString (changeText));
#endif
            SelectItemText (changeTextItem);
        }
    }
}

void Led_StdDialogHelper_SpellCheckDialog::OnSuggestionListDoubleClick ()
{
    OnChangeButton ();
}

void Led_StdDialogHelper_SpellCheckDialog::DoFindNextCall ()
{
    delete fCurrentMisspellInfo;
    fCurrentMisspellInfo = NULL;
    fCurrentMisspellInfo = fCallback.GetNextMisspelling ();

#if qStroika_Foundation_Common_Platform_Windows
    DialogItemID undefinedTextItem = kLedStdDlg_SpellCheckBox_UnknownWordText;
    DialogItemID changeTextItem    = kLedStdDlg_SpellCheckBox_ChangeText;
#elif qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
    DialogItemID undefinedTextItem = fLookupTextWidget;
    DialogItemID changeTextItem    = fChangeTextWidget;
#endif

#if qStroika_Foundation_Common_Platform_Windows
    ::SendMessage (GetDlgItem (GetHWND (), kLedStdDlg_SpellCheckBox_SuggestedList), LB_RESETCONTENT, 0, 0);
#endif

    Led_tString undefinedWordText;
    Led_tString changeText;
    if (fCurrentMisspellInfo != NULL) {
        undefinedWordText = fCurrentMisspellInfo->fUndefinedWord;
        if (not fCurrentMisspellInfo->fSuggestions.empty ()) {
            changeText = fCurrentMisspellInfo->fSuggestions[0];
#if qStroika_Foundation_Common_Platform_Windows
            for (auto i = fCurrentMisspellInfo->fSuggestions.begin (); i != fCurrentMisspellInfo->fSuggestions.end (); ++i) {
                ::SendMessage (GetDlgItem (GetHWND (), kLedStdDlg_SpellCheckBox_SuggestedList), LB_ADDSTRING, 0,
                               reinterpret_cast<LPARAM> (Led_tString2SDKString (*i).c_str ()));
            }
#endif
        }
    }

#if qSupportLedDialogWidgets
    fUndefinedWordWidget.SetText (undefinedWordText);
    fChangeTextWidget.SetText (changeText);
#else
    SetItemText (undefinedTextItem, Led_tString2SDKString (undefinedWordText));
    SetItemText (changeTextItem, Led_tString2SDKString (changeText));
#endif

#if qStroika_Foundation_Common_Platform_Windows
    SetItemEnabled (kLedStdDlg_SpellCheckBox_IgnoreOnce, fCurrentMisspellInfo != NULL);
    SetItemEnabled (kLedStdDlg_SpellCheckBox_IgnoreAll, fCurrentMisspellInfo != NULL);
    SetItemEnabled (kLedStdDlg_SpellCheckBox_ChangeOnce, fCurrentMisspellInfo != NULL);
    SetItemEnabled (kLedStdDlg_SpellCheckBox_ChangeAll, fCurrentMisspellInfo != NULL);
    SetItemEnabled (kLedStdDlg_SpellCheckBox_AddDictionary, fCurrentMisspellInfo != NULL and fCallback.AddToDictionaryEnabled ());
#endif

    SelectItemText (undefinedTextItem);
    SelectItemText (changeTextItem);
    SetFocusedItem (changeTextItem);
}

#if qStroika_FeatureSupported_XWindows && qUseGTKForLedStandardDialogs
void Led_StdDialogHelper_SpellCheckDialog::Static_OnIgnoreButtonClick (GtkWidget* widget, gpointer data)
{
    Led_StdDialogHelper_SpellCheckDialog* dlg = reinterpret_cast<Led_StdDialogHelper_SpellCheckDialog*> (data);
    dlg->OnIgnoreButton ();
}

void Led_StdDialogHelper_SpellCheckDialog::Static_OnIgnoreAllButtonClick (GtkWidget* widget, gpointer data)
{
    Led_StdDialogHelper_SpellCheckDialog* dlg = reinterpret_cast<Led_StdDialogHelper_SpellCheckDialog*> (data);
    dlg->OnIgnoreAllButton ();
}

void Led_StdDialogHelper_SpellCheckDialog::Static_OnChangeButtonClick (GtkWidget* widget, gpointer data)
{
    Led_StdDialogHelper_SpellCheckDialog* dlg = reinterpret_cast<Led_StdDialogHelper_SpellCheckDialog*> (data);
    dlg->OnChangeButton ();
}

void Led_StdDialogHelper_SpellCheckDialog::Static_OnChangeAllButtonClick (GtkWidget* widget, gpointer data)
{
    Led_StdDialogHelper_SpellCheckDialog* dlg = reinterpret_cast<Led_StdDialogHelper_SpellCheckDialog*> (data);
    dlg->OnChangeAllButton ();
}

void Led_StdDialogHelper_SpellCheckDialog::Static_OnAddToDictionaryButtonClick (GtkWidget* widget, gpointer data)
{
    Led_StdDialogHelper_SpellCheckDialog* dlg = reinterpret_cast<Led_StdDialogHelper_SpellCheckDialog*> (data);
    dlg->OnAddToDictionaryButton ();
}

void Led_StdDialogHelper_SpellCheckDialog::Static_OnLookupOnWebButtonClick (GtkWidget* widget, gpointer data)
{
    Led_StdDialogHelper_SpellCheckDialog* dlg = reinterpret_cast<Led_StdDialogHelper_SpellCheckDialog*> (data);
    dlg->OnLookupOnWebButton ();
}

void Led_StdDialogHelper_SpellCheckDialog::Static_OnOptionsDialogButtonClick (GtkWidget* widget, gpointer data)
{
    Led_StdDialogHelper_SpellCheckDialog* dlg = reinterpret_cast<Led_StdDialogHelper_SpellCheckDialog*> (data);
    dlg->OnOptionsDialogButton ();
}

void Led_StdDialogHelper_SpellCheckDialog::Static_OnCloseButtonClick (GtkWidget* widget, gpointer data)
{
    Led_StdDialogHelper_SpellCheckDialog* dlg = reinterpret_cast<Led_StdDialogHelper_SpellCheckDialog*> (data);
    dlg->OnCloseButton ();
}
#endif

#endif
