StroikaSourceRoot	:=	$(abspath ../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Foundation/DataExchange/StructuredStreamEvents/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/DataExchange/StructuredStreamEvents/


vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)IConsumer${OBJ_SUFFIX}\
	$(ObjDir)ObjectReader${OBJ_SUFFIX}\


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)



