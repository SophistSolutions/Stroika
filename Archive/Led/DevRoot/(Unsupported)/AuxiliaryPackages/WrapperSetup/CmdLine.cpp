// CmdLine.cpp

#include "stdafx.h"
#include <windows.h>
#include "MTSetup.h"


LPTSTR SetStartupPath(LPTSTR lpPath)
{
	LPTSTR p;

	p = strrchr(lpPath, '\\');

	if(p)
	{
		*p = '\0';
		return _strdup(lpPath);
	}
	else
		return NULL;
}

BOOL RegWriteCmdLine(LPSTR lpCmdLine)
{
	BOOL bRetVal = FALSE;
	HKEY hKLM;
	HKEY hWrapper;
	DWORD dwDisp;
	LPCTSTR lpInstaller = "CmdLine";

	if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				"SOFTWARE\\Microsoft\\Windows\\CurrentVersion",
				0,
				KEY_ALL_ACCESS,
				&hKLM))
				{
		// Failed
		return FALSE;
	}

	RegCreateKeyEx(hKLM, "Wrapper", 0,
				NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hWrapper, &dwDisp);
	
	if(dwDisp == REG_CREATED_NEW_KEY)
	{
		if(ERROR_SUCCESS == RegSetValueEx(hWrapper, lpInstaller,
				0, REG_SZ, (CONST BYTE*) lpCmdLine, lstrlen(lpCmdLine) + 1))
		{
			return TRUE;
		}
	}
	RegCloseKey(hWrapper);
	return bRetVal;
}