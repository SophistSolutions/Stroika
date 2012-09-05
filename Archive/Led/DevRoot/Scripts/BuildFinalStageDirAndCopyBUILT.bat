@ECHO OFF
rem
rem
rem $Header: /cygdrive/k/CVSRoot/Scripts/BuildFinalStageDirAndCopyBUILT.bat,v 1.16 2004/01/04 21:58:23 lewis Exp $
rem $Log: BuildFinalStageDirAndCopyBUILT.bat,v $
rem Revision 1.16  2004/01/04 21:58:23  lewis
rem Add copy of bins for ActiveSpelledIt
rem
rem Revision 1.15  2002/05/06 21:31:30  lewis
rem <========================================= Led 3.0.1 Released ========================================>
rem
rem Revision 1.14  2001/11/27 00:37:51  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.13  2001/11/27 00:28:29  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.12  2001/05/27 14:27:12  lewis
rem lose LedItPP/MFC - and replace with plain copy of LedIt/
rem
rem Revision 1.11  2001/01/20 18:21:46  lewis
rem BorlandCPPBuilderTest
rem
rem Revision 1.10  2001/01/20 16:44:14  lewis
rem BorlandCPPBuilderTest support
rem
rem Revision 1.9  2000/10/23 20:12:11  lewis
rem lose netledits not needed etc from diff projects
rem
rem Revision 1.8  2000/10/23 15:30:26  lewis
rem copy NetLedIt! stuff too
rem
rem Revision 1.7  2000/10/22 14:48:06  lewis
rem *** empty log message ***
rem
rem
rem
setlocal
rm -rf \FinalStageDir
mkdir \FinalStageDir
cd \FinalStageDir
Echo Exporting all sources from CVS to FinalStageDir
cvs -q -d:local:\\Pythagoras\Led\CVSRoot export  -D 1/1/2010 all > c:\tmp\devnul
Echo Copying Built files
xcopy /E \ActiveLedIt\Built ActiveLedIt\Built\
xcopy /E \ActiveSpelledIt\Built ActiveSpelledIt\Built\
xcopy /E \LedIt\Built LedIt\Built\
xcopy /E \LedLineItMFC\Built LedLineItMFC\Built\
xcopy /E \LedLineItPP\Built LedLineItPP\Built\
xcopy /E \LedTestWin32\Built LedTestWin32\Built\
xcopy /E \BorlandCPPBuilderTest\Built BorlandCPPBuilderTest\Built\
xcopy /E \LedTestXWindows\Built LedTestXWindows\Built\
xcopy /E \NetLedIt\Built NetLedIt\Built\
xcopy /E \DocsAndNotes\AutoDoc\DocsOut DocsAndNotes\ClassLibDocs\ReferenceManual\
endlocal