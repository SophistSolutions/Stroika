StroikaSourceRoot	:=	$(abspath ../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Foundation/Cryptography/SSL/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/Cryptography/SSL/

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk

vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)Certificate${OBJ_SUFFIX}\
	$(ObjDir)PEMFile${OBJ_SUFFIX}\
	$(ObjDir)PrivateKey${OBJ_SUFFIX}\


all:	$(Objs)

