include ../../../../../../IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir					=	$(StroikaRoot)Library/Sources/Stroika/Foundation/DataExchange/StructuredStreamEvents/
ObjDir					=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/DataExchange/StructuredStreamEvents/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)IConsumer${OBJ_SUFFIX}\
	$(ObjDir)ObjectReader${OBJ_SUFFIX}\


all:	$(ObjDir) $(Objs)



