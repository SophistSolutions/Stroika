export StroikaRoot?=$(shell realpath ../../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Characters/Concrete/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Characters/Concrete/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk


SubDirs	=	\
	Private \

vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)String_ExternalMemoryOwnership_ApplicationLifetime${OBJ_SUFFIX}\
	$(ObjDir)String_ExternalMemoryOwnership_StackLifetime${OBJ_SUFFIX}\

.PHONY:	$(SubDirs)

all:	$(Objs) $(SubDirs)



list-objs::
	@for sd in $(SubDirs);\
	 do\
		$(MAKE) --directory $$sd -s list-objs;\
	 done


Private:
	@$(MAKE) --directory Private -s
