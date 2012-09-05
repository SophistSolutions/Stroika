@ECHO OFF
rem
rem
rem $Header: /cygdrive/k/CVSRoot/Scripts/BuildInstallerActiveLedIt.bat,v 1.22 2003/05/09 22:31:26 lewis Exp $
rem $Log: BuildInstallerActiveLedIt.bat,v $
rem Revision 1.22  2003/05/09 22:31:26  lewis
rem SPR#1471: use rmdir instead of DEL and copy icon files(ledit/ledlineit). SPR#1481: fix installer (ledit/ledlineit) to not refer to l: - copy icon from c:\temp\x...dir
rem
rem Revision 1.21  2003/03/27 20:47:34  lewis
rem *** empty log message ***
rem
rem Revision 1.20  2003/03/26 22:14:22  lewis
rem SPR#1378 - call Scripts\MungeALSamplesForCABFILE to get Sample files munged for CODEBASE declaration. SPR#0795 - Add new installers for ActiveLedIt SDK and ActiveLedItDEMO SDK
rem
rem Revision 1.19  2003/01/24 21:51:31  lewis
rem add support for building an ActiveLedIt DEMO SDK
rem
rem Revision 1.18  2002/09/04 04:01:18  lewis
rem fix build installer scripts to use installshieldexpress
rem
rem Revision 1.17  2002/05/06 21:31:30  lewis
rem <========================================= Led 3.0.1 Released ========================================>
rem
rem Revision 1.16  2001/11/27 00:37:51  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.15  2001/11/27 00:28:30  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.14  2001/08/31 02:24:31  lewis
rem fix spelling error - licence -> license
rem
rem Revision 1.13  2000/11/08 23:27:52  lewis
rem *** empty log message ***
rem
rem Revision 1.12  2000/11/08 23:15:11  lewis
rem *** empty log message ***
rem
rem Revision 1.11  2000/11/07 15:25:42  lewis
rem reorg layout of activeledit built dir - having docs section, readme, etc
rem
rem Revision 1.10  2000/10/22 14:48:06  lewis
rem *** empty log message ***
rem
rem
rem	USAGE:
rem		BuildInstallerActiveLedIt  SHORTVER STARTVERNUM_1 STARTVERNUM_2 STARTVERNUM_3
rem	e.g:
rem		BuildInstallerActiveLedIt  a4 49 2 4
rem

setlocal

rmdir /S /Q \ActiveLedIt\Built
mkdir \ActiveLedIt\Built
mkdir \ActiveLedIt\Built\SDK
mkdir \ActiveLedIt\Built\DemoSDK
cd \ActiveLedIt
call BuildCAB %2 %3 %4 1 %2 %3 %4 0
cd \
xcopy /S /I \ActiveLedIt\Samples \ActiveLedIt\Built\SDK\Samples
xcopy /S /I \ActiveLedIt\Samples \ActiveLedIt\Built\DemoSDK\Samples
copy \ActiveLedIt\ReadMe.rtf \ActiveLedIt\Built\SDK\
copy "\ActiveLedIt\Demo ReadMe.rtf" \ActiveLedIt\Built\DemoSDK\

mkdir \ActiveLedIt\Built\SDK\Binaries
mkdir \ActiveLedIt\Built\SDK\Binaries\CABFiles
mkdir \ActiveLedIt\Built\DemoSDK\Binaries
mkdir \ActiveLedIt\Built\DemoSDK\Binaries\CABFiles
copy \ActiveLedIt\ActiveLedIt.cab \ActiveLedIt\Built\SDK\Binaries\CABFiles\ActiveLedIt_3.1%1.cab
copy \ActiveLedIt\ActiveLedItDemo.cab \ActiveLedIt\Built\SDK\Binaries\CABFiles\ActiveLedItDemo_3.1%1.cab
copy \ActiveLedIt\ActiveLedItDemo.cab \ActiveLedIt\Built\DemoSDK\Binaries\CABFiles\ActiveLedItDemo_3.1%1.cab

REM patch Sample files to have the right CABFILE reference
REM (dont bother doing - since doesn't seem to work too well - and not really needed since installer now
REM		registers the OCX)
call \Scripts\MungeALSamplesForCABFILE \ActiveLedIt\Built\SDK\Samples "..\\..\\Binaries\\CABFiles\\ActiveLedIt_3.1%1.cab#version=%2,%3,%4,1"
call \Scripts\MungeALSamplesForCABFILE \ActiveLedIt\Built\DemoSDK\Samples "..\\..\\Binaries\\CABFiles\\ActiveLedItDemo_3.1%1.cab#version=%2,%3,%4,0"

