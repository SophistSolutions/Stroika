StroikaRoot	:=	$(abspath ../../../../../../../)/
SrcDir		:=   $(StroikaRoot)Library/Sources/Stroika/Foundation/IO/Network/InteretProtocol/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/IO/Network/InteretProtocol/

include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=-Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaLibDir)stroika-foundation.pdb)
endif

vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)ICMP${OBJ_SUFFIX}\
	$(ObjDir)IP${OBJ_SUFFIX}\


all:	$(Objs)

