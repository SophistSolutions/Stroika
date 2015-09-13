//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "BCBLedTest.h"
#include "WinUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
  : TForm(Owner)
{
  re = new TRichEdit(this);
  re->Parent = this;
  re->Left = 10;
  re->Top  = 10;
  re->Width = 300;
  re->Height = 300;

  ledwin = new MyLedWindow();
	ledwin->Create(szWindowClass, szTitle, WS_CHILD | WS_BORDER,
                 320, 10, 300, 300, Form1->Handle, NULL, hInst);
	HWND hWnd = ledwin->GetHWND ();

	ShowWindow (hWnd, SW_SHOW);
	UpdateWindow (hWnd);

}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormDestroy(TObject *Sender)
{
  delete ledwin;
  delete re;
}
//---------------------------------------------------------------------------


