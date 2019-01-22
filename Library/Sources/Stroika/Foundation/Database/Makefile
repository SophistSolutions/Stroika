StroikaRoot=$(abspath ../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Database/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Database/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk



vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)ODBCClient${OBJ_SUFFIX}\
	$(ObjDir)SQLite${OBJ_SUFFIX}\


all:	$(Objs)
