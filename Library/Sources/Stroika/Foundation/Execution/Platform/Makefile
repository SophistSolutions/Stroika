StroikaSourceRoot	:=	$(abspath ../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Foundation/Execution/Platform/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/Execution/Platform/

vpath %cpp $(SrcDir)

Objs	:=	\

SubDirs	:=	\
	POSIX \
	Windows \


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)

.PHONY:	$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
