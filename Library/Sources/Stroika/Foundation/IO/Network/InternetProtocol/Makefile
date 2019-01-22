export StroikaRoot?=$(abspath ../../../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=   $(StroikaRoot)Library/Sources/Stroika/Foundation/IO/Network/InteretProtocol/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/IO/Network/InteretProtocol/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk




vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)ICMP${OBJ_SUFFIX}\
	$(ObjDir)IP${OBJ_SUFFIX}\


all:	$(Objs)

