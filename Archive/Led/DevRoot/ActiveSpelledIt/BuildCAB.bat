@Echo OFF
@SetLocal
rem
rem
rem $Header: /cygdrive/k/CVSRoot/ActiveSpelledIt/BuildCAB.bat,v 1.1 2003/06/10 21:50:34 lewis Exp $
rem $Log: BuildCAB.bat,v $
rem Revision 1.1  2003/06/10 21:50:34  lewis
rem SPR#1526: Added template readme etc files - must rewrite before actual release
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



Echo "Building ActiveSpelledIt! OCX Cab file"
rm -f ActiveSpelledIt.inf
sed s/FILE_VERSION_STRING/%1,%2,%3,%4/g < ActiveSpelledItTemplate.inf > ActiveSpelledIt.inf
REM Make the CAB file (-s leaves room to be signed)
cabarc -s 6144 N ActiveSpelledIt.cab Release\ActiveSpelledIt.ocx ActiveSpelledIt.inf

Echo "Building ActiveSpelledIt! DEMO OCX Cab file"
rm -f ActiveSpelledIt.inf
sed s/FILE_VERSION_STRING/%5,%6,%7,%8/g < ActiveSpelledItTemplate.inf > ActiveSpelledIt.inf
cabarc -s 6144 N ActiveSpelledItDemo.cab Demo-Release\ActiveSpelledIt.ocx ActiveSpelledIt.inf

REM Copy the (REAL) PVK and CERT from special place for building
copy \\PYTHAGORAS\Led\DigitalCertificate\SophistSolutionsInc-Authenticode.pvk SophistSolutions.pvk
copy \\PYTHAGORAS\Led\DigitalCertificate\SophistSolutionsInc-Authenticode.spc SophistSolutions.spc

REM Sign the CAB file
Echo "Signing the cab files- you must enter a password here(twice)"
signcode -v SophistSolutions.pvk -spc SophistSolutions.spc -t http://timestamp.verisign.com/scripts/timstamp.dll ActiveSpelledIt.cab
signcode -v SophistSolutions.pvk -spc SophistSolutions.spc -t http://timestamp.verisign.com/scripts/timstamp.dll ActiveSpelledItDemo.cab

rm -f SophistSolutions.pvk SophistSolutions.spc

REM hack on any machine that will run TEST -signed CAB files
REM "C:\Program files\Inetsdk\bin\setreg" -q 1 "TRUE"
endlocal
