StroikaRoot=$(abspath ../../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

SrcDir	=   $(StroikaRoot)Library/Sources/Stroika/Frameworks/SystemPerformance/Support/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/SystemPerformance/Support/

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=/Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Frameworks.pdb)
endif


vpath %cpp $(SrcDir)

Objs	=	\


ifneq ($(findstring Windows,$(TARGET_PLATFORMS)),)
Objs+=	$(ObjDir)WMICollector${OBJ_SUFFIX}
endif

include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk

all:	$(Objs)

