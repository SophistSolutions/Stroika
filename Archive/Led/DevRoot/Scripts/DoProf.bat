REM - example usage:
REM	-	cd to dir where EXE is, and then say: "DoProf LedIt" - note leave out the .EXE at the end of LedIt!
rem
rem
rem $Header: /cygdrive/k/CVSRoot/Scripts/DoProf.bat,v 1.6 2002/05/06 21:31:38 lewis Exp $
rem $Log: DoProf.bat,v $
rem Revision 1.6  2002/05/06 21:31:38  lewis
rem <========================================= Led 3.0.1 Released ========================================>
rem
rem Revision 1.5  2001/11/27 00:37:56  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.4  2001/11/27 00:28:35  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.3  2000/10/22 14:49:10  lewis
rem header/log and call endlocal at end
rem
rem
rem

prep /ft /om %1
REM prep /at /stack 2 /ft /om %1
profile %1
prep /m %1
REM can cahnge /stc to /st
plist /stc %2 /PW 500 %1
