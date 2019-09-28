StroikaRoot=$(abspath ../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk

SrcDir	=   $(StroikaRoot)Library/Sources/Stroika/Foundation/Configuration/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Configuration/

include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=/Fd$(call FUNCTION_CONVERT_FILES_TO_COMPILER_NATIVE,$(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Foundation.pdb)
endif


vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)Locale${OBJ_SUFFIX}\
	$(ObjDir)StroikaConfig${OBJ_SUFFIX}\
	$(ObjDir)SystemConfiguration${OBJ_SUFFIX}\
	$(ObjDir)Version${OBJ_SUFFIX}


SubDirs	=	\
	Platform \


all:	$(Objs) $(SubDirs)

.PHONY:	$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
