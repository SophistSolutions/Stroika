StroikaSourceRoot	:=	$(abspath ../../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Foundation/IO/Network/Platform/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/IO/Network/Platform/

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


SubDirs	:=				\
	Windows				\


vpath %cpp $(SrcDir)

Objs	:=	\

all:	$(Objs) $(SubDirs)

.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
