StroikaRoot	:=	$(abspath ../../../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Foundation/DataExchange/Variant/FormURLEncoded/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/DataExchange/Variant/FormURLEncoded/

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

Objs	:=	\
	$(ObjDir)Reader${OBJ_SUFFIX}\
	$(ObjDir)Writer${OBJ_SUFFIX}\


all:	$(Objs)



