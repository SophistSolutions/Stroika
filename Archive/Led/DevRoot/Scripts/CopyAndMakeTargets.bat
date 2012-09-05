@echo off
rem
rem
rem $Header: /cygdrive/k/CVSRoot/Scripts/CopyAndMakeTargets.bat,v 1.20 2004/01/04 22:12:36 lewis Exp $
rem $Log: CopyAndMakeTargets.bat,v $
rem Revision 1.20  2004/01/04 22:12:36  lewis
rem *** empty log message ***
rem
rem Revision 1.19  2004/01/04 18:31:12  lewis
rem a few misc cleanups, NetLedIt loss, added ActiveSpelledIt, etc
rem
rem Revision 1.18  2004/01/01 04:05:19  lewis
rem lose NetLedIt stuff
rem
rem Revision 1.17  2003/06/12 23:12:32  lewis
rem SPR#1536: change names like WIN and MAC to studly caps
rem
rem Revision 1.16  2003/05/09 20:03:12  lewis
rem SPR#1479: Lose build of ActiveLedIt SDK and Demo SDK from CopyAndMakeTargets.bat. Add extra arg to script which is host to build linux stuff (was linicus now usually vmlinux). gzip arg now must be --best.
rem
rem Revision 1.15  2003/03/28 12:25:10  lewis
rem *** empty log message ***
rem
rem Revision 1.14  2003/03/24 14:39:09  lewis
rem hopefully fix the final build/copy script for ActiveLedItDemo package
rem
rem Revision 1.13  2003/01/24 21:51:33  lewis
rem add support for building an ActiveLedIt DEMO SDK
rem
rem Revision 1.12  2002/05/14 15:50:02  lewis
rem fix name from NedLedIt to NetLedIt
rem
rem Revision 1.11  2002/05/06 21:31:37  lewis
rem <========================================= Led 3.0.1 Released ========================================>
rem
rem Revision 1.10  2001/11/27 00:37:56  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.9  2001/11/27 00:28:34  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.8  2001/05/30 23:06:44  lewis
rem SPR#0947 - copy IntermediateFiles dir for NetLedItSRC targets
rem
rem Revision 1.7  2000/10/23 21:36:49  lewis
rem *** empty log message ***
rem
rem Revision 1.6  2000/10/23 21:10:15  lewis
rem Add echo statements so I can see progress better
rem
rem Revision 1.5  2000/10/23 20:53:05  lewis
rem *** empty log message ***
rem
rem Revision 1.4  2000/10/22 15:00:43  lewis
rem NETLEDIT support
rem
rem Revision 1.3  2000/10/22 14:49:10  lewis
rem header/log and call endlocal at end
rem
rem
rem
setlocal
REM Call with full version name arg and path to build dir to copy extra stuff- as in : "CallAndMakeTargets 3.0b2 m:\ vmlinux1"

Echo Making the _Target_ directories

Echo --Led_src_Win_TrgWin_%1--
call Led_src_Win_%1\scripts\CopyAndMakeWINTarget.bat Led_src_Win_%1 Led_src_Win_TrgWin_%1

Echo --Led_src_Win_TrgMacWin_%1--
call Led_src_Win_%1\scripts\CopyAndMakeMACWINTarget.bat Led_src_Win_%1 Led_src_Win_TrgMacWin_%1

Echo --Led_src_Mac_TrgMac_%1--
call Led_src_Win_%1\scripts\CopyAndMakeMACTarget.bat Led_src_Mac_%1 Led_src_Mac_TrgMac_%1

Echo --ActiveLedIt_src_Win_TrgWin_%1--
call Led_src_Win_%1\scripts\CopyAndMakeActiveLedItSrcTarget.bat Led_src_Win_%1 ActiveLedIt_src_Win_TrgWin_%1

Echo --ActiveSpelledIt_src_Win_TrgWin_%1--
call Led_src_Win_%1\scripts\CopyAndMakeActiveSpelledItSrcTarget.bat Led_src_Win_%1 ActiveSpelledIt_src_Win_TrgWin_%1

Echo --ActiveLedIt_SDKs_%1--
copy ActiveLedIt_src_Win_TrgWin_%1\Built\ActiveLedIt*Installer*.exe  .

Echo --ActiveSpelledIt_SDKs_%1--
copy ActiveSpelledIt_src_Win_TrgWin_%1\Built\ActiveSpelledIt*Installer*.exe  .

Echo --Led_src_XWin_TrgXWin_%1--
call Led_src_Win_%1\scripts\CopyAndMakeXTarget.bat Led_src_XWindows_%1 Led_src_XWin_TrgXWin_%1


Echo Zipping up zip files
"C:\Program Files\WinZip\wzzip" -P -r -ex ActiveLedIt_src_Win_TrgWin_%1.zip ActiveLedIt_src_Win_TrgWin_%1
"C:\Program Files\WinZip\wzzip" -P -r -ex ActiveSpelledIt_src_Win_TrgWin_%1.zip ActiveSpelledIt_src_Win_TrgWin_%1
"C:\Program Files\WinZip\wzzip" -P -r -ex Led_src_Win_TrgWin_%1.zip Led_src_Win_TrgWin_%1
"C:\Program Files\WinZip\wzzip" -P -r -ex Led_src_Win_TrgMacWin_%1.zip Led_src_Win_TrgMacWin_%1


Echo GZipping UNIX files
rsh %3 rm -rf Led_src_XWin_TrgXWin_%1 Led_src_XWin_TrgXWin_%1.tar.gz
rcp -b -r Led_src_XWin_TrgXWin_%1 %3:
rsh %3 chmod +x Led_src_XWin_TrgXWin_%1/LedTestXWindows/Built/LedTestXWindows
rsh %3 tar cvf Led_src_XWin_TrgXWin_%1.tar Led_src_XWin_TrgXWin_%1
rsh %3 gzip --best Led_src_XWin_TrgXWin_%1.tar
rcp -b %3:Led_src_XWin_TrgXWin_%1.tar.gz .
endlocal
