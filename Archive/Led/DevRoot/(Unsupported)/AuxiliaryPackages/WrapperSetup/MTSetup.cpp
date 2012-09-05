// MTSetup.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <windows.h>
#include "MTSetup.h"
#include <stdlib.h>

DWORD				g_dwOSVerFlags			= 0;
DWORD				g_dwInitRebootReason	= 0;
CRITICAL_SECTION	g_cs;
DWORD				g_dwMainThreadId = 0;
HANDLE				g_hAppMutex;

char*	gCmdLine	=	NULL;
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG			msg;
	DWORD		dwThreadId;
	HANDLE		hThread;
	
	__try
	{
		//g_dwOSVerFlags = GetOperatingSystemVer();
		if(!lstrcmp(__argv[1],"DCOMREBOOT"))
		{
			LPTSTR szCmdLinePath = SetStartupPath(_strdup(__argv[0]));
			// DCOM Caused reboot
			
			if(szCmdLinePath != NULL && !SetCurrentDirectory(szCmdLinePath) )
				MessageBox(NULL, "SetCurrentDirectory Failed.", "Error", MB_ICONERROR);	
			g_bDCOMReboot = TRUE;
			delete szCmdLinePath;
		}
		else if(!lstrcmp(__argv[1],"MDACREBOOT"))
		{
			// MDAC Caused reboot
			LPTSTR szCmdLinePath = SetStartupPath(_strdup(__argv[0]));

			if(!SetCurrentDirectory(szCmdLinePath))
				MessageBox(NULL, TEXT("SetCurrentDirectory Failed"), g_szAppName, MB_ICONERROR);
				//SetCurrentDirectory(szCmdLinePath);
			g_bMDACReboot = TRUE;
			delete szCmdLinePath;
		}
		else
		{
			gCmdLine = strdup (lpCmdLine);
			// Write CmdLine to Reg
			RegWriteCmdLine(lpCmdLine);
		}

 		if(!Init(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
			return 0;

		hThread = CreateThread(
							NULL,
							0,
							(LPTHREAD_START_ROUTINE)InstallThread,
							NULL,
							0,
							&dwThreadId);
							

		while(GetMessage(&msg, NULL, 0, 0))
		{
			__try
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				// Log Exception
				MessageBox(NULL, TEXT("An exception has occured in MsgLoop"), g_szAppName, MB_ICONERROR);
			}
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		// Log Exception
		MessageBox(NULL, TEXT("An excption has occured in Wrapper.exe"), g_szAppName, MB_ICONERROR);
	}

	return msg.wParam;
}
