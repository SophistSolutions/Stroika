StroikaSourceRoot	:=	$(abspath ../../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Foundation/DataExchange/Variant/FormURLEncoded/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/DataExchange/Variant/FormURLEncoded/

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)Reader${OBJ_SUFFIX}\
	$(ObjDir)Writer${OBJ_SUFFIX}\


all:	$(Objs)



