include ../../../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir					=       $(StroikaRoot)Library/Sources/Stroika/Foundation/Cryptography/OpenSSL/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Cryptography/OpenSSL/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk



SubDirs	=	\

vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)CipherAlgorithm.o\
	$(ObjDir)DigestAlgorithm.o\
	$(ObjDir)DerivedKey.o\
	$(ObjDir)Exception.o\

all:	$(Objs)  $(SubDirs)


