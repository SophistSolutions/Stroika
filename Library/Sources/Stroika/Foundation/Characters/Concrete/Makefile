include ../../../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir					=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Characters/Concrete/
ObjDir					=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Characters/Concrete/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk


SubDirs	=	\
	Private \

vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)String_ExternalMemoryOwnership_ApplicationLifetime.o\
	$(ObjDir)String_ExternalMemoryOwnership_StackLifetime.o\

.PHONY:	$(SubDirs)

all:	$(Objs) $(SubDirs)

PRIVATE_OBJS := $(addprefix "Private/", $(shell $(MAKE) --directory Private -s list-objs CONFIGURATION=$(CONFIGURATION))


list-objs::
	@echo $(PRIVATE_OBJS)


Private:
	@$(MAKE) --directory Private -s  CONFIGURATION=$(CONFIGURATION)
