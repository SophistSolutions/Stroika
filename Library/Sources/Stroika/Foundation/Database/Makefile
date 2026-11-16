StroikaSourceRoot	:=	$(abspath ../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Foundation/Database/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/Database/

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


SubDirs	:=	\
	Document \
	SQL \


.PHONY:				$(SubDirs)


vpath %cpp $(SrcDir)

Objs	=	\
	$(ObjDir)Exception${OBJ_SUFFIX}\



all:	$(Objs)  $(SubDirs)


$(SubDirs):
	@$(MAKE) --directory $@ -s

