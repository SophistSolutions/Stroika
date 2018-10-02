@Echo OFF
@SetLocal
rem
rem
rem $Header: /cygdrive/k/CVSRoot/ActiveLedIt/BuildCAB.bat,v 2.15 2003/06/10 21:25:43 lewis Exp $
rem $Log: BuildCAB.bat,v $
rem Revision 2.15  2003/06/10 21:25:43  lewis
rem *** empty log message ***
rem
rem Revision 2.14  2003/05/08 23:42:18  lewis
rem add path for VC.NET 2003
rem
rem Revision 2.13  2003/03/26 22:15:48  lewis
rem SPR#1378 - adjust args to BuildCAB so we pass along #version string for Sample .html files
rem
rem Revision 2.12  2002/09/04 02:01:19  lewis
rem use new PATH variable addition instead of explicit path for signcode etc calls. Don't assume old locations for that code - instead use VC.NET dir.
rem
rem Revision 2.11  2002/05/06 21:34:17  lewis
rem <=============================== Led 3.0.1 Released ==============================>
rem
rem Revision 2.10  2001/11/27 00:32:27  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 2.9  2001/01/20 18:22:27  lewis
rem use timestamp server so we get a legit timestamp on signed code
rem
rem Revision 2.8  2000/10/22 15:07:39  lewis
rem *** empty log message ***
rem
rem
rem Usage:
rem		BuildCAB V1 V2 V3 V4 W1 W2 W3 W4
rem


REM OLD - VC6 and earlier
REM Set Path=%Path%;"C:\Program Files\CabDevKit\BIN\"
REM Set Path=%Path%;"C:\Program files\Inetsdk\bin\"

REM VC++.Net
Set Path=%Path%;"C:\Program Files\Microsoft Visual Studio .NET\Common7\Tools\Bin"
Set Path=%Path%;"C:\Program Files\Microsoft Visual Studio .NET 2003\Common7\Tools\Bin"

REM - this check no longer needed cuz of the ActiveLedItTemplate.inf code - I think - LGP 2003-06-10
REM Echo "*****Warning - when releasing a copy of the CAB file to the outside world - be sure to update the version# in the .INF file"


Echo "Building INTERNAL_UNICODE ActiveLedIt! OCX Cab file"
rm -f ActiveLedIt.inf
sed s/FILE_VERSION_STRING/%1,%2,%3,%4/g < ActiveLedItTemplate.inf > ActiveLedIt.inf
REM Make the CAB file (-s leaves room to be signed)
cabarc -s 6144 N ActiveLedIt.cab ActiveLedIt___Win32_Partial_UNICODE_Release\ActiveLedIt.ocx ActiveLedIt.inf

Echo "Building INTERNAL_UNICODE ActiveLedIt! DEMO OCX Cab file"
rm -f ActiveLedIt.inf
sed s/FILE_VERSION_STRING/%5,%6,%7,%8/g < ActiveLedItTemplate.inf > ActiveLedIt.inf
cabarc -s 6144 N ActiveLedItDemo.cab ActiveLedIt___Win32_Partial_UNICODE_DEMO_Release\ActiveLedIt.ocx ActiveLedIt.inf


REM Make a BOGUS certificate. This will be REPLACED with copying the PVK/CER file from some standardized place when I get it from CA
REM makecert -sv SophistSolutions.pvk -n "CN=SophistSolutionsInc" SophistSolutions.cer

REM Create SPC file (not sure what this is for, but I THINK its still needed even with real PVK/CER)
REM cert2spc SophistSolutions.cer SophistSolutions.spc

REM Copy the (REAL) PVK and CERT from special place for building
copy \\PYTHAGORAS\Led\DigitalCertificate\SophistSolutionsInc-Authenticode.pvk SophistSolutions.pvk
copy \\PYTHAGORAS\Led\DigitalCertificate\SophistSolutionsInc-Authenticode.spc SophistSolutions.spc

REM Sign the CAB file
Echo "Signing the cab files- you must enter a password here(twice)"
signcode -v SophistSolutions.pvk -spc SophistSolutions.spc -t http://timestamp.verisign.com/scripts/timstamp.dll ActiveLedIt.cab
signcode -v SophistSolutions.pvk -spc SophistSolutions.spc -t http://timestamp.verisign.com/scripts/timstamp.dll ActiveLedItDemo.cab

rm -f SophistSolutions.pvk SophistSolutions.spc

REM hack on any machine that will run TEST -signed CAB files
REM "C:\Program files\Inetsdk\bin\setreg" -q 1 "TRUE"
endlocal
