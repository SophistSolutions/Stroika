StroikaRoot=$(abspath ../../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Characters/Concrete/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Characters/Concrete/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=/Fd$(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Foundation.pdb
endif

SubDirs	=	\
	Private \

vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)String_ExternalMemoryOwnership_ApplicationLifetime${OBJ_SUFFIX}\
	$(ObjDir)String_ExternalMemoryOwnership_StackLifetime${OBJ_SUFFIX}\

all:	$(Objs) $(SubDirs)


list-objs::
	@for sd in $(SubDirs);\
	 do\
		$(MAKE) --directory $$sd -s list-objs;\
	 done


.PHONY:	$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
