StroikaRoot	:=	$(abspath ../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Cache/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Cache/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk

Objs	:=	\
	$(ObjDir)Statistics${OBJ_SUFFIX}


vpath %cpp $(SrcDir)

all:	$(Objs)
