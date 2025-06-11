StroikaRoot	:=	$(abspath ../../../../../../../)/

SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Cryptography/Encoding/Algorithm/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Cryptography/Encoding/Algorithm/

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=-Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaLibDir)stroika-foundation.pdb)
endif

vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)AES${OBJ_SUFFIX}\
	$(ObjDir)Base64${OBJ_SUFFIX}\
	$(ObjDir)RC4${OBJ_SUFFIX}\

include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


all:	$(Objs)

