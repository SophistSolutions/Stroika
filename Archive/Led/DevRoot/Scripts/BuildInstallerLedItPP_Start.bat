@ECHO OFF
rem
rem
rem $Header: /cygdrive/k/CVSRoot/Scripts/BuildInstallerLedItPP_Start.bat,v 1.7 2002/05/06 21:31:31 lewis Exp $
rem $Log: BuildInstallerLedItPP_Start.bat,v $
rem Revision 1.7  2002/05/06 21:31:31  lewis
rem <========================================= Led 3.0.1 Released ========================================>
rem
rem Revision 1.6  2001/11/27 00:37:52  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.5  2001/11/27 00:28:30  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.4  2001/05/25 13:25:45  lewis
rem SPR#0919- merge LedItPP code (etc) into just one LedIt app
rem
rem Revision 1.3  2000/10/22 14:48:06  lewis
rem *** empty log message ***
rem
rem
rem
rm -rf \LedIt\Built\MacOS
mkdir \LedIt\Built
mkdir \LedIt\Built\MacOS
mkdir \LedIt\Built\MacOS\LedIt
xcopy /E \DocsAndNotes\_LedIt_Mac_ \LedIt\Built\MacOS\LedIt\LedItDocs\
