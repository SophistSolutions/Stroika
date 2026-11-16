StroikaSourceRoot	:=	$(abspath ../../../../../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Foundation/DataExchange/Archive/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Foundation/DataExchange/Archive/

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


vpath %cpp $(SrcDir)

Objs	:=	\

SubDirs	:=	\
	7z \
	Zip \



all:	$(Objs) $(SubDirs)


.PHONY:				$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s
