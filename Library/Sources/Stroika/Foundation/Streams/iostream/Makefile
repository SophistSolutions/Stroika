StroikaRoot=$(abspath ../../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=   $(StroikaRoot)Library/Sources/Stroika/Foundation/Streams/iostream/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Streams/iostream/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=/Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Foundation.pdb)
endif

vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)CommonInsertersAndExtractors${OBJ_SUFFIX}\
	$(ObjDir)FStreamSupport${OBJ_SUFFIX}\
	$(ObjDir)SerializeItemToBLOB${OBJ_SUFFIX}\
	$(ObjDir)Utilities${OBJ_SUFFIX}\
	

include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


all:	$(Objs)
