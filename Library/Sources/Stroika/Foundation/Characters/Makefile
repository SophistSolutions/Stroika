StroikaRoot=$(abspath ../../../../../)/

include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

SrcDir	=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Characters/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Characters/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)Library/Projects/Unix/SharedMakeVariables-Default.mk

ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
CXXFLAGS+=/Fd$(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Foundation.pdb
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


all:	$(Objs) $(SubDirs)

.PHONY:	$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
