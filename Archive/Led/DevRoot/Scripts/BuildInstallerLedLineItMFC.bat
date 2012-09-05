@ECHO OFF
rem
rem
rem $Header: /cygdrive/k/CVSRoot/Scripts/BuildInstallerLedLineItMFC.bat,v 1.18 2003/05/09 22:31:27 lewis Exp $
rem $Log: BuildInstallerLedLineItMFC.bat,v $
rem Revision 1.18  2003/05/09 22:31:27  lewis
rem SPR#1471: use rmdir instead of DEL and copy icon files(ledit/ledlineit). SPR#1481: fix installer (ledit/ledlineit) to not refer to l: - copy icon from c:\temp\x...dir
rem
rem Revision 1.17  2002/11/07 03:37:58  lewis
rem SPR#1170 change dir where we copy data files from since no obvious way todo path vars (like in IS Developer 7) - and it hardwires full path.
rem
rem Revision 1.16  2002/11/02 02:14:05  lewis
rem build image WebDeployment instead of SingleImage - so its smaller for web downloads
rem
rem Revision 1.15  2002/09/04 04:01:20  lewis
rem fix build installer scripts to use installshieldexpress
rem
rem Revision 1.14  2002/05/06 21:31:32  lewis
rem <========================================= Led 3.0.1 Released ========================================>
rem
rem Revision 1.13  2001/11/27 00:37:53  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.12  2001/11/27 00:28:31  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.11  2001/07/31 17:21:19  lewis
rem renamed LedLineIt.sln to LedLineItInstaller.sln to avoid MSVC70 (.NET) compat problem
rem
rem Revision 1.10  2001/01/20 18:22:11  lewis
rem use timestamp server so we get a legit timestamp on signed code
rem
rem Revision 1.9  2000/10/23 21:52:07  lewis
rem dont hardwire l: in place to copy setup.exe from
rem
rem Revision 1.8  2000/10/22 14:48:47  lewis
rem header/log and call endlocal at end
rem
rem
rem

setlocal

REM OLD - VC6 and earlier
REM Set Path=%Path%;"C:\Program Files\CabDevKit\BIN\"
REM Set Path=%Path%;"C:\Program files\Inetsdk\bin\"

REM VC++.Net
Set Path=%Path%;"C:\Program Files\Microsoft Visual Studio .NET\Common7\Tools\Bin"
Set Path=%Path%;"C:\Program Files\InstallShield\Express\System"

REM Set Image=SingleImage
Set Image=WebDeployment


set TmpInstallDataDir="C:\temp\LedLineItWin32InstallerData\"
mkdir "C:\temp\"
rmdir /S /Q %TmpInstallDataDir%
mkdir %TmpInstallDataDir%


cd \LedLineItMFC\
copy \LedLineItMFC\LedLineIt___Win32_Partial_UNICODE_Release\LedLineIt.exe %TmpInstallDataDir%
copy \LedLineItMFC\Sources\LedLineIt.ico %TmpInstallDataDir%
xcopy /E \DocsAndNotes\_LedLineIt_Win_ %TmpInstallDataDir%LedLineItDocs\



REM Build actual installer
IsExpCmdBld.exe -p "LedLineItWin32Installer.ism" -r %Image%


REM - SIGN the EXE
REM Copy the (REAL) PVK and CERT from special place for building
copy \\PYTHAGORAS\Led\DigitalCertificate\SophistSolutionsInc-Authenticode.pvk SophistSolutions.pvk
copy \\PYTHAGORAS\Led\DigitalCertificate\SophistSolutionsInc-Authenticode.spc SophistSolutions.spc
Echo "Signing the EXE file- you must enter a password here"
signcode -v SophistSolutions.pvk -spc SophistSolutions.spc -t http://timestamp.verisign.com/scripts/timstamp.dll LedLineItWin32Installer\Express\%Image%\DiskImages\DISK1\setup.exe



REM rm -rf Built
REM mkdir Built
REM mkdir Built\LedLineIt
REM xcopy /y /E LedLineItWin32Installer\LedLineItDocs Built\LedLineIt\LedLineItDocs\
REM copy /y LedLineItWin32Installer\LedLineIt.exe Built\LedLineIt\
DEL /F /S /Q Built\Win32
mkdir Built
mkdir Built\Win32
mkdir Built\Win32\LedLineItMFC
xcopy /y /E %TmpInstallDataDir%* Built\Win32\LedLineItMFC\

copy /y LedLineItWin32Installer\Express\%Image%\DiskImages\DISK1\setup.exe Built\Install_LedLineIt_Win_3.1%1.exe
