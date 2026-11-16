StroikaSourceRoot	:=	$(abspath ../../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=   $(StroikaSourceRoot)Library/Sources/Stroika/Foundation/IO/Network/InteretProtocol/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/IO/Network/InteretProtocol/

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)ICMP${OBJ_SUFFIX}\
	$(ObjDir)IP${OBJ_SUFFIX}\


all:	$(Objs)

