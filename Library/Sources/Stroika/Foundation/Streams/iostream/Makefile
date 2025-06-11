StroikaRoot	:=	$(abspath ../../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Foundation/Streams/iostream/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Streams/iostream/

include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=-Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaLibDir)stroika-foundation.pdb)
endif

vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)FStreamSupport${OBJ_SUFFIX}\
	$(ObjDir)Utilities${OBJ_SUFFIX}\
	

include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


all:	$(Objs)
