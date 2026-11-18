# DEPRECATED: make's own build (run by Makefile-Legacy), kept until Stroika v3.0d26 ships - Stroika builds with CMake (CMakeLists.txt)
StroikaSourceRoot	:=	$(abspath ../../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Frameworks/UPnP/SSDP/Server/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Frameworks/UPnP/SSDP/Server/

vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)BasicServer${OBJ_SUFFIX}\
	$(ObjDir)LocationProvider${OBJ_SUFFIX}\
	$(ObjDir)PeriodicNotifier${OBJ_SUFFIX}\
	$(ObjDir)SearchResponder${OBJ_SUFFIX}\


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)
