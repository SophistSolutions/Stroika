StroikaRoot=$(abspath ../../../../../../)/

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

SrcDir	=	$(StroikaRoot)Library/Sources/Stroika/Foundation/DataExchange/StructuredStreamEvents/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/DataExchange/StructuredStreamEvents/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)IConsumer${OBJ_SUFFIX}\
	$(ObjDir)ObjectReader${OBJ_SUFFIX}\


include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


all:	$(Objs)



