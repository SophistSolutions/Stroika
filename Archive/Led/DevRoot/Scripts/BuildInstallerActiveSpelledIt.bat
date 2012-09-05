@ECHO OFF
rem
rem
rem $Header: /cygdrive/k/CVSRoot/Scripts/BuildInstallerActiveSpelledIt.bat,v 1.1 2003/06/12 23:13:04 lewis Exp $
rem $Log: BuildInstallerActiveSpelledIt.bat,v $
rem Revision 1.1  2003/06/12 23:13:04  lewis
rem *** empty log message ***
rem
rem
rem
rem	USAGE:
rem		BuildInstallerActiveSpelledIt  SHORTVER STARTVERNUM_1 STARTVERNUM_2 STARTVERNUM_3
rem	e.g:
rem		BuildInstallerActiveSpelledIt  a4 49 2 4
rem

setlocal

rmdir /S /Q \ActiveSpelledIt\Built
mkdir \ActiveSpelledIt\Built
mkdir \ActiveSpelledIt\Built\SDK
mkdir \ActiveSpelledIt\Built\DemoSDK
cd \ActiveSpelledIt
call BuildCAB %2 %3 %4 1 %2 %3 %4 0
cd \
xcopy /S /I \ActiveSpelledIt\Samples \ActiveSpelledIt\Built\SDK\Samples
xcopy /S /I \ActiveSpelledIt\Samples \ActiveSpelledIt\Built\DemoSDK\Samples
copy \ActiveSpelledIt\ReadMe.rtf \ActiveSpelledIt\Built\SDK\
copy "\ActiveSpelledIt\Demo ReadMe.rtf" \ActiveSpelledIt\Built\DemoSDK\

mkdir \ActiveSpelledIt\Built\SDK\Binaries
mkdir \ActiveSpelledIt\Built\SDK\Binaries\CABFiles
mkdir \ActiveSpelledIt\Built\DemoSDK\Binaries
mkdir \ActiveSpelledIt\Built\DemoSDK\Binaries\CABFiles
copy \ActiveSpelledIt\ActiveSpelledIt.cab \ActiveSpelledIt\Built\SDK\Binaries\CABFiles\ActiveSpelledIt_3.1%1.cab
copy \ActiveSpelledIt\ActiveSpelledItDemo.cab \ActiveSpelledIt\Built\SDK\Binaries\CABFiles\ActiveSpelledItDemo_3.1%1.cab
copy \ActiveSpelledIt\ActiveSpelledItDemo.cab \ActiveSpelledIt\Built\DemoSDK\Binaries\CABFiles\ActiveSpelledItDemo_3.1%1.cab

REM patch Sample files to have the right CABFILE reference
REM (dont bother doing - since doesn't seem to work too well - and not really needed since installer now
REM		registers the OCX)
call \Scripts\MungeASSamplesForCABFILE \ActiveSpelledIt\Built\SDK\Samples "..\\..\\Binaries\\CABFiles\\ActiveSpelledIt_3.1%1.cab#version=%2,%3,%4,1"
call \Scripts\MungeASSamplesForCABFILE \ActiveSpelledIt\Built\DemoSDK\Samples "..\\..\\Binaries\\CABFiles\\ActiveSpelledItDemo_3.1%1.cab#version=%2,%3,%4,0"

REM Copy licenses
mkdir \ActiveSpelledIt\Built\SDK\Licenses
mkdir \ActiveSpelledIt\Built\DemoSDK\Licenses
Copy \Licenses\ActiveSpelledIt31License.htm \ActiveSpelledIt\Built\SDK\Licenses\
Copy \Licenses\ActiveSpelledIt31License.htm \ActiveSpelledIt\Built\DemoSDK\Licenses\
REM Copy \Licenses\ActiveSpelledIt30SourceLicense.htm \ActiveSpelledIt\Built\SDK\Licenses\

REM Documenation
mkdir \ActiveSpelledIt\Built\SDK\Documentation
mkdir \ActiveSpelledIt\Built\DemoSDK\Documentation
copy \ActiveSpelledIt\Sources\ActiveSpelledIt.idl \ActiveSpelledIt\Built\SDK\Documentation\
copy \ActiveSpelledIt\Sources\ActiveSpelledIt.idl \ActiveSpelledIt\Built\DemoSDK\Documentation\
copy "\ActiveSpelledIt\ActiveSpelledIt Reference.rtf" \ActiveSpelledIt\Built\SDK\Documentation\
copy "\ActiveSpelledIt\ActiveSpelledIt Reference.rtf" \ActiveSpelledIt\Built\DemoSDK\Documentation\


REM Build installers
REM			We first copy to a place in \temp - because IS Express hardwires full paths,
REM		and we want to be able to build from different drives (e.g. L and M).
REM			Then run ISBuild to actually build the installers.

Set Path=%Path%;"C:\Program Files\InstallShield\Express\System"


REM Set Image=SingleImage
Set Image=WebDeployment


set TmpInstallDataDir="C:\temp\ActiveSpelledItInstallerTmp\"
mkdir "C:\temp\"
rmdir /S /Q %TmpInstallDataDir%
mkdir %TmpInstallDataDir%


DEL /F /S /Q %TmpInstallDataDir%
xcopy /E \ActiveSpelledIt\Built %TmpInstallDataDir%
copy \ActiveSpelledIt\Release\ActiveSpelledIt.ocx %TmpInstallDataDir%SDK\Binaries\
copy \ActiveSpelledIt\Demo-Release\ActiveSpelledIt.ocx %TmpInstallDataDir%DemoSDK\Binaries\
copy \ActiveSpelledIt\Sources\ActiveSpelledIt.ico %TmpInstallDataDir%

Rem Build the installer
IsExpCmdBld.exe -p "\ActiveSpelledIt\ActiveSpelledItSDK.ism" -r %Image%
IsExpCmdBld.exe -p "\ActiveSpelledIt\ActiveSpelledItDemoSDK.ism" -r %Image%

copy /y \ActiveSpelledIt\ActiveSpelledItSDK\Express\%Image%\DiskImages\DISK1\setup.exe \ActiveSpelledIt\Built\ActiveSpelledItSDKInstaller_3.1%1.exe
copy /y \ActiveSpelledIt\ActiveSpelledItDemoSDK\Express\%Image%\DiskImages\DISK1\setup.exe \ActiveSpelledIt\Built\ActiveSpelledItDemoSDKInstaller_3.1%1.exe

REM - SIGN the EXEs
REM Copy the (REAL) PVK and CERT from special place for building
copy \\PYTHAGORAS\Led\DigitalCertificate\SophistSolutionsInc-Authenticode.pvk SophistSolutions.pvk
copy \\PYTHAGORAS\Led\DigitalCertificate\SophistSolutionsInc-Authenticode.spc SophistSolutions.spc
Echo "Signing the EXE files- you must enter a password here(twice)"
signcode -v SophistSolutions.pvk -spc SophistSolutions.spc -t http://timestamp.verisign.com/scripts/timstamp.dll \ActiveSpelledIt\Built\ActiveSpelledItSDKInstaller_3.1%1.exe
signcode -v SophistSolutions.pvk -spc SophistSolutions.spc -t http://timestamp.verisign.com/scripts/timstamp.dll \ActiveSpelledIt\Built\ActiveSpelledItDemoSDKInstaller_3.1%1.exe

endlocal
