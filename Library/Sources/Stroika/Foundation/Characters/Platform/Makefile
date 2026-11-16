StroikaSourceRoot	:=	$(abspath ../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Foundation/Characters/Platform/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/Characters/Platform/

vpath %cpp $(SrcDir)

Objs	:=	\

SubDirs	:=	\
	Windows \


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)

.PHONY:	$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
