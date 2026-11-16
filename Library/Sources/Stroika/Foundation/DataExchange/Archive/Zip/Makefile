StroikaSourceRoot	:=	$(abspath ../../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Foundation/DataExchange/Archive/Zip/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/DataExchange/Archive/Zip/

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk

vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)Private_minizip_${OBJ_SUFFIX}\
	$(ObjDir)Reader${OBJ_SUFFIX}\
	$(ObjDir)Writer${OBJ_SUFFIX}\

all:	$(Objs)
