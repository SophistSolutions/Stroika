@ECHO OFF
rem
rem
rem $Header: /cygdrive/k/CVSRoot/Scripts/BuildInstallerLedTestWin32.bat,v 1.6 2002/05/06 21:31:33 lewis Exp $
rem $Log: BuildInstallerLedTestWin32.bat,v $
rem Revision 1.6  2002/05/06 21:31:33  lewis
rem <========================================= Led 3.0.1 Released ========================================>
rem
rem Revision 1.5  2001/11/27 00:37:53  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.4  2001/11/27 00:28:32  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.3  2000/10/22 14:48:47  lewis
rem header/log and call endlocal at end
rem
rem
rem
rm -rf \LedTestWin32\Built
mkdir \LedTestWin32\Built
copy \LedTestWin32\LedTestWin32___Win32_Partial_UNICODE_Release\LedTestWin32.exe \LedTestWin32\Built
