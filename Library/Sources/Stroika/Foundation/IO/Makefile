StroikaRoot	:=	$(abspath ../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/IO/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/IO/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SubDirs	:=	\
	FileSystem	\
	Network		\

vpath %cpp $(SrcDir)

Objs	:=	\

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s

