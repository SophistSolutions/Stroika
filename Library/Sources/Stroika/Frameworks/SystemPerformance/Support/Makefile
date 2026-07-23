StroikaRoot	:=	$(abspath ../../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Frameworks/SystemPerformance/Support/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/SystemPerformance/Support/

include $(StroikaRoot)Build/ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=-Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaLibDir)stroika-frameworks.pdb)
endif


vpath %cpp $(SrcDir)

Objs	:=	\


ifneq ($(findstring Windows,$(TargetPlatforms)),)
Objs+=	$(ObjDir)WMICollector${OBJ_SUFFIX}
endif

include $(StroikaRoot)Build/ScriptsLib/SharedBuildRules-Default.mk

all:	$(Objs)

