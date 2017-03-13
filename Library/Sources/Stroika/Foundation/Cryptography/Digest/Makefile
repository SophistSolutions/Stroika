include ../../../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir					=       $(StroikaRoot)Library/Sources/Stroika/Foundation/Cryptography/Digest/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Cryptography/Digest/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk



.PHONY:			Algorithm


SubDirs	=	\
	Algorithm	\

vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)Digester.o\
	$(ObjDir)DigestDataToString.o\

all:	$(ObjDir) $(Objs)  $(SubDirs)


Algorithm_OBJS := $(shell cd Algorithm; $(MAKE) -s list-objs CONFIGURATION=$(CONFIGURATION))


list-objs::
	@echo $(Algorithm_OBJS)


$(SubDirs):
	$(MAKE) --directory $@ -s CONFIGURATION=$(CONFIGURATION)
