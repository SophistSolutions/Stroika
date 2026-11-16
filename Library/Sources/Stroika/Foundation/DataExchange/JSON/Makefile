StroikaSourceRoot	:=	$(abspath ../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Foundation/DataExchange/JSON/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/DataExchange/JSON/

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk



vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)JWT${OBJ_SUFFIX}\
	$(ObjDir)Patch${OBJ_SUFFIX}\
	$(ObjDir)Pointer${OBJ_SUFFIX}\


all:	$(Objs) $(SubDirs)


.PHONY:				$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
