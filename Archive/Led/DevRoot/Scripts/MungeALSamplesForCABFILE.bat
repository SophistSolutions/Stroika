@echo off
rem
rem
rem $Header: /cygdrive/k/CVSRoot/Scripts/MungeALSamplesForCABFILE.bat,v 1.1 2003/03/26 22:12:56 lewis Exp $
rem $Log: MungeALSamplesForCABFILE.bat,v $
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

call MungeSingleALSampleForCABFILE ASP-JScript\SimpleActiveLedItASP2FromServerDemo.asp %2

call MungeSingleALSampleForCABFILE ASP-VBScript\SimpleActiveLedItASP2FromServerDemo.asp %2

call MungeSingleALSampleForCABFILE HTML\PreloadingSample.html %2
call MungeSingleALSampleForCABFILE HTML\SimpleParamTagExample.html %2
call MungeSingleALSampleForCABFILE HTML\SimplestSample.html %2
call MungeSingleALSampleForCABFILE HTML\TwoControls.html %2

call MungeSingleALSampleForCABFILE HTML-JavaScript\SimpleAutomation.html %2
call MungeSingleALSampleForCABFILE HTML-JavaScript\WebPageWithToolbarDemo\WebPageWithToolbarDemo.htm ..\\\\%2

call MungeSingleALSampleForCABFILE HTML-VBScript\SimpleAutomation.html %2
call MungeSingleALSampleForCABFILE HTML-VBScript\SimpleAutomationAndFileIO.html %2
call MungeSingleALSampleForCABFILE HTML-VBScript\SimpleAutomationAndRTFAccess.html %2

endlocal
