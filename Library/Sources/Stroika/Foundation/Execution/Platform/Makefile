StroikaRoot	:=	$(abspath ../../../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/Execution/Platform/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/Execution/Platform/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

vpath %cpp $(SrcDir)

Objs	:=	\

SubDirs	:=	\
	POSIX \
	Windows \


include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(Objs) $(SubDirs)

.PHONY:	$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
