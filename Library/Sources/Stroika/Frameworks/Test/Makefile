StroikaRoot:=$(abspath ../../../../../)/

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

SrcDir	:=   $(StroikaRoot)Library/Sources/Stroika/Frameworks/Test/
ObjDir	:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/Test/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=-Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Frameworks.pdb)
endif


vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)ArchtypeClasses${OBJ_SUFFIX}\
	$(ObjDir)TestHarness${OBJ_SUFFIX}\


include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


all:	$(Objs)