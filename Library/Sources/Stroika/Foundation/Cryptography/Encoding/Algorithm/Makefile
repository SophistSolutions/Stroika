StroikaSourceRoot	:=	$(abspath ../../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Foundation/Cryptography/Encoding/Algorithm/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/Cryptography/Encoding/Algorithm/

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk

vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)AES${OBJ_SUFFIX}\
	$(ObjDir)Base64${OBJ_SUFFIX}\
	$(ObjDir)Hex${OBJ_SUFFIX}\
	$(ObjDir)RC4${OBJ_SUFFIX}\


all:	$(Objs)

