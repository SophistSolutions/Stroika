StroikaSourceRoot	:=	$(abspath ../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Foundation/Cryptography/SSL/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/Cryptography/SSL/

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk

vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)ClientContext${OBJ_SUFFIX}\
	$(ObjDir)ServerContext${OBJ_SUFFIX}\
	$(ObjDir)SocketStream${OBJ_SUFFIX}\

all:	$(Objs)

