// registry.c

#include "stdafx.h"
#include <windows.h>
#include "MTSetup.h"

extern DWORD	g_dwOSVerFlags;

BOOL RegReadCmdLine(LPSTR lpCmdLine, DWORD* pcbKeySize)
{
	HKEY hCmdLine;
	TCHAR szWrapperKey[MAX_PATH];
	TCHAR szRegKey[] = TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Wrapper");
	LPVOID lpType = 0;
	UINT cbKeySize;
	LONG lRet;

	wsprintf(szWrapperKey, TEXT("%s"), szRegKey);

	lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szWrapperKey, 0, KEY_ALL_ACCESS, &hCmdLine);

	if(ERROR_SUCCESS == lRet)
	{
		cbKeySize = sizeof(lpCmdLine);

		lRet = RegQueryValueEx(hCmdLine, "CmdLine", NULL, NULL, (BYTE*)lpCmdLine, pcbKeySize);

		if(ERROR_SUCCESS == lRet)
		{
			RegCloseKey(hCmdLine);
			return TRUE;
		}					
	}
	return FALSE;
}

BOOL SetRunOnceKey(DWORD dwReason)
{
	BOOL bRetVal = FALSE;
	HKEY hRunOnce;
	//LPCTSTR lpDCOMVal = "Wrapper.exe DCOMREBOOT";
	//LPCTSTR lpMDACVal = "Wrapper.exe MDACREBOOT";
	LPCTSTR lpDCOMVal = "DCOMREBOOT";
	LPCTSTR lpMDACVal = "MDACREBOOT";

	LPCTSTR lpRunOnceExFlagsValue = "Flags";
	DWORD dwRunOnceExFlags = 0x3;
	TCHAR lpRunOnceCmd[MAX_PATH];
	TCHAR szCurrentDir[MAX_PATH];
	LONG lRet;

	if((g_dwOSVerFlags & dwWin95) == dwWin95)
	{
		if((dwReason & dwDCOM) == dwDCOM)
		{
			lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
						TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce"),
						0,
						KEY_ALL_ACCESS,
						&hRunOnce);
			if(ERROR_SUCCESS != lRet)
			{
				// Error opening key
				MessageBox(NULL, "Error Opening Key", "SetRunOnceKey", MB_OK);
			}
			else
			{
				GetCurrentDirectory(sizeof(szCurrentDir), szCurrentDir);
				//wsprintf(lpRunOnceCmd, TEXT("%s\\%s"), szCurrentDir, lpDCOMVal); 
				wsprintf(lpRunOnceCmd, 
						TEXT("%s\\%s.exe %s"), 
						szCurrentDir, g_szAppName,
						lpDCOMVal); 


				lRet = RegSetValueEx(hRunOnce, 
						"Wrapper",
						0, 
						REG_SZ, 
						(CONST BYTE*) lpRunOnceCmd, 
						lstrlen(lpRunOnceCmd) + 1);
				if(ERROR_SUCCESS != lRet)
				{
					MessageBox(NULL, TEXT("Error setting RegValue"), TEXT("SetRunOnceKey"), MB_OK);
					return FALSE;
				}
				RegCloseKey(hRunOnce);
				return TRUE;
			}
		}
		if((dwReason & dwMDAC) == dwMDAC)
		{
			lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
						TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce"),
						0,
						KEY_ALL_ACCESS,
						&hRunOnce);
			if(ERROR_SUCCESS != lRet)
			{
				// Log error
				return FALSE;
			}
			else
			{
				GetCurrentDirectory(sizeof(szCurrentDir), szCurrentDir);
				
				//wsprintf(lpRunOnceCmd, TEXT("%s\\%s"), szCurrentDir, lpMDACVal); 
				
				wsprintf(lpRunOnceCmd, 
						TEXT("%s\\%s.exe %s"), 
						szCurrentDir, g_szAppName,
						lpDCOMVal); 

				lRet = RegSetValueEx(hRunOnce, 
						"Wrapper",
						0, 
						REG_SZ, 
						(CONST BYTE*) lpRunOnceCmd, 
						lstrlen(lpRunOnceCmd) + 1);
				if(ERROR_SUCCESS != lRet)
				{
					MessageBox(NULL, TEXT("Error setting RegValue"), TEXT("SetRunOnceKey"), MB_ICONERROR);
					return FALSE;
				}

				RegCloseKey(hRunOnce);
				return TRUE;
			}
		}
	}
	else
	{
		// Use RunOnceEx for 98/NT
		if((dwReason & dwMDAC) == dwMDAC)
		{
			if(!CreateRunOnceSetupKey())
				return FALSE;

			if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE,
						"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce\\Setup",
						0,
						KEY_ALL_ACCESS,
						&hRunOnce))
			{
				// Error opening key
				MessageBox(NULL, "Error Opening Key", "SetRunOnceKey", MB_ICONERROR);
			}
			else
			{
				GetCurrentDirectory(sizeof(szCurrentDir), szCurrentDir);
				
				//wsprintf(lpRunOnceCmd, "%s\\%s", szCurrentDir, lpMDACVal); 
				
				wsprintf(lpRunOnceCmd, 
						TEXT("%s\\%s.exe %s"), 
						szCurrentDir, g_szAppName,
						lpDCOMVal); 

				if(ERROR_SUCCESS != RegSetValueEx(hRunOnce, 
						"Wrapper",
						0, 
						REG_SZ, 
						(CONST BYTE*) lpRunOnceCmd, 
						lstrlen(lpRunOnceCmd) + 1))
				{
					MessageBox(NULL, "Error setting RegValue", "SetRunOnceKey", MB_ICONERROR);
					return FALSE;
				}

				
				if(ERROR_SUCCESS != RegSetValueEx(hRunOnce, 
						lpRunOnceExFlagsValue,
						0, 
						REG_DWORD, 
						(CONST BYTE*)&dwRunOnceExFlags, 
						sizeof(DWORD)))
				{
					MessageBox(NULL, "Error setting Flags RegValue", "SetRunOnceKey", MB_ICONERROR);
					return FALSE;
				}
				
				RegCloseKey(hRunOnce);
				return TRUE;
			}
		}
	}
	return bRetVal;
}
BOOL CreateRunOnceSetupKey()
{
	BOOL bRetVal = FALSE;
	HKEY hKLM;
	HKEY hRunOnceSetup;
	DWORD dwDisp;

	if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce",
				0,
				KEY_ALL_ACCESS,
				&hKLM))
				{
		// Failed
		return FALSE;
	}

	RegCreateKeyEx(hKLM, "Setup", 0,
				NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hRunOnceSetup, &dwDisp);
	
	if(REG_CREATED_NEW_KEY == dwDisp)
	{
		RegCloseKey(hRunOnceSetup);
		return TRUE;
	}
	else if(REG_OPENED_EXISTING_KEY == dwDisp)
	{
		RegCloseKey(hRunOnceSetup);
		return TRUE;
	}
	else
	{
		MessageBox(NULL, "The registry key RunOnceSetup was not created", "Error", MB_ICONERROR);
		return FALSE;
	}
}

BOOL CleanReg()
{
	BOOL bRetVal = FALSE;
	TCHAR szRegKey[] = TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Wrapper");
	LONG lRet;

	lRet = RegDeleteKey(HKEY_LOCAL_MACHINE, szRegKey);
	if(ERROR_SUCCESS == lRet)
		return TRUE;
	return bRetVal;
}