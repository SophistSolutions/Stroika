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


all:	$(ObjDir) $(Objs)  $(SubDirs)


$(SubDirs):
	@$(MAKE) --directory $@ -s
