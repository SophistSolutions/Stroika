# DEPRECATED: make's own build (run by Makefile-Legacy), kept until Stroika v3.0d26 ships - Stroika builds with CMake (CMakeLists.txt)
StroikaSourceRoot	:=	$(abspath ../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Foundation/Common/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/Common/

vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)GUID${OBJ_SUFFIX}\
	$(ObjDir)Locale${OBJ_SUFFIX}\
	$(ObjDir)StdCompat${OBJ_SUFFIX}\
	$(ObjDir)StroikaConfig${OBJ_SUFFIX}\
	$(ObjDir)SystemConfiguration${OBJ_SUFFIX}\
	$(ObjDir)Version${OBJ_SUFFIX}

SubDirs	=	\
	Platform \

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk

all:	$(Objs) $(SubDirs)

.PHONY:	$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s

