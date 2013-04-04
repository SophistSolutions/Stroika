@ECHO OFF

rem
rem Copyright(c) Records For Living, Inc. 2004-2012.  All rights reserved
rem

@echo off
setlocal


Set DIRNAME=CURRENT

Echo Checking ThirdPartyLibs\openssl

Set Path=%Path%;..\..\bin

Set Failures=

Call CheckForExistsOrSetFailure_ %DIRNAME%\out32\libeay32.lib
if "%Failures%" NEQ "" goto Failed
Call CheckForExistsOrSetFailure_ %DIRNAME%\out32\ssleay32.lib
if "%Failures%" NEQ "" goto Failed

Call CheckForExistsOrSetFailure_ %DIRNAME%\out32.dbg\libeay32.lib
if "%Failures%" NEQ "" goto Failed
Call CheckForExistsOrSetFailure_ %DIRNAME%\out32.dbg\ssleay32.lib
if "%Failures%" NEQ "" goto Failed

Echo [SUCCEEDED]
goto Exit

:Failed
Echo [FAILED] - %Failures%

:Exit

endlocal
