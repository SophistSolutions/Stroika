include ../../../../../../../IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir					=   $(StroikaRoot)Library/Sources/Stroika/Foundation/DataExchange/Variant/CharacterDelimitedLines/
ObjDir					=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/DataExchange/Variant/CharacterDelimitedLines/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk



vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)Reader${OBJ_SUFFIX}\
	$(ObjDir)Writer${OBJ_SUFFIX}\


all:	$(ObjDir) $(Objs)



