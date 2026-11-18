# DEPRECATED: make's own build (run by Makefile-Legacy), kept until Stroika v3.0d26 ships - Stroika builds with CMake (CMakeLists.txt)
StroikaSourceRoot	:=	$(abspath ../../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Foundation/IO/Network/HTTP/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/IO/Network/HTTP/

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)CacheControl${OBJ_SUFFIX}\
	$(ObjDir)ClientErrorException${OBJ_SUFFIX}\
	$(ObjDir)ContentEncoding${OBJ_SUFFIX}\
	$(ObjDir)Cookie${OBJ_SUFFIX}\
	$(ObjDir)Exception${OBJ_SUFFIX}\
	$(ObjDir)Headers${OBJ_SUFFIX}\
	$(ObjDir)KeepAlive${OBJ_SUFFIX}\
	$(ObjDir)MessageStartTextInputStreamBinaryAdapter${OBJ_SUFFIX}\
	$(ObjDir)Request${OBJ_SUFFIX}\
	$(ObjDir)Response${OBJ_SUFFIX}\
	$(ObjDir)TransferEncoding${OBJ_SUFFIX}\


all:	$(Objs)

