StroikaRoot:=$(abspath ../../../../../../)/

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

SrcDir	:=   $(StroikaRoot)Library/Sources/Stroika/Foundation/DataExchange/JSON/
ObjDir	:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/DataExchange/JSON/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk


vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)Patch${OBJ_SUFFIX}\


all:	$(Objs) $(SubDirs)


.PHONY:				$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
