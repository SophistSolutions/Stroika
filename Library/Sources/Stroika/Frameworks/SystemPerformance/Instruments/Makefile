# DEPRECATED: make's own build (run by Makefile-Legacy), kept until Stroika v3.0d26 ships - Stroika builds with CMake (CMakeLists.txt)
StroikaSourceRoot	:=	$(abspath ../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=   $(StroikaSourceRoot)Library/Sources/Stroika/Frameworks/SystemPerformance/Instruments/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Frameworks/SystemPerformance/Instruments/

vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)CPU${OBJ_SUFFIX}\
	$(ObjDir)Memory${OBJ_SUFFIX}\
	$(ObjDir)Filesystem${OBJ_SUFFIX}\
	$(ObjDir)Network${OBJ_SUFFIX}\
	$(ObjDir)Process${OBJ_SUFFIX}\


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)

