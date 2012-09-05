// version.cpp

#include "stdafx.h"
#include <windows.h>
#include "MTSetup.h"

DWORD GetOperatingSystemVer()
{
	DWORD dwRetVal = 0;
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize  = sizeof(OSVERSIONINFO);

	if(GetVersionEx(&osvi))
	{
		if(VER_PLATFORM_WIN32_NT == osvi.dwPlatformId)
		{
			// Windows NT / Windows 2000
			switch(osvi.dwMajorVersion)
			{
			case 5:
				// Windows 2000
				dwRetVal |= dwWin2K;
				break;
			case 4:
				// Windows NT
				dwRetVal |= dwWinNT;
				break;
			}
		}
		else if(VER_PLATFORM_WIN32_WINDOWS == osvi.dwPlatformId)
		{
			// Windows 9x
			switch(osvi.dwMinorVersion)
			{
			case 0:
				// Win95
				dwRetVal |= dwWin95;
				break;
			default:
				dwRetVal |= dwWin98;
				break;
			}
		}
	}
	else
	{
		//MessageBox(NULL, TEXT("OS Version Could not be determined.\n"), g_szAppName, MB_ICONERROR);
		return 0;
	}
	return dwRetVal;
}