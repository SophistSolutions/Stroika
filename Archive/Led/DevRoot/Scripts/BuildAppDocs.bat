REM @ECHO OFF
setlocal
rem
rem
rem $Header: /cygdrive/k/CVSRoot/Scripts/BuildAppDocs.bat,v 1.5 2002/05/06 21:31:30 lewis Exp $
rem $Log: BuildAppDocs.bat,v $
rem Revision 1.5  2002/05/06 21:31:30  lewis
rem <========================================= Led 3.0.1 Released ========================================>
rem
rem Revision 1.4  2001/11/27 00:37:51  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.3  2001/11/27 00:28:29  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.2  2001/11/06 20:51:47  lewis
rem *** empty log message ***
rem

SET MAKE_MODE=unix
SET CYGROOT=g:\cygnus\b19
SET CYGFS=g:/cygnus/b19
SET CYGREL=B19
SET GCC_EXEC_PREFIX=%CYGROOT%\H-i386-cygwin32\lib\gcc-lib\
SET TCL_LIBRARY=%CYGROOT%\share\tcl8.0\
SET GDBTK_LIBRARY=%CYGFS%/share/gdbtcl
SET PATH=%CYGROOT%\H-i386-cygwin32\bin;%PATH%
echo Cygnus Cygwin32 %CYGREL%
cd \DocsAndNotes
bash BuildLedItDocs.sh
endlocal
