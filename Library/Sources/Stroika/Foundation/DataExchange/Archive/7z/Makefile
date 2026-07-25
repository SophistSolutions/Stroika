StroikaRoot	:=	$(abspath ../../../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/DataExchange/Archive/7z/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/DataExchange/Archive/7z/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk

vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)Reader${OBJ_SUFFIX}\

all:	$(Objs)
