@echo off
rem
rem
rem $Header: /cygdrive/k/CVSRoot/Scripts/CopyAndMakeActiveSpelledItSrcTarget.bat,v 1.1 2004/01/04 22:05:01 lewis Exp $
rem $Log: CopyAndMakeActiveSpelledItSrcTarget.bat,v $
rem Revision 1.1  2004/01/04 22:05:01  lewis
rem *** empty log message ***
rem
rem
rem
rem
setlocal
cp -r %1\ActiveSpelledIt %2

REM Copy licenses
mkdir %2\Licenses
Copy %1\Licenses\ActiveSpelledIt31License.htm %2\Licenses\
Copy %1\Licenses\ActiveSpelledIt31SourceLicense.htm %2\Licenses\
rm -f %2\ActiveSpelledItReference.rtf
rm -f %2\ReadMe.rtf
endlocal
