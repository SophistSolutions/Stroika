StroikaSourceRoot	:=	$(abspath ../../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Foundation/Execution/Platform/Windows/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/Execution/Platform/Windows/

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk

vpath %cpp $(SrcDir)

Objs	:=

ifneq ($(findstring Windows,$(TargetPlatforms)),)
Objs+=	$(ObjDir)Exception${OBJ_SUFFIX}
Objs+=	$(ObjDir)HRESULTErrorException${OBJ_SUFFIX}
Objs+=	$(ObjDir)StructuredException${OBJ_SUFFIX}
Objs+=	$(ObjDir)Users${OBJ_SUFFIX}
Objs+=	$(ObjDir)WaitSupport${OBJ_SUFFIX}
endif


all:	 $(Objs)



