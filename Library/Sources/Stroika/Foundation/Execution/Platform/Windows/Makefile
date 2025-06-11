StroikaRoot	:=	$(abspath ../../../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Foundation/Execution/Platform/Windows/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Execution/Platform/Windows/

include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=-Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaLibDir)stroika-foundation.pdb)
endif

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



