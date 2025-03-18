StroikaRoot:=$(abspath ../../../../../../)/

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

SrcDir	:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Database/Document/
ObjDir	:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Database/Document/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=-Fd$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Foundation.pdb)
endif


vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)Collection${OBJ_SUFFIX}\
	$(ObjDir)Connection${OBJ_SUFFIX}\
	$(ObjDir)Filter${OBJ_SUFFIX}\
	$(ObjDir)MongoDBClient${OBJ_SUFFIX}\
	$(ObjDir)Projection${OBJ_SUFFIX}\
	$(ObjDir)SQLite${OBJ_SUFFIX}\

SubDirs	:=	\

.PHONY:				$(SubDirs)

include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk

all:	$(Objs)  $(SubDirs)


$(SubDirs):
	@$(MAKE) --directory $@ -s

