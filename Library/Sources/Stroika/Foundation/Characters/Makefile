StroikaRoot=$(abspath ../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk

SrcDir	=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Characters/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Characters/

include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=/Fd$(call FUNCTION_CONVERT_FILES_TO_COMPILER_NATIVE,$(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Foundation.pdb)
endif


vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)Character${OBJ_SUFFIX}\
	$(ObjDir)CodePage${OBJ_SUFFIX}\
	$(ObjDir)FloatConversion${OBJ_SUFFIX}\
	$(ObjDir)Format${OBJ_SUFFIX}\
	$(ObjDir)RegularExpression${OBJ_SUFFIX}\
	$(ObjDir)String${OBJ_SUFFIX}\
	$(ObjDir)StringBuilder${OBJ_SUFFIX}\
	$(ObjDir)String2Int${OBJ_SUFFIX}\
	$(ObjDir)ToString${OBJ_SUFFIX}\

SubDirs	=	\
	Concrete \
	CString \
	Platform \


all:	$(Objs) $(SubDirs)

.PHONY:	$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
