StroikaSourceRoot	:=	$(abspath ../../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Foundation/IO/Network/SOAP/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/IO/Network/SOAP/

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)Fault${OBJ_SUFFIX}\


all:	$(Objs)

