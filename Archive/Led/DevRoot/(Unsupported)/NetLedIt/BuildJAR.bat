@Echo OFF
rem
rem
rem $Header: /cygdrive/k/CVSRoot/(Unsupported)/NetLedIt/BuildJAR.bat,v 1.1 2004/01/01 04:20:05 lewis Exp $
rem $Log: BuildJAR.bat,v $
rem Revision 1.1  2004/01/01 04:20:05  lewis
rem moved NetLedIt to (Unsupported)
rem
rem Revision 1.3  2002/05/06 21:34:50  lewis
rem <=============================== Led 3.0.1 Released ==============================>
rem
rem Revision 1.2  2001/11/27 00:32:53  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.1  2000/10/22 15:01:04  lewis
rem *** empty log message ***
rem
rem
rem


rm -f NetLedIt_WIN.zip
REM rm -f NetLedIt_WIN.jar
"C:\Program Files\WinZip\wzzip" NetLedIt_WIN IntermediateFiles\NetLedIt.class Release\NPNetLedIt.dll
REM mv NetLedIt_WIN.zip NetLedIt_WIN.jar


rm -f NetLedItDemo_WIN.zip
REM rm -f NetLedItDemo_WIN.jar
"C:\Program Files\WinZip\wzzip" NetLedItDemo_WIN IntermediateFiles\NetLedIt.class NetLedIt___Win32_Release_Demo\NPNetLedIt.dll
REM mv NetLedItDemo_WIN.zip NetLedItDemo_WIN.jar

