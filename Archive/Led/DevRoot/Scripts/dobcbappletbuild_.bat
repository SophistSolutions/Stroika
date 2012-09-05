@echo off
rem
rem
rem $Header: /cygdrive/k/CVSRoot/Scripts/dobcbappletbuild_.bat,v 1.5 2002/11/22 02:54:49 lewis Exp $
rem $Log: dobcbappletbuild_.bat,v $
rem Revision 1.5  2002/11/22 02:54:49  lewis
rem *** empty log message ***
rem
rem Revision 1.4  2002/05/06 21:31:40  lewis
rem <========================================= Led 3.0.1 Released ========================================>
rem
rem Revision 1.3  2001/11/27 00:37:57  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.2  2001/11/27 00:28:36  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.1  2001/01/20 14:23:15  lewis
rem add build borland CPP Builder test
rem
rem
rem
rem
setlocal
echo "Doing BUILD for %1"
cd %1
mkdir Objs
bcb %2 /b /obuildResults.txt
grep -i error buildResults.txt
echo .
echo .
echo .
endlocal
