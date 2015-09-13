// dcom.c
#include "stdafx.h"
#include <windows.h>
#include "MTSetup.h"

extern DWORD g_dwOSVerFlags;
extern CRITICAL_SECTION	g_cs;
extern DWORD	g_dwInitRebootReason;

BOOL InstallDCOM()
{
	DWORD dwRetVal = 0;
	//BOOL bRetVal = FALSE;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	DWORD dwExitCode;
	TCHAR szCurrentDir[MAX_PATH];
	TCHAR szFileName[MAX_PATH];
	//TCHAR szErr[MAX_PATH];
	BOOL bProcessExitCode;
	BOOL bCreateProc = FALSE;
	DWORD dwErrCode = 0;
	//MessageBox(NULL, TEXT("Enter InstallDCOM()"), g_szAppName, MB_OK);
	if((g_dwOSVerFlags & dwWin95) == dwWin95)
		if(!SetRunOnceKey(dwDCOM))
			MessageBox(NULL, "DCOM RunOnce Key not set for Win95", "Error", MB_ICONERROR);
	GetCurrentDirectory(sizeof(szCurrentDir), szCurrentDir);
	wsprintf(szFileName, "%s\\%s /q", szCurrentDir, TEXT("DCOM95.EXE"));
	//MessageBox(NULL, szFileName, "Starting DCOM Installation", MB_OK);
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	bCreateProc = CreateProcess(NULL,
		szFileName,//"DCOM95.exe ", // run in quite mode
		NULL,
		NULL,
		FALSE,
		0,   
		NULL,
		NULL,
		&si,
		&pi);

	if(bCreateProc)
	{
		// DCOM Installation has started
		CloseHandle(pi.hThread);
		EnterCriticalSection(&g_cs);
		g_dwInitRebootReason = dwDCOM;
		LeaveCriticalSection(&g_cs);
		WaitForSingleObject(pi.hProcess, INFINITE);
		bProcessExitCode = GetExitCodeProcess(pi.hProcess, &dwExitCode);
		CloseHandle(pi.hProcess);	
		return TRUE;
	}
	else
	{
		TCHAR szErr[25];
		dwErrCode = GetLastError();
		wsprintf(szErr, TEXT("DCOM Installation failed!  Error Code:%u"), dwErrCode);
		MessageBox(NULL, szErr, g_szAppName, MB_OK);
		return FALSE;
	}
}

BOOL IsDCOMPresent()
{
	BOOL bRetVal = FALSE;
	TCHAR szSysDir[MAX_PATH];
	TCHAR szFile[MAX_PATH];
	HMODULE hMod = NULL;
	BOOL hRet = FALSE;
	DWORD dwInvalidOSVersions = dwWin98 | dwWin98SE | dwWinME | dwWinNT | dwWin2K;

	if((g_dwOSVerFlags & dwInvalidOSVersions))
		return TRUE;

	GetSystemDirectory(szSysDir, MAX_PATH);
	lstrcpy(szFile, szSysDir);
	lstrcat(szFile, "\\OLE32.DLL");

	hMod = LoadLibrary(szFile);
	if(NULL == hMod)
	{
		// See Winerror.h for explanation of error code
		DWORD dwErrorCode = GetLastError();
		TCHAR ErrMsg[255];
		wsprintf(ErrMsg, TEXT("LoadLibrary() Failed with: %i\n"), dwErrorCode);
		return FALSE;
	}
	if(!GetProcAddress(hMod, "CoCreateInstanceEx"))
	{
		// DCOM is not installed
		return FALSE;
	}
	else
	{
		// DCOM is installed
		FreeLibrary(hMod);
		return TRUE;
	}
}
