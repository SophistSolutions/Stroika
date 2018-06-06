/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

#include "StdAfx.h"

#include <windows.h>

#include "Resource.h"

#include "Stroika/Frameworks/Led/Command.h"
#include "Stroika/Frameworks/Led/Platform/Windows.h"
#include "Stroika/Frameworks/Led/SimpleLed.h"
#include "Stroika/Frameworks/Led/SimpleTextStore.h"
#include "Stroika/Frameworks/Led/WordProcessor.h"

#include "Test.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::Platform;

#define qBuildWP 1

#if qBuildWP
typedef SimpleLedWordProcessor _BASE_;
#else
typedef SimpleLedLineEditor _BASE_;
#endif

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(4 : 4250)
#endif

class MyLedWindow : public _BASE_ {
private:
    typedef _BASE_ inherited;

public:
    virtual LRESULT WndProc (UINT message, WPARAM wParam, LPARAM lParam) override;

public:
    MyLedWindow ()
        : inherited ()
    {
        SetScrollBarType (h, eScrollBarAsNeeded);
        SetScrollBarType (v, eScrollBarAlways);
    }
    virtual void OnNCDestroy_Msg () override;
};
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif

void MyLedWindow::OnNCDestroy_Msg ()
{
    inherited::OnNCDestroy_Msg ();
    delete this;
}

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                         // current instance
TCHAR     szTitle[MAX_LOADSTRING];       // The title bar text
TCHAR     szWindowClass[MAX_LOADSTRING]; // The title bar text

inline const void* LoadAppResource (HINSTANCE hIntstance, long resID, LPCTSTR resType)
{
    HRSRC hrsrc = ::FindResource (hIntstance, MAKEINTRESOURCE (resID), resType);
    AssertNotNull (hrsrc);
    HGLOBAL     hglobal    = ::LoadResource (hIntstance, hrsrc);
    const void* lockedData = ::LockResource (hglobal);
    AssertNotNull (lockedData);
    return (lockedData);
}

// Foward declarations of functions included in this code module:
ATOM    MyRegisterClass (HINSTANCE hInstance);
BOOL    InitInstance (HINSTANCE, int);
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK About (HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain (HINSTANCE hInstance, [[maybe_unused]] HINSTANCE hPrevInstance, [[maybe_unused]] LPSTR lpCmdLine, int nCmdShow)
{
#if CRTDBG_MAP_ALLOC
    _CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    // Initialize global strings
    LoadString (hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString (hInstance, IDC_LEDTESTWIN32, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass (hInstance);

    StandardUnknownTypeStyleMarker::sUnknownPict = (const Led_DIB*)LoadAppResource (hInstance, kUnknownEmbeddingPictID, RT_BITMAP);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow)) {
        return FALSE;
    }

    // Main message loop:
    HACCEL hAccelTable = LoadAccelerators (hInstance, (LPCTSTR)IDC_LEDTESTWIN32);
    MSG    msg;
    while (GetMessage (&msg, NULL, 0, 0)) {
        if (!TranslateAccelerator (msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage (&msg);
            DispatchMessage (&msg);
        }
    }

    return msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass (HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof (WNDCLASSEX);

    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = (WNDPROC)MyLedWindow::StaticWndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = hInstance;
    wcex.hIcon         = LoadIcon (hInstance, (LPCTSTR)IDI_LEDTESTWIN32);
    wcex.hCursor       = LoadCursor (NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wcex.lpszMenuName  = (LPCTSTR)IDC_LEDTESTWIN32;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm       = LoadIcon (wcex.hInstance, (LPCTSTR)IDI_SMALL);

    return RegisterClassEx (&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance (HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    MyLedWindow* myWindow = new MyLedWindow ();
    myWindow->Create (szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance);
    HWND hWnd = myWindow->GetHWND ();

    ShowWindow (hWnd, nCmdShow);
    UpdateWindow (hWnd);

    return TRUE;
}

LRESULT MyLedWindow::WndProc (UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_COMMAND: {
            int  wmId    = LOWORD (wParam);
            [[maybe_unused]]int  wmEvent = HIWORD (wParam);
            HWND hWnd    = GetHWND ();
            // Parse the menu selections:
            switch (wmId) {
                case IDM_ABOUT:
                    DialogBox (hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
                    break;
                case IDM_EXIT:
                    DestroyWindow (hWnd);
                    break;
                case ID_EDIT_UNDO: {
                    if (GetCommandHandler ()->CanUndo ())
                        OnUndoCommand ();
                    else
                        Led_BeepNotify ();
                } break;
                case ID_EDIT_REDO: {
                    if (GetCommandHandler ()->CanRedo ())
                        OnRedoCommand ();
                    else
                        Led_BeepNotify ();
                } break;
                case ID_EDIT_CUT:
                    OnCutCommand ();
                    break;
                case ID_EDIT_COPY:
                    OnCopyCommand ();
                    break;
                case ID_EDIT_PASTE:
                    OnPasteCommand ();
                    break;
                case ID_EDIT_CLEAR:
                    OnClearCommand ();
                    break;
                case ID_EDIT_SELECT_ALL:
                    OnSelectAllCommand ();
                    break;
                default:
                    return DefWindowProc (message, wParam, lParam);
            }
        } break;

        case WM_DESTROY:
            PostQuitMessage (0);
            break;

        default:
            return inherited::WndProc (message, wParam, lParam);
    }
    return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About (HWND hDlg, UINT message, WPARAM wParam, [[maybe_unused]] LPARAM lParam)
{
    switch (message) {
        case WM_INITDIALOG:
            return TRUE;

        case WM_COMMAND:
            if (LOWORD (wParam) == IDOK || LOWORD (wParam) == IDCANCEL) {
                EndDialog (hDlg, LOWORD (wParam));
                return TRUE;
            }
            break;
    }
    return FALSE;
}
