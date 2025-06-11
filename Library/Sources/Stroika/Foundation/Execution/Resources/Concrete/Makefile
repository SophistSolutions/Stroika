StroikaRoot	:=	$(abspath ../../../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Foundation/Execution/Resources/Concrete/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Execution/Resources/Concrete/

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=-Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaLibDir)stroika-foundation.pdb)
endif

vpath %cpp $(SrcDir)

Objs	:=

ifneq ($(findstring POSIX,$(TargetPlatforms)),)
Objs+=	$(ObjDir)ElfResourceManager${OBJ_SUFFIX}
endif
ifneq ($(findstring Windows,$(TargetPlatforms)),)
Objs+=	$(ObjDir)WindowsResourceManager${OBJ_SUFFIX}
endif


all:	 $(Objs)