REM Copy licenses
mkdir \ActiveLedIt\Built\SDK\Licenses
mkdir \ActiveLedIt\Built\DemoSDK\Licenses
Copy \Licenses\ActiveLedIt30License.htm \ActiveLedIt\Built\SDK\Licenses\ActiveLedIt30License.htm
Copy \Licenses\ActiveLedIt30License.htm \ActiveLedIt\Built\DemoSDK\Licenses\ActiveLedIt30License.htm
Copy \Licenses\ActiveLedIt30SourceLicense.htm \ActiveLedIt\Built\SDK\Licenses\ActiveLedIt30SourceLicense.htm

REM Documenation
mkdir \ActiveLedIt\Built\SDK\Documentation
mkdir \ActiveLedIt\Built\DemoSDK\Documentation
copy \ActiveLedIt\Sources\ActiveLedIt.odl \ActiveLedIt\Built\SDK\Documentation\
copy \ActiveLedIt\Sources\ActiveLedIt.odl \ActiveLedIt\Built\DemoSDK\Documentation\
copy \ActiveLedIt\ActiveLedItReference.rtf \ActiveLedit\Built\SDK\Documentation\
copy \ActiveLedIt\ActiveLedItReference.rtf \ActiveLedit\Built\DemoSDK\Documentation\


REM Build installers
REM			We first copy to a place in \temp - because IS Express hardwires full paths,
REM		and we want to be able to build from different drives (e.g. L and M).
REM			Then run ISBuild to actually build the installers.

Set Path=%Path%;"C:\Program Files\InstallShield\Express\System"


REM Set Image=SingleImage
Set Image=WebDeployment


set TmpInstallDataDir="C:\temp\ActiveLedItInstallerTmp\"
mkdir "C:\temp\"
rmdir /S /Q %TmpInstallDataDir%
mkdir %TmpInstallDataDir%


DEL /F /S /Q %TmpInstallDataDir%
xcopy /E \ActiveLedIt\Built %TmpInstallDataDir%
copy \ActiveLedIt\ActiveLedIt___Win32_Partial_UNICODE_Release\ActiveLedIt.ocx %TmpInstallDataDir%SDK\Binaries\
copy \ActiveLedIt\ActiveLedIt___Win32_Partial_UNICODE_DEMO_Release\ActiveLedIt.ocx %TmpInstallDataDir%DemoSDK\Binaries\
copy \ActiveLedIt\Sources\ActiveLedIt.ico %TmpInstallDataDir%

Rem Build the installer
IsExpCmdBld.exe -p "\ActiveLedIt\ActiveLedItSDK.ism" -r %Image%
IsExpCmdBld.exe -p "\ActiveLedIt\ActiveLedItDemoSDK.ism" -r %Image%

copy /y \ActiveLedIt\ActiveLedItSDK\Express\%Image%\DiskImages\DISK1\setup.exe \ActiveLedIt\Built\ActiveLedItSDKInstaller_3.1%1.exe
copy /y \ActiveLedIt\ActiveLedItDemoSDK\Express\%Image%\DiskImages\DISK1\setup.exe \ActiveLedIt\Built\ActiveLedItDemoSDKInstaller_3.1%1.exe

REM - SIGN the EXEs
REM Copy the (REAL) PVK and CERT from special place for building
copy \\PYTHAGORAS\Led\DigitalCertificate\SophistSolutionsInc-Authenticode.pvk SophistSolutions.pvk
copy \\PYTHAGORAS\Led\DigitalCertificate\SophistSolutionsInc-Authenticode.spc SophistSolutions.spc
Echo "Signing the EXE files- you must enter a password here(twice)"
signcode -v SophistSolutions.pvk -spc SophistSolutions.spc -t http://timestamp.verisign.com/scripts/timstamp.dll \ActiveLedIt\Built\ActiveLedItSDKInstaller_3.1%1.exe
signcode -v SophistSolutions.pvk -spc SophistSolutions.spc -t http://timestamp.verisign.com/scripts/timstamp.dll \ActiveLedIt\Built\ActiveLedItDemoSDKInstaller_3.1%1.exe

endlocal
