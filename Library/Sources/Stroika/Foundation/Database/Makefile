StroikaRoot	:=	$(abspath ../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Database/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Database/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


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

