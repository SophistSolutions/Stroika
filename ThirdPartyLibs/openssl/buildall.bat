@echo off

rem
rem Copyright(c) Records For Living, Inc. 2004-2012.  All rights reserved
rem

setlocal

set BLD_TRG=%1%
if '%BLD_TRG%'=='' set BLD_TRG=Build


Echo **************************** STARTING BUILD: openssl **************************
sh -c 'date'


REM DoCreateSymLink - this isn't currently used, and doesn't work well with windows (sets sys file attribute which casues
REM DOS delete file issues) - and slink doesnt really work except in cygwin tools
Set DoCreateSymLink=0


Set GZFILENAME=openssl-1.0.0.tar.gz
set EXTRACTED_DIRNAME=openssl-1.0.0
set SLINKDIRNAME=openssl-1.0.0
set USE_DIRNAME=CURRENT

if '%BLD_TRG%'=='Clean' rm -rf %SLINKDIRNAME%
if '%BLD_TRG%'=='Clean' rm -rf %USE_DIRNAME%
if '%BLD_TRG%'=='Clean' goto DONE


REM For MSBuild ETC
call ..\..\BuildScripts\SetupBuildCommonVars.bat



set DOEXTRACT=
if NOT EXIST %USE_DIRNAME%\openssl.spec set DOEXTRACT=1
if "%DOEXTRACT%"=="1" echo DOING %SLINKDIRNAME% EXTRACT
if "%DOEXTRACT%" NEQ "1" echo SKIPING %SLINKDIRNAME% EXTRACT
if "%DOEXTRACT%" NEQ "1" goto DONE_WITH_UNZIP
REM if "%DOEXTRACT%"=="1" copy Origs\%GZFILENAME% .
REM if "%DOEXTRACT%"=="1" gzip -d %GZFILENAME%
if "%DOEXTRACT%"=="1" rm -f %SLINKDIRNAME%
if "%DOEXTRACT%"=="1" tar xf Origs/%GZFILENAME% --no-same-owner
REM AVOID RARE BUT OCCASIONAL RACE AFTER END OF TAR BEFORE MV WORKS (cb cygwin bug or antivirus scan?)
sleep 1
if "%DOEXTRACT%"=="1" IF "%EXTRACTED_DIRNAME%" NEQ "%USE_DIRNAME%" mv "%EXTRACTED_DIRNAME%" "%USE_DIRNAME%"
if "%DOEXTRACT%"=="1" rm -f %SLINKDIRNAME%
if "%DOEXTRACT%"=="1" IF "%DoCreateSymLink%"=="1" ln -s %USE_DIRNAME% %SLINKDIRNAME%

if "%DOEXTRACT%"=="1" echo Patching openssl
if "%DOEXTRACT%"=="1" patch -t CURRENT\e_os.h Patches\e_os.h.patch
if "%DOEXTRACT%"=="1" pushd CURRENT && attrib -S /S && popd

:DONE_WITH_UNZIP


pushd %USE_DIRNAME%


REM instructions on how todo build from INSTALL.W32


set ASMARGS_CONFIGURE = nasm
REM set ASMARGS_CONFIGURE = no-asm


REM - only reconfigure if we just did the extract
if "%DOEXTRACT%" NEQ "1" Echo Skipping 'Configure' script since already configured
if "%DOEXTRACT%"=="1" perl Configure VC-WIN32 %ASMARGS_CONFIGURE% --prefix=c:\some\openssl\dir
if "%DOEXTRACT%"=="1" call ms\do_nasm

REM not sure why this file created (in ms\do_nasm) but causes problems for windows....
REM 		--LGP 2012-11-02
if "%DOEXTRACT%"=="1" rm -f NUL

REM this is REDUNDANT. Some of it done in ms\do_nasm. This redoes just
REM the one makefile we are acutally using (with the appropriately defined nams/no-nasm arg)
perl util\mk1mf.pl %ASMARGS_CONFIGURE% VC-WIN32 >ms\nt.mak

REM Also do a DEBUG make (see innerds of ms\do_nasm and http://www.opensource.apple.com/darwinsource/Current/OpenSSL096-6.2/openssl/INSTALL.W32
perl util\mk1mf.pl debug %ASMARGS_CONFIGURE% VC-WIN32 >ms\nt-DBG.mak


REM the above script calls echo on??
@echo off


REM use -s to slightly lessen the verbosity of the makefile (LGP 2008-10-04)
REM But - alas - the -s doesn't appear to do much to suppress noise.
REM SO - do 2 builds in a row - the first does most of the work, and we ignore its output (save to file)
REM IF it fails - when we re-run, we'll see the error (which is why we run twice) 

REM this trick make line is just to make build more quiet - it can be elimianted and makes all
REM To get rid of it - just delete the 2 make lines that use this define
SET JUNK_MUST_BLD_TO_GET_SILENT_BUT_SUCCESSFUL_BUILD = tmp32 inc32 out32 inc32\openssl headers lib

Echo Make Release Build... (first  quietly - output saved to .txt file)
nmake /NOLOGO -s -f ms\nt.mak banner
nmake /NOLOGO -s -f ms\nt.mak %JUNK_MUST_BLD_TO_GET_SILENT_BUT_SUCCESSFUL_BUILD% >> NT.MAK.BUILD-Output.txt
nmake /NOLOGO -s -f ms\nt.mak

Echo Make Debug Build... (first  quietly - output saved to .txt file)
nmake /NOLOGO -s -f ms\nt-DBG.mak banner
nmake /NOLOGO -s -f ms\nt-DBG.mak %JUNK_MUST_BLD_TO_GET_SILENT_BUT_SUCCESSFUL_BUILD% >> NT-DBG.MAK.BUILD-Output.txt
nmake /NOLOGO -s -f ms\nt-DBG.mak

Echo running openssl tests...
sh -c "nmake /NOLOGO -s -f ms/nt.mak test 2>&1" > TEST-OUT.txt
sh -c "nmake /NOLOGO -s -f ms/nt-DBG.mak test 2>&1" > TEST-DBG-OUT.txt


popd


echo Test DIFFS (REDO THIS IN PERL WHERE ITS EASIER TO COUNT LINES ETC)
echo 2 lines coming next - each should be less than 40/44 to be safe...
diff -b CURRENT/TEST-OUT.txt REFERENCEOUTOUT.txt | wc -l
diff -b CURRENT/TEST-DBG-OUT.txt REFERENCEOUTOUT.txt | wc -l


call checkall.bat
sh -c 'date'
Echo **************************** ENDING build: openssl ****************************


:DONE
endlocal
