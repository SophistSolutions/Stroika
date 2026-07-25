StroikaRoot	:=	$(abspath ../../../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/IO/Network/Transfer/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/IO/Network/Transfer/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


vpath %cpp $(SrcDir)


Objs	:=	\
	$(ObjDir)Cache${OBJ_SUFFIX}\
	$(ObjDir)Connection_libcurl${OBJ_SUFFIX}\
	$(ObjDir)Connection_WinHTTP${OBJ_SUFFIX}\
	$(ObjDir)Connection${OBJ_SUFFIX}\
	$(ObjDir)ConnectionPool${OBJ_SUFFIX}\
	$(ObjDir)Exception${OBJ_SUFFIX}\
	$(ObjDir)Request${OBJ_SUFFIX}\
	$(ObjDir)Response${OBJ_SUFFIX}\


all:	$(Objs)

