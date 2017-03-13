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


all:	$(Objs)  $(SubDirs)


Algorithm_OBJS := $(addprefix "./Algorithm/", $(shell $(MAKE) --directory Algorithm --no-print-directory -s list-objs))


list-objs::
	@echo $(Algorithm_OBJS)

Algorithm:
	@$(MAKE) --directory Algorithm --no-print-directory -s
