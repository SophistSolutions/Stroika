@echo off

set _LINK=%1
set _TARGET=%2
set _DIR=%3

::echo %_LINK%
::echo %_TARGET%


echo "***DEPRECATED***  in Stroika 2.1d14"
:: BatchGotAdmin
:-------------------------------------
REM  --> Check for permissions
>nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"

REM --> If error flag set, we do not have admin.
if '%errorlevel%' NEQ '0' (
    ::echo Requesting administrative privileges...
    goto UACPrompt
) else ( goto gotAdmin )

:UACPrompt
    echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
    echo UAC.ShellExecute "%~s0", "%_LINK% %_TARGET% %CD%", "", "runas", 1 >> "%temp%\getadmin.vbs"

    cscript "%temp%\getadmin.vbs"
    exit /B

:gotAdmin
    if exist "%temp%\getadmin.vbs" ( del "%temp%\getadmin.vbs" )
    ::pushd "%CD%"
    ::CD /D "%~dp0"
 
::put your main script here
::set
::echo %_LINK%
::echo %_TARGET%
::echo %_DIR%
cd %_DIR%
::echo mklink /D %_LINK% %_TARGET%
::pause 
mklink /D %_LINK% %_TARGET%
