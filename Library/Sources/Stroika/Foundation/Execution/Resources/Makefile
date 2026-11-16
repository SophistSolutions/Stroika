StroikaSourceRoot	:=	$(abspath ../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Foundation/Execution/Resources/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/Execution/Resources/

vpath %cpp $(SrcDir)

Objs	:=	\
	$(ObjDir)Manager${OBJ_SUFFIX}\
	$(ObjDir)Name${OBJ_SUFFIX}\
	$(ObjDir)ResourceNotFoundException${OBJ_SUFFIX}\

SubDirs	:=	\
	Concrete \


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)

.PHONY:	$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
