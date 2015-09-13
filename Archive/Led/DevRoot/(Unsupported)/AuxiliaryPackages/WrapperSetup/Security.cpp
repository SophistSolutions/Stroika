// security.cpp

#include "stdafx.h"
#include <windows.h>
#include "MTSetup.h"


// From Q111542
BOOL IsAdmin()
{
   HANDLE hProcess, hAccessToken;
   TCHAR InfoBuffer[1024];
   PTOKEN_GROUPS ptgGroups = (PTOKEN_GROUPS)InfoBuffer;
   DWORD dwInfoBufferSize;
   SID_NAME_USE snuInfo;
   TCHAR szAccountName[MAX_PATH], szDomainName[MAX_PATH];
   DWORD dwAccountNameSize, dwDomainNameSize;
   UINT x;

   hProcess = GetCurrentProcess();

   if(!OpenProcessToken(hProcess,TOKEN_READ,&hAccessToken))
      return FALSE;

   if(!GetTokenInformation(
					hAccessToken,
					TokenGroups,
					InfoBuffer,
					1024, 
					&dwInfoBufferSize)) 
	 return(FALSE);

   for(x=0;x<ptgGroups->GroupCount;x++)
   {
      dwAccountNameSize = 256;
      dwDomainNameSize = 256;

      LookupAccountSid(
					NULL, 
					ptgGroups->Groups[x].Sid,
					szAccountName, 
					&dwAccountNameSize,
					szDomainName, 
					&dwDomainNameSize, 
					&snuInfo);

      if(!lstrcmp(szAccountName, TEXT("Administrators")) &&
			!lstrcmp(szDomainName,"BUILTIN"))
      return TRUE ;
  }
  return FALSE;
}