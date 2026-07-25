StroikaRoot	:=	$(abspath ../../../../../../../)/

SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Foundation/DataExchange/Variant/INI/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/DataExchange/Variant/INI/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)Profile${OBJ_SUFFIX}\
	$(ObjDir)Reader${OBJ_SUFFIX}\
	$(ObjDir)Writer${OBJ_SUFFIX}\


all:	$(Objs)



