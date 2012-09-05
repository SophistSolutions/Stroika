@ECHO OFF
rem
rem
rem $Header: /cygdrive/k/CVSRoot/Scripts/MakeExportDir.bat,v 1.13 2003/06/03 22:35:36 lewis Exp $
rem $Log: MakeExportDir.bat,v $
rem Revision 1.13  2003/06/03 22:35:36  lewis
rem cleanup error messages on rmdir bug workaround
rem
rem Revision 1.12  2003/05/15 13:08:04  lewis
rem work on still flaky deletes with del/rmdir!
rem
rem Revision 1.11  2003/05/14 20:48:56  lewis
rem use DEL and RMDIR and REN instead of rm/mv
rem
rem Revision 1.10  2002/05/06 21:31:39  lewis
rem <========================================= Led 3.0.1 Released ========================================>
rem
rem Revision 1.9  2001/11/27 00:37:57  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.8  2001/11/27 00:28:36  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.7  2000/10/22 14:49:10  lewis
rem header/log and call endlocal at end
rem
rem
rem
setlocal
cd\
REM for reasons I don't understand - the RMDIR seems to fail occasionally and the DEL leaves extra empty dirs occasionally, but together they work fine? - LGP 2003-05-13
del /F /Q /S ExportDir.old > c:\tmp\devnul 2>&1
rmdir /Q /S ExportDir.old > c:\tmp\devnul 2>&1
IF EXIST ExportDir.old. (
	Echo .. ExportDir.old STILL EXISTS - BUGGY RMDIR/DEL - but try to work around!
	rm -rf ExportDir.old
	del /F /Q /S ExportDir.old
	rmdir /Q /S ExportDir.old
	IF EXIST ExportDir.old. (
		Echo .... ExportDir.old STILL EXISTS - workaround attemp failed!!! Export unreliable!
	) ELSE (
		Echo .... ExportDir.old delete workaround succeeded. Sigh...
	)
)
REM mv ExportDir ExportDir.old > c:\tmp\devnul 2>&1
IF EXIST ExportDir. (
	ren ExportDir ExportDir.old > c:\tmp\devnul 2>&1
)
mkdir \ExportDir
cd \ExportDir
Echo Doing Export
cvs -d:local:\\Pythagoras\Led\CVSRoot -q export  -D 1/1/2010 all > c:\tmp\devnul
endlocal
