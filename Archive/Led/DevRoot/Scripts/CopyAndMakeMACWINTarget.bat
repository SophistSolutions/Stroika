@echo off
rem
rem
rem $Header: /cygdrive/k/CVSRoot/Scripts/CopyAndMakeMACWINTarget.bat,v 1.14 2004/01/04 18:31:11 lewis Exp $
rem $Log: CopyAndMakeMACWINTarget.bat,v $
rem Revision 1.14  2004/01/04 18:31:11  lewis
rem a few misc cleanups, NetLedIt loss, added ActiveSpelledIt, etc
rem
rem Revision 1.13  2002/05/06 21:31:35  lewis
rem <========================================= Led 3.0.1 Released ========================================>
rem
rem Revision 1.12  2001/11/27 00:37:54  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.11  2001/11/27 00:28:33  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.10  2001/08/31 02:24:32  lewis
rem fix spelling error - licence -> license
rem
rem Revision 1.9  2001/08/30 22:24:23  lewis
rem SPR#1005- more work on getting right project names and deleting unneeded modules for particular packages
rem
rem Revision 1.8  2001/08/30 18:41:59  lewis
rem SPR#1005- try to get stuff not applying to TRG-PLATFORM (platform specific to a non-purchased platform) removed
rem
rem Revision 1.7  2001/05/25 13:26:06  lewis
rem SPR#0919- merge LedItPP code (etc) into just one LedIt app
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
rm -f %2/Led/Headers/Led_Gtk*.h
rm -f %2/Led/Sources/Led_Gtk*.cpp
rm -f %2/LedIt/Makefile
rm -f %2/LedIt/*UNIX*

rem Lose Unneeded files in ProjectDir
rm -f %2/LedIt/Makefile
rm -f %2/LedIt/*UNIX*
rmdir /s /q "%2\LedIt\Built\XWindows"
rm -f %2/LedIt/Built/*.tar.*

rem Lose Unneeded TopLevelDirs
rmdir /s /q "%2\LedTestXWindows"
rmdir /s /q "%2\ActiveLedIt"
rmdir /s /q "%2\ActiveSpelledIt"
rmdir /s /q "%2\Scripts"

rem Lose Unneeded "Licenses"
rm -f %2/Licenses/Active*

endlocal
