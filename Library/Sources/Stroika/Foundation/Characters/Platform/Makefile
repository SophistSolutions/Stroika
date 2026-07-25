StroikaRoot	:=	$(abspath ../../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Characters/Platform/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Characters/Platform/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

vpath %cpp $(SrcDir)

Objs	:=	\

SubDirs	:=	\
	Windows \


include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)

.PHONY:	$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
