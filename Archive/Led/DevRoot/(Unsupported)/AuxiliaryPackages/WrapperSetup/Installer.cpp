// Installer.cpp

#include "stdafx.h"
#include <windows.h>
#include "MTSetup.h"

extern DWORD g_dwOSVerFlags;

BOOL InstallMSI()
{
	if (IsWindowsInstallerPresent()) {
		return true;
	}


	BOOL bRetVal = FALSE;	
	BOOL bCreateProc = FALSE;
	DWORD dwOSVersion;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	

	dwOSVersion = GetVersion();
	
	
	if(dwOSVersion < 0x80000000)		// NT & W2K
	{
		//Check for Admin privledges here
		if(!IsAdmin())
		{
			MessageBox(
				NULL, 
				TEXT("You must be an administrator to install this software"), 
				g_szAppName, 
				MB_OK);
			return FALSE;
		}
		// Create Winnt\instmsi.exe process
		bCreateProc = CreateProcess(NULL,
					"InstMsiW.exe /q",
//					"WinNT\\InstMsi.exe /q",
					NULL,
					NULL,
					FALSE,
					0,  
					NULL,
					NULL,
					&si,
					&pi);
		// We want to wait for InstMsi to finish before we proceed.
		CloseHandle(pi.hThread);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		bRetVal = bCreateProc;

	}
	else					// Win9X
	{
		// Spawn Win9X\InstMsi.exe process
		bCreateProc = CreateProcess(NULL,
//					"Win9X\\InstMsi.exe /q",
					"InstMsiA.exe /q",
					NULL,
					NULL,
					FALSE,
					0,   
					NULL,
					NULL,
					&si,
					&pi);
		// We want to wait for InstMsi to finish before we proceed.
		CloseHandle(pi.hThread);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);

		bRetVal = bCreateProc;
	}

	return bRetVal;
}

BOOL IsWindowsInstallerPresent()
{
	TCHAR szSysDir[MAX_PATH];
	TCHAR szFile[MAX_PATH];
	HMODULE hMod = NULL;
	BOOL bModuleFound = FALSE;
	BOOL hRet = FALSE;

	if((g_dwOSVerFlags & dwWin2K) == dwWin2K)
		return TRUE;

	GetSystemDirectory(szSysDir, MAX_PATH);
	lstrcpy(szFile, szSysDir);
	lstrcat(szFile, "\\MSI.DLL");

	hMod = LoadLibrary(szFile);

	// If you require a particular version of MSI.DLL, then perform your check here.
	bModuleFound = hMod ? TRUE : FALSE;

	FreeLibrary(hMod);
	
	return bModuleFound;
	
}