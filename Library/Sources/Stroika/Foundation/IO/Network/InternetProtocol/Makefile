StroikaRoot	:=	$(abspath ../../../../../../../)/
SrcDir		:=   $(StroikaRoot)Library/Sources/Stroika/Foundation/IO/Network/InteretProtocol/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/IO/Network/InteretProtocol/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)ICMP${OBJ_SUFFIX}\
	$(ObjDir)IP${OBJ_SUFFIX}\


all:	$(Objs)

