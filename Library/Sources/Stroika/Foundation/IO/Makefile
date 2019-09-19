StroikaRoot=$(abspath ../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir					=	$(StroikaRoot)Library/Sources/Stroika/Foundation/IO/
ObjDir					=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/IO/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=/Fd$(shell cygpath --mixed $(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Foundation.pdb)
endif


SubDirs	=	\
	FileSystem	\
	Network		\



vpath %cpp $(SrcDir)


Objs	=	\
	$(ObjDir)AccessMode${OBJ_SUFFIX}\


all:	$(Objs) $(SubDirs)


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s

