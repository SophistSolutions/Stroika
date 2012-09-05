@echo off
rem
rem
rem $Header: /cygdrive/k/CVSRoot/Scripts/MungeASSamplesForCABFILE.bat,v 1.1 2003/06/12 23:13:04 lewis Exp $
rem $Log: MungeASSamplesForCABFILE.bat,v $
rem Revision 1.1  2003/06/12 23:13:04  lewis
rem *** empty log message ***
rem
rem Revision 1.1  2003/03/26 22:12:56  lewis
rem SPR#1378 - Fix files so CODEBASE refers to magic flag replaced
rem
rem
rem
rem	MungeALSamplesForCABFILE <SAMPLESDIR> <CABFILESTR>
rem		e.g.
rem		MungeALSamplesForCABFILE Samples "..\\..\\Binaries\\ActiveLedIt_3.1a4.cab#version=49,2,5,1"
setlocal

cd %1

set path=%path%;\scripts

call MungeSingleALSampleForCABFILE HTML-JavaScript\SimpleTest.htm %2
call MungeSingleALSampleForCABFILE HTML-JavaScript\SimpleActiveLedItSpellCheck.htm %2

endlocal
