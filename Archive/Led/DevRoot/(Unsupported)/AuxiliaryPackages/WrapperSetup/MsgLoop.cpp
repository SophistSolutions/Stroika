// MsgLoop.cpp
#include "stdafx.h"
#include <windows.h>
#include "MTSetup.h"

extern DWORD				g_dwInitRebootReason;
extern CRITICAL_SECTION		g_cs;
extern DWORD				g_dwOSVerFlags;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int r = 0;
	DWORD dwRebootReason;
	switch(message)
	{
	case WM_CREATE:
		return 0;
	case WM_QUERYENDSESSION:
		EnterCriticalSection(&g_cs);
		dwRebootReason = g_dwInitRebootReason;
		LeaveCriticalSection(&g_cs);
		if((g_dwOSVerFlags & dwWin95) != dwWin95)
			SetRunOnceKey(g_dwInitRebootReason);
		return TRUE;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_ENDSESSION:
		return 0;
	case WM_CLOSE:
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
