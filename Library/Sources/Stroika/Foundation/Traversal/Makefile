StroikaRoot	:=	$(abspath ../../../../../)/

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk
include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk

SrcDir	:=   $(StroikaRoot)Library/Sources/Stroika/Foundation/Traversal/
ObjDir	:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Traversal/


ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=-Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaLibDir)stroika-foundation.pdb)
endif

vpath %cpp $(SrcDir)

Objs	:=	\



all:	$(Objs)
