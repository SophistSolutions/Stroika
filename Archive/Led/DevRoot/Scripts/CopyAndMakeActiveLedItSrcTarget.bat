@echo off
rem
rem
rem $Header: /cygdrive/k/CVSRoot/Scripts/CopyAndMakeActiveLedItSrcTarget.bat,v 1.10 2002/05/06 21:31:34 lewis Exp $
rem $Log: CopyAndMakeActiveLedItSrcTarget.bat,v $
rem Revision 1.10  2002/05/06 21:31:34  lewis
rem <========================================= Led 3.0.1 Released ========================================>
rem
rem Revision 1.9  2001/11/27 00:37:54  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.8  2001/11/27 00:28:32  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.7  2001/11/06 14:45:50  lewis
rem lose redundant copies of ReadMe.rtf and ActiveLedItReference.rtf (contained in BUILT subdir) from ActiveLedIt! source distribution. Now we have a new readme file specifically for that product
rem
rem Revision 1.6  2001/08/31 02:24:31  lewis
rem fix spelling error - licence -> license
rem
rem Revision 1.5  2001/08/30 18:41:59  lewis
rem SPR#1005- try to get stuff not applying to TRG-PLATFORM (platform specific to a non-purchased platform) removed
rem
rem Revision 1.4  2000/10/22 14:48:47  lewis
rem header/log and call endlocal at end
rem
rem
rem
setlocal
cp -r %1\ActiveLedIt %2

REM Copy licenses
mkdir %2\Licenses
Copy %1\Licenses\ActiveLedIt30License.htm %2\Licenses\ActiveLedIt30License.htm
Copy %1\Licenses\ActiveLedIt30SourceLicense.htm %2\Licenses\ActiveLedIt30SourceLicense.htm
rm -f %2\ActiveLedItReference.rtf
rm -f %2\ReadMe.rtf
endlocal
