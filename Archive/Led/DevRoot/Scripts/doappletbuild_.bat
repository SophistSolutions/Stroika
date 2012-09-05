@echo off
rem
rem
rem $Header: /cygdrive/k/CVSRoot/Scripts/doappletbuild_.bat,v 1.10 2003/06/24 14:12:19 lewis Exp $
rem $Log: doappletbuild_.bat,v $
rem Revision 1.10  2003/06/24 14:12:19  lewis
rem SPR#1542: added SpelledIt\MungeDictionaries&ScavangeWordsFromNet to buildall.bat etc
rem
rem Revision 1.9  2003/06/10 17:09:32  lewis
rem SPR#1526: build ActiveSpelledIt in buildall.bat script
rem
rem Revision 1.8  2003/05/09 19:47:09  lewis
rem SPR#1476: Fix build script to build NetLedIt release build.
rem
rem Revision 1.7  2003/03/25 14:44:00  lewis
rem get rid of WrapperSetup build and only do Release build for AutoDoc
rem
rem Revision 1.6  2002/09/04 03:59:47  lewis
rem replace old (MUCH EASIER MUCH BETTER) MSDEV cmdline calls with to DEVENV. TRY to find a better way (equiv of /rebuildall)
rem
rem Revision 1.5  2002/05/06 21:31:39  lewis
rem <========================================= Led 3.0.1 Released ========================================>
rem
rem Revision 1.4  2001/11/27 00:37:57  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.3  2001/11/27 00:28:36  lewis
rem <=============== Led 3.0 Released ===============>
rem
rem Revision 1.2  2000/10/22 14:49:10  lewis
rem header/log and call endlocal at end
rem
rem
rem
setlocal
echo "Doing BUILD for %1"
cd %1
Rem msdev %2 /make all
Rem With MSDEV (60) it was so nice and simple!  Now I must know each config name!
Rem Sigh. Hardwire/hack for each config name I need based on directory name...

if %1==AutoDoc devenv %2 /build "Release"
if NOT %1==AutoDoc (
	if NOT %1==NetLedIt (
		if NOT %1==ActiveSpelledIt (
			if NOT %1==SpelledIt\MungeDictionaries (
				if NOT %1==SpelledIt\ScavangeWordsFromNet (
					devenv %2 /build "Full UNICODE Release"
					devenv %2 /build "Partial UNICODE Release"
					if %1==ActiveLedIt devenv %2 /build "Partial UNICODE Demo Release"
				)
			)
		)
	)
)
if %1==NetLedIt devenv %2 /build "Release Demo"
if %1==NetLedIt devenv %2 /build "Release"
if %1==ActiveSpelledIt devenv %2 /build "Demo-Release"
if %1==ActiveSpelledIt devenv %2 /build "Release"
if %1==SpelledIt\MungeDictionaries devenv %2 /build "Release"
if %1==SpelledIt\ScavangeWordsFromNet devenv %2 /build "Release"
echo .
echo .
echo .
endlocal
