StroikaRoot	:=	$(abspath ../../../../../)/
SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Foundation/Time/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Time/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)Date${OBJ_SUFFIX}\
	$(ObjDir)DateTime${OBJ_SUFFIX}\
	$(ObjDir)Duration${OBJ_SUFFIX}\
	$(ObjDir)TimeOfDay${OBJ_SUFFIX}\
	$(ObjDir)Timezone${OBJ_SUFFIX}\


include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)

