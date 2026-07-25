StroikaRoot	:=	$(abspath ../../../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/IO/Network/Platform/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/IO/Network/Platform/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


SubDirs	:=				\
	Windows				\


vpath %cpp $(SrcDir)

Objs	:=	\

all:	$(Objs) $(SubDirs)

.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
