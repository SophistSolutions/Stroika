StroikaRoot=$(abspath ../../../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Cryptography/Encoding/Algorithm/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Cryptography/Encoding/Algorithm/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=/Fd$(call FUNCTION_CONVERT_FILES_TO_COMPILER_NATIVE,$(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Foundation.pdb)
endif

vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)AES${OBJ_SUFFIX}\
	$(ObjDir)Base64${OBJ_SUFFIX}\
	$(ObjDir)RC4${OBJ_SUFFIX}\

include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


all:	$(Objs)

