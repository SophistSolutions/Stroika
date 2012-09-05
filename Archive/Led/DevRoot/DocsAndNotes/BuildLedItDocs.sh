#!/bin/sh
#
# $Log: BuildLedItDocs.sh,v $
# Revision 2.11  2002/09/04 03:42:04  lewis
# 3.1
#
# Revision 2.10  2001/11/27 00:37:08  lewis
# <=============== Led 3.0 Released ===============>
#
# Revision 2.9  2000/06/17 08:13:56  lewis
# 2.3 -> 3.0
#
# Revision 2.8  1999/08/28 14:08:02  lewis
# change version# from 2.2 to 2.3
#
# Revision 2.7  1997/06/28 17:22:40  lewis
# copy Images and index.htm/index.html files on build.
#
#

CP=cp
GCC=gcc
GREP=egrep
MKDIR=mkdir
RM=rm

DocSrcDir=LedItDocs
MacLedItDir=_LedIt_Mac_
WinLedItDir=_LedIt_Win_
MacLedLineItDir=_LedLineIt_Mac_
WinLedLineItDir=_LedLineIt_Win_


fileList="TableOfContents.html Support.html Commands.html Menus.html AboutLed.html Features.html Installation.html"



echo Making Mac LedIt Docs - ${MacLedItDir}
CompileLedItMac()
{
	${RM} -f TMP.cpp 
	${CP} ${DocSrcDir}/${1} TMP.cpp
	echo " compiling "${1}
	${GCC} -ansi -E -x c -DqVersion="3.1" -DqBuildForLedIt -DqMacOS -DqApplication -DqAppName="LedIt!" TMP.cpp | $GREP -v "# " > ${MacLedItDir}/${1} 
	${RM} -f TMP.cpp 
}
${RM} -rf ${MacLedItDir}
${MKDIR} ${MacLedItDir}
for curFile in ${fileList} ; do CompileLedItMac ${curFile} ; done 
cp ${MacLedItDir}/TableOfContents.html ${MacLedItDir}/index.html
cp -r ${DocSrcDir}/Images ${MacLedItDir}/

echo Making Win LedIt Docs - ${WinLedItDir}
CompileLedItWin()
{
	${RM} -f TMP.cpp 
	${CP} ${DocSrcDir}/${1} TMP.cpp
	echo " compiling "${1}
	${GCC} -ansi -E -x c -DqVersion="3.1" -DqBuildForLedIt -DqWindows -DqApplication -DqAppName="LedIt!" TMP.cpp | $GREP -v "# " > ${WinLedItDir}/${1} 
	${RM} -f TMP.cpp 
}
${RM} -rf ${WinLedItDir}
${MKDIR} ${WinLedItDir}
for curFile in ${fileList} ; do CompileLedItWin ${curFile} ; done 
cp ${WinLedItDir}/TableOfContents.html ${WinLedItDir}/index.htm
cp -r ${DocSrcDir}/Images ${WinLedItDir}/



echo Making Mac LedLineIt Docs - ${MacLedLineItDir}
CompileLedLineItMac()
{
	${RM} -f TMP.cpp 
	${CP} ${DocSrcDir}/${1} TMP.cpp
	echo " compiling "${1}
	${GCC} -ansi -E -x c -DqVersion="3.1" -DqBuildForLedLineIt -DqMacOS -DqApplication -DqAppName="LedLineIt!" TMP.cpp | $GREP -v "# " > ${MacLedLineItDir}/${1} 
	${RM} -f TMP.cpp 
}
${RM} -rf ${MacLedLineItDir}
${MKDIR} ${MacLedLineItDir}
for curFile in ${fileList} ; do CompileLedLineItMac ${curFile} ; done 
cp ${MacLedLineItDir}/TableOfContents.html ${MacLedLineItDir}/index.html
cp -r ${DocSrcDir}/Images ${MacLedLineItDir}/


echo Making Win LedLineIt Docs - ${WinLedLineItDir}
CompileLedLineItWin()
{
	${RM} -f TMP.cpp 
	${CP} ${DocSrcDir}/${1} TMP.cpp
	echo " compiling "${1}
	${GCC} -ansi -E -x c -DqVersion="3.1" -DqBuildForLedLineIt -DqWindows -DqApplication -DqAppName="LedLineIt!" TMP.cpp | $GREP -v "# " > ${WinLedLineItDir}/${1} 
	${RM} -f TMP.cpp 
}
${RM} -rf ${WinLedLineItDir}
${MKDIR} ${WinLedLineItDir}
for curFile in ${fileList} ; do CompileLedLineItWin ${curFile} ; done 
cp ${WinLedLineItDir}/TableOfContents.html ${WinLedLineItDir}/index.htm
cp -r ${DocSrcDir}/Images ${WinLedLineItDir}/


