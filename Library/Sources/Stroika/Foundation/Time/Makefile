StroikaSourceRoot	:=	$(abspath ../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Foundation/Time/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/Time/

vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)Date${OBJ_SUFFIX}\
	$(ObjDir)DateTime${OBJ_SUFFIX}\
	$(ObjDir)Duration${OBJ_SUFFIX}\
	$(ObjDir)TimeOfDay${OBJ_SUFFIX}\
	$(ObjDir)Timezone${OBJ_SUFFIX}\


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)

