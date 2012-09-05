@echo off
rem
rem
rem $Header: /cygdrive/k/CVSRoot/Scripts/CopyAndMakeWINTarget.bat,v 1.13 2004/01/04 18:31:12 lewis Exp $
rem $Log: CopyAndMakeWINTarget.bat,v $
rem Revision 1.13  2004/01/04 18:31:12  lewis
rem a few misc cleanups, NetLedIt loss, added ActiveSpelledIt, etc
rem
rem Revision 1.12  2002/05/06 21:31:38  lewis
rem <========================================= Led 3.0.1 Released ========================================>
rem
rem Revision 1.11  2001/11/27 00:37:56  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.10  2001/11/27 00:28:35  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.9  2001/09/15 23:18:04  lewis
rem lose Led_TCL stuff from (unsupported)
rem
rem Revision 1.8  2001/08/31 02:24:32  lewis
rem fix spelling error - licence -> license
rem
rem Revision 1.7  2001/08/30 22:24:23  lewis
rem SPR#1005- more work on getting right project names and deleting unneeded modules for particular packages
rem
rem Revision 1.6  2000/10/23 20:12:11  lewis
rem lose netledits not needed etc from diff projects
rem
rem Revision 1.5  2000/10/22 14:49:10  lewis
rem header/log and call endlocal at end
rem
rem
rem
setlocal
cp -r %1 %2

rem Lose Unneeded Led library headers
rm -f %2/Led/Headers/Led_MacOS*.h
rm -f %2/Led/Sources/Led_MacOS*.cpp
rm -f %2/Led/Headers/Led_PP*.h
rm -f %2/Led/Sources/Led_PP*.cpp
rm -f %2/Led/Headers/Led_Gtk*.h
rm -f %2/Led/Sources/Led_Gtk*.cpp

rem Lose Unneeded "Unsupported"
rmdir /s /q "%2\(Unsupported)\LedItPP"
rmdir /s /q "%2\(Unsupported)\LedTestTCL"
rm -f %2/(Unsupported)/Led_TCL.*


rem Lose Unneeded "AuxiliaryPackages"
rm -rf "%2\AuxiliaryPackages\Internet Config Support"
rm -rf "%2\AuxiliaryPackages\MacOSNetscapePluginSDK"

rem Lose Unneeded files in ProjectDir
rm -f %2/LedIt/Makefile
rm -f %2/LedIt/*UNIX*
rmdir /s /q "%2\LedIt\Built\MacOS"
rmdir /s /q "%2\LedIt\Built\XWindows"
rm -f %2/LedIt/Built/*.tar.*
rm -f %2/LedIt/Built/*.sit

rem Lose Unneeded TopLevelDirs
rmdir /s /q "%2\LedLineItPP"
rmdir /s /q "%2\LedTestXWindows"
rmdir /s /q "%2\ActiveLedIt"
rmdir /s /q "%2\ActiveSpelledIt"
rmdir /s /q "%2\Scripts"

rem Lose Unneeded "Licenses"
rm -f %2/Licenses/Active*

endlocal
