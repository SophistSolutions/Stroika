StroikaSourceRoot	:=	$(abspath ../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Foundation/Cache/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/Cache/

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk

Objs	:=	\
	$(ObjDir)Statistics${OBJ_SUFFIX}


vpath %cpp $(SrcDir)

all:	$(Objs)
