# DEPRECATED: make's own build (run by Makefile-Legacy), kept until Stroika v3.0d26 ships - Stroika builds with CMake (CMakeLists.txt)
StroikaSourceRoot	:=	$(abspath ../../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Foundation/Cryptography/Digest/Algorithm/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/Cryptography/Digest/Algorithm/

vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)CRC32${OBJ_SUFFIX}\
	$(ObjDir)Jenkins${OBJ_SUFFIX}\
	$(ObjDir)MD5${OBJ_SUFFIX}\
	$(ObjDir)SuperFastHash${OBJ_SUFFIX}\

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)

