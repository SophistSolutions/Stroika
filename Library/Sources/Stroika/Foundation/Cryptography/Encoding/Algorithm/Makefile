StroikaRoot	:=	$(abspath ../../../../../../../)/

SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Cryptography/Encoding/Algorithm/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Cryptography/Encoding/Algorithm/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk

vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)AES${OBJ_SUFFIX}\
	$(ObjDir)Base64${OBJ_SUFFIX}\
	$(ObjDir)RC4${OBJ_SUFFIX}\


all:	$(Objs)

