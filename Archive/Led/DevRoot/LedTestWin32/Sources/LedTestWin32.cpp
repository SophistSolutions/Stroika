/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/LedTestWin32/Sources/LedTestWin32.cpp,v 1.22 2003/04/09 16:13:29 lewis Exp $
 *
 * Description:
 *
 *
 * Changes:
 *	$Log: LedTestWin32.cpp,v $
 *	Revision 1.22  2003/04/09 16:13:29  lewis
 *	get rid of gMyWindow global altogether. Not needed and obfuscates the example code
 *	
 *	Revision 1.21  2003/04/04 16:47:35  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.20  2003/03/24 23:42:54  lewis
 *	SPR#1376- Use NCDestroy message to destroy MyLedWindow instead of static file scope 
 *	variable. Define StandardUnknownTypeStyleMarker::sUnknownPict to be a valid pict.
 *	
 *	Revision 1.19  2003/02/28 19:01:16  lewis
 *	SPR#1316 - added DebugMemoryLeaks configuration
 *	
 *	Revision 1.18  2003/02/24 21:07:13  lewis
 *	small cahnge cuz Led_Win32 helper class now defines DefWindowProc
 *	
 *	Revision 1.17  2002/09/19 14:21:19  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.16  2002/09/19 02:08:49  lewis
 *	SPR#1103 - redo using SimpleLed
 *	
 *	Revision 1.15  2002/05/06 21:36:36  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 1.14  2001/11/27 00:33:29  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.13  2001/08/30 01:09:23  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.12  2001/07/19 21:07:11  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.11  2000/10/06 13:01:37  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.10  2000/09/30 19:38:00  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.9  2000/05/01 22:04:12  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.8  2000/04/04 20:50:19  lewis
 *	SPR# 0732- added  Led_Set_OutOfMemoryException_Handler/Led_Set_BadFormatDataException_Handler/
 *	Led_Set_ThrowOSErrException_Handler. And qGenerateStandardPowerPlantExceptions and
 *	qGenerateStandardMFCExceptions defines. The idea is to make it easier to override these
 *	exception throw formats, and get better default behavior for cases other than MFC and PowerPlant.
 *	
 *	Revision 1.7  2000/01/22 22:59:24  lewis
 *	added Create() method - you should call this instead of ::CreateWindow(). Then hook WM_CREATE
 *	messsage so SetHWND () method called automatically - This was SPR#0633
 *	
 *	Revision 1.6  1999/12/09 17:35:54  lewis
 *	lots of cleanups/fixes for new (partial/full) UNICODE support - SPR#0645
 *	
 *	Revision 1.5  1999/11/29 23:27:24  lewis
 *	added cut/copy/paste/undo/redo commands - spr#0635
 *	
 *	Revision 1.4  1999/11/29 22:37:56  lewis
 *	cleanup
 *	
 *	Revision 1.3  1999/11/29 19:00:07  lewis
 *	no more need for 'HelloWorld' hack text. And now set default sbar values
 *	
 *	Revision 1.2  1999/11/28 17:25:07  lewis
 *	futz enuf so that we can display a minimal - HelloWorld Led-based display
 *	
 *	Revision 1.1  1999/11/15 22:16:59  lewis
 *	Initial revision
 *	
 *
 *
 */

#include	"StdAfx.h"

#include	<windows.h>

#include	"Resource.h"




#include	"Command.h"
#include	"Led_Win32.h"
#include	"SimpleTextStore.h"
#include	"SimpleLed.h"
#include	"WordProcessor.h"

#include	"LedTestWin32.h"



#define	qBuildWP	1

#if		qBuildWP
	typedef	SimpleLedWordProcessor	_BASE_;
#else
	typedef	SimpleLedLineEditor		_BASE_;
#endif


class	MyLedWindow : public _BASE_ {
	private:
		typedef	_BASE_	inherited;
	public:
		override	LRESULT	WndProc (UINT message, WPARAM wParam, LPARAM lParam);

	public:
		MyLedWindow ():
			inherited ()
			{
				SetScrollBarType (h, eScrollBarAsNeeded);
				SetScrollBarType (v, eScrollBarAlways);
			}
		override	void	OnNCDestroy_Msg ();
};




void	MyLedWindow::OnNCDestroy_Msg ()
{
	inherited::OnNCDestroy_Msg ();
	delete this;
}



#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text


inline	const	void*	LoadAppResource (HINSTANCE hIntstance, long resID, LPCTSTR resType)
	{
		HRSRC	hrsrc	=	::FindResource (hIntstance, MAKEINTRESOURCE (resID), resType);
		Led_AssertNotNil (hrsrc);
		HGLOBAL	hglobal	=	::LoadResource (hIntstance, hrsrc);
		const void*	lockedData	=	::LockResource (hglobal);
		Led_EnsureNotNil (lockedData);
		return (lockedData);
	}



// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	#if		CRTDBG_MAP_ALLOC
		_CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LEDTESTWIN32, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass (hInstance);

	StandardUnknownTypeStyleMarker::sUnknownPict = (const Led_DIB*)LoadAppResource (hInstance, kUnknownEmbeddingPictID, RT_BITMAP);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) {
		return FALSE;
	}

	// Main message loop:
	HACCEL	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_LEDTESTWIN32);
	MSG		msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
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
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)MyLedWindow::StaticWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_LEDTESTWIN32);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_LEDTESTWIN32;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
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

	MyLedWindow*	myWindow = new MyLedWindow ();
	myWindow->Create (szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance);
	HWND hWnd = myWindow->GetHWND ();

	ShowWindow (hWnd, nCmdShow);
	UpdateWindow (hWnd);

	return TRUE;
}


LRESULT	MyLedWindow::WndProc (UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case WM_COMMAND: {
			int	wmId    = LOWORD(wParam); 
			int	wmEvent = HIWORD(wParam); 
			HWND	hWnd	=	GetHWND ();
			// Parse the menu selections:
			switch (wmId) {
				case IDM_ABOUT:				DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About); break;
				case IDM_EXIT:				DestroyWindow(hWnd);	break;
				case ID_EDIT_UNDO:			{ if (GetCommandHandler ()->CanUndo ()) OnUndoCommand (); else Led_BeepNotify (); }	break;
				case ID_EDIT_REDO:			{ if (GetCommandHandler ()->CanRedo ()) OnRedoCommand (); else Led_BeepNotify (); } break;
				case ID_EDIT_CUT:			OnCutCommand ();	break;
				case ID_EDIT_COPY:			OnCopyCommand ();	break;
				case ID_EDIT_PASTE:			OnPasteCommand ();	break;
				case ID_EDIT_CLEAR:			OnClearCommand ();	break;
				case ID_EDIT_SELECT_ALL:	OnSelectAllCommand ();	break;
				default:					return DefWindowProc (message, wParam, lParam);
			}
		}
		break;
	
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return inherited::WndProc (message, wParam, lParam);
	}
	return 0;
}




// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}
