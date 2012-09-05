//---------------------------------------------------------------------------
#ifndef WinUnitH
#define WinUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

class MyLedWindow;

class TForm1 : public TForm
{
__published:	// IDE-managed Components
  void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations
  TRichEdit *re;
  MyLedWindow *ledwin;

public:		// User declarations
  __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
