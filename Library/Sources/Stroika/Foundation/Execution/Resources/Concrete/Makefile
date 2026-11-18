# DEPRECATED: make's own build (run by Makefile-Legacy), kept until Stroika v3.0d26 ships - Stroika builds with CMake (CMakeLists.txt)
StroikaSourceRoot	:=	$(abspath ../../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Foundation/Execution/Resources/Concrete/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/Execution/Resources/Concrete/

vpath %cpp $(SrcDir)

Objs	:=

ifneq ($(findstring POSIX,$(TargetPlatforms)),)
Objs+=	$(ObjDir)ElfResourceManager${OBJ_SUFFIX}
endif
ifneq ($(findstring Windows,$(TargetPlatforms)),)
Objs+=	$(ObjDir)WindowsResourceManager${OBJ_SUFFIX}
endif

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk

all:	 $(Objs)



