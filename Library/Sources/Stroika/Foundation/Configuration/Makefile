StroikaSourceRoot	:=	$(abspath ../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Library/Sources/Stroika/Foundation/Configuration/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/Configuration/


vpath %cpp $(SrcDir)

Objs	=	\


SubDirs	=	\


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)

.PHONY:	$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
