StroikaRoot	:=	$(abspath ../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Cache/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Cache/

include $(StroikaRoot)Build/ScriptsLib/SharedMakeVariables-Default.mk

include $(StroikaRoot)Build/ScriptsLib/SharedBuildRules-Default.mk

Objs	:=	\
	$(ObjDir)Statistics${OBJ_SUFFIX}


ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=-Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaLibDir)stroika-foundation.pdb)
endif



vpath %cpp $(SrcDir)

all:	$(Objs)
