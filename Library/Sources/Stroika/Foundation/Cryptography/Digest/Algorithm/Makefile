StroikaRoot	:=	$(abspath ../../../../../../../)/

SrcDir		:=  $(StroikaRoot)Library/Sources/Stroika/Foundation/Cryptography/Digest/Algorithm/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Cryptography/Digest/Algorithm/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)CRC32${OBJ_SUFFIX}\
	$(ObjDir)Jenkins${OBJ_SUFFIX}\
	$(ObjDir)MD5${OBJ_SUFFIX}\
	$(ObjDir)SuperFastHash${OBJ_SUFFIX}\

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs)

