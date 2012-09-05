@echo off
rem
rem
rem $Header: /cygdrive/k/CVSRoot/Scripts/MungeSingleALSampleForCABFILE.bat,v 1.1 2003/03/26 22:12:57 lewis Exp $
rem $Log: MungeSingleALSampleForCABFILE.bat,v $
rem Revision 1.1  2003/03/26 22:12:57  lewis
rem SPR#1378 - Fix files so CODEBASE refers to magic flag replaced
rem
rem
rem
rem	MungeALSamplesForCABFILE <FILENAME> <CABFILESTR>
rem		e.g.
rem		MungeSingleALSampleForCABFILE PreloadingSample.html "..\\..\\Binaries\\ActiveLedIt_3.1a4.cab#version=49,2,5,1"
setlocal

set TMPFILENAME="tmpfile.htm"
DEL /Q/F %TMPFILENAME%

sed s/USE_CODEBASE_VAR/%2/g < %1 > %TMPFILENAME%
DEL /Q/F %1
mv %TMPFILENAME% %1

endlocal
