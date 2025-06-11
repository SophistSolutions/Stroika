StroikaRoot	:=	$(abspath ../../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Cryptography/SSL/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Cryptography/SSL/

include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)Certificate${OBJ_SUFFIX}\
	$(ObjDir)PEMFile${OBJ_SUFFIX}\
	$(ObjDir)PrivateKey${OBJ_SUFFIX}\


all:	$(Objs)

