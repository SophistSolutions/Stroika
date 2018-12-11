include ../../../../../../../IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir					=   $(StroikaRoot)Library/Sources/Stroika/Foundation/Cryptography/Digest/Algorithm/
ObjDir					=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Cryptography/Digest/Algorithm/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)CRC32.o\
	$(ObjDir)Jenkins.o\
	$(ObjDir)MD5.o\
	$(ObjDir)SuperFastHash.o\


all:	$(ObjDir) $(Objs)

