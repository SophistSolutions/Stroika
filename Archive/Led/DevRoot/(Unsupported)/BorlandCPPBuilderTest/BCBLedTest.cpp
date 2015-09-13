//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "BCBLedTest.h"
#include "WinUnit.h"

USERES("Sources\BCBLedTest.res");
USEFORM("Sources\WinUnit.cpp", Form1);
USEUNIT("..\Led\Sources\WordWrappedTextInteractor.cpp");
USEUNIT("..\Led\Sources\CodePage.cpp");
USEUNIT("..\Led\Sources\Command.cpp");
USEUNIT("..\Led\Sources\FlavorPackage.cpp");
USEUNIT("..\Led\Sources\HiddenText.cpp");
USEUNIT("..\Led\Sources\Led_Win32.cpp");
USEUNIT("..\Led\Sources\LedGDI.cpp");
USEUNIT("..\Led\Sources\LedHandySimple.cpp");
USEUNIT("..\Led\Sources\LedStdDialogs.cpp");
USEUNIT("..\Led\Sources\LedSupport.cpp");
USEUNIT("..\Led\Sources\LineBasedPartition.cpp");
USEUNIT("..\Led\Sources\Marker.cpp");
USEUNIT("..\Led\Sources\MarkerCover.cpp");
USEUNIT("..\Led\Sources\MultiRowTextImager.cpp");
USEUNIT("..\Led\Sources\PartitioningTextImager.cpp");
USEUNIT("..\Led\Sources\SimpleTextImager.cpp");
USEUNIT("..\Led\Sources\SimpleTextInteractor.cpp");
USEUNIT("..\Led\Sources\SimpleTextStore.cpp");
USEUNIT("..\Led\Sources\StandardStyledTextImager.cpp");
USEUNIT("..\Led\Sources\StandardStyledTextInteractor.cpp");
USEUNIT("..\Led\Sources\StyledTextEmbeddedObjects.cpp");
USEUNIT("..\Led\Sources\StyledTextImager.cpp");
USEUNIT("..\Led\Sources\StyledTextIO.cpp");
USEUNIT("..\Led\Sources\StyledTextIO_HTML.cpp");
USEUNIT("..\Led\Sources\StyledTextIO_LedNative.cpp");
USEUNIT("..\Led\Sources\StyledTextIO_MIMETextEnriched.cpp");
USEUNIT("..\Led\Sources\StyledTextIO_PlainText.cpp");
USEUNIT("..\Led\Sources\StyledTextIO_RTF.cpp");
USEUNIT("..\Led\Sources\StyledTextIO_STYLText.cpp");
USEUNIT("..\Led\Sources\SyntaxColoring.cpp");
USEUNIT("..\Led\Sources\TextBreaks.cpp");
USEUNIT("..\Led\Sources\TextImager.cpp");
USEUNIT("..\Led\Sources\TextInteractor.cpp");
USEUNIT("..\Led\Sources\TextInteractorMixins.cpp");
USEUNIT("..\Led\Sources\TextStore.cpp");
USEUNIT("..\Led\Sources\WordProcessor.cpp");
USEUNIT("..\Led\Sources\WordWrappedTextImager.cpp");
USEUNIT("..\Led\Sources\ChunkedArrayTextStore.cpp");
USEUNIT("..\Led\Sources\SimpleLed.cpp");
USEUNIT("..\Led\Sources\BiDiLayoutEngine.cpp");
USEUNIT("..\Led\Sources\IdleManager.cpp");
//---------------------------------------------------------------------------
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);


//MyLedWindow	gMyWindow;


WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
  hInst = hInstance;

  try
  {
     Application->Initialize();

     strcpy(szWindowClass, "LEDTESTWIN32");
     strcpy(szTitle, "LED!");
	   MyRegisterClass(hInstance);

     // CreateForm runs constructor
     Application->Title = "Borland C++ Led Test Application";
                 Application->CreateForm(__classid(TForm1), &Form1);
                 Application->Run();
  }
  catch (Exception &exception)
  {
     Application->ShowException(&exception);
  }
  return 0;
}
//---------------------------------------------------------------------------

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
	wcex.hIcon			= 0; //LoadIcon(hInstance, (LPCTSTR)IDI_LEDTESTWIN32);
	wcex.hCursor		= 0; //LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0; //(LPCTSTR)IDC_LEDTESTWIN32;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= 0; //LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}


LRESULT	MyLedWindow::WndProc (UINT message, WPARAM wParam, LPARAM lParam)
{
	try {
		switch (message) {
			case WM_COMMAND: {
				int	wmId    = LOWORD(wParam);
				int	wmEvent = HIWORD(wParam);
				// Parse the menu selections:
				switch (wmId) {
	/*				case IDM_ABOUT:				DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About); break;
					case IDM_EXIT:				DestroyWindow(hWnd);	break;
					case ID_EDIT_UNDO:			{ if (GetCommandHandler ()->CanUndo ()) OnUndoCommand (); else Led_BeepNotify (); }	break;
					case ID_EDIT_REDO:			{ if (GetCommandHandler ()->CanRedo ()) OnRedoCommand (); else Led_BeepNotify (); } break;
					case ID_EDIT_CUT:			OnCutCommand ();	break;
					case ID_EDIT_COPY:			OnCopyCommand ();	break;
					case ID_EDIT_PASTE:			OnPasteCommand ();	break;
					case ID_EDIT_CLEAR:			OnClearCommand ();	break;
					case ID_EDIT_SELECT_ALL:	OnSelectAllCommand ();	break;
	*/
					default:					return DefWindowProc(message, wParam, lParam);
				}
			}
			break;
	
			case WM_DESTROY:
				PostQuitMessage(0);
				break;
	
			default:
				return inherited::WndProc (message, wParam, lParam);
		}
	}
	catch (...) {
		Led_BeepNotify ();
	}
	return 0;
}

