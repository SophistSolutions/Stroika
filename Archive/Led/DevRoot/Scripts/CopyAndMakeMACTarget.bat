@echo off
rem
rem
rem $Header: /cygdrive/k/CVSRoot/Scripts/CopyAndMakeMACTarget.bat,v 1.18 2003/06/12 23:13:37 lewis Exp $
rem $Log: CopyAndMakeMACTarget.bat,v $
rem Revision 1.18  2003/06/12 23:13:37  lewis
rem lose Led_ATL for Mac/XWin
rem
rem Revision 1.17  2002/05/06 21:31:35  lewis
rem <========================================= Led 3.0.1 Released ========================================>
rem
rem Revision 1.16  2001/11/27 00:37:54  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.15  2001/11/27 00:28:33  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.14  2001/08/31 02:24:32  lewis
rem fix spelling error - licence -> license
rem
rem Revision 1.13  2001/08/30 22:24:23  lewis
rem SPR#1005- more work on getting right project names and deleting unneeded modules for particular packages
rem
rem Revision 1.12  2001/08/30 18:41:59  lewis
rem SPR#1005- try to get stuff not applying to TRG-PLATFORM (platform specific to a non-purchased platform) removed
rem
rem Revision 1.11  2001/05/25 13:26:06  lewis
rem SPR#0919- merge LedItPP code (etc) into just one LedIt app
rem
rem Revision 1.10  2001/01/20 18:20:09  lewis
rem BorlandCPPBuilderTest
rem
rem Revision 1.9  2000/10/23 20:12:11  lewis
rem lose netledits not needed etc from diff projects
rem
rem Revision 1.8  2000/10/22 14:48:47  lewis
rem header/log and call endlocal at end
rem
rem
rem
setlocal
xcopy /E /Q %1 %2\

rem Lose Unneeded Led library headers
rm -f %2/Led/Headers/Led_Win32*.h
rm -f %2/Led/Sources/Led_Win32*.cpp
rm -f %2/Led/Headers/Led_MFC*.h
rm -f %2/Led/Sources/Led_MFC*.cpp
rm -f %2/Led/Headers/Led_ATL*.h
rm -f %2/Led/Sources/Led_ATL*.cpp
rm -f %2/Led/Headers/Led_Gtk*.h
rm -f %2/Led/Sources/Led_Gtk*.cpp

rem Lose Unneeded "Unsupported"
rmdir /s /q "%2\(Unsupported)\LedItMFC"

rem Lose Unneeded "AuxiliaryPackages"
rmdir /s /q "%2\AuxiliaryPackages\Quicktime for Windows"
rmdir /s /q "%2\AuxiliaryPackages\Win32NetscapePluginSDK"
rmdir /s /q "%2\AuxiliaryPackages\WrapperSetup"

rem Lose Unneeded files in ProjectDir
rm -f %2/LedIt/*.dsw
rm -f %2/LedIt/*.dsp
rm -f %2/LedIt/Makefile
rm -f %2/LedIt/*UNIX*
rm -f %2/LedIt/*Win32*
rmdir /s /q "%2\LedIt\Built\Win32"
rmdir /s /q "%2\LedIt\Built\XWindows"
rm -f %2/LedIt/Built/*.zip
rm -f %2/LedIt/Built/*.tar.*
rm -f %2/LedIt/Built/*.exe
rm -f %2/LedIt/Built/*.msi

rem Lose Unneeded TopLevelDirs
rmdir /s /q "%2\BorlandCPPBuilderTest"
rmdir /s /q "%2\LedLineItMFC"
rmdir /s /q "%2\LedTestWin32"
rmdir /s /q "%2\LedTestXWindows"
rmdir /s /q "%2\ActiveLedIt"
rmdir /s /q "%2\NetLedIt"
rmdir /s /q "%2\Scripts"

rem Lose Unneeded "Licenses"
rm -f %2/Licenses/Active*
rm -f %2/Licenses/NetLed*

endlocal
