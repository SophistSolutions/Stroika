StroikaRoot	:=	$(abspath ../../../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Foundation/DataExchange/Variant/JSON/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/DataExchange/Variant/JSON/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)Reader${OBJ_SUFFIX}\
	$(ObjDir)Writer${OBJ_SUFFIX}\


all:	$(Objs)



