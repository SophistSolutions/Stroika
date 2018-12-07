include ../../../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir					=       $(StroikaRoot)Library/Sources/Stroika/Foundation/Cryptography/Encoding/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Cryptography/Encoding/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk



.PHONY:			Algorithm


SubDirs	=	\
	Algorithm	\

vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)OpenSSLCryptoStream.o\


all:	$(ObjDir) $(Objs)  $(SubDirs)

$(SubDirs):
	@$(MAKE) --directory $@ --no-print-directory -s
