// InstallThread.cpp

#include "stdafx.h"
#include <windows.h>
#include "MTSetup.h"

extern DWORD	g_dwInitRebootReason;
extern CRITICAL_SECTION	g_cs;
extern DWORD g_dwMainThreadId;
extern DWORD g_dwOSVerFlags;

DWORD WINAPI InstallThread(PVOID pvParam)
{
	
	DWORD dwRetVal = 0;
	__try
	{	
		BOOL bCreateProc = FALSE;
		DWORD dwOsVer = 0;
		DWORD cbCmdSize;
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		TCHAR lpGetCmdLine[MAX_PATH];
		TCHAR szCurrentDir[MAX_PATH];
		TCHAR szSystemDir[MAX_PATH];
		TCHAR szMsiPackage[MAX_PATH];

#if 0
		if(!IsDCOMPresent())
		{
			if(!g_bDCOMReboot) 
			{
				if(!InstallDCOM())
				{
					CleanReg();
					PostThreadMessage(g_dwMainThreadId, WM_QUIT, 0, 0);
					return 0;
				}
				else
				{
					return 0;
				}
			}
			else
			{
				MessageBox(NULL, TEXT("DCOM rebooted the system but the installation was unsuccessful"),
					g_szAppName, MB_ICONERROR);
				return dwRetVal;
			}
		}
#endif

		if(!InstallMSI())
		{
			MessageBox(
					NULL, 
					TEXT("There was an error while installing Microsoft Windows Installer"),
					g_szAppName,
					MB_ICONERROR);
			return 0;
		}
#if 0

		if(!IsMDACPresent())
		{
			if(!g_bMDACReboot)
			{
				// System did not previously reboot because of MDAC
				if(!InstallMDAC())
				{
					// close app
					CleanReg();
					PostThreadMessage(g_dwMainThreadId, WM_QUIT, 0, 0);
					//ExitThread(0);
					ExitProcess(0);
				}
				else
				{
					return 0;
				}

			}
			else
			{	
				MessageBox(NULL, 
						TEXT("MDAC previously rebooted the system but the installation was unsuccessful"),
						g_szAppName, 
						MB_ICONERROR);
				return 0;
			}
		}
#endif

	
		//Check for Admin privledges here
		if ((g_dwOSVerFlags & dwWinNT) == dwWinNT && !IsAdmin()) {
			MessageBox(
				NULL, 
				TEXT("Installing this software without running as Administrator under WinNT may fail"), 
				g_szAppName, 
				MB_OK);
		}

		//
		// Install package.
		//
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		cbCmdSize = sizeof(lpGetCmdLine);
		RegReadCmdLine(lpGetCmdLine, &cbCmdSize);
		GetCurrentDirectory(sizeof(szCurrentDir), szCurrentDir );
		GetSystemDirectory(szSystemDir, MAX_PATH);
		extern	char*	gCmdLine;
		if (gCmdLine == NULL) {
			wsprintf(szMsiPackage, TEXT("%s\\msiexec.exe %s"),szSystemDir, lpGetCmdLine);
		}
		else {
			wsprintf(szMsiPackage, TEXT("%s\\msiexec.exe %s"),szSystemDir, gCmdLine);
		}


		bCreateProc = CreateProcess(NULL,
					szMsiPackage,
					NULL,
					NULL,
					FALSE,
					0,   
					NULL,
					szCurrentDir,
					&si,
					&pi);
		if(!bCreateProc)
		{
			DWORD dwErrCode = 0;
			TCHAR szErr[25];
			dwErrCode = GetLastError();
			wsprintf(szErr, TEXT("Error Code:%u"), dwErrCode);
			MessageBox(NULL, szErr, TEXT("MSI Package Failed"), MB_OK);
			ExitProcess(0);
		}
		else
		{

			CloseHandle(pi.hThread);
			WaitForSingleObject(pi.hProcess, INFINITE);
			CloseHandle(pi.hProcess);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		MessageBox(NULL, TEXT("An exception has occured during installation.  Please " \
				"contact your System Administrator"), 
				g_szAppName, MB_OK);
	}
	//PostQuitMessage(0);
	//
	CleanReg();
	// Let main thread know all installation is complete and return
	PostThreadMessage(g_dwMainThreadId, WM_QUIT, 0, 0);
	return dwRetVal;
}