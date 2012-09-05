#ifndef BCBLedTestH
#define BCBLedTestH

#include "Command.h"
#include "Led_Win32.h"
#include "SimpleTextStore.h"
#include "WordProcessor.h"
#include "SimpleLed.h"

using namespace Led;


class	MyLedWindow : public SimpleLedWordProcessor {
	private:
		typedef	SimpleLedWordProcessor	inherited;
	public:
		override	LRESULT	WndProc (UINT message, WPARAM wParam, LPARAM lParam);

	public:
		MyLedWindow ():
			inherited ()
			{
				SetScrollBarType (h, eScrollBarAsNeeded);
				SetScrollBarType (v, eScrollBarAlways);
			}
};

extern MyLedWindow gMyWindow;

extern HINSTANCE hInst;
extern TCHAR szTitle[];								// The title bar text
extern TCHAR szWindowClass[];								// The title bar text

#endif BCBLedTestH
