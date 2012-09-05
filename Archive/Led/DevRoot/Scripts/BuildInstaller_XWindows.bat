@ECHO OFF
rem
rem
rem $Header: /cygdrive/k/CVSRoot/Scripts/BuildInstaller_XWindows.bat,v 1.7 2003/05/09 20:10:06 lewis Exp $
rem $Log: BuildInstaller_XWindows.bat,v $
rem Revision 1.7  2003/05/09 20:10:06  lewis
rem SPR#1480: FIX BuildInstaller_XWindows.bat to take ARG with HOSTNAME and BuildInstallerLedItUNIX.bat too THEN update the RELESE_X_INSTALL_INSTRUCTIONS.txt file
rem
rem Revision 1.6  2002/05/06 21:31:34  lewis
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
rm -rf \LedTestXWindows\Built
mkdir \LedTestXWindows\Built
rcp -b %1:ExportDir/LedTestXWindows/LedTestXWindows \LedTestXWindows\Built
