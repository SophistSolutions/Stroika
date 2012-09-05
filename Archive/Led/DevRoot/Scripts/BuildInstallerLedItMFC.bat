@ECHO OFF
rem
rem
rem $Header: /cygdrive/k/CVSRoot/Scripts/BuildInstallerLedItMFC.bat,v 1.18 2003/05/09 22:31:26 lewis Exp $
rem $Log: BuildInstallerLedItMFC.bat,v $
rem Revision 1.18  2003/05/09 22:31:26  lewis
rem SPR#1471: use rmdir instead of DEL and copy icon files(ledit/ledlineit). SPR#1481: fix installer (ledit/ledlineit) to not refer to l: - copy icon from c:\temp\x...dir
rem
rem Revision 1.17  2002/11/07 03:37:57  lewis
rem SPR#1170 change dir where we copy data files from since no obvious way todo path vars (like in IS Developer 7) - and it hardwires full path.
rem
rem Revision 1.16  2002/11/02 02:14:03  lewis
rem build image WebDeployment instead of SingleImage - so its smaller for web downloads
rem
rem Revision 1.15  2002/09/04 04:01:19  lewis
rem fix build installer scripts to use installshieldexpress
rem
rem Revision 1.14  2002/05/06 21:31:31  lewis
rem <========================================= Led 3.0.1 Released ========================================>
rem
rem Revision 1.13  2001/11/27 00:37:52  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.12  2001/11/27 00:28:30  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.11  2001/05/25 12:37:01  lewis
rem fix for new DIRNAME for MFC build of LedIt - now in 'LedIt' directory
rem
rem Revision 1.10  2001/01/20 18:22:11  lewis
rem use timestamp server so we get a legit timestamp on signed code
rem
rem Revision 1.9  2000/10/23 21:52:07  lewis
rem dont hardwire l: in place to copy setup.exe from
rem
rem Revision 1.8  2000/10/22 14:48:06  lewis
rem *** empty log message ***
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


set TmpInstallDataDir="C:\temp\LedItWin32InstallerData\"
mkdir "C:\temp\"
rmdir /S /Q %TmpInstallDataDir%
mkdir %TmpInstallDataDir%



cd \LedIt\
copy \LedIt\LedIt___Win32_Partial_UNICODE_Release\LedIt.exe %TmpInstallDataDir%
copy \LedIt\Sources\LedIt.ico %TmpInstallDataDir%
xcopy /E \DocsAndNotes\_LedIt_Win_ %TmpInstallDataDir%LedItDocs\


Rem Build the installer
IsExpCmdBld.exe -p "LedItWin32Installer.ism" -r %Image%


REM - SIGN the EXE
REM Copy the (REAL) PVK and CERT from special place for building
copy \\PYTHAGORAS\Led\DigitalCertificate\SophistSolutionsInc-Authenticode.pvk SophistSolutions.pvk
copy \\PYTHAGORAS\Led\DigitalCertificate\SophistSolutionsInc-Authenticode.spc SophistSolutions.spc
Echo "Signing the EXE file- you must enter a password here"
signcode -v SophistSolutions.pvk -spc SophistSolutions.spc -t http://timestamp.verisign.com/scripts/timstamp.dll LedItWin32Installer\Express\%Image%\DiskImages\DISK1\setup.exe


DEL /F /S /Q Built\Win32
mkdir Built
mkdir Built\Win32
mkdir Built\Win32\LedIt
xcopy /y /E %TmpInstallDataDir%* Built\Win32\LedIt\

copy /y LedItWin32Installer\Express\%Image%\DiskImages\DISK1\setup.exe Built\Install_LedIt_Win_3.1%1.exe

endlocal
