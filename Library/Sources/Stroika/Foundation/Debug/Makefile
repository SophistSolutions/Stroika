# DEPRECATED: make's own build (run by Makefile-Legacy), kept until Stroika v3.0d26 ships - Stroika builds with CMake (CMakeLists.txt)
StroikaSourceRoot	:=	$(abspath ../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Foundation/Debug/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/Debug/

vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)Assertions${OBJ_SUFFIX}\
	$(ObjDir)AssertExternallySynchronizedChecker${OBJ_SUFFIX}\
	$(ObjDir)BackTrace${OBJ_SUFFIX}\
	$(ObjDir)Debugger${OBJ_SUFFIX}\
	$(ObjDir)Demangle${OBJ_SUFFIX}\
	$(ObjDir)Fatal${OBJ_SUFFIX}\
	$(ObjDir)MallocGuard${OBJ_SUFFIX}\
	$(ObjDir)Main${OBJ_SUFFIX}\
	$(ObjDir)Trace${OBJ_SUFFIX}\
	$(ObjDir)Visualizations${OBJ_SUFFIX}\

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk

all:	$(Objs)
