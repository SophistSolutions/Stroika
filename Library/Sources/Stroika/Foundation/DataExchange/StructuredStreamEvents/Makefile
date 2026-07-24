StroikaRoot	:=	$(abspath ../../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/DataExchange/StructuredStreamEvents/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/DataExchange/StructuredStreamEvents/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)IConsumer${OBJ_SUFFIX}\
	$(ObjDir)ObjectReader${OBJ_SUFFIX}\


include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)



