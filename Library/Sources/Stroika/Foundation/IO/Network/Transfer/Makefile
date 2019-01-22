StroikaRoot=$(abspath ../../../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=	$(StroikaRoot)Library/Sources/Stroika/Foundation/IO/Network/Transfer/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/IO/Network/Transfer/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)Client${OBJ_SUFFIX}\
	$(ObjDir)Client_libcurl${OBJ_SUFFIX}\
	$(ObjDir)Client_WinHTTP${OBJ_SUFFIX}\
	$(ObjDir)Connection${OBJ_SUFFIX}\
	$(ObjDir)Exception${OBJ_SUFFIX}\
	$(ObjDir)Request${OBJ_SUFFIX}\
	$(ObjDir)Response${OBJ_SUFFIX}\


all:	$(Objs)

