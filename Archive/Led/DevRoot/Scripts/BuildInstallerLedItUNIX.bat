@ECHO OFF
rem
rem
rem $Header: /cygdrive/k/CVSRoot/Scripts/BuildInstallerLedItUNIX.bat,v 1.8 2003/06/05 22:46:06 lewis Exp $
rem $Log: BuildInstallerLedItUNIX.bat,v $
rem Revision 1.8  2003/06/05 22:46:06  lewis
rem dont hardwire linicus - but use ARG#2 as UNIXHOST
rem
rem Revision 1.7  2003/05/08 23:49:30  lewis
rem *** empty log message ***
rem
rem Revision 1.6  2002/11/04 02:49:04  lewis
rem version 3.1
rem
rem Revision 1.5  2002/05/06 21:31:32  lewis
rem <========================================= Led 3.0.1 Released ========================================>
rem
rem Revision 1.4  2001/11/27 00:37:52  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.3  2001/11/27 00:28:31  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.2  2001/05/25 13:15:48  lewis
rem chmod +x on .sh scriptfile
rem
rem Revision 1.1  2001/05/24 21:01:23  lewis
rem SPR#0934- first cut at building ledit UNIX installer
rem
rem
rem
rem
setlocal
Set INSTNAME=Install_LedIt_Linux_3.1%1
set UNIXHOST=%2
rm -rf \LedIt\Built\XWindows
mkdir \LedIt\Built
mkdir \LedIt\Built\XWindows
rcp -b %UNIXHOST%:ExportDir/LedIt/LedIt-NoUnicode \LedIt\Built\XWindows\
rsh %UNIXHOST% (cd ExportDir/LedIt; rm -rf bin data.tar; mkdir bin; cp LedIt-NoUnicode bin/LedIt; chmod 755 bin/LedIt; tar cf data.tar bin LedItUNIXReadMe.rtf)
rsh %UNIXHOST% (cd ExportDir/LedIt; rm -rf %INSTNAME%; mkdir %INSTNAME%; cp data.tar UNIXInstall.sh LedItUNIXInstall.txt %INSTNAME%; chmod +x %INSTNAME%/UNIXInstall.sh; tar cf %INSTNAME%.tar %INSTNAME%; gzip --best %INSTNAME%.tar)
rcp -b %UNIXHOST%:ExportDir/LedIt/%INSTNAME%.tar.gz \LedIt\Built\
endlocal
