StroikaRoot=$(abspath ../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=   $(StroikaRoot)Library/Sources/Stroika/Foundation/Time/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Time/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=/Fd$(call FUNCTION_CONVERT_FILES_TO_COMPILER_NATIVE,$(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Foundation.pdb)
endif

vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)Date${OBJ_SUFFIX}\
	$(ObjDir)DateTime${OBJ_SUFFIX}\
	$(ObjDir)Duration${OBJ_SUFFIX}\
	$(ObjDir)DurationRange${OBJ_SUFFIX}\
	$(ObjDir)Realtime${OBJ_SUFFIX}\
	$(ObjDir)TimeOfDay${OBJ_SUFFIX}\
	$(ObjDir)Timezone${OBJ_SUFFIX}\


all:	$(Objs)

