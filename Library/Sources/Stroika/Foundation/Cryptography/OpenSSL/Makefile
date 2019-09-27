StroikaRoot=$(abspath ../../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Cryptography/OpenSSL/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Cryptography/OpenSSL/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk


SubDirs	=	\

vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)CipherAlgorithm${OBJ_SUFFIX}\
	$(ObjDir)DigestAlgorithm${OBJ_SUFFIX}\
	$(ObjDir)DerivedKey${OBJ_SUFFIX}\
	$(ObjDir)Exception${OBJ_SUFFIX}\


all:	$(Objs)  $(SubDirs)


