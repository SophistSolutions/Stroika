@ECHO OFF
rem
rem
rem $Header: /cygdrive/k/CVSRoot/Scripts/BuildInstallerLedLineItPP_Start.bat,v 1.6 2002/05/06 21:31:33 lewis Exp $
rem $Log: BuildInstallerLedLineItPP_Start.bat,v $
rem Revision 1.6  2002/05/06 21:31:33  lewis
rem <========================================= Led 3.0.1 Released ========================================>
rem
rem Revision 1.5  2001/11/27 00:37:53  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.4  2001/11/27 00:28:31  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.3  2000/10/22 14:48:47  lewis
rem header/log and call endlocal at end
rem
rem
rem
rm -rf \LedLineItPP\Built
mkdir \LedLineItPP\Built
mkdir \LedLineItPP\Built\LedLineIt
xcopy /E \DocsAndNotes\_LedLineIt_Mac_ \LedLineItPP\Built\LedLineIt\LedLineItDocs\
