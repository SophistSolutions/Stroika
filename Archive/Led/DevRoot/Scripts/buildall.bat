@echo off
rem
rem
rem $Header: /cygdrive/k/CVSRoot/Scripts/buildall.bat,v 1.19 2003/12/31 17:27:17 lewis Exp $
rem $Log: buildall.bat,v $
rem Revision 1.19  2003/12/31 17:27:17  lewis
rem SPR#1611: lose build of BCBLedTest and NetLedIt
rem
rem Revision 1.18  2003/06/24 14:12:17  lewis
rem SPR#1542: added SpelledIt\MungeDictionaries&ScavangeWordsFromNet to buildall.bat etc
rem
rem Revision 1.17  2003/06/10 17:09:31  lewis
rem SPR#1526: build ActiveSpelledIt in buildall.bat script
rem
rem Revision 1.16  2003/03/25 14:43:14  lewis
rem *** empty log message ***
rem
rem Revision 1.15  2002/09/04 04:00:24  lewis
rem react to switch to VC++.NET. .SLN files instead of dsw, lose MTSetup, etc
rem
rem Revision 1.14  2002/05/06 21:31:39  lewis
rem <========================================= Led 3.0.1 Released ========================================>
rem
rem Revision 1.13  2001/11/27 00:37:57  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.12  2001/11/27 00:28:36  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.11  2001/05/24 22:28:57  lewis
rem no longer have LedItMFC project - just LedIt
rem
rem Revision 1.10  2001/01/20 14:23:15  lewis
rem add build borland CPP Builder test
rem
rem Revision 1.9  2000/10/22 14:49:10  lewis
rem header/log and call endlocal at end
rem
rem
rem
setlocal
cd \
call scripts\doappletbuild_ LedIt LedIt.sln
call scripts\doappletbuild_ LedLineItMFC LedLineIt.sln
call scripts\doappletbuild_ LedTestWin32 LedTestWin32.sln
call scripts\doappletbuild_ ActiveLedIt ActiveLedIt.sln
call scripts\doappletbuild_ ActiveSpelledIt ActiveSpelledIt.sln
REM call scripts\doappletbuild_ NetLedIt NetLedIt.sln
call scripts\doappletbuild_ SpelledIt\MungeDictionaries MungeDictionaries.sln
call scripts\doappletbuild_ SpelledIt\ScavangeWordsFromNet ScavangeWordsFromNet.sln
REM call scripts\dobcbappletbuild_ BorlandCPPBuilderTest BCBLedTest.bpr
REM cd \AuxiliaryPackages
cd \DocsAndNotes
call \scripts\doappletbuild_ AutoDoc AutoDoc.sln
cd \DocsAndNotes\AutoDoc
call RunAutoDoc.bat
cd \
call Scripts\BuildAppDocs.bat
endlocal
