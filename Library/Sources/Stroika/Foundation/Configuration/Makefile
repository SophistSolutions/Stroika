StroikaRoot=$(abspath ../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=   $(StroikaRoot)Library/Sources/Stroika/Foundation/Configuration/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Configuration/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk



vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)Locale${OBJ_SUFFIX}\
	$(ObjDir)StroikaConfig${OBJ_SUFFIX}\
	$(ObjDir)SystemConfiguration${OBJ_SUFFIX}\
	$(ObjDir)Version${OBJ_SUFFIX}


all:	$(Objs)
