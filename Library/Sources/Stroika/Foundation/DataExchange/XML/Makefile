StroikaRoot=$(abspath ../../../../../../)/

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

SrcDir	=   $(StroikaRoot)Library/Sources/Stroika/Foundation/DataExchange/XML/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/DataExchange/XML/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)Namespace${OBJ_SUFFIX}\
	$(ObjDir)SAXReader${OBJ_SUFFIX}\
	$(ObjDir)SerializationConfiguration${OBJ_SUFFIX}\
	$(ObjDir)WriterUtils${OBJ_SUFFIX}\


all:	$(Objs)
