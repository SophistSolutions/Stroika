include ../../../../../../IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir					=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Cryptography/OpenSSL/
ObjDir					=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Cryptography/OpenSSL/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk



SubDirs	=	\

vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)CipherAlgorithm${OBJ_SUFFIX}\
	$(ObjDir)DigestAlgorithm${OBJ_SUFFIX}\
	$(ObjDir)DerivedKey${OBJ_SUFFIX}\
	$(ObjDir)Exception${OBJ_SUFFIX}\


all:	$(ObjDir) $(Objs)  $(SubDirs)


