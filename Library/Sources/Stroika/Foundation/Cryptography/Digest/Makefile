StroikaRoot=$(abspath ../../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Cryptography/Digest/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Cryptography/Digest/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk


SubDirs	=	\
	Algorithm	\


vpath %cpp $(SrcDir)


Objs	=	\


all:	$(Objs)  $(SubDirs)


.PHONY:	$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
