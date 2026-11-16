StroikaSourceRoot	:=	$(abspath ../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Foundation/IO/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/IO/

SubDirs	:=	\
	FileSystem	\
	Network		\

vpath %cpp $(SrcDir)

Objs	:=	\

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s

